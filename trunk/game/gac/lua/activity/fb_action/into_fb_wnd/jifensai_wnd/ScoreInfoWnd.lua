gac_require "activity/fb_action/into_fb_wnd/jifensai_wnd/ScoreInfoWndInc"


function CreateJFSScoreInfoWnd(parent)
	local Wnd = CJFSScoreInfoWnd:new()
	Wnd:CreateFromRes("FbActionInfoWnd",parent) 
	Wnd.m_InfoList = Wnd:GetDlgChild("ListInfo")
	Wnd:InitListWnd()
	return Wnd
end

local function SetWndRect(CurrWnd,Height)
	local rt = CFRect:new()
	CurrWnd:GetLogicRect(rt)
	rt.bottom = rt.top + Height
	
	CurrWnd:SetLogicRect(rt)
	CurrWnd.m_InfoList:SetLogicRect(rt)
end

local function GetWndWidth(ChildWnd)
	local rt = CFRect:new()
	ChildWnd:GetLogicRect(rt)
	local width = rt.right-rt.left
	return width
end

function CJFSScoreInfoWnd:InitListWnd()
	local Width = GetWndWidth(self.m_InfoList)
	--self.m_InfoList:SetWndWidth(Width)
	self.m_InfoList:InsertColumn(0,Width*0.4)
	self.m_InfoList:InsertColumn(1,Width*0.6)
	
	self.m_InfoList:InsertItem(0, 30)
	self.m_InfoList:InsertItem(1, 30)
	self.m_InfoList:InsertItem(2, 30)
	
	self:InsertItemList(0,0,GetStaticTextClient(9421))
	self:InsertItemList(1,0,GetStaticTextClient(9431))
	self:InsertItemList(2,0,GetStaticTextClient(9432))
	
	SetWndRect(self,100)
end

function CJFSScoreInfoWnd:InsertItemList(ItemNum, ColNum, ColTitle)
	local Item = self.m_InfoList:GetSubItem(ItemNum,ColNum)
	Item:SetWndText(ColTitle)
end

function CJFSScoreInfoWnd:InitInfoWnd()--(PlayerNum,EnemyNum)
	local Item = self.m_InfoList:GetSubItem(0,1)
	Item:SetWndText(0)
	self.m_FriedNum = 0
	self.m_EnemyNum = 0
	self:SetFriendNum(0)
	self:SetEnemyNum(0)
	self:ShowWnd(true)
end

--己方存活人数
function CJFSScoreInfoWnd:SetFriendNum(PlayerNum)
	local Item = self.m_InfoList:GetSubItem(1,1)
	PlayerNum = GetStaticTextClient(9424, PlayerNum)
	Item:SetWndText(PlayerNum)
end

--敌方存活人数
function CJFSScoreInfoWnd:SetEnemyNum(PlayerNum)
	local Item = self.m_InfoList:GetSubItem(2,1)
	PlayerNum = GetStaticTextClient(9424, PlayerNum)
	Item:SetWndText(PlayerNum)
end

--经过时间
function CJFSScoreInfoWnd:SetTime()
	local CostTime = 0
	local Item = self.m_InfoList:GetSubItem(0,1)
	Item:SetWndText(self:GetListGameTime(CostTime))
	
	local function Time()
		local nItem = self.m_InfoList:GetSubItem(0,1)
		if nItem then
			CostTime = CostTime + 1
			nItem:SetWndText(self:GetListGameTime(CostTime))
		else
			UnRegisterTick(g_GameMain.m_FbActionScoreTick)
			g_GameMain.m_FbActionScoreTick = nil
		end
	end
	if g_GameMain.m_FbActionScoreTick then
		UnRegisterTick(g_GameMain.m_FbActionScoreTick)
		g_GameMain.m_FbActionScoreTick = nil
	end
	g_GameMain.m_FbActionScoreTick = RegisterTick( "JFSScoreInfoWndTick", Time, 1000)
end

function CJFSScoreInfoWnd:GetListGameTime(SecondNum)
	local Hour = 0
	local Min = 0
	local Sec = 0
	Hour = math.floor(SecondNum / 3600)
	Min = math.floor((SecondNum-(Hour*3600)) / 60)
	Sec = SecondNum - Hour*3600 - Min * 60
	local GameTime = Hour..GetStaticTextClient(1121)..Min..GetStaticTextClient(1120)..Sec..GetStaticTextClient(1119)
	return GameTime
end

--添加玩家的信息
function CJFSScoreInfoWnd:AddMemberInfo(TargetEntityID,TeamId)
	if g_MainPlayer:GetEntityID() == TargetEntityID then
		self.m_FriendTeamID = TeamId
	end
	if self.m_FriendTeamID == TeamId then
		self.m_FriedNum = self.m_FriedNum + 1
		self:SetFriendNum(self.m_FriedNum)
	else
		self.m_EnemyNum = self.m_EnemyNum + 1
		self:SetEnemyNum(self.m_EnemyNum)
	end
end

--更新玩家的信息
function CJFSScoreInfoWnd:UpdateMemberInfo(FriendNum,EnemyNum)
	self:SetFriendNum(FriendNum)
	self:SetEnemyNum(EnemyNum)
end

--更新玩家的信息
function CJFSScoreInfoWnd:AllMember(TeamID, FriendNum, EnemyNum)
	if self.m_FriendTeamID == TeamID then
		self.m_FriedNum = FriendNum
		self:SetFriendNum(self.m_FriedNum)
		self.m_EnemyNum = EnemyNum
		self:SetEnemyNum(self.m_EnemyNum)
	else
		self.m_FriedNum = EnemyNum
		self:SetFriendNum(self.m_FriedNum)
		self.m_EnemyNum = FriendNum
		self:SetEnemyNum(self.m_EnemyNum)
	end
end

----接收其它人的信息,填加到信息面板
--function Gas2Gac:RetJFSAddMemberInfo(Conn,TargetEntityID,TeamId)
--	g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd:AddMemberInfo(TargetEntityID,TeamId)
--end

--更新信息,填加到信息面板
function Gas2Gac:RetJFSUpdateMemberInfo(Conn,FriendNum,EnemyNum)
	local Wnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
	Wnd:UpdateMemberInfo(FriendNum,EnemyNum)
end
--
----添加玩家的信息
--function Gas2Gac:RetJFSAllMember(Conn, TeamId, FriedNum, EnemyNum)
--	local Wnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
--	Wnd:AllMember(TeamId, FriedNum, EnemyNum)
--end
