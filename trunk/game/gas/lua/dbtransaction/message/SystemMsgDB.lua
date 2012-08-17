local SystemMsgSql = class()

local SystemMsgDB = CreateDbBox(...)
--------------------------------------------------------------------------------------------------------
local StmtDef = {
		"Insert_System_Message",
		[[insert into tbl_system_message (cs_uId,sm_uMsgId) values (?,?) ]]
}
DefineSql( StmtDef, SystemMsgSql )

--@brief 保存玩家不在线的系统信息
--@param uCharId:玩家Id
--@param content：系统消息内容
function SystemMsgDB.SaveSystemMsg(data)
	local uCharId = data["uCharId"]
	local messageId = data["messageId"]
	if uCharId == 0 or messageId == 0 then
		return
	end
	SystemMsgSql.Insert_System_Message:ExecSql('',uCharId,messageId)
end

--------------------------------------------------------------------------------------------------------
local StmtDef = {
	"Select_System_Message",
	[[ select sm_uMsgId from tbl_system_message where cs_uId = ?]] 
}
DefineSql(StmtDef, SystemMsgSql)

local StmtDef = {
	"Delete_System_Message",
	[[ delete from tbl_system_message where cs_uId = ?]] 
}
DefineSql(StmtDef, SystemMsgSql)

--@brief 上线向客户端发送系统消息
function SystemMsgDB.SendSystemMsg(uCharID)
	--根据玩家id查询系统消息
	local system_msg_set = SystemMsgSql.Select_System_Message:ExecStat(uCharID)
	--查询完消息后删除已经查看过的系统消息
	SystemMsgSql.Delete_System_Message:ExecStat(uCharID)
	return system_msg_set
end

return SystemMsgDB






