#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"

int RobotFactory::returnNumberOfPeers() {
    return peers;
}

void RobotFactory::setNumberOfPeers(int peerPassed) {
    peers = peerPassed;
}

RobotInfo RobotFactory::CreateRegularRobot(CustomerRequest request, int engineer_id) {
	RobotInfo robot;

    robot.CopyRequest(request);
	robot.SetEngineerId(engineer_id);

    robot.SetAdminId(-1);
	return robot;
}


RobotInfo RobotFactory::CreateSpecialRobot(CustomerRequest request, int engineer_id) {
	RobotInfo robot;
    robot.CopyRequest(request);

	robot.SetEngineerId(engineer_id);

	std::promise<RobotInfo> prom;
	std::future<RobotInfo> fut = prom.get_future();

	std::unique_ptr<ExpertRequest> req = std::unique_ptr<ExpertRequest>(new ExpertRequest);
            req->robot = robot;
	req->prom = std::move(prom);

	erq_lock.lock();
	erq.push(std::move(req));
	erq_cv.notify_one();
	erq_lock.unlock();

	robot = fut.get();
	return robot;
}

RobotInfo RobotFactory::CreateRobotWithAdmin(CustomerRequest request, int engineer_id) {
	RobotInfo robot;
    robot.CopyRequest(request);

	robot.SetEngineerId(engineer_id);

	std::promise<RobotInfo> prom;
	std::future<RobotInfo> fut = prom.get_future();

	std::unique_ptr<AdminRequest> req = std::unique_ptr<AdminRequest>(new AdminRequest);
	req->robot = robot;
	req->prom = std::move(prom);

	admin_req_lock.lock();
	adminRequestQueue.push(std::move(req));
	admin_req_cv.notify_one();
	admin_req_lock.unlock();

	robot = fut.get();
	return robot;
}
//Check here to see who is connected to this server.
//Either Primary Server or Client.
//
void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {

    //Whenever a cUST places an order, engineer thread receives order.
    //Check for acknowledge message.
    //PFA will also send an acknowledge message to the IFA threads.
    //
	int engineer_id = id;


	int request_type;
//	RobotOrder order;
    CustomerRequest request;


	RobotInfo robot;
    CustomerRecord record;

	ServerStub stub;

	stub.Init(std::move(socket));
	int returnedAcknowledgement = stub.initialAcknowledgementReceived();
	//Identified as Primary if returnedAcknowledgement is = 0;
	//Identified as Backup if returnedAcknowledgement is = 1;
	//Have if statement - if == 0 use engineer logic if == 1 use IFA logic.
	//If == 0 set up connection socket in PFA thread.
	//If == 1 set up listening sockets in IFA thread.
//	std::cout << "FACTORY ID IS: " << factory_id << std::endl;
//	std::cout << "PRIMARY ID IS (PRE UPDATE REQUEST): " << primary_id << std::endl;
//	std::cout << "SENT FLAG: " << returnedAcknowledgement << std::endl;
//
    if(returnedAcknowledgement == 0) { //Engineer connected to client
        while (true) {
            //In Server stub add function that checks receive message.

            request = stub.ReceiveRequest(); //Write function in stub that checks acknoledgement.

            if (!request.IsValid()) {
                break;
            }
            //Check if client connected here or if it is a PFA (meaning this is an idle factory).
            request_type = request.GetRequestType();

            switch (request_type) {
                case 1:

                    primary_id = factory_id;

//                    std::cout << "PRIMARY ID IS (POST UPDATE REQUEST): " << primary_id << std::endl;
                    robot = CreateRobotWithAdmin(request, engineer_id);

                    stub.SendRobot(robot);
                    break;
                case 2:
                    //Get customer record request
                    record = GetCustomerRecord(request);
                    stub.ReturnRecord(record);
                    break;
                case 3:
                    record = GetCustomerRecord(request);
                    stub.ReturnRecord(record);
                    break;
                default:
                    std::cout << "Undefined request type (Server): "
                        << request_type << std::endl;

            }
        }
    } else if (returnedAcknowledgement == 1) { //connected to PFA so IFA role.
//        for(int i = 0; i < (int)uniqueIdVector.size(); i++) {
//            std::cout << "IN ROBOT FACTORY IFA -> OTHER SERVER: " << uniqueIdVector[i] << " " << ipAddressVector[i] << " " << portVector[i] << std::endl;
//        }

            ReplicationRequest replication_request = stub.ReceiveReplicationRequest();
            primary_id = replication_request.GetFactoryId();

            MapOp customerRequestLogFromPrimary;
            customerRequestLogFromPrimary.opcode = 1;
            customerRequestLogFromPrimary.arg1 = replication_request.GetArg1();
            customerRequestLogFromPrimary.arg2 = replication_request.GetArg2();
            //check if last index from primary is the same as the last index I just pushed from.  /./11:25 10/22/20
            //if last_index in IFA +1 // == replication_request.last index
            smr_log.push_back(customerRequestLogFromPrimary); //MAY NEED TO REVIEW LAST INDEX LOGIC>**********@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

//            last_index = replication_request.GetLastIndex();
            last_index = smr_log.size() - 1;


//            std::cout << "LOG SIZE: " << smr_log.size() << std::endl;
            //Check commited index of this server and
            //for loop myCommited to the PassedCommited index
            //MapOP passed would be index 7, the commited index passed would be 6, and the FIA's index would be 5.
            replication_request.Print();
            std::cout << "IFA COMMITED INDEX " << committed_index << std::endl;
            //
            if (last_index - committed_index > -1) {
                std::cout << last_index - committed_index << std::endl;
            }
            int passedCommitedIndex = replication_request.GetCommittedIndex();
            //IMPLEMENT LOGIC SO THAT IF PIMARY IS SWITCHED THE FORMER PRIMARY DOESN'T ---> update as it is already caught up
            //Scenario
            //1) Primary commited index = 5 / Idle1 = 4 / Idle2 = 4
            //2) Switch Primary --> Primary (Idle1) commited needs to jump by 1 (index 5) and update the log (Do this where we set primary ID)
            //2) Idle1(FormerPrimary) will do nothing when replication request is called on update from Current Primary (Former Idle1) as it is up to date
            //2) Idle2 will update accordingly as Primary instructs with commit at index 5.
            if (passedCommitedIndex > -1) {

                customer_record[smr_log[passedCommitedIndex].arg1] = smr_log[passedCommitedIndex].arg2;

//                            //Update map with log request
//                if(customer_record.count(smr_log[replication_request.GetCommittedIndex()].arg1) < 0) {
//                    customer_record.insert(std::pair<int,int>(smr_log[replication_request.GetCommittedIndex()].arg1, smr_log[replication_request.GetCommittedIndex()].arg2));
//                } else {
//                    customer_record.erase(smr_log[replication_request.GetCommittedIndex()].arg1);
//                    customer_record.insert(std::pair<int,int>(smr_log[replication_request.GetCommittedIndex()].arg1, smr_log[replication_request.GetCommittedIndex()].arg2));
////                    customer_record[customerRequestLog.arg1] = customerRequestLog.arg2;
//                }





//                if(customer_record.count(customerRequestLogFromPrimary.arg1) < 0) {
//                    customer_record.insert(std::pair<int,int>(customerRequestLogFromPrimary.arg1, customerRequestLogFromPrimary.arg2));
//                } else {
//                    customer_record.erase(customerRequestLogFromPrimary.arg1);
//                    customer_record.insert(std::pair<int,int>(customerRequestLogFromPrimary.arg1, customerRequestLogFromPrimary.arg2));
//                    //customer_record[customerRequestLog.arg1] = customerRequestLog.arg2;
//                }

            }


            committed_index = last_index;
//            committed_index = replication_request.GetCommittedIndex();

            //set primary id = factory id
            //create mapop object with replication_request data
//            replication_request.Print();
            stub.ReplicationResponse();
//            std::cout<<"REPLICATION RESPONE COMPLETE" << std::endl;

    }

}

void RobotFactory::AdminThread(int id) {
	std::unique_lock<std::mutex> ul(admin_req_lock, std::defer_lock);

	while (true) {
		ul.lock();

		if (adminRequestQueue.empty()) {
			admin_req_cv.wait(ul, [this]{ return !adminRequestQueue.empty(); }); //Can be triggered when thread is waked up.
		}

		auto adminRequest = std::move(adminRequestQueue.front());
		adminRequestQueue.pop();

        //Create MapOp struct

		MapOp customerRequestLog;
		customerRequestLog.opcode = 1;
		customerRequestLog.arg1 = adminRequest->robot.GetCustomerId();
		customerRequestLog.arg2 = adminRequest->robot.GetOrderNumber();
		//Add struct to log
        smr_log.push_back(customerRequestLog);
        last_index = smr_log.size() - 1;
        ReplicationRequest request_to_send;
        request_to_send.SetRequest(factory_id, committed_index, last_index, customerRequestLog.opcode, customerRequestLog.arg1, customerRequestLog.arg2);

//        //HERE PFA LOGIC STARTS BEFORE WE DO ANYTHING WITH MAP.
        for (int i = 0; i < peers; i++) {
            ServerClientStub server_client_stub;
            server_client_stub.Init(ipAddressVector[i], portVector[i]);
            server_client_stub.PFAInitialAcknowledgement();
            server_client_stub.ReplicationRequestSendRec(request_to_send);
//            std::cout << "IN ROBOT FACTORY PFA -> OTHER SERVER: " << uniqueIdVector[i] << " " << ipAddressVector[i] << " " << portVector[i] << std::endl;
        }
        //For real life implementation should use LOG LAST INDEX>
		//Update map with log request
		if(customer_record.count(smr_log[last_index].arg1) < 0) {
//            customer_record.insert(std::pair<int,int>(customerRequestLog.arg1, customerRequestLog.arg2));
            customer_record.insert(std::pair<int,int>(smr_log[last_index].arg1, smr_log[last_index].arg2)); //CHANGED POST OFFICE HOURS
		} else {
            customer_record.erase(smr_log[last_index].arg1);
            customer_record.insert(std::pair<int,int>(smr_log[last_index].arg1, smr_log[last_index].arg2)); //CHANGED POST OFFICE HOURS
            //customer_record[customerRequestLog.arg1] = customerRequestLog.arg2;
		}
		committed_index = last_index;
		std::cout << "PFA COMMITED INDEX " << committed_index << std::endl;
        //Modify socket function if needed. **10/22/2020.
        //

		ul.unlock();

        adminRequest->robot.SetAdminId(id);
		adminRequest->prom.set_value(adminRequest->robot);
	}

}

void RobotFactory::ExpertThread(int id) {
	std::unique_lock<std::mutex> ul(erq_lock, std::defer_lock);
	while (true) {
		ul.lock();

		if (erq.empty()) {
			erq_cv.wait(ul, [this]{ return !erq.empty(); }); //Can be triggered when thread is waked up.
		}

		auto req = std::move(erq.front());
		erq.pop();


		ul.unlock();

		std::this_thread::sleep_for(std::chrono::microseconds(100));
        req->robot.SetAdminId(id);
		req->prom.set_value(req->robot);
	}
}

CustomerRecord RobotFactory::GetCustomerRecord(CustomerRequest request) {

    CustomerRecord recordToReturn;

    int customer_id_return = request.GetCustomerId();

    admin_req_lock.lock(); //MAKE A NEW LOCK
    if (customer_record.count(customer_id_return) > 0) {

        //Add wait with mutex lock based on true/false state of adminWriting boolean. If false then wait, if true

        int last_order_return = customer_record.find(customer_id_return)->second;
        recordToReturn.setCustomerInformation(customer_id_return, last_order_return);

        admin_req_lock.unlock();
        return recordToReturn;
    } else {
        recordToReturn.setCustomerInformation(-1, -1);
        admin_req_lock.unlock();
        return recordToReturn;
    }
}

void RobotFactory::setVectors(std::vector<int> uniqueIdVectorPassed, std::vector<int> portVectorPassed, std::vector<std::string> ipAddressVectorPassed) {
    uniqueIdVector = uniqueIdVectorPassed;
    portVector = portVectorPassed;
    ipAddressVector = ipAddressVectorPassed;
}

void RobotFactory::setPrimaryId(int idPassed) {
    primary_id = idPassed;
}

void RobotFactory::setFactoryId(int idPassed) {
    factory_id = idPassed;
}

void RobotFactory::setCommitedIndex(int commitedIndexPassed) {
    committed_index = commitedIndexPassed;
}
