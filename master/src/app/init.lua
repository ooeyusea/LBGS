
nodes = {}

core:addNodeEventListener({ onOpen = function(self, node)
	if nodes[node.nodeType] == nil then
		nodes[node.nodeType] = {}
	end
	
	table.insert(nodes[node.nodeType], node)
	
	core:fork(function() 
		core:wildCall("newNode", node) 
	end)
end, onClose = function(self, node)

end})

core:register("getNodes", "", function()
	core:ret(nodes)
end)
