gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
local MailDB = "MailDB"
MailMgr = class()
        
local ActionState = {
	Succ = true,
	Failure = false
}
-------------------------------------RPC 函数---------------------------------
--获取邮件
function MailMgr.GetMail(Conn, MailId)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local nCharID = Player.m_uID
	local function CallBack(result)
		if result == nil then
			return
		end
		local mailinfo = result[1]
		if not(mailinfo:GetRowNum() > 0) then 
		  return
		end
		local state = result[4]	
		Gas2Gac:RetGetMailInfo(Conn,mailinfo(1,1),mailinfo(1,2),mailinfo(1,3),mailinfo(1,4),mailinfo(1,5),state,mailinfo(1,7),mailinfo(1,10),mailinfo(1,11),mailinfo(1,12))

		for i=1, #result[2] do
				local ItemInfo = result[2][i][6]
				local nBigID,nIndex,nItemID,nBindType = result[2][i][3],result[2][i][4],result[2][i][1],result[2][i][5]
				Gas2Gac:RetGetMailGoods(Conn, MailId, result[2][i][1],result[2][i][2],result[2][i][3],result[2][i][4],1)
		end
		Gas2Gac:RetGetMailGoodsEnd(Conn,MailId)
	end
	local parameter = {}
	parameter.char_id= nCharID
	parameter.mail_id = MailId
	CallAccountManualTrans(Conn.m_Account, MailDB, "RecieveMail", CallBack, parameter)
end

--获取邮件列表
function MailMgr.GetMailList(Conn)
	local function CallBack(result)
		if not result then
			return
		end
		local bFull = result.m_bFull
		if bFull then
			MsgToConn(Conn,4020)
		end
		Gas2Gac:RetMailListBegin(Conn)
		for i = 1, #result do
			Gas2Gac:RetMailList(Conn, unpack(result[i]))
		end
		Gas2Gac:RetMailListEnd(Conn)
	end
	
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local parameter = {}
	parameter["char_id"] = Player.m_uID
	CallAccountManualTrans(Conn.m_Account,MailDB, "GetMailList", CallBack, parameter)
end

--服务器端保存邮件mail_id,mail_content, money部分到conn对象
--请求发送邮件文字部分
function MailMgr.SendMail(Conn, sRecieverName, sMailTitle, sMailContent, uMoney,nMoneyType)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	if sMailTitle =="" then 
		MsgToConn(Conn, 4002)
		Gas2Gac:RetSendEmail(Conn,false)
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		Gas2Gac:RetSendEmail(Conn,false)
		return 
	end
	--用“**”替换所有不合法字符
	local textFltMgr = CTextFilterMgr:new()
  sMailTitle = textFltMgr:ReplaceInvalidChar(sMailTitle)
  sMailContent = textFltMgr:ReplaceInvalidChar(sMailContent)
  
	if GetCharCount(sMailContent) > 500 then return end
	if GetCharCount(sMailTitle) > 16 then return end
		
	local function CallBack(result)
		if not result[1] then
			if result[3] ~= nil then
				MsgToConn(Conn, result[2], result[3])
			else
				MsgToConn(Conn, result[2])
			end
			Gas2Gac:RetSendEmail(Conn,false)
			return
		end
		local reciever_id = result["reciever_id"]
		local mail_info={reciever_id,sMailTitle,sMailContent, uMoney,nMoneyType}
		Conn.tbl_mail={}
		table.insert(Conn.tbl_mail,mail_info)
		Conn.ItemData = {}
		Gas2Gac:SendMailGoodsBegin(Conn)
	end
	local parameter = {["player_id"] = Player.m_uID, ["reciever_name"]=sRecieverName}
	local nRet = CallAccountManualTrans(Conn.m_Account, MailDB, "SendMailContent", CallBack, parameter)
	if nRet == 1 then
		Gas2Gac:RetSendEmail(Conn,false)
	end
end
--服务器端保存邮件物品部分到conn对象
--客户端发送邮件物品部分
function MailMgr.SendMailGoods(Conn,goods_id,mail_grid)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	if not Conn.tbl_mail then return end
	if not Conn.ItemData then return end
	local mail_item_info={goods_id,mail_grid}
	table.insert(Conn.tbl_mail,mail_item_info)	
	table.insert(Conn.ItemData, goods_id )			
end

--发送完毕
function MailMgr.SendMailGoodsEnd(Conn)
	local Player = Conn.m_Player
	if Player == nil then return end
	local nCharID = Player.m_uID
	local mail_info=Conn.tbl_mail
	if mail_info == nil then return end

	if GetCharCount(mail_info[1][2]) > 64 then
		--, "标题长度不能大于64字符！"
    	MsgToConn(Conn, 4005)
    	Conn.tbl_mail=nil
    	Gas2Gac:RetSendEmail(Conn,false)
    	return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		Gas2Gac:RetSendEmail(Conn,false)
		return 
	end
	
	if mail_info[1][4] > 100*1000*1000 then   
		--, "发送金钱不能大于100000000！"
		MsgToConn(Conn, 4006)
		Conn.tbl_mail=nil
		Gas2Gac:RetSendEmail(Conn,false)
		return
	elseif mail_info[1][4] < 0 then   
		--, "发送金钱不能小于0！"
		MsgToConn(Conn, 4007)
		Conn.tbl_mail=nil
		Gas2Gac:RetSendEmail(Conn,false)
		return
	end
	
	if(table.getn(mail_info)==1 and mail_info[1][3]=="" and mail_info[1][4]==0) then
		MsgToConn(Conn, 4003)
		Conn.tbl_mail=nil
		Gas2Gac:RetSendEmail(Conn,false)
		return
	end
	
	local mail_pay = 10+10*(table.getn(Conn.ItemData))
	local money_type = mail_info[1][5] or 1
	local send_money = mail_info[1][4] or 0
	
	local function CallBack(result)
	
		if not result[1] then
			if IsNumber(result[2]) then
				if result[3] then
					MsgToConn(Conn, result[2], result[3])
				else
					MsgToConn(Conn, result[2])
				end
			end
			Gas2Gac:RetSendEmail(Conn,false)
			Conn.tbl_mail = nil
			return
		end
		MsgToConn(Conn, 4013)
		Gas2Gac:RetSendEmail(Conn,true)
		Conn.tbl_mail = nil
	end
	
	local parameter = {}
	parameter["mail_money"] = mail_pay
	parameter["send_money"] = send_money
	parameter["mail_title"] = mail_info[1][2]
	parameter["mail_content"] = mail_info[1][3]
	parameter["mail_info"] = mail_info
	parameter["item_info"] = Conn.ItemData
	parameter["reciever_id"] = mail_info[1][1]
	parameter["sender_id"] = nCharID
	parameter["money_type"] = money_type
	local nRet = CallAccountManualTrans(Conn.m_Account, MailDB, "RealSendMail", CallBack, parameter) 
	if 1 == nRet then
		Gas2Gac:RetSendEmail(Conn,false)
	end
end	

--提取单个格子的物品
function MailMgr.TakeAttackmentByIndex(Conn, uMailId, SlotIndex)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local uCharId = Player.m_uID
	local function CallBack( result)
		if not result[1] then
			if IsNumber(result[2]) then
				if result[3] then
					MsgToConn(Conn,result[2],result[3])
				else
					MsgToConn(Conn,result[2])
				end
			end
			Gas2Gac:RetTakeAtachmentByIndex(Conn,SlotIndex,ActionState.Failure, uMailId)
			return
		end
		Gas2Gac:RetTakeAtachmentByIndex(Conn, SlotIndex ,ActionState.Succ, uMailId)
	end
	
	local parameter = {}
	parameter.mail_id = uMailId
	parameter.slot_index = SlotIndex
	parameter.char_id = Player.m_uID
	CallAccountManualTrans(Conn.m_Account, MailDB, "GetMailItem", CallBack, parameter)
end

function MailMgr.TakeAttachment(Conn, uMailId)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local uCharId = Player.m_uID
	local function CallBack(bFlag,uMsgID,MoneySucc)
		if MoneySucc then
			Gas2Gac:RetTakeAtachmentByIndex(Conn,0,ActionState.Succ,uMailId)
		else
			Gas2Gac:RetTakeAtachmentByIndex(Conn,0,ActionState.Failure,uMailId)
		end
		if not bFlag then
			if uMsgID then
					MsgToConn(Conn,uMsgID)
			end
			Gas2Gac:RetTakeAtachment(Conn,ActionState.Failure, uMailId)
		else
			Gas2Gac:RetTakeAtachment(Conn,ActionState.Succ,uMailId)
		end
	end
	local parameter = { ["mail_id"]=uMailId, ["char_id"]=uCharId }
	CallAccountManualTrans(Conn.m_Account, MailDB, "GetAllMailItem", CallBack, parameter)
end

--提取文本附件
function MailMgr.TakeContextAtachment(Conn,mail_id)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local uCharId = Player.m_uID
	local function CallBack(result,uMsgID,sMsg)
		if not result then
			if IsNumber(uMsgID) then
				if sMsg ~= nil then
					MsgToConn(Conn,uMsgID,sMsg)
				else
					MsgToConn(Conn,uMsgID)
				end
			end
			return
		end
	end
	local parameter = {["playerid"] = Player.m_uID, ["mailid"] = mail_id, ["playername"] = Player.m_Properties:GetCharName(),
						["SceneName"] = Conn.m_Player.m_Scene.m_SceneName }
	CallAccountManualTrans(Conn.m_Account, MailDB, "CreateLetter", CallBack, parameter)
end
--删除邮件
function MailMgr.DeleteMail(Conn, MailId)
	local function CallBack(result)
		if not result or IsNumber(result) then
			if IsNumber(result) then
				MsgToConn(Conn,result)
			end
			Gas2Gac:RetDeleteMail(Conn, ActionState.Failure)
			return
		end
		Gas2Gac:RetDeleteMail(Conn, ActionState.Succ)
	end
	
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local parameter = { ["mail_id"]=MailId, ["char_id"]=Player.m_uID }
	CallAccountManualTrans(Conn.m_Account, MailDB, "DeleteMail", CallBack, parameter)
end
                                                                        
function MailMgr.GetMailMoney(Conn,MailId)
	
	local function CallBack(result)
		if not result[1] then
			if IsNumber(result[2]) then
				MsgToConn(Conn,result[2])
			end
			Gas2Gac:RetTakeAtachmentByIndex(Conn,0,ActionState.Failure, MailId)
			return
		end
		Gas2Gac:RetTakeAtachmentByIndex(Conn,0,ActionState.Succ, MailId)--返回提取金钱
	end
	
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local parameter = { ["mail_id"]=MailId, ["char_id"]=Player.m_uID }
	CallAccountManualTrans(Conn.m_Account, MailDB, "GetMailMoney", CallBack, parameter)
end 

function MailMgr.DeleteBatchMailBegin(Conn)
	Conn.DeleteBatchMailInfo = {}
end

function MailMgr.DeleteBatchMail(Conn, MailID)
	table.insert(Conn.DeleteBatchMailInfo, MailID)
end

function MailMgr.DeleteBatchMailEnd(Conn)
	local Player = Conn.m_Player
	if Player == nil then return end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160032)
		return 
	end
	local function CallBack(result)
		if not result or IsNumber(result) then
			if IsNumber(result) then
				MsgToConn(Conn,result)
			end
			Gas2Gac:RetDeleteMail(Conn, ActionState.Failure)
		else
			Gas2Gac:RetDeleteMail(Conn, ActionState.Succ)
		end
	end
	local parameter = { ["mail_id_tbl"]=Conn.DeleteBatchMailInfo , ["char_id"]=Player.m_uID}
	CallAccountManualTrans(Conn.m_Account, MailDB, "DeleteBatchMail", CallBack, parameter)
end

--- @brief 获取文本附件
function MailMgr.CheckMailTextAttachment(Conn, nRoomIndex, nPos)
	local function CallBack(result)
		local row = result:GetRowNum()
		local info = result:GetRowSet(1)
		if row > 0 then
			Gas2Gac:RetCheckMailTextAttachment(Conn,info(1),info(2),info(3),info(4),info(5),info(6))
		end
	end
	
	local parameter = {["player_id"] = Conn.m_Player.m_uID, ["room_index"] = nRoomIndex, ["pos"] = nPos}
	CallAccountManualTrans(Conn.m_Account, "MailItemDB", "SendMailTextAttachment", CallBack, parameter)
end

function MailMgr.GetSendMoneyCess(Conn,SendMoney)
	local Player = Conn.m_Player
 	if not (Player and IsCppBound(Player))  then return end
 	
 	local function CallBack(cess)
 			Gas2Gac:RetGetSendMoneyCess(Conn,SendMoney*cess,cess*100)
 	end
 	CallDbTrans("MoneyMgrDB", "GetMoneyExchangeCess", CallBack, {["char_id"] = Player.m_uID,["exchange_money"] = SendMoney})
end

---------------------------------------------------------------
function MailMgr.SystemFriendMsgToClient(uCharId,msgID)
	SystemFriendMsgToClientCrossServer(uCharId, msgID)
end

function MailMgr.DelAllOvertimeMail()
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans(MailDB,"DelAllOvertimeMail", nil,{})
		end
	end
	g_AlarmClock:AddTask("DelAllOvertimeMail", {wday = {1,2,3,4,5,6,7},time = {"02:00"}}, CallBack)
end