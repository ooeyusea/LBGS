/*
 * net_function.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#ifndef NET_FUNCTION_H_
#define NET_FUNCTION_H_

namespace net {

	bool setNonBlocking(int fd);
	bool setHardClose(int fd);
	bool setRecvBuffSize(int fd, long buflen);
	bool setSendBuffSize(int fd, long buflen );
	bool setNonNegal(int fd);
	bool setNegal(int fd);
	bool setAddrReuse(int fd);

}

#endif /* NET_FUNCTION_H_ */
