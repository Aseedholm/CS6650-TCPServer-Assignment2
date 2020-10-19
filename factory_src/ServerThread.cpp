#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"

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
	int engineer_id = id;


	int request_type;
//	RobotOrder order;
    CustomerRequest request;


	RobotInfo robot;
    CustomerRecord record;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
        request = stub.ReceiveRequest();

		if (!request.IsValid()) {
			break;
		}

        request_type = request.GetRequestType();

		switch (request_type) {
			case 1:
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
//	notWriting = true;
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

		//Update map with log request
		if(customer_record.count(customerRequestLog.arg1) < 0) {
            customer_record.insert(std::pair<int,int>(customerRequestLog.arg1, customerRequestLog.arg2));
		} else {
            customer_record.erase(customerRequestLog.arg1);
            customer_record.insert(std::pair<int,int>(customerRequestLog.arg1, customerRequestLog.arg2));
		}


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

////    admin_req_lock.lock();
//    adminMutexLock.lock();
//    if (!notWriting) {
//        admin_req_cv.wait(adminMutexLock, [this]{return notWriting;});
//    }
    admin_req_lock.lock();
    if (customer_record.count(customer_id_return) > 0) {
////        adminMutexLock.lock(); //


        //Add wait with mutex lock based on true/false state of adminWriting boolean. If false then wait, if true

        int last_order_return = customer_record.find(customer_id_return)->second;
        recordToReturn.setCustomerInformation(customer_id_return, last_order_return);

//        notWriting = false;
//        adminMutexLock.unlock();
////        admin_req_lock.unlock();
        admin_req_lock.unlock();
        return recordToReturn;
    } else {
        recordToReturn.setCustomerInformation(-1, -1);
        return recordToReturn;
    }


}
