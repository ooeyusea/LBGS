require "net.Cluster"

cluster = Cluster:new("transit")

core:register("authen", "", function(platform, authenCode)
	core:ret(authenCode)
end)
