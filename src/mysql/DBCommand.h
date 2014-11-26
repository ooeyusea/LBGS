/*
 * DBCommand.h
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */

#ifndef DBCOMMAND_H_
#define DBCOMMAND_H_
#include "DBConnection.h"

struct IDBCommand
{
	virtual ~IDBCommand() {}

	virtual bool onExecuteSql(DBConnection& connection) = 0;
	virtual void onError(unsigned int errorCode, const char* errorInfo) = 0;
	virtual void onEffectRow(int count) = 0;
	virtual void onComplete() = 0;
	virtual void release() = 0;
};

#endif /* DBCOMMAND_H_ */
