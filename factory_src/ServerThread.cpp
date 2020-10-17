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
		switch (request_type) {
			case 0:
				robot = CreateRegularRobot(request, engineer_id);
				break;
			case 1:
				robot = CreateSpecialRobot(request, engineer_id);
				break;
			default:
				std::cout << "Undefined robot type: "
					<< request_type << std::endl;

		}
		stub.SendRobot(robot);
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
