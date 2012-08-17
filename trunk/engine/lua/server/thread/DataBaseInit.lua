local DeployFileName = CBaseAppConfigServer_Inst():GetString("deploy_filename")

etc_gas_require(DeployFileName)

local password = DeployConfig.MySqlUser .. DeployConfig.MySqlPassWord
local MD5Digester = CMd5Digester:new()
MD5Digester:Begin()
MD5Digester:Append(password, string.len(password))
local passwd_MD5 = CreateCBufAcr( CreateCBuf(33) )
MD5Digester:GetMD5Base16Str(passwd_MD5)
local password = passwd_MD5:ToString()

local DbConnNum = CBaseAppConfigServer_Inst():GetNumber("db_conn_num_per_vm")

g_DbChannelMgr = CDbChannelMgr:new(
	DbConnNum,
	DeployConfig.MySqlIP,
	DeployConfig.MySqlPort,
	DeployConfig.MySqlUser,
	password,
	DeployConfig.MySqlDatabase,
	GasConfig.MaxDBResultSize
)

function DefineSql( StmtDef , Table )
	--如果statement创建失败，直接退出了
	local ret, msg = pcall(CDbChannelMgr.CreateStatement, g_DbChannelMgr, StmtDef, Table)
	if not ret then
		local err = "数据库创建Statement失败\n" .. msg .. "\n可能需要重建数据库"
		ShowErrorMsgAndExit(err)
	end
end

function DefineDatabaseSql(StmtDef, Table, DBName)
	local name, query_str = StmtDef[1], StmtDef[2]
	query_str = string.format(query_str, DBName)
	QueryTable = {name, query_str}
	local ret, msg = pcall(CDbChannelMgr.CreateStatement, g_DbChannelMgr, QueryTable, Table)
	if not ret then
		local err = "数据库创建Statement失败\n" .. msg .. "\n可能需要重建数据库"
		ShowErrorMsgAndExit(err)
	end
end

local g_queTablePool = CQueue:new()

function GetTblFromPool()
	local table
	if g_queTablePool:empty() then
		table = {}
	else
		table = g_queTablePool:front()
		g_queTablePool:pop()
	end
	return table
end

function ReturnTblToPool(table)
	for k, v in pairs(table) do
		table[k] = nil
	end
	g_queTablePool:push(table)
end
