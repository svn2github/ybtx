--gac_require "activity/fb_action/mercenary_eduact_wnd/ScoreInfoWndInc"
gac_gas_require "activity/fb/MercenaryEducateActCfgCheck"
lan_load "fb_game/Lan_MercenaryEducateAction_Server"

CYbEduActScoreInfoWnd = class(SQRDialog)
local CYbEduActInfoWnd = class(SQRDialog)
local CYbEduActInfoBtn = class(SQRDialog)
local CXiuXingTaTipsWnd = class(SQRDialog)

--local function CreateInfoWndBtn(parent)
function CYbEduActInfoBtn:Ctor(parent)
--  local Wnd = CYbEduActInfoBtn:new()
  self:CreateFromRes("XiuXingTaInfo", parent)
  self:ShowWnd(true)
  self.m_XiuXingTaInfoBtn = self:GetDlgChild("InfoBtn")
--	return Wnd
end

-- CreateYbEduActScoreInfoWnd
function CYbEduActScoreInfoWnd:Ctor(Parent)
  --local Wnd = CYbEduActScoreInfoWnd:new()
  self:CreateFromRes("FbActionInfoWnd", Parent) 
  self.m_InfoList = self:GetDlgChild("ListInfo")
  
  self.m_XiuXingTaInfoWnd = CYbEduActInfoBtn:new(Parent)--CreateInfoWndBtn(self)
  
  self.m_InfoList.m_ItemTbl = {}
  self.m_MercenaryInfo = CYbEduActInfoWnd:new(self)--self:CreateYbEduActInfoWnd(self)
  self:InitListWnd()
  --return Wnd
end

function CYbEduActInfoBtn:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		local Wnd = g_GameMain.m_YbEducateActInfoWnd
		if Child == self.m_XiuXingTaInfoBtn then
			if Wnd:IsShow() and not Wnd.m_IsCountDown then
				Wnd:CloseWnd()
			else
				if not Wnd.m_IsCountDown and Wnd.m_RulesText and Wnd.m_NowGameID then
					Wnd:InfoWnd(Wnd.m_RulesText, 0, Wnd.m_NowGameID)
				end
			end
		end
	end
end

--function CYbEduActScoreInfoWnd:CreateYbEduActInfoWnd(Parent)
function CYbEduActInfoWnd:Ctor(Parent)
--  local Wnd = CYbEduActInfoWnd:new()
  self:CreateFromRes("Fb_MercenaryInfo",Parent) 	-- 活动介绍
  self:ShowWnd(true)
  self.m_Info = self:GetDlgChild("Info")
  self.m_Title = self:GetDlgChild("Title")
  
  self.ScoreList = {}
--  return Wnd
end

--
--local function CreateXiuXingTaTipsWnd(Parent)
function CXiuXingTaTipsWnd:Ctor(Parent)
--	local Wnd = CXiuXingTaTipsWnd:new()
	self:CreateFromRes("Fb_MercenaryTips", Parent)
	self:ShowWnd(true)
--	return Wnd
end

function CYbEduActScoreInfoWnd:SamllInfoWnd(NowGameID)
	local InfoWnd = self.m_MercenaryInfo
	InfoWnd.m_Title:SetWndText(GetStaticTextClient(9452))
	
	local TargetList = ""
	local OverTypeTbl = g_MercenaryEduActMgr[NowGameID].OverTypeTbl
	local MercMgr = Lan_MercenaryEducateAction_Server(NowGameID)
	local Names = loadstring("return {"..MercMgr("ShowName").."}")()
	if Names then
		InfoWnd.ScoreList = {}
		InfoWnd.ResumeList = {}
		for i, NameTbl in pairs(Names) do
			if NameTbl[1] then
				InfoWnd.ScoreList[NameTbl[1]] = {}
				InfoWnd.ScoreList[NameTbl[1]].Score = 0
				if OverTypeTbl[1][1] == "生存时间" then
					InfoWnd.ScoreList[NameTbl[1]].TotalNum = 1	-- 保护NPC类的 先写为1
				else
					InfoWnd.ScoreList[NameTbl[1]].TotalNum = OverTypeTbl[i][2]
				end
				--InfoWnd.ScoreList[NameTbl[1]].TotalNum = OverTypeTbl[i][2]
				--TargetList = TargetList..NameTbl[1].."    "..OverTypeTbl[i][2].."#r"
			end
		end
	end
	self:UpDateSmallInfoWnd()
end

-- 更新右下角计分板
function CYbEduActScoreInfoWnd:UpDateSmallInfoWnd()
	local TargetList = ""
	local InfoWnd = self.m_MercenaryInfo
	if InfoWnd then
		for NpcName, Tbl in pairs(InfoWnd.ScoreList) do
			TargetList = TargetList..NpcName.."    "..Tbl.Score.."/"..Tbl.TotalNum.."#r"
		end
		--print("TargetList -- "..TargetList)
		InfoWnd.m_Info:SetWndText(TargetList)	
	end
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

function CYbEduActScoreInfoWnd:InitListWnd()
  local Width = GetWndWidth(self.m_InfoList)
  --self.m_InfoList:SetWndWidth(Width)
  self.m_InfoList:InsertColumn(0,Width*0.4)
  self.m_InfoList:InsertColumn(1,Width*0.6)
  
  self.m_InfoList:InsertItem(0, 30)
  self.m_InfoList:InsertItem(1, 30)
  self.m_InfoList:InsertItem(2, 30)
  self.m_InfoList:InsertItem(3, 30)
  
  self:InsertItemList(0,0,GetStaticTextClient(9433))
  self:InsertItemList(1,0,GetStaticTextClient(9434))
  self:InsertItemList(2,0,GetStaticTextClient(9435))
  self:InsertItemList(3,0,GetStaticTextClient(9436))
  
  SetWndRect(self,150)
end

function CYbEduActScoreInfoWnd:InsertItemList(ItemNum, ColNum, ColTitle)
  local Item = self.m_InfoList:GetSubItem(ItemNum,ColNum)
  Item:SetWndText(ColTitle)
  
  if ColNum == 0 and (ItemNum == 1 or ItemNum == 3) then
  	self.m_InfoList.m_ItemTbl[ItemNum] = CXiuXingTaTipsWnd:new(Item) --CreateXiuXingTaTipsWnd(Item)
  	local ShowTips = ""
  	if ItemNum == 1 then
  		ShowTips = ToolTipsText(1000)
  	elseif ItemNum == 3 then
  		ShowTips = ToolTipsText(1001)
  	end
  	self.m_InfoList.m_ItemTbl[ItemNum]:SetMouseOverDescAfter(ShowTips)
  end
end

function CYbEduActScoreInfoWnd:InitInfoWnd()
  local Item = self.m_InfoList:GetSubItem(0,1)
  Item:SetWndText(0)
  self:InsertItemList(1,1,0)
  self:InsertItemList(2,1,0)
  self:InsertItemList(3,1,0)
  self:ShowWnd(true)
end

--经过时间
function CYbEduActScoreInfoWnd:SetTime()
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

function CYbEduActScoreInfoWnd:GetListGameTime(SecondNum)
  local Hour = 0
  local Min = 0
  local Sec = 0
  Hour = math.floor(SecondNum / 3600)
  Min = math.floor((SecondNum-(Hour*3600)) / 60)
  Sec = SecondNum - Hour*3600 - Min * 60
  local GameTime = Hour..GetStaticTextClient(1121)..Min..GetStaticTextClient(1120)..Sec..GetStaticTextClient(1119)
  return GameTime
end

--更新玩家的信息
function CYbEduActScoreInfoWnd:UpdateWndInfo(MercMoney,FloorNum,ExpNum, ToTalFloor)
	self:InsertItemList(1,1,MercMoney)
	self:InsertItemList(2,1,FloorNum.."/"..ToTalFloor)
	self:InsertItemList(3,1,(ExpNum/100))
end

--更新信息,填加到信息面板
function Gas2Gac:RetUpdateYbEduActInfo(Conn,MercMoney,FloorNum,ExpNum, ToTalFloor)
	if FloorNum == "奖励层" then
		FloorNum = GetStaticTextClient(9437)
	end
  g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd:UpdateWndInfo(MercMoney,FloorNum,ExpNum, ToTalFloor)
end

function Gas2Gac:RetBeginYbEduActGame(Conn, DownTime)
  local Wnd = g_GameMain.m_FbActionMgrWnd
  
  if Wnd.m_DownTimeTick then
    UnRegisterTick(Wnd.m_DownTimeTick)
    Wnd.m_DownTimeTick = nil
  end
  
  if DownTime <= 0 then
    Wnd.m_TitleWnd:SetWndText("")
    return
  end
  
  local nowTime = DownTime
  local function DownTime()
    nowTime = nowTime - 1
    local StrInfo = GetStaticTextClient(1017, nowTime)
    if nowTime <= 0 then
      if Wnd.m_DownTimeTick then
        UnRegisterTick(Wnd.m_DownTimeTick)
        Wnd.m_DownTimeTick = nil
      end
      StrInfo = ""
    elseif nowTime <= 5 then
      MsgClient(191017,nowTime)
    end
    Wnd.m_TitleWnd:SetWndText(StrInfo)
  end
  
  Wnd.m_DownTimeTick = RegisterTick("FbActionMgrWndTick",DownTime,1000)
end
