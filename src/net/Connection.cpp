/*
 * Connection.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#include "Connection.h"
#include "net_header.h"
#include <stdlib.h>
#include <string.h>
#include "net_function.h"
#include "Connector.h"
#include "NetModule.h"
#include "lua/LuaEngine.h"

namespace net {

Connection::Connection(int fd, NetModule * module)
	: _fd(fd)
	, _sendBuffer(NULL)
	, _sendBufferSize(DEFAULT_BUFFER_SIZE)
	, _sendedBufferSize(0)
	, _canSend(true)
	, _recvBuffer(NULL)
	, _recvBufferSize(DEFAULT_BUFFER_SIZE)
	, _recvedBufferSize(0)
	, _module(module)
	, _connector(NULL)
	, _luaSession(0)
{
	// TODO Auto-generated constructor stub
	_sendBuffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
	_recvBuffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
}

Connection::Connection(int fd, NetModule * module, Connector *  connector)
	: _fd(fd)
	, _sendBuffer(NULL)
	, _sendBufferSize(DEFAULT_BUFFER_SIZE)
	, _sendedBufferSize(0)
	, _canSend(true)
	, _recvBuffer(NULL)
	, _recvBufferSize(DEFAULT_BUFFER_SIZE)
	, _recvedBufferSize(0)
	, _module(module)
	, _connector(connector)
	, _luaSession(0)
{
	// TODO Auto-generated constructor stub
	_sendBuffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
	_recvBuffer = (char*)malloc(DEFAULT_BUFFER_SIZE);
}

Connection::~Connection() {
	// TODO Auto-generated destructor stub
}

void Connection::send(const char * buffer, int len)
{
	if (!_sendBuffer)
	{
		printf("connection send but buffer is null\n");
		return;
	}

	if (_sendedBufferSize + len > _sendBufferSize)
	{
		close();
	}
	else
	{
		::memcpy(_sendBuffer + _sendedBufferSize, buffer, len);
		_sendedBufferSize += len;
	}
}

void Connection::onEstablish()
{
	if (_luaSession > 0)
		lua_extend::LuaEngine::Instance()->executeScriptObjectFunction(_luaSession, "onEstablish", 0);
}

void Connection::onRecv(const char * buffer, int len)
{
	if (_luaSession > 0)
	{
		lua_extend::LuaEngine::Instance()->pushString(buffer, len);
		lua_extend::LuaEngine::Instance()->executeScriptObjectFunction(_luaSession, "onRecv", 1);
	}
}

void Connection::OnTerminate()
{
	if (_luaSession > 0)
		lua_extend::LuaEngine::Instance()->executeScriptObjectFunction(_luaSession, "onTerminate", 0);

	if (_connector)
		_connector->connectionBroke();
}

void Connection::onRelease()
{
	if (_luaSession > 0)
	{
		lua_extend::LuaEngine::Instance()->removeScriptObject(_luaSession);
	}
}

void Connection::close()
{
	OnTerminate();
	onRelease();
	_module->remove(this);
	::close(_fd);
	_fd = -1;

	delete this;
}

void Connection::run()
{
	if (_canSend)
	{
		while (_sendedBufferSize > 0)
		{
			int ret = ::send(_fd, _sendBuffer, _sendedBufferSize, 0);
			if (ret < 0)
			{
				if( EAGAIN != errno )
				{
					onError();
					close();
				}
				else
					_canSend = false;
				break;
			}
			else if (ret > 0)
			{
				if (_sendedBufferSize >= (size_t)ret)
				{
					::memmove(_sendBuffer, _sendBuffer + ret, _sendedBufferSize - ret);
					_sendedBufferSize -= ret;
				}
			}
		}
	}
}

void Connection::setRecvBufferSize(size_t size)
{
	_sendBuffer = (char*)realloc(_sendBuffer, size);
	_sendBufferSize = size;
}

void Connection::setSendBufferSize(size_t size)
{
	_recvBuffer = (char*)realloc(_recvBuffer, size);
	_recvBufferSize = size;
}

void Connection::onIn()
{
	if (!_recvBuffer)
	{
		close();
		return;
	}

	while (true)
	{
		if (_recvedBufferSize >= _recvBufferSize)
		{
			close();
			return;
		}

		int len = ::recv(_fd, _recvBuffer + _recvedBufferSize, _recvBufferSize - _recvedBufferSize, 0);
		if (len > 0)
		{
			_recvedBufferSize += len;

			size_t pos = 0;
			while (pos < _recvedBufferSize)
			{
				int  used  = parse(_recvBuffer + pos, _recvedBufferSize - pos);
				if ( used == 0 )
				{
					break;
				}
				else if(used > 0)
				{
					onRecv(_recvBuffer + pos, used);
					pos += (size_t)used;
				}
				else
				{
					close();
					return;
				}
			}

			if (_recvedBufferSize > pos)
			{
				::memmove(_recvBuffer, _recvBuffer + pos, _recvedBufferSize - pos);
				_recvedBufferSize -= pos;
			}
			else
				_recvedBufferSize = 0;
		}
		else if (len < 0)
		{
			if ((errno != EAGAIN) && (errno != EINPROGRESS))
			{
				onError();
				close();
			    printf("recv:%d\n", errno);
            }
			break;
		}
		else
		{
			close();
			break;
		}
	}
}

void Connection::onError()
{
	printf("connection error:%d\n", errno);
	close();
}

int Connection::parse(const char * buffer, int len)
{
	if (len > 4)
	{
		int needLen = *((unsigned short*)(buffer + 2));
		if (len < needLen)
			return 0;

		return needLen;
	}
	else
		return 0;
}

} /* namespace net */
