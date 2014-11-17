MessageHandlerBase = {}
MessageHandlerBase.handlers = nil

function MessageHandlerBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    return ret
end

function MessageHandlerBase:handler(session, msg)
	local next,cmd,len1 = string.unpack(msg,"HH")
    local next,msgBody = string.unpack(msg,"A"..(len1 -4), next)
    if self.handlers ~= nil and self.handlers[cmd] ~= nil then
        self.handlers[cmd](session, msgBody)
    else
        print("discard unknow message cmd=" .. cmd)
    end
end

function MessageHandlerBase:register(cmd, decoder, func)
	if self.handlers == nil then
		self.handlers = {}
	end
	self.handlers[cmd] = function(session, msg)
		info = decoder()
		if pcall(info.ParseFromString, info, msg) then
			func(session, info)
		end
	end
end

return MessageHandlerBase
