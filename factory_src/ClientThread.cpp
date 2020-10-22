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
	stub.InitialAcknowledgement(); //Send server a message saying it is the client connecting. **************

	if (request_type == 2) {
        CustomerRequest request;

		RobotInfo robot;
		CustomerRecord record;
//		order.SetOrder(customer_id, i, robot_type); //CHANGE TO CUSTOMER REQUEST ***********************************************************
        request.SetRequest(customer_id, -1, request_type);

        record = stub.ReadRecord(request);
        record.Print();
	} else if(request_type == 1 || request_type == 3){


        for (int i = 0; i < num_orders; i++) {
//                std::cout << "In num orders for loop: " << i << std::endl;
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
//                    robot.Print();
                    break;
                case 2:
    ////				//Customer record reading
    //                record = stub.ReadRecord(request);
    //                record.Print();
                    break;
                case 3:
                    request.SetRequest(i, -1, request_type);

                    record = stub.ReadRecord(request);
                    if (record.getCustomerId() != -1) {
                        record.Print();
                    }

                    //All customer records.
                    break;
                default:
                    std::cout << "Undefined request type "
                        << request_type << std::endl;

            } //end switch


            timer.EndAndMerge();

            if ( (!robot.IsValid()) && request_type == 1) {
                std::cout << "Invalid robot " << customer_id << std::endl;
                break;
            }
        } //end for loop
	} // END OF ELSE FOR IF request_type == 2
//	else if (request_type == 3) {
////        CustomerRequest request;
////
////		RobotInfo robot;
////		CustomerRecord record;
//////		order.SetOrder(customer_id, i, robot_type); //CHANGE TO CUSTOMER REQUEST ***********************************************************
////        request.SetRequest(customer_id, -1, request_type);
////
////        record = stub.ReadRecord(request);
//
//	}
}

ClientTimer ClientThreadClass::GetTimer() {
	return timer;
}

