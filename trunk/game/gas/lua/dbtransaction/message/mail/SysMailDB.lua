local g_ItemInfoMgr = CItemInfoMgr:new()	
local SysMailExecutor = CreateDbBox(...)

--根据物品id查询物品静态信息
function SysMailExecutor.GetItemInfoByTblIDs(item_ids)
	local query_string = "select is_uId,is_uType,is_sName from tbl_item_static where is_uId in ("
	local item_list = ""
	for i =1, #item_ids-1 do
		item_list = item_list .. item_ids[i][1] .. ", "
	end
	if #item_ids > 0 then
		item_list = item_list .. item_ids[#item_ids][1] .. ")"
	end
	item_list = item_list .. "order by is_uType,is_sName"
	query_string = query_string .. item_list
	local _, query_result = g_DbChannelMgr:TextExecute(query_string)
	return query_result
end

--【发送系统邮件接口】
--参数：SenderName系统邮件名称、RecieverID收件者id、MailTitle邮件标题、
--MailContent邮件内容、item_ids发送的物品id（二维table、没有传空table）、money_num发送的金钱（没有传0）
--SenderName\MailTitle\MailContent模仿现有的邮件，在disigner_Log_Common.dif中填写
--logMailType表示log类型，用来记录log的，在event_type_tbl中
--msgID是收到邮件后提示信息的messageid，在designer_SystemFriendMessage_Common.dif中
function SysMailExecutor.SendSysMail(SenderName,RecieverID,MailTitle,MailContent,item_ids,money_num,logMailType,msgID)
	local lMailExecutor = RequireDbBox("MailDB")
	local logItemTbl = {}
	local BoxitemDB = RequireDbBox("BoxitemDB")
   local tblMailID = lMailExecutor.SendMail(SenderName,RecieverID,MailTitle,MailContent,money_num,item_ids,nil,2,logMailType,msgID)
	if not tblMailID[1] then
		return false
	end
	if IsTable(item_ids) and # item_ids > 0 then
		local item_info = SysMailExecutor.GetItemInfoByTblIDs(item_ids)
		--从第一个格子放到第四个格子，格子不够，剩余物品则放到下一封邮件
		local nGridNum = 1
		local nHaveNum = 0
		for i=1,item_info:GetRowNum() do
			local nItemID,nItemType,sItemName = item_info(i,1),item_info(i,2),item_info(i,3)
			table.insert(logItemTbl,nItemID)
			if i > 1 then
				local BeforType,BeforName = item_info(i-1,2),item_info(i-1,3)
				local FoldLimit = g_ItemInfoMgr:GetItemInfo(BeforType,BeforName,"FoldLimit") or 1
				if nItemType ~= BeforType or BeforName ~= sItemName or (nHaveNum >= FoldLimit) then
					nHaveNum = 0
					if nGridNum == 4 then
						nGridNum = 1
						tblMailID = lMailExecutor.SendMail(SenderName,RecieverID,MailTitle,MailContent,money_num,item_ids,nil,2,logMailType,msgID)
					else
						nGridNum = nGridNum + 1
					end
				end
			end
			if not tblMailID[1] then
			    return false
			else
				local ret = BoxitemDB.UpdateBoxitemBelonging(nItemType, RecieverID, nItemID)
				if ret == false then
			  	 return false 
				end
				if not lMailExecutor.AddMailItem(tblMailID[1],nItemID,nGridNum) then
					return false
				end
			end
			nHaveNum = nHaveNum +1
		end
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")

	g_LogMgr.PlayerSendEmail( logMailType or SenderName,RecieverID,logItemTbl,nil,money_num)
	return true
end

return SysMailExecutor