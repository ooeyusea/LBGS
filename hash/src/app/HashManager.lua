HashManager = {}

function HashManager:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function HashManager:ctor()
	self.guids = {}
	self.nodes = {}
	self.node_queue = {}
end

function HashManager:calcHash(guid)
	if #self.node_queue == 0 then
		return nil
	end
	
	local key = 1
	return self.node_queue[key]
end

function HashManager:hash(guid)
	if self.guids[guid] == nil then
		self.guids[guid] = self:calcHash(guid)
	end
	
	core:ret(self.guids[guid])
end

function HashManager:onDataOpen(node)
	if self.nodes[node.nodeType] == nil then
		self.nodes[node.nodeType] = {}
	end
	
	if not self.nodes[node.nodeType][node.nodeId] then
		self.nodes[node.nodeType][node.nodeId] = true
		table.insert(self.node_queue, node)
	end
end

return HashManager
