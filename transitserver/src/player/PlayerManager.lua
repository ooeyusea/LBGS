require "proto.ClietToServerProtocol_pb"
module("PlayerManager", package.seeall)

players = {}
playersForUsername = {}
playersForGuid = {}
reconnectInfos = {}

function createPlayer(gateId, inGateId)
	local player = Player:new()
	player:initWithGateId(gateId, inGateId)
	
	print(""..player.gateId..","..player.inGateId)
	
	if players[gateId] == nil then
		players[gateId] = {}
	end
	players[gateId][inGateId] = player
end

function setPlayerByUsername(player)
	if playersForUsername[player.platform] == nil then
		playersForUsername[player.platform] = {}
	end
	if playersForUsername[player.platform][player.server] == nil then
		playersForUsername[player.platform][player.server] = {}
	end
	playersForUsername[player.platform][player.server][player.username] = player
end

function setPlayerByGuid(player)
	playersForGuid[player.guid] = player
end

function authen(gateId, inGateId, loginReq)
	if players[gateId] ~= nil and players[gateId][inGateId] ~= nil then
		local player = players[gateId][inGateId]
		player:doEvent({ loginReq = loginReq })
	end
end

function authenCallback(gateId, inGateId, loginAck)
	if players[gateId] ~= nil and players[gateId][inGateId] ~= nil then
		local player = players[gateId][inGateId]
		player:doEvent({ loginAck = loginAck })
	end
end

function reconnect(gateId, inGateId, reconnectReq)
	if players[gateId] ~= nil and players[gateId][inGateId] ~= nil then
		local player = players[gateId][inGateId]
		player:doEvent({ reconnectReq = reconnectReq })
	end
end

function loadCallback(gateId, inGateId, loadAck)
	if players[gateId] ~= nil and players[gateId][inGateId] ~= nil then
		local player = players[gateId][inGateId]
		player:doEvent({ loadAck = loadAck })
	else
		CSSession.Manager:send(CSPacketBuilder:checkAndNotifyOffline(gateId, inGateId, loadAck.data.guid))
	end
end

function onRecv(gateId, inGateId, data)
	if players[gateId] ~= nil and players[gateId][inGateId] ~= nil then
		local player = players[gateId][inGateId]
		MessageHandler:handler(player, data)
	end
end

function logout(gateId, inGateId)
	if players[gateId] ~= nil and players[gateId][inGateId] ~= nil then
		local player = players[gateId][inGateId]
		player:doEvent({ logout = true })
	end
end

function kickOffByUsername(platform, server, username)
	if playersForUsername[platform] ~= nil and playersForUsername[platform][server] ~= nil and playersForUsername[platform][server][username] ~= nil then
		local player = playersForUsername[platform][server][username]
		player:doEvent({ kickoff = true })
	end
end

function kickOffByGuid(guid)
	if playersForGuid[guid] ~= nil then
		local player = playersForGuid[guid]
		player:doEvent({ kickoff = true })
	end
end

function addReconnectInfo(player)
	reconnectInfos[player.key] = {
		username = player.username,
		server = player.server,
		platform = player.platform,
	}
	
	local key = player.key
	TimerQueue.instance():create(function()
		reconnectInfos[key] = nil
	end, 600000)
end

function checkReconnect(key)
	if reconnectInfos[key] ~= nil then
		return true, reconnectInfos[key]
	end
	return false, nil
end

function remove(player)
	if player.username ~= nil then
		playersForUsername[player.platform][player.server][player.username] = nil
	end
	if player.guid ~= nil then
		playersForGuid[player.guid] = nil
	end
	players[player.gateId][player.inGateId] = nil
end

MessageHandler = MessageHandlerBase:new()
MessageHandler:register(ProtocolCode.ClientAndServer.LoginReq, ClietToServerProtocol_pb.LoginReq, function(player, info)
	player:doEvent({ loginReq = info })
end)
