function WebShopGiveItem(_req)
	local role_id 		= _req["cid"]
	local item_type 	= _req["itype"]
	local item_name 	= _req["iname"]
	local item_number	= _req["inum"] or 1
	local money 			= _req["mnum"]
	local money_type 	= _req["mtype"]
	
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	
	local data = {["item_name"] = item_name, ["char_id"] = role_id, ["item_type"] = item_type, ["count"] = item_number, ["money"] = money, ["money_type"] = money_type}
	CallDbTrans("GMToolsDbActionDB", "BuyItemFromWebShop", CallBack, data)
	
	local ret = coroutine.yield()
	if not ret[1] then
		return false, ret[2]
	end
	return true, 200
end

function WebShopChangeMoney(_req)
	local role_id = _req["cid"]
	local money 	= _req["num"]
	
	local co=coroutine.running()
	local function CallBack(result)
		if not result then
			coroutine.resume(co, -1)
			return
		end
		
		local function CallBack2(rs)
			if not rs then
				coroutine.resume(co, -2)
				return
			end
			coroutine.resume(co, 200)
		end
		
		local startTime = os.clock()
		while(os.clock()-startTime < 10) do
		end
		
		local param = {["char_id"] = role_id}
		CallDbTrans("GMToolsDbActionDB", "IsDrawBalanceSucc", CallBack2, param)
	end
	
	local data = {["char_id"] = role_id, ["money"] = money}
	CallDbTrans("GMToolsDbActionDB", "AddDrawBalanceInfo", CallBack, data)
	
	local ret = coroutine.yield()
	
	return true, ret
end

g_WebFuncDef["giveItem.do"]=WebShopGiveItem
g_WebFuncDef["changeMoney.do"]=WebShopChangeMoney