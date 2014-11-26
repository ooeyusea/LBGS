CharacterManager = {}

function CharacterManager:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function CharacterManager:ctor()
	self.characters = {}
	self.usernames = {}
	self.hashs = {}
	self.cmds = {}
end

function CharacterManager:register(cmd, lock, node)
	self.cmds[cmd] = { lock = lock, func = function(...)
		core:call(node, "dispatch", false, ...) 
	end}
end

function CharacterManager:registerByType(cmd, lock, nodeType)
	self.cmds[cmd] = { lock = lock, func = function(...)
		core:randCall(nodeType, "dispatch", false, ...)
	end}
end

function CharacterManager:registerDataNodeMsg(cmd, lock)
	self.cmds[cmd] = { lock = lock, func = function(guid, ...)
		local node = self:getHash(guid)
		core:call(node, "dispatch", false, guid, ...)
	end}	
end

function CharacterManager:getHash(guid)
	if self.hashs[guid] == nil then
		self.hashs[guid] = core:randCall("hash", "getHash", true, guid)
	end
	
	return self.hashs[guid]
end

function CharacterManager:query(platform, server, username)
	local tmp = platform..":"..server..":"..username
	if self.usernames[tmp] then
		core:ret(self.usernames[tmp])
		return
	end
	
	local guid = core:randCall("list", "query", true, platform, server, username)
	
	self.usernames[platform..":"..server..":"..username] = guid
	
	if self.characters[guid] == nil then 
		self.characters[guid] = { platform = platform, server = server, username = username }
	end
	
	core:ret(guid)
end

function CharacterManager:dispatch(guid, cmd, msg)
	if self.characters[guid] == nil then
		return
	end
	
	if self.cmds[cmd] ~= nil then
		if self.cmds[cmd].lock then
			core:lock(self.characters[guid].lock)
			
			self.cmds[cmd].func(guid, cmd, msg)
			
			core:unlock(self.characters[guid].lock)
		end
	else
		print("invalid cmd:"..cmd)
	end
end

return CharacterManager
