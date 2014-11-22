module ("ClientSession", package.seeall)

Session = {}
function Session:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function Session:ctor(id, manager, connection)
	self.id = id
	self.manager = manager
	self.connection = connection
end

function Session:send(msg)
	if self.connection then
		self.connection:send(msg)
		return true
	end
	return false
end

function Session:close()
	if self.connection then
		self.connection:close()
	end
end

function Session:onEstablish()
	self.manager.sessions[self.id] = self
	
	core:fork(function()
		if not core:randCall("transit", "createPlayer", false, config:getNode(), self.id) then
			self:close()
		end
	end)
end

function Session:onTerminate()
	self.manager.sessions[self.id] = nil
	
	core:fork(function()
		core:randCall("transit", "destoryPlayer", false, config:getNode(), self.id)
	end)
end

function Session:onRecv(buff)
	core:fork(function()
		core:randCall("transit", "doPlayerMessage", false, config:getNode(), self.id, buff)
	end)
end

SessionManager = {}
function SessionManager:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function SessionManager:ctor()
	self.nextSession = 0
	self.sessions = {}
end

function SessionManager:createSession(connection)
	self.nextSession = self.nextSession + 1
	if self.nextSession > 1000000000 then
		self.nextSession = 1
	end

    connection:setSession(ClientSession:new(self.nextSession, self, connection))
end

function SessionManager:close(id)
	if self.sessions[id] ~= nil then
		self.sessions[id]:close()
	end
end

function SessionManager:closeAll()
	for k, v in pairs(self.sessions) do
		v:close()
	end
end

function SessionManager:send(id, msg)
	if self.sessions[id] ~= nil then
		self.sessions[id]:send(msg)
	end
end
