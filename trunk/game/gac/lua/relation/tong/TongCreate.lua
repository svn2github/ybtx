gac_require "relation/tong/TongCreateInc"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "relation/army_corps/ArmyCorpsMgr"

local TongType = 1
local ArmyCorpsType = 2

function CreateTongCreateWnd(Parent)
	local Wnd = CTongCreateWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongCreateWnd:Init(Parent)
	self:CreateFromRes("TongCreate",Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongCreateWnd:OnChildCreated()
	self.m_Money		= self:GetDlgChild("Money")
	self.m_InputArea	= self:GetDlgChild("InputArea")
	self.m_PurposeEdit	= self:GetDlgChild("PurposeEdit")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_CancelBtn	= self:GetDlgChild("CancelBtn")
	self.m_XBtn			= self:GetDlgChild("XBtn")
	
	self.m_Title 				= self:GetDlgChild("TongTitle")
	self.m_MoneyLable2 	= self:GetDlgChild("MoneyLable2")
	self.m_PromptText	 	= self:GetDlgChild("PromptText")
	self.m_PurposeLable = self:GetDlgChild("PurposeLable")
	self.m_MoneyLable1 = self:GetDlgChild("MoneyLable1")
	
	self.m_InputArea:SetMaxTextLenght(8)
	self.m_PurposeEdit:SetMaxTextLenght(512)
	self.m_CreateType = 0
end

function CTongCreateWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then 
			self:OnCreateTong()
		elseif(Child == self.m_CancelBtn or Child == self.m_XBtn) then
			self:ShowWnd(false)
		end
	end
end

function CTongCreateWnd:OpenPanel(CreateType)
	self.m_CreateType = CreateType
	if self.m_CreateType == nil then
		return
	end
	if self.m_CreateType == TongType then
		if( g_MainPlayer.m_Properties:GetTongID() ~= 0 ) then
			MsgClient(9009)
			return
		end
		if( g_MainPlayer:CppGetLevel() < g_TongMgr:GetRegistLevel() ) then
			MsgClient(9004)
			return
		end
		assert(g_MainPlayer:GetBindMoney())
		assert(g_TongMgr:GetRegistMoney())
		if( g_MainPlayer:GetBindMoney() < g_TongMgr:GetRegistMoney()) then
			MsgClient(9003)
			return
		end
		Gac2Gas:RequestCreateTong(g_Conn)
	elseif self.m_CreateType == ArmyCorpsType then
		if( g_MainPlayer:CppGetLevel() < g_ArmyCorpsMgr:GetRegistLevel() ) then
			MsgClient(8501)
			return
		end
		if( g_MainPlayer:GetMoney() < g_ArmyCorpsMgr:GetRegistMoney()) then
			MsgClient(8500)
			return
		end
		Gac2Gas:RequestCreateArmyCorps(g_Conn)
	end
end

function CTongCreateWnd:ChangeWndIInfo(Type)
	self.m_CreateType = Type
	if Type == TongType then
		self.m_Title:SetWndText(GetStaticTextClient(9301))
		self.m_PromptText:SetWndText(GetStaticTextClient(9302))
		self.m_Money:SetWndText(g_TongMgr:GetRegistMoney()/10000)
		self.m_MoneyLable2:SetWndText(GetStaticTextClient(9303))
		self.m_PurposeLable:SetWndText(GetStaticTextClient(9304))
		self.m_MoneyLable1:SetWndText(GetStaticTextClient(9310))
	elseif Type == ArmyCorpsType then
		self.m_Title:SetWndText(GetStaticTextClient(9305))
		self.m_PromptText:SetWndText(GetStaticTextClient(9306))
		self.m_Money:SetWndText(g_ArmyCorpsMgr:GetRegistMoney()/10000)
		self.m_MoneyLable2:SetWndText(GetStaticTextClient(9307))
		self.m_PurposeLable:SetWndText(GetStaticTextClient(9308))
		self.m_MoneyLable1:SetWndText(GetStaticTextClient(9311))
	end
	self.m_InputArea:SetWndText("")
	self.m_PurposeEdit:SetWndText("")
end

function CTongCreateWnd:OnCreateTong()
	local tongName = self.m_InputArea:GetWndText()
	local tongPurpose = self.m_PurposeEdit:GetWndText()
	local textFilter = CTextFilterMgr:new()
	
	if( not textFilter:IsValidName(tongName) ) then
		if self.m_CreateType == TongType then
			MsgClient(9198)
		else
			MsgClient(8540)
		end
		return
	end
	
	if( not textFilter:IsValidName(tongPurpose) ) then
		if self.m_CreateType == TongType then
			MsgClient(9199)
		else
			MsgClient(8541)
		end
		return
	end
	
	if("" == tongName) then
		if self.m_CreateType == TongType then
			MsgClient(9166)
		end
		return
	end
	if self.m_CreateType == TongType then
		Gac2Gas:CreateTong(g_Conn, tongName, tongPurpose)
	elseif self.m_CreateType == ArmyCorpsType then
		Gac2Gas:CreateArmyCorps(g_Conn, tongName, tongPurpose)
	end
end

---------------------------------------------------------------------------------------
function Gas2Gac:ReturnRequestCreateTong(Conn)
	local wnd = g_GameMain.m_TongCreate
	wnd:ChangeWndIInfo(TongType)
	wnd:ShowWnd(true) --显示帮会创建窗口
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
end

function Gas2Gac:ReturnRequestCreateArmyCorps(Conn)
	local wnd = g_GameMain.m_TongCreate
	wnd:ChangeWndIInfo(ArmyCorpsType)
	wnd:ShowWnd(true) --显示佣兵团创建窗口
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(wnd)
end