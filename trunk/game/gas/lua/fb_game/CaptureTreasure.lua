CaptureTreasure = class()

local RoomTbl = {}
RoomTbl[1] = "跳跳大夺宝副本1"
RoomTbl[2] = "跳跳大夺宝副本2"
RoomTbl[3] = "跳跳大夺宝副本3"
RoomTbl[4] = "跳跳大夺宝副本4"


local function CreateFbAllTrap(Scene)
	if Scene then
		local TrapPos = CPos:new()
		for _, index in pairs(DaTaoShaTransport_Server:GetKeys()) do
			TrapPos.x = g_DaTaoShaPointMgr[index]["OutRoomPoint"][1]
			TrapPos.y = g_DaTaoShaPointMgr[index]["OutRoomPoint"][2]
			local Trap = CreateServerTrap(Scene, TrapPos, "大夺宝传送点")
		end
	end
end

local function GetPlayerToPos()
	local MaxRoomNum = table.getn(g_DaTaoShaPointMgr)
	local SelRoomNum = math.random(1,MaxRoomNum)
	--print("SelRoomNum",SelRoomNum)
	if g_DaTaoShaPointMgr[SelRoomNum] then
		local Pos = CPos:new()
		Pos.x = g_DaTaoShaPointMgr[SelRoomNum]["BornPoint"][1]
		Pos.y = g_DaTaoShaPointMgr[SelRoomNum]["BornPoint"][2]
		return Pos
	end
end

local function IsSameLevel(Conn,LevelType)
	if LevelType and LevelType >= 1 and LevelType <= 4 then

		local PlayerLevel = Conn.m_Player:CppGetLevel()
		if LevelType == 1 then
			if PlayerLevel >= 10 and PlayerLevel <= 20 then
				return true
			end
		elseif LevelType == 2 then
			if PlayerLevel >= 21 and PlayerLevel <= 30 then
				return true
			end
		elseif LevelType == 3 then
			if PlayerLevel >= 31 and PlayerLevel <= 40 then
				return true
			end
		elseif LevelType == 4 then
			if PlayerLevel > 40 then
				return true
			end
		end
	end
	return false
end

local function DuoBaoChangeScene(Conn,LevelType,MoneyCount)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local RoomName = RoomTbl[LevelType]
	if not Scene_Common[RoomName] then
		return
	end
	
	local ToScenePos = GetPlayerToPos()
	if not ToScenePos then
		return
	end
	local PosX = ToScenePos.x
	local PosY = ToScenePos.y
	
	
	local function CallBack(isSucceed, sceneId, serverId)
		if isSucceed then
			MultiServerChangeScene(Conn, sceneId, serverId, PosX , PosY)
		else
			if sceneId then
				MsgToConn(Conn, sceneId)
			end
		end
	end	
	
	local data = {}
	data["charId"] = Player.m_uID
	data["SceneName"] = RoomName
	data["MoneyNum"] = MoneyCount
	CallAccountAutoTrans(Conn.m_Account, "SceneMgrDB", "DaDuoBaoChangeScene", CallBack, data)
end

function CaptureTreasure.JoinDaDuoBao(Conn, LevelType)
	if SearchClosedActionTbl("跳跳大夺宝") then
		MsgToConn(Conn,3521)--活动已经关闭,不可以报名
		return
	end
	if not IsSameLevel(Conn,LevelType) then
		--你当前的等级不能进入副本
		MsgToConn(Conn,3500)
		return
	end
	
	local Money = 0
	if LevelType == 1 then
		Money = 30
	elseif LevelType == 2 then
		Money = 60
	elseif LevelType == 3 then
		Money = 90
	elseif LevelType == 4 then
		Money = 100
	else
		return
	end
	
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local moneyCount = money:ChangeGoldAndArgentToMoney(0, Money, 0)
	
	DuoBaoChangeScene(Conn,LevelType,moneyCount)
end

function CaptureTreasure.ShowDaDuoBaoWnd(Conn, NpcID)
	if not CheckAllNpcFunc(Conn.m_Player,NpcID,"跳跳大夺宝报名处") then
		return
	end
	
	if not CheckActionIsBeginFunc("跳跳大夺宝") then
		MsgToConn(Conn,3520)--活动还没有开放,不可以报名
		return
	end
	
	Gas2Gac:RetShowDaDuoBaoWnd(Conn)
end

--玩家掉线
function CaptureTreasure.PlayerOffLineDaDuoBaoFb(Player)
	
end

function CaptureTreasure.IntoDaDuoBaoScene(Player)
	CreateFbAllTrap(Player.m_Scene)
	Player:PlayerDoFightSkillWithoutLevel("600秒")
end

--玩家在副本中切场景
function CaptureTreasure.LeaveDaDuoBaoScene(Player)
	--清除时间buff
	Player:PlayerDoFightSkillWithoutLevel("撤销600秒")
end

--玩家在副本中死掉
function CaptureTreasure.PlayerDeadInDaDuoBao(Attacker, Player)
	--清除时间buff
	--Player:PlayerDoFightSkillWithoutLevel("撤销600秒")
	local SceneName = Player.m_MasterSceneName
	local Pos = Player.m_MasterScenePos
	ChangeSceneByName(Player.m_Conn,SceneName,Pos.x,Pos.y)
end

function CaptureTreasure.PlayerBuffTimeIsOver(Player)
	local SceneType = Player.m_Scene.m_SceneAttr.SceneType
	if SceneType == 22 then
		--print("时间到了,退出副本")
		local SceneName = Player.m_MasterSceneName
		local Pos = Player.m_MasterScenePos
		ChangeSceneByName(Player.m_Conn,SceneName,Pos.x,Pos.y)
	end
end
