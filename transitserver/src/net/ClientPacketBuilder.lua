require "net.PacketBuilderBase"
require "proto.ClietToServerProtocol_pb"
require "proto.ProtocolCode"

ClientPacketBuilder = PacketBuilderBase:new()

function ClientPacketBuilder:loginSuccAck(player)
	local msg = ClietToServerProtocol_pb.LoginAck()
	msg.isSucc = true
	msg.data.username = player.username
	msg.data.key = player.key
	msg.data.guid = player.guid
	return self:pack(ProtocolCode.ClientAndServer.LoginAck, msg)
end

function ClientPacketBuilder:loginFailAck(id, data)
	local msg = ClietToServerProtocol_pb.LoginAck()
	msg.isSucc = false
	return self:pack(ProtocolCode.ClientAndServer.LoginAck, msg)
end

return ClientPacketBuilder
