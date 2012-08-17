
CExchangeMoney = class()
--【查询可领取余额】
function CExchangeMoney.GetTotalTicket(Conn)
	if GasConfig.GasType == 1 then
		Gas2Gac:OpenMessageBox(Conn, 1400)
		return
	end
	
		local Player = Conn.m_Player
		if not (Player and IsCppBound(Player)) then return end
		local nCharID = Player.m_uID
		if GasConfig.SkipGBSAuth==1 then
   		Gas2GacById:RetGetTotalTicket(nCharID,0)
			return
		end
		
		local function ERating_GetUserBalance_Succ(nTotalBalance)
			Gas2GacById:RetGetTotalTicket(nCharID,nTotalBalance)
		end
		
		local function ERating_GetUserBalance_Failed(ReturnCode)
			if "timeout" == ReturnCode then
				MsgToConnById(nCharID,412)
				return
			end
			MsgToConnById(nCharID,411,ReturnCode)
		end
		
		local userid = Conn.m_Account:GetID()
		local subjectid = 3 --货币类型：未领取余额
		local data = {userid,Conn.m_Player.m_uID,tonumber(ERatingConfig["AGIP_Gateway_id"])}
		g_EProtocol:GetUserBalanceInfo(data,{ERating_GetUserBalance_Succ,ERating_GetUserBalance_Failed})
end

--【点击兑换按钮】
function CExchangeMoney.ChangeTicketToMoney(Conn,nTicket)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160048)
		return 
	end
	local nCharID = Player.m_uID
	if nTicket <= 0 then return end
	
	local function CallBack(result)
			Gas2GacById:RetDelTicket(nCharID,0)
			if not result then return end
			if IsNumber(result) then
					MsgToConn(Conn,result)
					return
			end
			MsgToConn(Conn,51)
	end
	
	local data = {}
	data.char_id = nCharID
	data.amount = nTicket
	data.DrawType = 1
	CallAccountManualTrans(Conn.m_Account, "AgiotageDB", "AddUserDrawBalanceInfo", CallBack, data)	
end

--【向erating发送兑换信息的tick】
--兑换成功，给玩家加钱
local function ERating_DrawBalance_Succ(ReturnCode,detail_id,char_id)	
		local data = {}
		data.detail_id = detail_id
		local function CallBack(flag,add_money)
			if not flag then
				Gas2GacById:RetDelTicket(char_id,0)
				return
			end
			if IsNumber(flag) then
				MsgToConnById(char_id,flag)
				Gas2GacById:RetDelTicket(char_id,0)
				return
			end
			if add_money > 0 then
				Gas2GacById:RetDelTicket(char_id,add_money*0.01)
				MsgToConnById(char_id,49)
			end
		end
		data.type = 1
		data.ReturnCode = ReturnCode
		CallDbTrans("AgiotageDB", "DelUserDrawBalanceInfo", CallBack, data,char_id)		
end

--兑换失败
local function ERating_DrawBalance_Failed(ReturnCode,detail_id,roleid)
			if error_msg then
				MsgToConnById(roleid,415,ReturnCode)
			end
			Gas2GacById:RetDelTicket(roleid,0)
		local data = {}
		data.detail_id = detail_id
		data.type = 2
		data.ReturnCode = ReturnCode
		CallDbTrans("AgiotageDB", "DelUserDrawBalanceInfo", nil, data,roleid)		
end

function CExchangeMoney.SendUserDrawBalanceInfo()
	if g_CurServerId ~= 1 then
		return
	end
	local function CallBack(res)
		local subjectid = 3 --货币类型：未领取余额
		local rating_id = 0
		for i=1,res:GetRowNum() do
			local detail_id,user_id,char_id = res(i,1),res(i,2),res(i,3)
			local amount = res(i,4)
			local data = {detail_id,user_id,char_id,rating_id,subjectid,amount}
			g_EProtocol:DrawBalance(data,{ERating_DrawBalance_Succ,ERating_DrawBalance_Failed})
		end
	end
	local data = {}
  CallDbTrans("AgiotageDB", "GetUserDrawBalanceInfo", CallBack, data)	
end

function CExchangeMoney.ClearUserDrawBalanceMemoryInfo()
	if g_CurServerId ~= 1 then
		return
	end
	local data = {}
  CallDbTrans("AgiotageDB", "ClearDrawBalanceMemory", nil, data)	
end

--【向erating发送金钱消费/流通信息】
--发送成功，修改状态
function CExchangeMoney.ERating_SendIBPayMsg_Succ(tblDetailID)
	if #tblDetailID > 0 then
		CallDbTrans("AgiotageDB", "UpdateIBPayState", nil,tblDetailID,"SendIBPaySucc")	
	end
end

function CExchangeMoney.ERating_UserCashExchange_Succ(tblDetailID)
	if #tblDetailID > 0 then
		CallDbTrans("AgiotageDB", "UpdateExchangeState", nil, tblDetailID,"UserCashExchange")	
	end
end

--【发送金币消费消息】
function CExchangeMoney.SendIBPayInfo()

		if g_EProtocol.m_nSendIBPayNum > 0 then return end
			
		local function CallBack(res)
			g_EProtocol.m_nSendIBPayNum = res:GetRowNum()
			for i =1,res:GetRowNum() do
				local detail_id,user_id,char_id,sex,class,level = res(i,1),res(i,2),res(i,3),res(i,4),res(i,5),res(i,6)
				local ib_code,buy_count,ip,subject_id= res(i,7),res(i,8),res(i,9),res(i,10)
				local price,buy_time,ib_code_type = res(i,11),res(i,12),res(i,13)
				local rating_id = tonumber(ERatingConfig["AGIP_Gateway_id"])
				local audit_flag = 2 --是否对单
				local package_flag = 1 --产品类型
				local ip_info = {}
				SplitString(ip, "%.", ip_info)
				local IB_Code = (ib_code_type == 1) and ib_code or GB2U8(ib_code)
				if not IB_Code or IB_Code == "" then
					LogErr("erating：IB_CODE错误","ib_code:" .. ib_code .. "detail_id:" ..  detail_id .. "Locale:[" .. os.setlocale() .. "]")
				end
				local nIP = tonumber(string.format("0x%02x%02x%02x%02x", tonumber(ip_info[1]), tonumber(ip_info[2]), tonumber(ip_info[3]), tonumber(ip_info[4])))
				local params = {user_id,char_id,sex,class,level,rating_id,
											IB_Code,package_flag,buy_count,nIP,subject_id,
											audit_flag,price,price,buy_time,detail_id}		
				g_EProtocol:UserIBPay(params)
			end
		end
		local data = {}
		data.EratingIBPaySendNum = GasConfig.EratingIBPaySendNum
  	CallDbTrans("AgiotageDB", "GetAllIBPayData", CallBack, data)	
end

--【发送金币流通消息】
function CExchangeMoney.SendMoneyExchangeInfo()

	if g_EProtocol.m_nSendMoneyExchangeNum > 0 then return end
	
	local function CallBack(exchange_res)
		g_EProtocol.m_nSendMoneyExchangeNum = exchange_res:GetRowNum()
		for j =1,exchange_res:GetRowNum() do
				local detail_id,src_user_id,src_char_id = exchange_res(j,1),exchange_res(j,2),exchange_res(j,3)
				local dst_user_id,dst_char_id,subject_id = exchange_res(j,4),exchange_res(j,5),exchange_res(j,6)
				local amount,time =  exchange_res(j,7),exchange_res(j,8)
				local params = {detail_id,src_user_id,src_char_id,
											dst_user_id,dst_char_id,subject_id,
											1,amount,time}
				g_EProtocol:UserCashExchange(params)
		end
	end
	local data = {}
	data.EratingExchangeSendNum = GasConfig.EratingExchangeSendNum
  CallDbTrans("AgiotageDB", "GetAllMoneyExchangeInfo", CallBack,data)	
end

--------------------------------对账相关--------------
local function ERating_SendAuditInfo_Succ(audit_id)
	local data = {}
	data.audit_id = audit_id
	CallDbTrans("AgiotageDB", "DelAuditInfo", nil, data,"EAudit" .. audit_id)	
end

function CExchangeMoney.SendEratingAuditTick()
	local function SendEratingAuditTick()
		local function CallBack(tblAuditInfo)
			if 0 == tblAuditInfo:GetRowNum() then
				UnRegisterTick(g_SendEratingAuditTick)
				g_SendEratingAuditTick = nil
			end
			for i =1,tblAuditInfo:GetRowNum() do
				local audit_id,subject_id				= tblAuditInfo(i,1),tblAuditInfo(i,2)
				local output,consume,left_money = tblAuditInfo(i,3),tblAuditInfo(i,4),tblAuditInfo(i,5)
				local audit_time = tblAuditInfo(i,6)
				local game_id = tblAuditInfo(i,7)
				local params = {audit_id,subject_id,output,consume,left_money,audit_time,game_id}		
				g_EProtocol:GWBalanceReport(params,{ERating_SendAuditInfo_Succ})
			end
		end
  	CallDbTrans("AgiotageDB", "GetAuditInfo", CallBack, {})	
	end
	if g_CurServerId ~= 1 then
		return
	end	
	if not g_SendEratingAuditTick then
		g_SendEratingAuditTick = RegisterTick("g_SendEratingAuditTick", SendEratingAuditTick,15*60*1000)
	end
end

function CExchangeMoney.EratingAuditTick()
	if g_CurServerId ~= 1 then
		return
	end	
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans("AgiotageDB","AddAuditInfo", nil,{})
		end
	end
	g_AlarmClock:AddTask("EratingAudit", {wday = {1,2,3,4,5,6,7},time = {"00:00","06:00","12:00","18:00"}}, CallBack)
end

function CExchangeMoney.ClearWMXInfo()
	if g_CurServerId ~= 1 then
		return
	end	
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans("MoneyMgrDB","ClearWMXInfo", nil,{})
		end
	end
	g_AlarmClock:AddTask("ClearWMXInfo", {wday = {1},time = {"05:00"}}, CallBack)
end








