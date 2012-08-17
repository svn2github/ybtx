CEquipInBagWnd = class(SQRDialog)
	
function CEquipInBagWnd:Ctor()
	self:Init(g_GameMain.m_NewFixEquipWnd.m_Static)
end

function CEquipInBagWnd:Init(parent)
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

--获取背包内待修装备的相关信息
local function GetToFixInfo()
    local SpaceMgr	= g_GameMain.m_BagSpaceMgr
	local space		= SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local BagSize	= space:Size()
	local moneyneedToFixEquip = 0
	local needLockedPos = {}
	local equipInfo = {}
	local k = 1
	local ReturnInfo = {}
	local numToRepair = 0
	for i = 1, BagSize do
		local nRoom, srcPos 
		if(i > g_PlayerMainBag.size) then  
			nRoom, srcPos = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetSlotIndex(i)
		else
			nRoom = g_StoreRoomIndex.PlayerBag
			srcPos = i
		end
		local CurGrid = SpaceMgr:GetGridByRoomPos(nRoom, srcPos)
		local nBigID, nIndex = CurGrid:GetType()
		if (not CurGrid:Empty()) then
		    if tonumber(nBigID) >= 5 and tonumber(nBigID) <= 9  then
--		    	local eEquipPart = g_GameMain.m_RoleStatus.Part[i].PartType
    			local GlobalID = CurGrid:GetItem():GetItemID()
    			local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(GlobalID)
    			if not DynInfo then	
					local ItemId = GlobalID or 0
					LogErr("物品动态信息错误" .. "sBigID:" .. nBigID .. "sIndex:" .. nIndex  .. "ID:" .. ItemId)
					return
				end
    			if tonumber(DynInfo.CurDuraValue) < tonumber(DynInfo.MaxDuraValue) then
    			    table.insert(needLockedPos, {nRoom, srcPos})
    			    local needMoney = MoneyConsumedFixingEquip_Common(DynInfo.BaseLevel,"MoneyConsumed")
    			    moneyneedToFixEquip = moneyneedToFixEquip + needMoney
    			    numToRepair = numToRepair + 1
    			   	equipInfo[k] = {nBigID,nIndex,GlobalID}
    			    k = k + 1
    			end
    	  	end
		end
	end
	ReturnInfo = {numToRepair,moneyneedToFixEquip,needLockedPos,equipInfo}
	return ReturnInfo
end

function CEquipInBagWnd:OnChildCreated()
	self.m_ShowMoneyNeededWnd = self:GetDlgChild("ShowMoneyNeededWnd")
	self.m_RepairBtn = self:GetDlgChild("RepairBtn")
	self.m_ListItemWnd = self:GetDlgChild("ListItemWnd")
	local str = GetStaticTextClient(1513)
	self.m_RepairBtn:SetWndText(str)
	self.m_ListCtrlPos = 0
end

--刷新窗口
function CEquipInBagWnd:SetData()
	self.m_RepairBtn:EnableWnd(true)
	self.m_ListItemWnd:SetVerticalPos(0)
	self.toFixInfo = GetToFixInfo()
	local numToRepair,totalNeededMoney,toFixEquipInfoTbl = self.toFixInfo[1],self.toFixInfo[2],self.toFixInfo[4]
	self.m_needLockedPos = self.toFixInfo[3]
	g_GameMain.m_CEquipInRoleWnd:InsertInfoWndFun(numToRepair,self.m_ListItemWnd,toFixEquipInfoTbl)
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
function CEquipInBagWnd:Refresh()
	g_GameMain.m_CEquipInRoleWnd:InsertInfoWndFun(self.toFixInfo[1],self.m_ListItemWnd,self.toFixInfo[4])
	local toFixInfo = GetToFixInfo()
	local totalNeededMoney = toFixInfo[2]
	local text = ShowTotalRequire(totalNeededMoney)
	self.m_ShowMoneyNeededWnd:SetWndText(text)
	if totalNeededMoney == 0 then
		self.m_RepairBtn:EnableWnd(false)
		local str = GetStaticTextClient(1515)
		self.m_RepairBtn:SetWndText(str)
	else
		local str = GetStaticTextClient(1513)
		self.m_RepairBtn:SetWndText(str)
	end
	self.m_ListItemWnd:SetVerticalPos(self.m_ListCtrlPos)
	self.m_InfoTbl = {totalNeededMoney}	
end

function CEquipInBagWnd:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if uMsgID == BUTTON_LCLICK and Child == self.m_RepairBtn then
		if g_MainPlayer.m_ItemBagLock then
        	MsgClient(160045)
            return
        end
		local needMoney = self.m_InfoTbl[1]		
		local needMoneyStr = c_money:ChangeMoneyToString(needMoney)
		local curMoneyType = g_MainPlayer:GetMoneyType()
		local function callback(Context,Button)
        	if Button == MB_BtnOK then
        		self:CheckMoneyEnoughForFixingEquip(curMoneyType, needMoney)
       		end
        	return true
        end
        self.m_MsgBox = MessageBox(self, MsgBoxMsg(1019, needMoneyStr), BitOr( MB_BtnOK, MB_BtnCancel), callback)
	end
end

--维修装备主功能函数
function CEquipInBagWnd:CheckMoneyEnoughForFixingEquip(curMoneyType, needMoney)
	local function callback(Context,Button)
		Context.m_MsgBox = nil
		return true
	end
	if needMoney > g_MainPlayer.m_nMoney + g_MainPlayer.m_nBindMoney then
		self.m_MsgBox = MessageBox(self, MsgBoxMsg(1018), MB_BtnOK,callback,self)
	else
		Gac2Gas:RenewEquipInBagDuraValue(g_Conn, curMoneyType)
	end
end

function CEquipInBagWnd:LockOrOnLockPosInBag(bFlag)
    if self.m_needLockedPos == nil then
        return
    end 
    for i, v in pairs(self.m_needLockedPos) do
        local roomIndex = v[1]
        local pos = v[2]
        local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( roomIndex )
        local y, x = SrcCt:GetIndexByPos(pos, roomIndex)
	    SrcCt:SetClickedWndState(x, y, roomIndex, bFlag)
    end
end

function CEquipInBagWnd.RefreshListCtrlInBag()
	g_GameMain.m_CEquipInBagWnd:Refresh()
end

function CEquipInBagWnd.RenewAllEquipEnd()
    g_GameMain.m_CEquipInBagWnd:LockOrOnLockPosInBag(true)
    g_GameMain.m_CEquipInBagWnd.m_needLockedPos = nil
end
