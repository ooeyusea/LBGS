/*
 * LuaStack.cpp
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#include "LuaStack.h"
#include <string.h>
#include "luabind.h"
#include "lpack.h"
#include "pb.h"
#include "lseri.h"

namespace lua_extend {
#include "lua_constants.h"

int lua_print(lua_State * luastate)
{
    int nargs = lua_gettop(luastate);

    std::string t;
    for (int i=1; i <= nargs; i++)
    {
        if (lua_istable(luastate, i))
            t += "table";
        else if (lua_isnone(luastate, i))
            t += "none";
        else if (lua_isnil(luastate, i))
            t += "nil";
        else if (lua_isboolean(luastate, i))
        {
            if (lua_toboolean(luastate, i) != 0)
                t += "true";
            else
                t += "false";
        }
        else if (lua_isfunction(luastate, i))
            t += "function";
        else if (lua_islightuserdata(luastate, i))
            t += "lightuserdata";
        else if (lua_isthread(luastate, i))
            t += "thread";
        else
        {
            const char * str = lua_tostring(luastate, i);
            if (str)
                t += lua_tostring(luastate, i);
            else
                t += lua_typename(luastate, lua_type(luastate, i));
        }
        if (i!=nargs)
            t += "\t";
    }
    printf("[LUA-print] %s\n", t.c_str());

    return 0;
}

LuaStack::LuaStack()
	: _state(NULL)
	, _nextObjectId(0)
{
	// TODO Auto-generated constructor stub

}

LuaStack::~LuaStack() {
	// TODO Auto-generated destructor stub
}

bool LuaStack::init()
{
    _state = luaL_newstate();
    luaL_openlibs(_state);

    // Register our version of the global "print" function
    const luaL_reg global_functions [] = {
        {"print", lua_print},
        {NULL, NULL}
    };
    luaL_register(_state, "_G", global_functions);

    lua_pushstring(_state, REFID_OBJECT_MAPPING);
    lua_newtable(_state);
    lua_rawset(_state, LUA_REGISTRYINDEX);

    lua_pushstring(_state, REFID_FUNCTION_MAPPING);
    lua_newtable(_state);
    lua_rawset(_state, LUA_REGISTRYINDEX);

	luaopen_pb(_state);
	luaopen_pack(_state);
	luaopen_seri(_state);

    registerNetToLua(_state);

    return true;
}

void LuaStack::release()
{
	lua_close(_state);
}

void LuaStack::addSearchPath(const char* path)
{
    lua_getglobal(_state, "package");
    lua_getfield(_state, -1, "path");
    const char* cur_path =  lua_tostring(_state, -1);
    lua_pushfstring(_state, "%s;%s/?.lua", cur_path, path);
    lua_setfield(_state, -3, "path");
    lua_pop(_state, 2);
}

int LuaStack::reload(const char* moduleFileName)
{
    if (!moduleFileName || strlen(moduleFileName) == 0)
    {
        return 1;
    }

    lua_getglobal(_state, "package");                         /* L: package */
    lua_getfield(_state, -1, "loaded");                       /* L: package loaded */
    lua_pushstring(_state, moduleFileName);
    lua_gettable(_state, -2);                                 /* L:package loaded module */
    if (!lua_isnil(_state, -1))
    {
        lua_pushstring(_state, moduleFileName);               /* L:package loaded module name */
        lua_pushnil(_state);                                  /* L:package loaded module name nil*/
        lua_settable(_state, -4);                             /* L:package loaded module */
    }
    lua_pop(_state, 3);

    std::string name = moduleFileName;
    std::string require = "require \'" + name + "\'";
    return executeString(require.c_str());
}

int LuaStack::executeString(const char *codes)
{
    luaL_loadstring(_state, codes);
    return executeFunction(0);
}

int LuaStack::executeScriptFile(const char* filename)
{
    std::string code("require \"");
    code.append(filename);
    code.append("\"");
    return executeString(code.c_str());
}

int LuaStack::executeGlobalFunction(const char* functionName)
{
    lua_getglobal(_state, functionName);       /* query function by name, stack: function */
    if (!lua_isfunction(_state, -1))
    {
        lua_pop(_state, 1);
        return 0;
    }
    return executeFunction(0);
}

void LuaStack::clean()
{
    lua_settop(_state, 0);
}

void LuaStack::pushInt(int intValue)
{
    lua_pushinteger(_state, intValue);
}

void LuaStack::pushFloat(float floatValue)
{
    lua_pushnumber(_state, floatValue);
}

void LuaStack::pushLong(long longValue)
{
    lua_pushnumber(_state, longValue);
}

void LuaStack::pushBoolean(bool boolValue)
{
    lua_pushboolean(_state, boolValue);
}

void LuaStack::pushString(const char* stringValue)
{
    lua_pushstring(_state, stringValue);
}

void LuaStack::pushString(const char* stringValue, int length)
{
    lua_pushlstring(_state, stringValue, length);
}

bool LuaStack::pushScriptObjectByHandler(int handler)
{
	return lua_extend::pushScriptObjectByHandler(_state, handler);
}

void LuaStack::pushNil()
{
    lua_pushnil(_state);
}

void LuaStack::pushResultSet(std::vector<std::vector<std::string>>& rs)
{
	lua_newtable(_state);
	int i = 1;
	for (auto& row : rs)
	{
		lua_newtable(_state);
		int j = 1;
		for (auto& field : row)
		{
			lua_pushlstring(_state, field.c_str(), field.size());
			lua_rawseti(_state, -2, j++);
		}
		lua_rawseti(_state, -2, i++);
	}
}

int LuaStack::executeFunction(int numArgs)
{
    int functionIndex = -(numArgs + 1);
    if (!lua_isfunction(_state, functionIndex))
    {
        lua_pop(_state, numArgs + 1); // remove function and arguments
        return 0;
    }

    int traceback = 0;
    lua_getglobal(_state, "__G__TRACKBACK__");                         /* L: ... func arg1 arg2 ... G */
    if (!lua_isfunction(_state, -1))
    {
        lua_pop(_state, 1);                                            /* L: ... func arg1 arg2 ... */
    }
    else
    {
        lua_insert(_state, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
        traceback = functionIndex - 1;
    }

    int error = 0;
    error = lua_pcall(_state, numArgs, 1, traceback);                  /* L: ... [G] ret */
    if (error)
    {
        if (traceback == 0)
        {
            lua_pop(_state, 1); // remove error message from stack
        }
        else                                                            /* L: ... G error */
        {
            lua_pop(_state, 2); // remove __G__TRACKBACK__ and error message from stack
        }
        return 0;
    }

    // get return value
    int ret = 0;
    if (lua_isnumber(_state, -1))
    {
        ret = (int)lua_tointeger(_state, -1);
    }
    else if (lua_isboolean(_state, -1))
    {
        ret = (int)lua_toboolean(_state, -1);
    }
    // remove return value from stack
    lua_pop(_state, 1);                                                /* L: ... [G] */

    if (traceback)
    {
        lua_pop(_state, 1); // remove __G__TRACKBACK__ from stack      /* L: ... */
    }

    return ret;
}

int LuaStack::retainScriptObjectHandler(int index)
{
	return lua_extend::retainScriptObjectHandler(_state, index);
}

void LuaStack::releaseScriptObjectHandler(int handler)
{
	lua_extend::releaseScriptObjectHandler(_state, handler);
}

int LuaStack::executeScriptObjectFunction(int handler, const char* func, int numArgs)
{
	if (!pushScriptObjectByHandler(handler)) /* L: .. arg1 arg2 ... handler */
	{
		lua_pop(_state, numArgs + 1);
		return 0;
	}

	lua_pushstring(_state, func); /* L: .. arg1 arg2 ... handler funcname */
	lua_gettable(_state, -2); /* L: .. arg1 arg2 ... handler func */

	if (!lua_isfunction(_state, -1))
	{
		lua_pop(_state, numArgs + 2); /* L: .. arg1 arg2 ... */
		return 0;
	}

	if (numArgs > 0)
	{
		lua_insert(_state, -(numArgs + 2)); /* L: .. func arg1 arg2 ... handler */
		lua_insert(_state, -(numArgs + 1)); /* L: .. func handler arg1 arg2 ... */
	}
	else
	{
		lua_insert(_state, -2); /* L: .. func handler */
	}

    return executeFunction(numArgs + 1);
}

int LuaStack::retainScriptFunctionHandler(int index)
{
	return lua_extend::retainScriptFunctionHandler(_state, index);
}

void LuaStack::releaseScriptFunctionHandler(int handler)
{
	lua_extend::releaseScriptFunctionHandler(_state, handler);
}

bool LuaStack::executeScriptFunction(int handler, int numArgs)
{
	if (!lua_extend::pushScriptFunctionByHandler(_state, handler)) /* L: arg1 arg2 ... func */
	{
		return false;
	}
	
	if (numArgs > 0)
	{
		lua_insert(_state, -(numArgs + 1)); /* L: func arg1 arg2 ... */
	}

	return executeFunction(numArgs) != 0;
}

} /* namespace lua_extend */
