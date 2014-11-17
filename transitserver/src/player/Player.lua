require "net.StateMachineBase"
require "net.StateBase"
require "net.ClientPacketBuilder"

Player = StateMachineBase:new()
Player.gateId = 0
Player.inGateId = 0

function Player:initWithGateId(gateId, inGateId)
	self.gateId = gateId
	self.inGateId = inGateId
	self:changeState(AuthenState:new())
end

function Player:gernateKey()
	self.key = "1231232311323"
end

function Player:send(msg)
	GateSession.Manager:sendBySeq(self.gateId, GatePacketBuilder:playerMessage(self.inGateId, msg))
end

function Player:finish()
	print(""..tostring(self.inGateId))
	GateSession.Manager:sendBySeq(self.gateId, GatePacketBuilder:kickoff(self.inGateId))
	if self.guid ~= nil then
		CSSession.Manager:send(CSPacketBuilder:checkAndNotifyOffline(self.gateId, self.inGateId, self.guid))
	end
	self.online = false;
	PlayerManager.remove(self)
end

AuthenState = StateBase:new()
function AuthenState:onEvent(player, evt)
	if evt.loginReq ~= nil then
		player.platform = evt.loginReq.platform
		player.server = evt.loginReq.server
		LSSession.Manager:randomSend(LSPacketBuilder:authenReq(player.gateId, player.inGateId, evt.loginReq.platform, evt.loginReq.authenCode))
		print("send authenReq to ls")
	elseif evt.reconnectReq ~= nil then
		local ret, data = PlayerManager.checkReconnect(evt.reconnectReq.key)
		if ret then
			player.username = data.username
			player.platform = data.platform
			player.server = data.server
			player:gernateKey()
			player:changeState(LoadDataState:new())
		else
			player:send(ClientPacketBuilder.reconnectFailAck(player))
		end
	elseif evt.loginAck ~= nil then
		if evt.loginAck.isSucc then
			player.username = evt.loginAck.username
			player:gernateKey()
			player:changeState(LoadDataState:new())
			print("player["..player.gateId..":"..player.inGateId.."] authen success")
		else
			player:send(ClientPacketBuilder:loginFailAck(player))
		end
	elseif evt.logout ~= nil or evt.kickoff ~= nil then
		player:finish()
	end
end

LoadDataState = StateBase:new()
function LoadDataState:onEnter(player)
	PlayerManager.setPlayerByUsername(player)
	
	CSSession.Manager:send(CSPacketBuilder:loadReq(player.gateId, player.inGateId, player.platform, player.server, player.username))
end

function LoadDataState:onEvent(player, evt)
	if evt.loadAck ~= nil then
		player.guid = evt.loadAck.guid
		player:changeState(OnlineState:new())
	elseif evt.logout ~= nil or evt.kickoff ~= nil then
		player:finish()
	end
end

OnlineState = StateBase:new()
function OnlineState:onEnter(player)
	PlayerManager.setPlayerByGuid(player)
	player:send(ClientPacketBuilder:loginSuccAck(player))
	player.online = true;
	print("player["..player.guid.."] online")
end

function OnlineState:onEvent(player, evt)
	if evt.logout ~= nil or evt.kickoff ~= nil then
		player:finish()
	end
end

return Player
