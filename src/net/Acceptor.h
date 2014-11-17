/*
 * Acceptor.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_
#include "EpollHanlder.h"

namespace net {

class NetModule;
class Connection;

class Acceptor : public EpollHandler {

	friend class NetModule;

public:
	Acceptor(NetModule * module);
	~Acceptor();

	bool init(int port, int recvBufferSize, int sendBufferSize);
	void setCreator(int creator) { _luaSessionCreator = creator; }
	void release();

	virtual void onIn();
	virtual void onError();

private:
	Connection * accept();
	void onCreateConnection(Connection * connection);

private:
	int _fd;
	int _port;
	int _recvBufferSize;
	int _sendBufferSize;

	NetModule * _module;

	int _luaSessionCreator;
};

} /* namespace net */
#endif /* ACCEPTOR_H_ */
