require "net.Cluster"

cluster = Cluster:new("center", "hash", "list", "billboard", "friendship")

mysql = core:startMysql("localhost", 3306, "test", "root", "123456", 0, "utf8")

core:register("dispatch", "", function(guid, cmd, msg)
end)

core:fork(function()
	local ret, data = core:query(mysql, "select * from test")
	print(ret, data)
	for i, v in ipairs(data) do
		for j, u in ipairs(v) do
			print(u)
		end
	end
end)
