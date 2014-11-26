require "proto.ClietToServerProtocol_pb"
module("PlayerManager", package.seeall)

players = {}
playersForUsername = {}
playersForGuid = {}
reconnectInfos = {}

function createPlayer(node, id)
	local player = Player:new(node, id)
	
	if players[node.nodeId] == nil then
		players[node.nodeId] = {}
	end
	players[node.nodeId][id] = player
end

function destroyPlayer(node, id)
	if players[node.nodeId] ~= nil and players[node.nodeId][id] ~= nil then
		local player = players[node.nodeId][id]
		player:logout()
		remove(player, false)
	end
end

function sendToPlayer(guid, msg)
	if playersForGuid[guid] ~= nil then
		playersForGuid[guid]:send(msg)
	end
end

function kickoff(node, id)
	if players[node.nodeId] ~= nil and players[node.nodeId][id] ~= nil then
		remove(players[node.nodeId][id], true)
	end	
end

function kickoffByUsername(platform, server, username)
	if playersForUsername[platform] ~= nil and playersForUsername[platform][server] ~= nil and playersForUsername[platform][server][username] ~= nil then
		remove(playersForUsername[platform][server][username], true)
	end	
end

function kickOffByGuid(guid)
	if playersForGuid[guid] ~= nil then
		remove(playersForGuid[guid], true)
	end
end

function dispatchMessage(node, id, msg)
	if players[node.nodeId] ~= nil and players[node.nodeId][id] ~= nil then
		local player = players[node.nodeId][id]
		local next,cmd,len1 = string.unpack(msg,"HH")
		if cmd == ProtocolCode.LoginReq then
			local next,msgBody = string.unpack(msg, "A"..(len1 -4), next)
			local info = ClietToServerProtocol_pb.LoginReq()
			if pcall(info.ParseFromString, info, msgBody) then
				player:login(info)
			end
		elseif cmd == ProtocolCode.Logout then
			local next,msgBody = string.unpack(msg, "A"..(len1 -4), next)
			local info = ClietToServerProtocol_pb.Logout()
			if pcall(info.ParseFromString, info, msgBody) then
				destroyPlayer(node, id)
			end
		else
			player:dispatch(cmd, msg)
		end
	end
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

function remove(player, notify)
	if player.username ~= nil then
		playersForUsername[player.platform][player.server][player.username] = nil
	end
	if player.guid ~= nil then
		playersForGuid[player.guid] = nil
	end
	players[player.node.nodeId][player.id] = nil
	player.dead = true
	if notify then
		core:call(player.node, "kickoff", false, player.id)
	end
end

function gateBroken(nodeId)
	if players[nodeId] ~= nil then
		for k, player in pairs(players[nodeId]) do
			if player.username ~= nil then
				playersForUsername[player.platform][player.server][player.username] = nil
			end
			if player.guid ~= nil then
				playersForGuid[player.guid] = nil
			end
			player.dead = true
		end
		players[nodeId] = nil
	end
end
