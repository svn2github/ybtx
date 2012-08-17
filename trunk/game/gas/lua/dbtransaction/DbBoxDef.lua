engine_require "common/Module/Module"

engine_require "server/thread/DbChannelMgr"
gas_require "dbtransaction/DbChannel"
engine_require "server/thread/DbTransaction"
gas_require "dbtransaction/DbCallGas"
gas_require "dbtransaction/CallDbTransDef"

local g_DBTransDef = g_DBTransDef


function RequireDbBox(modname)
	local filename = g_DBTransDef[modname] 
	if not filename then
		error(modname .. " not a valid key in g_DBTransDef table")
	end

	local ret, msg = apcall(gas_require, filename)
	return msg		

end


local DbBoxMt = GetModuleMt(true)

DbBoxMt.g_CurServerId = g_CurServerId
DbBoxMt.IsNumber = IsNumber
DbBoxMt.IsString = IsString
DbBoxMt.IsTable = IsTable
DbBoxMt.CPos 				= CPos
DbBoxMt.g_DbChannelMgr = g_DbChannelMgr
DbBoxMt.CDbResultSet = CDbResultSet
DbBoxMt.DefineSql = DefineSql
DbBoxMt.RequireDbBox = RequireDbBox
DbBoxMt.g_DBTransDef = g_DBTransDef
DbBoxMt.AddDbProcessResult = AddDbProcessResult
DbBoxMt.Db2GasCall = Db2GasCall
DbBoxMt.Db2CallBack = Db2CallBack
DbBoxMt.Db2GacById = Db2GacById
DbBoxMt.DbMsgToConnById = DbMsgToConnById
DbBoxMt.CheckFuncType = CheckDbFuncType
DbBoxMt.SetDbGlobalFuncType = SetDbGlobalFuncType
DbBoxMt.SetDbLocalFuncType = SetDbLocalFuncType
DbBoxMt.DefineDatabaseSql = DefineDatabaseSql
DbBoxMt.CancelTran = CancelTran

 
function CreateDbBox(DbBoxName)
	local ret = Module(DbBoxName, DbBoxMt, 3)
	return ret
end

local function RequireDbBoxFile()
	if GasConfig.RequireDbBoxFile and GasConfig.RequireDbBoxFile == 1 then
		for i,p in pairs(g_DBTransDef) do
			 RequireDbBox(i)
		end
	end
end
RequireDbBoxFile()

AddCheckLeakFilterObj(CreateDbBox)
