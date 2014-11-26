#ifndef MYSQL_DEFINE_H
#define MYSQL_DEFINE_H

#ifdef WIN32
#include <winsock2.h>
#include "mysql.h"
#else
#include <mysql/mysql.h>
#endif


#include <string>
using namespace std;

typedef struct StrConnectionInfo
{
	std::string  user;//
	std::string  psd;//
	std::string  dbname;//
	std::string  db_host;//
	std::string  unix_handler_name;//
	unsigned int port;//
	unsigned int client_flag;//
	std::string  charset;

	StrConnectionInfo():port(0),client_flag(0)
	{}

	inline const char* User()
	{
		return user.c_str();
	}

	inline const char* Password()
	{
		return psd.c_str();
	}

	inline const char* DBName()
	{
		return dbname.c_str();
	}

	inline const char* Host()
	{
		if ( db_host.empty() )
			return NULL;

		return db_host.c_str();
	}

	inline const char* UnixHanderName()
	{
		if ( unix_handler_name.empty() )
			return NULL;

		return unix_handler_name.c_str();
	}

	inline unsigned int Port()
	{
		return port;
	}
	
	inline unsigned int ClientFlag()
	{
		return client_flag;
	}

	inline const char* Charset()
	{
		return charset.c_str();
	}

}ConnectionInfo;

#endif
