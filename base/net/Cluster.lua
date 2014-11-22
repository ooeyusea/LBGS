Cluster = {}

function Cluster:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function Cluster:ctor(...)
	self.nodes = {}
	self.node_wather = {}
	
	for i, v in ipairs{...} do
		self.node_wather[v] = true
	end
	
	core:addNodeEventListener({ onOpen = function(node)
		if node.nodeType == "master" then
			core:fork(function() 
				local allNodes = core:call(node, "getNodes", true)
				for k, v in pairs(allNodes) do
					if self.node_wather[k] then
						for j, u in ipairs(v) do
							if self.nodes[k] == nil then
								self.nodes[k] = {}
							end
							
							if not self.nodes[k][u.nodeId] then
								self.nodes[k][u.nodeId] = true
								core:openNode(u.uri)
							end
						end
					end
				end
			end)
		end
	end, onClose = function(node)

	end})

	core:register("newNode", "", function(node)
		if self.node_wather[node.nodeType] then
			if self.nodes[node.nodeType] == nil then
				self.nodes[node.nodeType] = {}
			end
			
			if not self.nodes[node.nodeType][node.nodeId] then
				self.nodes[node.nodeType][node.nodeId] = true
				core:openNode(node.uri)
			end
		end
	end)
end

return Cluster
