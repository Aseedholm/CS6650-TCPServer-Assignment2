#ifndef __SERVERTHREAD_H__
#define __SERVERTHREAD_H__

#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <map>
#include <iterator>

#include "Messages.h"
#include "ServerSocket.h"

struct ExpertRequest {
	RobotInfo robot;
	std::promise<RobotInfo> prom;
};

struct AdminRequest {
    RobotInfo robot;
    std::promise<RobotInfo> prom;
};


struct MapOp {
    int opcode;
    int arg1;
    int arg2;
};


class RobotFactory { //Add map and log here. Most likely need to make a queue containing a struct that will have the admin id.
private:
	std::queue<std::unique_ptr<ExpertRequest>> erq;
	std::mutex erq_lock;
	std::condition_variable erq_cv;

    std::queue<std::unique_ptr<AdminRequest>> adminRequestQueue;
    std::mutex admin_req_lock;
    std::mutex primary_lock; // This is for the Primary Server and PFA thread.
    std::condition_variable admin_req_cv;
	std::map<int, int> customer_record;
	std::vector<MapOp> smr_log;

	bool notWriting = false;
	int peers;

//	RobotInfo CreateRegularRobot(RobotOrder order, int engineer_id);
//	RobotInfo CreateSpecialRobot(RobotOrder order, int engineer_id);
	RobotInfo CreateRegularRobot(CustomerRequest request, int engineer_id);
	RobotInfo CreateSpecialRobot(CustomerRequest request, int engineer_id);
    RobotInfo CreateRobotWithAdmin(CustomerRequest request, int engineer_id);
    CustomerRecord GetCustomerRecord(CustomerRequest request);
public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void AdminThread(int id);
	void ExpertThread(int id);
	int returnNumberOfPeers();
	void setNumberOfPeers(int peerPassed);
};

#endif // end of #ifndef __SERVERTHREAD_H__

