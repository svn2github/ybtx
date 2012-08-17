local QuestCoroutine = {}
setmetatable(QuestCoroutine, { __mode = "v" })
local QuestResult = {}
local QuestId = 0

local function WaitEnd(questId)
	local co = QuestCoroutine[questId]
	QuestResult[questId] = nil
	if co then
		coroutine.resume(co,1)
	end
end

function Gas2GasDef:ReceiveQuestNpcPosition(Conn, serverId, questId, result)
	if QuestResult[questId] and QuestCoroutine[questId] then
		QuestResult[questId][serverId] = result
		local isFinish = true
		for id in pairs(g_ServerList) do
			if not QuestResult[questId][serverId] then
				isFinish = false
				break
			end
		end
		if isFinish then
			if coroutine.status (QuestCoroutine[questId]) == "suspended" then
				coroutine.resume(QuestCoroutine[questId],2)
			end
		end
	end
end

function Gas2GasDef:QuestNpcPosition(Conn, questId, npc_name)
	local scene_info = {}
	local scene_name, x_pos, y_pos
	for i,p in pairs(g_SceneMgr.m_tbl_Scene) do
		if g_NpcBornMgr._m_AllNpc[i] then
			for _,Npc in pairs(g_NpcBornMgr._m_AllNpc[i]) do
				if Npc.m_Properties:GetCharName() == npc_name then
					local pos = CPos:new() 
					Npc:GetGridPos(pos)
					table.insert(scene_info, "{\"" .. p.m_SceneName .. "\"," ..  pos.x .. "," .. pos.y .. "},")
				end
			end
		end
	end
	local tbl_str = ""
	for i = 1, #scene_info do
		tbl_str = tbl_str .. scene_info[i]
	end
	Gas2GasAutoCall:ReceiveQuestNpcPosition(Conn, g_CurServerId, questId, tbl_str)
end

local function NpcPosition(_req)
	local npc_name = _req["npc_name"]
	local npc = Npc_Common(npc_name)
	if not npc then
		return true, "npc not exist"
	end
	
	local co = coroutine.running()
	QuestId = QuestId + 1
	local questId = QuestId
	QuestCoroutine[questId] = co
	QuestResult[questId] = {}

	local serverNum = 0
	for serverId in pairs(g_ServerList) do
		serverNum = serverNum + 1
		Gas2GasAutoCall:QuestNpcPosition(GetServerAutoConn(serverId), questId, npc_name)
	end

	local type = 2
	if serverNum > 1 then   --多服才会有异步查询
		RegisterOnceTick(g_App, "QuestNpcPosition", WaitEnd, 2000, questId)
		type = coroutine.yield()
	end
	
	if type == 1 then
		return true, "Query Time Out"
	elseif type ~= 2 then
		return true, "Query Failed"
	end
	
	local tbl_str = ""
	for _, v in pairs(QuestResult[questId]) do
		tbl_str = tbl_str .. v
	end
	
	QuestResult[questId] = nil
	QuestCoroutine[questId] = nil

	if tbl_str == "" then
		return true, "NPC not found"
	end
	
	local scene_info = loadstring("return {" .. tbl_str .. "}")()
	local strResult = ""
	for i = 1, #scene_info do
		strResult = strResult .. string.format("%s,%s,%s;", unpack(scene_info[i]))
	end
	return true, strResult
end

local function l_getMatchedNpcName(str)
	local NpcNameTbl = {}
	for _, NpcName in pairs(Npc_Common:GetKeys()) do
	--for NpcName,v in pairs(Npc_Common) do
		if string.find(NpcName,str) then
			local ShowName = Lan_Npc_Common(MemH64(NpcName),"Title")
			table.insert(NpcNameTbl,{NpcName,ShowName})
		end
	end
	return NpcNameTbl
end

local function NpcNameMatch(_req)
	local npc_name = _req["npc_name"]
	if not npc_name then
		return true, "<root></root>"
	end
	local tbl_Npc = l_getMatchedNpcName(npc_name)
	local ret_xml = ""
	ret_xml = ret_xml .. "<root>\n"
	for i,v in pairs(tbl_Npc) do
		ret_xml = ret_xml .. "<npc npc_name=\"" .. v[1] .. "\"></npc>\n"
	end
	ret_xml = ret_xml .. "</root>\n"
	return true, ret_xml
end

local function NpcDrop(_req)
	local npc_name = _req["npc_name"]
	local npc = Npc_Common(npc_name)
	if not npc then
		return false, "no_npc"
	end

	if not g_NpcDropObjTable[npc_name] then
		return false, "no_drop"
	end

	local str = "<root>\n"
	for i, v in pairs(g_NpcDropObjTable[npc_name]) do
		local team, rate = v["DropTeam"], v["DropRateStr"]
		
		if string.find(team, "#class#") then	
			local ClassNameTbl = {"大剑","魔剑","骑士","法师","邪魔","牧师","矮骑","精灵弓","兽战",}
			for i, v in pairs(ClassNameTbl) do
				teamindex = string.gsub(team, "#class#", v)
				str = str .. NpcDropReq(teamindex,rate)
			end
		end
		
		if string.find(team, "#camp#") then	
			local CampNameTbl = {"艾米","神圣","修斯",}
			for i, v in pairs(CampNameTbl) do
				teamindex = string.gsub(team, "#camp#", v)
				str = str .. NpcDropReq(teamindex,rate)
			end
		end		
		
		if string.find(team, "#class#") ~= nil and string.find(team, "#camp#") ~= nil then
			str = str .. NpcDropReq(team,rate)
		end
	end
	return true, str .. "</root>\n"
end

function NpcDropReq(team,rate)
	local idx = 1
	local str = ""
	str = str .. "<group name=\"" .. team .. "\" rate=\"" .. rate .. "\">\n"
	local ItemTable = DropItemObjTeam_Server(team)
	if ItemTable ~= nil then
		while true do
			local tbl_str = "<item name=\"%s\" rate=\"%s\" />\n"
			local DropName = ItemTable("DropName"..idx)
			local DropRate = ItemTable("DropRate"..idx)
			if DropName == "" then
				break
			end
			str = str .. string.format(tbl_str, DropName, DropRate)
			idx = idx + 1
		end
	end
	str = str .. "</group>\n"
	return str
end

local function NpcShop(_req)
	local npc_name = _req["npc_name"]
	local ItemTable = NpcShop_Common(npc_name)
	local idx = 1

	local npc = Npc_Common(npc_name)
	if not npc then
		return false, "no_npc"
	end

	if not ItemTable then
		return false, "no_shop"
	end
	
	local ret_str = ""

	local pay_type = ItemTable("PayType")
	if pay_type then
		ret_str = ret_str .. "<root pay_type=\""..pay_type.."\">\n"
		while true do
			local item_type = ItemTable("ItemType"..idx)
			local item_name = ItemTable("ItemName"..idx)
			if item_name == "" then
				break
			end
			local ItemInfo = g_ItemInfoMgr:HaveItem(tonumber(item_type),item_name)
			if ItemInfo then
				local BaseLevel = g_ItemInfoMgr:GetItemInfo(tonumber(item_type),item_name,"BaseLevel") or 0
				ret_str = ret_str .. "<item type=\"" .. item_type .. "\" name=\"" .. item_name .."\" level=\"" .. BaseLevel .. "\" />\n" 
				idx = idx + 1
			end
		end
		ret_str = ret_str .. "</root>\n"
	end
	return true, ret_str	
end

g_WebFuncDef["NpcShop"] = NpcShop
g_WebFuncDef["NpcDrop"] = NpcDrop
g_WebFuncDef["NpcPosition"] = NpcPosition
g_WebFuncDef["NpcNameMatch"] = NpcNameMatch
