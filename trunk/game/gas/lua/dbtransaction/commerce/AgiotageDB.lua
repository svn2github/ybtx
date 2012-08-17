
local StmtOperater = {}	
local event_type_tbl = event_type_tbl
local AgiotageDB	=	CreateDbBox(...)


local StmtDef=
{
    "_GetDetailID",
    [[
    	insert into tbl_erating_detail_id values()
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetAllIBPayInfo",
    [[
    	select 	a.tid_uDetailID,b.us_uId,a.cs_uId,b.cs_uSex,
    					b.cs_uClass,a.cs_uLevel,a.tid_IBCode,
    					a.tid_uBuyCount,a.tid_sIP,a.tid_uSubjectID,
    					a.tid_uPrice,
    					unix_timestamp(a.tid_dtBuyTime),
    					a.tid_uIBCodeType 
    	from 
    					tbl_char_static as b,tbl_IBPay_detail as a
    	left join 	tbl_IBPay_detail_memory as idbm
    	on (a.tid_uDetailID = idbm.tid_uDetailID)
    	where 
    				a.cs_uId = b.cs_uId 
    				and idbm.tid_uDetailID is null 
    				limit ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

--添加消费记录、定时发送到erating
local StmtDef=
{
    "_AddIBPayDetailInfo",
    [[
    	insert into tbl_IBPay_detail 
    	set tid_uDetailID = ?,cs_uId = ?,cs_uLevel = ?,tid_IBCode = ?,
    	tid_uBuyCount = ?,tid_dtBuyTime = now(),tid_sIP = ?,tid_uSubjectID = ?,
    	tid_uPrice = ?,tid_uIBCodeType = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_PayListQuery",
    [[
    	select tid_uDetailID from tbl_IBPay_detail
    	where tid_uSubjectID = ? and tid_uDetailID >= ? 
    	order by  tid_uDetailID ASC limit ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetPayMoneyInfo",
    [[
    	select sum(tid_uPrice) from tbl_IBPay_detail 
    	where 	tid_uSubjectID = ? and tid_uDetailID >= ?
    	order by  tid_uDetailID ASC limit ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = {
	"_AddMoneyExchangeInfo",
	[[
		insert into tbl_money_exchange set tid_uDetailID = ?,cs_uId = ?,cs_uTargetID = ?,tme_uSubjectID = ?,
		tme_uAmount = ?,tme_dtTime = now()
	]]
}
DefineSql(StmtDef, StmtOperater)
local StmtDef=
{
    "_GetExchangeInfo",
    [[
    	select 
    				tme.tid_uDetailID,cs1.us_uId,tme.cs_uId,cs2.us_uId,tme.cs_uTargetID,tme.tme_uSubjectID,tme.tme_uAmount,unix_timestamp(tme.tme_dtTime)
    	from 
    				tbl_char_static as cs1,tbl_char_static as cs2,tbl_money_exchange as tme
    	
    	left join 	tbl_IBPay_detail_memory as idbm
    	on 		(tme.tid_uDetailID = idbm.tid_uDetailID)
    	
    	where cs1.cs_uId = tme.cs_uId 
    		and cs2.cs_uId = tme.cs_uTargetID 
    		and idbm.tid_uDetailID is null
    	limit ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddIBPayLog",
    [[
    	insert ignore into tbl_IBPay_log(tid_uDetailID,tid_uSubjectID,cs_uId,cs_uLevel,tid_sIP,tid_IBCode,tid_uBuyCount,tid_uPrice,tid_dtBuyTime,tid_uIBCodeType,til_uRetCode)
			select tid_uDetailID,tid_uSubjectID,cs_uId,cs_uLevel,tid_sIP,tid_IBCode,tid_uBuyCount,tid_uPrice,tid_dtBuyTime,tid_uIBCodeType,?
			from tbl_IBPay_detail where tid_uDetailID = ? 
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelIbpayinfoByID",
    [[
    	delete from tbl_IBPay_detail where tid_uDetailID = ? 
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddExchangeLog",
    [[
    	insert into tbl_money_exchange_log(tid_uDetailID,tme_uSubjectID,cs_uId,cs_uTargetID,tme_uAmount,tme_dtTime)
			select tid_uDetailID,tme_uSubjectID,cs_uId,cs_uTargetID,tme_uAmount,tme_dtTime 
			from tbl_money_exchange 
			where 	tid_uDetailID = ? 
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelExchangeInfoByID",
    [[
    	delete from tbl_money_exchange 
			where 	tid_uDetailID = ? 
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddUserDrawBalanceInfo",
    [[
    	insert into tbl_user_draw_balance set tid_uDetailID = ?,cs_uId = ?,udb_uAmount = ?,udb_uType = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef=
{
    "_GetUserDrawBalanceInfo",
    [[
    	select 	udb.tid_uDetailID,cs.us_uId,udb.cs_uId,udb.udb_uAmount
    	from 		tbl_char_static as cs,tbl_user_draw_balance as udb
    	left join 	tbl_user_draw_balance_memory as udbm
    	on (udb.tid_uDetailID = udbm.tid_uDetailID)
    	where 	udb.cs_uId = cs.cs_uId
    	and udbm.tid_uDetailID is null
    	limit 20
    ]] 
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetDrawBalanceInfoByID",
    [[
    	select 	cs_uId,udb_uAmount,udb_uType
    	from 		tbl_user_draw_balance
    	where 	tid_uDetailID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelDrawBalanceInfoByID",
    [[
    	delete from 	tbl_user_draw_balance
    	where 	tid_uDetailID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelDrawBalanceMemoryInfoByID",
    [[
    	delete from 	tbl_user_draw_balance_memory
    	where 	tid_uDetailID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetAGIPGatewayID",
    [[
    	select 	sc_sVarValue
    	from 		tbl_conf_server
    	where 	sc_sVarName = "AGIP_Gateway_id"
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelDrawBalanceMemoryInfo",
    [[
    	delete from tbl_user_draw_balance_memory 
    	where unix_timestamp(now()) - unix_timestamp(udbm_dtSendTime) >= 600
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddDrawBalanceMemoryInfo",
    [[
    	replace into tbl_user_draw_balance_memory 
    	set tid_uDetailID = ?,udbm_dtSendTime = now()
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddIBPayMemoryInfo",
    [[
    	replace into tbl_IBPay_detail_memory 
    	set tid_uDetailID = ?,idm_dtSendTime = now()
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelIBPayMemoryInfoByID",
    [[
    	delete from tbl_IBPay_detail_memory 
    	where tid_uDetailID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelIBPayMemoryInfoByTime",
    [[
    	delete from tbl_IBPay_detail_memory 
    	where unix_timestamp(now()) - unix_timestamp(idm_dtSendTime) >= 600
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelDetailId",
    [[
    	delete from tbl_erating_detail_id where tid_uDetailID < ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetMaxDetailId",
    [[
    	select max(tid_uDetailID) from tbl_erating_detail_id 
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddDrawBalanceLogInfo",
    [[
    	insert into tbl_user_draw_balance_log 
    	set tid_uDetailID = ?,cs_uId = ?,udb_uAmount = ?,
    	udbl_RetCode = ?,udbl_dtTime = now(),udb_uType = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddWebshopDrawDetail",
    [[
    	replace into tbl_webshop_draw_detail 
    	set cs_uId = ?,wdd_uRetCode = ?,wdd_dtTime = now()
    ]]
}
DefineSql(StmtDef,StmtOperater)
--消费信息发送成功，修改状态
function AgiotageDB.UpdateIBPayState(tblDetailID)
	for i =1,#tblDetailID do
		local point_name = g_DbChannelMgr:SetSavepoint()
		local detail_id = tblDetailID[i][1]
		local ret_code = tblDetailID[i][2]
		StmtOperater._AddIBPayLog:ExecStat(ret_code,detail_id)
		local row_num = g_DbChannelMgr:LastAffectedRowNum()
		StmtOperater._DelIbpayinfoByID:ExecStat(detail_id)
		if row_num > 0 and g_DbChannelMgr:LastAffectedRowNum() == 0 then
			g_DbChannelMgr:RollbackTransTo(point_name)
		end
		StmtOperater._DelIBPayMemoryInfoByID:ExecStat(detail_id)
	end
end

--金钱流通向erating发送成功后修改信息状态
function AgiotageDB.UpdateExchangeState(tblDetailID)
	for i =1,#tblDetailID do
		local point_name = g_DbChannelMgr:SetSavepoint()
		local detail_id = tblDetailID[i]
		StmtOperater._AddExchangeLog:ExecStat(detail_id)
		local row_num = g_DbChannelMgr:LastAffectedRowNum()
		StmtOperater._DelExchangeInfoByID:ExecStat(detail_id)
		if row_num > 0 and g_DbChannelMgr:LastAffectedRowNum() == 0 then
			g_DbChannelMgr:RollbackTransTo(point_name)
		end
		StmtOperater._DelIBPayMemoryInfoByID:ExecStat(detail_id)
	end
end

--获得金钱消费和流通信息
function AgiotageDB.GetAllIBPayData(data)
	local ibpay_result = StmtOperater._GetAllIBPayInfo:ExecStat(data.EratingIBPaySendNum)
	for i =1,ibpay_result:GetRowNum() do
		local detail_id = ibpay_result(i,1)
		StmtOperater._AddIBPayMemoryInfo:ExecStat(detail_id)
	end
	return ibpay_result
end

function AgiotageDB.GetAllMoneyExchangeInfo(data)
	local exchange_res = StmtOperater._GetExchangeInfo:ExecStat(data.EratingExchangeSendNum)
	for i =1,exchange_res:GetRowNum() do
		local detail_id = exchange_res(i,1)
		StmtOperater._AddIBPayMemoryInfo:ExecStat(detail_id)
	end
	return exchange_res
end

--5分钟清理一次memory表时间大于10分钟的数据
function AgiotageDB.ClearDrawBalanceMemory()
	StmtOperater._DelDrawBalanceMemoryInfo:ExecStat()
	StmtOperater._DelIBPayMemoryInfoByTime:ExecStat()
	local tbl_max_detail = StmtOperater._GetMaxDetailId:ExecStat()
	if tbl_max_detail:GetRowNum() > 0 then 
		StmtOperater._DelDetailId:ExecStat(tbl_max_detail(1,1))
	end
end

--查询兑换信息发送到erating兑换
function AgiotageDB.GetUserDrawBalanceInfo(ParamsInfo)
	local res = StmtOperater._GetUserDrawBalanceInfo:ExecStat()
	for i =1,res:GetRowNum() do
		local detail_id = res(i,1)
		StmtOperater._AddDrawBalanceMemoryInfo:ExecStat(detail_id)
	end
	return res
end

function AgiotageDB.ToEratingDetailID(detail_id)
	local AGIP_Gateway_id = StmtOperater._GetAGIPGatewayID:ExecStat()
	local sGateWay_id = tostring(AGIP_Gateway_id(1,1))
	local len = string.len(sGateWay_id)
	local prefix = string.sub(sGateWay_id,len-2,len)
	local pad = "00"
	local detail_len = string.len(detail_id)
	local str = ""
	for i =1,(10 - detail_len) do 
		str = str .. "0"
	end
	local sDetailID = prefix .. pad .. str .. detail_id
	return sDetailID
end

function AgiotageDB.GetDetailID()
	StmtOperater._GetDetailID:ExecStat()
	local detail_id = g_DbChannelMgr:LastInsertId()
	detail_id = ToEratingDetailID(detail_id)
	return detail_id
end

--erating兑换消息返回成功,给玩家加钱
function AgiotageDB.DelUserDrawBalanceInfo(ParamsInfo)
	local type = ParamsInfo.type
	local res = StmtOperater._GetDrawBalanceInfoByID:ExecStat(ParamsInfo.detail_id)
	local char_id,add_money = 0,0
	if type == 2 then
		StmtOperater._DelDrawBalanceInfoByID:ExecStat(ParamsInfo.detail_id)
		if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
			CancelTran()
			return 
		end
	elseif type == 1 and res:GetRowNum() > 0 then
		char_id,add_money = res(1,1),res(1,2)*100
		StmtOperater._DelDrawBalanceInfoByID:ExecStat(ParamsInfo.detail_id)
		if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
			CancelTran()
			return 50
		end
		local MoneyMgrDB=	RequireDbBox("MoneyMgrDB")
		local sFunction = "Agiotage"
		local sModule = "ExchangeMoney"
		local event_type = event_type_tbl["佣兵币兑换流通钱"]
		local bFlag,uMsgID = MoneyMgrDB.AddMoney(sFunction,sModule,char_id,add_money,nil,event_type)
		if not bFlag then 
			CancelTran()
			return uMsgID
		end
		if not MoneyMgrDB.AddRmbMoney(char_id,add_money) then
			return 
		end
		if not AgiotageDB.AddEverydayOutput(char_id,5,add_money) then return end
		StmtOperater._AddDrawBalanceLogInfo:ExecStat(ParamsInfo.detail_id,char_id,res(1,2),ParamsInfo.ReturnCode,res(1,3))
		if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
			CancelTran()
			return
		end
	end
	StmtOperater._DelDrawBalanceMemoryInfoByID:ExecStat(ParamsInfo.detail_id)
	if res(1,3) == 2 then
		StmtOperater._AddWebshopDrawDetail:ExecStat(char_id,ParamsInfo.ReturnCode)
		if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
			CancelTran()
			return
		end
	end
	return true,add_money
end

--添加金钱兑换信息
function AgiotageDB.AddUserDrawBalanceInfo(ParamsInfo)
	local MoneyMgrDB=	RequireDbBox("MoneyMgrDB")
	if MoneyMgrDB.BeOverUpperLimit(ParamsInfo.char_id,(ParamsInfo.amount)*100) then
		return 309
	end 
	local detail_id = AgiotageDB.GetDetailID()
	StmtOperater._AddUserDrawBalanceInfo:ExecStat(detail_id,ParamsInfo.char_id,ParamsInfo.amount,ParamsInfo.DrawType)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return 50
	end
	return true
end

--添加金钱流通信息
function AgiotageDB.AddMoneyExchangeInfo(char_id,target_id,subject_id,amount)
	local detail_id = AgiotageDB.GetDetailID()
	StmtOperater._AddMoneyExchangeInfo:ExecStat(detail_id,char_id,target_id,subject_id,amount)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return 
	end
	return true
end

--添加金钱消费信息
function AgiotageDB.AddIBPayInfo(ParamsInfo)
	local detail_id = ParamsInfo.detail_id or AgiotageDB.GetDetailID()
	StmtOperater._AddIBPayDetailInfo:ExecStat(detail_id,ParamsInfo.char_id,ParamsInfo.char_level,ParamsInfo.IB_Code,ParamsInfo.buy_count,
										ParamsInfo.char_ip,ParamsInfo.subject_id,ParamsInfo.uPrice,ParamsInfo.IBCodeType)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return 
	end
	if not AgiotageDB.AddEverydayOutput(ParamsInfo.char_id,ParamsInfo.subject_id,-ParamsInfo.uPrice*ParamsInfo.buy_count) then
		return 
	end
	return detail_id
end


----------------------------------对账部分------------------------------
local StmtDef=
{
    "_GetTotalLeftMoney",
    [[
    	select sum(cm_uMoney),sum(cm_uRmbMoney),us_uGameID
    	 from tbl_char_money as cm,tbl_char_static as cs,tbl_user_static as us
    	where cm.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId
    	group by us_uGameID
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddAuditInfo",
    [[
    	insert into tbl_erating_audit 
    	set us_uGameID = ?,ea_uSubjectID = ?,ea_uTotalOutput = ?,ea_uTotalConsume = ?,ea_uLeftMoney = ?,ea_dtAuditTime = now()
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetAuditInfo",
    [[
    	select ea_ID,ea_uSubjectID,ea_uTotalOutput,ea_uTotalConsume,ea_uLeftMoney,unix_timestamp(ea_dtAuditTime),us_uGameID
    	from tbl_erating_audit 
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddAuditLog",
    [[
    	insert into tbl_erating_audit_log 
    	select * from tbl_erating_audit where ea_ID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelAuditLog",
    [[
    	delete from tbl_erating_audit where ea_ID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddEverydayOutput",
    [[
    	insert ignore into tbl_money_everyday_output
    	set us_uGameID = ?,tme_uSubjectID = ?,meo_uOutput = ?,meo_uConsume = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetEverydayOutputByType",
    [[
    	select count(1) from tbl_money_everyday_output
    	where us_uGameID = ? and tme_uSubjectID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelEverydayOutputInfo",
    [[
    	delete from tbl_money_everyday_output
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_CountEverydayOutputInfo",
    [[
    	select count(1) from tbl_money_everyday_output
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_UpEverydayOutputByType",
    [[
    	update tbl_money_everyday_output
    	set meo_uOutput = meo_uOutput + ?,meo_uConsume = meo_uConsume + ?
    	where us_uGameID = ? and tme_uSubjectID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetGameidByCharid",
    [[
    	select us_uGameID from tbl_user_static as us,tbl_char_static as cs
    	where cs.us_uId = us.us_uId and cs.cs_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetOutputInfoByType",
    [[
    	select us_uGameID,meo_uOutput,meo_uConsume from tbl_money_everyday_output
    	where tme_uSubjectID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

function AgiotageDB.AddEverydayOutput(char_id,subject_id,add_money)
	local tbl_gameid = StmtOperater._GetGameidByCharid:ExecStat(char_id)
	local output,consume = 0,0
	if add_money > 0 then
		output = add_money
	else
		consume = -add_money
	end
	local tbl_count = StmtOperater._GetEverydayOutputByType:ExecStat(tbl_gameid(1,1),subject_id)
	if tbl_count(1,1) > 0 then
		StmtOperater._UpEverydayOutputByType:ExecStat(output,consume,tbl_gameid(1,1),subject_id)
	else
		StmtOperater._AddEverydayOutput:ExecStat(tbl_gameid(1,1),subject_id,output,consume)
	end
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return
	end
	return true
end

function AgiotageDB.AddAuditInfo(data)
	local money_subject_id,ticket_subject_id = 6,5
	local output_money 		= StmtOperater._GetOutputInfoByType:ExecStat(money_subject_id)
	local left_money 			= StmtOperater._GetTotalLeftMoney:ExecStat()
	local draw_ticket			= StmtOperater._GetOutputInfoByType:ExecStat(ticket_subject_id)
	local tblInfo = {}
	for i=1,output_money:GetRowNum() do
		local game_id = output_money(i,1)
		tblInfo[game_id] 					= tblInfo[game_id] or {}
		tblInfo[game_id].OPMoney 	= output_money(i,2)
		tblInfo[game_id].CUMMoney = output_money(i,3)
	end
	for i=1,left_money:GetRowNum() do
		local game_id = left_money(i,3)
		tblInfo[game_id] 						= tblInfo[game_id] or {}
		tblInfo[game_id].LeftMoney 	= left_money(i,1) - left_money(i,2)
		tblInfo[game_id].LeftTicket = left_money(i,2)
	end
	for i=1,draw_ticket:GetRowNum() do
		local game_id = draw_ticket(i,1)
		tblInfo[game_id] = tblInfo[game_id] or {}
		tblInfo[game_id].DrawTicket = draw_ticket(i,2)
		tblInfo[game_id].CUMTicket = draw_ticket(i,3)
	end
	for i,v in pairs(tblInfo) do
		local game_id = i
		StmtOperater._AddAuditInfo:ExecStat(game_id,money_subject_id,v.OPMoney or 0,v.CUMMoney or 0,v.LeftMoney or 0)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("添加每日对账信息失败")
		end
		StmtOperater._AddAuditInfo:ExecStat(game_id,ticket_subject_id,v.DrawTicket or 0,v.CUMTicket or 0,v.LeftTicket or 0)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("添加每日对账信息失败")
		end
	end
	local tbl_output_count = StmtOperater._CountEverydayOutputInfo:ExecStat()
	if tbl_output_count(1,1) > 0 then
		StmtOperater._DelEverydayOutputInfo:ExecStat()
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("删除前日产出\消耗金钱数据失败")
		end
	end
	Db2CallBack("SendEratingAuditTick")
end

function AgiotageDB.GetAuditInfo(data)
	local audit_info	= StmtOperater._GetAuditInfo:ExecStat()
	return audit_info
end

function AgiotageDB.DelAuditInfo(data)
	StmtOperater._AddAuditLog:ExecStat(data.audit_id)
 	if  g_DbChannelMgr:LastAffectedRowNum() > 0 then
 		StmtOperater._DelAuditLog:ExecStat(data.audit_id)
	end
end

return AgiotageDB