NodeSession = {}
function NodeSession:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function NodeSession:ctor(node)
	self.node = node
end

function NodeSession:send(msg)
	if self.connection then
		self.connection:send(msg)
		return true
	end
	return false
end

function NodeSession:close()
	if self.connection then
		self.connection:close()
	end
end

function NodeSession:createSession(connection)
	local session = self:new(self.node)
    session.connection = connection
    connection:setSession(session)
end

function NodeSession:onEstablish()
	self:send(self.node:buildReport())
end

function NodeSession:onTerminate()
	self.node:remove(self)
	self.connection = nil
end

function NodeSession:onRecv(buff)
	if self.remoteNode then
		self.node:dispatch(self, buff)
	else
		self.node:readNode(self, buff)
	end
end

return NodeSession
