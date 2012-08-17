cfg_load "fb_game/MatchGame_Common"
g_MonsterComeTimeTbl = {}
local function LoadMatchGame()
	for _, gameName in pairs(MatchGame_Common:GetKeys()) do
		local Info = MatchGame_Common(gameName)
		if Info ~= nil then
			if Info("MonsterComeTime") ~= "" then
				local TempTbl = loadstring("return {" .. Info("MonsterComeTime") .. "}")()
				local MaxIndex = table.maxn(TempTbl)
				local TotalTime = TempTbl[MaxIndex][2]
				if TotalTime == nil then
					TotalTime = TempTbl[MaxIndex - 1][2]
				end
				TempTbl.m_TotalTime = TotalTime
				g_MonsterComeTimeTbl[gameName] = TempTbl
			end
		end
	end
end

LoadMatchGame()