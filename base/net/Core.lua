require "net.NodeManager"
require "net.NodeSession"
require "proto.NodeProtocol_pb"
require "proto.NodeProto"
require "net.Net"

function printtable(t)
	print("table print")
	for k, v in pairs(t) do
		print(tostring(k).." "..tostring(v))
	end
end

Core = {}

function Core:new(...)
    local ret = {}
    setmetatable(ret,self)
    self.__index = self
	ret:ctor(...)
    return ret
end

function Core:ctor(config)
	self.config = config
	self.manager = NodeManager:new()
	self.proto = {}
	
	self.coroutine_pool = {}
	
	self.nextSeq = 0
	self.sequence = {}
	self.co_sequence = {}
	self.co_sequence_node = {}
	self.wait_sequence = {}
	
	self.mysqls = {}
	self.wait_sql = {}
	
	self.mutexId = 0
	self.mutexs = {}
	self.co_mutex = {}
	self.wakeup_mutexs = {}
	
	self.fork_queue = {}
	
	self.msg_queue = {}
	
	self.nodeEventListeners = {}
end

function Core:openNode(uri)
	return Net.startConnect(uri, NodeSession:new(self))
end

function Core:listenNode(port)
	return Net.startListen(port, NodeSession:new(self))
end

function Core:addNodeEventListener(listener)
	table.insert(self.nodeEventListeners, listener)
end

function Core:startMysql(host, port, db, user, password, flag, charset)
	local mysql = DBModule.create()
	if not mysql:init(user, host, db, password, port, flag, charset) then
		mysql:release()
		return nil
	end
	table.insert(self.mysqls, mysql)
	return #self.mysqls
end

function Core:release()
	for i, v in ipairs(self.mysqls) do
		v:release()
	end
	self.mysqls = {}
end

--------------------------------------------- util  ---------------------------------------------

function Core:getNextSeq()
	self.nextSeq = self.nextSeq + 1
	
	if self.nextSeq < 1 then
		self.nextSeq = 1
	end
	
	return self.nextSeq
end

function Core:nextMutex()
	self.mutexId = self.mutexId + 1
	
	if self.mutexId > 1000000000 then
		self.mutexId = 1
	end
	
	self.mutexs[self.mutexId] = { used = false, watcher = {} }
	
	return self.mutexId
end

function Core:send(node, ...)
	local unit = self.manager:getNode(node)
	if unit then
		return unit.session:send(self:buildMessage(string.serialize(...)))
	end
	return false
end

function Core:randSend(nodeType, ...)
	local unit = self.manager:getRandomNodeByType(nodeType)
	if unit then
		return unit.session:send(self:buildMessage(string.serialize(...)))
	end
	return false
end

------------------------------------------- coroutine -------------------------------------------

function Core:createCo(func)
	local co = table.remove(self.coroutine_pool)
	if co == nil then
		co = coroutine.create(function()
			func()
			while true do
				func = nil
				self.coroutine_pool[#self.coroutine_pool+1] = co
				func = coroutine.yield("EXIT")
				func(coroutine.yield())
			end
		end)
		
	else
		coroutine.resume(co, func)
	end
	return co
end

function Core:coroutineExit(co, excption)
	local seq = self.co_sequence[co]
	if seq ~= nil then
		self.sequence[seq] = nil
		local node = self.co_sequence_node[co]
		if excption and node ~= nil then
			self:send(node, "RESULT", seq, false)
		end
		self.co_sequence[co] = nil
		self.co_sequence_node[co] = nil
	end	
	
	if self.co_mutex[co] ~= nil then
		for mutex, v in self.co_mutex[co] do
			table.insert(self.wakeup_mutexs, co)
			
			if self.mutexs[mutex] ~= nil then
				self.mutexs[mutex].used = false
			end
		end
		self.co_mutex[co] = nil
	end
end

function Core:coroutineCall(co, seq)
	self.wait_sequence[seq] = co
end

function Core:coroutineSQL(co, seq)
	self.wait_sql[seq] = co
end

function Core:coroutineLock(co, mutex)
	table.insert(self.mutexs[mutex], co)
end

function Core:coroutineReturn(co, ...)
	local seq = self.co_sequence[co]
	if seq == nil then
		error("return but co is not doing a rpc call")
	end
	
	local node = self.co_sequence_node[co]
	if node == nil then
		error("cannot find node")
	end
	
	return self:suspend(co, coroutine.resume(co, self:send(node, "RESULT", seq, true, ...))) 
end

function Core:suspend(co, result, command, ...)
	if not result then
		self:coroutineExit(co, true)
		error("coroutine stop exception:"..command)
	end
	
	if command == "EXIT" then
		self:coroutineExit(co)
	elseif command == "CALL" then
		self:coroutineCall(co, ...)
	elseif command == "SQL" then
		self:coroutineSQL(co, ...)
	elseif command == "LOCK" then
		self:coroutineLock(co, ...)
	elseif command == "RETURN" then
		return self:coroutineReturn(co, ...)
	else
		error("Unknown command : " .. command)
	end
end

function Core:create(node, seq, f, ...)
	local args = {...}
	local co = self:createCo(function() f(unpack(args)) end)
	if co then
		self.sequence[seq] = co
		self.co_sequence[co] = seq
		self.co_sequence_node[co] = node
		
		return self:suspend(co, coroutine.resume(co))
	end
end

function Core:fork(f, ...)
	local args = {...}
	local co = self:createCo(function() f(unpack(args)) end)
	if co then
		table.insert(self.fork_queue, co)
	end	
end

function Core:run()	
	while true do
		local msg = table.remove(self.msg_queue, 1)
		if msg == nil then
			break
		end
		
		if msg.type == "NETMESSAGE" then
			self:handle(msg.node, string.unserialize(msg.msg))
		elseif msg.type == "OPENEED" then
			for i, listener in ipairs(self.nodeEventListeners) do
				listener:onOpen(msg.node)
			end
		elseif msg.type == "CLOSED" then
			for i, listener in ipairs(self.nodeEventListeners) do
				listener:onClose(msg.node)
			end
		end
	end
	
	while true do
		local co =  table.remove(self.fork_queue, 1)
		if co == nil then
			break
		end
		self:suspend(co, coroutine.resume(co))
	end
	
	while true do
		local mutex = table.remove(self.wakeup_mutexs, 1)
		if mutex == nil then
			break
		end
		
		if self.mutexs[mutex] ~= nil then
			if self.mutexs[mutex].used then
				error("wake mutex but is still used")
			end
			local co = table.remove(self.mutexs[mutex].watcher)
			if co then
				self:suspend(co, coroutine.resume(co))
			end
		end
	end
	
	for i, v in ipairs(self.mysqls) do
		v:run()
	end
end

--------------------------------------------- proto ---------------------------------------------

function Core:register(name, nodeType, func)
	self.proto[name] = { limit = nodeType, func = func }
end

function Core:rpc(node, seq, func, ...)
	if seq == nil then
		return
	end

	if func and self.proto[func] then
		self:create(node, seq, self.proto[func].func, ...)
	else
		self:send(node, "RESULT", seq, false)
	end
end

function Core:rpcnr(func, ...)
	if self.proto[func] then
		self:fork(self.proto[func].func, ...)
	end
end

function Core:respone(seq, succ, ...)
	if self.wait_sequence[seq] ~= nil then
		local co = self.wait_sequence[seq]
		if co then
			self.wait_sequence[seq] = nil
			return self:suspend(co, coroutine.resume(co, succ, ...))
		end
	end
end

function Core:handle(node, what, ...)
	if what == "RPC" then
		self:rpc(node, ...)
	elseif what == "RPCNR" then
		self:rpcnr(...)
	elseif what == "RESULT" then
		self:respone(...)
	end
end

function Core:callback(errorInfo, succ, ...)
	if not succ then
		error(errorInfo)
	end
	return ...
end

function Core:call(node, func, wait, ...)
	if wait then
		local seq = self:getNextSeq()
		if not self:send(node, "RPC", seq, func, ...) then
			error("call "..func.." node["..node.nodeType..":"..node.nodeId.."] unreachable")
		end
		return self:callback("call "..func.." node["..node.nodeType..":"..node.nodeId.."] failed", coroutine.yield("CALL", seq))
	else
		if not self:send(node, "RPCNR", func, ...) then
			error("call "..func.." node["..node.nodeType..":"..node.nodeId.."] unreachable")
		end
	end
end

function Core:randCall(nodeType, func, wait, ...)
	if wait then
		local seq = self:getNextSeq()
		if not self:randSend(nodeType, "RPC", seq, func, ...) then
			error("rand call "..func.." node["..nodeType.."] unreachable")
		end
		return self:callback("call "..func.." node["..nodeType.."] failed", coroutine.yield("CALL", seq))		
	else
		if not self:randSend(nodeType, "RPCNR", func, ...) then
			error("rand call "..func.." node["..nodeType.."] unreachable")
		end
	end
end

function Core:wildCall(func, ...)
	self.manager:brocastNode(self:buildMessage(string.serialize("RPCNR", func, ...)))
end

function Core:ret(...)
	return coroutine.yield("RETURN", ...)
end

------------------------------------------------ mysql ------------------------------------------------

function Core:waitSql(seq)
	return function(...)
		if self.wait_sql[seq] ~= nil then
			local co = self.wait_sql[seq]
			if co then
				self.wait_sql[seq] = nil
				return self:suspend(co, coroutine.resume(co, ...))
			end
		end
	end
end

function Core:query(mysql, sql)
	if self.mysqls[mysql] ~= nil then
		local seq = self:getNextSeq()
		self.mysqls[mysql]:query(sql, self:waitSql(seq))
		return coroutine.yield("SQL", seq)
	end
	
	return false, "unknown mysql"
end

function Core:exec(mysql, sql, wait)
	if self.mysqls[mysql] ~= nil then
		if wait then
			local seq = self:getNextSeq()
			self.mysqls[mysql]:exec(sql, self:waitSql(seq))
			return coroutine.yield("SQL", seq)
		else
			self.mysqls[mysql]:exec(sql, function() end)
			return true
		end
	else
		return false, "unknown mysql"
	end
end

----------------------------------------------- lock -----------------------------------------------------

function Core:lock(mutex)
	if mutex == nil then
		mutex = self:nextMutex()
	end
	
	if self.mutexs[mutex].used then
		coroutine.yield("LOCK", mutex)
	end
	
	self.mutexs[mutex].used = true
	
	local co = coroutine.running()
	if self.co_mutex[co] == nil then
		self.co_mutex[co] = {}
	end
	self.co_mutex[co][mutex] = true
	return mutex
end

function Core:unlock(mutex)
	if self.mutexs[mutex] ~= nil then
		self.mutexs[mutex].used = false
		
		local co = coroutine.running()
		self.co_mutex[co][mutex] = nil
		
		table.insert(self.wakeup_mutexs, mutex)
	end
end

function Core:isLocked(mutex)
	if mutex == nil then
		return false
	end
	
	return self.mutexs[mutex].used
end

-------------------------------------------- session --------------------------------------------

function Core:readNode(session, msg)
	local next,cmd,len1 = string.unpack(msg,"HH")
	if cmd ~= NodeProto.report then
		print("first packet is not report msg")
		session:close()
		return
	end
	
    local next,msgBody = string.unpack(msg,"A"..(len1 -4), next)
	local info = NodeProtocol_pb.Report()
	if pcall(info.ParseFromString, info, msgBody) then
		session.remoteNode = { nodeType = info.nodeType, nodeId = info.nodeId, uri = info.uri }
		self.manager:register(session.remoteNode, session, info.uri)
		
		table.insert(self.msg_queue, { type = "OPENEED", node = session.remoteNode} )
	else
		print("read report msg failed")
		session:close()
	end
end

function Core:remove(session)
	if session.remoteNode then
		table.insert(self.msg_queue, { type = "CLOSED", node = session.remoteNode} )
		self.manager:unregister(session.remoteNode)
		session.remoteNode = nil
	end
end

function Core:dispatch(session, msg)
	local next,cmd,len1 = string.unpack(msg,"HH")
	local next,msgBody = string.unpack(msg,"A"..(len1 -4), next)
	if cmd == NodeProto.message then
		local info = NodeProtocol_pb.Message()
		if pcall(info.ParseFromString, info, msgBody) then
			table.insert(self.msg_queue, { type = "NETMESSAGE", node = session.remoteNode, msg = info.msg } )
		else
			print("parse node msg failed")
		end
	else
		print("unknown node msg")
	end
end

------------------------------------------- packet ----------------------------------------------------

function Core:buildMessage(msg)
	info = NodeProtocol_pb.Message()
	info.msg = msg
	
	return self:pack(NodeProto.message, info)
end

function Core:buildReport()
	info = NodeProtocol_pb.Report()
	info.nodeType = self.config.nodeType
	info.nodeId = self.config.nodeId
	info.uri = self.config.uri
	
	return self:pack(NodeProto.report, info)
end

function Core:pack(cmd, info)
	local body = info:SerializeToString()
	local len = #body + 4
	return string.pack("HHA",cmd,len,body)
end

return Core