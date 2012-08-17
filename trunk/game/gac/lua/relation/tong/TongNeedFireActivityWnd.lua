cfg_load "tong/TongFirewoodExp_Common"

CTongNeedFireActivityWnd = class(SQRDialog)
CTongNeedFireActivityItemWnd = class(SQRDialog)

function CTongNeedFireActivityWnd:Ctor()
	self:CreateFromRes("TongNeedFireWnd",g_GameMain)
	self.m_StartBtn = self:GetDlgChild("StartBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_ExplainTxt = self:GetDlgChild("ExplainTxt")
	self.m_ExplainTxt:SetWndText(GetStaticTextClient(330005))
end

function CTongNeedFireActivityWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_StartBtn) then
			local uTongPos = g_GameMain.m_TongBase.m_TongPos
			if uTongPos == g_TongMgr.m_tblPosInfo["团长"] or uTongPos == g_TongMgr.m_tblPosInfo["副团长"] then
				Gac2Gas:OpenTongNeedFireActivity(g_Conn)
			else
				MsgClient(351000)
			end
		elseif(Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		end
	end
end


function CTongNeedFireActivityItemWnd:Ctor()
	self:CreateFromRes("TongNeedFireItemWnd",g_GameMain)
	self.m_NeedFireMsgWnd = self:GetDlgChild("NeedFireMsgWnd")
	self.m_NeedFireItemBtn = self:GetDlgChild("NeedFireItemBtn")
	self.m_AddNeedFireItemBtn = self:GetDlgChild("AddNeedFireItemBtn")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_AddNeedFireItemBtn:EnableWnd(false)
	self.m_FireWoodTbl = {}
	self.m_FireWoodItemWnd = {}
end

--@brief 给物品框上放置要燃烧的木材
function CTongNeedFireActivityItemWnd:SetNeedFireActivityItemInfo(context)
  local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
  local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )

	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nBigId, itemName = grid:GetType()
	local itemID = grid:GetItem():GetItemID()
	
	local itemType = tonumber(nBigId)
	if not TongFirewoodExp_Common(itemName) then
		MsgClient(351003)
		g_WndMouse:ClearCursorAll()
		return  
	end
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, itemName, "SmallIcon")
	g_DelWndImage(self.m_NeedFireItemBtn,1,IP_ENABLE, IP_CLICKDOWN)
	g_LoadIconFromRes(SmallIcon, self.m_NeedFireItemBtn, -1,  IP_ENABLE, IP_CLICKDOWN)
	g_SetWndMultiToolTips(self.m_NeedFireItemBtn, itemType, itemName, itemID, 1)    
	g_WndMouse:ClearCursorAll()
	if next(self.m_FireWoodItemWnd) then
		local x, y, fromRoom = unpack(self.m_FireWoodItemWnd)
		local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
		SrcCt:SetClickedWndState(x, y, fromRoom, true)
	end
	local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
	SrcCt:SetClickedWndState(x, y, fromRoom, false)
	self.m_FireWoodItemWnd = {x, y, fromRoom}
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemID)	
	local ColorStr = g_Tooltips:GetSuitNameColor(DynInfo,DynInfo.m_nBigID,DynInfo.m_nIndex)
	self.m_FireWoodTbl = {fromRoom, srcPos,itemName,ColorStr}
	self.m_AddNeedFireItemBtn:EnableWnd(true)
	
end


local function InitNeedFireItem()
	local wnd = g_GameMain.m_TongNeedFireActivityItemWnd
	wnd.m_FireWoodTbl = {}
	if next(wnd.m_FireWoodItemWnd) then
		local x, y, fromRoom = unpack(wnd.m_FireWoodItemWnd)
		local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
		SrcCt:SetClickedWndState(x, y, fromRoom, true)
	end
	g_DelWndImage(wnd.m_NeedFireItemBtn,1,IP_ENABLE, IP_CLICKDOWN)
	wnd.m_NeedFireItemBtn:SetMouseOverDescAfter("")
	wnd.m_AddNeedFireItemBtn:EnableWnd(false)
	wnd.m_NeedFireItemBtn:EnableWnd(false)
end

function CTongNeedFireActivityItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CloseBtn) then
			InitNeedFireItem()
			self.m_NeedFireItemBtn:EnableWnd(true)
			self:ShowWnd(false)
		elseif(Child == self.m_NeedFireItemBtn) then
			local state, context = g_WndMouse:GetCursorState()
			if state == ECursorState.eCS_PickupItem then 
				self:SetNeedFireActivityItemInfo(context)
			end
		elseif(Child == self.m_AddNeedFireItemBtn) then
			Gac2Gas:AddNeedFireItem(g_Conn,unpack(self.m_FireWoodTbl))
		end
	end
end

function CTongNeedFireActivityWnd.CheckNeedFireIsOpen(EntityID)
	if g_MainPlayer.m_Properties:GetTongID() == 0 then
		MsgClient(351000)
		return
	end
	Gac2Gas:CheckNeedFireIsOpen(g_Conn,EntityID)
end

local function ShowNeedFireActivityItemWnd()
	g_GameMain.m_TongNeedFireActivityWnd:ShowWnd(false)
	if not g_GameMain.m_TongNeedFireActivityItemWnd then
		g_GameMain.m_TongNeedFireActivityItemWnd = CTongNeedFireActivityItemWnd:new()
	end
	g_GameMain.m_TongNeedFireActivityItemWnd:ShowWnd(true) 
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TongNeedFireActivityItemWnd)
end

--------------------------------------【RPC】---------------------------------------
function CTongNeedFireActivityWnd.RetOpenTongNeedFireActivity(Conn,succ,msgId)
	if succ then
		ShowNeedFireActivityItemWnd()
	else
		if msgId == 351002 then
			ShowNeedFireActivityItemWnd()
		end
	end
end

local function ShowTimeTickFun()
	CTongNeedFireActivityItemWnd.InitLeftTimeShowFun()
end

function CTongNeedFireActivityWnd.RetCheckNeedFireIsOpen(Conn,succ,uLeftTime)
	if succ then
		if not g_GameMain.m_TongNeedFireActivityItemWnd then
			g_GameMain.m_TongNeedFireActivityItemWnd = CTongNeedFireActivityItemWnd:new()
		end
		if 0 ~= uLeftTime then
			if not g_GameMain.m_TongNeedFireActivityItemWnd.m_CoolDownTick then
				g_GameMain.m_TongNeedFireActivityItemWnd.m_AddNeedFireItemBtn:SetWndText(GetStaticTextClient(330007,uLeftTime))
				g_GameMain.m_TongNeedFireActivityItemWnd.m_CoolDownTime = uLeftTime
				g_GameMain.m_TongNeedFireActivityItemWnd.m_CoolDownTick = RegisterTick( "FirewoodCoolDownTick", ShowTimeTickFun, 1000)
			end
		end
		g_GameMain.m_TongNeedFireActivityItemWnd:ShowWnd(true) 
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TongNeedFireActivityItemWnd)
	else
		if not g_GameMain.m_TongNeedFireActivityWnd then
			g_GameMain.m_TongNeedFireActivityWnd = CTongNeedFireActivityWnd:new()
		end
		g_GameMain.m_TongNeedFireActivityWnd:ShowWnd(true) 
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_TongNeedFireActivityWnd)
	end
end

function CTongNeedFireActivityItemWnd.InitLeftTimeShowFun()
	local wnd = g_GameMain.m_TongNeedFireActivityItemWnd
	local left_time = wnd.m_CoolDownTime
	if left_time == 1 then
		wnd.m_AddNeedFireItemBtn:SetWndText(GetStaticTextClient(330006))
		wnd.m_NeedFireItemBtn:EnableWnd(true)
		if wnd.m_CoolDownTick then
			UnRegisterTick(wnd.m_CoolDownTick)
			wnd.m_CoolDownTick = nil
		end
		return
	end
	left_time = left_time - 1
	wnd.m_CoolDownTime = left_time
	wnd.m_AddNeedFireItemBtn:SetWndText(GetStaticTextClient(330007,left_time))
	wnd.m_AddNeedFireItemBtn:EnableWnd(false)
	wnd.m_NeedFireItemBtn:EnableWnd(false)
end

function CTongNeedFireActivityItemWnd.RetAddNeedFireItem(Conn,succ) 
	InitNeedFireItem()
	if succ then
		local wnd = g_GameMain.m_TongNeedFireActivityItemWnd
		if not wnd.m_CoolDownTick then
			wnd.m_CoolDownTime = 180
			wnd.m_CoolDownTick = RegisterTick( "FirewoodCoolDownTick", ShowTimeTickFun, 1000)
		end
	end
end

function CTongNeedFireActivityWnd.RetAddFirewoodMsg(Conn,memberName,sItemName,ColorStr)
	if not g_GameMain.m_TongNeedFireActivityItemWnd then
		g_GameMain.m_TongNeedFireActivityItemWnd = CTongNeedFireActivityItemWnd:new()
	end

	local sDisplayName = ColorStr .. g_ItemInfoMgr:GetItemLanInfo(1,sItemName,"DisplayName")
	local new_msg = "#W" .. GetStaticTextClient(330008,memberName,os.date("%H:%M:%S"),sDisplayName)
	local old_msg = g_GameMain.m_TongNeedFireActivityItemWnd.m_NeedFireMsgWnd:GetWndText()
	new_msg = new_msg .. old_msg 
	g_GameMain.m_TongNeedFireActivityItemWnd.m_NeedFireMsgWnd:SetWndText(new_msg)
end

function CTongNeedFireActivityWnd.RetNeedFireActivityEnd(Conn,uMsgId)
	local wnd = g_GameMain.m_TongNeedFireActivityItemWnd 
	if wnd then
		if wnd:IsShow() then
			InitNeedFireItem()
			wnd.m_NeedFireItemBtn:EnableWnd(true)
			wnd:ShowWnd(false)
			MsgClient(uMsgId)
		end
	end
end