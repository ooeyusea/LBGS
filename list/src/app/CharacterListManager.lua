CharacterListManager = {}

function CharacterListManager:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function CharacterListManager:ctor()
	self.guids = {}
end

function CharacterListManager:init()
	return true
end

function CharacterListManager:createCharacter(platform, server, username)
	return platform..":"..server..":"..username
end

function CharacterListManager:query(platform, server, username)
	local tmp = platform..":"..server..":"..username
	if self.guids[tmp] == nil then
		self.guids[tmp] = self:createCharacter(platform, server, username)
	end
	
	core:ret(self.guids[tmp])
end

return CharacterListManager
