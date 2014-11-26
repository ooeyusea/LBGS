/*
 * DBResultSet.h
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */

#ifndef DBRESULTSET_H_
#define DBRESULTSET_H_
#include "mysql_define.h"
#include <string>

class DBResultSet
{
public:
	DBResultSet();
	~DBResultSet();

	void setResult(MYSQL& mysql);
	void closeResult();

	bool next();
	bool empty() { return _rowCount == 0; }
	int fieldCount() { return _fieldCount; }

	long long toLongLong(unsigned int index);
	unsigned long long toUnsignedLongLong(unsigned int index);
	int toInt(unsigned int index);
	unsigned int toUnsignedInt(unsigned int index);
	long toLong(unsigned int index);
	unsigned long toUnsignedLong(unsigned int index);
	short toShort(unsigned int index);
	unsigned short toUnsignedShort(unsigned int index);
	char toChar(unsigned int index);
	unsigned char toUnsignedChar(unsigned int index);
	float toFloat(unsigned int index);
	double toDouble(unsigned int index);
	std::string toString(unsigned int index);
	std::string toBuffer(unsigned int index);
	bool toBool(unsigned int index);

	bool getString(unsigned int index, std::string& ret);
	int getData(unsigned int index, char* buffer, int maxSize);

private:
	MYSQL_RES* _result;
	MYSQL_FIELD* _fields;
	MYSQL_ROW _row;
	unsigned long* _curRowFieldLengths;
	unsigned int _fieldCount;
	unsigned int _rowCount;
};

#endif /* DBRESULTSET_H_ */
