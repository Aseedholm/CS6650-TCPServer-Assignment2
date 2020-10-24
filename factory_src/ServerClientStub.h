#ifndef __SERVER_CLIENT_STUB_H__
#define __SERVER_CLIENT_STUB_H__

#include <string>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "ServerClientSocket.h"
#include "Messages.h"

class ServerClientStub {
private:
	ServerClientSocket socket;
public:
	ServerClientStub();
	int Init(std::string ip, int port);
//    ReplicationRequest ReplicationRequestSendRec(ReplicationRequest replication_request);
    int ReplicationRequestSendRec(ReplicationRequest replication_request);
    void PFAInitialAcknowledgement();
    void closeSocket();
};


#endif // end of #ifndef __SERVER_CLIENT_STUB_H__
