require "net.PacketBuilderBase"
require "proto.TSToLSProtocol_pb"
require "proto.ProtocolCode"

LSPacketBuilder = PacketBuilderBase:new()

function LSPacketBuilder:authenReq(gateId, inGateId, platform, authencode)
	local msg = TSToLSProtocol_pb.AuthenReq()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.platform = platform
	msg.authenCode = authencode
	return self:pack(ProtocolCode.TSAndLS.AuthenReq, msg)
end

return GatePacketBuilder
