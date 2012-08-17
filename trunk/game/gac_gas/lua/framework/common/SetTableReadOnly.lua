
function ReadOnly (t)
	local proxy = {}
	local mt = {			-- create metatable
		__index = t,
		__newindex = function (t,k,v)
			error("attempt to update a read-only table", 2)
		end
	}

	setmetatable(proxy, mt)
	return proxy
end

