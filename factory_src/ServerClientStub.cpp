#include "ServerClientStub.h"

ServerClientStub::ServerClientStub() {}

int ServerClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

ReplicationRequest ServerClientStub::ReplicationRequestSendRec(ReplicationRequest replication_request) {
	ReplicationRequest replication_request_returned;
	char buffer[32];
	int size;
	replication_request.Marshal(buffer);
	size = replication_request.Size();
	if (socket.Send(buffer, size, 0)) { //if send succeeds return 1, 0.
		size = replication_request_returned.Size();
		if (socket.Recv(buffer, size, 0)) {
			replication_request_returned.Unmarshal(buffer);
		}
	}
	return replication_request_returned;
}

void ServerClientStub::PFAInitialAcknowledgement() {
    char buffer[4];
    int pfaServerFlag = 1;
    int net_flag = htonl(pfaServerFlag);
    memcpy(buffer, &net_flag, sizeof(net_flag));
    int size = sizeof(pfaServerFlag);
    if(socket.Send(buffer, size, 0) == 0) {
        std::cout << "Issue sending Acknowledgement message" << std::endl;
    }
}

