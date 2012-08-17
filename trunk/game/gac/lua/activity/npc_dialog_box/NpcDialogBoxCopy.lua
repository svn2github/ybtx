gac_require "activity/npc_dialog_box/NpcDialogBoxCopyInc"
gac_gas_require "framework/common/CMoney"
cfg_load "liveskill/LiveSkillName_Client"
lan_load "liveskill/Lan_LiveSkillName_Client"
lan_load "liveskill/Lan_BasicLiveskill_Common"

function CreateNpcDlgCopy(partent)
	local wnd = CNpcDialogBoxCopy:new()
	wnd:Init(partent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:InitFunLink()
	return wnd
end

function CNpcDialogBoxCopy:Init(partent)
	self.m_sType = ""
	self:CreateFromRes("LiveSkillLearn", partent)
end

function CNpcDialogBoxCopy:OnChildCreated()
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_Icon			= self:GetDlgChild("Icon")
	self.m_SkillName	= self:GetDlgChild("SkillName")
	self.m_NeedLevel	= self:GetDlgChild("NeedLevel")
	self.m_CurSkill		= self:GetDlgChild("CurSkill")
	self.m_EXPProgress	= self:GetDlgChild("EXPProgress")
	self.m_Description	= self:GetDlgChild("Description")
	self.m_Money		= self:GetDlgChild("Money")
	self.m_UnLearn		= self:GetDlgChild("UnLearn")
	
	self.m_tblBtn		= {}
	for i = 1, 3 do
		self.m_tblBtn[i] = self:GetDlgChild("Btn" .. i)
	end
end

function CNpcDialogBoxCopy:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_XBtn) then
			self:ShowWnd(false)
		else
			for i, v in ipairs(self.m_tblBtn) do
				if(Child == v) then
					v.Func(self)
					self:ShowWnd(false)
				end
			end
		end
	end
end

function CNpcDialogBoxCopy:GetFuncIndex(sType, bFlag)
	local tbl = {}
	for i, v in ipairs(self.ENpcShowDlg[sType]) do
		if(v[2] == bFlag) then
			table.insert(tbl, i)
		end
	end
	return tbl
end

function CNpcDialogBoxCopy:SetPanelInfo(tbl)
	self:UpdateInfo()
	for i, v in ipairs(self.m_tblBtn) do
		local bFlag = tbl[i] and true or false
		if(bFlag) then
			v.Func = self.linkfun[tbl[i]]
			v:SetWndText( self.ENpcShowDlg[self.m_sType][tbl[i]][1] )
		end
		v:ShowWnd(bFlag)
	end
end

function CNpcDialogBoxCopy:OpenPanel(sType, bFlag)
	self.m_sType = sType
	local tbl = self:GetFuncIndex(sType, bFlag)
	self:SetPanelInfo(tbl)
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CNpcDialogBoxCopy:UpdateInfo()
	local skillInfoNPC		= g_GameMain.m_LiveSkillMainWnd:GetSkillInfoByName(self.m_sType)
	local nSmallIconNPC		= LiveSkillName_Client(self.m_sType, "SmallIcon")
	local sDisplayNameNPC	= Lan_LiveSkillName_Client(MemH64(self.m_sType), "DisplayName")
	local bFlag				= skillInfoNPC and true or false
	if(bFlag) then --已经学习了该技能
		local nLevel			= skillInfoNPC[2]
		local nEXP				= skillInfoNPC[3]
		local nextLevelExpNeed	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, nLevel + 1, "ExperienceNeed")
		if(nextLevelExpNeed) then --未到达顶级
			local curLevelExpNeed	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, nLevel, "ExperienceNeed")
			local nextNeedBaseLevel	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, nLevel + 1, "BaseLevel")
			local nNeedMoney		= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, nLevel + 1, "NeedMoney")
			self.m_EXPProgress:SetRange(nextLevelExpNeed - curLevelExpNeed)
			self.m_EXPProgress:SetPos(nEXP - curLevelExpNeed)
			self.m_EXPProgress:SetWndText(nEXP .. "/" .. nextLevelExpNeed)
			self.m_SkillName:SetWndText( string.format("%s(%d)", sDisplayNameNPC, nLevel + 1) )
			self.m_NeedLevel:SetWndText(GetStaticTextClient(3042) .. nextNeedBaseLevel)
			self.m_Money:SetWndText(g_MoneyMgr:ChangeMoneyToString(nNeedMoney))
			self.m_Description:SetWndText(Lan_BasicLiveskill_Common(MemH64(self.m_sType .. nLevel + 1), "Description"))
		else
			self.m_EXPProgress:SetRange(1)
			self.m_EXPProgress:SetPos(1)
			self.m_EXPProgress:SetWndText(nEXP .. "/" .. nEXP)
			self.m_SkillName:SetWndText( string.format("%s(%d)", sDisplayNameNPC, nLevel) )
			self.m_NeedLevel:SetWndText(GetStaticTextClient(3043))
			self.m_Money:SetWndText("---")
			self.m_Description:SetWndText(Lan_BasicLiveskill_Common(MemH64(self.m_sType .. nLevel), "Description"))
		end
	else
		local nextNeedBaseLevel	= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, 1, "BaseLevel")
		local nNeedMoney		= g_LiveSkillMgr:GetSkillInfoByName(self.m_sType, 1, "NeedMoney")
		self.m_SkillName:SetWndText( string.format("%s(%d)", sDisplayNameNPC, 1) )
		self.m_NeedLevel:SetWndText(GetStaticTextClient(3042) .. nextNeedBaseLevel)
		self.m_Money:SetWndText(g_MoneyMgr:ChangeMoneyToString(nNeedMoney))
		self.m_Description:SetWndText(Lan_BasicLiveskill_Common(MemH64(self.m_sType .. 1), "Description"))
	end
	
	self:SetCurProdSkill()
	self.m_EXPProgress:ShowWnd(bFlag)
	self.m_UnLearn:ShowWnd(not bFlag)
	g_LoadIconFromRes( nSmallIconNPC, self.m_Icon, -1, IP_ENABLE, IP_ENABLE )
end

function CNpcDialogBoxCopy:SetCurProdSkill()
	local sType		= g_GameMain.m_LiveSkillProdMain.m_sType
	local curDisplayName = "" == sType and GetStaticTextClient(3044) or string.format("%s(%d)", Lan_LiveSkillName_Client(MemH64(sType), "DisplayName"), g_GameMain.m_LiveSkillProdMain.m_nLevel)
	self.m_CurSkill:SetWndText(curDisplayName)
end

local function LearnOrWashCallBack(LearnFunc, button)
	if( button == MB_BtnOK ) then
		LearnFunc()
	end
	g_GameMain.m_MsgBox = nil
	return true
end
--***************************************************************************************************
--
--***************************************************************************************************
function CNpcDialogBoxCopy:Learn()
	local CallBack	= LearnOrWashCallBack
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160035)
		return 
	end
	local param		= function()
		Gac2Gas:LearnLiveSkill(g_Conn, self.m_sType)
	end
	local nMsgID = self.DlgTextType[self.m_sType][1]
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(nMsgID), BitOr( MB_BtnOK, MB_BtnCancel), CallBack, param)
end

function CNpcDialogBoxCopy:Upgrade()
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160035)
		return 
	end
	Gac2Gas:LearnLiveSkill(g_Conn, self.m_sType)
end

function CNpcDialogBoxCopy:Wash()
	if g_MainPlayer.m_ItemBagLock then
			MsgClient(160037)
			return 
	end
	local CallBack	= LearnOrWashCallBack
	local param		= function()
		g_GameMain.RetWashLiveSkill = function()
			if( g_GameMain.m_LiveSkillProdMain and g_GameMain.m_LiveSkillProdMain:IsShow() ) then
				g_GameMain.m_LiveSkillProdMain:ChangePageTo(1)
				g_GameMain.m_LiveSkillProdMain:OpenWnd(false)
			end
		end
		Gac2Gas:WashLiveSkill(g_Conn, self.m_sType)
	end
	local nMsgID = self.DlgTextType[self.m_sType][2]
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(nMsgID), BitOr( MB_BtnOK, MB_BtnCancel), CallBack, param)
end

function CNpcDialogBoxCopy:OpenExpert()
	g_GameMain.m_LiveSkillExpertNPC:OpenWnd(self.m_sType)
end