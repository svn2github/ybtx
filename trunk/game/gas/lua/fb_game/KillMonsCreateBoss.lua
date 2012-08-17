gas_require "fb_game/KillMonsCreateBossInc"
gas_require "world/npc/ServerNpc"
cfg_load "fb_game/KillMonsCreateBoss_Common"

local dir = {{-2,-1}, {-1,-1}, {0,-1}, {1,-1}, {2,-1},
						 {-2,0}, {-1,0}, {0,0}, {1,0}, {2,0},
						 {-2,1}, {-1,1}, {0,1}, {1,1}, {2,1},
						 {-2,2}, {-1,2}, {0,2}, {1,2}, {2,2}}

g_KillMonsCreateBoss = CKillMonsCreateBoss:new()

function CKillMonsCreateBoss:KillMonsterAddNum(Npc)
	local Scene = Npc.m_Scene
	local SceneName = Scene.m_SceneName
	local SceneId = Scene.m_SceneId
	local NpcName = Npc.m_Name
	
	for _, index in pairs(KillMonsCreateBoss_Common:GetKeys()) do
		local tbl = KillMonsCreateBoss_Common(index)
		local NpcTbl = GetCfgTransformValue(true, "KillMonsCreateBoss_Common", index, "NpcNames")
		for _, npcName in pairs(NpcTbl) do
			if npcName == NpcName and SceneName == tbl("SceneName") then
				if not Scene.KillMonsterNum then
					Scene.KillMonsterNum = {}
				end
				
				if not Scene.KillMonsterNum[index] then
					Scene.KillMonsterNum[index] = 0
				end
				
				Scene.KillMonsterNum[index] = Scene.KillMonsterNum[index] + 1
				
				if Scene.KillMonsterNum[index] == tbl("Number") then
					local fPos = CFPos:new(tbl("PosX") * EUnits.eGridSpan, tbl("PosY") * EUnits.eGridSpan)
					local BossName = tbl("CreateBoss")
					local Level = g_NpcBornMgr:GetNpcBornLevel(BossName)
					g_NpcServerMgr:CreateServerNpc(BossName, Level, Scene, fPos)
					
					Scene.KillMonsterNum[index] = nil
				end
			end
		end
	end
end