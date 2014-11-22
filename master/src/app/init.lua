
nodes = {}

core:addNodeEventListener({ onOpen = function(node)
	if nodes[node.nodeType] == nil then
		nodes[node.nodeType] = {}
	end
	
	table.insert(nodes[node.nodeType], node)
	
	core:fork(function() 
		core:wildCall("newNode", node) 
	end)
end, onClose = function(node)

end})

core:register("getNodes", "", function()
	core:ret(nodes)
end)
