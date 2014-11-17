/*
 * NetModule.cpp
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#include "NetModule.h"
#include "net_header.h"
#include "EpollHanlder.h"
#include "Connection.h"
#include "Connector.h"
#include "Acceptor.h"
#include <stdio.h>

#define MAX_EPOLL_SIZE 2000

namespace net {

NetModule::NetModule() {
	// TODO Auto-generated constructor stub

}

NetModule::~NetModule() {
	// TODO Auto-generated destructor stub
}

bool NetModule::init()
{
	_epollFd = epoll_create(MAX_EPOLL_SIZE);
	if (_epollFd < 0)
	{
		printf("create epoll failed.\n");
		return false;
	}

	return true;
}

void NetModule::release()
{

}

void NetModule::run()
{
	{
		epoll_event evts[MAX_EPOLL_SIZE];

		int wait = (int)_accepts.size() + (int)_connections.size() + (int)_connectors.size();
		int count = epoll_wait(_epollFd, evts, wait, 50);
		if(count < 0)
		{
			if(errno != EINTR)
			{
				printf("epoll wait error:%d.\n", errno);
			}
		}
		else
		{
			for (int i = 0; i < count; ++i)
			{
				EpollHandler* handler = static_cast<EpollHandler*>(evts[i].data.ptr);
				if (!handler)
					continue;

				if(evts[i].events & EPOLLERR)
				{
					handler->onError();
					continue;
				}

				if (evts[i].events & EPOLLHUP)
				{
					handler->onError();
					continue;
				}

				if(evts[i].events & EPOLLOUT)
				{
					handler->onOut();
				}

				if(evts[i].events & EPOLLIN)
				{
					handler->onIn();
				}
			}
		}
	}

	{
		std::set<Connector*>::iterator itr = _connectors.begin();
		for (; itr != _connectors.end(); ++itr)
		{
			if ((*itr)->isUnconnected())
			{
				(*itr)->reconnect();
			}
		}
	}

	{
		std::set<Connection*> tmp = _connections;
		std::set<Connection*>::iterator itr = tmp.begin();
		for (; itr != tmp.end(); ++itr)
		{
			(*itr)->run();
		}
	}
}

Acceptor * NetModule::createAcceptor()
{
	return new Acceptor(this);
}

Connector * NetModule::createConnector()
{
	return new Connector(this);
}

bool NetModule::add(Acceptor * acceptor)
{
	epoll_event evt;
	evt.events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;
	evt.data.ptr = acceptor;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, acceptor->_fd, &evt) < 0)
	{
		printf("epoll add acceptor failed.\n");
		acceptor->onError();
		return false;
	}

	_accepts.insert(acceptor);
	return true;
}

void NetModule::remove(Acceptor * acceptor)
{
	epoll_event evt;
	if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, acceptor->_fd, &evt) < 0)
	{
		printf("epoll remove acceptor failed.\n");
		acceptor->onError();
		return;
	}
	_accepts.erase(acceptor);
}

bool NetModule::addToEpoll(Connector * connector)
{
	epoll_event evt;
	evt.events = EPOLLOUT | EPOLLET | EPOLLERR | EPOLLHUP;
	evt.data.ptr = connector;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, connector->_fd, &evt) < 0)
	{
		printf("epoll add connector failed.\n");
		connector->onError();
		return false;
	}

	return true;
}

void NetModule::removeFromEpoll(Connector * connector)
{
	epoll_event evt;
	if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, connector->_fd, &evt) < 0)
	{
		printf("epoll remove connector failed.\n");
		connector->onError();
	}
}

bool NetModule::add(Connection * connection)
{
	epoll_event evt;
	evt.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLERR | EPOLLHUP;
	evt.data.ptr = connection;

	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, connection->_fd, &evt) < 0)
	{
		printf("epoll add connection failed.\n");
		connection->onError();
		return false;
	}

	_connections.insert(connection);
	return true;
}

void NetModule::remove(Connection * connection)
{
	epoll_event evt;
	if(epoll_ctl(_epollFd, EPOLL_CTL_DEL, connection->_fd, &evt) < 0)
	{
		printf("epoll remove connection failed.\n");
		connection->onError();
		return;
	}

	_connections.erase(connection);
}

} /* namespace net */
