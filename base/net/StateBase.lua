StateBase = {}

function StateBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    return ret
end

function StateBase:onEnter(obj)
end

function StateBase:onExit(obj)
end

function StateBase:onEvent(obj, evt)
end

return StateBase
