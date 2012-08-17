gac_gas_require "scene_mgr/SceneCfg"
lan_load "fb_game/Lan_AreaFb_Common"
cfg_load "fb_game/JoinCountLimit_Server"

CAreaFbSelWndNew = class(SQRDialog)
local CAreaFbSelWndListItem = class(SQRDialog)
local TrackInfoColorStr = "#e000000"..g_ColorMgr:GetColor("追踪面板文字")

function CAreaFbSelWndNew:Ctor()
	self:CreateFromRes("AreaFbSelWndNew",g_GameMain)
	self.m_ListInfo = self:GetDlgChild("ListInfo")
	self.m_Start = self:GetDlgChild("Start")
	self.m_Close = self:GetDlgChild("Close")
	self.m_tblListInfoItem = {}
	self.m_ActionTimesTbl = {}
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CAreaFbSelWndNew:CreateListItem(parent)
	local wnd = CAreaFbSelWndListItem:new()
	wnd:CreateFromRes("FBAreaSelWndListItem", parent)
	wnd.m_FbName = wnd:GetDlgChild("FbName")
	wnd.m_NeedLevel = wnd:GetDlgChild("NeedLevel")
	wnd.m_Type = wnd:GetDlgChild("Type")
	wnd.m_GameMode = wnd:GetDlgChild("GameMode")
	wnd.m_ResidueDegree = wnd:GetDlgChild("ResidueDegree")
	wnd:ShowWnd(true)
	return wnd
end

function CAreaFbSelWndNew:InsertListInfo(FbName, NeedLevel, Type, GameMode, ResidueDegree )
	if FbName then
		local ItemCount = self.m_ListInfo:GetItemCount()
		self.m_ListInfo:InsertItem(ItemCount, 30)
		local Item = self.m_ListInfo:GetSubItem(ItemCount, 0)
		Item:SetMouseWheel(true)
		local itemWnd = self:CreateListItem(Item)

		local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
		itemWnd:SetWndTextColor(Flag,0xffffffff)
		
		local NumberColor = AreaFb_Common(FbName, "NumberColor")
		local Color = ""
		if NumberColor ~= "" then
			Color = "#e000000"..g_ColorMgr:GetColor(NumberColor)
		end
		
		local PlayerLev = g_MainPlayer:CppGetLevel()
		local BandColor = ""
		if PlayerLev < AreaFb_Common(FbName, "NeedLevel") then
			BandColor = "#ca8966b"
			Color = BandColor
			ResidueDegree = GetStaticTextClient(8008)
		end
			
		itemWnd.m_FbName:SetWndText(BandColor..GetSceneDispalyName(FbName))
		itemWnd.m_NeedLevel:SetWndText(BandColor..NeedLevel)
		itemWnd.m_Type:SetWndText(BandColor..Type)
		itemWnd.m_GameMode:SetWndText(BandColor..GameMode)

		itemWnd.m_ResidueDegree:SetWndText(Color..ResidueDegree)
		table.insert(self.m_tblListInfoItem, itemWnd)
		Item.m_SceneName = FbName
		--Item.m_WndTexFile = AreaFb_Common(sceneName, "WndTexFile")
	end
end

function CAreaFbSelWndNew:InitAreaFbSelWnd()
	Gac2Gas:GetDungeonCount(g_Conn)
end

function CAreaFbSelWndNew.SetDungeonCount(Conn, SceneName, Counts)
	--local LimitType = AreaFb_Common(SceneName, "LimitType")
	local ActionName = AreaFb_Common(SceneName, "ActionName")
	local LimitType = FbActionDirect_Common(ActionName, "MaxTimes")
	g_GameMain.m_AreaFbSelWndNew.m_ActionTimesTbl[SceneName] = JoinCountLimit_Server(LimitType, "Count") - Counts
end

function CAreaFbSelWndNew.RetShowDungeonInfoWnd(Conn)
	if IsCppBound(g_MainPlayer) then
		g_GameMain.m_AreaFbSelWndNew.m_AreaFbName = nil
		g_GameMain.m_AreaFbSelWndNew:OpenAreaFbSelWnd()
	end
end

function CAreaFbSelWndNew:OpenAreaFbSelWnd()
	self:InitListWnd()
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CAreaFbSelWndNew:InitListWnd()
	self.m_ListInfo:DeleteAllItem()
	self.m_ListInfo:InsertColumn(0, self.m_ListInfo:GetWndOrgWidth())
	self.m_tblListInfoItem = {}
	------------
	--加一个默认的贴图
--	local strTex = g_ImageMgr:GetImagePath(1334)	
--	self:AddImage(strTex)
	-------------
	-- 默认显示自然副本
	self:ShowFbList()
end

function CAreaFbSelWndNew:OpenAllAreaFb(Conn)
	g_GameMain.m_AreaFbSelWndNew.m_GmOpenAllAreaFb = true
end

-- 显示副本列表
function CAreaFbSelWndNew:ShowFbList()
		-- 清除列表
		self.m_ListInfo:DeleteAllItem()
		self.m_ListInfo:InsertColumn(0, self.m_ListInfo:GetWndOrgWidth())

		-- 插入列表		
		local PlayerLev = g_MainPlayer:CppGetLevel()
		for _, FbName in pairs(AreaFb_Common:GetKeys()) do
			local p = AreaFb_Common(FbName)
--			if PlayerLev >= p("NeedLevel") then
			if self.m_GmOpenAllAreaFb or AreaFb_Common(FbName, "IsShow") == 1 then
				--Gac2Gas:CheckAreaFbIsOpen(g_Conn, FbName)
				local times = self.m_ActionTimesTbl[FbName]
				local PropertyValue = p("PropertyValue")
				local Type = Lan_AreaFb_Common(MemH64(FbName), "Type")..":"..PropertyValue
				local Mode = Lan_AreaFb_Common(MemH64(FbName), "ModeShowName")
				self:InsertListInfo(FbName, p("NeedLevel"),Type , Mode, times)						
			end
		end
end

function CAreaFbSelWndNew.RetAreaFbBossNum(SceneName, SceneId, ServerId, LeftNum, BossNum)
	local function IsJoinScene(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:DecideJoinAreaFb(g_Conn, SceneName, SceneId, ServerId)
		end
		g_GameMain.m_ChangeOutMsgBox = nil
		return true
	end
	if g_GameMain.m_ChangeOutMsgBox then
		return
	end
local str = MsgBoxMsg(5015, GetSceneDispalyName(SceneName), LeftNum, BossNum)
	g_GameMain.m_ChangeOutMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsJoinScene,g_GameMain)
end

function CAreaFbSelWndNew.RetDelAllAreaFb()
	for SceneName, Wnd in pairs(g_GameMain.m_AreaFbMsgWnd) do
		if Wnd:IsShow() then
			Wnd:Close()
			g_GameMain.m_AreaFbMsgWnd[SceneName] = nil
		end
	end
end

function CAreaFbSelWndNew.RetIsJoinAreaFb(SceneName, FbDifficulty, FbLev, SceneID, ServerId)
	local function IsJoinAreaFb(wnd, btnIndex)
		if btnIndex == 1 then
			Gac2Gas:AgreedJoinAreaFb(g_Conn, SceneName, SceneID, ServerId)
		end
		wnd:Close()
		g_GameMain.m_AreaFbMsgWnd[SceneName] = nil
	end
	
	if g_GameMain.m_AreaFbMsgWnd[SceneName] then
		g_GameMain.m_AreaFbMsgWnd[SceneName]:Close()
		g_GameMain.m_AreaFbMsgWnd[SceneName] = nil
	end
	
	local msg = MsgBoxMsg(5020, GetSceneDispalyName(SceneName))
	msg = msg.." (%d)"
	local OkStr = GetStaticTextClient(1124)
	local CancelStr = GetStaticTextClient(1025)
	g_GameMain.m_AreaFbMsgWnd[SceneName] = CCountdownWnd:new(g_GameMain, msg, 30, IsJoinAreaFb, {OkStr, CancelStr})
	
	if FbDifficulty then
		g_GameMain.m_AreaFbMsgWnd[SceneName].m_EquipAssessWnd = CEquipAssessWnd:new(g_GameMain.m_AreaFbMsgWnd[SceneName], FbDifficulty, FbLev, SceneName)
	end
end

function CAreaFbSelWndNew.RetIsChangeOut(time, msgID)
	local function IsChangeOut(wnd, btnIndex)
		if btnIndex == 1 then
			Gac2Gas:AgreedChangeOut(g_Conn)
		end
		wnd:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	if g_GameMain.m_ChangeOutMsgBox then
		g_GameMain.m_ChangeOutMsgBox:Close()
		g_GameMain.m_ChangeOutMsgBox = nil
	end
	local OkStr = GetStaticTextClient(1117)
	local msg = GetStaticTextClient(msgID) .. " %d "..GetStaticTextClient(8800)
	g_GameMain.m_ChangeOutMsgBox = CCountdownWnd:new(g_GameMain, msg, time - 1, IsChangeOut, {OkStr})
end

function CAreaFbSelWndNew:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Start then
			local SceneName = self.m_AreaFbName
			if SceneName == nil then
				return
			end
			local Wnd = CAreaFbInfoWnd.GetWnd()
			local TargetNpcId = Wnd.m_TargetNpcId
			Gac2Gas:EnterGeneralAreaFb(g_Conn, TargetNpcId, SceneName)
			self:ShowWnd(false)
		elseif Child == self.m_Close then
			self:ShowWnd(false)
		end
	elseif uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_SHIFTDOWN or uMsgID == ITEM_CTRLDOWN then
		if Child == self.m_ListInfo then
			local Item = self.m_ListInfo:GetSubItem(uParam1, 0)
			self.m_AreaFbName = Item.m_SceneName
		end
	end
end
