require "net.Cluster"
require "app.CharacterListManager"

cluster = Cluster:new()
listManager = CharacterListManager:new()
if not listManager:init() then
	error("init list manager failed.")
end

core:register("query", "", function(platform, server, username)
	listManager:query(platform, server, username)
end)
