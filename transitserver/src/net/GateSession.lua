require "net.SessionBase"
require "net.MultiSessionManagerBase"
require "net.GatePacketBuilder"
require "net.MessageHandlerBase"
require "proto.ProtocolCode"
module("GateSession", package.seeall)

Session = SessionBase:new()

function Session:onEstablishPlus()
	Manager.sessions[self.id] = self
end

function Session:onTerminatePlus()
	Manager.sessions[self.id] = nil
	if self.seq ~= nil then
		Manager.sessionsBySeq[self.seq] = nil
	end
end

function Session:onRecv(buff)
    MessageHandler:handler(self, buff)
end

Manager = MultiSessionManagerBase:new()
Manager.sessionsBySeq = {}
Manager:setSession(Session)

function Manager:sendBySeq(seq, msg)
	local session = self.sessionsBySeq[seq]
	if session ~= nil then
		session:send(msg)
		return true
	end
	return false
end

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.GateAndTS.ReportGate, GateToTSProtocol_pb.ReportGate, function(session, info)
	session.seq = info.id
	Manager.sessionsBySeq[session.seq] = session
	print("session seq:"..info.id)
end)

MessageHandler:register(ProtocolCode.GateAndTS.CreatePlayer, GateToTSProtocol_pb.CreatePlayer, function(session, info)
	PlayerManager.createPlayer(session.seq, info.id)
	print("player create:"..session.seq.."["..info.id.."]")
end)

MessageHandler:register(ProtocolCode.GateAndTS.DestroyPlayer, GateToTSProtocol_pb.DestoryPlayer, function(session, info)
	PlayerManager.logout(session.seq, info.id)
end)

MessageHandler:register(ProtocolCode.GateAndTS.PlayerMessage, GateToTSProtocol_pb.PlayerMessage, function(session, info)
	PlayerManager.onRecv(session.seq, info.id, info.data)
end)
