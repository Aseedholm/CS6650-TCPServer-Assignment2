#include "ClientThread.h"
#include "Messages.h"

#include <iostream>

ClientThreadClass::ClientThreadClass() {}

void ClientThreadClass::ThreadBody(std::string ip, int port, int id, int orders, int type) {
	customer_id = id;
	num_orders = orders;
//	robot_type = type;
    request_type = type;
	if (!stub.Init(ip, port)) {
		std::cout << "Thread " << customer_id << " failed to connect" << std::endl;
		return;
	}
	for (int i = 0; i < num_orders; i++) {
//		RobotOrder order; //CHANGE TO CUSTOMER REQUEST ***********************************************************
		CustomerRequest request;

		RobotInfo robot;
		CustomerRecord record;
//		order.SetOrder(customer_id, i, robot_type); //CHANGE TO CUSTOMER REQUEST ***********************************************************
        request.SetRequest(customer_id, i, request_type);

		timer.Start();
//		robot = stub.OrderRobot(order); //CHANGE TO CUSTOMER REQUEST ***********************************************************
		switch (request_type) {
			case 1:
				robot = stub.OrderRobot(request);
                robot.Print();
				break;
			case 2:
				//Customer record reading
                record = stub.ReadRecord(request);
                record.Print();
				break;
            case 3:
                //All customer records.
                break;
			default:
				std::cout << "Undefined request type "
					<< request_type << std::endl;

		}


		timer.EndAndMerge();

		if (!robot.IsValid()) {
			std::cout << "Invalid robot " << customer_id << std::endl;
			break;
		}
	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;
}

