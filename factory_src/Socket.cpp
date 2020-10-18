#include "Socket.h"

#include <iostream>

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/types.h>

Socket::Socket() : is_initialized_(false), nagle_(NAGLE_ON) { } ///NAGLE?

Socket::~Socket() {
	if (is_initialized_) {
		Close();
	}
}


int Socket::Send(char *buffer, int size, int flags) { ///MARSHALLS SOME DATA AND SENDS IT? NOT MARSHALLING ALL AT ONCE AND THEN SENDING IT AT ONE TIME?
	int bytes_written = 0;
	int offset = 0;
	while (size > 0) {
		bytes_written = send(fd_, buffer + offset, size, flags); //Actually sending as a full buffer. //In unreliable network cases, TCP can't send full buffer at once. //Return value of send is bytes sent so far. // If you send a large buffer only part COULD be set. In that case send will tell you how many bytes sent, if that amount is less. // Useful when doing a synchronous send.
		if (bytes_written < 0) {
			/*
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				perror("ERROR: send retry");
				continue;
			}
			*/
			perror("ERROR: failed to send");
			Close();
			return 0;
		}
		size -= bytes_written;
		offset += bytes_written;
		assert(size >= 0);
	}
	return 1;
}

int Socket::Recv(char *buffer, int size, int flags) { ///UNMARSHALLS SOME DATA AND RECEIVES IT? NOT UMARSHALLING ALL AT ONCE?

	int bytes_read = 0;
	int offset = 0;
	while (size > 0) {
		bytes_read = recv(fd_, buffer + offset, size, flags);
		if (bytes_read <= 0) {
			/*
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				//perror("ERROR: recv retry");
				continue;
			}
			*/
			//perror("ERROR: failed to recv");
			Close();
			return 0;
		}
		assert(bytes_read != 0);

		size -= bytes_read;
		offset += bytes_read;
		assert(size >= 0);
	}
//	   std::cout << "RECEIVE " << std::endl;
	return 1;
}

int Socket::NagleOn(bool on_off) { //////////*********************WHAT IS THIS?
	nagle_ = (on_off ? NAGLE_ON : NAGLE_OFF);
	int result = setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY,
				(void *) &nagle_, sizeof(int));
	if (result < 0) {
		perror("ERROR: setsockopt failed");
		return 0;
	}
	return 1;
}

bool Socket::IsNagleOn() {
	return (nagle_ == NAGLE_ON) ? true : false;
}

void Socket::Close() {
	shutdown(fd_, SHUT_RDWR);
	close(fd_);
	//perror("Socket closed");
	is_initialized_ = false;
}
