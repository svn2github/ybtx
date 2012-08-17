cfg_load "appellation/AppellationText_Common"
cfg_load "appellation/AppellationIdAndText_Common"
lan_load "appellation/Lan_AppellationText_Common"

CAppellationAndMoralWnd = class( SQRDialog )

function CAppellationAndMoralWnd:Ctor()
	self:Init()
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CAppellationAndMoralWnd:Init()
	self.m_tblAppellationCombox1	= {[0] = ""}
	self.m_tblAppellationCombox2	= {[0] = ""}
	self:CreateFromRes("AppellationAndMoralWnd",g_GameMain)
end

function CAppellationAndMoralWnd:OnChildCreated()
	self.m_TitleWnd									= self:GetDlgChild("TitleWnd")
	self.m_CurrentAppellation 						= self:GetDlgChild("CurrentAppellation")
	self.m_CurrentAppellationEditWnd 				= self:GetDlgChild("CurrentAppellationEditWnd")
	self.m_CheckBtn1								= self:GetDlgChild("CheckBtn1")
	self.m_UseAppellationEdit						= self:GetDlgChild("UseAppellationEdit")
	self.m_AppellationNameList						= self:GetDlgChild("AppellationNameList")
	self.m_CheckBtn2								= self:GetDlgChild("CheckBtn2")
	self.m_UseAssembledAppellation					= self:GetDlgChild("UseAssembledAppellation")
	self.m_AppellationCombox1						= self:GetDlgChild("AppellationCombox1")
	self.m_AppellationCombox2						= self:GetDlgChild("AppellationCombox2")
	self.m_CheckBtn3								= self:GetDlgChild("CheckBtn3")
	self.m_NonuseAppellation						= self:GetDlgChild("NonuseAppellation")
	self.m_CloseBtn									= self:GetDlgChild("CloseBtn")
	self.m_UseAppellationBtn						= self:GetDlgChild("UseAppellationBtn")
	self.m_UseMoralBtn								= self:GetDlgChild("UseMoralBtn")
	self.m_CancelBtn								= self:GetDlgChild("CancelBtn")
	self.m_CurrentMoraWnd							= self:GetDlgChild("CurrentMoraWnd")
	self.m_MoraList									= self:GetDlgChild("MoraList")
	self.m_CurrentMoraTitle							= self:GetDlgChild("CurrentMoraTitle")
	self.m_UseMoralBtn:ShowWnd(false)
	self.m_MoraList:ShowWnd(false)
	self.m_CurrentMoraWnd:ShowWnd(false)
	self.m_CurrentMoraTitle:ShowWnd(false)
	self.m_CheckBtn1:SetCheck(false)
	self.m_CheckBtn2:SetCheck(false)
	self.m_CheckBtn3:SetCheck(true)
	self.m_CurrentAppellationEditWnd:SetWndText("")
	self.m_AppellationNameList:EnableWnd(false)
	self.m_AppellationCombox1:EnableWnd(false)
	self.m_AppellationCombox2:EnableWnd(false)
	self.m_AppellationCombox1.m_uAppellationIdTbl = {}
	self.m_AppellationCombox2.m_uAppellationIdTbl = {}
	self:InitListWnd()
end

function CAppellationAndMoralWnd:UseAppellation()
	local appellationId1 = self.m_CurrentAppellationId1 or 0
	local appellationId2 = self.m_CurrentAppellationId2 or 0
	Gac2Gas:SavePlayerAppellation(g_Conn,appellationId1,appellationId2)
	self:ShowWnd(false)
end

function CAppellationAndMoralWnd:UseMoral()
	
end

local title = ""
local appellationText,appellationText1,appellationText2 = "","",""
local flag
function CAppellationAndMoralWnd:OnCtrlmsg(Child,uMsgID)
	if (uMsgID == BUTTON_LCLICK ) then
		if (Child == self.m_CloseBtn or Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_UseAppellationBtn) then
			self:UseAppellation()
		elseif (Child == self.m_UseMoralBtn) then
			self:UseMoral()
		elseif (Child == self.m_CheckBtn1 or Child == self.m_UseAppellationEdit) then
			self:SetCheck(self.m_CheckBtn1)
		elseif (Child == self.m_CheckBtn2 or Child == self.m_UseAssembledAppellation) then
			self:SetCheck(self.m_CheckBtn2)
		elseif (Child == self.m_CheckBtn3 or Child == self.m_NonuseAppellation) then
			self:SetCheck(self.m_CheckBtn3)
			self.m_CurrentAppellationId1 = 0
			self.m_CurrentAppellationId2 = 0
		end
	elseif uMsgID == ITEM_LBUTTONCLICK then
		if(Child == self.m_AppellationNameList) then
			local chooseItemIndex = self.m_AppellationNameList:GetSelectItem( -1 )
			local chooseItem = self.m_AppellationNameList:GetSubItem(chooseItemIndex, 0)
			local itemText = chooseItem:GetWndText()
			title = itemText
			flag = 1
			self.m_CurrentAppellationId1 = chooseItem.m_uAppellationId
			self.m_CurrentAppellationId2 = 0
		elseif (Child == self.m_AppellationCombox1) then
			appellationText1	= self.m_AppellationCombox1:GetSelText()
			flag = 2
			self.m_CurrentAppellationId1 = self.m_AppellationCombox1.m_uAppellationIdTbl[self.m_AppellationCombox1:GetCurrentSel()]
		elseif (Child == self.m_AppellationCombox2) then
			appellationText2	= self.m_AppellationCombox2:GetSelText()
			flag = 2
			self.m_CurrentAppellationId2 = self.m_AppellationCombox2.m_uAppellationIdTbl[self.m_AppellationCombox2:GetCurrentSel()]
		end
		appellationText = appellationText1 .. appellationText2
		if flag == 1 and title ~= "" then
			self.m_CurrentAppellationEditWnd:SetWndText(title)
		elseif flag == 2 and appellationText ~= ""   then
			self.m_CurrentAppellationEditWnd:SetWndText(appellationText)
		end
		appellationText = ""
	end
end

function CAppellationAndMoralWnd:SetCheck(wnd)
	if wnd == self.m_CheckBtn1 then
		wnd:SetCheck(true)
		self.m_CheckBtn2:SetCheck(false)
		self.m_CheckBtn3:SetCheck(false)
	elseif wnd == self.m_CheckBtn2 then
		wnd:SetCheck(true)
		self.m_CheckBtn1:SetCheck(false)
		self.m_CheckBtn3:SetCheck(false)
	elseif wnd == self.m_CheckBtn3 then
		wnd:SetCheck(true)
		self.m_CurrentAppellationEditWnd:SetWndText("")
		self.m_CheckBtn1:SetCheck(false)
		self.m_CheckBtn2:SetCheck(false)
	end
	local b_CheckBtn1 = self.m_CheckBtn1:GetCheck()
	local b_CheckBtn2 = self.m_CheckBtn2:GetCheck()
	local b_CheckBtn3 = self.m_CheckBtn3:GetCheck()
	
	self.m_AppellationNameList:EnableWnd(b_CheckBtn1)
	self.m_AppellationCombox1:EnableWnd(b_CheckBtn2)
	self.m_AppellationCombox2:EnableWnd(b_CheckBtn2)
end

local function GetWndWidth(ChildWnd)
	local rt = CFRect:new()
	ChildWnd:GetLogicRect(rt)
	local width = rt.right-rt.left
	return width
end

function CAppellationAndMoralWnd:InitListWnd()
	local Width = GetWndWidth(self.m_AppellationNameList)
	self.m_AppellationNameList:InsertColumn(0,Width)
end

function CAppellationAndMoralWnd:InitCombox(wnd)
	local Width = GetWndWidth(wnd)
	wnd:InsertColumn(0,Width)
end

function CAppellationAndMoralWnd:InsertItemList(ItemNum, ColNum,ItemTitle,AppellationId)
	local Item = self.m_AppellationNameList:GetSubItem(ItemNum,ColNum)
	Item.m_uAppellationId = AppellationId
	Item:SetWndText(ItemTitle)
end

function CAppellationAndMoralWnd:InitAppellationShow(param,addType)
	local AppellationText,AppellationType
	local AppellationId = 0
	if IsNumber(param) then	
		AppellationId = param
		param = AppellationIdAndText_Common(param,"Content")
	else
		AppellationId = AppellationText_Common(param,"ID")
	end
	AppellationType = AppellationText_Common(param,"Type")
	if nil == AppellationType then
		return
	end
	AppellationText = Lan_AppellationText_Common(MemH64(param),"Content") 
	
	if AppellationType == 1 then
		local itemcount = self.m_AppellationNameList:GetItemCount()
		self.m_AppellationNameList:InsertItem(itemcount,20)
		self:InsertItemList(itemcount, 0,AppellationText,AppellationId)
	elseif AppellationType == 2 then
		local itemcount = self.m_AppellationCombox1:GetItemCount()
		self.m_AppellationCombox1:InsertItem(AppellationText, itemcount, -1)
		self.m_AppellationCombox1.m_uAppellationIdTbl[itemcount] = AppellationId
	elseif AppellationType == 3 then
		local itemcount = self.m_AppellationCombox2:GetItemCount()
		self.m_AppellationCombox2:InsertItem(AppellationText, itemcount, -1)
		self.m_AppellationCombox2.m_uAppellationIdTbl[itemcount] = AppellationId
	end
end
------------------------------------------RPC--------------------------------------------------------

function CAppellationAndMoralWnd.ReturnAddNewAppellation(Conn,appellationIndex)
	if appellationIndex == 0 then
		g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationEditWnd:SetWndText("")
		g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit()
	end
	g_GameMain.m_AppellationAndMoralWnd:InitAppellationShow(appellationIndex,1)
end

function CAppellationAndMoralWnd.ReturnDelAppellationBegin(Conn)
	g_GameMain.m_AppellationAndMoralWnd.m_AppellationNameList:DeleteAllItem()
	g_GameMain.m_AppellationAndMoralWnd:InitListWnd()
	g_GameMain.m_AppellationAndMoralWnd.m_AppellationCombox1:RemoveAllItem()
	g_GameMain.m_AppellationAndMoralWnd:InitCombox(g_GameMain.m_AppellationAndMoralWnd.m_AppellationCombox1)
	g_GameMain.m_AppellationAndMoralWnd.m_AppellationCombox2:RemoveAllItem()
	g_GameMain.m_AppellationAndMoralWnd:InitCombox(g_GameMain.m_AppellationAndMoralWnd.m_AppellationCombox2)
end

function CAppellationAndMoralWnd.ReturnPlayerAppellationInfo(Conn)
	g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationId1 = g_MainPlayer.m_Properties:GetCharAppellation1()
	g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationId2 = g_MainPlayer.m_Properties:GetCharAppellation2()
	local appellationWndText,appellationWndText1,appellationWndText2 = "","",""
	if g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationId1 ~= 0 then
		appellationWndText1 = AppellationIdAndText_Common(g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationId1,"Content")
		appellationWndText1 = Lan_AppellationText_Common(MemH64(appellationWndText1),"Content")
	end
	if g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationId2 ~= 0 then
		appellationWndText2 = AppellationIdAndText_Common(g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationId2,"Content")
		appellationWndText2 = Lan_AppellationText_Common(MemH64(appellationWndText2),"Content")
	end
	Lan_AppellationText_Common(MemH64(param),"Content")
	appellationWndText = appellationWndText1 .. appellationWndText2
	g_GameMain.m_AppellationAndMoralWnd.m_CurrentAppellationEditWnd:SetWndText(appellationWndText)
	g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit()
end
