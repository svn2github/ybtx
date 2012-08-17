cfg_load "fb_game/JuQingTransmit_Common"
lan_load "fb_game/Lan_JuQingTransmit_Common"

CJuQingTransmitWnd = class(SQRDialog)

local CCheckBtnWnd = class(SQRDialog)
local CheckBtnNum = 5


local function CheckBtnWnd(parent)
	local Wnd = CCheckBtnWnd:new()
	Wnd:CreateFromRes("JuQing_Btn_Wnd",parent)
	Wnd.m_Title = Wnd:GetDlgChild("Title")
	Wnd.m_Info = Wnd:GetDlgChild("Info")
	Wnd:ShowWnd(true)
	
	return Wnd
end

function CJuQingTransmitWnd:Ctor(parent)
	self:CreateFromRes("JuQing_Wnd",parent)
	self.m_AutoBtn = self:GetDlgChild("AutoBtn")
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CloseBtn = self:GetDlgChild("Close")
	self.m_upBtn = self:GetDlgChild("up")
	self.m_downBtn = self:GetDlgChild("down")
	
	self.m_CheckBtn = {}
	for i=1, CheckBtnNum do
		self.m_CheckBtn[i] = self:GetDlgChild("CheckBtn"..i)
		self.m_CheckBtn[i].CheckBtn = CheckBtnWnd(self.m_CheckBtn[i])
	end
	
	self.m_Depict = self:GetDlgChild("Depict")
	self.m_Info = self:GetDlgChild("Info")
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CJuQingTransmitWnd.GetWnd()
	local Wnd = g_GameMain.m_JuQingTransmitWnd
	if not Wnd then
		Wnd = CJuQingTransmitWnd:new(g_GameMain)
		g_GameMain.m_JuQingTransmitWnd = Wnd
	end
	return Wnd
end

function CJuQingTransmitWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn or Child == self.m_AutoBtn then
			local ActionName = nil
			for i=1, CheckBtnNum do
				if self.m_CheckBtn[i]:GetCheck() then
					ActionName = self.m_CheckBtn[i].m_ActionName
					break
				end
			end
			if ActionName then
				Gac2Gas:EnterSenarioFb(g_Conn, ActionName, Child==self.m_AutoBtn)
				self:ShowWnd(false)
			end
		elseif Child == self.m_CloseBtn then
			self:ShowWnd(false)
		elseif Child == self.m_upBtn then
			self:ModifyShowCheckNum(true)
			self:InitCheckBtnInfo()
		elseif Child == self.m_downBtn then
			self:ModifyShowCheckNum(false)
			self:InitCheckBtnInfo()
		else
			for i=1, CheckBtnNum do
				if Child == self.m_CheckBtn[i] then
					local ActionName = Child.m_ActionName
					if ActionName then
						self:InitInfo(ActionName)
					end
					break
				end
			end
		end
	end
end

--修改显示那几个副本活动
function CJuQingTransmitWnd:ModifyShowCheckNum(UporDown)
	local left = self.m_BeginEndCheckBtn[1]
	local right = self.m_BeginEndCheckBtn[2]
	if UporDown then
		if (left-1) >= 1 then
			self.m_BeginEndCheckBtn = {left-1,right-1}
		end
	else
		local keys = JuQingTransmit_Common:GetKeys()
		if (right+1) <= #(keys) then
			self.m_BeginEndCheckBtn = {left+1,right+1}
		end
	end
end

--初始选择按扭列表的信息(参数,为活动ID)
function CJuQingTransmitWnd:InitCheckBtnInfo()
	for i=1, CheckBtnNum do
		self.m_CheckBtn[i].m_Lock = false
		self.m_CheckBtn[i].m_ActionName = nil
		self.m_CheckBtn[i].CheckBtn.m_Title:SetWndText("")
		self.m_CheckBtn[i].CheckBtn.m_Info:SetWndText("")
		self.m_CheckBtn[i]:SetCheck(false)
	end
	
	self.m_AutoBtn:EnableWnd(false)
	self.m_OkBtn:EnableWnd(false)
	
	local PlayerLevel = g_MainPlayer:CppGetLevel()
	
	local keys = JuQingTransmit_Common:GetKeys()
	for FbID=1, #(keys) do
		if FbID >= self.m_BeginEndCheckBtn[1] and FbID <= self.m_BeginEndCheckBtn[2] then
			local ActionName = keys[FbID]
			for i=1, CheckBtnNum do
				local Btn = self.m_CheckBtn[i]
				if not Btn.m_Lock then
					Btn.m_Lock = true
					
					Btn.m_ActionName = ActionName
					
					local str1 = Lan_JuQingTransmit_Common(MemH64(ActionName), "DisplayName")
					local str2 = Lan_JuQingTransmit_Common(MemH64(ActionName), "Introduce")
					Btn.CheckBtn.m_Title:SetWndText(str1)
					Btn.CheckBtn.m_Info:SetWndText(g_QuestInfoTextTransfer(str2))
					
					local level = JuQingTransmit_Common(ActionName, "Level")
					if PlayerLevel >= level then
						Btn:EnableWnd(true)
					else
						Btn:EnableWnd(false)
					end
					
					break
				end
			end
			
		end
	end
end

local function AddImage(Wnd, strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

--初始化副本的信息
function CJuQingTransmitWnd:InitInfo(ActionName)
	local EndInfo = Lan_JuQingTransmit_Common(MemH64(ActionName),"EndInfo")
	self.m_Depict:SetWndText(g_QuestInfoTextTransfer(EndInfo))
	
	local strTex = JuQingTransmit_Common(ActionName,"BgTex")
	if strTex ~= "" then
		AddImage(self, strTex)
	end
	
	local DecInfo = Lan_JuQingTransmit_Common(MemH64(ActionName),"DepictInfo")
	self.m_Info:SetWndText(g_QuestInfoTextTransfer(DecInfo))
	
	if self.m_FinishAction[ActionName] then
		self.m_AutoBtn:EnableWnd(false)
		self.m_OkBtn:EnableWnd(false)
	else
		self.m_AutoBtn:EnableWnd(true)
		self.m_OkBtn:EnableWnd(true)
	end
end

local FinishAction = nil
function CJuQingTransmitWnd:InitActionInfo(ActionName)
	if not FinishAction then
		FinishAction = {}
	end
	table.insert(FinishAction, ActionName)
end

--打开剧情本,并初始化信息
function CJuQingTransmitWnd:InitWnd(EntityID)
	self.m_FinishAction = {}
	if FinishAction then
		for i=1, #(FinishAction) do
			self.m_FinishAction[FinishAction[i]] = true
		end
		FinishAction = nil
	end
	
	self.m_Depict:SetWndText("")
	self.m_Info:SetWndText("")
	
	self.m_BeginEndCheckBtn = {1,5}
	self:InitCheckBtnInfo()
	
	self.m_AutoBtn:SetFocus()
	self.m_OkBtn:SetFocus()
	
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end




--确定进副本的消息(剧情副本)
function CJuQingTransmitWnd.JoinScene(SceneName, PosX, PosY)

	local function IsJoinScopesFb(wnd, btnIndex)
		if btnIndex == 1 then
			Gac2Gas:AgreedJoinSenarioFb(g_Conn, SceneName, PosX, PosY)
		end
		wnd:Close()
		g_GameMain.m_SenarioMsgBox = nil
	end
	
	if g_GameMain.m_SenarioMsgBox then
		g_GameMain.m_SenarioMsgBox:Close()
		g_GameMain.m_SenarioMsgBox = nil
	end
	
	local msg = MsgBoxMsg(5001, GetSceneDispalyName(SceneName))
	msg = msg.." (%d)"
	local OkStr = GetStaticTextClient(1124)
	local CancelStr = GetStaticTextClient(1025)
	g_GameMain.m_SenarioMsgBox = CCountdownWnd:new(g_GameMain, msg, 30, IsJoinScopesFb, {OkStr, CancelStr})
end





local CJQCameraWnd = class(SQRDialog)

local function CreateWnd(Info)
	local Wnd = CJQCameraWnd:new()
	Wnd:CreateFromRes("jq_camera_wnd",g_GameMain)
	Wnd.m_Info = Wnd:GetDlgChild("Info")
	
	Wnd.m_Info:SetWndText(Info)
	Wnd:ShowWnd(true)
	return Wnd
end


local function BeginChangeCamera(Height)
	if g_MainPlayer and g_MainPlayer.m_ChangeCamera then
		local RenderScene=g_CoreScene:GetRenderScene()
		local RunTime = 50000--转一圈所用的时间单位ms
		local CurrLocal = RenderScene:GetCameraOffset()
		local MoveCofe = (Height * 64 - CurrLocal) / 4000 * 20--每20毫秒要移动的距离
		
		local function Tick_CallBack(Tick)
			if CurrLocal < Height * 64 then
				if RenderScene.SetCameraOffset then
					CurrLocal = CurrLocal + MoveCofe
					RenderScene:SetCameraOffset(CurrLocal)
				end
			else
				if RenderScene.SetCameraXRotate then
					RenderScene:SetCameraXRotate( RenderScene:GetCameraXRotate() + 2*3.1415926/(RunTime/20) )
					RenderScene:SetCameraYRotate( RenderScene:GetCameraYRotate() + 2*3.1415926/(RunTime/20) )
				end
			end
		end
		
		--开始转镜头
		g_GameMain.m_CameraMoveTick = RegisterTick("CameraMoveTick",Tick_CallBack,20)
	end
end

--改变剧情本中的镜头位置信息
function CJuQingTransmitWnd.RetChangeCameraPos(ActionName, IsOpen)
	if g_GameMain.m_CameraMoveTick then
		UnRegisterTick(g_GameMain.m_CameraMoveTick)
		g_GameMain.m_CameraMoveTick = nil
	end
	
	if IsOpen then
		local Height = JuQingTransmit_Common(ActionName,"CameraHeight")
		if Height and IsNumber(Height) and (Height >= 20 and Height <= 40)then
			local RenderScene = g_CoreScene:GetRenderScene()
			--RenderScene:SetDistSpeed(99999)
			RenderScene:SetDist(5,Height)
			--RenderScene:SetCameraOffset( Height * 64 )
			g_MainPlayer.m_ChangeCamera = true
			local str = Lan_JuQingTransmit_Common(MemH64(ActionName), "CameraInfo")
			
			if g_GameMain.m_JQCameraWnd then
				g_GameMain.m_JQCameraWnd:ShowWnd(false)
				g_GameMain.m_JQCameraWnd:DestroyWnd()
				g_GameMain.m_JQCameraWnd = nil
			end
			g_GameMain.m_JQCameraWnd = CreateWnd(str)
			
			BeginChangeCamera(Height)
		end
	else
		CRenderSystemClient_Inst():SetCameraNoChange(false)
		CRenderSystemClient_Inst():InitCameraInfo()
		g_MainPlayer.m_ChangeCamera = nil
		
		g_GameMain.m_JQCameraWnd:ShowWnd(false)
		g_GameMain.m_JQCameraWnd:DestroyWnd()
		g_GameMain.m_JQCameraWnd = nil
	end
end
