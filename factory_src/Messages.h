#ifndef __MESSAGES_H__
#define __MESSAGES_H__

#include <string>

class RobotOrder {
private:
	int customer_id;
	int order_number;
	int robot_type;

public:
	RobotOrder();
	void operator = (const RobotOrder &order) {
		customer_id = order.customer_id;
		order_number = order.order_number;
		robot_type = order.robot_type;
	}
	void SetOrder(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRobotType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};


/////////////////////////////CUSTOMER RECORD///////////////////////
class CustomerRecord {
private:
    int customer_id;
    int last_order;
public:
    CustomerRecord();
    CustomerRecord(int customer_id_passed, int last_order_passed);
    void setCustomerInformation(int customer_id_passed, int last_order_passed);
    int getCustomerId();
    int getLastOrder();
    void Marshal(char *buffer);
	void Unmarshal(char *buffer);
	int Size();
    void Print();

};
////////////////////////////CUSTOMER RECORD//////////////////////
////////////////////////////////////CUSTOMER REQUEST
class CustomerRequest {
private:
	int customer_id;
	int order_number;
	int request_type;

public:
	CustomerRequest();
	void operator = (const CustomerRequest &request) {
		customer_id = request.customer_id;
		order_number = request.order_number;
		request_type = request.request_type;
	}
	void SetRequest(int cid, int order_num, int type);
	int GetCustomerId();
	int GetOrderNumber();
	int GetRequestType();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};
////////////////////////////////////CUSTOMER REQUEST

class RobotInfo {
private:
	int customer_id;
	int order_number;
//	int robot_type;
    int request_type;
	int engineer_id;
//	int expert_id;
    int admin_id;

public:
	RobotInfo();
	void operator = (const RobotInfo &info) {
		customer_id = info.customer_id;
		order_number = info.order_number;
//		robot_type = info.robot_type;
        request_type = info.request_type;
		engineer_id = info.engineer_id;
//		expert_id = info.expert_id;
        admin_id = info.admin_id;
	}
//	void SetInfo(int cid, int order_num, int type, int engid, int expid);
	void SetInfo(int cid, int order_num, int type, int engid, int adminid);

//	void CopyOrder(RobotOrder order);
    void CopyRequest(CustomerRequest request);

	void SetEngineerId(int id);

//	void SetExpertId(int id);
    void SetAdminId(int id);

	int GetCustomerId();
	int GetOrderNumber();

//	int GetRobotType();
    int GetRequestType();

	int GetEngineerId();

//	int GetExpertId();
    int GetAdminId();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};
////////////////////////////////////ReplicationRequest//////////////////////////////
class ReplicationRequest {
private:
	int factory_id;
	int committed_index;
	int last_index;
	int opcode;
	int arg1;
	int arg2;

public:
	ReplicationRequest();
	void operator = (const ReplicationRequest &replication_request) {
		factory_id = replication_request.factory_id;
		committed_index = replication_request.committed_index;
		last_index = replication_request.last_index;
        opcode = replication_request.opcode;
		arg1 = replication_request.arg1;
		arg2 = replication_request.arg2;
	}
	void SetRequest(int factory_id_passed, int committed_index_passed, int last_index_passed, int opcode_passed, int arg1_passed, int arg2_passed);
	int GetFactoryId();
	int GetCommittedIndex();
	int GetLastIndex();
    int GetOpCode();
	int GetArg1();
	int GetArg2();

	int Size();

	void Marshal(char *buffer);
	void Unmarshal(char *buffer);

	bool IsValid();

	void Print();
};
////////////////////////////////////ReplicationRequest//////////////////////////////

#endif // #ifndef __MESSAGES_H__
