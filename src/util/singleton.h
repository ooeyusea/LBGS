/*
 * singleton.h
 *
 *  Created on: 2012-2-23
 *      Author: liuqian
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

namespace util
{
	template <typename T>
	struct Singleton
	{
		static T* Instance()
		{
			static T instance;
			return &instance;
		}
	};
}

#endif /* SINGLETON_H_ */
