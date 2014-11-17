PacketBuilderBase = {}

function PacketBuilderBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    return ret
end

function PacketBuilderBase:pack(cmd, info)
	local body = info:SerializeToString()
	local len = #body + 4
	return string.pack("HHA",cmd,len,body)
end

return PacketBuilderBase
