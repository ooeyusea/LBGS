require "net.SessionBase"
require "net.SingleSessionManagerBase"
require "net.MessageHandlerBase"
require "net.TSPacketBuilder"
require "proto.ProtocolCode"
module("TransitSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	Manager.usedSession = self
	self:send(TSPacketBuilder:reportGate(Config.id))
end

function Session:onTerminatePlus()
	Manager.usedSession = nil
	ClientSession.Manager:closeAll()
end

function Session:onRecv(buff)
    MessageHandler:handler(self, buff)
end

Manager = SingleSessionManagerBase:new()
Manager:setSession(Session)

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.GateAndTS.KickOff, GateToTSProtocol_pb.KickOff, function(session, info)
	ClientSession.Manager:close(info.id)
end)

MessageHandler:register(ProtocolCode.GateAndTS.PlayerMessage, GateToTSProtocol_pb.PlayerMessage, function(session, info)
        ClientSession.Manager:send(info.id, info.data)
end)

