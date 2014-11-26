/*
 * LuaStack.h
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#ifndef LUASTACK_H_
#define LUASTACK_H_
#include <luajit/lua.hpp>
#include <unistd.h>
#include <string>
#include "luafunction.h"
#include <string>
#include <vector>

namespace lua_extend {

class LuaStack
{
public:
	LuaStack();
    ~LuaStack();

    lua_State* getLuaState() { return _state; }

    bool init();
    void release();

    void addSearchPath(const char* path);
    int reload(const char* moduleFileName);

    int executeString(const char* codes);
    int executeScriptFile(const char* filename);
    int executeGlobalFunction(const char* functionName);

    void clean();
    void pushInt(int intValue);
    void pushFloat(float floatValue);
    void pushLong(long longValue);
    void pushBoolean(bool boolValue);
    void pushString(const char* stringValue);
    void pushString(const char* stringValue, int length);
    bool pushScriptObjectByHandler(int handler);
    void pushNil();
	void pushResultSet(std::vector<std::vector<std::string>>& rs);

	template <typename T>
	void pushObject(const std::string& name, T * t)
	{
		lua_extend::pushObject(_state, name, t);
	}

    int executeFunction(int numArgs);

    int retainScriptObjectHandler(int index);
    void releaseScriptObjectHandler(int handler);
    int executeScriptObjectFunction(int handler, const char* func, int numArgs);

    int retainScriptFunctionHandler(int index);
    void releaseScriptFunctionHandler(int handler);
    bool executeScriptFunction(int handler, int numArgs = 0);

private:
    lua_State * _state;

    int _nextObjectId;
};

} /* namespace lua_extend */
#endif /* LUASTACK_H_ */
