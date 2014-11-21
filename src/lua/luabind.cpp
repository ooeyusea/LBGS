/*
 * luabind.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#include "luabind.h"
#include "luafunction.h"
#include "net/Acceptor.h"
#include "net/Connector.h"
#include "net/Connection.h"
#include "net/NetModule.h"
#include "time/TimerQueue.h"
#include "lua_constants.h"

namespace lua_extend {

int registerNetModuleFunctionInstance(lua_State * state)
{
	pushObject<net::NetModule>(state, "NetModule", net::NetModule::Instance());
	return 1;
}

int registerNetModuleFunctionCreateAcceptor(lua_State * state)
{
	net::NetModule * t = toObject<net::NetModule>(state, -1, "NetModule");
	net::Acceptor * ret = t->createAcceptor();
	if (ret)
	{
		pushObject<net::Acceptor>(state, "Acceptor", ret);
	}
	else
		lua_pushnil(state);
	return 1;
}

int registerNetModuleFunctionCreateConnector(lua_State * state)
{
	net::NetModule * t = toObject<net::NetModule>(state, -1, "NetModule");
	net::Connector * ret = t->createConnector();
	if (ret)
	{
		pushObject<net::Connector>(state, "Connector", ret);
	}
	else
		lua_pushnil(state);
	return 1;
}

int registerNetModuleFunctionAddAcceptor(lua_State * state)
{
	net::NetModule * t = toObject<net::NetModule>(state, -2, "NetModule");
	net::Acceptor * acceptor = toObject<net::Acceptor>(state, -1, "Acceptor");
	t->add(acceptor);
	return 0;
}

int registerNetModuleFunctionAddConnector(lua_State * state)
{
	net::NetModule * t = toObject<net::NetModule>(state, -2, "NetModule");
	net::Connector * connector = toObject<net::Connector>(state, -1, "Connector");
	t->add(connector);
	return 0;
}

int registerNetModuleFunctionDestroyAcceptor(lua_State * state)
{
	net::NetModule * t = toObject<net::NetModule>(state, -2, "NetModule");
	net::Acceptor * acceptor = toObject<net::Acceptor>(state, -1, "Acceptor");
	t->destroyAcceptor(acceptor);
	return 0;
}

int registerNetModuleFunctionDestroyConnector(lua_State * state)
{
	net::NetModule * t = toObject<net::NetModule>(state, -2, "NetModule");
	net::Connector * connector = toObject<net::Connector>(state, -1, "Connector");
	t->destroyConnector(connector);
	return 0;
}

void registerNetModule(lua_State * state)
{
	createClass(state, "NetModule");
	createClassFunction(state, "NetModule", "instance", registerNetModuleFunctionInstance);
	createClassFunction(state, "NetModule", "createAcceptor", registerNetModuleFunctionCreateAcceptor);
	createClassFunction(state, "NetModule", "createConnector", registerNetModuleFunctionCreateConnector);
	createClassFunction(state, "NetModule", "addAcceptor", registerNetModuleFunctionAddAcceptor);
	createClassFunction(state, "NetModule", "addConnector", registerNetModuleFunctionAddConnector);
	createClassFunction(state, "NetModule", "destroyAcceptor", registerNetModuleFunctionDestroyAcceptor);
	createClassFunction(state, "NetModule", "destroyConnector", registerNetModuleFunctionDestroyConnector);
}

int registerAcceptorFunctionInit(lua_State * state)
{
	net::Acceptor * t = toObject<net::Acceptor>(state, -4, "Acceptor");
	int port = (int)lua_tonumber(state, -3);
	int recvBufferSize = (int)lua_tonumber(state, -2);
	int sendBufferSize = (int)lua_tonumber(state, -1);

	if (t && t->init(port, recvBufferSize, sendBufferSize))
	{
		lua_pushboolean(state, true);
	}
	else
		lua_pushboolean(state, false);
	return 1;
}

int registerAcceptorFunctionSetCreator(lua_State * state)
{
	net::Acceptor * t = toObject<net::Acceptor>(state, -2, "Acceptor");
	int handler = retainScriptObjectHandler(state, -1);
	if (handler > 0)
		t->setCreator(handler);
	return 0;
}

void registerAcceptor(lua_State * state)
{
	createClass(state, "Acceptor");
	createClassFunction(state, "Acceptor", "init", registerAcceptorFunctionInit);
	createClassFunction(state, "Acceptor", "setCreator", registerAcceptorFunctionSetCreator);
}

int registerConnectorFunctionInit(lua_State * state)
{
	net::Connector * t = toObject<net::Connector>(state, -4, "Connector");
	const char * url = lua_tostring(state, -3);
	int recvBufferSize = (int)lua_tonumber(state, -2);
	int sendBufferSize = (int)lua_tonumber(state, -1);

	if (t && url && t->init(url, recvBufferSize, sendBufferSize))
	{
		lua_pushboolean(state, true);
	}
	else
		lua_pushboolean(state, false);
	return 1;
}

int registerConnectorFunctionSetCreator(lua_State * state)
{
	net::Connector * t = toObject<net::Connector>(state, -2, "Connector");
	int handler = retainScriptObjectHandler(state, -1);
	if (handler > 0)
		t->setCreator(handler);
	return 0;
}

void registerConnector(lua_State * state)
{
	createClass(state, "Connector");
	createClassFunction(state, "Connector", "init", registerConnectorFunctionInit);
	createClassFunction(state, "Connector", "setCreator", registerConnectorFunctionSetCreator);
}

int registerConnectionFunctionSetSession(lua_State * state)
{
	net::Connection * t = toObject<net::Connection>(state, -2, "Connection");
	int handler = retainScriptObjectHandler(state, -1);
	if (handler > 0)
		t->setSession(handler);
	return 0;
}

int registerConnectionFunctionSend(lua_State * state)
{
	net::Connection * t = toObject<net::Connection>(state, -2, "Connection");
	size_t len = 0;
	const char * buffer = lua_tolstring(state, -1, &len);
	if (len > 0)
		t->send(buffer, len);
	return 0;
}

int registerConnectionFunctionClose(lua_State * state)
{
	net::Connection * t = toObject<net::Connection>(state, -1, "Connection");
	if (t)
		t->close();
	return 0;
}

int registerConnectionFunctionGetLocal(lua_State * state)
{
	net::Connection * t = toObject<net::Connection>(state, -1, "Connection");
	lua_pushstring(state, t->getLocal());
	return 1;
}

int registerConnectionFunctionGetRemote(lua_State * state)
{
	net::Connection * t = toObject<net::Connection>(state, -1, "Connection");
	lua_pushstring(state, t->getRemote());
	return 1;
}

void registerConnection(lua_State * state)
{
	createClass(state, "Connection");
	createClassFunction(state, "Connection", "setSession", registerConnectionFunctionSetSession);
	createClassFunction(state, "Connection", "send", registerConnectionFunctionSend);
	createClassFunction(state, "Connection", "close", registerConnectionFunctionClose);
	createClassFunction(state, "Connection", "getLocal", registerConnectionFunctionGetLocal);
	createClassFunction(state, "Connection", "getRemote", registerConnectionFunctionGetRemote);
}

int registerTimerQueueFunctionInstance(lua_State * state)
{
	pushObject<TimerQueue>(state, "TimerQueue", TimerQueue::Instance());
	return 1;
}

int registerTimerQueueFunctionCreate(lua_State * state)
{
	TimerQueue * t = toObject<TimerQueue>(state, -3, "TimerQueue");
	int handler = retainScriptFunctionHandler(state, -2);
	long msseconds = (long)lua_tonumber(state, -1);
	if (t && handler > 0)
	{
		t->CreateLuaTimer(handler, msseconds);
	}
	return 0;
}

void RegisterTimerQueue(lua_State * state)
{
	createClass(state, "TimerQueue");
	createClassFunction(state, "TimerQueue", "instance", registerTimerQueueFunctionInstance);
	createClassFunction(state, "TimerQueue", "create", registerTimerQueueFunctionCreate);
}

void registerNetToLua(lua_State * state)
{
	registerNetModule(state);
	registerAcceptor(state);
	registerConnector(state);
	registerConnection(state);
	RegisterTimerQueue(state);
}

} /* namespace lua_extend */
