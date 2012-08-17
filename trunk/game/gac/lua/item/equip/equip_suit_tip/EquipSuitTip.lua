cfg_load "equip/intensify/EquipSoulAttr_Common"
CEquipSuitTipWnd = class (SQRDialog)

local NoticeColor = g_ColorMgr:GetColor("不可用颜色")

local function CreateEquipSuitTipWnd(wnd)
    wnd:CreateFromRes("EquipSuitTipWnd", g_GameMain.m_EquipUpIntensifyWnd)
    
    wnd:InitChild()
    g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
end

function CEquipSuitTipWnd:Ctor()
    CreateEquipSuitTipWnd(self)
end


function CEquipSuitTipWnd:InitChild()
    self.m_CloseBtn     = self:GetDlgChild("CloseBtn")
    self.m_EquipPart    = self:GetDlgChild("EquipPart")
    self.m_SoulRoot     = self:GetDlgChild("SoulRoot")
    self.m_Attr1        = self:GetDlgChild("Attr1")
    self.m_Attr2        = self:GetDlgChild("Attr2")
    self.m_TwoCountSuitList = self:GetDlgChild("TwoCountSuit")
    self.m_ThreeCountSuitList = self:GetDlgChild("ThreeCountSuit")
    self.m_FourCountSuitList  = self:GetDlgChild("FourCountSuit")
    self.m_SixCountSuitList   = self:GetDlgChild("SixCountSuit")
    self.m_EightCountSuitList = self:GetDlgChild("EightCountSuit")
    
    self.m_SuitListCtrlTbl = {self.m_TwoCountSuitList, self.m_ThreeCountSuitList, self.m_FourCountSuitList,
    self.m_SixCountSuitList, self.m_EightCountSuitList}
    self.m_CurChoosedSuitIndex = 0
    self.m_CurEquipID = 0
end

function CEquipSuitTipWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
        end
    elseif ( uMsgID == ITEM_LBUTTONCLICK) then
        local item = Child:GetSubItem(uParam1,0)
        if item ~= nil then
            if self.m_CurChoosedSuitIndex ~= item.Pos then
                local lastChoosedSuitIndex = self.m_CurChoosedSuitIndex
                self.m_CurChoosedSuitIndex = item.Pos
                self:UpdateChoosedSuitNameFlag(lastChoosedSuitIndex, item.Pos)
            end
        end
    end
end

function CEquipSuitTipWnd:SetEquipTipInfo(equipID)
    local lastChoosedIndex = self.m_CurChoosedSuitIndex 

    if self.m_CurEquipID == 0 or self.m_CurEquipID ~= equipID then
        self.m_CurEquipID = equipID
        self.m_CurChoosedSuitIndex = 0
    end        

    local DynInfo  = g_DynItemInfoMgr:GetDynItemInfo(equipID)
    local itemType = DynInfo.m_nBigID
    local itemName = DynInfo.m_nIndex
    local itemInfo = g_ItemInfoMgr:GetItemInfo(itemType, itemName)
    local displayEquipPart 
    if g_ItemInfoMgr:IsWeapon(itemType) 
		or g_ItemInfoMgr:IsAssociateWeapon(itemType) 
		or g_ItemInfoMgr:IsRing(itemType) then
		 displayEquipPart = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayEquipType")
	else
		displayEquipPart = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayEquipPart")
	end
    self.m_EquipPart:SetWndText(displayEquipPart)
    local displaySoulRoot = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName, "SoulRootDisplayName")
    self.m_SoulRoot:SetWndText(displaySoulRoot)
    if DynInfo.IntensifyPhase >= 1 then
        local displayAttr = Lan_PropertyName_Common(MemH64(DynInfo.AddAttr1),"DisplayName")
        self.m_Attr1:SetWndText(displayAttr)
    else
        self.m_Attr1:SetWndText("")
        self.m_Attr2:SetWndText("")
    end
    if DynInfo.IntensifyPhase >= 2 then
        local displayAttr = Lan_PropertyName_Common(MemH64(DynInfo.AddAttr2),"DisplayName")
        self.m_Attr2:SetWndText(displayAttr)
    else
        self.m_Attr2:SetWndText("")
    end
    
    local soulRoot = itemInfo("SoulRoot")
    local levelType = GetEquipLevelType(DynInfo.BaseLevel)
    local equipPart =  GetEquipTypeInInten(itemType, itemName)
    local showListType = self:GetEquipSuitDetail(soulRoot, tostring(levelType), DynInfo, equipPart)
    self:SetEquipSuitDetail(itemInfo, DynInfo, showListType, equipPart)
    self:UpdateChoosedSuitNameFlag(lastChoosedIndex, self.m_CurChoosedSuitIndex)
end


local TwoCountAllSuitTbl = {}
local ThreeCountAllSuitTbl = {}
local FourCountAllSuitTbl = {}
local SixCountAllSuitTbl = {}
local EightCountAllSuitTbl = {}

local TwoCountCurSuitTbl = {}
local ThreeCountCurSuitTbl = {}
local FourCountCurSuitTbl = {}
local SixCountCurSuitTbl = {}
local EightCountCurSuitTbl = {}

local SuitCountTbl = {}
SuitCountTbl[1] = {TwoCountAllSuitTbl,"TwoCountSuit", TwoCountCurSuitTbl}
SuitCountTbl[2] = {ThreeCountAllSuitTbl,"ThreeCountSuit",ThreeCountCurSuitTbl}
SuitCountTbl[3] = {FourCountAllSuitTbl,"FourCountSuit",FourCountCurSuitTbl}
SuitCountTbl[4] = {SixCountAllSuitTbl,"SixCountSuit",SixCountCurSuitTbl}
SuitCountTbl[5] = {EightCountAllSuitTbl,"EightCountSuit",EightCountCurSuitTbl}


--根据强化属性1、2，魂根类型、装备部位等找出对应的可产出套装名称
function CEquipSuitTipWnd:GetSuitNameListByAttrInfo(suitCfg, levelTypeStr, attr1, attr2, equipPart,index)
    local infoTbl = suitCfg:GetKeys(levelTypeStr, attr1, attr2, equipPart)
    for q, r in pairs(infoTbl) do
        for h, n in pairs (SuitCountTbl) do
            local suitName =  suitCfg(levelTypeStr, attr1, attr2, equipPart, r, n[2])
            if suitName ~= "" and suitName ~= nil then
                local Name = CTooltips.GetIsTwinEquip(suitName,attr1,attr2,equipPart)
                local displaySuitName = g_SuitNameToLanTbl[suitName] .. Name
                if n[index][displaySuitName] == nil then
                    n[index][displaySuitName] = suitName
                end
            end
        end
    end    
end

---根据装备所属等级（1~40属于1，41~150属于2）、魂根属性、装备部位、GodWartSuit_Common（或者MagicActSuit_Common）
--查找到符合要求的所有强化属性1和强化属性2 
function CEquipSuitTipWnd:GetAllSuitNameInfo(levelTypeStr, soulRoot, equipPart, suitCfg)
    local defaultKeys = EquipSoulAttr_Common:GetKeys(levelTypeStr, soulRoot, equipPart)
    for i , v in pairs (defaultKeys) do
        local attr1 = EquipSoulAttr_Common(levelTypeStr, soulRoot, equipPart, v, "SoulAttr")--找出强化属性1
        local keys = suitCfg:GetKeys(levelTypeStr, attr1) --找出对应的强化属性2
        for j, k in pairs (keys) do
            local attr2 = k
            self:GetSuitNameListByAttrInfo(suitCfg, levelTypeStr, attr1, attr2, equipPart,1)
        end
    end    
end

function CEquipSuitTipWnd:GetEquipSuitDetail(soulRoot, levelTypeStr, DynInfo, equipPart)
    local intenPhase = DynInfo.IntensifyPhase
    local suitCfg =  GetSuitCfgBySoulType(soulRoot)
    SuitCountTbl[1][1] = {}
    SuitCountTbl[2][1] = {}
    SuitCountTbl[3][1] = {}
    SuitCountTbl[4][1] = {}
    SuitCountTbl[5][1] = {}
    SuitCountTbl[1][3] = {}
    SuitCountTbl[2][3] = {}
    SuitCountTbl[3][3] = {}
    SuitCountTbl[4][3] = {}
    SuitCountTbl[5][3] = {}
    local showList = 1
    if intenPhase < 1 then
        self:GetAllSuitNameInfo(levelTypeStr, soulRoot, equipPart, suitCfg)
        showList = 1
        
    elseif intenPhase == 1 then
        self:GetAllSuitNameInfo(levelTypeStr, soulRoot, equipPart, suitCfg)
        local attr1 = DynInfo.AddAttr1
        local keys = suitCfg:GetKeys(levelTypeStr, attr1)
        for j, k in pairs (keys) do
            local attr2 = k
            self:GetSuitNameListByAttrInfo(suitCfg, levelTypeStr, attr1, attr2, equipPart,3)
        end
        showList = 2
        
    elseif intenPhase >= 2 then
        self:GetAllSuitNameInfo(levelTypeStr, soulRoot, equipPart, suitCfg)
        local attr1 = DynInfo.AddAttr1
        local attr2 = DynInfo.AddAttr2
        --if equipPart ~= "盾牌" then
            self:GetSuitNameListByAttrInfo(suitCfg, levelTypeStr, attr1, attr2, equipPart,3)
        --end
        showList = 2
    end
    return showList
end


--套装已产出，设置套装查看面板相关信息
local function SetOutSuitInfo(dynInfo, displaySuitName, item, suitColor, suitCount, suitName)
    local color
    local equipAdaptedClass
    if g_ItemInfoMgr:IsWeapon(dynInfo.m_nBigID) then
        local equipType = g_ItemInfoMgr:GetItemInfo(dynInfo.m_nBigID, dynInfo.m_nIndex,"EquipType")	
        equipAdaptedClass= GetWeaponAdaptedClass(equipType)
    end
    if suitCount == 3 then --4件套,产出时对角色职业有要求

	    local needPartsCountKeys = EquipSuit_Common:GetKeys(suitName)
        if equipAdaptedClass ~=nil then -- 要求产出的职业套装和玩家自己的职业相符
            local needPartsCountKeys = EquipSuit_Common:GetKeys(suitName)
            for i,v in pairs (needPartsCountKeys) do
                local needPartsCount = v
                if tonumber(needPartsCount) == nil then
                    if equipAdaptedClass[needPartsCount] == nil then
                        item:EnableWnd(false)
                        color = g_ColorMgr:GetColor("出售价格") 
                        return color
                    else
                        item:EnableWnd(true)
                        color = g_ColorMgr:GetColor("描述")
                    end
                end
            end
        end
    end
    if equipAdaptedClass ~= nil then
        if IsEquipSuitAdaptedClass(suitName, equipAdaptedClass) then
            item:EnableWnd(true)
            color = g_ColorMgr:GetColor("描述")    
        else
            item:EnableWnd(false)
            color = g_ColorMgr:GetColor("出售价格") 
            return color
        end
    end
    if dynInfo.IntensifyPhase >= 3  then
        local SuitPartName = CTooltips.GetEquipPartName(dynInfo.m_nBigID,dynInfo.m_nIndex)
        local Name = CTooltips.GetIsTwinEquip(dynInfo.SuitName,dynInfo.AddAttr1,dynInfo.AddAttr2,SuitPartName)
        local equipDisplaySuitName = ""
        if g_SuitNameToLanTbl[dynInfo.SuitName] ~= nil then
            equipDisplaySuitName = g_SuitNameToLanTbl[dynInfo.SuitName] .. Name
        end
        if equipDisplaySuitName ~= displaySuitName then
            item:EnableWnd(false)
            color = g_ColorMgr:GetColor("出售价格")
        else
            item:EnableWnd(true)
            color = suitColor
        end
    else
        item:EnableWnd(true)
        color = g_ColorMgr:GetColor("描述")
    end
    return color
end


function CEquipSuitTipWnd:SetEquipSuitDetail(itemInfo, dynInfo, showListType, equipPart)
    self.m_TwoCountSuitList:DeleteAllItem()
    self.m_ThreeCountSuitList:DeleteAllItem()
    self.m_FourCountSuitList:DeleteAllItem()
    self.m_SixCountSuitList:DeleteAllItem()
    self.m_EightCountSuitList:DeleteAllItem()
    self.m_ItemTbl = {}
    self.m_ItemWndTbl = {}
    local pos = 1
    for i, v in pairs (SuitCountTbl) do
        local suitInfoTbl = v[1]
        self.m_SuitListCtrlTbl[i]:InsertColumn(0, 120)
        local index = 1 
        for j, k in pairs (suitInfoTbl) do
            local equipSuitTipItem = SQRDialog:new()
            local suitName = k
            self.m_SuitListCtrlTbl[i]:InsertItem(index-1, 20)
            local item = self.m_SuitListCtrlTbl[i]:GetSubItem(index-1, 0)
            equipSuitTipItem:CreateFromRes("EquipSuitTipItem", item)
            local displaySuitName = j
            local suitPartDes = ""
            local suitColor = ""
            if i == 1 or i== 4 then --2件套或者6件套
                suitColor = g_ColorMgr:GetColor("紫色装备")
            else
                suitColor = g_ColorMgr:GetColor("橙色装备")
            end
            suitPartDes = g_Tooltips:GetEquipSuitShowDesc(suitPartDes,itemInfo, dynInfo, suitColor, suitName)
            
            local keys = EquipSuit_Common:GetKeys(suitName)
            local partDescStr = EquipSuit_Common(suitName, keys[1], "NeedEquipParts")
            local partDescTbl = loadstring("return {" .. partDescStr .. "}")()
            local str = ""
            if # partDescTbl == 0 then
                str = NoticeColor .. GetStaticTextClient(4050)
            elseif # partDescTbl == 1 then
                if string.find(partDescStr, "戒", 1, true) then
                    str = NoticeColor .. GetStaticTextClient(4049)
                else
                    str = NoticeColor .. GetStaticTextClient(4048) 
                end
            elseif # partDescTbl == 2 then
                str = NoticeColor .. GetStaticTextClient(4051)
            end
             
            item:SetMouseOverDescAfter(suitPartDes .. "#r" .. str)
            table.insert(self.m_ItemTbl, item)
            
            local color 
            if showListType == 2 then --已强化最少一阶段，有些套装已经不能产出了
                local suitCurInfoTbl = v[3]
                if suitCurInfoTbl[displaySuitName] == nil then
                    item:EnableWnd(false)
                    color = g_ColorMgr:GetColor("出售价格")
                else
                    local displaySuitName = j
                    color = SetOutSuitInfo(dynInfo, displaySuitName, item, suitColor, i, suitName)
                end
            else
                local displaySuitName = j
                color = SetOutSuitInfo(dynInfo, displaySuitName, item, suitColor, i, suitName)
            end
            equipSuitTipItem:GetDlgChild("RichText"):SetWndText(color .. displaySuitName)
            equipSuitTipItem:ShowWnd(true)
            equipSuitTipItem.DisplaySuitName = displaySuitName
            equipSuitTipItem.Color = color
            table.insert(self.m_ItemWndTbl, equipSuitTipItem)
            index = index + 1
            item.Pos =pos
            pos = pos + 1
        end
    end
end

function CEquipSuitTipWnd.UpdateEquipSuitTipWnd(equipID)
    if g_GameMain.m_EquipUpIntensifyWnd.m_EquipSuitTipWnd ~= nil and
        g_GameMain.m_EquipUpIntensifyWnd.m_EquipSuitTipWnd:IsShow() then
        local DynInfo  = g_DynItemInfoMgr:GetDynItemInfo(equipID)
        local equipAdvancePhase = DynInfo.CurAdvancePhase
        local equipMaxAdvancePhase = DynInfo.MaxAdvancePhase
        if equipMaxAdvancePhase >= 5 and equipAdvancePhase == equipMaxAdvancePhase and
            DynInfo.IntensifyQuality > 0 then
            g_GameMain.m_EquipUpIntensifyWnd.m_EquipSuitTipWnd:SetEquipTipInfo(equipID) 
        else
            g_GameMain.m_EquipUpIntensifyWnd.m_EquipSuitTipWnd:ShowWnd(false)
        end    
    end
end


function CEquipSuitTipWnd:UpdateChoosedSuitNameFlag(lastChoosedSuitIndex, index)
    if self.m_ItemWndTbl[index] ~= nil then
        local str =  self.m_ItemWndTbl[index].Color .. self.m_ItemWndTbl[index].DisplaySuitName .. NoticeColor .. "(" ..  GetStaticTextClient(5021) .. ")"
        self.m_ItemWndTbl[index]:GetDlgChild("RichText"):SetWndText(str)
    end
    
    local lastSetCheckSuitWnd = self.m_ItemWndTbl[lastChoosedSuitIndex]
    if  lastSetCheckSuitWnd ~= nil and (lastChoosedSuitIndex ~= index or index ==0 ) then
        local lastChoosedSuitName =  lastSetCheckSuitWnd.DisplaySuitName
        lastSetCheckSuitWnd:GetDlgChild("RichText"):SetWndText(lastSetCheckSuitWnd.Color .. lastChoosedSuitName)
        self.m_ItemTbl[lastChoosedSuitIndex]:SetSelected(false)
    end
end
