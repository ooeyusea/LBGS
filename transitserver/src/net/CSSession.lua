require "net.SessionBase"
require "net.SingleSessionManagerBase"
require "net.CSPacketBuilder"
require "net.MessageHandlerBase"
require "proto.ProtocolCode"
module("CSSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	if Manager.usedSession ~= nil then
		print("dup cs session")
	end
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
MessageHandler:register(ProtocolCode.TSAndCS.LoadAck, TSToCSProtocol_pb.LoadAck, function(session, info)
	PlayerManager.loadCallback(info.gateId, info.inGateId, info.data)
end)

MessageHandler:register(ProtocolCode.TSAndCS.KickOff, TSToCSProtocol_pb.KickOff, function(session, info)
	PlayerManager.kickOffByGuid(info.guid)
end)

