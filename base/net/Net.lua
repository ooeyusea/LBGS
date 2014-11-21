module("Net", package.seeall)

function startConnect(uri, manager, config)
	local connector = NetModule.instance():createConnector()
	local ret = false
	if config ~= nil then
		ret = connector:init(uri, config.send, config.recv)
	else
		ret = connector:init(uri, 0, 0)
	end
	if not ret then
		NetModule.instance():destroyConnector(connector)
		return false
	end
	
	connector:setCreator(manager)
	NetModule.instance():addConnector(connector)
	return true
end

function startListen(port, manager, config)
	local acceptor = NetModule.instance():createAcceptor()
	local ret = false
	if config ~= nil then
		ret = acceptor:init(port, config.send, config.recv)
	else
		ret = acceptor:init(port, 0, 0)
	end
    if not ret then
		NetModule.instance():destroyAcceptor(acceptor)
		return false
    end

    acceptor:setCreator(manager)
    NetModule.instance():addAcceptor(acceptor)
	return true
end
