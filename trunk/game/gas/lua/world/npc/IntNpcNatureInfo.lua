cfg_load "npc/NpcNature_Server"

g_NatureArg2NpcTbl = {}		--记录个性中参数关注NPC的所有NPC名称
g_NatureArg2ObjTbl = {}		--记录个性中参数关注OBJ的所有OBJ名称

function _GetNatureInfo()
	for _, i in pairs(NpcNature_Server:GetKeys()) do
		for _, j in pairs(NpcNature_Server:GetKeys(i)) do
			local info = NpcNature_Server(i, j)
			if j == "相聚" or j == "怯懦" then
				if g_NatureArg2NpcTbl[info("NatureArg")] == nil then
					g_NatureArg2NpcTbl[info("NatureArg")] = 1
				end
			elseif j == "喜好" or j == "厌恶" or j == "吸引" then
				if g_NatureArg2ObjTbl[info("NatureArg")] == nil then
					g_NatureArg2ObjTbl[info("NatureArg")] = 1
				end
			end
		end
	end
end

_GetNatureInfo()