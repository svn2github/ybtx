local StmtContainer = class()

local StmtDef = {
	"_GetMailTextAttachment",
	[[
		select 
			unix_timestamp(iml.iml_tCreateTime), iml.cs_uSenderName, 
			iml.iml_sTitle, iml.iml_uMailContext, iml.iml_nType,iml.iml_nStrType
		from 
			tbl_item_mail_letter iml, tbl_item_in_grid isr,tbl_grid_in_room as igp
		where 
			igp.gir_uGridID = isr.gir_uGridID and  iml.is_uID = isr.is_uID and igp.cs_uId= ? 
			and gir_uRoomIndex = ? and gir_uPos = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = {
	"_GetMailTextAttachmentByID",
	[[
		select 
			cs_uSenderName, iml_sTitle
		from 
			tbl_item_mail_letter
		where 
			is_uId = ?
		
	]]
}
DefineSql(StmtDef, StmtContainer)


local MailItemDB = CreateDbBox(...)

function MailItemDB.SendMailTextAttachment(data)
	local player_id, room_index, pos = data["player_id"], data["room_index"], data["pos"]
	local ret = StmtContainer._GetMailTextAttachment:ExecStat(player_id, room_index, pos)
	return ret
end

function MailItemDB.GetMailTextAttachmentByID(nItemID)
	local query_result = StmtContainer._GetMailTextAttachmentByID:ExecStat(nItemID)
	local row = query_result:GetRowNum()
	if row == 0 then
		return {"ÐÅ¼þ","Î´Öª"}
	else
		return {query_result:GetData(0,1),query_result:GetData(0,0)}
	end
end

SetDbLocalFuncType(MailItemDB.SendMailTextAttachment)
return MailItemDB