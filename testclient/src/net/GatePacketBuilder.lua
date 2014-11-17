require "net.PacketBuilderBase"
require "proto.ClietToServerProtocol_pb"
require "proto.ProtocolCode"

GatePacketBuilder = PacketBuilderBase:new()

function GatePacketBuilder:loginReq(platform, server, authenCode)
	local msg = ClietToServerProtocol_pb.LoginReq()
	msg.platform = platform
	msg.server = server
	msg.authenCode = authenCode
	return self:pack(ProtocolCode.ClientAndServer.LoginReq, msg)
end

function GatePacketBuilder:logout()
	local msg = ClietToServerProtocol_pb.Logout()
	msg.reserve = 0
	return self:pack(ProtocolCode.ClientAndServer.Logout, msg)
end

return GatePacketBuilder
