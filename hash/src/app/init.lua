require "net.Cluster"
require "app.HashManager"

cluster = Cluster:new()
hashManager = HashManager:new()

core:register("getHash", "", function(guid)
	hashManager:hash(guid)
end)

core:addNodeEventListener({
	onOpen = function(node)
		if node.nodeType == "data" then
			hashManager:onDataOpen(node)
		end
	end, onClose = function(node)

	end
})

