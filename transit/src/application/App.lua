require "net.Core"

local App = {}

function App:init()

	core = Core:new({ nodeType = "transit", nodeId = "1", uri = "127.0.0.1:7800"}, self)
	core:listenNode(7800)
	core:openNode("127.0.0.1:7700")
	
	return true
end

function App:onOpen(node)
	core:fork(function () 
		local ret = core:randCall("master", "add", true, 1, 2)
		print("call master add 1, 2 result "..ret)
	end)
end

function App:onClose(node)
end

function App:run()
	core:run()
end

function App:release()
end

return App
