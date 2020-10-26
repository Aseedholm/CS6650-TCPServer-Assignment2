#ifndef __SERVER_STUB_H__
#define __SERVER_STUB_H__

#include <memory>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "ServerSocket.h"
#include "Messages.h"

class ServerStub {
private:
	std::unique_ptr<ServerSocket> socket;
public:
	ServerStub();
	int Init(std::unique_ptr<ServerSocket> socket);
	RobotOrder ReceiveOrder();
	CustomerRequest ReceiveRequest();

	int SendRobot(RobotInfo info);

	int ReturnRecord(CustomerRecord record);

//    int initialAcknowledgementReceived();
    int initialAcknowledgementReceived(int committed_index);
//    int initialAcknowledgementReceived(int committed_index, int idleId);
    ReplicationRequest ReceiveReplicationRequest();

    void ReplicationResponse();

    //receiveReplicationUpdate
    //sendReplicationResponse

};

#endif // end of #ifndef __SERVER_STUB_H__
