local StmtOperater = class()
local LogErr = LogErr


local StmtDef = 
{
	"GetMsgId",
	"select cm_uId from tbl_client_msg where cs_uId = ? and cm_sTypeName = ? "
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"CreateMsgId",
	[[
	insert into tbl_client_msg(cs_uId, cm_sTypeName) values(?,?)
	on duplicate key update cm_uId = cm_uId + 1
	]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef = 
{
	"ClearRecordById",
	"delete from tbl_client_msg where cs_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"ClearOnLineCharMsgByServerId",
	[[
		delete cmr 
		from tbl_client_msg as cmr,tbl_char_online as co
	  where cmr.cs_uId = co.cs_uId and co.co_uOnServerId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"ClearOffLineCharMsg",
	[[
		delete cmr 
		from tbl_client_msg as cmr
	  where cmr.cs_uId not in (select cs_uId from tbl_char_online)
	]]
}
DefineSql(StmtDef,StmtOperater)


local ClientMsgDB = CreateDbBox(...)

function ClientMsgDB.ClearClientMsg(serverId)
	StmtOperater.ClearOnLineCharMsgByServerId:ExecStat(serverId)
	StmtOperater.ClearOffLineCharMsg:ExecStat()
end

function ClientMsgDB.ClearRecordById(charId)
	StmtOperater.ClearRecordById:ExecStat(charId)
end

function ClientMsgDB.GetMsgState(charId, typeName)
	StmtOperater.CreateMsgId:ExecStat(charId, typeName)
	local result = StmtOperater.GetMsgId:ExecStat(charId, typeName)
	return result:GetData(0,0)
end

function ClientMsgDB.TestDb2Gac(data)
	Db2GacById:TestDb2Gac( "a", 100000, g_CurServerId, "aaaa1")
	DbMsgToConnById("a", 100000, 191036, "½Ç¶·³¡")
end

return ClientMsgDB
