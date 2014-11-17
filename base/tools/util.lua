module("util", package.seeall)

function random_select(container)
	local ret = nil
	
	local total = 1
	for k, v in pairs(container) do
		if math.random(total) == 1 then
			ret = v
		end
		total = total + 1
	end
	
	return ret
end
