/*
 * net_function.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */
#include "net_function.h"
#include "net_header.h"

namespace net {

	bool setNonBlocking(int fd)
	{
		int nFlags = fcntl(fd, F_GETFL, 0);
		nFlags |= O_NONBLOCK;
		return fcntl(fd, F_SETFL, nFlags) != -1;
	}

	bool setHardClose(int fd)
	{
		linger linger;
		linger.l_onoff  = 1;
		linger.l_linger = 0;
		if (setsockopt(fd , SOL_SOCKET, SO_LINGER, (const char *)&linger, sizeof(linger) ) != 0 ){
			return false;
		}
		return true;
	}

	bool setRecvBuffSize(int fd, long buflen)
	{
		long val = buflen;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *)&val,   sizeof(val)) != 0) {
			return false;
		}
		return true;
	}

	bool setSendBuffSize(int fd, long buflen )
	{
		long val = buflen;
		if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&val,   sizeof(val)) != 0) {
			return false;
		}
		return true;
	}

	bool setNonNegal(int fd)
	{
		long val = 1l;
		if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,(const char*)&val,  sizeof(val)) != 0) {
			return false;
		}
		return true;
	}

	bool setNegal(int fd)
	{
		long val = 0l;
		if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,(const char*)&val,  sizeof(val)) != 0) {
			return false;
		}
		return true;
	}

	bool setAddrReuse(int fd)
	{
		long val = 1l;
		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&val, sizeof(val)) != 0){
			return false;
		}
		return true;
	}
}

