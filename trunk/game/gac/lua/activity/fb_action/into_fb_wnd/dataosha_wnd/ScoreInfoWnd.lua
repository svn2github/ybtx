gac_require "activity/fb_action/into_fb_wnd/dataosha_wnd/ScoreInfoWndInc"

function CreateDTSScoreInfoWnd(parent)
	local Wnd = CDTSScoreInfoWnd:new()
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

function CDTSScoreInfoWnd:InitListWnd()
	local Width = GetWndWidth(self.m_InfoList)
	--self.m_InfoList:SetWndWidth(Width)
	self.m_InfoList:InsertColumn(0,Width*0.4)
	self.m_InfoList:InsertColumn(1,Width*0.6)
	
	self.m_InfoList:InsertItem(0, 30)
	self.m_InfoList:InsertItem(1, 30)
	self.m_InfoList:InsertItem(2, 30)
	self.m_InfoList:InsertItem(3, 30)
	self.m_InfoList:InsertItem(4, 30)
	
	self:InsertItemList(0,0,GetStaticTextClient(9421))
	self:InsertItemList(1,0,GetStaticTextClient(9417))
	self:InsertItemList(2,0,GetStaticTextClient(9416))
	self:InsertItemList(3,0,GetStaticTextClient(9422))
	self:InsertItemList(4,0,GetStaticTextClient(9423))
	
	SetWndRect(self,150)
end

function CDTSScoreInfoWnd:InsertItemList(ItemNum, ColNum, ColTitle)
	local Item = self.m_InfoList:GetSubItem(ItemNum,ColNum)
	Item:SetWndText(ColTitle)
end

function CDTSScoreInfoWnd:InitInfoWnd()
	local Item = self.m_InfoList:GetSubItem(0,1)
	Item:SetWndText(0)
	self:InsertItemList(1,1,0)
	self:InsertItemList(2,1,0)
	self:InsertItemList(3,1,0)
	self:InsertItemList(4,1,0)
	self.m_StillPlayerNum = 0
	self:ShowWnd(true)
end

--经过时间
function CDTSScoreInfoWnd:SetTime()
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

function CDTSScoreInfoWnd:GetListGameTime(SecondNum)
	local Hour = 0
	local Min = 0
	local Sec = 0
	Hour = math.floor(SecondNum / 3600)
	Min = math.floor((SecondNum-(Hour*3600)) / 60)
	Sec = SecondNum - Hour*3600 - Min * 60
	local GameTime = Hour..GetStaticTextClient(1121)..Min..GetStaticTextClient(1120)..Sec..GetStaticTextClient(1119)
	return GameTime
end

function CDTSScoreInfoWnd:UpdateMemberInfo(KillNum,StillNum,CurrPoint,MostKillNum)
	self.m_StillPlayerNum = self.m_StillPlayerNum - 1
	self:InsertItemList(1,1,GetStaticTextClient(9424, KillNum))
	self:InsertItemList(2,1,GetStaticTextClient(9424, StillNum))
	self:InsertItemList(3,1,GetStaticTextClient(9424, MostKillNum))
	self:InsertItemList(4,1,CurrPoint)
end

function Gas2Gac:DtsUpdateAllInfo(Conn,KillNum,StillNum,CurrPoint,MostKillNum)
	g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd:UpdateMemberInfo(KillNum,StillNum,CurrPoint,MostKillNum)
end

function Gas2Gac:DtsUpdateLivePlayerNum(Conn, StillNum)
	g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd:InsertItemList(2,1,GetStaticTextClient(9424, StillNum))
end