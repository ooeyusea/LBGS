/*
 * luafunction.h
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#ifndef LUAFUNCTION_H_
#define LUAFUNCTION_H_
#include <string>
#include <luajit/lua.hpp>
#include <unistd.h>
#include <string.h>

namespace lua_extend {

	void createClass(lua_State * state, const std::string& name);
	void createClassFunction(lua_State * state, const std::string& name, const std::string& functionName, lua_CFunction func);

	bool isObjectOf(lua_State * state, int index, const std::string& name);

	template <typename T>
	T* toObject(lua_State * state, int index, const std::string& name)
	{
		if (!isObjectOf(state, index, name))
			return NULL;

		return *((T**)lua_touserdata(state, index));
	}

	template <typename T>
	void pushObject(lua_State * state, const std::string& name, T * t)
	{
		void * p = lua_newuserdata(state, sizeof(T*));
		memcpy(p, &t, sizeof(T*));

		lua_getglobal(state, name.c_str());
		lua_setmetatable(state, -2);
	}

	bool pushScriptObjectByHandler(lua_State * state, int handler);
	int retainScriptObjectHandler(lua_State * state, int index);
	void releaseScriptObjectHandler(lua_State * state, int handler);

	bool pushScriptFunctionByHandler(lua_State * state, int handler);
	int retainScriptFunctionHandler(lua_State * state, int index);
	void releaseScriptFunctionHandler(lua_State * state, int handler);

} /* namespace lua_extend */

#endif /* LUAFUNCTION_H_ */
