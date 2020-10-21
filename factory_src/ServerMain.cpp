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
	int uniqueID;
	int peers;
	ServerSocket socket;
	RobotFactory factory; //Only one factory used -> This is because factory stores teh shared queue and this makes it availabel to ALL THREADS.
	std::unique_ptr<ServerSocket> new_socket; //Return socket from accept. --> Unique Pointer being sent into thread. Making it explicit that there is only 1 owner for this thread.
	std::vector<std::thread> thread_vector; //
	std::vector<int> portVector;
	std::vector<int> uniqueIdVector;
	std::vector<std::string> ipAddress;

//	std::vector<std::thread> admin_vector;
//    std::cout << argc << std::endl;
	if (argc < 4) {
		std::cout << "not enough arguments" << std::endl;
		std::cout << argv[0] << "[port #] [unique ID] [# peers]" << std::endl;
		return 0;
	}
	port = atoi(argv[1]);
	uniqueID = atoi(argv[2]);
    peers = atoi(argv[3]);

    factory.setFactoryId(uniqueID);
    factory.setPrimaryId(-1);
    std::cout << "THIS SERVER: PORT: " << port << " UNIQUE ID: " << uniqueID << " PEERS: " << peers << std::endl;

    int correctNumberOfArguments = peers * 3 + 4;

    if (argc < correctNumberOfArguments || argc > correctNumberOfArguments ) {
        std::cout << "Not enough arguments according to number of peers or too many arguments." << std::endl;
		std::cout << argv[0] << "PER PEER -> [Server ID] [Server IP] [Port #]." << std::endl;
		std::cout << "You've indicated there are " << peers
                                                   << " # of peers, so there should be exactly "
                                                   << correctNumberOfArguments << " arguments. You entered " << argc << std::endl;
		return 0;
    }

    int counter = 0;
    for (int i = 4; i < argc; i++) {
        if(counter == 0) {
//            std::cout << "UNIQUE ID" << std::endl;
            uniqueIdVector.push_back(atoi(argv[i]));
            counter += 1;
        } else if(counter == 1) {
//            std::cout << "IP ADDRESS" << std::endl;
            ipAddress.push_back(argv[i]);
            counter += 1;
        } else {
//            std::cout << "PORT #" << std::endl;
            portVector.push_back(atoi(argv[i]));
            counter = 0;
        }
//        std::cout << "i = " << i << " || " << "Passed value: " << argv[i] << std::endl;
    }

    for(int i = 0; i < (int)uniqueIdVector.size(); i++) {
        std::cout << "OTHER SERVER: " << uniqueIdVector[i] << " " << ipAddress[i] << " " << portVector[i] << std::endl;
    }
    factory.setNumberOfPeers(peers);
    factory.setVectors(std::move(uniqueIdVector), std::move(portVector), std::move(ipAddress));


////Instatiating thread pool.
//	for (int i = 0; i < num_experts; i++) {
//		std::thread expert_thread(&RobotFactory::ExpertThread, &factory, engineer_cnt++);
//		thread_vector.push_back(std::move(expert_thread));
//	}

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
