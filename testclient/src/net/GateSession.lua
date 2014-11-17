require "net.SessionBase"
require "net.SingleSessionManagerBase"
require "net.MessageHandlerBase"
require "net.GatePacketBuilder"
require "proto.ProtocolCode"
module("GateSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	Manager.usedSession = self
	self:send(GatePacketBuilder:loginReq("test", "s1", "test"))
end

function Session:onTerminatePlus()
	Manager.usedSession = nil
end

function Session:onRecv(buff)
    MessageHandler:handler(self, buff)
end

Manager = SingleSessionManagerBase:new()
Manager:setSession(Session)

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.ClientAndServer.LoginAck, ClietToServerProtocol_pb.LoginAck, function(session, info)
	print("login result:"..tostring(info.isSucc))
end)
