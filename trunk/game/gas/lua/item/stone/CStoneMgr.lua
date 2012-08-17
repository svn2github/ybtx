cfg_load "item/Stone_Common"

local lStoneSqr = "StoneMgrDB"

CStoneMgr = class()
--获取CStoneMgr对象
function CStoneMgr.GetStoneMgr()
	g_StoneMgr = g_StoneMgr or CStoneMgr:new()
	return g_StoneMgr
end

function CStoneMgr:AddPropertyValueByName(Player, sPropertyName, fValue, panel, depart, slot, bExtra )
	if not Player then
		return
	end
	local uTransactionID = Player:CreateTransactionID()
	Player:ChangeTransactionPropertyValue(uTransactionID, sPropertyName, fValue)
	Player:CommitTransaction(uTransactionID)
	
	Player.m_StoneEffTransactionTbl = Player.m_StoneEffTransactionTbl or {}
	Player.m_StoneExtraEffTransactionTbl = Player.m_StoneExtraEffTransactionTbl or {}
	if not bExtra then
		if Player.m_StoneEffTransactionTbl[panel] then
			if Player.m_StoneEffTransactionTbl[panel][depart] then
				if Player.m_StoneEffTransactionTbl[panel][depart][slot] then
					assert(false, "宝石效果无法安装,这个孔已经镶嵌了!")
					return
				end
			else
				Player.m_StoneEffTransactionTbl[panel][depart] = {}
			end
		else
			Player.m_StoneEffTransactionTbl[panel] = {}
			Player.m_StoneEffTransactionTbl[panel][depart] = {}
		end
		Player.m_StoneEffTransactionTbl[panel][depart][slot] = uTransactionID
	else
		if Player.m_StoneExtraEffTransactionTbl[panel] then
			if Player.m_StoneExtraEffTransactionTbl[panel][depart] then
				if Player.m_StoneExtraEffTransactionTbl[panel][depart][slot] then
					assert(false, "宝石效果无法安装,这个孔的附加效果已经有了!")
					return
				end
			else
				Player.m_StoneExtraEffTransactionTbl[panel][depart] = {}
			end
		else
			Player.m_StoneExtraEffTransactionTbl[panel] = {}
			Player.m_StoneExtraEffTransactionTbl[panel][depart] = {}
		end
		Player.m_StoneExtraEffTransactionTbl[panel][depart][slot] = uTransactionID
	end
	CGasFightingEvaluation.UpdateFightingEvaluationInfo(Player)
end

function CStoneMgr:DelPropertyValueByName(Player, panel, depart, slot, bExtra )
	if not Player then
		return
	end
	if not bExtra then
		if Player.m_StoneEffTransactionTbl[panel] and Player.m_StoneEffTransactionTbl[panel][depart]
			and Player.m_StoneEffTransactionTbl[panel][depart][slot] then
			Player:DestroyTransaction(Player.m_StoneEffTransactionTbl[panel][depart][slot])
			Player.m_StoneEffTransactionTbl[panel][depart][slot] = nil
		else
			assert(false, "宝石效果无法撤消,这个孔还没镶嵌")
		end
	else
		if Player.m_StoneExtraEffTransactionTbl[panel] and Player.m_StoneExtraEffTransactionTbl[panel][depart]
			and Player.m_StoneExtraEffTransactionTbl[panel][depart][slot] then
			Player:DestroyTransaction(Player.m_StoneExtraEffTransactionTbl[panel][depart][slot])
			Player.m_StoneExtraEffTransactionTbl[panel][depart][slot] = nil
		else
			assert(false, "宝石效果无法撤消,这个孔没有附加效果")
		end		
	end
	CGasFightingEvaluation.UpdateFightingEvaluationInfo(Player)
end

function CStoneMgr:DelAllStonePropertyValue(Player)
	if Player.m_StoneEffTransactionTbl ~= nil then
		for i, v in pairs(Player.m_StoneEffTransactionTbl) do
			for j, k in pairs(v) do
				for m, l in pairs(k) do
					Player:DestroyTransaction(l)
				end
			end
		end
	end
	if Player.m_StoneExtraEffTransactionTbl ~= nil then
		for i, v in pairs(Player.m_StoneExtraEffTransactionTbl) do
			for j, k in pairs(v) do
				for m, l in pairs(k) do
					Player:DestroyTransaction(l)
				end
			end
		end
	end
end


local function lSendHoleInfo(Conn, OpenedHolesList, AllInlayedHolesInfo, nowpanel)
	for i=1,table.getn(OpenedHolesList) do
		local panel,depart,slot = OpenedHolesList[i][2],OpenedHolesList[i][3],OpenedHolesList[i][4]
		Gas2Gac:ReturnGetOpenedHole(Conn,panel,depart,slot)
	end
	local count = AllInlayedHolesInfo:GetRowNum()
	for i=1, count do
		local panel = AllInlayedHolesInfo(i,2)
		local depart = AllInlayedHolesInfo(i,3)
		local slot = AllInlayedHolesInfo(i,4)
		local name = AllInlayedHolesInfo(i,6)
		Gas2Gac:ReturnGetInlayedHole(Conn,panel,depart,slot,g_ItemInfoMgr:GetStoneBigID(),name)	
		if slot == 4 or slot == 8 then
			Gas2Gac:NotifyAddAttr(Conn,panel,depart,slot)
		end
	end
end

local function UnusePanle(Conn, AllInlayedHolesInfo, oldPanel)
	local ExtraHole = {}
	local count = AllInlayedHolesInfo:GetRowNum()
	for i=1, count do
		local panel = AllInlayedHolesInfo(i,2)
		local depart = AllInlayedHolesInfo(i,3)
		local slot = AllInlayedHolesInfo(i,4)
		local type = AllInlayedHolesInfo(i,5)
		local name = AllInlayedHolesInfo(i,6)
		if panel == nowpanel then
			ExtraHole[panel] = ExtraHole[panel] or {}
			ExtraHole[panel][depart] = ExtraHole[panel][depart] or {}
			ExtraHole[panel][depart][slot]= {type, name}
		end
	end
	
	local player = Conn.m_Player
	if not ExtraHole[oldPanel] then
		return
	end
	local StoneMgr = CStoneMgr.GetStoneMgr()
	for i,v in pairs(ExtraHole[oldPanel]) do
		local max_m = 0
		for m, n in pairs(v) do
			if max_m == m-1 then
				max_m = m
			end
			StoneMgr:DelPropertyValueByName(player, oldPanel, i, m, false )
		end
		if max_m == 8 then --取消第八孔附加
			StoneMgr:DelPropertyValueByName(player, oldPanel, i, 8, true )
		end
		if max_m > 4 then  --取消第四孔附加
			StoneMgr:DelPropertyValueByName(player, oldPanel, i, 4, true )
		end
	end
end

function CStoneMgr:InitAllInlayStone(Player, AllInlayedHolesInfo, nowpanel)
	local ExtraHole = {}
	local count = AllInlayedHolesInfo:GetRowNum()
	for i=1, count do
		local panel = AllInlayedHolesInfo(i,2)
		local depart = AllInlayedHolesInfo(i,3)
		local slot = AllInlayedHolesInfo(i,4)
		local type = AllInlayedHolesInfo(i,5)
		local name = AllInlayedHolesInfo(i,6)
		if panel == nowpanel then
			ExtraHole[panel] = ExtraHole[panel] or {}
			ExtraHole[panel][depart] = ExtraHole[panel][depart] or {}
			ExtraHole[panel][depart][slot]= {type, name}
		end
	end
	
	if not ExtraHole[nowpanel] then
		return
	end
	--ExtraHole = [panel][depart][slot]
	for i,v in pairs(ExtraHole[nowpanel]) do
		local max_m = 0
		for m, n in pairs(v) do
			local StoneName = n[2]
			self:AddPropertyValueByName(Player, GetPropertyAdderName(Stone_Common(StoneName,"AttrType")), tonumber(Stone_Common(StoneName,"AttrValue")), nowpanel, i, m, false )
			if max_m == m-1 then
				max_m = m
			end
		end
		if max_m == 8 then  --设置第八孔附加
			local StoneName = v[8][2]
			self:AddPropertyValueByName(Player, GetPropertyAdderName(Stone_Common(StoneName,"AttrType")), tonumber(Stone_Common(StoneName,"Award8")), nowpanel, i, 8, true )
		end
		if max_m >= 4 then  --设置第四孔附加
			local StoneName =v[4][2]
			self:AddPropertyValueByName(Player, GetPropertyAdderName(Stone_Common(StoneName,"AttrType")), tonumber(Stone_Common(StoneName,"Award4")), nowpanel, i, 4, true)
		end
	end
end

--*****************************************************************************************************
--摘除宝石
--1、创建一个新的宝石，并且级别比原来的宝石低1
--2、删除要摘除的宝石
--事务处理1、2 begin....end
--****************************************************************************************************
----------------------------------------------------RPC-----------------------------------------------
function CStoneMgr:LoginInitStone(Player, OpenedHolesList, AllInlayedHolesInfo, nowpanel)
	local player_level = Player:CppGetLevel()
	if(player_level>=60 and player_level<120) then
		Gas2Gac:NotifyOpenPanel(Player.m_Conn,2)
	end
	if (player_level >= 120) then
		Gas2Gac:NotifyOpenPanel(Player.m_Conn,2)
		Gas2Gac:NotifyOpenPanel(Player.m_Conn,3)
	end
		
	Gas2Gac:UpdateAllHoleInfoBegin(Player.m_Conn)
	apcall(lSendHoleInfo,Player.m_Conn, OpenedHolesList, AllInlayedHolesInfo, nowpanel)
	Gas2Gac:SendAllHoleInfoEnd(Player.m_Conn)
	Gas2Gac:UsePanel(Player.m_Conn,nowpanel)
end

function CStoneMgr:ChangeFrame(Conn,oldFrameID,FrameID)
	if oldFrameID == FrameID then
		return
	end
	
	local function CallBack(result)
		if not result[1] then
			return
		end
		local oldPanel, newPanel = result[2], result[3]
		local OpenedHolesList = result["OpenedHolesList"]
		local AllInlayedHolesInfo = result["AllInlayedHolesInfo"]
		UnusePanle(Conn, AllInlayedHolesInfo, oldPanel)
		if not IsCppBound(Conn.m_Player) then
			return
  	end
  	self:InitAllInlayStone(Conn.m_Player, AllInlayedHolesInfo, nowpanel)
		Gas2Gac:UpdateAllHoleInfoBegin(Conn)
		lSendHoleInfo(Conn, OpenedHolesList, AllInlayedHolesInfo, newPanel)
		Gas2Gac:SendAllHoleInfoEnd(Conn)
	end
	
	local parameter = {}
	parameter["player_id"] = Conn.m_Player.m_uID
	parameter["new_panel"] = FrameID
	parameter["old_panal"] = oldFrameID
	
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "ChangeFrame", CallBack, parameter)
end

--申请打孔
function CStoneMgr:RequestOpenHole(Conn,Panel,Depart,Slot,ItemID)
	if not IsCppBound(Conn.m_Player) then
		return
  end
  if Depart~=1 and Depart~=5 and Depart~= 9 then
  	if Conn.m_Player:CppGetLevel() < 60 or 
  		(Depart~=2 and Depart~=6 and Depart~= 10)then
  		return
  	end
  end
	local function CallBack(result)
		if (not result[1]) then
			if result[2] then
				MsgToConn(Conn, result[2])
			end		
			Gas2Gac:ReturnOpenHole(Conn,false)
			return
		end
		if result[1] then
			Gas2Gac:UpdateAllHoleInfoBegin(Conn)
			local OpenedHolesList = result["OpenedHolesList"]
			local nowpanel = result["nowpanel"]
			local AllInlayedHolesInfo = result["AllInlayedHolesInfo"]
			
			Gas2Gac:UpdateAllHoleInfoBegin(Conn)
			lSendHoleInfo(Conn, OpenedHolesList, AllInlayedHolesInfo, nowpanel)
			Gas2Gac:SendAllHoleInfoEnd(Conn)
		
			Gas2Gac:ReturnOpenHole(Conn,true)
		end
	end
	local parameter = {}
	parameter["player_id"] = Conn.m_Player.m_uID
	parameter["player_level"] = Conn.m_Player:CppGetLevel()
	parameter["panel"] = Panel
	parameter["depart"] = Depart
	parameter["slot"] = Slot
	parameter["item_id"] = ItemID
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "OpenHole", CallBack, parameter)
end

--添加宝石，增加属性
local function InlayAddAttr(Conn,data, AllInlayedHolesInfo, Panel,Depart, Slot)
	local nowpanel = data["now_panel"]
	if (nowpanel ~= Panel)then
		return
	end
	
	local item_name = data["item_name"]
	local player = Conn.m_Player
	local ExtraHole = {}
	local count = AllInlayedHolesInfo:GetRowNum()
	for i=1, count do
		local panel = AllInlayedHolesInfo(i,2)
		local depart = AllInlayedHolesInfo(i,3)
		local slot = AllInlayedHolesInfo(i,4)
		local type = AllInlayedHolesInfo(i,5)
		local name = AllInlayedHolesInfo(i,6)
		if panel == nowpanel then
			ExtraHole[panel] = ExtraHole[panel] or {}
			ExtraHole[panel][depart] = ExtraHole[panel][depart] or {}
			ExtraHole[panel][depart][slot]= {type, name}
		end
	end	
	
	local StoneMgr = CStoneMgr.GetStoneMgr()
	StoneMgr:AddPropertyValueByName(player, GetPropertyAdderName(Stone_Common(item_name,"AttrType")), tonumber(Stone_Common(item_name,"AttrValue")), Panel, Depart, Slot, false)
	
	
	local new_max, ole_max = 0, 0
	for m,n in pairs(ExtraHole[nowpanel][Depart]) do
		if new_max == m-1 then
			new_max = m
			--old_max = m
		end
--		if (m == Slot) then
--			old_max = old_max-1
--		end
	end
	old_max = Slot-1
	
	local Extra1 = math.floor(old_max/4)
	local Extra2 = math.floor(new_max/4)
	
	for i = 1, Extra2-Extra1 do
		local name = ExtraHole[nowpanel][Depart][i*4][2]		
		if i == 1 then
			if (Slot <= 4) then
				StoneMgr:AddPropertyValueByName(player, GetPropertyAdderName(Stone_Common(name,"AttrType")), tonumber(Stone_Common(name,"Award4")), Panel, Depart, 4, true )
			else
				name = ExtraHole[nowpanel][Depart][8][2]		
				StoneMgr:AddPropertyValueByName(player, GetPropertyAdderName(Stone_Common(name,"AttrType")), tonumber(Stone_Common(name,"Award8")), Panel, Depart, 8, true )
			end
		elseif i == 2 then
			StoneMgr:AddPropertyValueByName(player, GetPropertyAdderName(Stone_Common(name,"AttrType")), tonumber(Stone_Common(name,"Award8")), Panel, Depart, 8, true )
		end
	end
	
end
--申请镶嵌
function CStoneMgr:RequestInlayStone(Conn,Panel,Depart,Slot,StoneId)
	if not IsCppBound(Conn.m_Player) then
		return
  end
  
	local function CallBack(result)
		if not result[1] then
			Gas2Gac:ReturnInlayStone(Conn,false)
			if result[2] then
				MsgToConn(Conn, result[2])
			end
			return
		end
		
		InlayAddAttr(Conn, result["extra"], result["AllInlayedHolesInfo"], Panel, Depart, Slot)

		Gas2Gac:UpdateAllHoleInfoBegin(Conn)
		lSendHoleInfo(Conn, result["OpenedHolesList"], 
							result["AllInlayedHolesInfo"], 
							result["extra"]["now_panel"])
		Gas2Gac:SendAllHoleInfoEnd(Conn)
		Gas2Gac:ReturnInlayStone(Conn,true)
	end
	local parameter = {}
	parameter["player_id"] = Conn.m_Player.m_uID
	parameter["panel"] = Panel 
	parameter["depart"] = Depart
	parameter["slot"] = Slot
	parameter["stoneId"] = StoneId
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName	
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "InlayStone", CallBack, parameter)
end

--摘除宝石删除属性
local function RemoveExtraAttrAfterRemover(Conn,data, AllInlayedHolesInfo, Depart, Slot)
	local nowpanel = data["now_panel"]
	local old_belong_panel = data["old_panel"]
	local old_name = data["old_name"]
	
	if (nowpanel ~= old_belong_panel) then
		return
	end
	
	local player = Conn.m_Player
	local ExtraHole = {}
	local count = AllInlayedHolesInfo:GetRowNum()
	for i=1, count do
		local panel = AllInlayedHolesInfo(i,2)
		local depart = AllInlayedHolesInfo(i,3)
		local slot = AllInlayedHolesInfo(i,4)
		local type = AllInlayedHolesInfo(i,5)
		local name = AllInlayedHolesInfo(i,6)
		if panel == nowpanel then
			ExtraHole[panel] = ExtraHole[panel] or {}
			ExtraHole[panel][depart] = ExtraHole[panel][depart] or {}
			ExtraHole[panel][depart][slot]= {type, name}
		end
	end	
	local StoneMgr = CStoneMgr.GetStoneMgr()
	StoneMgr:DelPropertyValueByName(player, nowpanel, Depart, Slot, false)
	
	if not ExtraHole[nowpanel] then
		return
	end
	
	local Depart = Depart
	local old_slot = Slot
	
	local new_max, old_max = 0, 0

	if old_slot == 1 then
		old_max=1
		new_max=1
	end
	ExtraHole[nowpanel][Depart] = ExtraHole[nowpanel][Depart] or {}
	for m,n in pairs(ExtraHole[nowpanel][Depart]) do
		if new_max == m-1 and Slot ~= 1 then
			new_max = m
		end
		if old_max == m-1 then
			old_max = m
		end
		if (m == old_slot-1) then
			old_max = old_max+1
		end
	end
	
	if old_max < old_slot then
		return
	end
	
	local Extra1 = math.floor(new_max/4)
	local Extra2 = math.floor(old_max/4)

	for i = 1, Extra2-Extra1 do
		if i == 1 then
			if old_slot <= 4 then
				StoneMgr:DelPropertyValueByName(player, nowpanel, Depart, 4, true)
			else
				StoneMgr:DelPropertyValueByName(player, nowpanel, Depart, 8, true)
			end
		elseif i == 2 then
			StoneMgr:DelPropertyValueByName(player, nowpanel, Depart, 8, true)
		end
	end
end
--申请摘除宝石
function CStoneMgr:RequestRemovalStone(Conn,Panel,Depart,Slot)
	local function CallBack(result)
		if not result[1] then
			MsgToConn(Conn, result[2])
			return
		end
		
		local extra_info = result["extra_info"]
		
		RemoveExtraAttrAfterRemover(Conn, extra_info, result["AllInlayedHolesInfo"], Depart, Slot)
		Gas2Gac:UpdateAllHoleInfoBegin(Conn)
		lSendHoleInfo(Conn, result["OpenedHolesList"], 
							result["AllInlayedHolesInfo"], 
							result["extra_info"]["now_panel"])
		Gas2Gac:SendAllHoleInfoEnd(Conn)

		Gas2Gac:ReturnRemovalStone(Conn,true)
	end
	
	local parameter = {}
	parameter["player_id"] = Conn.m_Player.m_uID
	parameter["Panel"] = Panel
	parameter["Depart"] = Depart
	parameter["Slot"] = Slot
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "RemovalStole", CallBack, parameter)
end

--白宝石鉴定
function CStoneMgr:UseWhiteStone(Connection, Player, nRoomIndex, nPos, nBigID, nIndex, nItemID,nEquipPart,Data)
	local result = Data["HighStoneResult"]
	if not result[1] then
		Gas2Gac:RetWhiteStoneAppraise(Connection,0,"失败",0,1)
		if result[2] then
			MsgToConn(Connection, result[2])
		end
		return
	end
	for i=1,#(result[4]) do
		Gas2Gac:RetWhiteStoneAppraiseFirstTime(Connection,g_ItemInfoMgr:GetStoneBigID(),result[4][i])
	end
	Connection.m_Player.whitestone_Name = result[5]
	Connection.m_Player.tbl_new_stoneName = result[4]
	Connection.m_Player.tbl_HighStoneID = {{result[3][2]}}
	Gas2Gac:RetWhiteStoneAppraise(Connection,g_ItemInfoMgr:GetStoneBigID(),result[3][1],result[3][2],1)
end

function CStoneMgr:SendSynthesisItemBegin(Conn)
	Conn.tbl_SynthesisItem={}
end

function CStoneMgr:SendSynthesisItem(Conn,ItemId)
	Conn.tbl_SynthesisItem[#Conn.tbl_SynthesisItem + 1] = ItemId
end

--发送完毕
function CStoneMgr:SendSynthesisItemEnd(Conn)
	local function CallBack(result)
		if not result[1] then
			Gas2Gac:ReturnSynthesisItemEnd(Conn,false)
			if result[2] then
				MsgToConn(Conn, result[2])
			end
			return
		end
		if result[3] then
			for i=1,#result[3] do
				Gas2Gac:ReturnSynthesisItem(Conn,result[3][i][1],result[3][i][2],result[3][i][3])
			end
		end
		if result[4][1] then
			if not result[4][2] then
				MsgToConn(Conn, 7501)
				return
			end
		end
		Gas2Gac:ReturnSynthesisItemEnd(Conn,true)
	end

	if Conn.m_Player == nil then return end
	local parameter = {}
	parameter["player_id"] = Conn.m_Player.m_uID
	parameter["tbl_item_id"]  = Conn.tbl_SynthesisItem
	parameter["room_index"]= g_StoreRoomIndex.PlayerBag
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "SynthesisItem", CallBack, parameter)
end

--再次鉴定 , 从12个已鉴定出的宝石中随机出一个
function CStoneMgr:RequestStoneAppraiseAgain(Conn)
	--Conn.m_Player.tbl_new_stoneName   保存了现在已经鉴定出12个宝石
	if not Conn.m_Player.tbl_new_stoneName or # Conn.m_Player.tbl_new_stoneName == 0 then
		Gas2Gac:RetWhiteStoneAppraise(Conn,0,"失败",0, 2)
		return
	end
	
	local function CallBack(result)
		Conn.m_Player.tbl_new_stoneName = {}
		Conn.m_Player.whitestone_Name = nil
		if not result[1] then
			Gas2Gac:RetWhiteStoneAppraise(Conn,0,"失败",0,2)
			return
		end
		Gas2Gac:RetWhiteStoneAppraise(Conn,g_ItemInfoMgr:GetStoneBigID(),result[3][1],result[3][2],2)
		Conn.m_Player.tbl_HighStoneID = {{result[3][2]}}
	end
	local parameter = {}
	parameter["player_id"] = Conn.m_Player.m_uID
	parameter["item_id"] = Conn.m_Player.tbl_HighStoneID[1][1] --修改需求后，这个tbl只会有一个值
	parameter["tbl_new_stoneName"] = Conn.m_Player.tbl_new_stoneName
	parameter["whitestone_Name"]     = Conn.m_Player.whitestone_Name
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "WhiteStoneAppraiseAgain", CallBack, parameter)
end
--提取鉴定生成的宝石
function CStoneMgr:TakeAppraisedStone(Conn)
	Conn.m_Player.tbl_new_stoneName = {}
	Conn.m_Player.whitestone_Name = nil
	local function CallBack(result)
		if result[1] then
			if not result[2] then
				Conn.m_Player.whitestone_Name = nil
				Conn.m_Player.tbl_new_stoneName = {}
				Gas2Gac:RetTakeAppraisedStone(Conn,true)
				MsgToConn(Conn, 7501)
				return
			end
			Conn.m_Player.whitestone_Name = nil
			Conn.m_Player.tbl_new_stoneName = {}
			Gas2Gac:RetTakeAppraisedStone(Conn,true)
			return
		else
			Conn.m_Player.tbl_HighStoneID = result[3]
		end
		Gas2Gac:RetTakeAppraisedStone(Conn,false)
	end
	
	if Conn.m_Player.tbl_HighStoneID == nil then
		return --没有合成高级宝石，不能提取
	end
	
	local data = {}
	data["player_id"] = Conn.m_Player.m_uID
	data["tbl_new_items"] = Conn.m_Player.tbl_HighStoneID
	data["type"] = "Appraised"
	Conn.m_Player.tbl_HighStoneID = nil --提取一次之后不，置空
	CallAccountManualTrans(Conn.m_Account, lStoneSqr, "TakeItem", CallBack, data)
end

