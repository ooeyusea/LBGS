require "net.Cluster"

cluster = Cluster:new("center")

core:register("dispatch", "", function(guid, cmd, msg)
end)
