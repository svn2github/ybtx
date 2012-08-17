CEquipRefine    =   class(SQRDialog)
local TotalNumColor = g_ColorMgr:GetColor("生活技能需要的材料总数目")
local NotEnoughNumColor =  g_ColorMgr:GetColor("生活技能物品数目不够")

function CEquipRefine.RetEquipRefine(sucFlag)
    if sucFlag then
        g_GameMain.m_EquipRefineWnd:UpdateRefineEquipInfo()
    end
end

local function CreateEquipRefineWnd(wnd)
    wnd:CreateFromRes("EquipRefine", g_GameMain)
    wnd.m_CloseBtn  =   wnd:GetDlgChild("CloseBtn")
    wnd.m_CancelBtn =   wnd:GetDlgChild("CancelBtn")
    wnd.m_OkBtn     =   wnd:GetDlgChild("OkBtn")
    wnd.m_EquipBtn  =   wnd:GetDlgChild("EquipBtn")
    wnd.m_StoneBtn  =   wnd:GetDlgChild("StoneBtn")
    wnd.m_StoneNum  =   wnd:GetDlgChild("StoneNum")
    wnd.m_Aoo5 = wnd:GetDlgChild("A005")
    wnd.m_Aoo1 = wnd:GetDlgChild("A001")

    wnd.m_EquipBtn:InserBefore(wnd.m_Aoo5)
    wnd.m_EquipBtn:InserBefore(wnd.m_Aoo1)
    wnd.m_StoneBtn:InserBefore(wnd.m_Aoo5)
    wnd.m_StoneBtn:InserBefore(wnd.m_Aoo1)

    
    wnd.m_RefineEquipInfo = {}
    g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
    wnd.m_OkBtn:EnableWnd(false)
end

function CEquipRefine:Ctor()
    CreateEquipRefineWnd(self)
end

function CEquipRefine:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    local state, context = g_WndMouse:GetCursorState()
    if uMsgID == BUTTON_LCLICK then
        if Child == self.m_CloseBtn then
            self:ShowWnd(false)
            
        elseif Child == self.m_CancelBtn then
            self:ShowWnd(false)
            
        elseif Child == self.m_OkBtn then
            
            Gac2Gas:EquipRefine(g_Conn, self.m_RefineEquipInfo[3], self.m_RefineEquipInfo[1], self.m_RefineEquipInfo[2], self.m_RefineEquipInfo[4], self.m_RefineEquipInfo[5])
        elseif Child == self.m_EquipBtn then
            if state == ECursorState.eCS_PickupItem then --手上有物品
                self:SetRefineEquipInfo(context)
            end
        elseif Child == self.m_StoneBtn then
            --self:SetAdaptedStoneInfo(0)
        end
        
    elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
        if Child == self.m_EquipBtn then
            self:ClearEquipRefineInfo()
        end
    end
end

function CEquipRefine.OpenEquipRefineWnd()
    g_GameMain.m_EquipRefineWnd:ShowWnd(true)
    local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_EquipRefineWnd)
end

function CEquipRefine:VirtualExcludeWndClosed()
    self:ClearEquipRefineInfo()
    self.m_OkBtn:EnableWnd(false)
end

--设置要精炼的装备信息
function CEquipRefine:SetRefineEquipInfo(context)
    local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
    local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )

	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nBigId, itemName = grid:GetType()
	local itemID = grid:GetItem():GetItemID()
	local itemType = tonumber(nBigId)
	if not g_ItemInfoMgr:IsEquip(itemType) then
	    MessageBox(self, MsgBoxMsg(1008), MB_BtnOK) 
	    g_WndMouse:ClearCursorAll()
        return  
	end

    local dynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemID)
    local curAdvancePhase = dynInfo.CurAdvancePhase
    local advanceQuality =  dynInfo.MaxAdvancePhase
    local intensifyQuality =  dynInfo.IntensifyQuality

    local suc, needStoneCount = GetEquipRefineNeedStoneCount(curAdvancePhase, advanceQuality, intensifyQuality, nBigId, itemName)
    if suc == false then
       MessageBox(self, MsgBoxMsg(needStoneCount), MB_BtnOK)
       g_WndMouse:ClearCursorAll()
       return  
    end

	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, itemName, "SmallIcon")
	g_DelWndImage(self.m_EquipBtn,1,IP_ENABLE, IP_CLICKDOWN)
	g_LoadIconFromRes(SmallIcon, self.m_EquipBtn, -1,  IP_ENABLE, IP_CLICKDOWN)
	g_SetWndMultiToolTips(self.m_EquipBtn, itemType, itemName, itemID, 1)    
	
	self.m_RefineEquipInfo = {itemType, itemName, itemID}
	self:SetAdaptedStoneInfo(needStoneCount, intensifyQuality)
	self.m_EquipBtn:SetFocus()
    self.m_StoneBtn:SetFocus()
end

--放置装备后，设置是用的精炼石信息
function CEquipRefine:SetAdaptedStoneInfo(needStoneCount, intensifyQuality)
    local equipRefineStoneType = g_ItemInfoMgr:GetEquipRefineStoneType()
    local itemList = g_GameMain.m_WndMainBag.m_ctItemRoom:GetAdaptedItemList(equipRefineStoneType)
    local count = 0
    local itemName = ""
    self.m_OkBtn:EnableWnd(false)
    for i,v  in pairs (itemList) do
         local adaptedPhase = g_EquipRefineItemAdaptedPhaseInfo[i]
         if adaptedPhase[1] <= intensifyQuality + 1 and intensifyQuality + 1 <= adaptedPhase[2] then
            count = v.Count
            itemName = i
         end
    end

    if count > 0 then
        local str = ""
        if count >=  needStoneCount then
            str = TotalNumColor .. count .. "/" .. needStoneCount
            self.m_OkBtn:EnableWnd(true)
        else
            str = NotEnoughNumColor .. count .. TotalNumColor .. "/" .. needStoneCount
            self.m_OkBtn:EnableWnd(false)
        end
        
        local smallIcon = g_ItemInfoMgr:GetItemInfo(equipRefineStoneType, itemName, "SmallIcon")
        g_DelWndImage(self.m_StoneBtn,1,IP_ENABLE, IP_CLICKDOWN)
    	g_LoadIconFromRes(smallIcon, self.m_StoneBtn, -1,  IP_ENABLE, IP_CLICKDOWN)
    	g_SetWndMultiToolTips(self.m_StoneBtn, equipRefineStoneType, itemName, 0, 1)  
    	
    	self.m_StoneNum:SetWndText(str)
    	self.m_RefineEquipInfo[4] = equipRefineStoneType
    	self.m_RefineEquipInfo[5] = itemName
    end
    if count == 0 then
        str = NotEnoughNumColor .. "-" .. TotalNumColor .. "/" .. needStoneCount
        self.m_StoneNum:SetWndText(str)
        g_DelWndImage(self.m_StoneBtn,1,IP_ENABLE, IP_CLICKDOWN)
        self.m_StoneBtn:SetMouseOverDescAfter("")
    end
    g_WndMouse:ClearCursorAll()
end

function CEquipRefine:ClearEquipRefineInfo()
    g_DelWndImage(self.m_EquipBtn,1,IP_ENABLE, IP_CLICKDOWN)
    self.m_EquipBtn:SetMouseOverDescAfter("")
    self.m_RefineEquipInfo = {}
    
    self:ClearEquipRefineStoneInfo()
end

function CEquipRefine:ClearEquipRefineStoneInfo()
    g_DelWndImage(self.m_StoneBtn,1,IP_ENABLE, IP_CLICKDOWN)
    self.m_StoneBtn:SetMouseOverDescAfter("")
    self.m_StoneNum:SetWndText("")
    self.m_RefineEquipStoneInfo  = {}
end

function CEquipRefine:UpdateRefineEquipInfo()
    local equipType = self.m_RefineEquipInfo[1]
    local equipName = self.m_RefineEquipInfo[2]
    local equipID = self.m_RefineEquipInfo[3]
    
    local dynInfo = g_DynItemInfoMgr:GetDynItemInfo(equipID)
    dynInfo.IntensifyQuality = dynInfo.IntensifyQuality + 1
    local curAdvancePhase = dynInfo.CurAdvancePhase
    local advanceQuality =  dynInfo.MaxAdvancePhase
    local intensifyQuality =  dynInfo.IntensifyQuality
    
    local suc, needStoneCount = GetEquipRefineNeedStoneCount(curAdvancePhase, advanceQuality, intensifyQuality, equipType, equipName)
    if suc == false then
        if needStoneCount == 188005 then
            local str = TotalNumColor .. "-/-" 
            self.m_StoneNum:SetWndText(str)
            return
        end
        MessageBox(self, MsgBoxMsg(needStoneCount), MB_BtnOK)
        g_WndMouse:ClearCursorAll()
        return  
    end
    self:SetAdaptedStoneInfo(needStoneCount, intensifyQuality)
end