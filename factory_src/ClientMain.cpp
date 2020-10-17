#include <array>
#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>

#include "ClientSocket.h"
#include "ClientThread.h"
#include "ClientTimer.h"

int main(int argc, char *argv[]) {
	std::string ip;
	int port;
	int num_customers;
	int num_orders;
	int robot_type; //Change to request_type ************************************************************
	ClientTimer timer;

	std::vector<std::shared_ptr<ClientThreadClass>> client_vector; //Could possibly have been a unqiue pointer. //Not a pointer for the thread, pointer for client thread class.
	std::vector<std::thread> thread_vector;

	if (argc < 6) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[ip] [port #] [# customers] ";
		std::cout << "[# orders] [robot type 0 or 1]" << std::endl;
		return 0;
	}

	ip = argv[1];
	port = atoi(argv[2]);
	num_customers = atoi(argv[3]);
	num_orders = atoi(argv[4]);
	robot_type = atoi(argv[5]); //Change to request_type ************************************************************


	timer.Start();
	for (int i = 0; i < num_customers; i++) {
		auto client_cls = std::shared_ptr<ClientThreadClass>(new ClientThreadClass());
		std::thread client_thread(&ClientThreadClass::ThreadBody, client_cls,
				ip, port, i, num_orders, robot_type); //Change to request_type ************************************************************

				//&ClientTHreadCLass::THreadBody - allows us to use member function of the class and if we give a member function as main thread function we can also specify the instance of the class (client_cls)
				//collecting statistics within client class. Want to use those saved values within class instance.


				//Each thread has timer, collects stats while running.
				//Give individual clients a timer and variables that collects. client_cls is the body of the class. When the thread is instantiaed we can access the variables in client_cls from ThreadBody function.
				//Because we are passing in a reference to the member function and the corresponding class isntance.
				//Class instance will survive EVEN after thread terminates.

				//ClientClass allows us to access variables from threadbody outside of the scope. Normally we can't do this.
		client_vector.push_back(std::move(client_cls));
		thread_vector.push_back(std::move(client_thread));
	}
	for (auto& th : thread_vector) {
		th.join();
	}
	timer.End();

	for (auto& cls : client_vector) {
		timer.Merge(cls->GetTimer());
	}
	timer.PrintStats();

	return 1;
}
