/*
 * LuaEngine.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#include "LuaEngine.h"

namespace lua_extend {

LuaEngine::LuaEngine() {
	// TODO Auto-generated constructor stub

}

LuaEngine::~LuaEngine() {
	// TODO Auto-generated destructor stub
}

bool LuaEngine::init()
{
	if (!_stack.init())
		return false;

	_stack.addSearchPath("./src");
	_stack.executeScriptFile("main");

	return true;
}

void LuaEngine::release()
{
	_stack.executeGlobalFunction("release");
	_stack.release();
}

void LuaEngine::addSearchPath(const char* path)
{
	_stack.addSearchPath(path);
}

int LuaEngine::reload(const char* moduleFileName)
{
	return _stack.reload(moduleFileName);
}

int LuaEngine::executeString(const char* codes)
{
	return _stack.executeString(codes);
}

int LuaEngine::executeScriptFile(const char* filename)
{
	return _stack.executeScriptFile(filename);
}

int LuaEngine::executeGlobalFunction(const char* functionName)
{
	return _stack.executeGlobalFunction(functionName);
}

void LuaEngine::clean()
{
	_stack.clean();
}

void LuaEngine::pushInt(int intValue)
{
	_stack.pushInt(intValue);
}

void LuaEngine::pushFloat(float floatValue)
{
	_stack.pushFloat(floatValue);
}

void LuaEngine::pushLong(long longValue)
{
	_stack.pushLong(longValue);
}

void LuaEngine::pushBoolean(bool boolValue)
{
	_stack.pushBoolean(boolValue);
}

void LuaEngine::pushString(const char* stringValue)
{
	_stack.pushString(stringValue);
}

void LuaEngine::pushString(const char* stringValue, int length)
{
	_stack.pushString(stringValue, length);
}

void LuaEngine::pushNil()
{
	_stack.pushNil();
}

int LuaEngine::executeFunction(int numArgs)
{
	return _stack.executeFunction(numArgs);
}

void LuaEngine::removeScriptObject(int handler)
{
	_stack.releaseScriptObjectHandler(handler);
}

int LuaEngine::executeScriptObjectFunction(int handler, const char* func, int numArgs)
{
	return _stack.executeScriptObjectFunction(handler, func, numArgs);
}

void LuaEngine::removeScriptFunction(int handler)
{
	_stack.releaseScriptFunctionHandler(handler);
}

bool LuaEngine::executeScriptFunction(int handler)
{
	return _stack.executeScriptFunction(handler);
}

} /* namespace lua_extend */
