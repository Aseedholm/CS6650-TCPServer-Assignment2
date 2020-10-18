#include <iostream>
#include <memory>

#include "ServerThread.h"
#include "ServerStub.h"

//RobotInfo RobotFactory::CreateRegularRobot(RobotOrder order, int engineer_id) {
RobotInfo RobotFactory::CreateRegularRobot(CustomerRequest request, int engineer_id) {
	RobotInfo robot;
//	robot.CopyOrder(order);
    robot.CopyRequest(request);
	robot.SetEngineerId(engineer_id);
//	robot.SetExpertId(-1);
    //Engineer will now request the admin to update the customer record and waits
    robot.SetAdminId(-1);
	return robot;
}

//RobotInfo RobotFactory::CreateSpecialRobot(RobotOrder order, int engineer_id) {
RobotInfo RobotFactory::CreateSpecialRobot(CustomerRequest request, int engineer_id) {
	RobotInfo robot;
//	robot.CopyOrder(order);
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
//	robot.CopyOrder(order);
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

//	int robot_type;
	int request_type;
//	RobotOrder order;
    CustomerRequest request;


	RobotInfo robot;

	ServerStub stub;

	stub.Init(std::move(socket));

	while (true) {
//		order = stub.ReceiveOrder();
        request = stub.ReceiveRequest();
//		if (!order.IsValid()) {
//			break;
//		}
		if (!request.IsValid()) {
			break;
		}
//		robot_type = order.GetRobotType();
        request_type = request.GetRequestType();

//		switch (robot_type) {
//			case 0:
//				robot = CreateRegularRobot(order, engineer_id);
//				break;
//			case 1:
//				robot = CreateSpecialRobot(order, engineer_id);
//				break;
//			default:
//				std::cout << "Undefined robot type: "
//					<< robot_type << std::endl;
//
//		}
        request.Print();
		switch (request_type) {
			case 0:
				robot = CreateRegularRobot(request, engineer_id);
				break;
			case 1:
				robot = CreateSpecialRobot(request, engineer_id);
				break;
            case 2:
                robot = CreateRobotWithAdmin(request, engineer_id);
                std::cout << "CUSTOMER MAP RECORD: " << customer_record.find(request.GetCustomerId())->second << std::endl;
                break;
			default:
				std::cout << "Undefined robot type: "
					<< request_type << std::endl;

		}
		stub.SendRobot(robot);
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

		//Update map with log request
        customer_record.insert(std::pair<int,int>(customerRequestLog.arg1, customerRequestLog.arg2));

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
//		req->robot.SetExpertId(id);
        req->robot.SetAdminId(id);
		req->prom.set_value(req->robot);
	}
}
