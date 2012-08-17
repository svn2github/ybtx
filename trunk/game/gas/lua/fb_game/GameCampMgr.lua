gas_require "fb_game/GameCampMgrInc"

local MaxAssignCampNum = 1000

local function AddCampSign(Scene)
	if Scene.m_GameCampSign < MaxAssignCampNum then
		Scene.m_GameCampSign = Scene.m_GameCampSign + 1
	else
		Scene.m_GameCampSign = 2
	end
end

--分配玩法阵营
function CGameCampMgr:AssignCamp(Scene, tbl)
	if Scene.m_GameCampTbl == nil then
		Scene.m_GameCampTbl = {}
		RegMemCheckTbl(Scene.m_SceneId.."_"..Scene.m_SceneName.."_GameCampTbl", Scene.m_GameCampTbl)
		Scene.m_GameCampSign = 1
	end
	for i = 1, MaxAssignCampNum do
		AddCampSign(Scene)
		if Scene.m_GameCampTbl[Scene.m_GameCampSign] == nil then
			Scene.m_GameCampTbl[Scene.m_GameCampSign] = tbl
			return Scene.m_GameCampSign
		end
	end
end

--场景内玩法阵营删除
function CGameCampMgr:CleanGameCamp(Scene)
	if Scene.m_GameCampTbl then
		for i, v in pairs(Scene.m_GameCampTbl) do
			v = nil
		end
		Scene.m_GameCampTbl = nil
	end
end
