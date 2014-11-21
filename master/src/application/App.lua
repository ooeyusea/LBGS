require "net.Core"

local App = {}

function App:init()

	core = Core:new({ nodeType = "master", nodeId = "1", uri = "127.0.0.1:7700"}, self)
	core:listenNode(7700)
	core:register("add", "", function(a, b)
		core:ret(a+b)
	end)

	return true
end

function App:onOpen(node)
end

function App:onClose(node)
end

function App:run()
	core:run()
end

function App:release()
end

return App
