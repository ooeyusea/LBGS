#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include "mysql_define.h"

class DBResultSet;
class DBConnection
{
public:
	DBConnection(const char* szUser, const char* szHost, const char* szDBName , const char* szPsd,  int port, unsigned int connectionflag, const char * szCharset);
	DBConnection();
	~DBConnection();

	bool open(const char* szUser, const char* szHost, const char* szDBName , const char* szPsd,  int port, unsigned int connectionflag, const char * szCharset);
	bool open();

	bool reopen();

	bool execute(const char* sql);
	bool execute(const std::string& sql);

	bool execute(const char* sql, DBResultSet& rs);
	bool execute(const std::string& sql, DBResultSet& rs);

	bool isActive();

	unsigned int GetLastErrNo();

	const char* GetLastErrInfo();

	const char*  EscapeStringEx(const char* pszSrc, int nSize, char* pszDest );

	unsigned int GetAffectedRow();

private:
	DBConnection( const DBConnection& );
	DBConnection& operator=( const DBConnection& );

private:
	MYSQL m_inner_mysql_handler;
	ConnectionInfo m_inner_connection_info;
};

#endif //DBCONNECTION_H
