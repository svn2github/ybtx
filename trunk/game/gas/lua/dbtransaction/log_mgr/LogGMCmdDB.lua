local StmtContainer = class()
local DBName = DeployConfig.MySqlDatabase .. "_log"
local GMLogger = CreateDbBox(...)


--[[
create table tbl_log_gmcommand
(                                        
  lg_sAccountName   varchar(96) not null,
  lg_sIP            varchar(48) not null,
  lg_sCallTime      varchar(192) not null,
  lg_sCmdContent    varchar(384) not null
)engine=myisam;
--]]


--插入GM指令
local StmtDef=
{
	"InsertGMCmd",
	[[
		insert into 
			%s.tbl_log_gmcommand (lg_sAccountName, lg_sIP, lg_sCallTime, lg_sCmdContent) values (?,?,now(),?)
	]]
}
DefineDatabaseSql(StmtDef,StmtContainer,DBName)


--查询所有的
local StmtDef=
{
	"SearchAllByTime",
	[[
		select 
			lg_sAccountName, lg_sIP, lg_sCallTime, lg_sCmdContent 
		from 
			%s.tbl_log_gmcommand
		where 
			lg_sCallTime > date_add(now(), interval -? day)
	]]
}
DefineDatabaseSql(StmtDef,StmtContainer,DBName)

--查询指定用户信息

local StmtDef = 
{
	"SearchLogByName",
	[[
		select 
			lg_sAccountName, lg_sIP, lg_sCallTime, lg_sCmdContent 
		from 
			%s.tbl_log_gmcommand
		where 
			lg_sCallTime > date_add(now(), interval -? day) and lg_sAccountName = ?
	]]
}
DefineDatabaseSql(StmtDef,StmtContainer,DBName)

--查询指定IP信息

local StmtDef = 
{
	"SeachLogByIP",
	[[
		select 
			lg_sAccountName, lg_sIP, lg_sCallTime, lg_sCmdContent 
		from 
			%s.tbl_log_gmcommand
		where 
			lg_sCallTime > date_add(now(), interval -? day) and lg_sIP = ?
	]]
}
DefineDatabaseSql(StmtDef,StmtContainer,DBName)

--删除大于7天的
local StmtDef =
{
	"DeleteGMCmdLog",
	"delete from %s.tbl_log_gmcommand where lg_sCallTime < date_add(now(), interval -7 day)"
}

DefineDatabaseSql(StmtDef,StmtContainer,DBName)



function GMLogger.InsertGmCommandLog(data)
	local account = data['name']
	local ip = data['ip']
	local content = data['content']
	local game_id  = data["game_id"]
	--StmtContainer.InsertGMCmd:ExecSql("",account, ip, content)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveGMLog(account, ip, content)
	local lGMDBExecutor = RequireDbBox("GMDB")
	local nGMLevel = lGMDBExecutor.GetGmLevel({["user_name"] = account,["game_id"] = game_id})
	return nGMLevel
end

function GMLogger.SelectAll(data)
	local days = data['days']
	local res = StmtContainer.SearchAllByTime:ExecSql("s[32]s[16]s[32]s[128]", days)
	
	return res
end

function GMLogger.SelectByName(data)
	local days = data['days']
	local name = data['name']
	local res = StmtContainer.SearchLogByName:ExecSql("s[32]s[16]s[32]s[128]", days, name)

	return res
end

function GMLogger.SelectByIP(data)
	local days = data['days']
	local ip = data['ip']
	local res = StmtContainer.SeachLogByIP:ExecSql("s[32]s[16]s[32]s[128]", days, ip)
	
	return res
end

return GMLogger
