CEquipInRoleWnd = class(SQRDialog)
CSingleInfoWnd = class(SQRDialog)

function CEquipInRoleWnd:Ctor()
	self:Init(g_GameMain.m_NewFixEquipWnd.m_Static)
end

function CEquipInRoleWnd:Init(parent)
	self:CreateFromRes("EquipInRoleWnd",parent)
end

--设置显示全部修理所需花费的窗口文本
local c_money = CMoney:new()
local function ShowTotalRequire(totalMoney)
	local shownText = c_money:ChangeMoneyToString(totalMoney)
	local str = GetStaticTextClient(1510)
	shownText = str .. shownText
	return shownText
end

--获取待修装备的相关信息
local nAllPartNum = 13
local function GetToFixInfo()
    local ReturnInfo = {}
    local totalNeedMoney = 0
    local numToRepair = 0
    local equipInfo = {}
    local k = 1
    for i = 1, nAllPartNum do
        if g_GameMain.m_RoleStatus.Part[i].m_Info ~= nil then
            local Info = g_GameMain.m_RoleStatus.Part[i].m_Info
            local eEquipPart = g_GameMain.m_RoleStatus.Part[i].PartType
			local nBigID,nIndex,nItemID = Info:GetBigID(),Info:GetIndex(),Info:GetItemID()
			local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
			if tonumber(DynInfo.CurDuraValue) < tonumber(DynInfo.MaxDuraValue) then
			    local needMoney = MoneyConsumedFixingEquip_Common(DynInfo.BaseLevel,"MoneyConsumed")
			    totalNeedMoney = totalNeedMoney + needMoney
			    numToRepair = numToRepair + 1
			   	equipInfo[k] = {nBigID,nIndex,nItemID,eEquipPart}
	    		k = k + 1
			end
	    end
	end
	ReturnInfo = {numToRepair,totalNeedMoney,equipInfo}
	return ReturnInfo
end

function CEquipInRoleWnd:OnChildCreated()
	self.m_RepairBtn = self:GetDlgChild("RepairBtn")
	self.m_ShowMoneyNeededWnd = self:GetDlgChild("ShowMoneyNeededWnd")
	self.m_ListItemWnd = self:GetDlgChild("ListItemWnd")
	local str = GetStaticTextClient(1513)
	self.m_RepairBtn:SetWndText(str)
	self.m_ListCtrlPos = 0
end

--用于第一次点击NPC初始化ListCtrl数据
function CEquipInRoleWnd:SetData()
	self.m_RepairBtn:EnableWnd(true)
	self.m_ListItemWnd:SetVerticalPos(0)
	self.toFixInfo = GetToFixInfo()
	local numToRepair,totalNeededMoney,toFixEquipInfoTbl = self.toFixInfo[1],self.toFixInfo[2],self.toFixInfo[3]
	self:InsertInfoWndFun(numToRepair,self.m_ListItemWnd,toFixEquipInfoTbl)
	local text = ShowTotalRequire(totalNeededMoney)
	self.m_ShowMoneyNeededWnd:SetWndText(text)
	if totalNeededMoney == 0 then
		self.m_RepairBtn:EnableWnd(false)
		local str = GetStaticTextClient(1516)
		self.m_RepairBtn:SetWndText(str)
	else
		local str = GetStaticTextClient(1513)
		self.m_RepairBtn:SetWndText(str)
	end
	self.m_InfoTbl = {totalNeededMoney}
end

--刷新ListCtrl数据
function CEquipInRoleWnd:Refresh()
	self:InsertInfoWndFun(self.toFixInfo[1],self.m_ListItemWnd,self.toFixInfo[3])
	local toFixInfo = GetToFixInfo()
	local totalNeededMoney = toFixInfo[2]
	local text = ShowTotalRequire(totalNeededMoney)
	self.m_ShowMoneyNeededWnd:SetWndText(text)
	if totalNeededMoney == 0 then
		self.m_RepairBtn:EnableWnd(false)
		local str = GetStaticTextClient(1515)
		self.m_RepairBtn:SetWndText(str)
	end
	self.m_ListItemWnd:SetVerticalPos(self.m_ListCtrlPos)
	self.m_InfoTbl = {totalNeededMoney}	
end

function CEquipInRoleWnd:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_RepairBtn then
			if g_MainPlayer.m_ItemBagLock then
                MsgClient(160045)
                return
            end
			local needMoney = self.m_InfoTbl[1]		
			local needMoneyStr = c_money:ChangeMoneyToString(needMoney)
			local curMoneyType = g_MainPlayer:GetMoneyType()
			local function callback(Context,Button)
                if Button == MB_BtnOK then
                    self:CheckMoneyEnoughForFixingEquip(curMoneyType,needMoney)
                end
                return true
            end
            self.m_MsgBox = MessageBox(self, MsgBoxMsg(1019, needMoneyStr), BitOr( MB_BtnOK, MB_BtnCancel), callback)
		end
	end
end
	

--创建显示列表中的元素窗口
function CEquipInRoleWnd:InsertInfoWndFun(totalNumToFix,listCtrl,equipInfoTbl)	
	local function CreateSingleInfoWnd(parent)
		local wnd = CSingleInfoWnd:new()
		wnd:CreateFromRes("SingleInfoWnd",parent)
		wnd:SetMouseWheel(true)
		wnd:ShowWnd(true)
		return wnd
	end
	
	local function ChildInit(wnd,tbl)
		wnd.m_ShowPicWnd = wnd:GetDlgChild("ShowPicWnd")
		wnd.m_ShowPicWnd:SetMouseWheel(true)
		wnd.m_ShowPicColor = wnd:GetDlgChild("ShowPicColor")
		wnd.m_ShowDurWnd = wnd:GetDlgChild("ShowDurWnd")
		wnd.m_ToFixBtn = wnd:GetDlgChild("ToFixBtn")
		wnd.m_ToFixBtn:SetMouseWheel(true)
		wnd.m_ShowMoneyNeededWnd = wnd:GetDlgChild("ShowMoneyNeededWnd")
		wnd.m_DescribeWnd = wnd:GetDlgChild("DescribeWnd")		
		wnd.m_ShowPicColor:SetParent(wnd.m_ShowPicWnd)
		wnd.m_ShowPicColor:ShowWnd(false)
		local str = GetStaticTextClient(1512)
		wnd.m_ToFixBtn:SetWndText(str)
		
		local nBigID = tbl[1]
		local nIndex = tbl[2]
		local nItemID = tbl[3]
		local eEquipPart = tbl[4]
		local iconID = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"SmallIcon")
		local toDescribe = g_ItemInfoMgr:GetItemLanInfo(nBigID,nIndex,"DisplayName")
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		local DisplayColor = g_Tooltips:GetSuitNameColor(DynInfo,nBigID,nIndex)
		toDescribe = DisplayColor .. toDescribe
		local duraStr = "(" .. DynInfo.CurDuraValue .. "/" .. DynInfo.MaxDuraValue .. ")"
		if DynInfo.CurDuraValue == DynInfo.MaxDuraValue then
			wnd.needMoney = 0
		else
			wnd.needMoney = MoneyConsumedFixingEquip_Common(DynInfo.BaseLevel,"MoneyConsumed")
		end		
		if DynInfo.CurDuraValue == 0 then
			wnd.m_ShowPicColor:ShowWnd(true)
		end
		if wnd.needMoney == 0 then
			local str = GetStaticTextClient(1514)
			wnd.m_ToFixBtn:EnableWnd(false)
			wnd.m_ToFixBtn:SetWndText(str)
		end
		wnd.needMoney = c_money:ChangeMoneyToString(wnd.needMoney)
		local needMoneyStr = GetStaticTextClient(1511) .. wnd.needMoney
		wnd.m_ShowMoneyNeededWnd:SetWndText(needMoneyStr)
		g_LoadIconFromRes(iconID, wnd.m_ShowPicWnd, -1,  IP_ENABLE, IP_ENABLE)

		wnd.m_ShowDurWnd:SetWndText(duraStr)
		wnd.m_DescribeWnd:SetWndText(toDescribe)
		g_SetItemRichToolTips(wnd.m_ShowPicWnd,nBigID, nIndex, nItemID)
		
		wnd.m_InfoTbl = {DynInfo,eEquipPart,nItemID}
	end


	listCtrl:DeleteAllItem()
	local itemCount
	listCtrl:InsertColumn(0, listCtrl:GetWndOrgWidth())
	for itemCount = 0 ,totalNumToFix - 1 do
		listCtrl:InsertItem(itemCount,60)
		local Item = listCtrl:GetSubItem(itemCount,0)
		Item:SetMouseWheel(true)
		local infoWnd = CreateSingleInfoWnd(Item)
		Item.m_Wnd = infoWnd
		temTbl = equipInfoTbl[itemCount + 1]
		ChildInit(Item.m_Wnd,temTbl)		
	end
end

--维修装备主功能函数
function CEquipInRoleWnd:CheckMoneyEnoughForFixingEquip(curMoneyType,needMoney)
	local function callback(Context,Button)
		Context.tembox = nil
		return true
	end
	if needMoney > g_MainPlayer.m_nMoney + g_MainPlayer.m_nBindMoney then
		self.tempbox = MessageBox(g_GameMain, MsgBoxMsg(1018), MB_BtnOK,callback,self)
	else
		Gac2Gas:RenewEquipOnWearingDuraValue(g_Conn, curMoneyType)
	end
end

function CSingleInfoWnd:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if Child == self.m_ToFixBtn and uMsgID == BUTTON_LCLICK then
		local function callback(Context,Button)
			if Button == MB_BtnOK then
                if g_MainPlayer.m_ItemBagLock then
                    MsgClient(160045)
                    return
                end
   				g_GameMain.m_RoleStatus:CheckMoneyEnoughForFixingEquip(self.m_InfoTbl[1].BaseLevel, self.m_InfoTbl[2], self.m_InfoTbl[3])
   				g_GameMain.m_CEquipInRoleWnd.m_ListCtrlPos = g_GameMain.m_CEquipInRoleWnd.m_ListItemWnd:GetVerticalPos()
   				g_GameMain.m_CEquipInBagWnd.m_ListCtrlPos = g_GameMain.m_CEquipInBagWnd.m_ListItemWnd:GetVerticalPos()
			end
			self.m_MsgBox = nil
			return true
		end		
      self.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(1019, self.needMoney), BitOr( MB_BtnOK, MB_BtnCancel), callback)
	end
end

function CEquipInRoleWnd.RetRenewEquipSuc(equipPart, equipType, equipName, equipID)
    if equipPart ~= 0 then
        if g_ItemInfoMgr:IsStaticWeapon(equipType) then
            if  equipPart == EEquipPart.eWeapon then
                g_MainPlayer:SetWeaponVisible(true, true)
            elseif equipPart == EEquipPart.eAssociateWeapon then
                g_MainPlayer:SetWeaponVisible(false, true)
            end
        end    
    end
end

--用于服务端完成操作后刷新客户端窗口
function CEquipInRoleWnd.RefreshListCtrlInRole()
	g_GameMain.m_CEquipInRoleWnd:Refresh()
end




