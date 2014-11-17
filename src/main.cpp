/*
 * main.cc
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#include <iostream>
#include "net/NetModule.h"
#include <sys/time.h>
#include "lua/LuaEngine.h"
#include <signal.h>
#include "time/TimerQueue.h"

bool running = true;

long long getTime()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	return ((long long)tv.tv_sec) * 1000 + tv.tv_usec / 1000;
}

void terminal(int Signal)
{
	running = false;
}

int main(int argc, char ** argv)
{
	if (argc < 2)
	{
		printf("unknown server name\n");
		return 0;
	}

	printf("%s starting.......\n", argv[1]);
	if (!net::NetModule::Instance()->init())
	{
		printf("net module initialize failed.\n");
		return 0;
	}

	if (!lua_extend::LuaEngine::Instance()->init())
	{
		printf("lua engine initialize failed.\n");
		return 0;
	}

	TimerQueue::Instance();

	signal(SIGTERM, terminal);
	signal(SIGPIPE, SIG_IGN);

	printf("%s started\n", argv[1]);

	long long check = getTime();
	while (running)
	{
		static long long last = getTime();
		net::NetModule::Instance()->run();
		lua_extend::LuaEngine::Instance()->executeGlobalFunction("run");

		long long diff = getTime() - last;
		if (diff < 50)
			usleep((50 - diff) * 1000);
		last = getTime();

		if (last > check)
		{
			int count = (last - check) / 50;
			for (int i = 0; i < count; ++i)
				TimerQueue::Instance()->Update();
			check += count * 50;
		}
		else
		{
			check = getTime();
		}
	}

	printf("%s stoping.......\n", argv[1]);
	lua_extend::LuaEngine::Instance()->release();
	net::NetModule::Instance()->release();
	printf("%s stoped\n", argv[1]);

	return 0;
}
