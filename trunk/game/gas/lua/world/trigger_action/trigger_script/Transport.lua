local function Transport(Arg, Trigger, Player)

	local TrapName = Trigger.m_Properties:GetCharName()
	local SceneName = Player.m_Scene.m_LogicSceneName 
	local PlayerId = Player.m_uID
	if Transport_Server(TrapName) then
		for _, i in pairs(Transport_Server:GetKeys(TrapName, "陷阱")) do
			local TransInfo = Transport_Server(TrapName, "陷阱", i.."")
	 		if (TransInfo("BeginScene") == "" or SceneName == TransInfo("BeginScene"))
	 			and CheckPlayerLev(Player:CppGetLevel(), TransInfo("LeastLev"), TransInfo("MaxLev")) 
				and (TransInfo("Camp") == 0 or TransInfo("Camp") == Player:CppGetCamp()) then
				if Scene_Common[TransInfo("EndScene")] == nil then
					return
				end
				local Pos = CPos:new()
				Pos.x, Pos.y = TransInfo("PosX"), TransInfo("PosY")
	--			if p.QuestNeed == "" then
				if TransInfo("EndScene") == SceneName then
					Player:SetGridPosByTransport(Pos)
					---下面这个函数主要是针对本地图内跨trap寻路
					Gas2Gac:PlayerTransportSetGrid(Player.m_Conn)
				else
					local SaveX,SaveY = nil,nil
					if TransInfo("SavePosX") ~= "" and TransInfo("SavePosY") ~= "" then
						SaveX,SaveY = TransInfo("SavePosX"), TransInfo("SavePosY")
					end
					NewChangeScene(Player.m_Conn, TransInfo("EndScene"), Pos.x , Pos.y, SaveX,SaveY)
				end
				break
			end	
		end
	end
end

local DaTaoShaTrans = DaTaoShaTransport
local function DaTaoShaTransport(Arg, Trigger, Player)
--	print("DaTaoShaTransport")
	local Type = Player.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	DaTaoShaTrans(Trigger, Player)
end

local function DaDuoBaoTransport(Arg, Trigger, Player)
	local Type = Player.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	
	local MaxRoomNum = table.getn(g_DaTaoShaPointMgr)
	local SelRoomNum = math.random(1,MaxRoomNum)
	--print("SelRoomNum",SelRoomNum)
	if g_DaTaoShaPointMgr[SelRoomNum] then
		local Pos = CPos:new()
		Pos.x = g_DaTaoShaPointMgr[SelRoomNum]["IntoRoomPoint"][1]
		Pos.y = g_DaTaoShaPointMgr[SelRoomNum]["IntoRoomPoint"][2]
		Player:SetGridPosByTransport( Pos )
	end
end

local function XiuXingTaTransport(Arg, Trigger, Player)
	--print("进入修行塔传送Trap")
	local Type = Player.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	
	local Scene = Player.m_Scene
	-- 检查下一层是不是奖励层
	Scene.m_MercenaryActionInfo.m_IsRewardFloor = CMercenaryEducateAct.IsNextRewardFloor(Scene)
	-- 计算下一层的房间号
	local SelRoomNum = CMercenaryEducateAct.GetNextRoomNum(Player)	
	--print("选出房间 SelRoomNum = "..SelRoomNum)

	if g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum] then
		--传送,调用弹出窗口并进行其它的加载处理(这个接口中有传送的坐标设置)
		CMercenaryRoomCreate.RandomLoadYbActionInfo(Player, SelRoomNum)
		DestroyServerTrap(Trigger, false)
	end
	CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.BeforeGame)
	local GameID = Scene.m_MercenaryActionInfo.m_GameID
	CMercenaryRoomCreate.YbEduActAddOrResetSkill(Player, GameID, true)
	--print("修行塔传送完毕")
end

local function AreaFbTransport(Arg, Trigger, Player)

	local Type = Player.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	
	local TrapName = Trigger.m_Properties:GetCharName()
	local SceneName = Player.m_Scene.m_SceneName 
	local EndScene = nil
	local PosX,PosY = nil,nil
	if Transport_Server(TrapName) then
		for _, i in pairs(Transport_Server:GetKeys(TrapName, "陷阱")) do
			local TransInfo = Transport_Server(TrapName, "陷阱", i.."")
			if TransInfo("BeginScene") ~= "" 
				and TransInfo("BeginScene") == SceneName then
				EndScene = TransInfo("EndScene") 
				PosX = TransInfo("PosX") 
				PosY = TransInfo("PosY") 
				break
			end
		end
	end
	if EndScene == nil or EndScene == "" 
		or PosX == nil or PosX == "" 
		or PosY == nil or PosY == "" then
		return
	end
	if g_AreaFbLev[EndScene] == nil then
		return
	end
	
	CAreaFbServer.ChangeToAreaFb(Player.m_Conn, EndScene)
end

local function TongSceneTransport(Arg, Trigger, Player)
	ChangeTongSceneByTrap(Player.m_Conn)
end

local function AllTongSceneTransport(Arg, Trigger, Player)
	CDragonCave:LeaveDragonCave(Player.m_Conn)
end

local TransFunTbl = {
	["大逃杀传送"] = DaTaoShaTransport,
	["大夺宝传送"] = DaDuoBaoTransport,
	["修行塔传送"] = XiuXingTaTransport,
	["地下城传送"] = AreaFbTransport,
	["帮会驻地传送"] = TongSceneTransport,
	["所有类型驻地传送"] = AllTongSceneTransport,
}	

local function TrapTriggerTrans(Arg, Trigger, Player)

	if not IsCppBound(Player) then
		return
	end
	local Type = Player.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	local fun = Transport
	if TransFunTbl[Arg[1]] then
		fun = TransFunTbl[Arg[1]]
	end
	if IsFunction (fun) then
		fun(Arg, Trigger, Player)
	end
end

local funTbl = {
	[ECharacterType.Trap] = TrapTriggerTrans,
}

local function Script(Arg, Trigger, Player)
	local type = Trigger.m_Properties:GetType()
	if IsFunction (funTbl[type]) then
		funTbl[type](Arg, Trigger, Player)
	end
end

return Script
