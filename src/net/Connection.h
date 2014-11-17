/*
 * Connection.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_
#include "EpollHanlder.h"
#include <string>

namespace net {

class NetModule;
class Connector;

class Connection : public EpollHandler {

	friend class NetModule;

public:
	Connection(int fd, NetModule * module);
	Connection(int fd, NetModule * module, Connector * connector);
	~Connection();

	void send(const char * buffer, int len);
	void onEstablish();
	void onRecv(const char * buffer, int len);
	void OnTerminate();
	void onRelease();
	void close();

	void run();

	void setRecvBufferSize(size_t size);
	void setSendBufferSize(size_t size);

	void setLocal(const char * local) { _local = local; }
	const char * getLocal() const { return _local.c_str(); }

	void setRemote(const char * remote) { _remote = remote; }
	const char * getRemote() const { return _remote.c_str(); }

	void setSession(int session) { _luaSession = session; }

	virtual void onIn();
	virtual void onOut() { _canSend = true; }
	virtual void onError();

	int parse(const char * buffer, int len);

private:
	int _fd;

	char* _sendBuffer;
	size_t _sendBufferSize;
	size_t _sendedBufferSize;
	bool _canSend;

	char* _recvBuffer;
	size_t _recvBufferSize;
	size_t _recvedBufferSize;

	std::string _local;
	std::string _remote;

	NetModule * _module;
	Connector * _connector;

	int _luaSession;
};

} /* namespace net */
#endif /* CONNECTION_H_ */
