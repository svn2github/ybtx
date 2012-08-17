gac_gas_require "commerce/npc_shop/NPCShopMgr"
cfg_load "npc/NpcShop_Common"

local ShopFitNotCheckFlag = 0       --不筛选适合玩家的
local ShopFitCheckFlag  = 1         --筛选适合玩家的
local ShopFitUnableCheckFlag = 2    --不允许筛选
local NotWithOneCommonShop = 0--非随身商店
local WithOneCommonShopName = "随身商店"

CNPCShopSellWnd = class ( SQRDialog )
CNPCShopSellJiFenWnd = class ( SQRDialog )
local TongNpcShop = 10--团贡商店，不允许玩家卖东西到商店
local LiuTongMoneyType 	= 1 	--流通金币支付方式
local BindMoneyType 	= 2		--绑定金币支付方式
local LiuTongShop       = 1     --流通商店
local CommonShop        = 0     --通用商店
local LiuTongShopTipFlag= 1     --1为提示，0为不提示
local CommonShopTipFlag = 1     --1为提示，0为不提示

local function CreateNPCShopSell(Wnd)
	Wnd:CreateFromRes("NpcShopBuy", g_GameMain)
	Wnd.jifenWnd = CNPCShopSellJiFenWnd:new()
	Wnd.jifenWnd:CreateFromRes("NpcShopJiFenWnd", Wnd)
	Wnd.jifenWnd:ShowWnd( true )
	Wnd.jifenWnd:InitChild(Wnd)

	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	return Wnd
end


function CNPCShopSellWnd:Ctor()
    CreateNPCShopSell(self)
end

function CNPCShopSellWnd.ReturnTongProfferCount( tongProfferCount)
	g_MainPlayer.m_TongProffer  = tongProfferCount
    if g_GameMain.m_JiFenDetailWnd and g_GameMain.m_JiFenDetailWnd:IsShow() then
	    g_GameMain.m_JiFenDetailWnd:UpdateJiFenDetail()
	end
	g_GameMain.m_NPCShopSell.m_JiFenStaticTbl[TongNpcShop+1]:SetWndText(tongProfferCount)
	if g_GameMain.m_NPCShopSell.m_PayType == TongNpcShop then
	   Gac2Gas:GetTongLevel(g_Conn) 
	end
end

function CNPCShopSellWnd.ReturnTongLevel(tongLevel, stationLinLevel)
	g_GameMain.m_NPCShopSell.m_TongLevel = tongLevel
    g_GameMain.m_NPCShopSell.m_TongPlaceLevel = stationLinLevel
end

function CNPCShopSellWnd.ReturnUpdateMoneyPayType()
    Gac2Gas:GetTongProfferCount(g_Conn)
end

function CNPCShopSellWnd.ReturnNpcShopBuyTipInfo(liuTongShopTipFlag, commonShopTipFlag)
    g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag = liuTongShopTipFlag
    g_GameMain.m_NPCShopSell.m_CommonShopTipFlag  = commonShopTipFlag
    
    if g_GameMain.m_SysSetting.m_SysOtherSettingWnd 
        and g_GameMain.m_SysSetting.m_SysOtherSettingWnd:IsShow() then
        g_GameMain.m_SysSetting.m_SysOtherSettingWnd:UpdateSettingShopTipChkBtn()     
    end 
end

local function GetMessageIdByJiFenType(jiFenType)
	local messageIDTbl = {}
	messageIDTbl[1] = 190004
	messageIDTbl[2] = 190005
	messageIDTbl[3] = 190006
	messageIDTbl[4] = 190010
	messageIDTbl[5] = 190011
	messageIDTbl[6] = 190007
	messageIDTbl[7] = 190013
	messageIDTbl[8] = 190014
	messageIDTbl[9] = 190015
	messageIDTbl[10] = 190016
	return messageIDTbl[jiFenType]
end

function CNPCShopSellWnd.UpdatePlayerPoint(jiFenType,modifiedJiFenCount, bSpecial)

	if(10 == jiFenType and not bSpecial) then return end
		
	if g_MainPlayer == nil then
		return 
	end
	local messageID = GetMessageIdByJiFenType(jiFenType)
	  g_MainPlayer:UpdateJiFenCountByType(jiFenType, modifiedJiFenCount)
	local curJiFenCount = g_MainPlayer:GetJiFenCountByType(jiFenType)
	
	if g_GameMain.m_NPCShopSell:IsShow() ~= true and
		g_GameMain.m_NPCShopPlayerSold:IsShow() ~= true then
		MsgClient(messageID, modifiedJiFenCount)
	end
	g_GameMain.m_NPCShopSell.m_JiFenStaticTbl[jiFenType + 1]:SetWndText(curJiFenCount)
	
	if g_GameMain.m_JiFenDetailWnd and g_GameMain.m_JiFenDetailWnd:IsShow() then
	    g_GameMain.m_JiFenDetailWnd:UpdateJiFenDetail()
	end
end


function CNPCShopSellWnd.ReturnPlayerSoldGoodsListBegin()
    g_GameMain.NPCShopPlayerSoldCount = 0
    g_GameMain.m_PlayerSoldGoodsList = {}
end

function CNPCShopSellWnd.ReturnPlayerSoldGoodsList(sGoodsType,sGoodsName,nGoodsCount, nViewIndex, itemId,payType,uPosIndex)
	if g_GameMain.m_NPCShopSell.m_PayType ~= payType then
		return
	end
	local nBigId = tonumber(sGoodsType)
	g_GameMain.NPCShopPlayerSoldCount = g_GameMain.NPCShopPlayerSoldCount + 1
	local tbl = g_GameMain.m_PlayerSoldGoodsList
	node = {}
	node.GoodsType	= nBigId
	node.GoodsName	= sGoodsName
	node.GoodsCount	= nGoodsCount
	node.GoodsID	= itemId
	node.GoodsPosIndex = uPosIndex
	tbl[nViewIndex]	= node
end

function CNPCShopSellWnd.ReturnPlayerSoldGoodsListEnd()
	g_GameMain.m_NPCShopPlayerSold:ClearWndInfo()
	local maxShowCount  = g_GameMain.NPCShopPlayerSoldCount
	local tbl =  g_GameMain.m_PlayerSoldGoodsList
	for i,p in pairs(tbl) do 
		g_GameMain.m_NPCShopPlayerSold:ShowNPCShopPlayerSoldWnd(p.GoodsType,p.GoodsName,p.GoodsCount,i, p.GoodsID,p.GoodsPosIndex)  
	end
	
	SetEvent( Event.Test.RetPlayerSoldGoodsEnd, true, true )
end

--购买或购回物品之后，面板上显示数目的变化
--暂时的钱币只为原有系统中的money
function CNPCShopSellWnd.RetNpcTradeSucc ()
	g_GameMain.m_NPCShopSell:SetCurMoneyStr()
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(false, nil)
	SetEvent( Event.Test.ChangedMoneyCount, true, true )
	--同步刷新修理装备面板的数据
	if g_GameMain.m_NewFixEquipWnd:IsShow() then
		g_GameMain.m_CEquipInRoleWnd:SetData()
		g_GameMain.m_CEquipInBagWnd:SetData()
	end	
end

--初始化npc商店出售窗口的相关子控件
function CNPCShopSellWnd:OnChildCreated()
	self.m_BuyBack			=	self:GetDlgChild("BuyBack")			--切换到购回面板的tab标签页
	self.m_Close			=	self:GetDlgChild("Close")
	self.m_GoodsKind_01		=	self:GetDlgChild("GoodsKind_01")
	self.m_GoodsKind_02		=	self:GetDlgChild("GoodsKind_02")
	self.m_GoodsKind_03		=	self:GetDlgChild("GoodsKind_03")
	self.m_BindWay_Btn		=	self:GetDlgChild("BindWay")			--绑定金币支付方式
	self.m_CommonWay_Btn	=	self:GetDlgChild("LiuTongWay")		--流通金币支付方式
	self.m_ItemLisWnd		=	self:GetDlgChild("ItemList")
	self.m_BindJin			=	self:GetDlgChild("JinCount")
	self.m_BindYin			=	self:GetDlgChild("YinCount")
	self.m_BindTong			=	self:GetDlgChild("TongCount")
	self.m_LiuTongJin		=	self:GetDlgChild("GoldCount")
	self.m_LiuTongYin		=	self:GetDlgChild("SliverCount")
	self.m_LiuTongTong		=	self:GetDlgChild("CopperCount")
	
	self.m_BindJinBtn		=	self:GetDlgChild("Jin")
	self.m_BindYinBtn		=	self:GetDlgChild("Yin")
	self.m_BindTongBtn		=	self:GetDlgChild("Tong")
	self.m_LiuTongJinBtn	=	self:GetDlgChild("Gold")
	self.m_LiuTongYinBtn	=	self:GetDlgChild("Sliver")
	self.m_LiuTongTongBtn	=	self:GetDlgChild("Copper")
	--self:SetMouseWheel(true)
	self.m_ItemLisWnd:SetMouseWheel(true)
	self.m_FitBtn           =   self:GetDlgChild("FitBtn")
	self.m_ItemLisWnd:InserBefore(self:GetDlgChild("Aside"))
end

function CNPCShopSellJiFenWnd:InitChild(parentWnd)
	local npcShopWnd = parentWnd
	npcShopWnd.m_JiFenStaticTbl = {}
	for i=1, 11 do
		local str = "JiFenStatic" .. (i-1)
		table.insert(npcShopWnd.m_JiFenStaticTbl, self:GetDlgChild(str))
	end
	npcShopWnd.m_JiFenChkBtnTbl = {}
	npcShopWnd.m_JiFenStaticTextTbl = {}
	for i=1, 10 do
		local str = "ChkBtn" .. i
		local child = self:GetDlgChild(str)
		table.insert(npcShopWnd.m_JiFenChkBtnTbl, child)
		child:EnableWnd(false)
		child:SetCheck(false)
		local jifenText = "JiFenStaticText" .. i
		local jifenTextChild = self:GetDlgChild(jifenText)
		local textNum = 250 + i
		if i >= 7 then
		    textNum = 298 + i - 7
 		end
 		if i == TongNpcShop then
 		    textNum = 1200
 	    end
		jifenTextChild:SetWndText(ToolTipsText(textNum))
		table.insert(npcShopWnd.m_JiFenStaticTextTbl, jifenTextChild)
	end
end

--如果是用积分支付的商店，显示玩家拥有的积分数目
function CNPCShopSellWnd:ShowJiFenCountInShopOrNot(isShow)
	self.jifenWnd:ShowWnd(isShow)
	for i=1, 10 do
		self.m_JiFenStaticTbl[i+ 1]:SetWndText(g_MainPlayer:GetJiFenCountByType(i))
		self.m_JiFenChkBtnTbl[i]:SetCheck(false)
		self.m_JiFenChkBtnTbl[i]:EnableWnd(false)
	end
end

function CNPCShopSellWnd:NPCShopSellWnd(npcName)
	SetEvent( Event.Test.OpenNpcSellWnd, true, true)
	if (g_GameMain.m_NPCShopSell:IsShow() == false or
		npcName ~= g_GameMain.m_NPCShopSell.npcName) then
		if  npcName == nil  then
		    if  g_GameMain.m_NPCShopSell:InitWithOneShop(1) == false then
                MessageBox(g_GameMain, MsgBoxMsg(104), MB_BtnOK, nil,g_GameMain)
    		    return 
		    end
		else
            if g_GameMain.m_NPCShopSell:InitNPCShop(npcName)  == false then--NpcShop_Common[npcName].ShopName
    		    MessageBox(g_GameMain, MsgBoxMsg(104), MB_BtnOK, nil,g_GameMain)
    		    return
    		end
        end
		
		g_GameMain.m_NPCShopSell:ShowWnd(true)
		g_GameMain.m_WndMainBag:ShowWnd(true)
		if g_GameMain.m_NPCShopSell.m_buyBatchWnd ~= nil and 
			g_GameMain.m_NPCShopSell.m_buyBatchWnd:IsShow() then
			g_GameMain.m_NPCShopSell.m_buyBatchWnd:ShowWnd(false)
		end
		if NpcShop_Common(npcName, "ShopType") == NotWithOneCommonShop then
    		local AutoCloseWnd = CAutoCloseWnd:new()
    		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_WndMainBag)
    		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_NPCShopSell)
        end
		g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(false, nil)
		g_GameMain.m_RoleStatus:UpdataEquipTips()
	end
end

function CNPCShopSellWnd:VirtualExcludeWndClosed()
	if g_GameMain.m_NPCShopPlayerSold:IsShow() == false then
	    g_GameMain.m_NPCShopSell.m_ShopOpenFlag =false
		g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(false, nil)
		g_GameMain.m_RoleStatus:UpdataEquipTips()
	end
	if g_GameMain.m_NPCShopSell.m_buyBatchWnd 
	    and g_GameMain.m_NPCShopSell.m_buyBatchWnd:IsShow() then
	    g_GameMain.m_NPCShopSell.m_buyBatchWnd:ShowWnd(false)
	end
end

function CNPCShopSellWnd:SetCurMoneyStr()
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local tblWnd1 = {self.m_BindJin,self.m_BindJinBtn,self.m_BindYin,self.m_BindYinBtn,self.m_BindTong,self.m_BindTongBtn}
	money:ShowMoneyInfo(g_MainPlayer.m_nBindMoney,tblWnd1)
	local tblWnd2 = {self.m_LiuTongJin,self.m_LiuTongJinBtn,self.m_LiuTongYin,self.m_LiuTongYinBtn,self.m_LiuTongTong,self.m_LiuTongTongBtn}
	money:ShowMoneyInfo(g_MainPlayer.m_nMoney,tblWnd2)
	
	local sold = g_GameMain.m_NPCShopPlayerSold
	local tblWnd3 = {sold.m_BindJin,sold.m_BindJinBtn,sold.m_BindYin,sold.m_BindYinBtn,sold.m_BindTong,sold.m_BindTongBtn}
	money:ShowMoneyInfo(g_MainPlayer.m_nBindMoney,tblWnd3)
	local tblWnd4 = {sold.m_LiuTongJin,sold.m_LiuTongJinBtn,sold.m_LiuTongYin,sold.m_LiuTongYinBtn,sold.m_LiuTongTong,sold.m_LiuTongTongBtn}
	money:ShowMoneyInfo(g_MainPlayer.m_nMoney,tblWnd4)
end

local function CheckNpcShopOpenValidate(NPCName)
	local tbl = g_NPCShopMgr[NPCName]
	if tbl == nil  then
		--print(NPCName .. "在NPCShop_Common中没有对应的信息！")
		return false
	end
	return true, tbl
end


--初始化NPC商店
function CNPCShopSellWnd:InitNPCShop(NPCName)
    local ret, tbl = CheckNpcShopOpenValidate(NPCName)
	if ret == false then
        return false 
	end
	local typeCount = 0
	self.m_NPCShopGoodsList = {}
	for i, p in pairs(tbl) do
		if IsTable( p ) and typeCount < 2 then
			local itemtypeName = p.ItemType
			typeCount = typeCount + 1
			local wndStr = "GoodsKind_0" .. typeCount
			--local typeStr = ItemTypeName[tonumber(itemtypeName)
			local tabNum = 21000+ tonumber(itemtypeName)
			if StaticText_Common(tabNum) == nil then
			    tabNum = 21001
		    end
			local typeStr = GetStaticTextClient(tabNum)
			self:GetDlgChild( wndStr ):SetWndText( typeStr )
			self.m_NPCShopGoodsList[typeCount] = {}
			self.m_NPCShopGoodsList[typeCount].type = p.ItemType
			self.m_NPCShopGoodsList[typeCount].goodslist = p			
		end
	end
	
	
    for i=2, 3 do
	    local childStr = "GoodsKind_0" .. i
	    if i <= typeCount then
	        self:GetDlgChild(childStr):ShowWnd(true)
	    else
	        self:GetDlgChild(childStr):ShowWnd(false)
	    end
	end
	
	if tbl[1] == nil then
		self.m_ItemLisWnd:DeleteAllItem()
		return false
	end
	--以上是判断商店是否开放了，开放后再在下面初始话商店的其他信息
	self:InitCommonShopInfo(NPCName) 
	
	self:ShowGoodsMsg(self.m_NPCShopGoodsList[1].goodslist)
end

function CNPCShopSellWnd:InitWithOneShop(type)
    local NPCName = WithOneCommonShopName .. type
    local ret, tbl = CheckNpcShopOpenValidate(NPCName)
	if ret == false or # g_NPCShopMgr[NPCName] == 0  then
        return false 
	end
	
	local shopSellList = g_NPCShopMgr[NPCName]
	
	self:InitCommonShopInfo(NPCName)
    for i=1, 3 do
	    local childStr = "GoodsKind_0" .. i
        self:GetDlgChild(childStr):ShowWnd(true)
        self:GetDlgChild(childStr):SetWndText(GetStaticTextClient(21051 + i))
	end

	self:ShowGoodsMsg(shopSellList)
end

function CNPCShopSellWnd:InitCommonShopInfo(NPCName)
    g_GameMain.m_NPCShopSell:UpdateChoosedMoneyPayType(NPCName)
	self.m_PayType = NpcShop_Common(NPCName,"PayType")--该商店支持的支付方式
	self.m_LiuTongShopFlag = tonumber(NpcShop_Common(NPCName,"LiuTongShop"))
	self.curItemType = 1
	self.curPage = 1
	self.batchCount = 1
	
    self.m_ShopOpenFlag = true

	self:SetFocus()
	g_GameMain.maxShowCount = 0
	g_GameMain.m_PlayerSoldGoodsList = {}
	g_GameMain.NPCShopPlayerSoldCount = 0
	self:SetCurMoneyStr()
	
	self.m_GoodsKind_01:SetCheck(true)
	self.m_TongLevel = 0
	self.m_TongPlaceLevel = 0
	g_GameMain.m_NPCShopSell.npcName = NPCName
	self.m_CurShopType = NpcShop_Common(NPCName, "ShopType")
	self.m_FitFlag = NpcShop_Common(NPCName, "FitFlag")
	self.m_CheckFitFlag = false
	if ShopFitUnableCheckFlag == self.m_FitFlag then
        self.m_FitBtn:EnableWnd(false)
        self.m_FitBtn:SetCheck(false)
	elseif ShopFitCheckFlag == self.m_FitFlag then
	    self.m_FitBtn:SetCheck(true)
	    self.m_CheckFitFlag = true
	    self.m_FitBtn:EnableWnd(true)
	elseif ShopFitNotCheckFlag == self.m_FitFlag then
        self.m_FitBtn:SetCheck(false)
	    self.m_CheckFitFlag = false
	    self.m_FitBtn:EnableWnd(true)
	end
end

function CNPCShopSellWnd:GetShopSellList(curTabType)
    local goodslist
    if self.m_CurShopType <= NotWithOneCommonShop then 
        goodslist = self.m_NPCShopGoodsList[self.curItemType].goodslist
    else
        local npcName = WithOneCommonShopName .. curTabType
	    g_GameMain.m_NPCShopSell.npcName = npcName
	    self.m_PayType = NpcShop_Common(npcName,"PayType")
	    self.m_LiuTongShopFlag = tonumber(NpcShop_Common(npcName,"LiuTongShop"))
	    g_GameMain.m_NPCShopSell:UpdateChoosedMoneyPayType(npcName)
        goodslist = g_NPCShopMgr[npcName]
    end
    return goodslist
end

function CNPCShopSellWnd:GetChoosedItemTypeAndName(choosedIndex)
    local goodslist
    if self.m_CurShopType <= NotWithOneCommonShop then 
        local itemType = self.m_NPCShopGoodsList[self.curItemType].type
        if self.m_CheckFitFlag then
            goodslist = self:FilterSellGoodsList(self.m_NPCShopGoodsList[self.curItemType].goodslist)
        else
            goodslist = self.m_NPCShopGoodsList[self.curItemType].goodslist
        end
        local itemName = goodslist[choosedIndex]
        return  itemType, itemName
    else
        goodslist = g_NPCShopMgr[WithOneCommonShopName .. self.curItemType][choosedIndex]
        if self.m_CheckFitFlag then
            goodslist = g_NPCShopMgr[WithOneCommonShopName .. self.curItemType]
            goodslist = self:FilterSellGoodsList(goodslist)
            goodslist = goodslist[choosedIndex]
        end 
        local itemType, itemName = goodslist[1], goodslist[2]
        return  itemType, itemName
    end
end

function CNPCShopSellWnd:ShowNpcSellListByType(curTabType)
    local goodslist = self:GetShopSellList(curTabType)
    self:ShowGoodsMsg( goodslist )
end

function CNPCShopSellWnd:UpdateChoosedMoneyPayType(NPCName)
    local liuTongShopFlag = tonumber(NpcShop_Common(NPCName,"LiuTongShop"))
  	local curMoneyType = g_MainPlayer:GetMoneyType()
  	local getTongProfferFlag = true
	if liuTongShopFlag == 1 and curMoneyType == BindMoneyType then
	    Gac2Gas:UpdateChoosedMoneyPayType(g_Conn, LiuTongMoneyType)
	    getTongProfferFlag = false
	end
	if liuTongShopFlag == 0 and curMoneyType == LiuTongMoneyType then
        Gac2Gas:UpdateChoosedMoneyPayType(g_Conn, BindMoneyType)
        getTongProfferFlag = false
	end  
	if getTongProfferFlag then
	    Gac2Gas:GetTongProfferCount(g_Conn)
	end
end

--不能使用的物品上面蒙一层红色
function CNPCShopSellWnd:ShowOverIconWnd( itemType, itemName)
	local bFlag1,bFlag2 = false,false
	local Camp = g_ItemInfoMgr:GetItemInfo( itemType, itemName,"Camp" )
	--检测阵营
	if Camp == nil or Camp == "" then
		bFlag1 = true
	else
		local tbl = loadstring( "return{" .. Camp .. "}" )()
		for i =1,#tbl do
			local playerCamp = g_MainPlayer:CppGetCamp() 
			if playerCamp == tbl[i] then
				bFlag1 = true
				break
			end
		end
	end
	
	--职业
	local Class = g_ItemInfoMgr:GetItemInfo( itemType, itemName,"Class" )
	if Class == nil or Class == "" then
		bFlag2 = true
	else
		local tbl = loadstring( "return{" .. Class .. "}" )()
		for i =1,#tbl do
			local playerClass = g_MainPlayer:CppGetClass() 
			if playerClass == tbl[i] then
				bFlag2 = true
				break
			end
		end
	end
	--等级
	local BaseLevel = g_ItemInfoMgr:GetItemInfo( itemType, itemName,"BaseLevel" )
	local nLevelLimit = BaseLevel or 0
	return ((bFlag1 == false) or (false == bFlag2) or (nLevelLimit > g_MainPlayer:CppGetLevel()))
end

function CNPCShopSellWnd:FilterSellGoodsList(goodslist)
    local itemlistLen = table.maxn( goodslist)
    local tbl = {}
    local playerLevel = g_MainPlayer:CppGetLevel()
    for i=1, itemlistLen do
   		local itemType, itemName 
		if self.m_CurShopType <= NotWithOneCommonShop then --非随身普通商店
		    itemType = tonumber(goodslist.ItemType)
		    itemName = goodslist[i]
		else
		    itemType = goodslist[i][1]
		    itemName = goodslist[i][2]
	    end
	    local itemBaseLevel = g_ItemInfoMgr:GetItemInfo(itemType, itemName, "BaseLevel")
        if itemBaseLevel <= playerLevel + 5 and  IsEquipSuitedToPlayerByClass(itemType, itemName) then
            if self.m_CurShopType <= NotWithOneCommonShop then --非随身普通商店
                tbl.ItemType = itemType
                table.insert(tbl, itemName)
            else
                local node = {itemType, itemName}
                table.insert(tbl, node)
            end
        end
    end
    return tbl
end

--读取NPC商店所卖货物的信息，将其显示在购买窗口
function CNPCShopSellWnd:ShowGoodsMsg( goodslist, shopType )
    local tbl = goodslist
    if self.m_CheckFitFlag then
        goodslist = self:FilterSellGoodsList(goodslist)
    end
    
	local itemlistLen = table.maxn( goodslist)
	local column =	2
	local row    = math.ceil(itemlistLen/2)
	self.m_ItemLisWnd:DeleteAllItem()
	local itemWidth = self.m_ItemLisWnd:GetWndOrgWidth()
	for i=1, column do
		self.m_ItemLisWnd:InsertColumn( i-1, itemWidth/2)
	end
	local sellItemIndex = 1		--npc商店出售物品的初始索引为1
	gac_gas_require "framework/common/CMoney"
	local c_money = CMoney:new()
	self.m_ItemIconWndList = {}
	self.m_ItemWndList = {}
	self.m_ItemList = {}
	self.m_DisWndList = {}

	local isNeedShowJifenWnd = false
	for i=1, row do
		self.m_ItemLisWnd:InsertItem(i-1, 44)
		for j=1, column do
			if sellItemIndex <=  itemlistLen then
    			local itemType, itemName 
    			if self.m_CurShopType <= NotWithOneCommonShop then --非随身普通商店
    			    itemType = tonumber(goodslist.ItemType)
    			    itemName = goodslist[sellItemIndex]
    			else
    			    itemType = goodslist[sellItemIndex][1]
    			    itemName = goodslist[sellItemIndex][2]
    		    end
    			local soulpearlInfo, soulCount
    			if g_ItemInfoMgr:IsSoulPearl(itemType) then
    				soulpearlInfo = itemName
    				itemName, soulCount = g_ItemInfoMgr:GetSoulpearlInfo(itemName)
    			end
    			
			    
				local itemWnd = SQRDialog:new()
				local item	  = self.m_ItemLisWnd:GetSubItem(i-1, j-1)
				item:EnableWnd(true)
				itemWnd:CreateFromRes("NpcShopBuyItem", item)
				itemWnd:ShowWnd( true )
				local iconWnd = itemWnd:GetDlgChild("ItemBtn")
				local disWnd  = itemWnd:GetDlgChild("ItemDis")
				local Item_OverIconBtn = itemWnd:GetDlgChild("OverIconBtn")
				local priceWnd  =   itemWnd:GetDlgChild("Price")
				Item_OverIconBtn:SetBackImageColor(0x52ff0000)
				Item_OverIconBtn:SetMouseWheel(true)
                local moneyType = EGoldType.GoldBar
				if g_GameMain.m_NPCShopSell.m_LiuTongShopFlag == LiuTongShop then
				    moneyType = EGoldType.GoldCoin
				    local tex = "guitex/tex/ybtx/gui/3rdEdition/misc/shopgoods_f.tex"
					g_DynImageMgr:AddImageByIP(itemWnd, tex, IP_ENABLE)
                end
				--不能使用的物品上面蒙一层红色
				Item_OverIconBtn:ShowWnd(self:ShowOverIconWnd( itemType, itemName))
				local SmallIcon = g_ItemInfoMgr:GetItemInfo( itemType, itemName,"SmallIcon" )
				g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
			    local DisplayName = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayName")
			    local properDisplayName = DisplayName
			    if GetCharCount(DisplayName)/2 > 10 then
                    properDisplayName = SubUTF8String(DisplayName,0,9) .. GetStaticTextClient(1232)
			    end

			    disWnd:SetWndText(properDisplayName)
			    local priceInTongShop = ""
			    if self.m_PayType == TongNpcShop then
			        local c_money = CMoney:new()
			        local price = GetItemPriceInTongShop(itemType, itemName, 1, soulCount)
			        if price > 0 then
			            priceInTongShop = c_money:ChangeMoneyToString(price,moneyType) .. "+" 
			        end
			    end
			    local priceDesc = ""
				if g_ItemInfoMgr:IsSoulPearl(itemType) then
				    priceDesc = GetSoulpearlPriceInfo(itemName,soulCount, self.m_PayType, 1, moneyType)
				else
				    priceDesc = g_Tooltips:GetItemPriceByPayType(itemType, itemName, true, moneyType)    
				end
				local displayPrice = priceInTongShop .. priceDesc
                if GetCharCount(displayPrice) > 21 then
                    displayPrice = SubUTF8String(displayPrice,0,15) .. GetStaticTextClient(1232)
			    end
				priceWnd:SetWndText(displayPrice)
				if self.m_PayType ~= 0 then
				    isNeedShowJifenWnd = true
				else
				    isNeedShowJifenWnd = false
				end
				table.insert(self.m_ItemIconWndList, iconWnd)
				table.insert(self.m_DisWndList, disWnd)
				table.insert(self.m_ItemWndList, itemWnd)
				table.insert(self.m_ItemList, item)
				item:SetMouseOverDescAfter("")
				local str = g_ItemInfoMgr:IsSoulPearl(itemType) and soulpearlInfo or itemName--如果是魂珠,soulpearlInfo存的是魂根属性和魂值的连接字符串
				g_SetWndMultiToolTips(item,itemType, str, 0, 1, true, moneyType)
    			sellItemIndex = sellItemIndex + 1
			else
				local itemWnd = SQRDialog:new()
				local item	  = self.m_ItemLisWnd:GetSubItem(i-1, j-1)
				item:EnableWnd(false)
			end
		end
	end
	if isNeedShowJifenWnd then
		self:ShowJiFenCountInShopOrNot(true)
		self.m_JiFenChkBtnTbl[self.m_PayType]:EnableWnd(true)
		self.m_JiFenChkBtnTbl[self.m_PayType]:SetCheck(true)
	else
		self:ShowJiFenCountInShopOrNot(false)
	end
end

--手上有物品时，点击NPC商店卖掉手中的物品
function CNPCShopSellWnd:SellItem( context )
    
    if TongNpcShop == self.m_PayType then
        MessageBox(g_GameMain, MsgBoxMsg(105), MB_BtnOK) 
        return 
    end
   
	local function Callback(g_GameMain,uButton)
		g_GameMain.m_MsgBox = nil
		return true
	end
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	if fromRoom == g_AddRoomIndexClient.PlayerExpandBag then --如果是从背包栏拿出的包裹
		g_WndMouse:ClearCursorAll()
		return
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )

	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nBigId, itemName = grid:GetType()
	if g_ItemInfoMgr:IsQuestItem(nBigId) then		--不能出售任务物品
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(100), MB_BtnOK, Callback,g_GameMain)
		return
	end
	if g_ItemInfoMgr:IsSoulPearl(nBigId) then
		local nItemID = grid:GetItem():GetItemID()
		local item = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		local soulCount = item.nSoulNum
		if GetSoulpearlPriceByTypeAndCount(itemName,soulCount, self.m_PayType) <= 0 then
			MsgClient(6105)
			return  
		end
	elseif g_ItemInfoMgr:IsPickOreItem(nBigId) then
		local nItemID = grid:GetItem():GetItemID()
		local item = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		if item.MaxDura and item.MaxDura > item.CurDura then
			MsgClient(6105)
			return
		end
	else
	    local price = GetItemPriceByPayType(nBigId, itemName, self.m_PayType)
	    if price  == 0 then
			MsgClient(6105)
			return
		end
	end
	g_ItemInfoMgr:PlayItemSound(nBigId,itemName)
	local count = grid:Size()
	g_WndMouse:ClearCursorAll()
	g_GameMain.m_PlayerSoldGoodsList = {}
	g_GameMain.NPCShopPlayerSoldCount = 0
	local npcName = g_GameMain.m_NPCShopSell.npcName
	if g_MainPlayer == nil then
        return  
	end
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160019)
		return 
	end
	Gac2Gas:PlayerSellItemByPos(g_Conn, npcName, fromRoom, srcPos, count)
end

function CNPCShopSellWnd:OnLButtonClick( nFlags,  uParam1, uParam2 )
	self:OnLButtonUp( nFlags,  uParam1, uParam2 )
end

--左键点击NPC商店，手上有物品直接卖掉
function CNPCShopSellWnd:OnLButtonUp( nFlags,  uParam1, uParam2 )
	local state, context = g_WndMouse:GetCursorState()
	if state == ECursorState.eCS_PickupItem then --手上有物品
		self:SellItem( context )

	elseif state == ECursorState.eCS_PickupItemFromNPCShop then  --手上的物品是从NPC商店获得的
		g_WndMouse:ClearCursorAll()
		g_WndMouse:SetCursorState( ECursorState.eCS_Normal )
	end
end

--先按列然后按行数，index从1开始
local function GetChoosedItemIndex(uParam1, uParam2)
	local colNum = 2
	local index = uParam1 * colNum  + uParam2 + 1
	return index
end

local function GetItemIDByPos(context)
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	if fromRoom == g_AddRoomIndexClient.PlayerExpandBag then --如果是从背包栏拿出的包裹
		g_WndMouse:ClearCursorAll()
		return
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )

	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nBigId, itemName = grid:GetType()
	if nBigId < 5 and nBigId >9 then
	   return 
	end
	local itemID = grid:GetItem():GetItemID()
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"SmallIcon")
    return itemID, SmallIcon
end

--NPC商店购买窗口事件响应
function CNPCShopSellWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	local state, context = g_WndMouse:GetCursorState()

	if uMsgID == BUTTON_LCLICK  then
		if Child == g_GameMain.m_NPCShopSell.m_BuyBack then
			self:ShowWnd(false)
			self.m_ShopOpenFlag = true
			Gac2Gas:PlayerSoldGoodsList( g_Conn, g_GameMain.m_NPCShopSell.npcName )
			g_GameMain.m_NPCShopPlayerSold:ShowWnd(true)
			if NpcShop_Common(g_GameMain.m_NPCShopSell.npcName, "ShopType") == NotWithOneCommonShop then
			    local AutoCloseWnd = CAutoCloseWnd:new()
			    AutoCloseWnd:AutoCloseWnd(g_GameMain.m_NPCShopPlayerSold)
			end
			self:ShowJiFenCountInShopOrNot(false)
			g_WndMouse:ClearCursorAll()
		elseif Child == g_GameMain.m_NPCShopSell.m_Close then
			g_GameMain.m_NPCShopSell:ShowWnd(false)
			g_WndMouse:ClearCursorAll()

		elseif Child == g_GameMain.m_NPCShopSell.m_GoodsKind_01 then
			self.curItemType = 1
			self:ShowNpcSellListByType(self.curItemType)

		elseif Child == g_GameMain.m_NPCShopSell.m_GoodsKind_02 then
			self.curItemType = 2
			self:ShowNpcSellListByType(self.curItemType)
			
        elseif Child == g_GameMain.m_NPCShopSell.m_GoodsKind_03 then
			self.curItemType = 3
			self:ShowNpcSellListByType(self.curItemType)

		elseif Child == g_GameMain.m_NPCShopSell.m_BindWay_Btn then
			Gac2Gas:SetMoneyType(g_Conn, 2)

		elseif Child == g_GameMain.m_NPCShopSell.m_CommonWay_Btn then
			Gac2Gas:SetMoneyType(g_Conn, 1)
		elseif Child == g_GameMain.m_NPCShopSell.m_FitBtn then
            self.m_CheckFitFlag = not self.m_CheckFitFlag
            self:ShowNpcSellListByType(self.curItemType)
            g_GameMain.m_NPCShopSell.m_FitBtn:SetCheck(self.m_CheckFitFlag)
		end

	elseif ( uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_RBUTTONCLICK ) then
		if state == ECursorState.eCS_PickupItem then --手上有物品
			self:SellItem( context )
		else
		    self.batchCount = 1
			self.uMsgID = uMsgID
			self.clickedItemIndex = GetChoosedItemIndex(uParam1, uParam2)
			self:BuyItem()
		end
	elseif uMsgID == ITEM_SHIFTDOWN then
		self.uMsgID = uMsgID
		self.batchCount = 1
		self.clickedItemIndex = GetChoosedItemIndex(uParam1, uParam2)
		if self.m_buyBatchWnd == nil then
			self.m_buyBatchWnd = CBuyBacthWnd:new()
		else
			self.m_buyBatchWnd:ShowWnd(true)
		end
		self.m_buyBatchWnd.m_InputWnd:SetWndText(self.batchCount)
		self.m_buyBatchWnd:ResetTotalPrice(self.batchCount)
		
		self.m_buyBatchWnd.m_InputWnd:SetFocus()
	end
end

--购买物品
function CNPCShopSellWnd:BuyItem()
	local curItemType	= self.curItemType
	local itemType, itemName = self:GetChoosedItemTypeAndName( self.clickedItemIndex)
	if g_ItemInfoMgr:IsSoulPearl(itemType) then
		itemName, self.SoulCount =  g_ItemInfoMgr:GetSoulpearlInfo(itemName)
	end
	if itemType == nil or itemName == nil or self.clickedItemIndex == nil  then
		return
	end
	local iconWnd		= self.m_ItemIconWndList[self.clickedItemIndex]
	local context		= {itemType, itemName, iconWnd} 
	self:BuyItemByRightOrLeftClickItem(itemType, itemName, context)
end

function CNPCShopSellWnd:CheckMoneyOrJiFenIsEnough(itemType, itemName, context, roomIndex, pos)
	if self.m_PayType == TongNpcShop then
        local suc, needTongLevel, needTongPlaceLevel= CheckNpcShopItemTongLevel(itemType, itemName, self.m_TongLevel, self.m_TongPlaceLevel) 
        if suc == false  then
            if needTongLevel ~= 0 or needTongPlaceLevel ~= 0 then
                MessageBox(g_GameMain, MsgBoxMsg(106, needTongLevel, needTongPlaceLevel), MB_BtnOK)
                return
            end
        end
	end
	if self.m_PayType ~= 0 then     --该商店的支付方式是积分
		local payTypeMoney = g_MainPlayer:GetJiFenCountByType(self.m_PayType)
		local itemPrice = 0
		if g_ItemInfoMgr:IsSoulPearl(itemType) then
		    itemPrice = GetSoulpearlPriceByTypeAndCount(itemName, self.SoulCount, self.m_PayType)
		else
            itemPrice = GetItemJiFenPrice(itemType, itemName, 1, self.m_PayType) 
		end
		local extraPrice = 0
		if self.m_PayType == TongNpcShop then
		    extraPrice = GetItemPriceInTongShop(itemType, itemName, self.SoulCount or 0)
		end
		if itemPrice == 0 then
            MessageBox(self, MsgBoxMsg(107), MB_BtnOK) 
			return
	    end
		if itemPrice * self.batchCount > payTypeMoney then
			MessageBox(self, MsgBoxMsg(101), MB_BtnOK) 
			return
		else
		    self:IsLeftMoneyEnough( extraPrice, context, roomIndex, pos)
			--self:AfterCallBack( context , 0, roomIndex, pos) --第二個參數是綁定的錢或者不綁定的錢的type值，由於是積分，所以傳0
		end
	else
		if g_ItemInfoMgr:IsSoulPearl(context[1]) then
			local price = GetSoulpearlPriceByTypeAndCount(itemName,self.SoulCount, self.m_PayType)
			self:IsLeftMoneyEnough( price, context, roomIndex, pos)
		else
			local Price = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"Price")
			self:IsLeftMoneyEnough(Price, context, roomIndex, pos)
		end
	end
end

--区分左键点击物品和右键点击物品不同的购买方式
function CNPCShopSellWnd:BuyItemByRightOrLeftClickItem(itemType, itemName, context)
	if self.uMsgID == ITEM_RBUTTONCLICK or self.uMsgID == ITEM_SHIFTDOWN then --右键点击物品，直接购买
        local state = g_WndMouse:GetCursorState()
		if state == ECursorState.eCS_PickupItemFromNPCShop  then
            g_WndMouse:ClearCursorAll()
		    g_WndMouse:SetCursorState( ECursorState.eCS_Normal ) 
		else
		    self:CheckMoneyOrJiFenIsEnough(itemType, itemName, context)
		end
	else
		local itemType, itemName, iconWnd = context[1], context[2], context[3]
		g_WndMouse:SetPickupItemfromNPCShop( itemType, itemName, self.batchCount, iconWnd, WndState, context )
	end
end

function CNPCShopSellWnd:CheckShowShopTipWnd(payMoneyType)
    if self.m_LiuTongShopFlag == LiuTongShop then
        if g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag == LiuTongShopTipFlag then
            return true         
        else
            return false  
        end
    elseif self.m_LiuTongShopFlag == CommonShop then
        if g_GameMain.m_NPCShopSell.m_CommonShopTipFlag == CommonShopTipFlag and
            payMoneyType == LiuTongMoneyType then
            return true
        else
            return false
        end
    end
end

function CNPCShopSellWnd:SendBuyItemRpc(buyItemInfo)
    local context, payMoneyType, roomIndex, pos = buyItemInfo[1],buyItemInfo[2],buyItemInfo[3],buyItemInfo[4]
    local itemType, itemName, iconWnd = context[1], context[2], context[3]
	local npcName = g_GameMain.m_NPCShopSell.npcName
	
	if g_ItemInfoMgr:IsSoulPearl(itemType) then
        itemName = itemName .. ":" .. self.SoulCount 
	end
	if self.uMsgID == ITEM_RBUTTONCLICK or self.uMsgID == ITEM_SHIFTDOWN then --右键点击物品，直接购买
		g_WndMouse:ClearCursorAll()
		g_WndMouse:SetCursorState( ECursorState.eCS_Normal )	
		if g_MainPlayer.m_ItemBagLock then
			MsgClient(160018)
			return 
		end			
		Gac2Gas:BuyItem( g_Conn, npcName, itemType, itemName , self.batchCount, payMoneyType)
	else
		if g_MainPlayer.m_ItemBagLock then
			MsgClient(160018)
			return 
		end			
		Gac2Gas:BuyItemByPos( g_Conn, npcName, itemType, itemName, self.batchCount, roomIndex, pos, payMoneyType)
	end
	self.batchCount = 1
end

function CNPCShopSellWnd:CheckNeedMoney(context)
    local itemType, itemName, iconWnd = context[1], context[2], context[3]
    if self.m_PayType ~= 0 then
        if self.m_PayType == TongNpcShop then --国贡积分商店
            local itemMoneyPrice =  GetItemPriceInTongShop(itemType, itemName, 1)
            if itemMoneyPrice == 0 then
                return false 
            end
        end
    end
    return true
end


function CNPCShopSellWnd:AfterCallBack(context, payMoneyType, roomIndex, pos)
    self.m_BuyItemInfo = {context, payMoneyType, roomIndex, pos}	
	if self:CheckShowShopTipWnd(payMoneyType) then
	    if self:CheckNeedMoney(context) then
            local wnd = CNpcShopMsgWnd:new()
            wnd:ShowWnd(true)
            wnd.m_Text:SetWndText(GetStaticTextClient(21050))
        else
            self:SendBuyItemRpc(self.m_BuyItemInfo)
        end
    else
        self:SendBuyItemRpc(self.m_BuyItemInfo)
    end
end

function CNPCShopSellWnd:CheckLiuTongShop(curMoneyType, needMoney,context, roomIndex, pos)
	if self.m_LiuTongShopFlag == 1 and curMoneyType == 2 then --流通商店，只允许使用流通金币
        local function CallBack(Context,Button)
            if (Button == MB_BtnOK) then
                curMoneyType = 1
                self:AfterLiuTongShopCheck(curMoneyType, needMoney,context, roomIndex, pos)
            end
            return true
        end
        MessageBox( self, MsgBoxMsg(108), BitOr( MB_BtnOK, MB_BtnCancel),CallBack)
    else
        self:AfterLiuTongShopCheck(curMoneyType, needMoney,context, roomIndex, pos)
	end
end

function CNPCShopSellWnd:AfterLiuTongShopCheck(curMoneyType, needMoney,context, roomIndex, pos)
    local function callback(Context,Button) --如果选择确定按钮，则将支付方式改为另一种类型
		if(Button == MB_BtnOK) then  --显示已更改支付方式看不出效果
			if curMoneyType == 2 then
				if needMoney > g_MainPlayer.m_nMoney then   
					self.m_MsgBox  = MessageBox( self, MsgBoxMsg(102), MB_BtnOK )

					local m_buyBatchWnd = self.m_buyBatchWnd
					if m_buyBatchWnd ~= nil  and m_buyBatchWnd:IsShow() then
						self.m_buyBatchWnd:ShowWnd(false)
					end
				else
					--Gac2Gas:SetMoneyType(g_Conn, 1)
					self:AfterCallBack( context , 1, roomIndex, pos)
				end
			else
				if needMoney > g_MainPlayer.m_nBindMoney then
					self.m_MsgBox = MessageBox( self, MsgBoxMsg(102), MB_BtnOK )

					local m_buyBatchWnd = self.m_buyBatchWnd
					if m_buyBatchWnd ~= nil  and m_buyBatchWnd:IsShow() then
						self.m_buyBatchWnd:ShowWnd(false)
					end
				else
					--Gac2Gas:SetMoneyType(g_Conn, 2)
					self:AfterCallBack( context , 2, roomIndex, pos)
				end
			end
		else
			g_WndMouse:ClearCursorAll()
			g_WndMouse:SetCursorState( ECursorState.eCS_Normal )
		end

		return true
	end

    if self.m_LiuTongShopFlag == 1 and curMoneyType == 2 then --流通商店，只允许使用流通金币
        return  
    end

	if curMoneyType == 1 then --选择的是流通金币
		if needMoney > g_MainPlayer.m_nMoney then
		    if self.m_LiuTongShopFlag ~= 1 then
			    self.m_MsgBox = MessageBox( self, MsgBoxMsg(103), BitOr( MB_BtnOK, MB_BtnCancel),callback)
			else
                MessageBox( self, MsgBoxMsg(109), MB_BtnOK,nil)
			end
		else
			self:AfterCallBack( context, 1, roomIndex, pos)
		end
	elseif curMoneyType == 2 then
		if needMoney > g_MainPlayer.m_nBindMoney then
            self.m_MsgBox = MessageBox( self, MsgBoxMsg(103), BitOr( MB_BtnOK, MB_BtnCancel),callback)
		else
			self:AfterCallBack( context, 2, roomIndex, pos)
		end
	end
end

--检验剩余金钱是否足够购买玩家选择的物品；参数：物品价钱，物品数目；返回值：支付方式
--如果两种支付方式都不足以购买该物品，则放回0
function CNPCShopSellWnd:IsLeftMoneyEnough( itemPrice, context, roomIndex, pos )
	local needMoney = itemPrice * self.batchCount
	local curMoneyType = g_MainPlayer:GetMoneyType()
	self:CheckLiuTongShop(curMoneyType, needMoney, context, roomIndex, pos)
end


----------------脱穿装备时，更新商店中的tooltip比较信息--------------------------
function CNPCShopSellWnd:NpcSellUpdateMultiTooltips()
	local goodsList = self:GetShopSellList(self.curItemType)
    if self.m_CheckFitFlag then
        goodsList = self:FilterSellGoodsList(goodsList)
    end
	for i =1, # goodsList do
	    local itemType, itemName = self:GetChoosedItemTypeAndName(i)
	    if g_ItemInfoMgr:IsStaticEquip(itemType) == true then
		    local iconWnd = self.m_ItemList[i]
		    g_SetWndMultiToolTips(iconWnd, itemType, itemName, 0, 1, true)
	    end
	end
end
