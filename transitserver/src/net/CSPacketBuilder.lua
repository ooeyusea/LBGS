require "net.PacketBuilderBase"
require "proto.TSToCSProtocol_pb"
require "proto.ProtocolCode"

CSPacketBuilder = PacketBuilderBase:new()

function CSPacketBuilder:loadReq(gateId, inGateId, platform, server, username)
	local msg = TSToCSProtocol_pb.LoadReq()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.platform = platform
	msg.server = server
	msg.username = username
	return self:pack(ProtocolCode.TSAndCS.LoadReq, msg)
end

function CSPacketBuilder:checkAndNotifyOffline(guid)
	local msg = TSToCSProtocol_pb.CheckAndNotifyOffline()
	msg.gateId = gateId
	msg.inGateId = inGateId
	msg.guid = guid
	return self:pack(ProtocolCode.TSAndCS.NotifyOffline, msg)
end

return GatePacketBuilder
