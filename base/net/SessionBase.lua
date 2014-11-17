SessionBase = {}
function SessionBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    return ret
end

function SessionBase:setConnection(connection)
    self.connection = connection
end

function SessionBase:send(msg)
    self.connection:send(msg)
end

function SessionBase:onEstablish()
    print(""..self.connection:getRemote().." establish")
	if self.onEstablishPlus ~= nil then
		self:onEstablishPlus()
	end
end

function SessionBase:onTerminate()
    print(""..self.connection:getRemote().." terminate")
	if self.onTerminatePlus ~= nil then
		self:onTerminatePlus()
	end
end

function SessionBase:close()
	self.connection:close()
end

return SessionBase
