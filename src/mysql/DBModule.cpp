/*
 * DBModule.cpp
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */

#include "DBModule.h"
#include "lua/LuaEngine.h"
#include "DBResultSet.h"

DBModule::DBModule()
	: _terminate(false)
	, _running(false)
{
	// TODO Auto-generated constructor stub

}

DBModule::~DBModule() {
	// TODO Auto-generated destructor stub
}

bool DBModule::init(const char* szUser,
				    const char* szHost,
				    const char* szDBName,
				    const char* szPsd,
				    int port,
				    unsigned int connectionflag,
				    const char* szCharset)
{
	if (!_connection.open(szUser, szHost, szDBName, szPsd, port, connectionflag, szCharset))
	{
		return false;
	}

	_thread = std::thread([this](){
		InnerThreadProc();
	});
	
	return true;
}

void DBModule::release()
{
	{
		_terminate = true;
		while (_running)
			_runningCondition.notify_all();
		_thread.join();
	}

	{
		std::unique_lock<std::mutex> guard(_completeMutex);
		while (!_completeCommands.empty())
		{
			(*_completeCommands.begin())->release();
			_completeCommands.pop_front();
		}
	}

	while (!_completeCommandsRun.empty())
	{
		(*_completeCommandsRun.begin())->release();
		_completeCommandsRun.pop_front();
	}
}

void DBModule::run(int count)
{
	int runCount = count;
	while ((count > 0) ? (runCount > 0) : true)
	{
		if (_completeCommandsRun.empty())
		{
			std::unique_lock<std::mutex> guard(_completeMutex);
			_completeCommandsRun.swap(_completeCommands);
		}

		if (_completeCommandsRun.empty()) break;

		IDBCommand* command = *_completeCommandsRun.begin();
		_completeCommandsRun.pop_front();

		command->onComplete();
		command->release();
		--runCount;
	}
}

void DBModule::pushCommand(IDBCommand* command)
{
	std::unique_lock<std::mutex> guard(_runningMutex);
	_runningCommands.push_back(command);
	if (_runningCommands.size() == 1)
		_runningCondition.notify_one();
}

class ExecuteCommand : public IDBCommand
{
public:
	ExecuteCommand(const char* sql, int luaFunction) : _sql(sql), _luaFunction(luaFunction), _errno(0) {}
	virtual ~ExecuteCommand() {}

	virtual bool onExecuteSql(DBConnection& connection)
	{
		return connection.execute(_sql);
	}
	
	virtual void onError(unsigned int errorCode, const char* errorInfo)
	{
		_errno = errorCode;
		_error = errorInfo;
	}
	
	virtual void onEffectRow(int count) {}
	virtual void onComplete() 
	{
		if (_luaFunction > 0)
		{
			lua_extend::LuaEngine::Instance()->pushBoolean(_errno != 0);
			if (_errno != 0)
			{
				lua_extend::LuaEngine::Instance()->pushString(_error.c_str());
				lua_extend::LuaEngine::Instance()->executeScriptFunction(_luaFunction, 2);	
			}
			else
				lua_extend::LuaEngine::Instance()->executeScriptFunction(_luaFunction, 1);
		}
	}
	
	virtual void release() 
	{
		lua_extend::LuaEngine::Instance()->removeScriptFunction(_luaFunction);
		delete this; 
	}
	
private:
	std::string _sql;
	int _luaFunction;
	int _errno;
	std::string _error;
};

void DBModule::execute(const char* sql, int handler)
{
	pushCommand(new ExecuteCommand(sql, handler));
}

class QueryCommand : public IDBCommand
{
public:
	QueryCommand(const char* sql, int luaFunction) : _sql(sql), _luaFunction(luaFunction), _errno(0) {}
	virtual ~QueryCommand() {}

	virtual bool onExecuteSql(DBConnection& connection)
	{
		DBResultSet rs;
		if (!connection.execute(_sql, rs))
			return false;
		
		while (rs.next())
		{
			std::vector<std::string> row;
			for (int i = 0; i < rs.fieldCount(); ++i)
				row.push_back(rs.toBuffer(i));
			_rs.push_back(row);
		}	
		return true;
	}
	
	virtual void onError(unsigned int errorCode, const char* errorInfo)
	{
		_errno = errorCode;
		_error = errorInfo;
	}
	
	virtual void onEffectRow(int count) {}
	virtual void onComplete() 
	{
		if (_luaFunction > 0)
		{
			lua_extend::LuaEngine::Instance()->pushBoolean(_errno != 0);
			if (_errno != 0)
				lua_extend::LuaEngine::Instance()->pushString(_error.c_str());
			else
				lua_extend::LuaEngine::Instance()->pushResultSet(_rs);
			lua_extend::LuaEngine::Instance()->executeScriptFunction(_luaFunction, 2);	
		}
	}
	
	virtual void release() 
	{
		lua_extend::LuaEngine::Instance()->removeScriptFunction(_luaFunction);
		delete this; 
	}
	
private:
	std::string _sql;
	int _luaFunction;
	std::vector<std::vector<std::string>> _rs;
	int _errno;
	std::string _error;
};

void DBModule::query(const char* sql, int handler)
{
	pushCommand(new QueryCommand(sql, handler));
}

void DBModule::InnerThreadProc()
{
	_running = true;
	BoolGuard<false> bool_guard(_running);
	//int processCount = 0;
	while (!_terminate)
	{
		IDBCommand* command = NULL;
		while (!_terminate || command != NULL)
		{
			std::unique_lock<std::mutex> guard(_runningMutex);
			if (_runningCommands.empty())
				_runningCondition.wait(guard);
			if (!_runningCommands.empty())
			{
				command = *_runningCommands.begin();
				_runningCommands.pop_front();
				break;
			}
		}

		if (command)
		{
			executeCommand(command);

			std::unique_lock<std::mutex> guard(_completeMutex);
			_completeCommands.push_back(command);
		}
	}

	_running = false;

	std::unique_lock<std::mutex> guard(_runningMutex);
	while (!_runningCommands.empty())
	{
		IDBCommand* command = *_runningCommands.begin();
		_runningCommands.pop_front();

		executeCommand(command);

		std::unique_lock<std::mutex> guard(_completeMutex);
		_completeCommands.push_back(command);
	}
}

void DBModule::executeCommand(IDBCommand* command)
{
	if (!command) return;

	_connection.isActive();

	if (!command->onExecuteSql(_connection))
	{
		command->onError(_connection.GetLastErrNo(), _connection.GetLastErrInfo());
	}
	else
		command->onEffectRow(_connection.GetAffectedRow());
}
