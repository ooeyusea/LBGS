#include "lseri.h"
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 2048

class SerializeStream
{
public:
	SerializeStream(unsigned char* buffer, size_t len) : _buffer(buffer), _len(len), _offset(0) {}
	SerializeStream() : _len(BUFF_SIZE), _offset(0) { _buffer = (unsigned char*)malloc(BUFF_SIZE); }
	
	void release() { if (_buffer) free(_buffer); }
	
	void * getBuffer() { return (void *)_buffer; }
	size_t len() { return _offset; }
	
	bool read(unsigned char* dst, size_t len)
	{
		if (_offset + len > _len)
			return false;
		memcpy(dst, _buffer + _offset, len);
		_offset += len;
        return true;
	}
	
	template <typename T>
	bool read(T& t)
	{
		return read((unsigned char*)&t, sizeof(T));
	}
	
	bool readString(const char *& str, size_t& sz)
	{
		int len = 0;
		if (!read(len))
			return false;
		
		sz = len;
		str = (const char*)(_buffer + _offset);
		_offset += len;
        return true;
	}
	
	bool write(unsigned char * src, size_t len)
	{
		if (_offset + len > _len)
			return false;

		memcpy(_buffer + _offset, src, len);
		_offset += len;
        return true;
	}
	
	template <typename T>
	bool write(T t)
	{
		return write((unsigned char*)&t, sizeof(T)); 
	}
	
	bool writeString(const char * src, size_t sz)
	{
		int len = (int)sz;
		if (!write(len))
			return false;
			
		memcpy(_buffer + _offset, src, len);
		_offset += len;
        return true;
	}
	
private:
	unsigned char * _buffer;
	size_t _len;
	size_t _offset;
};

namespace lseri {
	const unsigned char NIL = 0;
	const unsigned char BOOLEAN = 1;
	const unsigned char INT = 2;
	const unsigned char DOUBLE = 3;
	const unsigned char STRING = 4;
	const unsigned char TABLE = 5;
}

using namespace lseri;

void pack_one(lua_State * L, SerializeStream& s, int index);

void pack_nil(lua_State * L, SerializeStream& s)
{
	if (!s.write(NIL))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
}

void pack_integer(lua_State * L, SerializeStream& s, int n)
{
	if (!s.write(INT))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
	if (!s.write(n))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
}

void pack_double(lua_State * L, SerializeStream& s, double n)
{
	if (!s.write(DOUBLE))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
	if (!s.write(n))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
}

void pack_number(lua_State * L, SerializeStream& s, int index)
{
	int x = (int)lua_tointeger(L, index);
	double n = (double)lua_tonumber(L, index);
	if ((double)x == n)
		pack_integer(L, s, x);
	else
		pack_double(L, s, n);
}

void pack_boolean(lua_State * L, SerializeStream& s, int index)
{
	bool b = lua_toboolean(L, index);
	if (!s.write(BOOLEAN))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
	if (!s.write(b))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
}

void pack_string(lua_State * L, SerializeStream& s, int index)
{
	size_t sz = 0;
	const char *str = lua_tolstring(L, index, &sz);
	if (!s.write(STRING))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
	if (!s.writeString(str, sz))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}	
}

void pack_table(lua_State * L, SerializeStream& s, int index)
{
	if (!s.write(TABLE))
	{
		s.release();
		luaL_error(L, "serialize buffer full");
	}
	lua_pushnil(L);
	while (lua_next(L, index) != 0) {
		pack_one(L, s, -2);
		pack_one(L, s, -1);
		lua_pop(L, 1);
	}
	pack_nil(L, s);
}

void pack_one(lua_State * L, SerializeStream& s, int index)
{
	int type = lua_type(L, index);
	switch (type) 
	{
		case LUA_TNIL: pack_nil(L, s); break;
		case LUA_TNUMBER: pack_number(L, s, index); break;
		case LUA_TBOOLEAN: pack_boolean(L, s, index); break;
		case LUA_TSTRING: pack_string(L, s, index); break;
		case LUA_TTABLE: {
			if (index < 0) {
				index = lua_gettop(L) + index + 1;
			}
			pack_table(L, s, index);
			break;
		}
		default: {
			s.release();
			luaL_error(L, "Unsupport type %s to serialize", lua_typename(L, type));
		}
	}
}

static int l_serialize(lua_State * L)
{
	SerializeStream s;
	
	int n = lua_gettop(L);
	for (int i = 1; i <= n; ++i) {
		pack_one(L, s, i);
	}
	
	lua_pushlstring(L, (const char *)s.getBuffer(), s.len());
	s.release();
	return 1;
}

void push_one(lua_State * L, SerializeStream& s);

void push_table(lua_State * L, SerializeStream& s)
{
	lua_newtable(L);
	while (true) 
	{
		push_one(L, s);
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			return;
		}
		push_one(L, s);
		lua_rawset(L, -3);
	}
}

void push_value(lua_State * L, SerializeStream& s, int type)
{
	switch (type)
	{
		case NIL: lua_pushnil(L); break;
		case BOOLEAN: {
			bool value;
			if (!s.read(value))
				luaL_error(L, "unserialize failed");
			lua_pushboolean(L, value); 
			break;
		}
		case INT: {
			int value;
			if (!s.read(value))
				luaL_error(L, "unserialize failed");
			lua_pushnumber(L, value); 
			break;
		}
		case DOUBLE: {
			double value;
			if (!s.read(value))
				luaL_error(L, "unserialize failed");
			lua_pushnumber(L, value); 
			break;
		}
		case STRING: {
			size_t len = 0; 
			const char * str = NULL;
			if (!s.readString(str, len)) 
				luaL_error(L, "unserialize failed");
			lua_pushlstring(L, str, len); 
			break;
		}
		case TABLE: push_table(L, s); break;
		default: 
			luaL_error(L, "Unsupport type %d to unserialize", type);
	}
}

void push_one(lua_State * L, SerializeStream& s)
{
	unsigned char type;
	if (!s.read(type))
		luaL_error(L, "unserialize failed");
	push_value(L, s, type);
}

static int l_unserialize(lua_State * L)
{
	if (lua_isnoneornil(L,1)) {
		return 0;
	}
	
	size_t len;
	unsigned char* buffer = (unsigned char*)lua_tolstring(L, 1, &len);
	
	if (len == 0) {
		return 0;
	}
	
	if (!buffer) {
		return luaL_error(L, "unserialize null pointer");
	}
	
	SerializeStream s(buffer, len); 

	lua_settop(L, 0);
	
	for (int i=0;; ++i) {
		if (i%16==15) {
			lua_checkstack(L, i);
		}
		
		unsigned char type;
		if (!s.read(type))
			break;
		push_value(L, s, type);
	}

	return lua_gettop(L);
}

static const luaL_reg R[] =
{
	{"serialize",	l_serialize},
	{"unserialize",	l_unserialize},
	{NULL,	NULL}
};

int luaopen_seri(lua_State *L)
{
	luaL_openlib(L, LUA_STRLIBNAME, R, 0);
	return 0;
}
