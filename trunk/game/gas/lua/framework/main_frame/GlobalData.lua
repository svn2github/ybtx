gas_require "relation/team/GasTeamMgr"
gas_require "relation/team/GasTemporaryTeamMgr"
gas_require "relation/tong/GasTongMgr"
gas_require "relation/army_corps/GasArmyCorpsMgr"
--这个文件里放置所有全局变量

--队伍管理器
	g_TeamMgr = CGasTeamMgr:new()
	g_TemporaryTeamMgr = CTemporaryTeam:new()
	g_GasTongMgr = CGasTongMgr:new()
	g_GasArmyCorpsMgr = CGasArmyCorpsMgr:new()