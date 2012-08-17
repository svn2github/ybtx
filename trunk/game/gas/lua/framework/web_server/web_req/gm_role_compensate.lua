function Gas2GasDef:SetPlayerExp(conn,nAdd,role_name)
	local Player = g_GetPlayerInfoByName(role_name)
	if Player and Player.m_Conn then
		local Connection = Player.m_Conn
		Player.m_uLevelExp = Player.m_uLevelExp + nAdd
		if Player.m_uLevelExp < 0 then
			Player.m_uLevelExp = 0
		end
		local nCurlevel = Player:CppGetLevel()
		while Player.m_uLevelExp >= LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") do
			--如果不能升级，当前经验为0
			if( LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") == 0 ) then
				Player.m_uLevelExp = 0
				break
			end
			--减去升级经验，并且提升一级
			Player.m_uLevelExp = Player.m_uLevelExp - LevelExp_Common(nCurlevel, "ExpOfCurLevelUp")

			nCurlevel=nCurlevel+1
			CGasTeam.SetTeamMemberLevel(Player.m_uID)
			if nCurlevel == LevelExp_Common.GetHeight() then
				Player.m_uLevelExp=0
			end
			if nCurlevel == g_TestRevisionMaxLevel then
				Player.m_uLevelExp=0
			end	
		end
		Player:CppSetLevel(nCurlevel)
		Player:SetSkillToPlayer()
		if(nCurlevel==60) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
		end
		if(nCurlevel==120) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
			Gas2Gac:NotifyOpenPanel(Connection,3)
		end
		Gas2Gac:SetExp( Connection,nAdd,0,0,Player.m_uLevelExp,LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") )
	end
end

local function lAddExp(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local num = tonumber(_req["num"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["num"] = num,["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "AddExp", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		else
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:SetPlayerExp(GetServerAutoConn(server_id),num,role_name)
		  end
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddMoney(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local num = tonumber(_req["num"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["num"] = num,["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "AddMoney", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddBindingMoney(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local num = tonumber(_req["num"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["num"] = num,["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "AddBindingMoney", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddTicket(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local num = tonumber(_req["num"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["num"] = num,["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "AddTicket", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		else
			Gas2GacById:ReturnAddTicket(ret[2],num)
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddQuest(_req)
	--删除身上装备
	--先踢下线，再删除
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),_req["role_name"])
  end
  local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["quest_name"] = _req["quest_name"],["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "AddQuest", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		end
	end
	if error_role == "" then
	  return true,200
	else
	 	return false, error_role
	end
end

local function lFinishQuest(_req)
	local quest_name = _req["quest_name"]
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["quest_name"] = quest_name,["role_name"] = role_name}
		if not Quest_Common(quest_name) then
			error_role = error_role .. role_name
		else
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),role_name)
		  end
			local co = coroutine.running()
			local function CallBack(result)
				coroutine.resume(co, result)
			end	
			CallDbTrans("GMToolsDbActionDB", "FinishQuest", CallBack, data)
			local ret = coroutine.yield()
			if not ret[1] then
				error_role = error_role .. role_name
			end
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lDelQuest(_req)
	local quest_name = _req["quest_name"]
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["quest_name"] = quest_name,["role_name"] = role_name}
		if not Quest_Common(quest_name) then
			error_role = error_role .. role_name
		else
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),role_name)
		  end
			local co = coroutine.running()
			local function CallBack(result)
				coroutine.resume(co, result)
			end	
			CallDbTrans("GMToolsDbActionDB", "DelQuest", CallBack, data)
			local ret = coroutine.yield()
			if not ret[1] then
				error_role = error_role .. role_name
			end
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddItem(_req)
	local num = tonumber(_req["num"])
	local item_info = _req["item_info"]
	item_info = string.gsub( item_info, "ItemType:", "")
	item_info = string.gsub( item_info, "ItemName", "")
	local index1 = string.find(item_info,":")
	local item_type = tonumber(string.sub(item_info,1,index1-1))
	local item_name = string.sub(item_info,index1+1,string.len(item_info))
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),role_name)
	  end
		local co = coroutine.running()
		local function CallBack(result)
			coroutine.resume(co, result)
		end
		local data = {["num"] = num,["item_type"] = item_type,["item_name"] = item_name,["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "AddItem", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddEquip(_req)
	return lAddItem(_req)
end



local function lAddSoul(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local num =  tonumber(_req["num"])
	local soulRoot,item_name = "通用","通用魂珠"
	local nBigId =  g_ItemInfoMgr:GetSoulPearlBigID()
	if num > 2000000000 then
		return false,500
	end
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["num"] = num,["soulRoot"] = soulRoot,["nBigId"] = nBigId,["item_name"] = item_name,["role_name"] = role_name}
		
		CallDbTrans("GMToolsDbActionDB", "AddSoul", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		else

		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function lAddBindingSoul(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local num =  tonumber(_req["num"])
	local soulRoot,item_name = "通用","通用魂珠"
	local nBigId =  g_ItemInfoMgr:GetSoulPearlBigID()
	if num > 2000000000 then
		return false,500
	end
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local data = {["num"] = num,["soulRoot"] = soulRoot,["nBigId"] = nBigId,["item_name"] = item_name,["role_name"] = role_name}
		
		CallDbTrans("GMToolsDbActionDB", "AddBindingSoul", CallBack, data)
		local ret = coroutine.yield()
		if not ret[1] then
			error_role = error_role .. role_name
		else

		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function AddGMMgr(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local user_name = _req["user_name"]
	local gm_level = tonumber(_req["gm_level"])
	local game_id = tonumber(_req["game_id"])
	local data = {["user_name"] = user_name,["gm_level"] = gm_level,["game_id"] = game_id}
	CallDbTrans("GMToolsDbActionDB", "AddGMMgr", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function AddCurrencyLimit(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local money_func = _req["money_func"]
	local index1 = string.find(money_func,"+")
	local func_name = string.sub(money_func,1,index1-1)
	local mode_name = string.sub(money_func,index1+1,string.len(money_func))
	local data = {["func_name"] = func_name,["mode_name"] = mode_name}
	CallDbTrans("GMToolsDbActionDB", "AddCurrencyLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end
local function AddJifenUseLimit(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local jifen_type = _req["jifen_type"]
	local data = {["jifen_type"] = jifen_type}
	CallDbTrans("GMToolsDbActionDB", "AddJifenUseLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end
local function AddItemUseLimit(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local item_name = _req["item_name"]
	local data = {["item_name"] = item_name}
	CallDbTrans("GMToolsDbActionDB", "AddItemUseLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end
local function AddItemToTong(_req)
	local tong_names = _req["tong_name"]
	local item_type = _req["item_type"]
	local item_name = _req["item_name"]
	local page = _req["page"]
	local num = tonumber(_req["num"])
	local log = _req["log"]
	
	local tbl_tong_name = {}
	SplitString(tong_names, ',', tbl_tong_name)
	local error_tong = ""
	for i=1,#tbl_tong_name do
		local tong_name = tbl_tong_name[i]
		
		local co = coroutine.running()
		
		local function CallBack(result)
			local function Back2(ret)
				if not ret then
					error_tong = error_tong .. tong_name
				end
				coroutine.resume(co, ret)
			end
			
			if result ~= 0 then
				local data = {["tong_id"] = result, ["item_type"] = item_type, ["item_name"] = item_name, ["page"] = page, ["num"] = num, ["log"] = log}
				CallDbTrans("GMToolsDbActionDB", "AddItemToTong", Back2, data)
			else
				error_tong = error_tong .. tong_name
				coroutine.resume(co, result)
			end
		end
		local data = {["tong_name"] = tong_name}
		CallDbTrans("GMToolsDbActionDB", "GetTongId", CallBack, data)
		coroutine.yield()
	end
	if error_tong == "" then
		return true, 200
	else
		return false, error_tong
	end
end

function AddMoneyToTong(_req)
	local tong_names = _req["tong_name"]
	local add_money = _req["add_money"]
	
	local tbl_tong_name = {}
	SplitString(tong_names, ',', tbl_tong_name)
	local error_tong = ""
	for i=1,#tbl_tong_name do
		local tong_name = tbl_tong_name[i]
		
		local co = coroutine.running()
		
		local function CallBack(result)
			local function Back2(ret)
				if not ret then
					error_tong = error_tong .. tong_name
				end
				coroutine.resume(co, ret)
			end
		
			if result ~= 0 then
				local data = {["tong_id"] = result, ["add_money"] = add_money}
				CallDbTrans("GMToolsDbActionDB", "AddMoneyToTong", Back2, data)
			else
				error_tong = error_tong .. tong_name
				coroutine.resume(co, ret)
			end
		end
		
		local data = {["tong_name"] = tong_name}
		CallDbTrans("GMToolsDbActionDB", "GetTongId", CallBack, data)
		coroutine.yield()
	end
	if error_tong == "" then
		return true, 200
	else
		return false, error_tong
	end
end

function AddResourceToTong(_req)
	local tong_names = _req["tong_name"]
	local add_resource = _req["add_resource"]
	
	local tbl_tong_name = {}
	SplitString(tong_names, ',', tbl_tong_name)
	local error_tong = ""
	for i=1,#tbl_tong_name do
		local tong_name = tbl_tong_name[i]
		
		local co = coroutine.running()
		
		local function CallBack(result)
			local function Back2(ret)
				if not ret then
					error_tong = error_tong .. tong_name
				end
				coroutine.resume(co, ret)
			end
		
			if result ~= 0 then
				local data = {["tong_id"] = result, ["add_resource"] = add_resource}
				CallDbTrans("GMToolsDbActionDB", "AddResourceToTong", Back2, data)
			else
				error_tong = error_tong .. tong_name
				coroutine.resume(co, ret)
			end
		end
		
		local data = {["tong_name"] = tong_name}
		CallDbTrans("GMToolsDbActionDB", "GetTongId", CallBack, data)
		coroutine.yield()
	end
	if error_tong == "" then
		return true, 200
	else
		return false, error_tong
	end
end

function AddHonorToTong(_req)
	local tong_names = _req["tong_name"]
	local add_honor = _req["add_honor"]
	
	local tbl_tong_name = {}
	SplitString(tong_names, ',', tbl_tong_name)
	local error_tong = ""
	for i=1,#tbl_tong_name do
		local tong_name = tbl_tong_name[i]
		
		local co = coroutine.running()
		
		local function CallBack(result)
			local function Back2(ret)
				if not ret then
					error_tong = error_tong .. tong_name
				end
				coroutine.resume(co, ret)
			end
		
			if result ~= 0 then
				local data = {["tong_id"] = result, ["add_honor"] = add_honor}
				CallDbTrans("GMToolsDbActionDB", "AddHonorToTong", Back2, data)
			else
				error_tong = error_tong .. tong_name
				coroutine.resume(co, ret)
			end
		end
		
		local data = {["tong_name"] = tong_name}
		CallDbTrans("GMToolsDbActionDB", "GetTongId", CallBack, data)
		coroutine.yield()
	end
	if error_tong == "" then
		return true, 200
	else
		return false, error_tong
	end
end
g_WebFuncDef["AddExp"] = lAddExp
g_WebFuncDef["AddMoney"] = lAddMoney
g_WebFuncDef["AddBindingMoney"] = lAddBindingMoney
g_WebFuncDef["AddTicket"] = lAddTicket
g_WebFuncDef["AddQuest"] = lAddQuest
g_WebFuncDef["FinishQuest"] = lFinishQuest
g_WebFuncDef["AddEquip"] = lAddEquip
g_WebFuncDef["AddItem"] = lAddItem
g_WebFuncDef["DelQuest"] = lDelQuest
g_WebFuncDef["AddSoul"] = lAddSoul
g_WebFuncDef["AddBindingSoul"] = lAddBindingSoul
g_WebFuncDef["ResAddGMMgr"] = AddGMMgr
g_WebFuncDef["MoneyCurrencyLimit"] = AddCurrencyLimit
g_WebFuncDef["ResAddJifenLimit"] = AddJifenUseLimit
g_WebFuncDef["ResAddItemLimit"] = AddItemUseLimit
g_WebFuncDef["AddItemToTong"] = AddItemToTong
g_WebFuncDef["AddMoneyToTong"] = AddMoneyToTong
g_WebFuncDef["AddResourceToTong"] = AddResourceToTong
g_WebFuncDef["AddHonorToTong"] = AddHonorToTong