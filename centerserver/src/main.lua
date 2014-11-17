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
	
	require "Config"
	require "net.NetInit"
	if not Net.Manager:init() then
		return false
	end

    return true
end

function run()
    
end

function release()
	Net.Manager:release()
end

local status, msg = xpcall(init, __G__TRACKBACK__)
if not status then
    error(msg)
end

return true
