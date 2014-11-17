/*
 * EpollHanlder.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#ifndef EPOLLHANLDER_H_
#define EPOLLHANLDER_H_

namespace net {

struct EpollHandler
{
	virtual ~EpollHandler() {}
	virtual void onIn() {}
	virtual void onOut() {}
	virtual void onError() {}
};

}

#endif /* EPOLLHANLDER_H_ */
