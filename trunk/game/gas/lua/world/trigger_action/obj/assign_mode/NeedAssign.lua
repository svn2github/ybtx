
local DefaultTime = 60
function NeedAssignOneObj(Conn, data, IntObj, ObjID)
	if IntObj.m_IsAssign then
		MsgToConn(Conn, 3059)
		return
	end
	IntObj.m_IsAssign = true
	
	local ItemType = data["nType"]
	local ItemNum = 1
	local ItemName = data["sName"]
	IntObj.m_NeedAssignPoint = {0,0}
	IntObj.m_GreedAssignPoint = {0,0}
	IntObj.m_AssignPlayers = 0
	IntObj.m_CountTime = DefaultTime
	IntObj.m_AddItemData = data
	for _, char_id in pairs(IntObj.m_AssignPlayer) do 
		local Player = g_GetPlayerInfo(char_id)
		AddPlayerNeedAssignTbl(char_id, ObjID)
		IntObj.m_AssignPlayers = IntObj.m_AssignPlayers + 1
		if IsCppBound(Player) then
			Gas2Gac:ShowNeedAssignWnd(Player.m_Conn, ItemType, ItemName, ItemNum, g_CurServerId, ObjID, Player:CppGetClass(), DefaultTime) --打开掷骰子面板
		end
	end
	local function AssignFun(Tick, IntObj)
		if not IsServerObjValid(IntObj) then
			UnRegisterTick(self)
			return
		end
		IntObj.m_CountTime = IntObj.m_CountTime - 1
		if IntObj.m_CountTime <= 0 then
			NeedAssignOver(IntObj)
		end
	end
	IntObj.m_AssignTick = RegisterTick("AssignTick", AssignFun, 1000, IntObj)
end

local function SendAssignMessageToMember(PlayersTbl, MsgID,...)
	local arg = {...}
	for _, CharId in pairs(PlayersTbl) do
		MsgToConnById(CharId, MsgID, ...)
	end
end

function NeedAssignOver(IntObj)
	if IntObj.m_AssignTick then
		UnRegisterTick(IntObj.m_AssignTick)
		IntObj.m_AssignTick = nil
	end
	local NeedPoint = IntObj.m_NeedAssignPoint
	local GreedPoint = IntObj.m_GreedAssignPoint
	local data = IntObj.m_AddItemData
	if NeedPoint[2] > 0 then
		data["char_id"] = NeedPoint[1]
		data["char_name"] = NeedPoint[3]
		SendAssignMessageToMember(IntObj.m_AssignPlayer, 3039, NeedPoint[3], NeedPoint[2], data["sName"])
		NeedAssignGiveItem(IntObj, data)
	elseif GreedPoint[2] > 0 then
		data["char_id"] = GreedPoint[1]
		data["char_name"] = GreedPoint[3]
		SendAssignMessageToMember(IntObj.m_AssignPlayer, 3039, GreedPoint[3], GreedPoint[2], data["sName"])
		NeedAssignGiveItem(IntObj, data)
	else
		SendAssignMessageToMember(IntObj.m_AssignPlayer, 3040, data["sName"])
		IntObj.m_FreePickup = true
		IntObj.m_IsClick = nil
	end
	RemoveObjAllAssignTbl(IntObj)
end

function Gac2Gas:SelectNeedAssign(Conn, ServerId, Class, ObjID, IsNeed)

	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	
	if ServerId == g_CurServerId then
		SelectNeedAssign(Conn, Class, ObjID, IsNeed, Player.m_uID, Player.m_Properties:GetCharName())
	else
		Gas2GasCall:SelectNeedAssign(GetServerAutoConn(ServerId), Class, ObjID, IsNeed, Player.m_uID, Player.m_Properties:GetCharName())
	end
end

function Gas2GasDef:SelectNeedAssign(Conn, Class, ObjID, IsNeed, PlayerId, CharName)
	SelectNeedAssign(nil, Class, ObjID, IsNeed, PlayerId, CharName)
end

function SelectNeedAssign(Conn, Class, ObjID, IsNeed, PlayerId, CharName)
	local IntObj = CIntObjServer_GetIntObjServerByID(ObjID)
	if not IsServerObjValid(IntObj) then
		return
	end
	local ObjName = IntObj.m_Properties:GetCharName()
	
	if not IntObj.m_AssignPlayer then
		LogErr("需求分配物品时出现异常!", "玩家("..CharName..") ID("..PlayerId..") OBJ("..ObjName..")没有分配列表", Conn)
		return
	end
	if not RemovePlayerNeedAssignTbl(PlayerId, ObjID) then
		local flag = false
		for _, CharId in pairs(IntObj.m_AssignPlayer) do
			if PlayerId == CharId then
				flag = true
				break
			end
		end
		if not flag then
			LogErr("需求分配物品时出现异常!", "玩家("..CharName..") ID("..PlayerId..") 该玩家不在OBJ("..ObjName..")的分配列表中", Conn)
		end
		return
	end
	IntObj.m_AssignPlayers = IntObj.m_AssignPlayers - 1
	if IsNeed == 1 then
		local ItemType = g_ObjActionArgTbl[ObjName][1]
		local ItemName = ObjName
		if ItemType == 16 or (ItemType >= 5 and ItemType <= 9) then
			local res,_,info = g_ItemInfoMgr:BeEquipClass(ItemType,ItemName,Class)
			if not res then
				local tempStr = "玩家("..CharName..") ID("..PlayerId..") 职业("..Class..") 该玩家不在OBJ("..ObjName..")的职业需求分配列表中"
				tempStr = tempStr .. info
				LogErr("需求分配物品时出现异常!", tempStr, Conn)
				MsgToConnById(PlayerId, 3104)
				return
			end
		end
		RandomNeedPoint(IntObj, PlayerId, CharName)	
	elseif IsNeed == 2 then
		RandomGreedPoint(IntObj, PlayerId, CharName)	
	else
		SendAssignMessageToMember(IntObj.m_AssignPlayer, 3041, CharName, ObjName)
	end
	if IntObj.m_AssignPlayers == 0 then
		NeedAssignOver(IntObj)
	end
end

function RandomNeedPoint(IntObj, PlayerId, CharName)
	local PlayersTbl = IntObj.m_AssignPlayer
	local RandomPoint = math.random(1, 100)
	
	if RandomPoint > IntObj.m_NeedAssignPoint[2] then
		IntObj.m_NeedAssignPoint = {PlayerId, RandomPoint, CharName}
	end
	SendAssignMessageToMember(PlayersTbl, 3036, CharName, IntObj.m_Properties:GetCharName(), RandomPoint)		--6 组队频道GetStaticTextServer(6011)
end

function RandomGreedPoint(IntObj, PlayerId, CharName)
	local PlayersTbl = IntObj.m_AssignPlayer
	local RandomPoint = math.random(1, 100)
	
	if RandomPoint > IntObj.m_GreedAssignPoint[2] then
		IntObj.m_GreedAssignPoint = {PlayerId, RandomPoint, CharName}
	end
	SendAssignMessageToMember(PlayersTbl, 3037, CharName, IntObj.m_Properties:GetCharName(), RandomPoint)		--6 组队频道GetStaticTextServer(6011)
end

function AddPlayerNeedAssignTbl(PlayerID, ObjID)
	if not g_NeedAssignTbls[PlayerID] then
		g_NeedAssignTbls[PlayerID] = {}
	end
	g_NeedAssignTbls[PlayerID][ObjID] = true
end

function RemovePlayerNeedAssignTbl(PlayerID, ObjID)
	if g_NeedAssignTbls[PlayerID] and g_NeedAssignTbls[PlayerID][ObjID] then
		g_NeedAssignTbls[PlayerID][ObjID] = nil
		if table.maxn(g_NeedAssignTbls[PlayerID]) == 0 then
			g_NeedAssignTbls[PlayerID] = nil
		end
		Gas2GacById:CloseNeedAssignWnd(PlayerID, ObjID)
		return true
	end
end

function RemoveObjAllAssignTbl(IntObj)
	for _, char_id in pairs(IntObj.m_AssignPlayer) do 
		RemovePlayerNeedAssignTbl(char_id, IntObj:GetEntityID())
		RemovePlayerAcutionAssignTbl(char_id, IntObj:GetEntityID())
	end
	local TeamID = IntObj.m_Properties:GetBelongToTeam()
	if g_AcutionAssignTeamTbls[TeamID] then
		g_AcutionAssignTeamTbls[TeamID] = nil
		return
	end
end