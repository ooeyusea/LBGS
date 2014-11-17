module("Net", package.seeall)

Manager = {}

function Manager:init()	
	local connector = NetModule.instance():createConnector()
	if not connector:init("localhost:7070", 0, 0) then
		return false
	end
	
	connector:setCreator(GateSession.Manager)
	NetModule.instance():addConnector(connector)
	
	return true
end

function Manager:release()

end
