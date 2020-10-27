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

void RobotFactory::EngineerThread(std::unique_ptr<ServerSocket> socket, int id) {
//    setPFAToIFAConnections();
	int engineer_id = id;


	int request_type;

    CustomerRequest request;


	RobotInfo robot;
    CustomerRecord record;

	ServerStub stub;

	stub.Init(std::move(socket));

	int returnedAcknowledgement = stub.initialAcknowledgementReceived(committed_index);

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


            ReplicationRequest replication_request = stub.ReceiveReplicationRequest();
            primary_id = replication_request.GetFactoryId();

            MapOp customerRequestLogFromPrimary;
            customerRequestLogFromPrimary.opcode = 1;
            customerRequestLogFromPrimary.arg1 = replication_request.GetArg1();
            customerRequestLogFromPrimary.arg2 = replication_request.GetArg2();

            smr_log.push_back(customerRequestLogFromPrimary);

            last_index = smr_log.size() - 1;


            int passedCommitedIndex = replication_request.GetCommittedIndex();

            if (passedCommitedIndex > -1) {

                customer_record[smr_log[passedCommitedIndex].arg1] = smr_log[passedCommitedIndex].arg2;
            }

            committed_index = last_index - 1;
            stub.ReplicationResponse();
    } else {
        primary_id = -1;
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

        if (last_index > committed_index && committed_index != -1) { //This logic is to handle if a Idle Factory becomes a Primary Factory.
            committed_index = last_index;
            ReplicationRequest request_to_sendCatchUp;
            request_to_sendCatchUp.SetRequest(factory_id, committed_index, last_index,
                                               smr_log[committed_index].opcode,
                                                smr_log[committed_index].arg1,
                                                 smr_log[committed_index].arg2);
            customer_record[smr_log[committed_index].arg1] = smr_log[committed_index].arg2; //Instead of the if else statment 257 -> 264
        }


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

        int server_client_socket_status = -1;
        int  passed_committed_index = -1;

        for (int i = 0; i < peers; i++) { //replication for loop.

            ServerClientStub server_client_stub;
            server_client_socket_status = server_client_stub.Init(ipAddressVector[i], portVector[i]);

            if (server_client_socket_status != -1) {
                passed_committed_index = server_client_stub.PFAInitialAcknowledgement();

                if ((committed_index - passed_committed_index > 1) && committed_index != -1) { //catch up if statement for failed server.


                    for(int j = 0; j < (int)smr_log.size(); j+=1) { //for loop for catch up for failed server.
                        if(j != 0) {
                            ServerClientStub server_client_stub;
                            server_client_socket_status = server_client_stub.Init(ipAddressVector[i], portVector[i]);
                            server_client_stub.PFAInitialAcknowledgement();
                        }
                        ReplicationRequest replication_catch_up_request;
                        replication_catch_up_request.SetRequest(factory_id, j, j + 1, smr_log[j].opcode, smr_log[j].arg1, smr_log[j].arg2);
                        server_client_stub.ReplicationRequestSendRec(replication_catch_up_request);
                        server_client_stub.closeSocket();
                    }
                    passed_committed_index = server_client_stub.PFAInitialAcknowledgement();
                }
                server_client_stub.ReplicationRequestSendRec(request_to_send);
                server_client_stub.closeSocket();

            }
            else {
                server_client_stub.closeSocket(); //Maybe add this after ReplicationREquestSendREc call above. We can close the socket in either if/else scenario after it has been used or can't be used at all.
            }
        }


		customer_record[smr_log[last_index].arg1] = smr_log[last_index].arg2; //Instead of the if else statment 257 -> 264

		committed_index = last_index;
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

void RobotFactory::setPFAToIFAConnections() {
    for (int i = 0; i < peers; i++) {
        ServerClientStub server_client_stub;
        server_client_stub.Init(ipAddressVector[i], portVector[i]);
        pfaToIfa.push_back(std::move(server_client_stub));
    }
}
