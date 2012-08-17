local NpcTbl = {}
NpcTbl[1] = "ÉÕÊ÷110"
NpcTbl[2] = "ÉÕÊ÷111"
NpcTbl[3] = "ÉÕÊ÷112"
NpcTbl[4] = "ÉÕÊ÷113"
NpcTbl[5] = "ÉÕÊ÷114"
NpcTbl[6] = "ÉÕÊ÷115"
NpcTbl[7] = "ÉÕÊ÷116"

local sign = 1

local function Script(Arg, Trigger, Player)
	
	if not Trigger.m_SmallGameTime then
		return
	end
	
	local Scene = Trigger.m_Scene
	
	local Nums = math.floor(15 / Trigger.m_SmallGameTime)
	local pos = CPos:new()
	local posIdTbl = {}
	posIdTbl[1] = Arg[1]
	posIdTbl[3] = Arg[2]
	
	local pathTbl = {}
	pathTbl[1] = "ÉÕÊ÷Â·¾¶"
	pathTbl[3] = "ÉÕÊ÷Â·¾¶×ó"
	
	local npcName
	local BossCurHp = Scene.m_FireTrap.m_FireNpc.m_BossNpc:CppGetHP()
	local BossFullHp = Scene.m_FireTrap.m_FireNpc.m_BossNpc:CppGetMaxHP()
	if BossCurHp > BossFullHp * 0.9 then
		npcName = NpcTbl[1]
	elseif BossCurHp > BossFullHp * 0.8 then
		npcName = NpcTbl[2]
	elseif BossCurHp > BossFullHp * 0.6 then
		npcName = NpcTbl[3]
	elseif BossCurHp > BossFullHp * 0.5 then
		npcName = NpcTbl[4]
	elseif BossCurHp > BossFullHp * 0.3 then
		npcName = NpcTbl[5]
	elseif BossCurHp > BossFullHp * 0.2 then
		npcName = NpcTbl[6]
	elseif BossCurHp > 0 then
		npcName = NpcTbl[7]
	end
	
	local Level = g_NpcBornMgr:GetNpcBornLevel(npcName)
	
	while(Nums > 0) do
		local ID = posIdTbl[sign + 2]
		local x, y = GetScenePosition(ID, Scene)
		pos.x = x + math.random(-2,2)
		pos.y = y + math.random(-2,2)
		local fPos = CFPos:new( pos.x * EUnits.eGridSpan, pos.y * EUnits.eGridSpan )
		local monster = g_NpcServerMgr:CreateServerNpc(npcName, Level, Scene, fPos)
		if not IsServerObjValid(monster) then
			return
		end
		monster:CppSetGameCamp(52)
		g_DynamicCreateMgr:CreatePath(monster, monster.m_Scene.m_SceneName, pathTbl[sign + 2])
		Nums = Nums - 1
		sign = -1 * sign
	end
	
end

return Script