/*
 * DBModule.h
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */

#ifndef DBMODULE_H_
#define DBMODULE_H_
#include "DBConnection.h"
#include "DBCommand.h"
#include <list>
#include <thread>
#include <mutex> 
#include <condition_variable>

class DBModule
{
	template <bool ValueSetWhenReturn>
	class BoolGuard
	{
	public:
		explicit BoolGuard(bool& guardValue) : _guardValue(guardValue) {}
		~BoolGuard() { _guardValue = ValueSetWhenReturn; }

	private:
		bool& _guardValue;
	};

public:
	DBModule();
	~DBModule();

	bool init(const char* szUser,
			  const char* szHost,
			  const char* szDBName,
			  const char* szPsd,
			  int port,
			  unsigned int connectionflag,
			  const char* szCharset);

	void release();

	void run(int count = -1);

	void pushCommand(IDBCommand* command);
	void execute(const char* sql, int handler);
	void query(const char* sql, int handler);

	void InnerThreadProc();

private:
	void executeCommand(IDBCommand* command);

private:
	DBConnection _connection;
	bool _terminate;
	bool _running;
	std::thread _thread;
	std::mutex _runningMutex;
	std::condition_variable _runningCondition;
	std::list<IDBCommand*> _runningCommands;
	std::mutex _completeMutex;
	std::list<IDBCommand*> _completeCommands;
	std::list<IDBCommand*> _completeCommandsRun;
};

#endif /* DBMODULE_H_ */
