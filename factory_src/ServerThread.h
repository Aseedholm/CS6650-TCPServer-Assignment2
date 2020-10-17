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

    std::queue<std::unique_ptr<MapOp>> adminRequestQueue;
	std::map<int, int> customer_record;
	std::vector<MapOp> smr_log;

//	RobotInfo CreateRegularRobot(RobotOrder order, int engineer_id);
//	RobotInfo CreateSpecialRobot(RobotOrder order, int engineer_id);
	RobotInfo CreateRegularRobot(CustomerRequest request, int engineer_id);
	RobotInfo CreateSpecialRobot(CustomerRequest request, int engineer_id);
public:
	void EngineerThread(std::unique_ptr<ServerSocket> socket, int id);
	void ExpertThread(int id);
};

#endif // end of #ifndef __SERVERTHREAD_H__

