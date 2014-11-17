StateMachineBase = {}
StateMachineBase.state = nil

function StateMachineBase:new()
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
    return ret
end

function StateMachineBase:changeState(state)
	if self.state ~= nil then
		self.state:onExit(self)
	end
	
	self.state = state
	
	if self.state ~= nil then
		self.state:onEnter(self)
	end
end

function StateMachineBase:doEvent(evt)
	if self.state ~= nil then
		self.state:onEvent(self, evt)
	end
end

return StateMachineBase
