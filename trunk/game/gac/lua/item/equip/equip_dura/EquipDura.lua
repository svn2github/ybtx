cfg_load "equip/EquipDuraStateShow_Client"
lan_load "equip/Lan_EquipDuraStateShow_Client"
CEquipDuraWnd = class (SQRDialog)

function CEquipDuraWnd:Ctor()
    self:CreateFromRes("EquipDuraWnd", g_GameMain)
    self:InitChild()
end

function CEquipDuraWnd:InitChild()
    self.ChildWndTbl = {}
    local wndStr = "Part"
    local wnd = nil
    for i=1, 5 do 
        wnd = self:GetDlgChild(wndStr .. i)
        self.ChildWndTbl[i] = wnd
    end
    for i=7, 11 do
        wnd = self:GetDlgChild(wndStr .. i)
        self.ChildWndTbl[i] = wnd
    end
    for i=14, 16 do
        wnd = self:GetDlgChild(wndStr .. i)
        self.ChildWndTbl[i] = wnd        
    end
    self.DemagedEquipTbl = {}
end


function CEquipDuraWnd:SetDuraDescription()
    local str = ""
    for i, v in pairs(self.DemagedEquipTbl) do
        str = str ..v .. "#r"
    end
    self:SetMouseOverDesc(str)
end

local function GetUpdateIconWnd(equipPart)
    local partTbl = g_GameMain.m_RoleStatus.Part
    local iconWnd = nil
    local colorWnd = nil
    for i, v in pairs(partTbl) do
        if equipPart == v.PartType then
            iconWnd = v.iconbtn
            colorWnd = v.IconColorBtn
        end
    end
    return iconWnd, colorWnd
end

function CEquipDuraWnd.RetEquipDuraValue(equipID, equipPart, duraState, curDuraValue, maxDuraValue)
    if curDuraValue >0 and curDuraValue < 1 then
        curDuraValue = 1
    else
        curDuraValue = string.format("%.0f", curDuraValue)
    end
    local nBigID, nIndex 
    if equipID ~= 0 then
        local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(equipID)
        if DynInfo == nil then
            LogErr("物品动态信息错误" .. "equipPart:" .. equipPart .. ",ID:" .. equipID)
        end
        nBigID, nIndex= DynInfo.m_nBigID,DynInfo.m_nIndex
        g_DynItemInfoMgr.m_tblGlobalItemInfo[equipID].MaxDuraValue = tonumber(maxDuraValue)
        g_DynItemInfoMgr.m_tblGlobalItemInfo[equipID].CurDuraValue = tonumber(curDuraValue)
    end
    if g_GameMain.m_RoleStatus ~= nil then
        if equipPart ~= 0 then
            local iconWnd, iconColorWnd = GetUpdateIconWnd(equipPart)
            if equipID ~= 0 then
            	g_SetItemRichToolTips(iconWnd,nBigID,nIndex,equipID)
            	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
            end
            local bWeaponVisibleFlag = true
            if duraState == 0 then
                bWeaponVisibleFlag = false
                iconColorWnd:ShowWnd(true)
                iconColorWnd:SetFocus()
            else
                iconColorWnd:ShowWnd(false)
                bWeaponVisibleFlag = true
            end
            if g_ItemInfoMgr:IsStaticWeapon(nBigID) then
                if  equipPart == EEquipPart.eWeapon then
                    g_MainPlayer:SetWeaponVisible(true, bWeaponVisibleFlag)
                elseif equipPart == EEquipPart.eAssociateWeapon then
                    g_MainPlayer:SetWeaponVisible(false, bWeaponVisibleFlag)
                end
            end
        end
    end
    local duraWnd = g_GameMain.m_EquipDuraWnd
    local duraStateInfo  = EquipDuraStateShow_Client(equipPart)
    local index = duraStateInfo("EquipPartIndex")
	local wnd = duraWnd.ChildWndTbl[index]
	
    local equipPartName = Lan_EquipDuraStateShow_Client(index,"DisplayEquipPart")
    if duraState < 2 then
        if  duraWnd:IsShow() == false then
            duraWnd:ShowWnd(true)
        end
        duraWnd.DemagedEquipTbl[equipPart] =  equipPartName .. ":" .. "#cff0000" .. curDuraValue ..  "#cffffff" .. "/"  ..  maxDuraValue
    else
        if duraWnd.DemagedEquipTbl[equipPart] ~= nil then
            duraWnd.DemagedEquipTbl[equipPart] = nil
        end
    end

    local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()

	local strTexture = ""
    if duraState == 0 then
        strTexture = duraStateInfo("InvalidateDura")
	    
    elseif duraState == 1 then
        strTexture = duraStateInfo("BadDura")
        
    elseif duraState == 2 then
        strTexture = duraStateInfo("NormalDura")
    end
    DefaultImage:AddImage(wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 0 ) 	
    wnd:SetWndBkImage( Flag, DefaultImage )
    duraWnd:SetDuraDescription()
end

function CEquipDuraWnd.RetShowEquipDuraWnd(bShowFlag)
    g_GameMain.m_EquipDuraWnd:ShowWnd(bShowFlag)
    g_GameMain.m_EquipDuraWnd.DemagedEquipTbl = {}
end


function CEquipDuraWnd.RetEquipDuraValueInBag(equipID, roomIndex, pos, curDuraValue, MaxDuraValue)
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(roomIndex,pos)
	local nBigID,nIndex = Grid:GetType()
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( roomIndex )
	local x, y = SrcCt:GetIndexByPos(pos, roomIndex)
	local vWnd = SrcCt:GetSubItem(y, x)
    g_DynItemInfoMgr.m_tblGlobalItemInfo[equipID].MaxDuraValue = MaxDuraValue
    g_DynItemInfoMgr.m_tblGlobalItemInfo[equipID].CurDuraValue = curDuraValue
    g_SetWndMultiToolTips(vWnd, nBigID, nIndex, equipID, 1)
end
