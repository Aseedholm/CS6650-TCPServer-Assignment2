#include "ServerClientStub.h"

ServerClientStub::ServerClientStub() {}

int ServerClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

//ReplicationRequest ServerClientStub::ReplicationRequestSendRec(ReplicationRequest replication_request) {
//	ReplicationRequest replication_request_returned;
//	char buffer[32];
//	int size;
//	replication_request.Marshal(buffer);
//	size = replication_request.Size();
//	if (socket.Send(buffer, size, 0)) { //if send succeeds return 1, 0.
////		size = replication_request_returned.Size();
//        size = sizeof(int);
//		if (socket.Recv(buffer, size, 0)) {
//			replication_request_returned.Unmarshal(buffer);
//		}
//	}
//	return replication_request_returned;
//}

int ServerClientStub::ReplicationRequestSendRec(ReplicationRequest replication_request) {
	ReplicationRequest replication_request_returned;
	char buffer[32];
	int size;
	replication_request.Marshal(buffer);
	size = replication_request.Size();
	if (socket.Send(buffer, size, 0)) { //if send succeeds return 1, 0.
//		size = replication_request_returned.Size();
        size = sizeof(int);
		if (socket.Recv(buffer, size, 0)) {
			replication_request_returned.Unmarshal(buffer);
		}
//		else {
//            return -1;
//		}
	}
//	else {
//        return -1;
//	}
	return 1;
}

//void ServerClientStub::PFAInitialAcknowledgement() {
//    char buffer[4];
//    int pfaServerFlag = 1;
//    int net_flag = htonl(pfaServerFlag);
//    memcpy(buffer, &net_flag, sizeof(net_flag));
//    int size = sizeof(pfaServerFlag);
//    if(socket.Send(buffer, size, 0) == 0) {
//        std::cout << "Issue sending Acknowledgement message" << std::endl;
//    }
//}

int ServerClientStub::PFAInitialAcknowledgement() {
    char buffer[4];
    int pfaServerFlag = 1;
    int net_flag = htonl(pfaServerFlag);
    memcpy(buffer, &net_flag, sizeof(net_flag));
    int net_committed_index;
    int committed_index = -1;
    int size = sizeof(pfaServerFlag);
    if(socket.Send(buffer, size, 0) == 0) {
//        std::cout << "Issue sending Acknowledgement message" << std::endl;
    } else {
        size = sizeof(int);
        if(socket.Recv(buffer, size, 0)) {
            memcpy(&net_committed_index, buffer, sizeof(net_committed_index));
            committed_index = ntohl(net_committed_index);
        }
    }
    return committed_index;
}

void ServerClientStub::closeSocket() {
    socket.Close();
}

//idleFactory ServerClientStub::PFAInitialAcknowledgement() {
//    char buffer[4];
//    int pfaServerFlag = 1;
//    int net_flag = htonl(pfaServerFlag);
//    memcpy(buffer, &net_flag, sizeof(net_flag));
//    int net_committed_index;
//    int net_idle_factory;
//    int committed_index = -1;
//    int idle_factory = -1;
//    idleFactory factoryToReturn;
//    int size = sizeof(pfaServerFlag);
//    if(socket.Send(buffer, size, 0) == 0) {
//        std::cout << "Issue sending Acknowledgement message" << std::endl;
//    } else {
//        size = sizeof(int);
//        if(socket.Recv(buffer, size, 0)) {
//
//
//            int offset = 0;
//            memcpy(&net_committed_index, buffer, sizeof(net_committed_index));
//            offset += sizeof(net_committed_index);
//            memcpy(&net_idle_factory, buffer + offset, sizeof(net_idle_factory));
//
//            committed_index = ntohl(net_committed_index);
//            idle_factory = ntohl(net_idle_factory);
//
//            factoryToReturn.committed_index_passed = committed_index;
//            factoryToReturn.idle_factory_id = idle_factory;
//        }
//    }
////    return committed_index;
//    return factoryToReturn;
//}


