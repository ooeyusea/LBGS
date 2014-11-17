/*
 * luafunction.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#include "luafunction.h"
#include "lua_constants.h"

namespace lua_extend {

int nextObjectId = 0;
int nextFunctionId = 0;

int lua_get(lua_State * L)
{
	lua_getmetatable(L, 1);
	lua_pushvalue(L, 2);
	lua_rawget(L, -2);

	return 1;
}

void createClass(lua_State * state, const std::string& name)
{
	lua_newtable(state);
	lua_pushstring(state, "__CLASS__");
	lua_pushstring(state, name.c_str());
	lua_rawset(state, -3);

	lua_pushstring(state, "__index");
	lua_pushcclosure(state, lua_get, 0);
	lua_rawset(state, -3);

	lua_setglobal(state, name.c_str());
}

void createClassFunction(lua_State * state, const std::string& name, const std::string& functionName, lua_CFunction func)
{
	lua_getglobal(state, name.c_str());
	if (lua_istable(state, -1))
	{
		lua_pushstring(state, functionName.c_str());
		lua_pushcclosure(state, func, 0);
		lua_rawset(state, -3);
	}
	lua_pop(state, 1);
}

bool isObjectOf(lua_State * state, int index, const std::string& name)
{
	if (!lua_isuserdata(state, index))
		return false;

	lua_getmetatable(state, index);
	lua_pushstring(state, "__CLASS__");
	lua_rawget(state, -2);

	if (lua_isstring(state, -1))
	{
		std::string classname = lua_tostring(state, -1);
		lua_pop(state, 2);
		return classname == name;
	}
	lua_pop(state, 2);
	return true;
}

bool pushScriptObjectByHandler(lua_State * state, int handler)
{
    lua_pushstring(state, REFID_OBJECT_MAPPING);
    lua_rawget(state, LUA_REGISTRYINDEX);
    lua_pushinteger(state, handler);
    lua_rawget(state, -2);
    lua_remove(state, -2);

    if (!lua_istable(state, -1))
	{
		lua_pop(state, 1);
		return false;
	}
	return true;
}

int retainScriptObjectHandler(lua_State * state, int index)
{
	if (!lua_istable(state, index)) return 0;

	++nextObjectId;

	if (nextObjectId < 1)
		nextObjectId = 1;

	lua_pushvalue(state, index);
    lua_pushstring(state, REFID_OBJECT_MAPPING);
    lua_rawget(state, LUA_REGISTRYINDEX);
    lua_pushinteger(state, nextObjectId);
    lua_pushvalue(state, -3);
    lua_rawset(state, -3);
    lua_pop(state, 2);

    return nextObjectId;
}

void releaseScriptObjectHandler(lua_State * state, int handler)
{
    lua_pushstring(state, REFID_OBJECT_MAPPING);
    lua_rawget(state, LUA_REGISTRYINDEX);
    lua_pushinteger(state, handler);
    lua_pushnil(state);
    lua_rawset(state, -3);
    lua_pop(state, 1);
}

bool pushScriptFunctionByHandler(lua_State * state, int handler)
{
	lua_pushstring(state, REFID_FUNCTION_MAPPING);
	lua_rawget(state, LUA_REGISTRYINDEX);
	lua_pushinteger(state, handler);
	lua_rawget(state, -2);
	lua_remove(state, -2);

	if (!lua_isfunction(state, -1))
	{
		lua_pop(state, 1);
		return false;
	}
	return true;
}

int retainScriptFunctionHandler(lua_State * state, int index)
{
	if (!lua_isfunction(state, index)) return 0;

	++nextFunctionId;

	if (nextFunctionId < 1)
		nextFunctionId = 1;

	lua_pushvalue(state, index);
    lua_pushstring(state, REFID_FUNCTION_MAPPING);
    lua_rawget(state, LUA_REGISTRYINDEX);
    lua_pushinteger(state, nextFunctionId);
    lua_pushvalue(state, -3);
    lua_rawset(state, -3);
    lua_pop(state, 2);

    return nextFunctionId;
}

void releaseScriptFunctionHandler(lua_State * state, int handler)
{
    lua_pushstring(state, REFID_FUNCTION_MAPPING);
    lua_rawget(state, LUA_REGISTRYINDEX);
    lua_pushinteger(state, handler);
    lua_pushnil(state);
    lua_rawset(state, -3);
    lua_pop(state, 1);
}

} /* namespace lua_extend */
