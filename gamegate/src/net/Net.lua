module("Net", package.seeall)

Manager = {}

function Manager:init()
	local acceptor = NetModule.instance():createAcceptor()
    if not acceptor:init(7070, 0, 0) then
		return false
    end

    acceptor:setCreator(ClientSession.Manager)
    NetModule.instance():addAcceptor(acceptor)
	
	local connector = NetModule.instance():createConnector()
	if not connector:init("localhost:7071", 0, 0) then
		return false
	end
	
	connector:setCreator(TransitSession.Manager)
	NetModule.instance():addConnector(connector)
	
	return true
end

function Manager:release()

end

function Manager:sendToClient(id, msg)
	ClientSession.Manager:send(id, msg)
end

function Manager:sendToTS(msg)
	TransitSession.Manager:send(msg)
end
