CDaDuoBaoWnd = class(SQRDialog)

function CDaDuoBaoWnd:Ctor(parent)
	self:CreateFromRes("Fb_DaDuoBaoWnd",parent) 
	self.m_CloseBtn = self:GetDlgChild("Close")
	
	self.m_JoinBtn = {}
	for i=1,4 do
		local strWnd = "Btn"..i
		self.m_JoinBtn[i] = self:GetDlgChild(strWnd)
	end
	
	--Wnd.m_FbGameInfo:SetWndText(Description_Client["积分赛介绍"].Content)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CDaDuoBaoWnd:GetWnd()
	local Wnd = g_GameMain.m_DaDuoBaoWnd
	if not Wnd then
		Wnd = CDaDuoBaoWnd:new(g_GameMain)
		g_GameMain.m_DaDuoBaoWnd = Wnd
	end
	return Wnd
end

function CDaDuoBaoWnd:InitWindow()
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

local function IsSameLevel(LevelType)
	if LevelType and LevelType >= 1 and LevelType <= 4 then

		local PlayerLevel = g_MainPlayer:CppGetLevel()
		if LevelType == 1 then
			if PlayerLevel >= 10 and PlayerLevel <= 20 then
				return true
			end
		elseif LevelType == 2 then
			if PlayerLevel >= 21 and PlayerLevel <= 30 then
				return true
			end
		elseif LevelType == 3 then
			if PlayerLevel >= 31 and PlayerLevel <= 40 then
				return true
			end
		elseif LevelType == 4 then
			if PlayerLevel > 40 then
				return true
			end
		end
	end
	return false
end

local function ShowMessageInfo(SelTypeNum)
	local function CallBack(g_GameMain,uButton)
		local bFlag = (uButton == MB_BtnOK)
		if bFlag then
			Gac2Gas:JoinDaDuoBao(g_Conn, SelTypeNum)
		end
		g_GameMain.m_MsgBox = nil
		return true
	end
	local Money = 0
	local ShowInfo = MsgBoxMsg(5007, Money)
	if SelTypeNum == 1 then
		Money = 30
		ShowInfo = MsgBoxMsg(5007, Money)
	elseif SelTypeNum == 2 then
		Money = 60
		ShowInfo = MsgBoxMsg(5007, Money)
	elseif SelTypeNum == 3 then
		Money = 90
		ShowInfo = MsgBoxMsg(5007, Money)
	elseif SelTypeNum == 4 then
		Money = 100
		ShowInfo = MsgBoxMsg(5008)
	else
		return
	end
	
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, ShowInfo, BitOr( MB_BtnOK, MB_BtnCancel), CallBack, g_GameMain)
end

function CDaDuoBaoWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			--关闭窗口
			self:ShowWnd(false)
		else
			for i=1,4 do
				if Child == self.m_JoinBtn[i] then
					if not IsSameLevel(i) then
						MsgClient(3500)
					else
						ShowMessageInfo(i)
					end
					self:ShowWnd(false)
					return
				end
			end
		end
	end
end
