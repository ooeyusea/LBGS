/*
 * Connector.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_
#include "EpollHanlder.h"
#include <string>


namespace net {

class NetModule;
class Connection;

class Connector : public EpollHandler {

	friend class NetModule;

	enum
	{
		UNCONNECTED,
		CONNECTING,
		CONNECTED,
	};

public:
	Connector(NetModule * module);
	~Connector();

	bool init(const char * url, int recvBufferSize, int sendBufferSize);
	void setCreator(int creator) { _luaSessionCreator = creator; }
	void release();

	void reconnect();
	void connectionBroke();
	bool isUnconnected() const { return _state == UNCONNECTED; }

	virtual void onOut();
	virtual void onError();

private:
	Connection * transfer();
	bool check(const char * url);
	void onCreateConnection(Connection * connection);

private:
	int _fd;
	std::string _url;
	int _recvBufferSize;
	int _sendBufferSize;
	int _state;

	NetModule * _module;

	int _luaSessionCreator;
};

} /* namespace net */
#endif /* CONNECTOR_H_ */
