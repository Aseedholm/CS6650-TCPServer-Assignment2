#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "ServerSocket.h"
#include "ServerThread.h"

int main(int argc, char *argv[]) {
	int port;
	int engineer_cnt = 0;
	int num_experts;
	ServerSocket socket;
	RobotFactory factory; //Only one factory used -> This is because factory stores teh shared queue and this makes it availabel to ALL THREADS.
	std::unique_ptr<ServerSocket> new_socket; //Return socket from accept. --> Unique Pointer being sent into thread. Making it explicit that there is only 1 owner for this thread.
	std::vector<std::thread> thread_vector; //
//	std::vector<std::thread> admin_vector;

	if (argc < 3) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port #] [# experts]" << std::endl;
		return 0;
	}
	port = atoi(argv[1]);
	num_experts = atoi(argv[2]);

//Instatiating thread pool.
	for (int i = 0; i < num_experts; i++) {
		std::thread expert_thread(&RobotFactory::ExpertThread, &factory, engineer_cnt++);
		thread_vector.push_back(std::move(expert_thread));
	}

	//CREATE ADMIN THREAD WILL ONLY BE 1
	std::thread admin_thread(&RobotFactory::AdminThread, &factory, engineer_cnt++);
    thread_vector.push_back(std::move(admin_thread));
    //CREATE ADMIN THREAD WILL ONLY BE 1


	if (!socket.Init(port)) {
		std::cout << "Socket initialization failed" << std::endl;
		return 0;
	}

	while ((new_socket = socket.Accept())) {
		std::thread engineer_thread(&RobotFactory::EngineerThread, &factory,
				std::move(new_socket), engineer_cnt++); ///WHY ARE WE PASSING &RobotFactory::EngineerThread and not factory.engineerThread <---function from object.
		thread_vector.push_back(std::move(engineer_thread));
	}
	return 0;
}
