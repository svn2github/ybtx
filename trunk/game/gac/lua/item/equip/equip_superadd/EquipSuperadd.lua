cfg_load "equip/superadd/EquipSuperaddAttrRate_Common"
CEquipSuperaddWnd   =   class (SQRDialog)
local TotalNumColor = g_ColorMgr:GetColor("生活技能需要的材料总数目")
local NotEnoughNumColor =  g_ColorMgr:GetColor("生活技能物品数目不够")

local function  CreateEquipSuperaddWnd(wnd)
    wnd:CreateFromRes("EquipSuperadd", g_GameMain)
    
    wnd:InitChild()
    g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
end

function CEquipSuperaddWnd:Ctor()
    CreateEquipSuperaddWnd(self)
end


function CEquipSuperaddWnd.OpenSuperaddWnd()
    g_GameMain.m_EquipSuperaddWnd:ShowWnd(true)
    local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_EquipSuperaddWnd)
    g_GameMain.m_EquipSuperaddWnd:ClearEquipSuperaddInfo() 
    g_GameMain.m_WndMainBag.m_ChkMoney:SetCheck(true)
    
end

function CEquipSuperaddWnd:InitChild()
    self.m_CloseBtn     =   self:GetDlgChild("CloseBtn")
    self.m_EquipBtn     =   self:GetDlgChild("EquipBtn") 
    self.m_StoneStatic  =   self:GetDlgChild("StoneStatic") 
    self.m_SucProb      =   self:GetDlgChild("SucProb") 
    self.m_SoulNum      =   self:GetDlgChild("SoulNum") 
    self.m_MoneyNum     =   self:GetDlgChild("MoneyNum") 
    self.m_SuperaddBtn  =   self:GetDlgChild("SuperaddBtn") 
    self.m_StoneNum     =   self:GetDlgChild("StoneNum")
    self.m_Aoo1         =   self:GetDlgChild("a002")
    self.m_Aoo1:InserBefore(self.m_StoneNum)
    self.m_SuperaddBtn:EnableWnd(false)
end

function CEquipSuperaddWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    local state, context = g_WndMouse:GetCursorState()
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
        
        elseif Child == self.m_EquipBtn then
            if state == ECursorState.eCS_PickupItem then --包裹里的装备
                self:SetEquipSuperaddInfoFromBag(context)
            elseif ( state == ECursorState.eCS_PickupEquip ) then --身上的装备
                self:SetEquipSuperaddInfoFromRole(context)
            end
            
        elseif Child == self.m_SuperaddBtn then
            if # self.m_EquipInfo  > 0 then
                local liutongShopTipFlag    =  g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag
                local function CallBack(Context,Button)
                    if (Button == MB_BtnOK) then
                        self:EquipSuperadd()
                    end
                    return true
                end
                if liutongShopTipFlag == 1 then --提示信息打开
                    local wnd = CNpcShopMsgWnd:new()
                    wnd:ShowWnd(true)
                    wnd.m_Text:SetWndText(GetStaticTextClient(21051))
                else
                    self:EquipSuperadd()
                end                
            end
        end
    elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
        if Child == self.m_EquipBtn then 
            self:ClearEquipSuperaddInfo() 
        end
    end
end

function CEquipSuperaddWnd:EquipSuperadd()
    if # self.m_EquipInfo  > 0 then
        local itemType = self.m_EquipInfo[1]
        local itemName  = self.m_EquipInfo[2]
        local equipID  = self.m_EquipInfo[3]
        Gac2Gas:EquipSuperadd(g_Conn, itemType, itemName, equipID)     
    end
end

function CEquipSuperaddWnd:VirtualExcludeWndClosed()
    g_GameMain.m_WndMainBag.m_ChkBindingMoney:SetCheck(true)
end


function CEquipSuperaddWnd:ClearEquipSuperaddInfo() 
    self.m_EquipInfo = {}
    g_DelWndImage(self.m_EquipBtn,1,IP_ENABLE, IP_CLICKDOWN)
    self.m_EquipBtn:SetMouseOverDescAfter("")
    
    g_DelWndImage(self.m_StoneStatic,1,IP_ENABLE, IP_CLICKDOWN)
    self.m_StoneStatic:SetMouseOverDescAfter("")
    
    self.m_SucProb:SetWndText("")
    self.m_StoneNum:SetWndText("")
    self.m_SoulNum:SetWndText("")
    
    self.m_SuperaddBtn:EnableWnd(false)
    self.m_MoneyNum:SetWndText("")
end

function CEquipSuperaddWnd:SetEquipSuperaddInfoFromBag(context)
    local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
    local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )

	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nBigId, itemName = grid:GetType()
	local itemID = grid:GetItem():GetItemID()
	local itemType = tonumber(nBigId)
    self:SetEquipSuperaddCommonInfo(nBigId, itemName, itemID)
end

function CEquipSuperaddWnd:SetEquipSuperaddInfoFromRole(context)
    local tblInfo = {}
    tblInfo.m_Info, tblInfo.PartType, tblInfo.iconbtn = unpack(context)
    local nBigId, itemName = tblInfo.m_Info:GetBigID(), tblInfo.m_Info:GetIndex()
    local itemID = tblInfo.m_Info:GetItemID()
    self:SetEquipSuperaddCommonInfo(nBigId, itemName, itemID)
end

function CEquipSuperaddWnd:SetEquipSuperaddCommonInfo(nBigId, itemName, itemID)
	if not g_ItemInfoMgr:IsEquip(nBigId) then
	    MessageBox(self, MsgBoxMsg(1008), MB_BtnOK) 
	    g_WndMouse:ClearCursorAll()
        return  
	end 
		
	local dynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemID)
    local curSuperaddPhase = dynInfo.CurSuperaddPhase
	local equipLevel = dynInfo.BaseLevel
	
	local maxSuperaddPhase = GetMaxEquipSuperaddPhase(equipLevel)
	local ret, msgID = CheckEquipCanBeSuperadded(dynInfo.MaxAdvancePhase, dynInfo.CurSuperaddPhase, maxSuperaddPhase)
	if ret == false  then
        MessageBox(self, MsgBoxMsg(msgID), MB_BtnOK) 
        if msgID == 1051 then --装备追加达到上限
            self.m_SucProb:SetWndText("-")
            self.m_StoneNum:SetWndText("-/-")
        end
	    g_WndMouse:ClearCursorAll()
        return  
	end
	
	self.m_EquipInfo = {nBigId, itemName, itemID}
	

	
	local moneyConsumed,soulConsumed ,neededStoneNum= GetEquipSuperaddConsumeInfo(curSuperaddPhase, equipLevel, nBigId, itemName)
	
	self:CheckEquipSuperaddItemCount(neededStoneNum, equipLevel)
    self:SetConsumeInfo(curSuperaddPhase,moneyConsumed,soulConsumed)
    
    local SmallIcon = g_ItemInfoMgr:GetItemInfo(nBigId, itemName, "SmallIcon")
	g_DelWndImage(self.m_EquipBtn,1,IP_ENABLE, IP_CLICKDOWN)
	g_LoadIconFromRes(SmallIcon, self.m_EquipBtn, -1,  IP_ENABLE, IP_CLICKDOWN)
	g_SetWndMultiToolTips(self.m_EquipBtn, nBigId, itemName, itemID, 1)  
end

function CEquipSuperaddWnd:SetConsumeInfo(curSuperaddPhase, moneyConsumed, soulConsumed)
	local moneyMgr = CMoney:new()
	local moneyStr = moneyMgr:ChangeMoneyToString(moneyConsumed, EGoldType.GoldCoin)
	self.m_MoneyNum:SetWndText(moneyStr)
	
	self.m_SoulNum:SetWndText(soulConsumed)
	
	local sucProb = EquipSuperaddAttrRate_Common(curSuperaddPhase + 1, "AddAttrRate")
	self.m_SucProb:SetWndText(sucProb .. "%")
end

function CEquipSuperaddWnd:CheckEquipSuperaddItemCount(neededStoneNum, equipLevel)
	local equipSuperaddItemType = g_ItemInfoMgr:GetEquipSuperaddItem()
	local haveStoneNum = 0
	local itemList = g_GameMain.m_WndMainBag.m_ctItemRoom:GetAdaptedItemList(equipSuperaddItemType)
	local stoneName = ""
	for i,v in pairs (itemList) do
        local itemName = i
        stoneName = itemName
        local levelTbl = g_EquipSuperaddItemAdaptedEquipLevel[itemName]
        if levelTbl[1] <= equipLevel and equipLevel <= levelTbl[2] then
            haveStoneNum = haveStoneNum + v.Count
        end
	end
	
	if haveStoneNum >= neededStoneNum then
        self.m_SuperaddBtn:EnableWnd(true)
        self.m_StoneNum:SetWndText(TotalNumColor .. haveStoneNum .. TotalNumColor .. "/" .. neededStoneNum)
	else
        self.m_SuperaddBtn:EnableWnd(false)
        if haveStoneNum == 0 then
            haveStoneNumStr = "-" 
        else
            haveStoneNumStr = haveStoneNum
        end
        self.m_StoneNum:SetWndText(NotEnoughNumColor .. haveStoneNumStr .. TotalNumColor ..  "/" .. neededStoneNum)
	end
	if haveStoneNum > 0 then
        local SmallIcon = g_ItemInfoMgr:GetItemInfo(equipSuperaddItemType, stoneName, "SmallIcon")
    	g_DelWndImage(self.m_StoneStatic,1,IP_ENABLE, IP_CLICKDOWN)
    	g_LoadIconFromRes(SmallIcon, self.m_StoneStatic, -1,  IP_ENABLE, IP_CLICKDOWN)
    	g_SetWndMultiToolTips(self.m_StoneStatic, equipSuperaddItemType, stoneName, 0, 1)    
	else
	    g_DelWndImage(self.m_StoneStatic,1,IP_ENABLE, IP_CLICKDOWN)
	end
	g_WndMouse:ClearCursorAll()
end

function CEquipSuperaddWnd.RetEquipSuperaddSuc()
    if # g_GameMain.m_EquipSuperaddWnd.m_EquipInfo > 0 then
        local equipType = g_GameMain.m_EquipSuperaddWnd.m_EquipInfo[1]
        local equipName = g_GameMain.m_EquipSuperaddWnd.m_EquipInfo[2]
        local equipID   = g_GameMain.m_EquipSuperaddWnd.m_EquipInfo[3]
        g_GameMain.m_EquipSuperaddWnd:SetEquipSuperaddCommonInfo(equipType, equipName, equipID)  
    end
end

function CEquipSuperaddWnd:CloseSelf(equipID)
    if self.m_EquipInfo and # self.m_EquipInfo > 0 then
        g_GameMain.m_EquipSuperaddWnd:ShowWnd(false)    
    end
end