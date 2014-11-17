/*
 * Acceptor.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#include "Acceptor.h"
#include "net_header.h"
#include "net_function.h"
#include "NetModule.h"
#include "Connection.h"
#include <stdio.h>
#include "lua/LuaEngine.h"

namespace net {

Acceptor::Acceptor(NetModule * module)
	: _fd(-1)
	, _recvBufferSize(0)
	, _sendBufferSize(0)
	, _module(module)
	, _luaSessionCreator(0)
{
	// TODO Auto-generated constructor stub

}

Acceptor::~Acceptor() {
	// TODO Auto-generated destructor stub
}

bool Acceptor::init(int port, int recvBufferSize, int sendBufferSize)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		printf("acceptor create socket:%d\n", errno);
		return false;
	}

	net::setAddrReuse(fd);
	net::setNonBlocking(fd);

	sockaddr_in addr = {0};
	addr.sin_family  = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if(bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		printf("acceptor bind error:%d\n", errno);
		return false;
	}

	if(listen(fd, 128) < 0)
	{
		printf("acceptor listen error:%d\n", errno);
		return false;
	}

	_fd = fd;
	_recvBufferSize = recvBufferSize;
	_sendBufferSize = sendBufferSize;
	return true;
}

void Acceptor::release()
{
	if (_fd > 0)
	{
		::close(_fd);
	}

	if (_luaSessionCreator > 0)
	{
		lua_extend::LuaEngine::Instance()->removeScriptObject(_luaSessionCreator);
	}
}

void Acceptor::onIn()
{
	while (Connection * connection = accept())
	{
		_module->add(connection);
	}
}

void Acceptor::onError()
{
	printf("acceptor error:%d\n", errno);
}

Connection * Acceptor::accept()
{
	sockaddr_in remote;
	unsigned int len = sizeof(remote);
	int fd = ::accept(_fd, (sockaddr*)&remote, &len);
	if(fd == -1)
	{
		if (errno != EAGAIN)
			printf("accept empty: %d.\n", errno);
		return NULL;
	}

	Connection * connection = new Connection(fd, _module);
	if (connection)
	{
		if (_recvBufferSize > 0) {
			connection->setRecvBufferSize(_recvBufferSize);
		}

		if (_sendBufferSize > 0) {
			connection->setSendBufferSize(_sendBufferSize);
		}

		net::setHardClose(fd);
		net::setNonBlocking(fd);
		net::setNonNegal(fd);

		sockaddr_in local;
		unsigned int len = sizeof(local);
		getsockname(_fd, (sockaddr*)&local, &len);
		char ip[128] = {0};
		sprintf(ip, "%s:%d", inet_ntoa(local.sin_addr), ntohs(local.sin_port));
		connection->setLocal(ip);
		sprintf(ip, "%s:%d", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));
		connection->setRemote(ip);
		onCreateConnection(connection);
		connection->onEstablish();
	}
	else
	{
		printf("connection create failed.\n");
	}

	return connection;
}

void Acceptor::onCreateConnection(Connection * connection)
{
	if (_luaSessionCreator > 0)
	{
		lua_extend::LuaEngine::Instance()->pushObject("Connection", connection);
		lua_extend::LuaEngine::Instance()->executeScriptObjectFunction(_luaSessionCreator, "createSession", 1);
	}
}

} /* namespace net */
