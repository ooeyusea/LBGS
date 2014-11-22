
local Config = {}

function Config:loadFrom(filename)
	self.node = {nodeType = "master", nodeId = "1", uri = "127.0.0.1:7700", port = 7700 }
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

return Config
