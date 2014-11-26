require "tools.util"

NodeManager = {}

function NodeManager:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function NodeManager:ctor()
	self.nodes = {}
end

function NodeManager:register(node, session, uri)
	if self.nodes[node.nodeType] == nil then
		self.nodes[node.nodeType] = {}
	end
	self.nodes[node.nodeType][node.nodeId] = { session = session, uri = uri }
	
	print("node"..node.nodeId.."["..node.nodeType.."] opened")
end

function NodeManager:unregister(node)
	if self.nodes[node.nodeType] ~= nil then
		self.nodes[node.nodeType][node.nodeId] = nil
	
		print("node"..node.nodeId.."["..node.nodeType.."] closed")
	end
end

function NodeManager:getNode(node)
	if not self.nodes[node.nodeType] then
		return nil
	end
	
	if not self.nodes[node.nodeType][node.nodeId] then
		return nil
	end
	
	return self.nodes[node.nodeType][node.nodeId]
end

function NodeManager:getRandomNodeByType(nodeType)
	if not self.nodes[nodeType] then
		return nil
	end

	return util.random_select(self.nodes[nodeType])
end

function NodeManager:getNodesByType(nodeType)
	return self.nodes[nodeType]
end

function NodeManager:sendToNode(node, msg)
	local unit = self:getNode(node)
	if unit then
		return unit.session:send(msg)
	end
	return false
end

function NodeManager:sendToNodeByType(nodeType, msg)
	local unit = self:getRandomNodeByType(nodeType)
	if unit then
		return unit.session:send(msg)
	end
	return false
end

function NodeManager:brocastNode(msg)
	for j, u in pairs(self.nodes) do
		for k, v in pairs(u) do
			v.session:send(msg)
		end
	end
end

function NodeManager:brocastNodeByType(nodeType, msg)
	if not nodes[nodeType] then
		return nil
	end
	
	for k, v in pairs(nodes[nodeType]) do
		v.session:send(msg)
	end
end

return NodeManager
