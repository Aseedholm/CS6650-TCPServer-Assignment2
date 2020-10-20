#include "ServerClientStub.h"

ServerClientStub::ServerClientStub() {}

int ServerClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

//RobotInfo ClientStub::OrderRobot(RobotOrder order) { /////******************Exactly what is occuring in if statements?
//RobotInfo ServerClientStub::OrderRobot(CustomerRequest request) {
//	RobotInfo info;
//	char buffer[32];
//	int size;
//	request.Marshal(buffer);
//	size = request.Size();
//	if (socket.Send(buffer, size, 0)) { //if send succeeds return 1, 0.
//		size = info.Size();
//		if (socket.Recv(buffer, size, 0)) {
//			info.Unmarshal(buffer);
//		}
//	}
//	return info;
//}

