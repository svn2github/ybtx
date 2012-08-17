
local BossNpcTbl = {}
BossNpcTbl[1] = "ÉÕÊ÷116"
BossNpcTbl[2] = "ÉÕÊ÷117"
BossNpcTbl[3] = "ÉÕÊ÷118"

local TimeLen = g_MatchGameMgr.m_CfgData["ÉÕ»ÙÎÛ×ÇÖ®Ê÷"].GameTime

local function HpTick(tick, Npc)
	if not IsCppBound(Npc) then
		UnRegisterTick(Npc.m_HpTick)
		Npc.m_HpTick = nil
		return
	end
	
	if not IsCppBound(Npc.m_BossNpc) then
		return
	end
	
	local NpcCurrentHp = Npc:CppGetHP()
	local BossCurrentHp = Npc.m_BossNpc:CppGetHP()
	
	if NpcCurrentHp < Npc.m_FullHp * 0.5 then
		return
	end
	
	local Xishu = NpcCurrentHp / Npc.m_FullHp * 2
	local BossSubHp = Xishu * Npc.m_BossNpc.m_FullHp / TimeLen
	Npc.m_BossNpc:CppSetHP(BossCurrentHp - BossSubHp)
	
	local NpcSubHp = Npc.m_FullHp * 0.01
	Npc:CppSetHP(NpcCurrentHp - NpcSubHp)
	
	BossCurrentHp = Npc.m_BossNpc:CppGetHP()
	local bossNpcName
	if BossCurrentHp > Npc.m_BossNpc.m_FullHp * 0.6 and BossCurrentHp <= Npc.m_BossNpc.m_FullHp * 0.7 then
		bossNpcName = BossNpcTbl[1]
	elseif BossCurrentHp > Npc.m_BossNpc.m_FullHp * 0.3 and BossCurrentHp <= Npc.m_BossNpc.m_FullHp * 0.4 then
		bossNpcName = BossNpcTbl[2]
	elseif BossCurrentHp > 0 and BossCurrentHp <= Npc.m_BossNpc.m_FullHp * 0.1 then
		bossNpcName = BossNpcTbl[3]
	end
	
	if bossNpcName then
		local Pos = Npc.m_RebornPos
		local fPos = CFPos:new(Pos.x * EUnits.eGridSpan, Pos.y * EUnits.eGridSpan)
		local Level = g_NpcBornMgr:GetNpcBornLevel(bossNpcName)
		local helpBossNpc = g_NpcServerMgr:CreateServerNpc(bossNpcName, Level, Npc.m_Scene, fPos)
		if not IsServerObjValid(helpBossNpc) then
			return
		end
		helpBossNpc:CppSetGameCamp(52)
	end
end

local function ReadyTick(tick, Npc)
	if Npc and Npc.m_ReadyTick ~= nil then
		UnRegisterTick(Npc.m_ReadyTick)
		Npc.m_ReadyTick = nil
		Npc.m_HpTick = RegisterTick("HpTick", HpTick, 1000, Npc)
	end
end
	
local function Script(Arg, Npc, Player)
	Npc.m_FullHp = Npc:CppGetMaxHP()
	Npc:CppSetGameCamp(1)
	local NpcTbl = Arg[1]
	local TrapTbl = Arg[2]
	
	local Scene = Npc.m_Scene
	
	local x,y = GetScenePosition(10059, Npc.m_Scene)
	local Pos = CFPos:new(x * EUnits.eGridSpan, y * EUnits.eGridSpan)
	local Level = g_NpcBornMgr:GetNpcBornLevel(NpcTbl[2])
	local BossNpc = g_NpcServerMgr:CreateServerNpc(NpcTbl[2], Level, Npc.m_Scene, Pos, nil, nil, nil, nil, nil, nil, 128)
	if not IsServerObjValid(BossNpc) then
		return
	end
	BossNpc:CppSetGameCamp(0)
	BossNpc.m_FullHp = BossNpc:CppGetMaxHP()
	Npc.m_BossNpc = BossNpc
	
	x,y = GetScenePosition(10058, Npc.m_Scene)
	Pos = CFPos:new(x, y)
	local FireTrap = CreateServerTrap(Scene, Pos, TrapTbl[2], 0)
	FireTrap.m_FireNpc = Npc
	
	Scene.m_FireTrap = FireTrap
	
	Npc.m_ReadyTick = RegisterTick("ReadyTick", ReadyTick, 30 * 1000, Npc)
	
end

return Script