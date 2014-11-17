require "net.SessionBase"
require "net.SingleSessionManagerBase"
require "net.MessageHandlerBase"
require "net.TSPacketBuilder"
require "proto.ProtocolCode"
module("TransitSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	Manager.usedSession = self
end

function Session:onTerminatePlus()
	Manager.usedSession = nil
end

function Session:onRecv(buff)
    MessageHandler:handler(self, buff)
    print("recv")
end

Manager = SingleSessionManagerBase:new()
Manager:setSession(Session)

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.TSAndLS.AuthenReq, TSToLSProtocol_pb.AuthenReq, function(session, info)
	print("authen:"..info.gateId..","..info.inGateId..","..info.platform..","..info.authenCode)
	session:send(TSPacketBuilder:authenSuccAck(info.gateId, info.inGateId, "hahaha"))
end)
