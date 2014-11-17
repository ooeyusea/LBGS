require "net.SessionBase"
require "net.MultiSessionManagerBase"
require "net.TSPacketBuilder"
module("ClientSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	print("give id:"..self.id)
	Manager.sessions[self.id] = self
	TransitSession.Manager:send(TSPacketBuilder:createPlayer(self.id))
end

function Session:onTerminatePlus()
	Manager.sessions[self.id] = nil
	TransitSession.Manager:send(TSPacketBuilder:destroyPlayer(self.id))
end

function Session:onRecv(buff)
    TransitSession.Manager:send(TSPacketBuilder:playerMessage(self.id, buff))
end

Manager = MultiSessionManagerBase:new()
Manager:setSession(Session)
