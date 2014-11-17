require "net.PacketBuilderBase"
require "proto.TSToLSProtocol_pb"
require "proto.ProtocolCode"

TSPacketBuilder = PacketBuilderBase:new()

function TSPacketBuilder:authenSuccAck(gateId, inGateId, username)
	local msg = TSToLSProtocol_pb.AuthenAck()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.data.isSucc = true
	msg.data.username = username
	return self:pack(ProtocolCode.TSAndLS.AuthenAck, msg)
end

function TSPacketBuilder:authenFailAck(gateId, inGateId)
	local msg = TSToLSProtocol_pb.AuthenAck()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.data.isSucc = false
	return self:pack(ProtocolCode.TSAndLS.AuthenAck, msg)
end

return TSPacketBuilder
