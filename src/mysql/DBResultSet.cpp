/*
 * DBResultSet.cpp
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */

#include "DBResultSet.h"
#include <stdlib.h>
#include <string.h>

DBResultSet::DBResultSet()
	: _result(NULL)
	, _fields(NULL)
	, _row(NULL)
	, _curRowFieldLengths(NULL)
	, _fieldCount(0)
	, _rowCount(0)
{
	// TODO Auto-generated constructor stub

}

DBResultSet::~DBResultSet()
{
	// TODO Auto-generated destructor stub
	closeResult();
}

void DBResultSet::setResult(MYSQL& mysql)
{
	_result = mysql_store_result(&mysql);
	if (_result)
	{
		_fields = mysql_fetch_fields(_result);
		_fieldCount = mysql_num_fields(_result);
		_rowCount = mysql_num_rows(_result);
	}
}

void DBResultSet::closeResult()
{
	if (_result)
	{
		mysql_free_result(_result);
		_result = NULL;
	}
	_fields = NULL;
	_row = NULL;
	_curRowFieldLengths = NULL;
	_fieldCount = 0;
	_rowCount = 0;
}

bool DBResultSet::next()
{
	if (_result)
	{
		_row = mysql_fetch_row(_result);
		if (_row)
		{
			_curRowFieldLengths = mysql_fetch_lengths(_result);
			return true;
		}
	}
	return false;
}

long long DBResultSet::toLongLong(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return atoll(_row[index]);
	}
	return 0;
}

unsigned long long DBResultSet::toUnsignedLongLong(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return strtoull(_row[index], NULL, 10);
	}
	return 0;
}

int DBResultSet::toInt(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return atoi(_row[index]);
	}
	return 0;
}

unsigned int DBResultSet::toUnsignedInt(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return (unsigned int)atoll(_row[index]);
	}
	return 0;
}

long DBResultSet::toLong(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return atol(_row[index]);
	}
	return 0;
}

unsigned long DBResultSet::toUnsignedLong(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return strtoul(_row[index], NULL, 10);
	}
	return 0;
}

short DBResultSet::toShort(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return (short)atoi(_row[index]);
	}
	return 0;
}

unsigned short DBResultSet::toUnsignedShort(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return (unsigned short)atoi(_row[index]);
	}
	return 0;
}

char DBResultSet::toChar(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return (char)atoi(_row[index]);
	}
	return 0;
}

unsigned char DBResultSet::toUnsignedChar(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;
		return (unsigned char)atoi(_row[index]);
	}
	return 0;
}

float DBResultSet::toFloat(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0.0f;
		return (float)atof(_row[index]);
	}
	return 0.0f;
}

double DBResultSet::toDouble(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0.0;
		return atof(_row[index]);
	}
	return 0.0;
}

bool DBResultSet::toBool(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return false;
		return atoi(_row[index]) != 0;
	}
	return false;
}

std::string DBResultSet::toString(unsigned int index)
{
	if (index < _fieldCount && _row[index])
	{
		return std::string(_row[index]);
	}
	return "";
}

std::string DBResultSet::toBuffer(unsigned int index)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return "";

		int len = 0;
		if (_curRowFieldLengths)
			len = _curRowFieldLengths[index];

		return std::string(_row[index], len);
	}
}

bool DBResultSet::getString(unsigned int index, std::string& ret)
{
	if (index < _fieldCount && _row[index])
	{
		ret = _row[index];
		return true;
	}
	return false;
}

int DBResultSet::getData(unsigned int index, char* buffer, int maxSize)
{
	if (index < _fieldCount)
	{
		if (!_row[index]) return 0;

		int len = 0;
		if (_curRowFieldLengths)
			len = _curRowFieldLengths[index];

		if (len > maxSize)
			len = maxSize;

		memcpy(buffer, _row[index], len);
		return len;
	}
	return -1;
}
