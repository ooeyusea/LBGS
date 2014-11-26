/*
 * DBConnection.cc
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */
#include "DBConnection.h"
#include "DBResultSet.h"
#include <string.h>

DBConnection::DBConnection( const char* szUser, const char* szHost, const char* szDBName , const char* szPsd,  int port, unsigned int connectionflag, const char * szCharset)
{
	mysql_init( &m_inner_mysql_handler );

	m_inner_connection_info.user = szUser;
	m_inner_connection_info.psd  = szPsd;
	m_inner_connection_info.dbname = szDBName;
	m_inner_connection_info.port   = port;
	m_inner_connection_info.client_flag = connectionflag;
	m_inner_connection_info.db_host = szHost;
	m_inner_connection_info.charset = szCharset;
}

DBConnection::DBConnection()
{
	mysql_init( &m_inner_mysql_handler );
}

DBConnection::~DBConnection()
{
	mysql_close( &m_inner_mysql_handler );
}

bool DBConnection::open(const char* szUser, const char* szHost, const char* szDBName , const char* szPsd,  int port, unsigned int connectionflag, const char * szCharset)
{
	m_inner_connection_info.user = szUser;
	m_inner_connection_info.psd  = szPsd;
	m_inner_connection_info.dbname = szDBName;
	m_inner_connection_info.port   = port;
	m_inner_connection_info.client_flag = connectionflag;
	m_inner_connection_info.db_host = szHost;
	m_inner_connection_info.charset = szCharset;
	return open();
}

bool DBConnection::open()
{
	if(  mysql_real_connect( &m_inner_mysql_handler,
		m_inner_connection_info.Host(),
		m_inner_connection_info.User(),
		m_inner_connection_info.Password(),
		m_inner_connection_info.DBName(),
		m_inner_connection_info.Port(),
		m_inner_connection_info.UnixHanderName(),
		m_inner_connection_info.ClientFlag() ) == NULL )
	{
		printf("Failed to connect to database: Error: %s\n",
			mysql_error(&m_inner_mysql_handler));

		return false;
	}

	int value = 1;
	mysql_options(&m_inner_mysql_handler, MYSQL_OPT_RECONNECT, (char *)&value);

	if (!m_inner_connection_info.charset.empty())
	{
		if (mysql_set_character_set(&m_inner_mysql_handler, m_inner_connection_info.Charset()))
		{
			printf("Failed to set names to %s: Error: %s\n",
				m_inner_connection_info.charset.c_str(), mysql_error(&m_inner_mysql_handler));

			return false;
		}
		printf("set names %s\n", m_inner_connection_info.charset.c_str());
	}

	return true;
}

bool DBConnection::reopen()
{
	mysql_close( &m_inner_mysql_handler );
	mysql_init( &m_inner_mysql_handler );

	return open();
}

bool DBConnection::execute(const char* sql)
{
	if (!isActive())
		return false;

	if (mysql_real_query(&m_inner_mysql_handler, sql, strlen(sql)))
		return false;

	return true;
}

bool DBConnection::execute(const std::string& sql)
{
	if (!isActive())
		return false;

	if (mysql_real_query(&m_inner_mysql_handler, sql.c_str(), sql.size()))
		return false;

	return true;
}

bool DBConnection::execute(const char* sql, DBResultSet& rs)
{
	if (!isActive())
		return false;

	if (mysql_real_query(&m_inner_mysql_handler, sql, strlen(sql)))
		return false;

	rs.setResult(m_inner_mysql_handler);
	return true;
}

bool DBConnection::execute(const std::string& sql, DBResultSet& rs)
{
	if (!isActive())
		return false;

	if (mysql_real_query(&m_inner_mysql_handler, sql.c_str(), sql.size()))
		return false;

	rs.setResult(m_inner_mysql_handler);
	return true;
}

bool DBConnection::isActive()
{
	return mysql_ping( &m_inner_mysql_handler ) == 0;
}

unsigned int DBConnection::GetLastErrNo()
{
	return mysql_errno(&m_inner_mysql_handler);
}

const char* DBConnection::GetLastErrInfo()
{
	return mysql_error(&m_inner_mysql_handler);
}

const char* DBConnection::EscapeStringEx(const char* pszSrc, int nSize, char* pszDest )
{
	if ( isActive() ){
		 mysql_real_escape_string(&m_inner_mysql_handler, pszDest, pszSrc, nSize);
		 return pszDest;
	}
	return NULL;
}

unsigned int DBConnection::GetAffectedRow()
{
	return (unsigned int)mysql_affected_rows(&m_inner_mysql_handler);
}

