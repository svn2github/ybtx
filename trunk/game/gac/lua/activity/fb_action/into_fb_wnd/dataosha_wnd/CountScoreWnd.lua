gac_require "activity/fb_action/into_fb_wnd/dataosha_wnd/CountScoreWndInc"

function CreateDTSCountScoreWnd(parent)
	local Wnd = CDTSCountScoreWnd:new()
	Wnd:CreateFromRes("FbActionCountInfoWnd",parent) 
	Wnd.m_OkBtn = Wnd:GetDlgChild("OkBtn")
	Wnd.m_Title = Wnd:GetDlgChild("Title")
	Wnd.m_RemnantNum = Wnd:GetDlgChild("TimesNum")
	Wnd.m_RemnantTitle = Wnd:GetDlgChild("TimesTitle")
	Wnd.m_ScoreList = Wnd:GetDlgChild("ScoreList")
	
	Wnd.m_RemnantNum:ShowWnd(false)
	Wnd.m_RemnantTitle:ShowWnd(false)
	Wnd:InitListWnd()
	return Wnd
end

function CDTSCountScoreWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn then
			Gac2Gas:ExitDTSFbAction(g_Conn)
			self:ShowWnd(false)
		end
	end
end

function CDTSCountScoreWnd:InitWnd()
	self:ShowWnd(true)
end

local function GetWndWidth(ChildWnd)
	local rt = CFRect:new()
	ChildWnd:GetLogicRect(rt)
	local width = rt.right-rt.left
	return width
end

function CDTSCountScoreWnd:InitListWnd()
	local Width = GetWndWidth(self.m_ScoreList)
	self.m_ScoreList:SetWndWidth(Width)
	self.m_ScoreList:InsertColumn(0,Width*0.20)
	self.m_ScoreList:InsertColumn(1,Width*0.30)
	self.m_ScoreList:InsertColumn(2,Width*0.20)
	self.m_ScoreList:InsertColumn(3,Width*0.30)
	
	self.m_ScoreList:InsertItem(0, 36)
	self.m_ScoreList:InsertItem(1, 36)
	self.m_ScoreList:InsertItem(2, 36)
	
	self:InsertItemList(0,0,2,GetStaticTextClient(9415),GetStaticTextClient(9416))
	self:InsertItemList(1,0,2,GetStaticTextClient(9417),GetStaticTextClient(9418))
	self:InsertItemList(2,0,2,GetStaticTextClient(9419),GetStaticTextClient(9444))
end

function CDTSCountScoreWnd:InsertItemList(ItemNum, ColNum1, ColNum2, ColTitle, ColInfo)
	local Item1 = self.m_ScoreList:GetSubItem(ItemNum,ColNum1)
	Item1:SetWndText(ColTitle)
	local Item3 = self.m_ScoreList:GetSubItem(ItemNum,ColNum2)
	Item3:SetWndText(ColInfo)
end

function CDTSCountScoreWnd:InitWnd(SecondNum,SpareTimes,StrInfoTbl)
	self:SetMatchInfo(SecondNum,StrInfoTbl)
	self.m_RemnantNum:SetWndText(SpareTimes)
	self.m_Title:SetWndText(GetStaticTextClient(1013))
	self:ShowWnd(true)
end

function CDTSCountScoreWnd:SetMatchInfo(SecondNum,StrInfoTbl)
	if StrInfoTbl == "" then
		return
	else
		local InfoTbl = loadstring("return {"..StrInfoTbl.."}")()
		local StillNum,KillNum,CurrGloryPoint,wWinTimes,cWinTimes,wGloryPoint = unpack(InfoTbl)
		
		local GameTime = self:GetListGameTime(SecondNum)
		local strWinTimes = GetStaticTextClient(9420, wWinTimes, cWinTimes)
		
		self:InsertItemList(0,1,3,GameTime,StillNum)
		self:InsertItemList(1,1,3,KillNum,strWinTimes)
		self:InsertItemList(2,1,3,CurrGloryPoint,wGloryPoint)
	end
end

function CDTSCountScoreWnd:GetListGameTime(SecondNum)
	local Hour = 0
	local Min = 0
	local Sec = 0
	
	Hour = math.floor(SecondNum / 3600)
	Min = math.floor((SecondNum-(Hour*3600)) / 60)
	Sec = SecondNum - Hour*3600 - Min * 60
	local GameTime = Hour..GetStaticTextClient(1121)..Min..GetStaticTextClient(1120)..Sec..GetStaticTextClient(1119)
	return GameTime
end
