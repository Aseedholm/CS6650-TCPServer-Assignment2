#include <cstring>
#include <iostream>

#include <arpa/inet.h>
#include "Messages.h"

//// ROBOT ORDER ********************************************************
RobotOrder::RobotOrder() {
	customer_id = -1;
	order_number = -1;
	robot_type = -1;
}

void RobotOrder::SetOrder(int id, int number, int type) {
	customer_id = id;
	order_number = number;
	robot_type = type;
}

int RobotOrder::GetCustomerId() { return customer_id; }
int RobotOrder::GetOrderNumber() { return order_number; }
int RobotOrder::GetRobotType() { return robot_type; }

int RobotOrder::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(robot_type);
}

void RobotOrder::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_robot_type = htonl(robot_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
}

void RobotOrder::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_robot_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	robot_type = ntohl(net_robot_type);
}

bool RobotOrder::IsValid() {
	return (customer_id != -1);
}

void RobotOrder::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "type " << robot_type << std::endl;
}

//// ROBOT ORDER ********************************************************



////***************************** CUSTOMER RECORD *****************************////
CustomerRecord::CustomerRecord() {
    customer_id = -1;
    last_order = -1;
}

CustomerRecord::CustomerRecord(int customer_id_passed, int last_order_passed) {
    customer_id = customer_id_passed;
    last_order = last_order_passed;
}

void CustomerRecord::setCustomerInformation(int customer_id_passed, int last_order_passed) {
    customer_id = customer_id_passed;
    last_order = last_order_passed;
}

int CustomerRecord::getCustomerId() {
    return customer_id;
}

int CustomerRecord::getLastOrder() {
    return last_order;
}

void CustomerRecord::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_last_order_number = htonl(last_order);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_last_order_number, sizeof(net_last_order_number));
}

void CustomerRecord::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_last_order_number;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_last_order_number, buffer + offset, sizeof(net_last_order_number));

	customer_id = ntohl(net_customer_id);
	last_order = ntohl(net_last_order_number);
}

int CustomerRecord::Size() {
	return sizeof(customer_id) + sizeof(last_order);
}

void CustomerRecord::Print() {
//	std::cout << "Customer ID (RECORD): " << customer_id << " ";
//	std::cout << "LAST ORDER (RECORD): " << last_order << std::endl;
	std::cout << "customer_id: " << customer_id << " \t " << "last_order: " << last_order << std::endl;
}
////***************************** CUSTOMER RECORD *****************************////

////***************************** CUSTOMER REQUEST *****************************////
CustomerRequest::CustomerRequest() {
	customer_id = -1;
	order_number = -1;
	request_type = -1;
}

void CustomerRequest::SetRequest(int id, int number, int type) {
	customer_id = id;
	order_number = number;
	request_type = type;
}

int CustomerRequest::GetCustomerId() { return customer_id; }
int CustomerRequest::GetOrderNumber() { return order_number; }
int CustomerRequest::GetRequestType() { return request_type; }

int CustomerRequest::Size() {
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type);
}

void CustomerRequest::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int offset = 0;
	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
}

void CustomerRequest::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int offset = 0;
	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
}

bool CustomerRequest::IsValid() {
	return (customer_id != -1);
}

void CustomerRequest::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "Request Type " << request_type << std::endl;
}
////***************************** CUSTOMER REQUEST *****************************////

//// ROBOT INFO ********************************************************

RobotInfo::RobotInfo() {
	customer_id = -1;
	order_number = -1;
//	robot_type = -1;
    request_type = -1;
	engineer_id = -1;
//	expert_id = -1;
    admin_id = -1;
}

//void RobotInfo::SetInfo(int id, int number, int type, int engid, int expid) {
void RobotInfo::SetInfo(int id, int number, int type, int engid, int adminid) {
	customer_id = id;
	order_number = number;
	request_type = type;
	engineer_id = engid;
	admin_id = adminid;
}

//void RobotInfo::CopyOrder(RobotOrder order) {
//	customer_id = order.GetCustomerId();
//	order_number = order.GetOrderNumber();
//	robot_type = order.GetRobotType();
//}

void RobotInfo::CopyRequest(CustomerRequest request) {
	customer_id = request.GetCustomerId();
	order_number = request.GetOrderNumber();
	request_type = request.GetRequestType();
}


void RobotInfo::SetEngineerId(int id) { engineer_id = id; }
//void RobotInfo::SetExpertId(int id) { expert_id = id; }
void RobotInfo::SetAdminId(int id) {admin_id = id; }


int RobotInfo::GetCustomerId() { return customer_id; }
int RobotInfo::GetOrderNumber() { return order_number; }
//int RobotInfo::GetRobotType() { return robot_type; }
int RobotInfo::GetRequestType() { return request_type; }
int RobotInfo::GetEngineerId() { return engineer_id; }
//int RobotInfo::GetExpertId() { return expert_id; }
int RobotInfo::GetAdminId() { return admin_id; }
int RobotInfo::Size() {
//	return sizeof(customer_id) + sizeof(order_number) + sizeof(robot_type)
//		+ sizeof(engineer_id) + sizeof(expert_id);
	return sizeof(customer_id) + sizeof(order_number) + sizeof(request_type)
		+ sizeof(engineer_id) + sizeof(admin_id);
}

void RobotInfo::Marshal(char *buffer) {
	int net_customer_id = htonl(customer_id);
	int net_order_number = htonl(order_number);
	int net_request_type = htonl(request_type);
	int net_engineer_id = htonl(engineer_id);
	int net_admin_id = htonl(admin_id);
	int offset = 0;

	memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(buffer + offset, &net_request_type, sizeof(net_request_type));
	offset += sizeof(net_request_type);
	memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(buffer + offset, &net_admin_id, sizeof(net_admin_id));

}

void RobotInfo::Unmarshal(char *buffer) {
	int net_customer_id;
	int net_order_number;
	int net_request_type;
	int net_engineer_id;
	int net_admin_id;
	int offset = 0;

	memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id));
	offset += sizeof(net_customer_id);
	memcpy(&net_order_number, buffer + offset, sizeof(net_order_number));
	offset += sizeof(net_order_number);
	memcpy(&net_request_type, buffer + offset, sizeof(net_request_type));
	offset += sizeof(net_request_type);
	memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id));
	offset += sizeof(net_engineer_id);
	memcpy(&net_admin_id, buffer + offset, sizeof(net_admin_id));

	customer_id = ntohl(net_customer_id);
	order_number = ntohl(net_order_number);
	request_type = ntohl(net_request_type);
	engineer_id = ntohl(net_engineer_id);
	admin_id = ntohl(net_admin_id);
}

bool RobotInfo::IsValid() {
	return (customer_id != -1);
}

void RobotInfo::Print() {
	std::cout << "id " << customer_id << " ";
	std::cout << "num " << order_number << " ";
	std::cout << "Request Type " << request_type << " ";
	std::cout << "engid " << engineer_id << " ";
	std::cout << "Admin ID " << admin_id << std::endl;
}
//// ROBOT Info ********************************************************

// Replication Record ********************************************************
ReplicationRequest::ReplicationRequest() {
	factory_id = -1;
	committed_index = -1;
	last_index = -1;
	opcode = -1;
	arg1 = -1;
	arg2 = -1;
}

void ReplicationRequest::SetRequest(int factory_id_passed, int committed_index_passed, int last_index_passed,
                                     int opcode_passed, int arg1_passed, int arg2_passed) {
	factory_id = factory_id_passed;
	committed_index = committed_index_passed;
	last_index = last_index_passed;
	opcode = opcode_passed;
	arg1 = arg1_passed;
	arg2 = arg2_passed;
}

int ReplicationRequest::GetFactoryId() { return factory_id; }
int ReplicationRequest::GetCommittedIndex() { return committed_index; }
int ReplicationRequest::GetLastIndex() { return last_index; }
int ReplicationRequest::GetOpCode() { return opcode; }
int ReplicationRequest::GetArg1() { return arg1; }
int ReplicationRequest::GetArg2() { return arg2; }


int ReplicationRequest::Size() {
	return sizeof(factory_id) + sizeof(committed_index) + sizeof(last_index) + sizeof(opcode) + sizeof(arg1) + sizeof(arg2);
}

void ReplicationRequest::Marshal(char *buffer) {
	int net_factory_id = htonl(factory_id);
	int net_committed_index = htonl(committed_index);
	int net_last_index = htonl(last_index);
    int net_opcode = htonl(opcode);
	int net_arg1 = htonl(arg1);
	int net_arg2 = htonl(arg2);

	int offset = 0;
	memcpy(buffer + offset, &net_factory_id, sizeof(net_factory_id));
	offset += sizeof(net_factory_id);

	memcpy(buffer + offset, &net_committed_index, sizeof(net_committed_index));
	offset += sizeof(net_committed_index);

	memcpy(buffer + offset, &net_last_index, sizeof(net_last_index));
    offset += sizeof(net_last_index);

    memcpy(buffer + offset, &net_opcode, sizeof(net_opcode));
	offset += sizeof(net_opcode);

	memcpy(buffer + offset, &net_arg1, sizeof(net_arg1));
	offset += sizeof(net_arg1);

	memcpy(buffer + offset, &net_arg2, sizeof(net_arg2));
}

void ReplicationRequest::Unmarshal(char *buffer) {
	int net_factory_id;
	int net_committed_index;
	int net_last_index;
    int net_opcode;
	int net_arg1;
	int net_arg2;

	int offset = 0;
	memcpy(&net_factory_id, buffer + offset, sizeof(net_factory_id));
	offset += sizeof(net_factory_id);

	memcpy(&net_committed_index, buffer + offset, sizeof(net_committed_index));
	offset += sizeof(net_committed_index);

	memcpy(&net_last_index, buffer + offset, sizeof(net_last_index));
    offset += sizeof(net_last_index);

	memcpy(&net_opcode, buffer + offset, sizeof(net_opcode));
	offset += sizeof(net_opcode);

	memcpy(&net_arg1, buffer + offset, sizeof(net_arg1));
	offset += sizeof(net_arg1);

	memcpy(&net_arg2, buffer + offset, sizeof(net_arg2));


	factory_id = ntohl(net_factory_id);
	committed_index = ntohl(net_committed_index);
	last_index = ntohl(net_last_index);
	opcode = ntohl(net_opcode);
	arg1 = ntohl(net_arg1);
	arg2 = ntohl(net_arg2);
}

bool ReplicationRequest::IsValid() {
	return (committed_index != -1);
}

void ReplicationRequest::Print() {
	std::cout << "Factory Id: " << factory_id << " ";
	std::cout << "Commited Index: " << committed_index << " ";
    std::cout << "Last Index " << last_index << " ";
	std::cout << "OpCode " << opcode << " ";
    std::cout << "Arg1 " << arg1 << " ";
	std::cout << "Arg2 " << arg2 << std::endl;
}
//// Replication Record ********************************************************
