require "net.SessionBase"
require "net.MultiSessionManagerBase"
require "net.LSPacketBuilder"
require "net.MessageHandlerBase"
require "proto.ProtocolCode"
module("LSSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	Manager.sessions[self.id] = self
end

function Session:onTerminatePlus()
	Manager.sessions[self.id] = nil
end

function Session:onRecv(buff)
    MessageHandler:handler(self, buff)
end

Manager = MultiSessionManagerBase:new()
Manager:setSession(Session)

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.TSAndLS.AuthenAck, TSToLSProtocol_pb.AuthenAck, function(session, info)
	PlayerManager.authenCallback(info.gateId, info.inGateId, info.data)
end)

