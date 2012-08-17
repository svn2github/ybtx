
CallGasDef = {}

--function CallGasDef.Test(n, s, t)
--	print(n, s)
--	for k,v in pairs(t) do
--		print(k, v)
--		if type(v) == "table" then
--			for k2,v2 in pairs(v) do
--				print("     ",k2, v2)
--			end
--		end
--	end
--	
--	return 1, nil, "sss", {1,2,"c",{1}}
--end
--
--local function ontesttick(tick)
-- 	local a = 1000
--	UnRegisterTick(tick)
--	local function callback(a1, a2, a3, a4, a5)
--		print(a)
--		print(a1, a2, a3, a4, a5)
--	end
--	local function timeout()
--	end
--	CallGas(GetServerConn(2), "Test", callback, timeout, 0, 10, "sss", {1,2,{"a","b"}})
--end
--
--if g_CurServerId == 1 then
--	RegisterTick("test",  ontesttick, 15000)
--end


function CallGasDef.CreateSecretKey(userName, charId, userId)
	local secretKey = charId .. " " ..  math.random(65535) .. os.time()
	AddSecretKeyTick(secretKey, userName, charId, userId)
	return secretKey
end