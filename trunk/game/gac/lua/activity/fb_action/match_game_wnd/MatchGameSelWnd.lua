cfg_load "fb_game/MatchGame_Common"
CMatchGameSelWnd = class(SQRDialog)
local CMatchGameSelWndListItem = class(SQRDialog)
local CMatchGameSelDesWnd = class(SQRDialog)

function CMatchGameSelWnd:Ctor()
	self.m_tblGameListItem = {}
	self.m_tblRoomListItem = {}
	self:CreateFromRes("MatchGameSelWnd", g_GameMain)
	self.m_DescriptionWnd = self:GetDlgChild("Description")
	self.m_GameList = self:GetDlgChild("GameList")
	self.m_JoinBtn = self:GetDlgChild("JoinBtn")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_AutoTeamBtn = self:GetDlgChild("AutoTeamBtn")
	
	--关闭一些暂时不用的控件显示
	--self.m_AutoTeamBtn:ShowWnd(false)
	self.m_DescriptionWnd:ShowWnd(false)
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

local function GetOneActionOfNpcCanJoin(npcName)
	for _, gameName in pairs(MatchGame_Common:GetKeys()) do
		local npcTbl = GetCfgTransformValue(true, "MatchGame_Common", gameName, "JoinNpcName")
		for _, name in pairs(npcTbl) do
			if name == npcName then
				return gameName
			end
		end
	end
end

function CMatchGameSelWnd:Init(gameNames, npcName)
	self.m_GameTbl = loadstring("return {" .. gameNames .. "}")()
	
	--[[
	if #self.m_GameTbl == 0 then
		MsgClient(191025)
		return
	end
	]]--
	
	self.m_tblGameListItem = {}
	
	self.m_GameList:DeleteAllItem()
	self.m_GameList:InsertColumn(0, self.m_GameList:GetWndOrgWidth())
	for i, name in pairs(self.m_GameTbl) do
		local showName = g_GetFbActionNameLanStr(name)
		self.m_GameList:InsertItem(i -1 , 25)
		local minLv = MatchGame_Common(name, "MinLevel")
		local maxLv = MatchGame_Common(name, "MaxLevel")
		local item = self.m_GameList:GetSubItem(i -1 , 0)
		item:SetMouseWheel(true)
		local itemWnd = self:CreateListItem(item)
		itemWnd:SetWndText(showName)
		table.insert(self.m_tblGameListItem, itemWnd)
	end
	
	
	local strTex = g_ImageMgr:GetImagePath(1336)
	self:AddImage(strTex)

	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
	
	local sampleAction =  GetOneActionOfNpcCanJoin(npcName)
	local TeamType = MatchGame_Common(sampleAction, "TeamType")
	if string.sub(TeamType,-2,-2) == "1"  then
		self.m_AutoTeamBtn:ShowWnd(true)
	else
		self.m_AutoTeamBtn:ShowWnd(false)
	end
	if string.sub(TeamType,-1,-1) == "1"  then
		self.m_JoinBtn:ShowWnd(true)
	else
		self.m_JoinBtn:ShowWnd(false)
	end
	
	if self.m_AutoTeamBtn:IsShow() then
		if self.m_JoinBtn:IsShow() then
			self.m_AutoTeamBtn:SetWndText(GetStaticTextClient(9449))
			self.m_JoinBtn:SetWndText(GetStaticTextClient(9450))
		else
			self.m_AutoTeamBtn:SetWndText(GetStaticTextClient(9451))
		end
	else
		if self.m_JoinBtn:IsShow() then
			self.m_JoinBtn:SetWndText(GetStaticTextClient(9451))
		end
	end
end

function CMatchGameSelWnd:AddImage(strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self:SetWndBkImage( Flag, DefaultImage )
end

function CMatchGameSelWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
		elseif Child == self.m_JoinBtn or Child == self.m_AutoTeamBtn then
			local gameIndex = self.m_GameList:GetSelectItem(-1) + 1
			if gameIndex <= 0 then
				return
			end
			if self.m_GameTbl[gameIndex] then
				Gac2Gas:JoinMatchGame(g_Conn, self.m_GameTbl[gameIndex], Child == self.m_AutoTeamBtn)
				self:ShowWnd(false)
			end
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_SHIFTDOWN or uMsgID == ITEM_CTRLDOWN) then
		if Child == self.m_GameList then
			if self.m_GameTbl[uParam1 + 1] then

				local texName = MatchGame_Common(self.m_GameTbl[uParam1 + 1], "WndTexFile")
				if texName ~= "" then
					self:AddImage(texName)
				end
			end
		end
	end
end

function CMatchGameSelWnd:CreateListItem(parent)
	local wnd = CMatchGameSelWndListItem:new()
	wnd:CreateFromRes("MatchGameSelWndListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end


