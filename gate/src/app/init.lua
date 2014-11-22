require "net.Cluster"

cluster = Cluster:new("transit")
sessionManager = ClientSession.SessionManager:new()
if not Net.startListen(config:getAPPort(), sessionManager) then
	error("listen client failed")
end

core:addNodeEventListener({
	onOpen = function(node)
	
	end, onClose = function(node)
		if node.nodeType == "transit" then
			sessionManager:closeAll()
		end
	end
})

core:register("kickoff", function(id)
	sessionManager:close(id)
end)
