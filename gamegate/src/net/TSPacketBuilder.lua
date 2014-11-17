require "net.PacketBuilderBase"
require "proto.GateToTSProtocol_pb"
require "proto.ProtocolCode"

TSPacketBuilder = PacketBuilderBase:new()

function TSPacketBuilder:reportGate(id)
	local msg = GateToTSProtocol_pb.ReportGate()
	msg.id = id
	return self:pack(ProtocolCode.GateAndTS.ReportGate, msg)
end

function TSPacketBuilder:createPlayer(id)
	local msg = GateToTSProtocol_pb.CreatePlayer()
	msg.id = id
	return self:pack(ProtocolCode.GateAndTS.CreatePlayer, msg)
end

function TSPacketBuilder:destroyPlayer(id)
	local msg = GateToTSProtocol_pb.DestoryPlayer()
	msg.id = id
	return self:pack(ProtocolCode.GateAndTS.DestroyPlayer, msg)
end

function TSPacketBuilder:playerMessage(id, data)
	local msg = GateToTSProtocol_pb.PlayerMessage()
	msg.id = id
	msg.data = data
	return self:pack(ProtocolCode.GateAndTS.PlayerMessage, msg)
end

return TSPacketBuilder
