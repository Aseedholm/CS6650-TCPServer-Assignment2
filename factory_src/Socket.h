#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <string>

#include <sys/poll.h>
#include <sys/select.h>

#define NAGLE_ON	0
#define NAGLE_OFF	1

class Socket {
protected:
	int fd_;
	bool is_initialized_;

private:
	int nagle_;

public:
	Socket();
	virtual ~Socket();

	int Send(char *buffer, int size, int flags = 0);
	int Recv(char *buffer, int size, int flags = 0);

	int NagleOn(bool on_off); //Allows TCP to wait for a certain amount of sockets to accumulate and sends it all together. Instead of sending packets one on one. Bunching sockets and sending at once can improve performance. 
	bool IsNagleOn();

	void Close();
};


#endif // end of #ifndef __SOCKET_H__
