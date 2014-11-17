require "tools.util"
MultiSessionManagerBase = {}
MultiSessionManagerBase.session = nil
MultiSessionManagerBase.sessions = nil
MultiSessionManagerBase.nextSessionId = 0

function MultiSessionManagerBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    ret.sessions = {}
    return ret
end

function MultiSessionManagerBase:setSession(session)
	self.session = session
end

function MultiSessionManagerBase:createSession(connection)
	self.nextSessionId = self.nextSessionId + 1
	if self.nextSessionId > 200000000 then
		self.nextSessionId = 1
	end
	
    local session = self.session:new()
    session:setConnection(connection)
	session.id = self.nextSessionId
    connection:setSession(session)
end

function MultiSessionManagerBase:send(id, msg)
	local session = self.sessions[id]
	if session ~= nil then
		session:send(msg)
		return true
	end
	return false
end

function MultiSessionManagerBase:randomSend(msg)
	local session = util.random_select(self.sessions)
	if session ~= nil then
		print("randSend")
		session:send(msg)
		return true
	end
	return false
end

function MultiSessionManagerBase:close(id)
	local session = self.sessions[id]
	if session ~= nil then
		session:close()
	end
end

function MultiSessionManagerBase:closeAll()
	for k, v in pairs(self.sessions) do
		v:close()
	end
end

return MultiSessionManagerBase
