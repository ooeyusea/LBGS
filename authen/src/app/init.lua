require "net.Cluster"

cluster = Cluster:new("transit")

core:register("authen", "", function(platform, authenCode)
	username = "12312323"
	core:ret(username)
end)
