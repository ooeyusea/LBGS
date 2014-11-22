require "net.Cluster"
require "app.player.PlayerManager"

cluster = Cluster:new("center")

core:register("createPlayer", "", function(node, id)
	PlayerManager.createPlayer(node, id)
end)

core:register("destroyPlayer", "", function(node, id)
	PlayerManager.destoryPlayer(node, id)
end)

core:register("doPlayerMessage", "", function(node, id, msg)
	PlayerManager.dispatchMessage(node, id, msg)
end)

core:addNodeEventListener({
	onOpen = function(node)
	
	end, onClose = function(node)
		if node.nodeType == "gate" then
			PlayerManager.gateBroken(node.nodeId)
		end
	end
})
