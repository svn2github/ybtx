cfg_load "commerce/ToolsMall_Common"
cfg_load "commerce/ToolsMallItemPrice_Common"

local StmtOperater = {}	
local g_ItemInfoMgr = g_ItemInfoMgr
local ToolsMall_Common = ToolsMall_Common
local g_MoneyMgr = CMoney:new()
local StmtOperater = {}	
local ToolsMallItemPrice_Common = ToolsMallItemPrice_Common
local event_type_tbl = event_type_tbl
local ToolsMallBox = CreateDbBox(...)

------------------
local StmtDef=
{
    "_AddItemBuyerInfo",
    [[
    	insert into tbl_toolsmall_item_buyed 
    	set cs_uId = ?,is_uType = ?,is_sName = ?,tib_uCount = ?,tib_dtTime = now()
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddItemGeterInfo",
    [[
    	insert into tbl_toolsmall_item_geter 
    	set tib_uDetailID = ?,cs_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetHoteItemInfo",
    [[
    	select is_sName,is_uType,sum(tib_uCount) as nCount
    	from tbl_toolsmall_item_buyed
    	group by is_uType,is_sName
    	order by nCount DESC limit 10
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetHoteItemInfoByType",
    [[
    	select distinct(tib.is_sName),tib.is_uType,count(*) as nCount
    	from tbl_toolsmall_item_buyed as tib,tbl_toolsmall_tabtype_memory as ttm
    	where tib.is_uType = tib.is_uType and tib.is_sName = ttm.is_sName
    	and 	ttm.ttm_uTabType = ?
    	group by tib.is_uType,tib.is_sName
    	order by nCount DESC limit 10
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_DelBuyedInfoByTime",
    [[
    	delete from tbl_toolsmall_item_buyed where unix_timestamp(now())- unix_timestamp(tib_dtTime) >= 604800
    ]]
}
DefineSql(StmtDef,StmtOperater)
local function GetMoneyByPaytype(nItemType,sItemName,nPayType,nCount)
	local nPay = 0
	if 1 == nPayType then
			--流通金
			nPay = g_ItemInfoMgr:GetItemInfo(nItemType, sItemName, "Price")
	elseif 2 == nPayType then
			--绑定金
			nPay = ToolsMallItemPrice_Common(tostring(nItemType), sItemName, "BindingPrice")
	elseif 3 == nPayType then
		--积分
		nPay = ToolsMallItemPrice_Common(tostring(nItemType), sItemName, "JiFenPrice")
	end
	return -(nPay*nCount)
end
----------------------------------------------
--获得热卖商品
function ToolsMallBox.GetHotSaleItemInfo(data)
	local tab_type = data.m_uType
	if tab_type == 0 then
		return StmtOperater._GetHoteItemInfo:ExecStat()
	end
	return StmtOperater._GetHoteItemInfoByType:ExecStat(tab_type)
end

function ToolsMallBox.PayItemPrice(player_id,nPayType,nPay,sBuyType)
	local MoneyMgrDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("ToolMall")
	local sFuncType = fun_info.BuyItemForSelf
	local nEventType = event_type_tbl["商城购买"]
	if sBuyType == "Buyforother" then
		sFuncType = fun_info.BuyItemForOther
		nEventType = event_type_tbl["商城赠送"]
	end
	if nPayType == 1 or nPayType == 2 then
		local Flag,MsgID = MoneyMgrDB.EnoughMoneyByType(player_id, nPayType, -nPay)
		if not Flag then
			return Flag,MsgID
		end
		local bFlag,uMsgID = MoneyMgrDB.AddMoneyByType(fun_info.FunName,sFuncType,player_id,nPayType,nPay,nil,nEventType)
		if not bFlag then
			CancelTran()
			return false,uMsgID
		end
	elseif nPayType == 3 then
		 return  --TOdO JIFEN
	else
		return
	end
	return true
end

--购买
function ToolsMallBox.BuyItemFromToolsMall(parameter)
	local nBuyerID = parameter.m_nBuyerID
	local sItemName,nItemType = parameter.m_sItemName,parameter.m_nItemType
	local nPayType = parameter.m_nPayType
	local nCount = parameter.m_nCount
	local nPay = GetMoneyByPaytype(nItemType,sItemName,nPayType,nCount)
	if 0 == nPay then
		return 410
	end
	local bFlag,uMsgID = ToolsMallBox.PayItemPrice(nBuyerID,nPayType,nPay)
	if not bFlag then 
	    return uMsgID 
	end

	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local binding_type = nPayType == 2 and 2 or nil

	local BuyItemData = {}
	BuyItemData.m_nType = nItemType
	BuyItemData.m_sName = sItemName
	BuyItemData.m_nBindingType = binding_type
	BuyItemData.m_nCharID = nBuyerID
	BuyItemData.m_sCreateType = 101
	local item_ids = {}
	for i=1,nCount do
		local nItemID	= g_RoomMgr.CreateItem(BuyItemData)
		if not nItemID then
		 	CancelTran()
		 	return 404
		end
		table.insert(item_ids,{nItemID})
	end
	StmtOperater._AddItemBuyerInfo:ExecStat(nBuyerID,nItemType,sItemName,nCount)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return 408
	end
	local RetRes = {}
	RetRes.nPayType = nPayType
	RetRes.nPay = nPay
	if not g_RoomMgr.HaveEnoughRoomPos(nBuyerID,nItemType,sItemName,nCount)then
		--背包空间不足，发送到邮箱
		local SysMailDB = RequireDbBox("SysMailDB")
		local SenderName = 5000
		local MailTitle = 5000
		local MailContent = "5001_item:" .. nItemType .. "|" .. sItemName
		local nSendMoney = 0
		local succ = SysMailDB.SendSysMail(SenderName,nBuyerID,MailTitle,MailContent,item_ids,nSendMoney,101)
		if not succ  then
			CancelTran()
			return
		end
		RetRes.m_AddType = "ToMailBox"
		return true,RetRes
	end
	local BuyCountData = {}
	BuyCountData.m_nCharID = nBuyerID
	BuyCountData.m_nItemType = nItemType
	BuyCountData.m_sItemName = sItemName
	BuyCountData.m_tblItemID = item_ids
	local bFlag, ret_info = item_bag_box.AddCountItem(BuyCountData)
	if not bFlag then
		return ret_info
	end
	
	RetRes.m_AddType = "ToBag"
	RetRes.m_RetInfo = ret_info
	RetRes.detail_id = detail_id
	return true,RetRes
end

--赠送
function ToolsMallBox.GiveItemInToolsMallDB(paramt)
	local GeterName = paramt.m_sGeterName
	local nBuyerID = paramt.m_nBuyerID
	local sItemName,nItemType = paramt.m_sItemName,paramt.m_nItemType
	local nPayType = paramt.m_nPayType
	local nCount = paramt.m_nCount
	local sMsg = paramt.m_sMsg
	
	local ex = RequireDbBox("Exchanger")
	local nGeterID = ex.getPlayerIdByName(GeterName)
	local BuyerName = ex.getPlayerNameById(nBuyerID)
	if nGeterID == 0 then return 405 end
	if nGeterID == nBuyerID then return 418 end
	local nPay = GetMoneyByPaytype(nItemType,sItemName,nPayType,nCount)

	if 0 == nPay then
		return 410
	end
	local bFlag,uMsgID = ToolsMallBox.PayItemPrice(nBuyerID,nPayType,nPay,"Buyforother")
	if not bFlag then return uMsgID end

	--添加物品
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local binding_type = nPayType == 2 and 2 or nil
	local CreatItemData= {}
	CreatItemData.m_nType = nItemType
	CreatItemData.m_sName = sItemName
	CreatItemData.m_nBindingType = binding_type
	CreatItemData.m_nCharID = nBuyerID
	CreatItemData.m_sCreateType = 102
	local item_ids = {}
	for i =1,nCount do
		local nItemID	= g_RoomMgr.CreateItem(CreatItemData)
		if not nItemID then
		 	CancelTran()
		 	return 404
		end
		table.insert(item_ids,{nItemID})
	end
	StmtOperater._AddItemBuyerInfo:ExecStat(nBuyerID,nItemType,sItemName,nCount)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
	 	return 408 
	end
	StmtOperater._AddItemGeterInfo:ExecStat(g_DbChannelMgr:LastInsertId(),nGeterID)
	if  g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return 409 
	end
	
	local SysMailDB = RequireDbBox("SysMailDB")
	local SenderName = 5000
	local MailTitle = 5000
	local MailContent = "5002_" .. BuyerName .. "," .. sMsg
	local nSendMoney = 0
	local succ = SysMailDB.SendSysMail(SenderName,nGeterID,MailTitle,MailContent,item_ids,nSendMoney,102)
	if not succ  then
			CancelTran()
			return	
	end
	local RetRes = {}
	RetRes.nPayType = nPayType
	RetRes.nPay = nPay
	RetRes.nGeterID = nGeterID
	RetRes.detail_id = detail_id
	return true,RetRes
end

--初始化配置表热卖的物品信息到数据库
function ToolsMallBox.InitToolsmallTabTypeInfo()
	g_DbChannelMgr:TextExecute("delete from tbl_toolsmall_tabtype_memory")
	local tbl = {}
	local Tabs = ToolsMall_Common:GetKeys()
	for j =1,#Tabs do
		local Tabj = Tabs[j]
		local DefaultKeys = ToolsMall_Common(Tabj):GetKeys()
		for i =1,#DefaultKeys do
				local v = ToolsMall_Common(Tabj,DefaultKeys[i])
				local ItemType,ItemName = v("ItemType"),v("ItemName")		
				local strItemInfo = string.format("%s%d%s%d%s%s%s", "(", Tabj, ",", ItemType, ",'", ItemName,"')")
				table.insert(tbl,strItemInfo)
		end
	end
	if #tbl > 0 then
		local query_string = "insert into tbl_toolsmall_tabtype_memory(ttm_uTabType,is_uType,is_sName) values"
		query_string = query_string .. table.concat(tbl,",")
		g_DbChannelMgr:TextExecute(query_string)
	end
end

function ToolsMallBox.ClearBuyedInfoByTime()
	StmtOperater._DelBuyedInfoByTime:ExecStat()
end
----------------------------------------------
SetDbLocalFuncType(ToolsMallBox.BuyItemFromToolsMall)
SetDbLocalFuncType(ToolsMallBox.GiveItemInToolsMallDB)
return ToolsMallBox
