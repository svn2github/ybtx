local StmtSql = class()

local UserAdviceDB = CreateDbBox(...)
--------------------------------------------------------------------------------------------------------
local StmtDef = {
		"_AddLogCode",
		[[	insert into tbl_user_advice (cs_uId,ua_sIp,ua_sErrorMsg,ua_nMsgType,ua_dtTime) values (?,?,?,?,now()) ]]
}
DefineSql( StmtDef, StmtSql )

function UserAdviceDB.GetUserLogCode(data)
	StmtSql._AddLogCode:ExecStat(data.m_uCharId,data.m_sIP,data.m_sMsg,data.m_uType)
end

SetDbLocalFuncType(UserAdviceDB.GetUserLogCode)

return UserAdviceDB






