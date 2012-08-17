gac_require "activity/direct/DirectShowWndInc"
cfg_load "player/Direct_Common"
lan_load "player/Lan_Direct_Common"

function CreateDirectShowWnd(Parent)
	local wnd = CDirectShowWnd:new()
	wnd:CreateFromRes("DirectShowWnd",Parent)
	wnd:ShowWnd(true)
	return wnd
end 

function CDirectShowWnd:OnChildCreated()
	self.m_DirectBtnTbl = {}
	self.m_BgBtnTbl = {}
	for i = 1, 5 do
		local Btn = self:GetDlgChild("PictureBtn" .. i)
		Btn.m_HavePicture = false
		Btn.m_Index = i
		table.insert(self.m_DirectBtnTbl,Btn)
		Btn:ShowWnd(false)
		local bgBtn = self:GetDlgChild("Btn" .. i)
		bgBtn:ShowWnd(false)
		table.insert(self.m_BgBtnTbl,bgBtn)
	end
end

function CDirectShowWnd:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if uMsgID == BUTTON_LCLICK or uMsgID == BUTTON_DRAG then
		Child.m_HavePicture = false
		self.m_BgBtnTbl[Child.m_Index]:ShowWnd(false)
		Child:ShowWnd(false)
		--打开攻略窗口 显示到对应的项
		local DirectName = self.m_DirectBtnTbl[Child.m_Index].m_DirectName
		local nBigType, sActionType = g_DirectMgr:GetDirectType(DirectName)
		if not nBigType then
			g_GameMain.m_DirectWnd:OpenPanel(true)
			return
		end
		g_GameMain.m_DirectWnd:OpenPanel(true, nBigType, sActionType, DirectName)
	end 
end

function CDirectShowWnd:SetDirectBtnPicture(DirectName)
	for i = 1, 5 do
		if self.m_DirectBtnTbl[i].m_DirectName == DirectName then
			return
		end
	end
	
	local DirectIcon = Direct_Common(DirectName, "EnableIcon")
	
	for i = 1, 5 do
		if not self.m_DirectBtnTbl[i].m_HavePicture then
			self.m_DirectBtnTbl[i]:ShowWnd(true)
			self.m_BgBtnTbl[i]:ShowWnd(true)
			self.m_DirectBtnTbl[i].m_DirectName = DirectName
			self.m_DirectBtnTbl[i].m_HavePicture = true
			local next = (i == 5) and 1 or i + 1
			self.m_DirectBtnTbl[next].m_HavePicture = false
			g_LoadIconFromRes(DirectIcon, self.m_DirectBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
			self.m_DirectBtnTbl[i]:SetMouseOverDescAfter( Lan_Direct_Common( MemH64(DirectName), "DisplayName" ) )
			break
		end
	end
end

function CDirectShowWnd:CloseOneBtn(sDirectName)
	for i = 1, 5 do
		if(self.m_DirectBtnTbl[i].m_DirectName == sDirectName) then
			self.m_DirectBtnTbl[i].m_HavePicture = false
			self.m_DirectBtnTbl[i]:ShowWnd(false)
			self.m_BgBtnTbl[i]:ShowWnd(false)
		end
	end
end