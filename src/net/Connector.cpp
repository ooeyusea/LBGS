/*
 * Connector.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#include "Connector.h"
#include "NetModule.h"
#include "Connection.h"
#include "net_header.h"
#include "net_function.h"
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include "lua/LuaEngine.h"

namespace net {

Connector::Connector(NetModule * module)
	: _fd(-1)
	, _recvBufferSize(0)
	, _sendBufferSize(0)
	, _state(UNCONNECTED)
	, _module(module)
	, _luaSessionCreator(0)
{
	// TODO Auto-generated constructor stub

}

Connector::~Connector() {
	// TODO Auto-generated destructor stub
}

bool Connector::init(const char * url, int recvBufferSize, int sendBufferSize)
{
	if (!check(url))
		return false;

	_url = url;
	_recvBufferSize = recvBufferSize;
	_sendBufferSize = sendBufferSize;

	return true;
}

void Connector::release()
{
	connectionBroke();

	if (_luaSessionCreator > 0)
	{
		lua_extend::LuaEngine::Instance()->removeScriptObject(_luaSessionCreator);
	}
}

void Connector::reconnect()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		return;

	net::setNonBlocking(fd);

	std::string hostname;
	int port = 0;
	std::string::size_type pos = _url.find(':');
	if (pos == std::string::npos)
	{
		hostname = _url;
		port = 80;
	}
	else
	{
		hostname = _url.substr(0, pos);
		port = atoi(_url.substr(pos + 1).c_str());
	}

	hostent *host = ::gethostbyname(hostname.c_str());
	if (!host)
	{
		::close(fd);
		return;
	}

	std::string realIp;
	char buffer[64] = {0};
	switch (host->h_addrtype)
	{
	case AF_INET:
		{
			if (host->h_addr_list[0] != NULL)
			{
				realIp = ::inet_ntop(AF_INET, host->h_addr_list[0], buffer, sizeof(buffer));
			}
			break;
		}
	}

	if (realIp.empty())
	{
		::close(fd);
		return;
	}

	//addr
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ::inet_addr(realIp.c_str());
	addr.sin_port = htons(port);
	if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		if( errno != EINPROGRESS)
		{
			return;
		}
	}

	_state = CONNECTING;
	_fd = fd;
	_module->addToEpoll(this);
}

void Connector::connectionBroke()
{
	if (_fd > 0)
	{
		_module->removeFromEpoll(this);
		::close(_fd);
		_fd = -1;
	}

	_state = UNCONNECTED;
}

void Connector::onOut()
{
	Connection * connection = transfer();
	if (connection)
	{
		_module->removeFromEpoll(this);
		_fd = -1;
		_state = CONNECTED;
		_module->add(connection);
	}
	else
	{
		connectionBroke();
	}
}

void Connector::onError()
{
	connectionBroke();
}

Connection * Connector::transfer()
{
	Connection * connection = new Connection(_fd, _module, this);

	if (connection)
	{
		if (_recvBufferSize > 0) {
			connection->setRecvBufferSize(_recvBufferSize);
		}

		if (_sendBufferSize > 0) {
			connection->setSendBufferSize(_sendBufferSize);
		}

		net::setHardClose(_fd);
		net::setNonNegal(_fd);

		sockaddr_in local;
		unsigned int len = sizeof(local);
		getsockname(_fd, (sockaddr*)&local, &len);
		char ip[128] = {0};
		sprintf(ip, "%s:%d", inet_ntoa(local.sin_addr), ntohs(local.sin_port));
		connection->setLocal(ip);
		connection->setRemote(_url.c_str());
		onCreateConnection(connection);
		connection->onEstablish();
	}

	return connection;
}

bool Connector::check(const char * url)
{
	std::string testurl = url;
	std::string hostname;
	std::string::size_type pos = testurl.find(':');
	if (pos == std::string::npos)
	{
		hostname = testurl;
	}
	else
	{
		hostname = testurl.substr(0, pos);
	}

	hostent *host = ::gethostbyname(hostname.c_str());
	if (!host)
	{
		return false;
	}

	std::string realIp;
	char buffer[64] = {0};
	switch (host->h_addrtype)
	{
	case AF_INET:
		{
			if (host->h_addr_list[0] != NULL)
			{
				realIp = ::inet_ntop(AF_INET, host->h_addr_list[0], buffer, sizeof(buffer));
			}
			break;
		}
	}

	if (realIp.empty())
	{
		return false;
	}

	return true;
}

void Connector::onCreateConnection(Connection * connection)
{
	if (_luaSessionCreator > 0)
	{
		lua_extend::LuaEngine::Instance()->pushObject("Connection", connection);
		lua_extend::LuaEngine::Instance()->executeScriptObjectFunction(_luaSessionCreator, "createSession", 1);
	}
}

} /* namespace net */
