require "app.proto.ClientPacketBuilder"

Player = {}

function Player:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function Player:ctor(node, id)
	self.node = node
	self.id = id
end

function Player:login(info)
	local succ, username = core:randCall("login", "authen", true, info.platform, info.authenCode)
	if self.dead then 
		return 
	end
	
	if not succ then
		core:call(self.node, "sendToClient", false, self.id, ClientPacketBuilder:buildLoginFailAck())
		return
	end
	
	self.platform = info.platform
	self.server = info.server
	self.username = username
	PlayerManager.kickoffByUsername(info.platform, info.server, username)
	PlayerManager.setPlayerByUsername(player)
	
	local guid = core:randCall("center", "queryGuid", true, self.platform, self.server, self.username)
	if self.dead then 
		return 
	end
	
	self.guid = guid
	PlayerManager.kickOffByGuid(guid)
	PlayerManager.setPlayerByGuid(player)
	
	self.online = true
	
	core:call(self.node, "sendToClient", false, self.id, ClientPacketBuilder:buildLoginSuccAck(self.username, self.guid))
end

function Player:dispatch(cmd, msg)
	if self.online then
		core:randCall("center", "dispatch", self.guid, cmd, msg)
	end
end

function Player:logout()
	
end

return Player
