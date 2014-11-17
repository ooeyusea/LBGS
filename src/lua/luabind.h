/*
 * luabind.h
 *
 *  Created on: Nov 5, 2014
 *      Author: ooeyusea
 */

#ifndef LUABIND_H_
#define LUABIND_H_
#include <luajit/lua.hpp>

namespace lua_extend {

	void registerNetToLua(lua_State * state);

} /* namespace lua_extend */
#endif /* LUABIND_H_ */
