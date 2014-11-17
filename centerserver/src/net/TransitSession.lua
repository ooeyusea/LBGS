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
end

Manager = SingleSessionManagerBase:new()
Manager:setSession(Session)

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.TSAndCS.LoadReq, TSToCSProtocol_pb.LoadReq, function(session, info)
	print("load:"..info.gateId..","..info.inGateId..","..info.platform..","..info.server..","..info.username)
	session:send(TSPacketBuilder:loadSuccAck(info.gateId, info.inGateId, "guid:12312312321312323"))
end)
