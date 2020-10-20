#ifndef __CLIENT_STUB_H__
#define __CLIENT_STUB_H__

#include <string>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "ClientSocket.h"
#include "Messages.h"

class ClientStub {
private:
	ClientSocket socket;
public:
	ClientStub();
	int Init(std::string ip, int port);
//	RobotInfo OrderRobot(RobotOrder order);
    RobotInfo OrderRobot(CustomerRequest request);
	//Add method ReadRecord
    CustomerRecord ReadRecord(CustomerRequest request);
    void InitialAcknowledgement();
};


#endif // end of #ifndef __CLIENT_STUB_H__
