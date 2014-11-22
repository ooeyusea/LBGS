
local Config = {}

function Config:loadFrom(filename)
	self.node = {nodeType = "gate", nodeId = "1", uri = "" }
	self.master = "127.0.0.1:7700"
end

function Config:init()
	if not self:loadFrom("../config.ini") then
		return false
	end
	
	return true
end

function Config:getNodeConfig()
	return self.node
end

function Config:getNode()
	return { nodeType = self.node.nodeType, nodeId = self.node.nodeId }
end

function Config:getMaster()
	return self.master
end

return Config
