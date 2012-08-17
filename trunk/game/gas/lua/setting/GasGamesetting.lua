CGameSetting = class()

function CGameSetting.GameSetting(Conn,team_setting,trans_setting,group_setting,friend_setting,troop_setting,autoMakeTeam_setting,selectQuestSort_setting)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	Player.tblSysSettingInfo = Player.tblSysSettingInfo or {}
	
  local parameters = {}
	parameters["uPlayerID"]	= Player.m_uID
	parameters["team_setting"]	= team_setting
	parameters["trans_setting"]	= trans_setting
	parameters["group_setting"]	= group_setting
	parameters["friend_setting"]	= friend_setting
	parameters["troop_setting"]	= troop_setting
	parameters["autoMakeTeam_setting"] = autoMakeTeam_setting
	parameters["selectQuestSort_setting"] = selectQuestSort_setting
	
	Player.tblSysSettingInfo["GameSetting"] = parameters
end

--得到玩家游戏设置信息
function CGameSetting.GetGameSettingInfo(Conn)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	local uPlayerID = Player.m_uID
	--回调函数【高速逻辑】
	local function CallBack(result)
		Gas2Gac:ReturnGetGameSettingInfo(Conn,unpack(result))
	end
	
  local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	
	--local setting_box = (g_DBTransDef["GameSettingDB"])
	CallAccountManualTrans(Conn.m_Account, "GameSettingDB", "GetGameSettingInfoDB", CallBack, parameters)
end