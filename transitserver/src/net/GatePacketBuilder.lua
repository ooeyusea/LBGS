require "net.PacketBuilderBase"
require "proto.GateToTSProtocol_pb"
require "proto.ProtocolCode"

GatePacketBuilder = PacketBuilderBase:new()

function GatePacketBuilder:kickoff(id)
	local msg = GateToTSProtocol_pb.KickOff()
	msg.id = id
	return self:pack(ProtocolCode.GateAndTS.KickOff, msg)
end

function GatePacketBuilder:playerMessage(id, data)
	local msg = GateToTSProtocol_pb.PlayerMessage()
	msg.id = id
	msg.data = data
	return self:pack(ProtocolCode.GateAndTS.PlayerMessage, msg)
end

return GatePacketBuilder
