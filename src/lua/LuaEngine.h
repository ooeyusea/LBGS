/*
 * LuaEngine.h
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#ifndef LUAENGINE_H_
#define LUAENGINE_H_
#include "util/singleton.h"
#include "LuaStack.h"

namespace lua_extend{

class LuaEngine : public util::Singleton<LuaEngine> {
public:
	LuaEngine();
	~LuaEngine();

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
    void pushNil();

	template <typename T>
	void pushObject(const std::string& name, T * t)
	{
		_stack.pushObject(name, t);
	}

	int executeFunction(int numArgs);

	void removeScriptObject(int handler);
	int executeScriptObjectFunction(int handler, const char* func, int numArgs);

	void removeScriptFunction(int handler);
	bool executeScriptFunction(int handler);

private:
	LuaStack _stack;
};

} /* namespace lua_extend */
#endif /* LUAENGINE_H_ */
