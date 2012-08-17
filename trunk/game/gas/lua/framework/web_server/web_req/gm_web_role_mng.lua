local MAX_NUM_PER_PAGE = 20

local function ItemNameMatch(_req)
	local ret_xml = ""
	local str_match = _req["str_match"] or "just_for_test"
	local tbl_matched = g_GetItemNameMatch(str_match)
	ret_xml = ret_xml .. "<root>\n"
	for i, v in pairs(tbl_matched) do
		ret_xml = ret_xml .. "<b big_id=\"" .. i .. "\">\n"
		for i=1,#(v) do 
			ret_xml = ret_xml .. "<s item_name=\"" .. v[i] .. "\"></s>\n"
		end
		ret_xml = ret_xml .. "</b>\n"
	end
	ret_xml = ret_xml .. "</root>\n"
	return true, ret_xml
end

local function QuestNameMatch(_req)
	local ret_xml = ""
	local str_match = _req["str_match"] or "just_for_test"
	ret_xml = ret_xml .. "<root>\n"
	local QuestKeys = Quest_Common:GetKeys()
	for i=1, table.getn(QuestKeys) do
		local QuestName = QuestKeys[i]
		if string.find(QuestName, str_match, 1, true) then
			ret_xml = ret_xml .. "<q qn=\"" .. QuestName .. "\"></q>\n"
		end
	end
	ret_xml = ret_xml .. "</root>\n"
	return true, ret_xml
end

function Gas2GasDef:ShutUpRole(conn, channel_id, role_id)
	local player=g_GetPlayerInfo(role_id)
	if player then
		player.m_tblForbidChannel[channel_id] = true
	end
end

local function ShutUp(_req)
	local channel_id = tonumber(_req["channel_id"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	
	local function CallBack(charIdTbl)
		for i=1,#charIdTbl do
			local role_id = charIdTbl[i]
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:ShutUpRole(GetServerAutoConn(server_id),channel_id,role_id)
			end
		end
	end
	
	local data = {["role_id"] = role_id,["role_name"] = tbl_role_name,["channel_id"] = channel_id}
	CallDbTrans("GMToolsDbActionDB", "ShutUp", CallBack, data)
	
	return true, 200
end

function Gas2GasDef:CancelShutUpRole(conn, channel_id, role_name, role_id)
	local player=g_GetPlayerInfoByName(role_name)
	if player then
		player.m_tblForbidChannel[channel_id] = false
	end
end

local function CancelShutUp(_req)
	local channel_id = tonumber(_req["channel_id"])
	local role_id = tonumber(_req["role_id"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local co=coroutine.running()
		local function CallBack(result)
			coroutine.resume(co, result)
		end
		local data = {["role_id"] = role_id,["role_name"] = role_name,["channel_id"] = channel_id}
		CallDbTrans("GMToolsDbActionDB", "ChannelShutUp", CallBack, data)
		local ret = coroutine.yield()
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:CancelShutUpRole(GetServerAutoConn(server_id),channel_id,role_name, role_id)
		end
	end
	return true, 200
end

function Gas2GasDef:KickOffRole(conn, role_name)
	local player = g_GetPlayerInfoByName(role_name)
	if player and player.m_Conn and CheckPlayerIsOnlineByName(role_name) then
		player.m_Conn:ShutDown("gm_KickOffRole")
	end
end

local function KickOff(_req)
	local role_id = tonumber(_req["role_id"])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),role_name)
  	end
  end
	return true, 200
end

function Gas2GasDef:KickRole(conn, role_id)
	local player = g_GetPlayerInfo(role_id)
	if player and player.m_Conn and CheckPlayerIsOnline(role_id) then
		player.m_Conn:ShutDown("gm_KickRole")
	end
end

local function KickRole(_req)
	local role_id = tonumber(_req["role_id"])
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:KickRole(GetServerAutoConn(server_id),role_id)
	end
  
  local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

function Gas2GasDef:GMMoveRole(conn, role_name, scene_name, x_pos, y_pos)
	--local scene = g_SceneMgr:GetScene(tonumber(scene_id))
	--local scene_name = scene.m_SceneName
	local player = g_GetPlayerInfoByName(role_name)
	if player and player.m_Conn then
		GMCommand.Executor.ChangeScene( player.m_Conn, scene_name, x_pos , y_pos )
	end
end

local function MoveRole(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end

	local scene_name = _req["scene_name"]
	local x_pos = tonumber(_req['x_pos'])
	local y_pos = tonumber(_req['y_pos'])
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:GMMoveRole(GetServerAutoConn(server_id),role_name, scene_name, x_pos, y_pos)
	  end
  end
  return true,200
end

local function SendWarning(_req)
	local warning = _req["warning"]
	local role_names = _req["role_name"]
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local co=coroutine.running()
		local function CallBack(result)
			coroutine.resume(co, result)
		end
		
		local data = {["role_name"] = role_name}
		CallDbTrans("GMToolsDbActionDB", "GetPlayerIdByName", CallBack, data)
		local ret = coroutine.yield()
		if ret ~= 0 then
			CGasFriendBasic.PrivateChatSendMsg(0,tonumber(ret),warning)
		end
	end
	return true, 200
end

function Gas2GasDef:GMSendMail(conn, role_name)
	local player = g_GetPlayerInfoByName(role_name)
	if player and player.m_Conn then
		SystemFriendMsgToClient(player.m_Conn, 5001)
	end
end

local function SendMail(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end	
	local role_names = _req["role_names"]
	local content		 = _req["content"]
	local money			 = tonumber(_req["money"])
	local items_info = _req["item_info"]
	
	local tbl_role_name = {}
	SplitString(role_names, ',', tbl_role_name)
	local error_role = ""
	for i=1,#tbl_role_name do
		local role_name = tbl_role_name[i]
		local SendMailData = {["role_name"] = role_name, ["content"] = content, ["money"] = money, ["items"] = items_info}
		CallDbTrans("GMToolsDbActionDB", "SendMail", CallBack, SendMailData)
		local result = coroutine.yield()
		if result ~= 0 then
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:GMSendMail(GetServerAutoConn(server_id),role_name)
		  end
		else
			error_role = error_role .. role_name
		end
	end
	if error_role == "" then
		return true, 200
	else
		return false, error_role
	end
end

local function FreezeUrs(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end

	local username = _req["urs"]
	local data ={}
	data["username"] = username
	data["game_id"] = tonumber(ERatingConfig.AGIP_GameID)
	CallDbTrans("GMToolsDbActionDB", "FreezeAccountDB", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return true, 500
	end
	local account=g_AccountMgr:GetAccountInfo(tonumber(ERatingConfig.AGIP_GameID), username )
	if account then
		account:Exit()
	end
	return true, 200
end

local function FreezeAccount(_req)
	local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

local function FastFreezeAccount(_req)
	local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

local function UnFreezeUrs(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end

	local username = _req["urs"]
	local data ={}
	data["username"] = username
	CallDbTrans("GMToolsDbActionDB", "CancelFreezeAccountDB", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return true, 500
	end
	return true, 200
end

local function UnfreezeAccount(_req)
	local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

local function FastUnfreezeAccount(_req)
	local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

local function DelItemInBag(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local item_type = _req["item_type"]
	local item_name = _req["item_name"]
	local item_num = tonumber(_req["item_num"])
	local role_name = _req["role_name"]
	local data = {["role_name"] = role_name,["item_type"] = item_type,["item_name"] = item_name, ["item_num"] = item_num}
	CallDbTrans("GMToolsDbActionDB", "DelItemInBag", CallBack, data)
	local ret = coroutine.yield()
	if not ret[1] then
		return false, 500
	end
	return true, 200
end

--------------------------------
local function DelItemOnBody(_req)
	--删除身上装备
	--先踢下线，再删除
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),_req["role_name"])
  end
  local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local data = {["role_name"] = _req["role_name"],["item_type"] = _req["item_type"],["item_name"] = _req["item_name"]}
	CallDbTrans("GMToolsDbActionDB", "DelItemOnBody", CallBack, data)
	local ret = coroutine.yield()
	if not ret[1] then
		return false,500
	end
  return true,200
end
-----------------------------

local function ChangeGMLevel(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local user_name = _req["user_name"]
	local gm_level = tonumber(_req["gm_level"])
	local game_id = tonumber(_req["game_id"])
	local data = {["gm_level"] = gm_level,["user_name"] = user_name,["game_id"] = game_id}
	CallDbTrans("GMToolsDbActionDB", "AddGMMgr", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function DelGMMmr(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local user_name = _req["user_name"]
	local game_id = tonumber(_req["game_id"])
	local data = {["user_name"] = user_name,["game_id"] = game_id}
	CallDbTrans("GMToolsDbActionDB", "DelGMMmr", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function DelCurrencyLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local money_func = _req["money_func"]
	local index1 = string.find(money_func,"+")
	local func_name = string.sub(money_func,1,index1-1)
	local mode_name = string.sub(money_func,index1+1,string.len(money_func))
	local data = {["func_name"] = func_name,["mode_name"] = mode_name}
	CallDbTrans("GMToolsDbActionDB", "DelCurrencyLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end
local function DelJifenUseLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local jifen_type = _req["jifen_type"]
	local data = {["jifen_type"] = jifen_type}
	CallDbTrans("GMToolsDbActionDB", "DelJifenUseLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end
local function DelItemUseLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local item_name = _req["item_name"]
	local data = {["item_name"] = item_name}
	CallDbTrans("GMToolsDbActionDB", "DelItemUseLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function ClearState(_req)
	local role_id = tonumber(_req["role_id"])
	local role_name = _req["role_name"]
	local state_name = _req["state_name"]
	role_id = role_id or 0
	local player = g_GetPlayerInfo(role_id)
	player = player or g_GetPlayerInfoByName(role_name)
	local flag = false
	if player then
		flag = player:EraseErasableState(name)
	end
	return flag, 200
end

local function DelItemInDepot(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	
	local item_type = tonumber(_req["item_type"])
	local item_name = _req["item_name"]
	local item_num = tonumber(_req["item_num"])
	local role_name = _req["role_name"]
	local data = {["role_name"] = role_name,["item_type"] = item_type,["item_name"] = item_name, ["item_num"] = item_num}
	CallDbTrans("GMToolsDbActionDB", "DelItemInDepot", CallBack, data)
	local ret = coroutine.yield()
	if not ret[1] then
		return false, 500
	end
	return true, 200
end

local function DelItemInTongDepot(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	
	local page = tonumber(_req["page"])
	local pos = _req["pos"]
	local tong_name = _req["tong_name"]
	local msg = _req["msg"]
	local data = {["page"] = page,["pos"] = pos,["tong_name"] = tong_name, ["msg"] = msg}
	CallDbTrans("GMToolsDbActionDB", "DelItemInTongDepot", CallBack, data)
	local ret = coroutine.yield()
	if not ret[1] then
		return false, 500
	end
	return true, 200
end

local function RepeatSendGlobalAnousment(tick,data)
	local content,endTime = unpack(data)
	if os.time() < tonumber(endTime) then
		
		for server_id in pairs(g_ServerList) do
			Gas2GasAutoCall:SendGlobalAnousment(GetServerAutoConn(server_id),content)
		end
	else
		UnRegisterTick(g_GMSendGlobalAnousmentTick)
		g_GMSendGlobalAnousmentTick = nil
	end
end

function Gas2GasDef:SendGlobalAnousment(conn, msg)
	Gas2Gac:SysMovementNotifyToClient(g_AllPlayerSender,msg)	
	MsgToConn(g_AllPlayerSender,208,msg)
end

local function Anousement(_req)
	local content = _req["content"]
	local tickTime = _req["tickTime"]
	local endTime = _req["endTime"]
	content = string.gsub(content, "\\\"", "\"")
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:SendGlobalAnousment(GetServerAutoConn(server_id),content)
	end
	if tickTime then
		g_GMSendGlobalAnousmentTick = RegisterTick("GMSendGlobalAnousmentTick", RepeatSendGlobalAnousment, tickTime,{content,endTime})
	end
	return true, 200
end

local binding_type = {[0] = "不绑定",[1] = "装备绑定",[2] = "绑定"}
local function GetEquipColor(Quality,nAdvance)
	local qualityIndex =  Quality 
	local quality_color = {
		[0] = "灰色[稀0]",
		[1] = "白色[稀1]",
		[2] = "绿色[稀2]",
		[3] = "蓝色[稀3]",
		[4] = "紫色[稀4]",
		[5] = "橙色[稀5]",
		[6] = "黄色[稀6]",
		[7] = "蓝绿色[稀7]",
	}
	local sColor = "无"
	if  nAdvance >= 2 and nAdvance < 4 then
			sColor = "深绿色[2-3阶段]"
	elseif nAdvance >= 4 and nAdvance < 7 then
			sColor = "深蓝色[4-6阶段]"
	elseif nAdvance == 7 then
			sColor = "橙色[7阶段]"
	else
			sColor = quality_color[qualityIndex]
	end
	return sColor
end

local function GetStrByEquipType(nCharID,nItemID,equip_info)
	local sStr = ""
	local item_type,item_name = equip_info.item_type,equip_info.item_name
	local Quality = g_ItemInfoMgr:GetItemInfo(item_type,item_name,"Quality")
	local equip_mgr = g_GetEquipMgr()
	local equip = equip_mgr:GetEquipFromRet(nItemID, nCharID,item_type,item_name,equip_info.m_tblEquipInfo)
	if equip == nil then
		return "查询错误"
	end
	local sColor = GetEquipColor(Quality,equip.CurAdvancePhase)
	local view_name = equip.ViewName
	if "" == view_name then
		view_name = item_name
	end
	if 5 == item_type then
		sStr = "<tr><th>名称</th><th>显示名称</th><th>蓝紫等</th><th>绑定类型</th><th>基础等级</th><th>当前等级</th><th>DPS</th><th>攻击速度</th><th>当前耐久</th><th>最大耐久</th></tr>\n"
		sStr = sStr .. "<tr><td>" .. (item_name or "-") .. "</td><td>" .. (view_name or "-") .."</td><td>" .. (sColor or "-") .. "</td><td>" .. (binding_type[equip.BindingType] or "-") .. "</td><td>" .. (equip.BaseLevel or "-") .. "</td><td>" .. (equip.CurLevel or "-") .. "</td><td>" .. (equip.DPS or "-") .. "</td><td>" .. (equip.AttackSpeed or "-")
						.. "</td><td>" .. (equip.CurDuraValue or "-") .. "</td><td>" .. (equip.MaxDuraValue or "-") .. "</td></tr>\n"
	elseif 6 == item_type or item_type == 9 then
		sStr = "<tr><th>名称</th><th>显示名称[ID]</th><th>蓝紫等</th><th>绑定类型</th><th>基础等级</th><th>当前等级</th><th>当前耐久</th><th>最大耐久</th></tr>\n"
		sStr = sStr .. "<tr><td>" .. (item_name or "-") .. "</td><td>" .. (view_name or "-") .."</td><td>" .. (sColor or "-") .. "</td><td>" .. (binding_type[equip.BindingType] or "-") .. "</td><td>" .. (equip.BaseLevel or "-") .. "</td><td>" .. (equip.CurLevel or "-")
						.. "</td><td>" .. (equip.CurDuraValue or "-") .. "</td><td>" .. (equip.MaxDuraValue or "-") .. "</td></tr>\n"
	elseif 7 == item_type then
		sStr = "<tr><th>名称</th><th>显示名称[ID]</th><th>蓝紫等</th><th>绑定类型</th><th>基础等级</th><th>当前等级</th><th>当前耐久</th><th>最大耐久</th></tr>\n"
		sStr = sStr .. "<tr><td>" .. (item_name or "-") .. "</td><td>" .. (view_name or "-") .."</td><td>" .. (sColor or "-") .. "</td><td>" .. (binding_type[equip.BindingType] or "-") .. "</td><td>" .. (equip.BaseLevel or "-") .. "</td><td>" .. (equip.CurLevel or "-")
						.. "</td><td>" .. (equip.CurDuraValue or "-") .. "</td><td>" .. (equip.MaxDuraValue or "-") .. "</td></tr>\n"
	elseif item_type == 8 then
		sStr = "<tr><th>名称</th><th>显示名称[ID]</th><th>蓝紫等</th><th>绑定类型</th><th>基础等级</th><th>当前等级</th><th>DPS</th><th>当前耐久</th><th>最大耐久</th></tr>\n"
		sStr = sStr .. "<tr><td>" .. (item_name or "-") .. "</td><td>" .. (view_name or "-") .."</td><td>" .. (sColor or "-") .. "</td><td>" .. (binding_type[equip.BindingType] or "-") .. "</td><td>" .. (equip.BaseLevel or "-") .. "</td><td>" .. (equip.CurLevel or "-") .. "</td><td>" .. (equip.DPS or "-")
						.. "</td><td>" .. (equip.CurDuraValue or "-") .. "</td><td>" .. (equip.MaxDuraValue or "-") .. "</td></tr>\n"
	end
	return sStr
end

local function GetEquipByID(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local item_id = tonumber(_req["item_id"])
	local nCharID = tonumber(_req["role_id"])
	local data = {["item_id"] = item_id}
	CallDbTrans("GMToolsDbActionDB", "GetEquipItemOnBody", CallBack, data)
	local ret = coroutine.yield()
	local ret_str = "<html><title>该装备详细信息如下</title><head></head>\n"
	ret_str = ret_str .. "<body>"
	ret_str = ret_str .. "<table border=1>"
	ret_str = ret_str .. GetStrByEquipType(nCharID,item_id,ret)
	ret_str = ret_str .. "</table>\n"
	ret_str = ret_str .. "</body></html>\n"
	return true,ret_str
end

local function AddQuestFinishLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local quest_name = _req["quest_name"]
	local data = {["quest_name"] = quest_name}
	CallDbTrans("GMToolsDbActionDB", "AddQuestFinishLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function AddObjDropLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local quest_name = _req["obj_name"]
	local data = {["obj_name"] = quest_name}
	CallDbTrans("GMToolsDbActionDB", "AddObjDropLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function DelQuestFinishLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local quest_name = _req["quest_name"]
	local data = {["quest_name"] = quest_name}
	CallDbTrans("GMToolsDbActionDB", "DelQuestFinishLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function DelObjDropLimit(_req)
	local co=coroutine.running()
	local function CallBack(result)
		coroutine.resume(co,result)
	end
	local quest_name = _req["obj_name"]
	local data = {["obj_name"] = quest_name}
	CallDbTrans("GMToolsDbActionDB", "DelObjDropLimit", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end


local function HotUpdate(_req)	
	local type = tonumber(_req["type"])
	local filename = _req["filename"]
	local md5 = _req["md5"]
	local code = _req["code"]
	code = string.gsub(code, "\\\"", "\"")
	code = string.gsub(code, "\\'", "'")
	code = string.gsub(code, "\\\\", "\\")
	local revision = GetCurrentRevision()
	
	local MD5Digester = CMd5Digester:new()
	MD5Digester:Begin()
	MD5Digester:Append(code, string.len(code))
	local CODE_MD5 = CreateCBufAcr( CreateCBuf(33) )
	MD5Digester:GetMD5Base16Str(CODE_MD5)
	local code_md5 = CODE_MD5:ToString()
	
	if code_md5 ~= md5 then
		return false, -201
	end
	
	if filename then
		filename = string.gsub(filename, "\\\"", "\"")
		local data = { ["type"] = type, ["revision"] = revision, ["filename"] = filename, ["code"] = code }
		CallDbTrans("GMToolsDbActionDB", "AddHotUpdateCode", nil, data)
	end	
	return true, 200
end

local function RoleRename(_req)
	local role_name = _req["role_name"]
	local new_name = _req["new_name"]
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:KickOffRole(GetServerAutoConn(server_id),role_name)
  end
  local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = {["role_name"] = role_name,["new_name"] = new_name}
	CallDbTrans("GMToolsDbActionDB", "RoleRename", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function TongRename(_req)
	local tong_id = _req["id"]
	local new_name = _req["new_name"]
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = {["tong_id"] = tong_id,["new_name"] = new_name}
	CallDbTrans("GMToolsDbActionDB", "TongRename", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function RemoveTongMember(_req)
	local tong_id = _req["tong_id"]
	local role_id = _req["role_id"]
	local msg = _req["msg"]
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = {["tong_id"] = tong_id,["role_id"] = role_id, ["msg"] = msg}
	CallDbTrans("GMToolsDbActionDB", "RemoveTongMember", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function ChangeTongMember(_req)
	local tong_id = _req["tong_id"]
	local role_id = _req["role_id"]
	local position = _req["position"]
	local msg = _req["msg"]
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = {["tong_id"] = tong_id,["role_id"] = role_id,["position"] = position, ["msg"] = msg}
	CallDbTrans("GMToolsDbActionDB", "ChangeTongMember", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function ChangeTongLevel(_req)
	local tong_id = _req["id"]
	local level = _req["level"]
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = {["tong_id"] = tong_id,["level"] = level}
	CallDbTrans("GMToolsDbActionDB", "ChangeTongLevel", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function GetBackEquip(_req)
	local type_handle = {}
	type_handle[5] = "GetBackWeapon"
	type_handle[6] = "GetBackArmor"
	type_handle[7] = "GetBackShield"
	type_handle[8] = "GetBackRing"
	type_handle[9] = "GetBackArmor"
	
	local co = coroutine.running()
	
	local function CallBack(ret)
		coroutine.resume(co, ret)
	end
	local data = _req
	data["title"]	= (_req["title"] or "-")
	data["content"]	= (_req["content"] or "-")
	data["money"]		= (_req["money"] or 0)
	CallDbTrans("GMToolsDbActionDB", type_handle[tonumber(_req["item_type"])], CallBack, data)
	local ret = coroutine.yield()
	if ret == 0 then
		return false, 500
	end
	return true, 200
end

local function DelChar(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = _req
	CallDbTrans("GMToolsDbActionDB", "DelChar", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function GetBackRole(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = _req
	CallDbTrans("GMToolsDbActionDB", "GetBackRole", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function ClearItemInBag(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = _req
	CallDbTrans("GMToolsDbActionDB", "ClearItemInBag", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function ClearItemInDepot(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	local data = _req
	CallDbTrans("GMToolsDbActionDB", "ClearItemInDepot", CallBack, data)
	local ret = coroutine.yield()
	if not ret then
		return false, 500
	end
	return true, 200
end

local function GetServerDate(_req)
	return true, os.date("%Y/%m/%d %H:%M:%S",os.time())
end

local function GetRoleRealTimeData(_req)
	local char_name = _req["role_name"]
	local ret = "<root>\n";
	local player = g_GetPlayerInfoByName(char_name)
	if player ~= nil then
		ret = ret .. "<HP>" 												.. player:CppGetHP() .. "/" .. player:CppGetMaxHP() 							 .. "</HP>\n"
		ret = ret .. "<MP>" 												.. player:CppGetMP() .. "/" .. player:CppGetMaxMP() 							 .. "</MP>\n"
		if player:CppGetPKState() then
			ret = ret .. "<PKState>1</PKState>\n"
		else
			ret = ret .. "<PKState>2</PKState>\n"
		end
		ret = ret .. "<PhysicalDPS>"								.. player:CppGetPropertyValueByName(GetPropertyName("攻击力"))		 .. "</PhysicalDPS>\n"
		ret = ret .. "<AccuratenessValue>"					.. player:CppGetPropertyValueByName(GetPropertyName("精准值"))		 .. "</AccuratenessValue>\n"
		ret = ret .. "<StrikeValue>"								.. player:CppGetPropertyValueByName(GetPropertyName("暴击值"))		 .. "</StrikeValue>\n"
		ret = ret .. "<CRIMax>"										  .. player:CppGetPropertyValueByName(GetPropertyName("暴击上限"))	 .. "</CRIMax>\n"
		ret = ret .. "<MagicHitValue>"							.. player:CppGetPropertyValueByName(GetPropertyName("法术命中"))	 .. "</MagicHitValue>\n"
		ret = ret .. "<ResilienceValue>"						.. player:CppGetPropertyValueByName(GetPropertyName("韧性值"))		 .. "</ResilienceValue>\n"
		ret = ret .. "<Defence>"										.. player:CppGetPropertyValueByName(GetPropertyName("护甲值"))		 .. "</Defence>\n"
		ret = ret .. "<PhysicalDodgeValue>"				  .. player:CppGetPropertyValueByName(GetPropertyName("物理躲避值")) .. "</PhysicalDodgeValue>\n"
		ret = ret .. "<ParryValue>"								  .. player:CppGetPropertyValueByName(GetPropertyName("招架值"))		 .. "</ParryValue>\n"
		ret = ret .. "<MagicDodgeValue>"						.. player:CppGetPropertyValueByName(GetPropertyName("法术躲避值")) .. "</MagicDodgeValue>\n"
		ret = ret .. "<NatureResistanceValue>"			.. player:CppGetPropertyValueByName(GetPropertyName("自然抗值"))	 .. "</NatureResistanceValue>\n"
		ret = ret .. "<DestructionResistanceValue>" .. player:CppGetPropertyValueByName(GetPropertyName("破坏抗值"))	 .. "</DestructionResistanceValue>\n"
		ret = ret .. "<EvilResistanceValue>"				.. player:CppGetPropertyValueByName(GetPropertyName("暗黑抗值"))	 .. "</EvilResistanceValue>\n"
		ret = ret .. "<MagicDamageValue>"					  .. player:CppGetPropertyValueByName(GetPropertyName("法伤"))			 .. "</MagicDamageValue>\n"
	else
		ret = ret .. "<no-player/>\n";
	end
	ret = ret .. "</root>\n"
	return true, ret
end

local function GetNpcInfo(_req)
	local NpcName = _req["npc_name"]
	local ret = "<root>\n"
	ret = ret .. "<npc>\n"
	ret = ret .. "<npc-name>" .. NpcName .. "</npc-name>\n"
	ret = ret .. "<npc-camp>" .. Npc_Common(NpcName,"Camp") .. "</npc-camp>\n"
	
	local NpcFuncName = Npc_Common(NpcName,"FuncName")
	if string.find(NpcFuncName, "任务NPC", 1, true) then
		ret = ret .. "<npc-quests>\n"
		local QPosKeys = QuestPos_Common:GetKeys()
		for j=1, table.getn(QPosKeys) do
			local QPosId = QPosKeys[j]
			local QPosMark = QuestPos_Common(QPosId, "Mark")
			if QPosMark == NpcName then
				ret = ret .. "<npc-quest-pos scene=\"" .. QuestPos_Common(QPosId, "SceneName") .. "\" x=\"" .. QuestPos_Common(QPosId, "PosX") .. "\" y=\"" .. QuestPos_Common(QPosId, "PosY") .. "\"/>\n"
			end
		end
		local QuestKeys = Quest_Common:GetKeys()
		for i=1, table.getn(QuestKeys) do
			local QuestName = QuestKeys[i]
			local QuestNpcName = Quest_Common(QuestName,"触发任务方式","Arg")
			if QuestNpcName == NpcName then
				ret = ret .. "<npc-quest>" .. QuestName .. "</npc-quest>\n"
			end
		end
		ret = ret .. "</npc-quests>\n"
	end
	if string.find(NpcFuncName, "商店", 1, true) then
		if NpcShop_Common(NpcName)~=nil then
			ret = ret .. "<npc-shop>\n"
			ret = ret .. "<npc-shop-pay>" .. NpcShop_Common(NpcName,"PayType") .. "</npc-shop-pay>\n"
			ret = ret .. "<npc-shop-items>\n"
			for j =1, 150 do
				local itemTypeStr = "ItemType" .. j
				local itemNameStr = "ItemName" .. j
				local itemType = NpcShop_Common(NpcName, itemTypeStr)
				local itemName = NpcShop_Common(NpcName, itemNameStr)
				if itemType == "" or itemType == nil  then
					break
				end
				ret = ret .. "<npc-shop-item type=\"" .. itemType .. "\">" .. itemName .. "</npc-shop-item>\n"
			end
			ret = ret .. "</npc-shop-items>\n"
			ret = ret .. "</npc-shop>\n"
		end
	end
	if NpcDropItemObj_Server(NpcName) ~= nil then
		ret = ret .. "<npc-drop>\n"
		local NpcDropNotes = NpcDropItemObj_Server(NpcName,"NOTES2")
		if NpcDropNotes ~= "" then
			ret = ret .. "<npc-drop-scene>" .. NpcDropNotes .. "</npc-drop-scene>\n"
		end
		local NpcDropLevel = NpcDropItemObj_Server(NpcName,"Level")
		if NpcDropLevel ~= "" then
			ret = ret .. "<npc-drop-level>" .. NpcDropLevel .. "</npc-drop-level>\n"
		end
		for j = 1, 100 do
			local team  = NpcDropItemObj_Server(NpcName,"DropTeam" .. j)
			if team ~= nil and team~="" then
				local rate  = NpcDropItemObj_Server(NpcName,"DropTeamRate" .. j)
				
				local function NpcDropTeam(team,rate)
					ret = ret .. "<npc-drop-team rate=\"" .. rate .. "\">\n"
					if DropItemObjTeam_Server(team) ~= nil then
						ret = ret .. "<npc-drop-team-num max=\"" .. DropItemObjTeam_Server(team,"DropMaxNum") .. "\" min=\"" .. DropItemObjTeam_Server(team,"DropMinNum") .. "\" />\n"
						for k = 1, 30 do
							local item_name = DropItemObjTeam_Server(team,"DropName" .. k)
							if item_name ~= nil and item_name ~= "" then
								ret = ret .. "<npc-drop-team-item rate=\"" .. DropItemObjTeam_Server(team,"DropRate" .. k) .. "\">" .. item_name .. "</npc-drop-team-item>\n"
							end
						end
					end
					ret = ret .. "</npc-drop-team>\n"
				end
				
				if string.find(team, "#class#") then	
					local ClassNameTbl = {"大剑","魔剑","法师","邪魔","牧师","兽战",}
					for _, v in pairs(ClassNameTbl) do
						teamindex = string.gsub(team, "#class#", v)
						NpcDropTeam(teamindex,rate)
					end
				end
				
				if string.find(team, "#camp#") then	
					local CampNameTbl = {"艾米","神圣","修斯",}
					for _, v in pairs(CampNameTbl) do
						teamindex = string.gsub(team, "#camp#", v)
						NpcDropTeam(teamindex,rate)
					end
				end		
				
				if string.find(team, "#class#") == nil and string.find(team, "#camp#") == nil then
					if DropItemObjTeam_Server(team) ~= nil then
						NpcDropTeam(team,rate)
					end
				end
			end
		end
		ret = ret .. "</npc-drop>\n"
	end
	
	ret = ret .. "</npc>\n"
	ret = ret .. "</root>\n"
	return true, ret
end

local function TrusteeUser(_req)
	local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

local function UnTrusteeUser(_req)
	local str = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
	str = str .. "<Response>\n"
	str = str .. "<result>1</result>\n"
	str = str .. "</Response>"
	return true, str
end

local function ClearChannelChat(_req)
	CallDbTrans("GMToolsDbActionDB", "ClearChannelChat", nil, _req)
	return true, 200
end

local function GetItemCfg(_req)
	local display = _req["name"]
	local columns = {}
	if _req["columns"] then
		SplitString(_req["columns"], ',', columns)
	end
	local ret = "";
	if display then
		local items = g_ItemInfoMgr:GetItemInfoByDisplayName(display)
		for j,k in pairs(items) do
			local ItemType = k[1]
			local ItemName = k[2]
			ret = ret .. "	<Item>\n"
			ret = ret .. "		<Name>" .. ItemName .. "</Name>\n"
			ret = ret .. "		<Type>" .. ItemType .. "</Type>\n"
			for i,n in pairs(columns) do
				local column = n
				local ItemValue = g_ItemInfoMgr:GetItemInfo(ItemType, ItemName, column)
				ret = ret .. "		<" .. column .. ">" .. ItemValue .. "</" .. column .. ">\n"
			end
			ret = ret .. "	</Item>\n"
		end
	end
	ret = "<root>\n" .. ret .. "</root>\n"
	return true, ret;
end

function GetTaskCfg(_req)
	local tbl = {}
	for i=1, 3 do
		table.insert(tbl,"quest/Lan_QuestAll_CommonA"..i)
		table.insert(tbl,"quest/Lan_QuestAll_CommonB"..i)
		table.insert(tbl,"quest/Lan_QuestAll_CommonC"..i)
	end
	
	table.insert(tbl,"quest/Lan_CallBack_Quest_Common")
	table.insert(tbl,"quest/Lan_ZhiYin_Quest_Common")
	
	table.insert(tbl,"quest/Lan_Hide_Quest_Common")
	table.insert(tbl,"quest/Lan_TreeBar_Quest_Common")
	table.insert(tbl,"quest/Lan_HeYangDaCaoYuan_Quest_Common")
	table.insert(tbl,"quest/Lan_ShengXueShan_Quest_Common")
	table.insert(tbl,"quest/Lan_ShiZiHeGu_Quest_Common")
	table.insert(tbl,"quest/Lan_XiLinDao_Quest_Common")
	table.insert(tbl,"quest/Lan_MercenaryQuest_Direct_Common")
	table.insert(tbl,"quest/Lan_MercenaryQuest_XiLinDao_CommonA")
	table.insert(tbl,"quest/Lan_MercenaryQuest_XiLinDao_CommonB")
	table.insert(tbl,"quest/Lan_MercenaryQuest_XiLinDao_CommonC")
	
	for i=1, 5 do
		table.insert(tbl,"quest/Lan_MercenaryQuest_CommonA"..i)
		table.insert(tbl,"quest/Lan_MercenaryQuest_CommonB"..i)
		table.insert(tbl,"quest/Lan_MercenaryQuest_CommonC"..i)
	end
	
	for i=1, 3 do
		table.insert(tbl,"quest/Lan_DareQuest_Common"..i)
		table.insert(tbl,"quest/Lan_ActionQuest_Common"..i)
		table.insert(tbl,"quest/Lan_LoopQuest_Common"..i)
	end
	lan_load_list("Lan_QuestCommon", unpack(tbl))
	local ret = ""
	local Keys = Lan_QuestCommon:GetKeys()
	--print(Lan_QuestCommon(Keys[1], "NewContent"))
	for i=1, table.getn(Keys) do
		if string.find(Lan_QuestCommon(Keys[i], "NewContent"), _req["name"], 1, true) then
		--if _req["name"] == Lan_QuestCommon(Keys[i], "NewContent") then
			local longName = MemH64ToMbcs(Keys[i])
			if( "任务名" == string.sub( longName, -6 ) ) then
				local mh64Name = MemH64( string.sub( longName, 1, -7) )
				ret = ret .. "	<task>\n"
				ret = ret .. "		<name>" .. string.sub( longName, 1, -7) .. "</name>\n";
				ret = ret .. "		<id>" .. mh64Name .. "</id>\n"
				ret = ret .. "	</task>\n"
			end
		end
	end
	ret = "<root>\n" .. ret .. "</root>\n"
	return true, ret
end

function GetNPCCfg(_req)
	local npc_name = _req["name"]
	local NpcNameTbl = {}
	for _, NpcName in pairs(Npc_Common:GetKeys()) do
		local ShowName = Lan_Npc_Common(MemH64(NpcName),"Title")	
	--for NpcName,v in pairs(Npc_Common) do
		if ShowName == npc_name then
			table.insert(NpcNameTbl,NpcName)
		end
	end
	local ret = "";
	for i,j in pairs(NpcNameTbl) do
		ret = ret .. "	<npc>" .. j .. "</npc>\n"
	end
	ret = "<root>\n" .. ret .. "</root>\n"
	return true, ret
end

function GetItemShowName(_req)
	local ret = "";
	local ItemInfo = g_ItemInfoMgr:GetItemInfoByIndex(_req["name"])
	if ItemInfo ~= nil then
		local ItemLanInfo = g_ItemInfoMgr:GetItemLanInfo(ItemInfo[1], ItemInfo[2], "DisplayName")
		if ItemLanInfo ~= nil then
			ret = ret .. "<item>" .. ItemLanInfo .. "</item>\n"
		end
	end
	ret = "<root>\n" .. ret .. "</root>\n"
	return true, ret
end

function Gas2GasDef:SetServerType(conn, type)
	g_ServerType = type
end

function Gas2GasDef:SetBuyCouponsAddress(conn)
	GetBuyCouponsWebInfo()
end

function SetConfServer(_req)
	local function CallBack(ret)
		if _req["name"] == "ServerType" then
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:SetServerType(GetServerAutoConn(server_id),tonumber(_req["value"]))
			end
		end
		if _req["name"] == "BuyCouponsWebIP" or _req["name"] == "BuyCouponsWebPort" then
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:SetBuyCouponsAddress(GetServerAutoConn(server_id))
			end
		end
	end
	CallDbTrans("GMToolsDbActionDB", "SetConfServer", CallBack, _req)
	return true, 200
end

function Gas2GasDef:UpdateJointSalesList(conn)
	UpdateCapableOfBuyingCouponsInGas()
end

function UpdateJointSalesList(_req)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:UpdateJointSalesList(GetServerAutoConn(server_id))
	end
	return true, 200
end

function SetRecruitMethod(_req)
	Gas2GasAutoCall:SetRecruitMethod(GetServerAutoConn(1),tonumber(_req["method"]),tonumber(_req["camp1"]),tonumber(_req["camp2"]),tonumber(_req["camp3"]))
	return true, 200
end

function SetRecruitInfo(_req)
	CallDbTrans("GMToolsDbActionDB", "UpdateRecruitInfo", nil, _req)
	return true, 200
end

function SetMaxOnLineUserNum(_req)
	if _req["type"] == "0" then --单服
		CallDbTrans("LoginQueueDB", "SetMaxOnLineUserNum", nil, _req)
	end
	if _req["type"] == "1" then --多服
		CallDbTrans("LoginQueueDB", "SetAllServerMaxUserNum", nil, _req)
	end
	return true, 200
end

function TempBackData(_req)
	CallDbTrans("GMToolsDbActionDB", "TempBackData", nil, {})
end

function GMUserTrust(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	UserTrust(_req["role_name"], _req["password"], _req["time"], CallBack)
	local ret = coroutine.yield()
	if ret ~= 1 then
		return false, ret
	end
	return true, 200
end

function GMCancelUserTrust(_req)
	local co = coroutine.running()
	local function CallBack(result)
		coroutine.resume(co, result)
	end
	CancelUserTrust(_req["role_name"], CallBack)
	local ret = coroutine.yield()
	if ret ~= 1 then
		return false, ret
	end
	return true, 200
end

g_WebFuncDef["ShutUp"]=ShutUp
g_WebFuncDef["CancelShutUp"]=CancelShutUp
g_WebFuncDef["KickOff"]=KickOff
g_WebFuncDef["KickRole"]=KickRole
g_WebFuncDef["MoveRole"]=MoveRole
g_WebFuncDef["SendWarning"]=SendWarning
g_WebFuncDef["SendMail"]=SendMail
g_WebFuncDef["FreezeUrs"]=FreezeUrs
g_WebFuncDef["UnFreezeUrs"]=UnFreezeUrs
g_WebFuncDef["FreezeAccount"]=FreezeAccount
g_WebFuncDef["UnfreezeAccount"]=UnfreezeAccount
g_WebFuncDef["FastFreezeAccount"]=FastFreezeAccount
g_WebFuncDef["FastUnfreezeAccount"]=FastUnfreezeAccount
g_WebFuncDef["DelItemInBag"]=DelItemInBag
g_WebFuncDef["DelItemOnBody"]=DelItemOnBody
g_WebFuncDef["ChangeGMLevel"]=ChangeGMLevel
g_WebFuncDef["DelGMMmr"]=DelGMMmr
g_WebFuncDef["ClearState"]=ClearState
g_WebFuncDef["DelCurrencyLimit"]=DelCurrencyLimit
g_WebFuncDef["DelJifenUseLimit"]=DelJifenUseLimit
g_WebFuncDef["DelItemUseLimit"]=DelItemUseLimit
g_WebFuncDef["SendAnousment"]=Anousement
g_WebFuncDef["ItemNameMatch"]=ItemNameMatch
g_WebFuncDef["QuestNameMatch"]=QuestNameMatch
g_WebFuncDef["DelItemInDepot"]=DelItemInDepot
g_WebFuncDef["DelItemInTongDepot"]=DelItemInTongDepot
g_WebFuncDef["GetRoleEquipByID"]=GetEquipByID
g_WebFuncDef["AddQuestFinishLimit"]=AddQuestFinishLimit
g_WebFuncDef["AddObjDropLimit"]=AddObjDropLimit
g_WebFuncDef["DelQuestFinishLimit"]=DelQuestFinishLimit
g_WebFuncDef["DelObjDropLimit"]=DelObjDropLimit
g_WebFuncDef["HotUpdate"]=HotUpdate
g_WebFuncDef["RoleRename"]=RoleRename
g_WebFuncDef["TongRename"]=TongRename
g_WebFuncDef["RemoveTongMember"]=RemoveTongMember
g_WebFuncDef["ChangeTongMember"]=ChangeTongMember
g_WebFuncDef["ChangeTongLevel"] = ChangeTongLevel
g_WebFuncDef["GetBackEquip"] = GetBackEquip
g_WebFuncDef["DelChar"] = DelChar
g_WebFuncDef["GetBackRole"] = GetBackRole
g_WebFuncDef["ClearItemInBag"] = ClearItemInBag
g_WebFuncDef["ClearItemInDepot"] = ClearItemInDepot
g_WebFuncDef["GetServerDate"] = GetServerDate
g_WebFuncDef["GetNpcInfo"] = GetNpcInfo
g_WebFuncDef["GetRoleRealTimeData"] = GetRoleRealTimeData
g_WebFuncDef["TrusteeUser"] = TrusteeUser
g_WebFuncDef["UnTrusteeUser"] = UnTrusteeUser
g_WebFuncDef["ClearChannelChat"] = ClearChannelChat
g_WebFuncDef["GetItemCfg"] = GetItemCfg
g_WebFuncDef["GetTaskCfg"] = GetTaskCfg
g_WebFuncDef["GetNPCCfg"] = GetNPCCfg
g_WebFuncDef["GetItemShowName"] = GetItemShowName
g_WebFuncDef["SetConfServer"] = SetConfServer
g_WebFuncDef["UpdateJointSalesList"] = UpdateJointSalesList
g_WebFuncDef["SetRecruitMethod"] = SetRecruitMethod
g_WebFuncDef["SetRecruitInfo"] = SetRecruitInfo
g_WebFuncDef["SetMaxOnLineUserNum"] = SetMaxOnLineUserNum
g_WebFuncDef["TempBackData"] = TempBackData
g_WebFuncDef["UserTrust"] = GMUserTrust
g_WebFuncDef["CancelUserTrust"] = GMCancelUserTrust