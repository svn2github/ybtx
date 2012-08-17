local collectgarbage = collectgarbage
local MemLogDB = CreateDbBox(...)

function MemLogDB.FetchMemUsed(data)
	return data[1], collectgarbage("count")
end

return MemLogDB