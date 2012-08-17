function DbCallBack:SendIBPayInfo()
	if GasConfig.EratingIBPaySend ~= 0 then
		Gas2GasAutoCall:SendIBPayInfo(GetServerAutoConn(1))
	end
end

function DbCallBack:SendEratingAuditTick()
	Gas2GasAutoCall:SendEratingAuditTick(GetServerAutoConn(1))
end

function DbCallBack:SendMoneyExchangeInfo()
	if GasConfig.EratingIBPaySend ~= 0 then
		Gas2GasAutoCall:SendMoneyExchangeInfo(GetServerAutoConn(1))
	end
end

function DbCallBack:ReturnAddMoney(char_id,money_type,money_amount)
	if 1 == money_type then
		Gas2GacById:ReturnAddMoney(char_id, money_amount)
	elseif 2 == money_type then
		Gas2GacById:ReturnAddBindingMoney(char_id, money_amount)
	elseif 3 == money_type then
		Gas2GacById:ReturnAddBindingTicket(char_id, money_amount)
	end
end

function DbCallBack:RetAddItemInfoEnd(char_id,item_info)
	CPutItemsToBag.RetAddItemInfoEnd(item_info,char_id)
end

function DbCallBack:RetItemMakerName(nCharID,tbl_res)
	CPutItemsToBag.RetItemMakerName(nCharID,tbl_res)
end

function DbCallBack:GetItemFromA2B(nCharID,AllItem,nARoom,nAPos,nBGridID,nBRoom,nBPos)
	CPutItemsToBag.GetItemFromA2B(nCharID,AllItem,nARoom,nAPos,nBGridID,nBRoom,nBPos)
end

function DbCallBack:UpdateItemBindingType(nCharID,itemInfo)
	CPutItemsToBag.UpdateItemBindingType(nCharID,itemInfo)
end

function DbCallBack:RetItemRoom(nCharID,nRoomIndex)
	CPutItemsToBag.RetItemRoom(nCharID,nRoomIndex)
end

function DbCallBack:RetAddItemID(nCharID,nItemID)
	CPutItemsToBag.RetAddItemID(nCharID,nItemID)
end

function DbCallBack:RetAddItemToGridEndEx(nCharID,nPosIndex,nPos)
	CPutItemsToBag.RetAddItemToGridEndEx(nCharID,nPosIndex,nPos)
end

function DbCallBack:RetRefreshBag(nCharID)
	CPutItemsToBag.RetRefreshBag(nCharID)
end

function DbCallBack:RetDelItemByPos(nCharID,nItemID,nRoomIndex,nPos,bFlag)
	CPutItemsToBag.RetDelItemByPos(nCharID,nItemID,nRoomIndex,nPos,bFlag)
end

function DbCallBack:ResetRoomPosByGridID(nCharID,nGridID,nRoomIndex,nPos)
	CPutItemsToBag.ResetRoomPosByGridID(nCharID,nGridID,nRoomIndex,nPos)
end

function DbCallBack:RetItemDynInfo(nCharID, nBigID, nIndex, nItemID, itemInfoTbl)
	CGetItemDynInfo.RetItemDynInfo(nCharID, nBigID, nIndex, nItemID, itemInfoTbl)
end

function DbCallBack:LiveSkillSendMail(nCharID)
	CDirectionsMake.LiveSkillSendMail(nCharID)
end

function DbCallBack:ReturnMakeDirectionForCM(nCharID,tblRetRes)
	CDirectionsMake.ReturnMakeDirectionForCM(nCharID,tblRetRes)
end

function DbCallBack:RetDelPet(nCharID,nItemName,nItemType)
	CPutItemsToBag.RetDelPet(nCharID,nItemName,nItemType)
end

function DbCallBack:ReturnTeamProfferPoint(nCharID,TeamProffer)
	Gas2GacById:ReturnTeamProfferPoint(nCharID,TeamProffer)
end

function DbCallBack:ReturnProfferPoint(nCharID,ProfferPoint)
	Gas2GacById:ReturnProfferPoint(nCharID,ProfferPoint)
end

function DbCallBack:SystemFriendMsgToClient(uCharId,msgID)
	MailMgr.SystemFriendMsgToClient(uCharId,msgID)
end
