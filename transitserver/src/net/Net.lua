module("Net", package.seeall)

Manager = {}

function Manager:init()
	local acceptor = NetModule.instance():createAcceptor()
    if not acceptor:init(7071, 0, 0) then
		return false
    end

    acceptor:setCreator(GateSession.Manager)
    NetModule.instance():addAcceptor(acceptor)
	
	
	acceptor = NetModule.instance():createAcceptor()
    if not acceptor:init(7072, 0, 0) then
		return false
    end

    acceptor:setCreator(LSSession.Manager)
    NetModule.instance():addAcceptor(acceptor)
	
	acceptor = NetModule.instance():createAcceptor()
    if not acceptor:init(7073, 0, 0) then
		return false
    end

    acceptor:setCreator(CSSession.Manager)
    NetModule.instance():addAcceptor(acceptor)
	
	return true
end

function Manager:release()

end

function Manager:sendToGate(id, msg)
	ClientSession.Manager:send(id, msg)
end
