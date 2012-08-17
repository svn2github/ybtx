gac_require "relation/team/TeamClientMgrHandlerInc"

function TeamClientMgrHandler:UpdateTeamInfo()
	g_GameMain.m_TeamBase:UpdateSyncInfoAll()
end
