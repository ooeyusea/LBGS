require "net.PacketBuilderBase"
require "proto.TSToCSProtocol_pb"
require "proto.ProtocolCode"

TSPacketBuilder = PacketBuilderBase:new()

function TSPacketBuilder:loadSuccAck(gateId, inGateId, guid)
	local msg = TSToCSProtocol_pb.LoadAck()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.data.isSucc = true
	msg.data.guid = guid
	return self:pack(ProtocolCode.TSAndCS.LoadAck, msg)
end

function TSPacketBuilder:loadFailAck(gateId, inGateId)
	local msg = TSToCSProtocol_pb.LoadAck()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.data.isSucc = false
	return self:pack(ProtocolCode.TSAndCS.LoadAck, msg)
end

return TSPacketBuilder
