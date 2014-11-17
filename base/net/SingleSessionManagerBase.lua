SingleSessionManagerBase = {}
SingleSessionManagerBase.session = nil
SingleSessionManagerBase.usedSession = nil

function SingleSessionManagerBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    return ret
end

function SingleSessionManagerBase:setSession(session)
	self.session = session
end

function SingleSessionManagerBase:createSession(connection)
    local session = self.session:new()
    session:setConnection(connection)
    connection:setSession(session)
end

function SingleSessionManagerBase:send(msg)
	if self.usedSession ~= nil then
		self.usedSession:send(msg)
		return true
	end
	return false
end

function SingleSessionManagerBase:close(id)
	if self.usedSession ~= nil then
		self.usedSession:close()
	end
end

return SingleSessionManagerBase
