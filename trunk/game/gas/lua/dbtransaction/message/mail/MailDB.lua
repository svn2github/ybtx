gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "framework/common/CMoney"
local g_MoneyMgr = CMoney:new()
local event_type_tbl = event_type_tbl
local StmtContainer = class()
local g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()

local MailExecutor = CreateDbBox(...)

local MailState = {
	UnRead = 1,								--有附件未读
	UnTakeAttachment = 2,			--有附件已读
	UnReadAttachment = 3,			--无附件未读
	TakeAttachment = 4,				--无附件已读
}

local StmtDef = {
	"_GetOvertimeMailBySenderID",
	[[
		select 
			ms_uID,cs_uReciever,m_sTitle
		from 
			 tbl_mail
		where 
			cs_uSenderID = ?
			and (m_state = ? or m_state = ?)
			and unix_timestamp(now()) - unix_timestamp(m_tUpdateTime) >= ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetAllOvertimeMail",
	[[
		select ms_uID,cs_uReciever,m_uMoney,m_state,m_sTitle,cs_uSenderID from tbl_mail
	where unix_timestamp(now()) - unix_timestamp(m_tCreateTime) >= ? limit 500
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_UpdateOvertimeMailInfo",
	[[
		update tbl_mail 
		set cs_uReciever = ?,cs_uSenderID = null,cs_sSender = ?,m_tUpdateTime = now(),m_tCreateTime = now(),
				m_sTitle = ?,m_nMailType = ?,m_uLogMailType = ?,m_nStrType = 1
		where 
			ms_uID = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_SelectMailGoodsInfo",
	[[
		select 
			s.is_uType,s.is_sName 
		from 
			 tbl_item_static s,tbl_item_in_grid sr,tbl_grid_in_room as igp
		where 
			s.is_uId = sr.is_uId
			and sr.gir_uGridID = igp.gir_uGridID
			and igp.cs_uId = ?
			and sr.is_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )


local StmtDef = {
 	"SelectMailGoodsStmt",
 	[[ 
		select 
			s.is_uId, mi_uSlotIndex, is_uType, is_sName,ifnull(isb_bIsbind,0) 
		from 
			tbl_mail as ms,tbl_mail_item as m, tbl_item_static as s
		left join tbl_item_is_binding as isb
		 on(s.is_uId= isb.is_uId)
		where 
			m.ms_uID=? and s.is_uId=m.is_uId and m.ms_uId = ms.ms_uID and ms.cs_uReciever=?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"SelectMailSlotGoodsStmt",
	[[
		select 
			s.is_uId, is_uType, is_sName 
		from 
			tbl_mail_item as m, tbl_item_static as s, tbl_mail as ms
		where 
			m.ms_uID=? and s.is_uId=m.is_uId and mi_uSlotIndex=?
			and m.ms_uId = ms.ms_uID and ms.cs_uReciever=?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_DeleteSlotItems",
	[[
		delete 
		from 
			tbl_mail_item
		where 
			ms_uID=? and mi_uSlotIndex=?
	]]
}
DefineSql( StmtDef, StmtContainer )

--判断是否有邮件
local StmtDef = {
	"GetMailIdStmt",
	[[ select ms_uID from tbl_mail where cs_uReciever=? and  m_state in (?,?) and unix_timestamp(now()) - unix_timestamp(m_tUpdateTime) >= ?]]
}
DefineSql( StmtDef, StmtContainer )
local StmtDef = {
	"SeleteMailStmt",
	[[
		select 
			ms_uID, ifnull(cs_uSenderID,0), cs_uReciever, m_sTitle, m_sContent, m_state, m_uMoney,m_tCreateTime,m_tUpdateTime, ifnull(cs_sSender,1014),m_nMailType,m_nStrType
		from 
			 tbl_mail as m
			 left join tbl_char as c
			 on(c.cs_uId = m.cs_uSenderID)
		where 
			cs_uReciever = ? and ms_uID = ?	
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_HasMailItem",              --获取邮件物品栏每个格子里的物品
	[[ select count(1) from tbl_mail_item where ms_uID=?]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
	"_GetMailItemByMailId",              --获取邮件物品栏每个格子里的物品
	[[ select is_uId from tbl_mail_item where ms_uID= ?]]
}
DefineSql (StmtDef,StmtContainer)

local StmtDef = {
	"ChangeMailState",
	"update tbl_mail set m_state = ?, m_tUpdateTime = now() where ms_uID = ? and cs_uReciever = ? "
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"SelecteMailListStmt",
	[[
		select 
			ms_uID, ifnull(cs_uSenderID,0), m_sTitle, m_state, unix_timestamp(m_tUpdateTime),ifnull(cs_sSender,1014),m_nMailType,m_uMoney
		from 
			tbl_mail m
			left join tbl_char c
			on(c.cs_uId = m.cs_uSenderID)
		where 
			cs_uReciever = ?
			and m_tCreateTime <  date_add(now(), interval -1 second)
		order by m_state asc,m_tUpdateTime desc
		limit 100
	]]
}
DefineSql( StmtDef, StmtContainer )
local StmtDef={
	"_GetMailMoney",
	"select m_uMoney,m_uLogMailType, cs_uSenderID from tbl_mail where ms_uID=? and cs_uReciever =? "
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"UpdateMailMoneyStmt",
	[[ update tbl_mail set m_uMoney=? where ms_uID=? and cs_uReciever =? ]]
}
DefineSql( StmtDef, StmtContainer )
local StmtDef = {
	"CountMailLetterByID",
	[[ select count(*) from tbl_item_mail_letter where ms_uID = ? ]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"CreateItemLetterStmt",
	[[ insert ignore into tbl_item_mail_letter(is_uId,iml_uMailContext,cs_uSenderName,cs_uRecieverName,iml_sTitle,iml_tCreateTime,iml_nType,ms_uID,iml_nStrType)
	 values(?,?,?,?,?,?,?,?,?) ]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"DeleteMailStmt",
	"delete from tbl_mail where ms_uID = ? and cs_uReciever = ? "
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"DelMailByID",
	"delete from tbl_mail where ms_uID = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetAllFriendsName",
	[[
		select c.c_sName from tbl_player_friends as pf,tbl_char as c
		where c.cs_uId = pf.pf_uFriendId and pf.fc_uId <> 2 and pf.cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"AddMailItemStmt",                    --添加到收件人邮件物品栏
	[[ insert into tbl_mail_item (ms_uID,is_uId,mi_uSlotIndex) values(?,?,?) ]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"AddMailStmt",
	[[
		insert into tbl_mail (cs_uSenderID,cs_sSender, cs_uReciever, m_sTitle, m_sContent, 
		m_uMoney, m_state, m_tCreateTime, m_tUpdateTime,m_nMailType,m_uLogMailType,m_nStrType) values (?,?, ?, ?, ?, ?, ?, now(),
		now(),?,?,1)
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
	"_GetMailCountByCharID",
	[[
		select count(*) from tbl_mail where cs_uReciever = ? and cs_uSenderID is not null
	]]
}
DefineSql( StmtDef, StmtContainer )

function MailExecutor.MailIsFull(char_id)
	local res =  StmtContainer._GetMailCountByCharID:ExecStat(char_id)
	local mail_count = res:GetData(0,0)
	return mail_count >= 100
end

local function DealWithOvertimeMailBySenderID(nCharID)
	local res =  StmtContainer._GetOvertimeMailBySenderID:ExecSql("nns[96]", nCharID, MailState.UnRead,MailState.UnReadAttachment,30*24*60*60)
	local row = res:GetRowNum() 
	if row == 0 then 
		return 
	end
	for i=1,row do
		local mail_id,resiver_id,mail_title = res:GetNumber(i-1,0),res:GetNumber(i-1,1),res:GetString(i-1,2)
		local ex = RequireDbBox("Exchanger")
		local sRecieverName = ex.getPlayerNameById(resiver_id)
		local new_title = "1024_" .. sRecieverName .. "," .. mail_title --"过期退回邮件【TO->" .. sRecieverName .. ":" .. mail_title .. "】"
		StmtContainer._UpdateOvertimeMailInfo:ExecSql("", nCharID,"1014",new_title,3,106, mail_id)
	end
end

local function UpdataMailMoney(char_id, mail_id, money)
	StmtContainer.UpdateMailMoneyStmt:ExecSql("", money, mail_id, char_id)
	return g_DbChannelMgr:LastAffectedRowNum() >= 0
end

local function GetMailMoney(mail_id, char_id)
	local ret = StmtContainer._GetMailMoney:ExecStat(mail_id, char_id)
	if ret:GetRowNum() == 0 then
		return 0
	end
	local money,logMailType = ret:GetNumber(0, 0),ret:GetNumber(0, 1)
	ret:Release()
	return money,logMailType
end
function MailExecutor.IfHaveUnReadMail(char_id)
	local mail=StmtContainer.GetMailIdStmt:ExecSql("n",char_id,MailState.UnRead,MailState.UnReadAttachment,30*24*60*60)
	return mail:GetRowNum() > 0
end

function MailExecutor.GetMail(uPlayerId, uMailId)
	local result_set = StmtContainer.SeleteMailStmt:ExecStat(uPlayerId, uMailId)
	
	return result_set
end

function MailExecutor._HasMailItem(mail_id,char_id)
	local ret = StmtContainer._HasMailItem:ExecSql('n',mail_id)
	local num = ret:GetData(0,0)
	ret:Release()
	local mail_money = GetMailMoney(mail_id, char_id)
	return (num > 0 or mail_money > 0) 
end

function MailExecutor.GetMailGoods(mail_id, char_id)
	
	local ret = StmtContainer.SelectMailGoodsStmt:ExecStat(mail_id, char_id)
	if not ret then
		return {}
	end
	local count=ret:GetRowNum()
	local info = {}
    local GetItemDynInfoBox = RequireDbBox("GetItemDynInfoDB")
	local tbl_id = {}
	for i=1,count do
		table.insert(info,{ret(i,1),ret(i,2),ret(i,3),ret(i,4),ret(i,5)})
		local nItemID = ret:GetNumber(i-1,0)
		local item_type = ret(i,3)
		GetItemDynInfoBox.GetItemDynInfo(nItemID, item_type, char_id,char_id, {nItemID})
	end

	return info
end

function MailExecutor.ChangeState(char_id, mail_id, state)
	StmtContainer.ChangeMailState:ExecSql("", state, mail_id, char_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function MailExecutor.RecieveMail(data)
	local char_id = data["char_id"]
	local mail_id = data["mail_id"]
	local mail_info = MailExecutor.GetMail(char_id, mail_id)
	if mail_info:GetRowNum() == 0 then
		return
	end
	local state = 0
	if MailExecutor._HasMailItem(mail_id,char_id) then
		state = MailState.UnTakeAttachment
	else
		state = MailState.TakeAttachment		
	end
	if state ~= mail_info(1,6) then
		MailExecutor.ChangeState(char_id, mail_id, state)
	end
	return {mail_info, MailExecutor.GetMailGoods(mail_id, char_id), MailExecutor.IfHaveUnReadMail(char_id),state}
end

function MailExecutor.DelAllOvertimeMail(data)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local over_time = 30*24*60*60
	local event_type = event_type_tbl["过期邮件删除"]
	local ex = RequireDbBox("Exchanger")
	local del_time = 0
	while(true) do
		local result_set = StmtContainer._GetAllOvertimeMail:ExecStat(over_time)
		local row = result_set:GetRowNum()
		if row == 0 or del_time >= 1000 then
			break
		end
		del_time = del_time + 1 
		for i =1,row do
			local mail_id = result_set(i,1)
			local char_id = result_set(i,2)
			local have_money = result_set(i,3)
			local mail_state = result_set(i,4)
			local mail_title = result_set(i,5)
			local sender_id = result_set(i,6)
			if sender_id and (mail_state == MailState.UnRead or mail_state == MailState.UnReadAttachment) then
				--退回
				local sRecieverName = ex.getPlayerNameById(char_id)
				local new_title = "1024_" .. sRecieverName .. "," .. mail_title
				StmtContainer._UpdateOvertimeMailInfo:ExecStat(sender_id,"1014",new_title,3,106, mail_id)
			else
				local tbl_mail_item = StmtContainer._GetMailItemByMailId:ExecStat(mail_id)
				for j =1,tbl_mail_item:GetRowNum() do
					local succ = g_RoomMgr.DelItemFromStaticTable(tbl_mail_item(j,1),char_id,event_type)
					if not succ then
						error("记录过期邮件删除log失败")
					end
				end
				if have_money > 0 then
					g_LogMgr.SaveTaxLog(have_money,char_id,event_type)
				end
				StmtContainer.DelMailByID:ExecStat(mail_id)
				if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
					error("删除过期邮件失败")
				end
			end
		end
	end
end

function MailExecutor.GetMailList(data)
	local char_id = data["char_id"]
	DealWithOvertimeMailBySenderID(char_id)
	local result_set = StmtContainer.SelecteMailListStmt:ExecStat(char_id)
	local row = result_set:GetRowNum()
	local info = {}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")
	local dbTime = ItemLifeMgrDB.GetUnixTimeFromDB()
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local ex = RequireDbBox("Exchanger")
	for i = 1, row do
		local up_time = result_set:GetData(i-1,4)
		local state = result_set:GetData(i-1,3)
		local leftdate = 30*24*60*60
		if state == MailState.UnTakeAttachment then
			leftdate = 7*24*60*60
		elseif state == MailState.TakeAttachment then
			leftdate = 3*24*60*60
		end
		local past_time = dbTime - up_time
		local mail_id = result_set:GetData(i-1,0)
		local have_money = result_set:GetData(i-1,7)
		local sender_id = result_set:GetData(i-1,1)
		local mail_title = result_set:GetData(i-1,2)
		local sender,mail_type = result_set:GetData(i-1,5),result_set:GetData(i-1,6)
		if past_time < leftdate then
			table.insert(info,{mail_id,sender_id,mail_title,state,sender,mail_type,leftdate-past_time})
		else
			if sender_id and sender_id > 0 and (state == MailState.UnRead or state == MailState.UnReadAttachment) then
				local sRecieverName = ex.getPlayerNameById(char_id)
				local new_title = "1024_" .. sRecieverName .. "," .. mail_title
				StmtContainer._UpdateOvertimeMailInfo:ExecStat(sender_id,"1014",new_title,3,106, mail_id)
			else
				local tbl_mail_item = StmtContainer._GetMailItemByMailId:ExecStat(mail_id)
				if tbl_mail_item:GetRowNum() > 0 then
					for j =1,tbl_mail_item:GetRowNum() do
						local succ = g_RoomMgr.DelItemFromStaticTable(tbl_mail_item(j,1),char_id,event_type_tbl["过期邮件删除"])
						if not succ then
							error("记录过期邮件删除log失败")
						end
					end
				end
				if have_money > 0 then
					g_LogMgr.SaveTaxLog(have_money,char_id,event_type_tbl["过期邮件删除"])
				end
				StmtContainer.DelMailByID:ExecStat(mail_id)
				if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
					error("删除过期邮件失败")
				end
			end
		end
	end
	result_set:Release()
	local bFull = MailExecutor.MailIsFull(char_id)
	info.m_bFull = bFull
	return info
end

local function GetMailItem(mail_id,slot_index,char_id)
	local ret=StmtContainer.SelectMailSlotGoodsStmt:ExecStat(mail_id,slot_index,char_id) 
	local slot_item_num=ret:GetRowNum()
	if slot_item_num == 0 then
		return {{}}
	end

	local tbl_item_ids = {}
	for i = 1, slot_item_num do
		table.insert(tbl_item_ids, {ret(i,1)})
	end
	
	local params = {}
		params.m_nCharID = char_id
		params.m_nItemType = ret(1,2)
		params.m_sItemName = ret(1,3)
		params.m_tblItemID = tbl_item_ids
	local lPutItem = RequireDbBox("CPutItemToBagDB")
	local succ,info=lPutItem.AddCountItem(params)
	if(not succ) then
		CancelTran()
		ret:Release()
		return {false,info}
	end
	
	StmtContainer._DeleteSlotItems:ExecSql("", mail_id, slot_index)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		return {}
	end
	
	local state = 0
	if MailExecutor._HasMailItem(mail_id,char_id) then
		state = MailState.UnTakeAttachment
	else
		state = MailState.TakeAttachment		
	end
	
	MailExecutor.ChangeState(char_id, mail_id, state)
		
	return {info, state, tbl_item_ids}
end

function MailExecutor.GetMailItem(data)
	local mail_id,slot_index,char_id = data["mail_id"], data["slot_index"], data["char_id"]
	local ret = GetMailItem(mail_id,slot_index,char_id)
	return ret
end

function MailExecutor.GetAllMailItem(parameter)
	local mail_id,char_id = parameter["mail_id"],parameter["char_id"]
	local one_money,logMailType = GetMailMoney(mail_id, char_id)
	local bGetItemSucc,uMsgID = true,nil
	for i = 1, 4 do
		local ret = GetMailItem(mail_id,i,char_id)
		if not ret[1] then
			bGetItemSucc,uMsgID = false,ret[2]
			break
		end
	end
	
	local bMoneySucc = false
	if one_money > 0 then
		local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
		local fun_info = g_MoneyMgr:GetFuncInfo("Mail")
		local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["GetMoney"],char_id, one_money,nil,logMailType)
		if not bFlag then
			CancelTran()
			return bFlag,uMsgID
		end
		if not UpdataMailMoney(char_id, mail_id, 0) then
			CancelTran()
			return false
		end
		bMoneySucc = true
	end	
	return bGetItemSucc,uMsgID,bMoneySucc
end

function MailExecutor.CreateLetter(data)
	local playerid, mailid, playername,SceneName = data["playerid"], data["mailid"], data["playername"],data["SceneName"]
	local mail_info = MailExecutor.GetMail(playerid, mailid)
	local tblCount = StmtContainer.CountMailLetterByID:ExecStat(mailid)
	if tblCount:GetData(0,0) > 0 then
		return false,4021
	end
	local nBigID,sName = g_ItemInfoMgr:GetMailBigID(),g_ItemInfoMgr:GetMailItemName()
	local params= {}
	params.m_nType = nBigID
	params.m_sName = sName
	params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(nBigID,sName,"BindingStyle") or 2
	params.m_nCharID = playerid
	params.m_sCreateType = event_type_tbl["新邮件"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_mail_id=g_RoomMgr.CreateItem(params)
	if not item_mail_id then 
		CancelTran()
		return false
	end
	local item_ids={}
	table.insert(item_ids,{item_mail_id})
	local lPutItem = RequireDbBox("CPutItemToBagDB")
	StmtContainer.CreateItemLetterStmt:ExecSql('',item_mail_id,mail_info(1,5),mail_info(1,10), playername,mail_info(1,4),mail_info(1,8),mail_info(1,11),mailid,mail_info(1,12))
  if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
   	CancelTran()
   	return false
  end
  
  local params2 = {}
	params2.m_nCharID = playerid
	params2.m_nItemType = nBigID
	params2.m_sItemName = sName
	params2.m_tblItemID = item_ids
	local succ,info=lPutItem.AddCountItem(params2)
	if( not succ) then
		CancelTran()
   	return false,info
	end
	
	return info
end 	                     

function MailExecutor.GetMailMoney(data)
	local mail_id,char_id = data["mail_id"], data["char_id"]
	local mail_money,logMailType = GetMailMoney(mail_id, char_id)
	if mail_money > 0 then
		local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	  local fun_info = g_MoneyMgr:GetFuncInfo("Mail")
		local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["GetMoney"],char_id, mail_money,nil,logMailType)
		if not bFlag then
			CancelTran()
			return {bFlag,uMsgID}
		end
	end
	
	if not UpdataMailMoney(char_id, mail_id, 0) then
		CancelTran()
		return {false}
	end
	return {true}
end

function MailExecutor.DeleteMail(parameter)
	local mail_id,char_id = parameter["mail_id"], parameter["char_id"]
	if MailExecutor._HasMailItem(mail_id,char_id) then
		return 4024
	end
	StmtContainer.DeleteMailStmt:ExecSql("", mail_id, char_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function MailExecutor.SendMailContent(data)
	local ex = RequireDbBox("Exchanger")
	local reciever_name = data["reciever_name"]
	local reciever_id = ex.getPlayerIdByName(reciever_name)
	if reciever_id == 0 then
		return { false, 4001, reciever_name }
	end
	
	local FriendDB = RequireDbBox("FriendsDB")
	if FriendDB.IsBlackName(reciever_id, data["player_id"]) then
		return {false, 10}
	end
	
	return {true, ["reciever_id"] = reciever_id}
end

function MailExecutor.SendMail(sSenderName, uReciever, sMailTitle, sMailContent, uMoney,item_info,nSenderID,MailType,logMailType,msgID)
	assert(IsNumber(logMailType),"邮件logMailType参数为空")
	local state = MailState.UnReadAttachment
	if (uMoney and uMoney > 0) or (IsTable(item_info) and # item_info > 0) then
		state = MailState.UnRead
	end
	StmtContainer.AddMailStmt:ExecSql("",nSenderID, sSenderName, uReciever, 
					sMailTitle, sMailContent, (uMoney or 0), state,MailType,logMailType)
	local mailid = g_DbChannelMgr:LastInsertId()
	if mailid == 0 then
		return {false, 106, ""}
	end
	--如果收件人不在线，将小助手提示消息保存数据库。
	local LoginServerSql = RequireDbBox("LoginServerDB")
	local SystemMsgDB = RequireDbBox("SystemMsgDB")

	if not LoginServerSql.IsPlayerOnLine(uReciever)  then
		local parameter = {
											["uCharId"] = uReciever,
											["messageId"]= (msgID or 5001 )
										}
		SystemMsgDB.SaveSystemMsg(parameter)
	else
		if msgID then
			Db2CallBack("SystemFriendMsgToClient", uReciever, msgID)
		end
	end
	return {mailid}
end

function MailExecutor.AddMailItem(mail_id,goods_id,mail_grid)
	StmtContainer.AddMailItemStmt:ExecSql('',mail_id,goods_id,mail_grid)
	return g_DbChannelMgr:LastAffectedRowNum()>0
end

function MailExecutor.SendMailGoods(player_id, mail_id,goods_id,mail_grid, reciever_id)
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local index_info = lRoomMgr.GetRoomIndexAndPosByItemId(player_id, goods_id)
	if index_info == nil then
		return false
	end
	
	local roomindex = index_info(1,1)
	local pos = index_info(1,2)
	
 --为了防外挂，所以要带角色id
	local goods_info = StmtContainer._SelectMailGoodsInfo:ExecSql('ns[32]',player_id, goods_id) 
	
	if goods_info:GetRowNum() == 0 then
		return false
	end
	local stype1, name1 = goods_info:GetNumber(0,0), goods_info:GetString(0,1)
	goods_info:Release()
	if lRoomMgr.CountItemLimitByName(stype1, name1) > 0 then
		return false,21
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local itemBindingType = g_RoomMgr.GetItemBindingTypeByID(goods_id)
	if itemBindingType == 2 or itemBindingType == 3 then
		-- ,"不能邮寄绑定物品 !"
		return  false, 4011
	end
	if g_ItemInfoMgr:IsSoulPearl(stype1) then
		local soul_count  = g_RoomMgr.CountSoulNumByPos(player_id,roomindex,pos)
		if soul_count > 1 then
			CancelTran()
			return false,4025
		end
	end
  local BoxitemDB = RequireDbBox("BoxitemDB")
  local ret= BoxitemDB.UpdateBoxitemBelonging(stype1, reciever_id, goods_id)
	if ret == false then
	   return false  
	end

	if not MailExecutor.AddMailItem(mail_id,goods_id,mail_grid) then
		return false
	end
	
	if not g_RoomMgr.MoveItemFromPackage(player_id,goods_id) then
		CancelTran()
		return false
	end
	
	return true
end

function MailExecutor.RealSendMail(data)
	local mail_money   = data["mail_money"]
	local send_money   = data["send_money"]
	local mail_title   = data["mail_title"]
	local mail_content = data["mail_content"]
	local mail_info    = data["mail_info"]
	local item_info    = data["item_info"]
	local reciever_id  = data["reciever_id"]
	local sender_id    = data["sender_id"]
	local nMoneyType = data["money_type"]
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Mail")
	if MailExecutor.MailIsFull(reciever_id) then
		return {false,4019}
	end
	
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local bSucc,uEventId
	local sysFeeFlag,uMsgID = MoneyManagerDB.SubtractMoneyBySysFee(mail_money, sender_id, fun_info, fun_info["SendMail"],event_type_tbl["邮件管理费"])
	if not sysFeeFlag then
		return {false,IsNumber(uMsgID) and uMsgID or 4015}
	end	
	
	if (send_money ~=0 ) then
		local real_send_money = MoneyManagerDB.SetNewCess(sender_id,send_money)
		if not real_send_money then 
			return {false}
		end
		bSucc,uEventId = MoneyManagerDB.AddMoneyByType(fun_info["FunName"],fun_info["SendMail"],sender_id,1, -send_money,nil,event_type_tbl["邮寄"],reciever_id,real_send_money)
		if not bSucc then
			return {false,IsNumber(uEventId) and uEventId or 4010}
		end
		if send_money - real_send_money > 0 then
       g_LogMgr.SaveTaxLog((send_money - real_send_money), sender_id, event_type_tbl["邮寄扣税"])
    end
		send_money = real_send_money  --税后实际发送的
	end
	
	local ex = RequireDbBox("Exchanger")
	local sender = ex.getPlayerNameById(sender_id)
	local ret = MailExecutor.SendMail(sender, reciever_id, mail_title, mail_content, send_money,item_info,sender_id,1,81,5001)
	if not ret[1] then
		CancelTran()
		return {false, ret[2], ret[3]}
	end
	
	local mail_id = ret[1]
	for i=2,#mail_info do
		local succ,uMsgID = MailExecutor.SendMailGoods(sender_id,mail_id,mail_info[i][1],mail_info[i][2], reciever_id)
		if succ==false then
			CancelTran()
			return {false,uMsgID}
		end
	end
	g_LogMgr.PlayerSendEmail( sender_id,reciever_id, item_info,uEventId)
	return {true, ["reciever_id"] = reciever_id}
end

function MailExecutor.DeleteBatchMail(data)
	local batchMailIDTbl = data["mail_id_tbl"]
	local char_id				 = data["char_id"]
	for i=1, # batchMailIDTbl do
		local mail_id = batchMailIDTbl[i]
		if MailExecutor._HasMailItem(mail_id,char_id) then
			CancelTran()
			return 4024
		end
		StmtContainer.DeleteMailStmt:ExecSql("",mail_id , char_id)
		if g_DbChannelMgr:LastAffectedRowNum() < 1 then
			CancelTran()
			return 4012
		end
	end
	return true
end


-------GM指令修改邮件剩余时间
local StmtDef = 
{
		"_UpdateMailLeftTime",
		[[
			update tbl_mail set m_tUpdateTime = date_add(now(),interval -(?-?) second)
			where ms_uID = ? and cs_uReciever = ?
		]]
}
DefineSql ( StmtDef, StmtContainer )
function MailExecutor.ChangeMailTimeGM(data)
	local uPlayerID				 = data.uPlayerID
	local nLeftTime				 = data.nLeftTime
	
	local result_set = StmtContainer.SelecteMailListStmt:ExecStat(uPlayerID)
	local row = result_set:GetRowNum()
	local info = {}
	for i = 1, row do
		local up_time = result_set:GetData(i-1,4)
		local state = result_set:GetData(i-1,3)
		local leftdate = 30*24*60*60
		if state == MailState.UnTakeAttachment then
			leftdate = 7*24*60*60
		elseif state == MailState.TakeAttachment then
			leftdate = 3*24*60*60
		end
		local mail_id = result_set:GetData(i-1,0)
		StmtContainer._UpdateMailLeftTime:ExecStat(leftdate,nLeftTime,mail_id,uPlayerID)
	end
end

SetDbLocalFuncType(MailExecutor.RecieveMail)
SetDbLocalFuncType(MailExecutor.GetMailList)
SetDbLocalFuncType(MailExecutor.SendMailContent)
SetDbLocalFuncType(MailExecutor.RealSendMail)
SetDbLocalFuncType(MailExecutor.GetMailItem)
SetDbLocalFuncType(MailExecutor.GetAllMailItem)
SetDbLocalFuncType(MailExecutor.CreateLetter)
SetDbLocalFuncType(MailExecutor.DeleteMail)
SetDbLocalFuncType(MailExecutor.GetMailMoney)
SetDbLocalFuncType(MailExecutor.DeleteBatchMail)
return MailExecutor
