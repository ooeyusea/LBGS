/*
 * NetModule.h
 *
 *  Created on: Oct 23, 2014
 *      Author: ooeyusea
 */

#ifndef NETMODULE_H_
#define NETMODULE_H_
#include <set>
#include "util/singleton.h"

namespace net {

class Acceptor;
class Connector;
class Connection;

class NetModule : public util::Singleton<NetModule> {

public:
	NetModule();
	virtual ~NetModule();

	bool init();
	void release();
	void run();

	Acceptor * createAcceptor();
	Connector * createConnector();
	
	void destroyAcceptor(Acceptor * accpetor);
	void destroyConnector(Connector * connector);

	bool add(Acceptor * acceptor);
	void remove(Acceptor * acceptor);

	void add(Connector * connector) { _connectors.insert(connector); }
	void remove(Connector * connector) { _connectors.erase(connector); }

	bool addToEpoll(Connector * connector);
	void removeFromEpoll(Connector * connector);

	bool add(Connection * connection);
	void remove(Connection * connection);

private:
	int _epollFd;

	std::set<Acceptor*> _accepts;
	std::set<Connector*> _connectors;
	std::set<Connection*> _connections;
};

} /* namespace net */
#endif /* NETMODULE_H_ */
