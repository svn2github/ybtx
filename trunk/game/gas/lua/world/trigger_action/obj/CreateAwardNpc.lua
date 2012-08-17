gas_require "framework/main_frame/SandBoxDef"
gas_require "world/npc/ServerNpc"
gas_require "world/trap/TrapServer"

local CreateServerTrap = CreateServerTrap
local g_NpcServerMgr = g_NpcServerMgr
local EUnits = EUnits
local g_IntObjServerMgr = g_IntObjServerMgr
local CMercenaryRoomCreate = CMercenaryRoomCreate
local RegisterOnceTick = RegisterOnceTick
local g_GetPlayerInfo = g_GetPlayerInfo
local g_MercenaryEduActMgr = g_MercenaryEduActMgr
local Entry = CreateSandBox(...)

local function AccountTrapPixelPos(n)
	local param = n % 64
	return n - param + 32
end

function Entry.Exec(Conn, ServerIntObj, ObjName, GlobalID)
	if ServerIntObj.m_IsUsed then
		return	
	end
	ServerIntObj.m_IsUsed = true
	local Player = Conn.m_Player
	local Scene = Player.m_Scene
	
	local function DestroyObjTick()
		g_IntObjServerMgr:Destroy(ServerIntObj, false)
	end
	
	local function BeginGiveAwardTick()
		CMercenaryRoomCreate.FinishGameGiveAward(Player, not Scene.m_MercenaryActionInfo.m_GameIsSucc, Scene)
	end
	
	CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.PrepareNextGame)
	if Scene.m_MercenaryActionInfo then
		local SelRoomNum = Scene.m_MercenaryActionInfo.m_SelMercenaryRoomNum
		if g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum] then
			-- 清除头上特效
			Gas2Gac:ClearCompassHeadDir(Conn)
			
			local NpcPos = CFPos:new()
			NpcPos.x = AccountTrapPixelPos(g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].TrapPoint[1] * EUnits.eGridSpanForObj)
			NpcPos.y = AccountTrapPixelPos(g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum].TrapPoint[2] * EUnits.eGridSpanForObj)
			g_NpcServerMgr:CreateServerNpc("修行塔用传送门", 1, Scene, NpcPos)
			
			-- 8秒后开始给奖励
			RegisterOnceTick(Scene, "XiuXingTaBeginGiveAwardTick", BeginGiveAwardTick, 8000)
			
			-- 改变机关动画
			g_IntObjServerMgr:IntObjDoAction(ServerIntObj,"respond02_1", 2)
			
			-- 4秒后删除机关
			RegisterOnceTick(Scene, "XiuXingTaDestroyObjTick", DestroyObjTick, 4000)
		end
	end

end
 
return Entry
 
