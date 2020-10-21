#include "ServerStub.h"

ServerStub::ServerStub() {}

void ServerStub::Init(std::unique_ptr<ServerSocket> socket) {
	this->socket = std::move(socket);
}

RobotOrder ServerStub::ReceiveOrder() {
	char buffer[32];
	RobotOrder order;
	if (socket->Recv(buffer, order.Size(), 0)) {
		order.Unmarshal(buffer);
	}
	return order;
}

CustomerRequest ServerStub::ReceiveRequest() {
	char buffer[32];
	CustomerRequest request;
	if (socket->Recv(buffer, request.Size(), 0)) {
		request.Unmarshal(buffer);
	}
	return request;
}

int ServerStub::SendRobot(RobotInfo info) {
	char buffer[32];
	info.Marshal(buffer);
	return socket->Send(buffer, info.Size(), 0);
}

int ServerStub::ReturnRecord(CustomerRecord record) {
	char buffer[32];
	record.Marshal(buffer);
	return socket->Send(buffer, record.Size(), 0);
}

int ServerStub::initialAcknowledgementReceived() {
    char buffer[4];
    int net_acknowledgement;
    int sentAcknowledgement = -1;
	if (socket->Recv(buffer, sizeof(int), 0)) {
        memcpy(&net_acknowledgement, buffer, sizeof(net_acknowledgement));
        sentAcknowledgement = ntohl(net_acknowledgement);
	}
	return sentAcknowledgement;
}

ReplicationRequest ServerStub::ReceiveReplicationRequest() {
    char buffer[32];
	ReplicationRequest replication_request;
	if (socket->Recv(buffer, replication_request.Size(), 0)) {
		replication_request.Unmarshal(buffer);
	}
	return replication_request;
}


void ServerStub::ReplicationResponse() {
    char buffer[4];
    int replicationComplete = 9;
    int net_replication_complete = htonl(replicationComplete);
    memcpy(buffer, &net_replication_complete, sizeof(net_replication_complete));
    int size = sizeof(replicationComplete);
    if(socket->Send(buffer, size, 0) == 0) {
        std::cout << "Issue sending replication response message" << std::endl;
    }
}


