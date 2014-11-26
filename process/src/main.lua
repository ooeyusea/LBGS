cclog = function(...)
    print(string.format(...))
end

function __G__TRACKBACK__(msg)
    cclog("----------------------------------------")
    cclog("LUA ERROR: " .. tostring(msg) .. "\n")
    cclog(debug.traceback())
    cclog("----------------------------------------")
    return msg
end

function init()
    collectgarbage("collect")
    -- avoid memory leak
    collectgarbage("setpause", 100)
    collectgarbage("setstepmul", 5000)
	
	package.path = package.path..";../base/?.lua;../base/protobuf/?.lua"
	
	math.randomseed(os.time())

	local config = require "Config"
	config:init()
	local node = config:getNodeConfig()
	
	require "net.Core"
	core = Core:new(node)
	if node.port ~= nil then
		if not core:listenNode(node.port) then
			error("start listen node failed")
		end
	end
	
	core:openNode(config:getMaster())
	
	require "app.init"
end

function run()
    core:run()
end

function release()
	core:release()
end

local status, msg = xpcall(init, __G__TRACKBACK__)
if not status then
    error(msg)
end

return status
