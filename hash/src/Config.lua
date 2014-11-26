
local Config = {}

function Config:loadFrom(filename)
	self.node = {nodeType = "hash", nodeId = "1", uri = "127.0.0.1:8100", port = 8100 }
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

function Config:getMaster()
	return self.master
end

return Config
