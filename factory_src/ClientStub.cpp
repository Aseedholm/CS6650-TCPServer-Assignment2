#include "ClientStub.h"

ClientStub::ClientStub() {}

int ClientStub::Init(std::string ip, int port) {
	return socket.Init(ip, port);
}

RobotInfo ClientStub::OrderRobot(RobotOrder order) { /////******************Exactly what is occuring in if statements?
	RobotInfo info;
	char buffer[32];
	int size;
	order.Marshal(buffer);
	size = order.Size();
	if (socket.Send(buffer, size, 0)) { //if send succeeds return 1, 0.
		size = info.Size();
		if (socket.Recv(buffer, size, 0)) { 
			info.Unmarshal(buffer);
		}
	}
	return info;
}
