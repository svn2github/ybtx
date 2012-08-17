gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "framework/common/CMoney"
local g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()
local event_type_tbl = event_type_tbl
local g_MoneyMgr = CMoney:new()

local StmtOperater = {}
local GMCompenate = CreateDbBox(...)

local StmtDef=
{
    "_AddDrawItem",
    [[
    	insert into tbl_gm_compenate 
    	set us_uId = ?,gc_uActivityId = ?,gc_uItemName = ?,gc_uBeginTime = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_CountDrawItem",
    [[
    	select count(*) from  tbl_gm_compenate 
    	where  us_uId = ? and gc_uActivityId = ? and gc_uItemName = ? and gc_uBeginTime = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelDrawItem",
    [[
    	delete from  tbl_gm_compenate 
    	where  us_uId = ? and gc_uActivityId = ? and gc_uItemName = ? and gc_uBeginTime = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)
function GMCompenate.CountDrawItem(user_id,activity_id,item_name,begin_time)
	local res = StmtOperater._CountDrawItem:ExecStat(user_id,activity_id,item_name,begin_time)
	return res(1,1)
end

function GMCompenate.DelCompenateItem(data)
	StmtOperater._DelDrawItem:ExecStat(data["user_id"],data["activity_id"],data["item_name"],data["begin_time"])
end

function GMCompenate.AddCompenateItem(data)
	local nItemType = data["item_type"]
	local sItemName = data["item_name"]
	local nCount 	= data["item_num"]
	local nCharID 	= data["char_id"]
	local user_id 	= data["user_id"]
	local activity_id 	= data["activity_id"]
	local begin_time = data["begin_time"]
	local SenderName = 7006
	local MailTitle = 7007
	local MailContent = 7007
	local nSendMoney = 0
	local item_ids = {}
	if GMCompenate.CountDrawItem(user_id,activity_id,sItemName,begin_time) > 0 then
		return false,"got"
	end
	if 0 == nItemType then
		if string.find(sItemName,"money") then
			local str_pos = string.find(sItemName,":")
			if str_pos then
				nSendMoney = tonumber(string.sub(sItemName,str_pos+1,string.len(sItemName)))*100
			end
		end
	else
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(nItemType,sItemName)
		local BuyItemData = {}
		BuyItemData.m_nType = nItemType
		BuyItemData.m_sName = sItemName
		BuyItemData.m_nCharID = nCharID
		BuyItemData.m_sCreateType = event_type_tbl["gm补偿"]
		BuyItemData.m_nBindingType = data["binding_type"]
		for i=1,nCount do
			local nItemID,uMsgID	= g_RoomMgr.CreateItem(BuyItemData)
			if not nItemID then
			 	CancelTran()
			 	local MsgID = uMsgID or 57
			 	return false,MsgID
			end
			table.insert(item_ids,{nItemID})
		end
	end
	--发送到邮箱
	local SysMailDB = RequireDbBox("SysMailDB")
	local succ = SysMailDB.SendSysMail(SenderName,nCharID,MailTitle,MailContent,item_ids,nSendMoney,event_type_tbl["gm补偿"],5001)
	if not succ  then
		CancelTran()
		return false,64
	end
	StmtOperater._AddDrawItem:ExecStat(user_id,activity_id,sItemName,begin_time)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		CancelTran()
		return false,65
	end
	return true
end

-------------------------------------激活码相同的账号奖励魂和经验的功能---------------------
local StmtDef = {
	"_GetUserEverydaySoul",
	[[ select esae_uSoul, esae_uExp from tbl_user_everyday_soul_and_exp where us_uId = ?]] 
}
DefineSql(StmtDef, StmtOperater)
local StmtDef = {
	"_AddUserEverydaySoul",
	[[ 
	replace into tbl_user_everyday_soul_and_exp 
	set esae_uSoul = ?,esae_uExp = ?,us_uId = ?
	]] 
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetUserActivationCode",
	[[ 
		select uac_ActCode from tbl_user_activation_code where us_uId = ?
	]] 
}
DefineSql(StmtDef, StmtOperater)
function GMCompenate.AddUserEverydaySoulAndExp(char_id,nAddNum,type)
	if nAddNum <= 0 then return true end
	local ex = RequireDbBox("Exchanger")
	local user_id = ex.GetUserIDByCharId(char_id)
	local tbl_activation_code = StmtOperater._GetUserActivationCode:ExecStat(user_id)
	if tbl_activation_code:GetRowNum() > 0 then
		local tbl_res = StmtOperater._GetUserEverydaySoul:ExecStat(user_id)
		local soul,exp = 0,0
		if tbl_res:GetRowNum() > 0 then
			soul,exp = tbl_res(1,1),tbl_res(1,2)
		end
		if type == 1 then
			exp = exp + nAddNum
		else
			soul = soul + nAddNum
		end
		StmtOperater._AddUserEverydaySoul:ExecStat(soul,exp,user_id)
		if g_DbChannelMgr:LastAffectedRowNum() == -1 then
			error("修改魂值失败，最后影响行数是-1")
		end
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CancelTran()
			return 
		end
	end
	return true
end

local StmtDef = {
	"_GetAllUserByActCode",
	[[ 
		select uac.us_uId from tbl_user_activation_code as uac,tbl_user_last_login_logout_time as ulllt
		where uac_ActCode = ? and ulllt.us_uId = uac.us_uId and unix_timestamp(now()) - unix_timestamp(ulllt_dtLoginTime) <= 604800
	]] 
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_AddUserCompenate",
	[[ 
		insert into tbl_user_activation_compenate 
		set us_uId = ?,is_uType = ?,is_sName = ?,uac_ItemCount = ?
	]] 
}
DefineSql(StmtDef, StmtOperater)


local StmtDef = {
	"_GetAllActivationCode",
	[[ 
		select distinct(uac_ActCode) from tbl_user_activation_code as uac,tbl_user_everyday_soul_and_exp as esae 
		where uac.us_uId = esae.us_uId
	]] 
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetExpAndSoulByActCode",
	[[ 
		select ifnull(sum(esae_uSoul),0),ifnull(sum(esae_uExp),0) from tbl_user_activation_code as uac,tbl_user_everyday_soul_and_exp as esae 
		where uac.us_uId = esae.us_uId and uac.us_uId <> ? and uac_ActCode = ? 
	]] 
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_GetUserMaxLevel",
	[[ 
		select max(cb_uLevel) from tbl_char_basic as cb,tbl_char_static as cs
		where cs.cs_uId = cb.cs_uId and us_uId = ?
	]] 
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = {
	"_DelExpAndSoulByActCode",
	[[ 
		delete esae from tbl_user_activation_code as uac,tbl_user_everyday_soul_and_exp as esae 
		where uac.us_uId = esae.us_uId and uac_ActCode = ?
	]] 
}
DefineSql(StmtDef, StmtOperater)

function GMCompenate.AssignUserSoulAndExp()
	local all_act_code = StmtOperater._GetAllActivationCode:ExecStat()
	local row = all_act_code:GetRowNum()
	local prop = 0.05
	for i =1,row do
		local ActCode = all_act_code(i,1)
		local tbl_user = StmtOperater._GetAllUserByActCode:ExecStat(ActCode)
		local point_name = g_DbChannelMgr:SetSavepoint()
		for j = 1,tbl_user:GetRowNum() do
			local user_id = tbl_user(j,1)
			local tbl_exp_soul = StmtOperater._GetExpAndSoulByActCode:ExecStat(user_id,ActCode)
			local soul,exp =  math.floor(tbl_exp_soul(1,1)*prop),math.floor(tbl_exp_soul(1,2)*prop)
			local max_level = StmtOperater._GetUserMaxLevel:ExecStat(user_id)
			if max_level(1,1) then
				local max_num =  math.floor((max_level(1,1)*3+50)*2250)      --(level*3+50)/10*60*5*150/2
				if soul > 0 then
					if soul > max_num then
						soul = max_num
					end
					local soul_name = "通用魂珠:" .. soul
					StmtOperater._AddUserCompenate:ExecStat(user_id,19,soul_name,1)
					if g_DbChannelMgr:LastAffectedRowNum() == 0 then
						g_DbChannelMgr:RollbackTransTo(point_name)
					end
				end
				if exp > 0 then
					if exp > max_num then
						exp = max_num
					end
					local exp_name = "经验球:" .. exp
					StmtOperater._AddUserCompenate:ExecStat(user_id,19,exp_name,1)
					if g_DbChannelMgr:LastAffectedRowNum() == 0 then
						g_DbChannelMgr:RollbackTransTo(point_name)
					end
				end
			end
		end
		StmtOperater._DelExpAndSoulByActCode:ExecStat(ActCode)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			g_DbChannelMgr:RollbackTransTo(point_name)
		end
	end
end

local StmtDef = {
	"_GetUserCompenate",
	[[ 
		select uac_uId,is_uType,is_sName,uac_ItemCount from tbl_user_activation_compenate 
		where us_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)
function GMCompenate.GetUserComenpate(data)
	return StmtOperater._GetUserCompenate:ExecStat(data["user_id"])
end

local StmtDef = {
	"_GetItemInfoByCompensateID",
	[[ 
		select is_uType,is_sName,uac_ItemCount from tbl_user_activation_compenate 
		where us_uId = ? and uac_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)
local StmtDef = {
	"_DelItemInfoByCompensateID",
	[[ 
		delete from tbl_user_activation_compenate 
		where uac_uId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)
function GMCompenate.TakeActivationCodeCompensate(data)
	local compensate_id = data["compensate_id"]
	local nCharID = data["role_id"]
	local item_info = StmtOperater._GetItemInfoByCompensateID:ExecStat(data["user_id"],compensate_id)
	if item_info:GetRowNum() == 0 then return end
	local nItemType,sItemName,nCount = item_info(1,1),item_info(1,2),item_info(1,3)
	local SenderName = 7006
	local MailTitle = 7007
	local MailContent = 7007
	local nSendMoney = 0
	local item_ids = {}
	if 0 == nItemType then
		if string.find(sItemName,"money") then
			local str_pos = string.find(sItemName,":")
			if str_pos then
				nSendMoney = tonumber(string.sub(sItemName,str_pos+1,string.len(sItemName)))
			end
		end
	else
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(nItemType,sItemName)
		local BuyItemData = {}
		BuyItemData.m_nType = nItemType
		BuyItemData.m_sName = sItemName
		BuyItemData.m_nCharID = nCharID
		BuyItemData.m_sCreateType = event_type_tbl["相同激活码账号经验和魂分配"]
		BuyItemData.m_nBindingType = 2
		for i=1,nCount do
			local nItemID	= g_RoomMgr.CreateItem(BuyItemData)
			if not nItemID then
			 	CancelTran()
			 	return 
			end
			table.insert(item_ids,{nItemID})
		end
	end
	local SysMailDB = RequireDbBox("SysMailDB")
	local succ = SysMailDB.SendSysMail(SenderName,nCharID,MailTitle,MailContent,item_ids,nSendMoney,event_type_tbl["相同激活码账号经验和魂分配"],5001)
	if not succ  then
		CancelTran()
		return
	end
	StmtOperater._DelItemInfoByCompensateID:ExecStat(compensate_id)
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		CancelTran()
		return 
	end
	
	return true,StmtOperater._GetUserCompenate:ExecStat(data["user_id"])
end

SetDbLocalFuncType(GMCompenate.AddCompenateItem)
SetDbLocalFuncType(GMCompenate.DelCompenateItem)
SetDbLocalFuncType(GMCompenate.TakeActivationCodeCompensate)
return GMCompenate

