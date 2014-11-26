require "net.Cluster"
require "app.CharacterManager"

cluster = Cluster:new("hash", "list", "billboard", "friendship", "chat")
characterManager = CharacterManager:new()

core:register("queryGuid", "", function(platform, server, username)
	characterManager:query(platform, server, username)
end)

core:register("dispatch", "", function(guid, cmd, msg)
	characterManager:dispatch(guid, cmd, msg)
end)


