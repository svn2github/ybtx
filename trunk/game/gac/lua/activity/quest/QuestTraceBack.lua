gac_require "message/message_box/MsgBoxMsg"
gac_gas_require "framework/common/CAlarmClock"
cfg_load "fb_game/FbActionDirect_Common"
cfg_load "fb_game/MatchGame_Common"

CQuestTraceBackWnd = class (SQRDialog)
local CQuestTrackInfoItemWnd = class (SQRDialog)
local CQuestNameItemWnd = class (SQRDialog)
--CTrackTypeSelectWnd = class (SQRDialog)
--CTrackPageSelectWnd = class (SQRDialog)

local TrackBound = 6
local QuestNameHeight = 22

local m_ZlTblName = GetStaticTextClient(1185)--总览
local m_ZxTblName = GetStaticTextClient(1186)--主线
local m_YbTblName = GetStaticTextClient(1187)--佣兵
local m_TzTblName = GetStaticTextClient(1188)--挑战
local m_WfTblName = GetStaticTextClient(1189)--玩法

local ZxColor = g_ColorMgr:Get16Color("主线任务名称")
local YbColor = g_ColorMgr:Get16Color("佣兵任务名称")
local TzColor = g_ColorMgr:Get16Color("挑战任务名称")
local WfColor = g_ColorMgr:Get16Color("玩法任务名称")
local TempColor = g_ColorMgr:Get16Color("低等级任务名称")

--local AdvanceNotTime = 30
--local EndActionNotifyTickTbl = {}
local TrackInfoColorStr = "#e000000"..g_ColorMgr:GetColor("追踪面板文字")
local LinkNpcBaseColor = g_ColorMgr:GetColor("自动寻路")
local LinkNpcColor = "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到NpcObj"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)
local LinkSkillBaseColor = g_ColorMgr:GetColor("技能超连接颜色")
local LinkSkillColor = "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到Skill"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)
local LinkItemBaseColor = g_ColorMgr:GetColor("物品超链接颜色")
local LinkItemColor = "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标移动到Item"), 3, -1) .. "#s"..string.sub(g_ColorMgr:GetColor("自动寻路鼠标单击"), 3, -1)


local function SetKeyHypeLink(questname, str, DownColor)
	local QuestHypeLink = {}
	if Quest_Common(questname, "超链接") then
		local Keys = Quest_Common:GetKeys(questname, "超链接")
		for i=table.getn(Keys),1,-1 do
			local Subject = Quest_Common(questname, "超链接", Keys[i], "Subject")
			local HyperLink = Quest_Common(questname, "超链接", Keys[i], "HyperLink")
			if Subject~="" and HyperLink~=0 and QuestPos_Common(HyperLink) then
				local SceneName = QuestPos_Common(HyperLink, "SceneName")
				local PosX = QuestPos_Common(HyperLink, "PosX")
				local PosY = QuestPos_Common(HyperLink, "PosY")
				local Mark = QuestPos_Common(HyperLink, "Mark")
				local FindStr = "%b{"..Subject.."}"
				local sTblInfo = string.match(str,FindStr)
				if sTblInfo then
					local RealName = Mark or ""
					local num = string.find(sTblInfo,",")
					local strHipeLink = string.sub(sTblInfo,2,num-1)
					local linkstr = LinkNpcBaseColor.."#u#l"..LinkNpcColor..strHipeLink.."#i[".. MemH64(RealName) .."]#l#u#W"..DownColor
					str = string.gsub(str, FindStr, linkstr)
					if not QuestHypeLink[RealName] then
						local tbl = {SceneName, PosX ,PosY}
						QuestHypeLink[RealName] = tbl
					end
				end
			end
		end
	end
	return str,QuestHypeLink
end

--物品的超链接
local function SetItemHypeLink(questname, str, DownColor)
	local ItemTbl = {}
	if Quest_Common(questname, "物品超链接") then
		local Keys = Quest_Common:GetKeys(questname, "物品超链接")
		for i=table.getn(Keys),1,-1 do
			local subject = Quest_Common(questname, "物品超链接", Keys[i], "Subject")
			local itemlink = GetCfgTransformValue(false, "Quest_Common", questname, "物品超链接", Keys[i], "Arg")
			if subject~="" and itemlink[1] then
				local FindStr = "%b{"..subject.."}"
				local sTblInfo = string.match(str,FindStr)
				if sTblInfo then
					local num = string.find(sTblInfo,",")
					local strHipeLink = string.sub(sTblInfo,2,num-1)
					str = string.gsub(str,FindStr, LinkItemBaseColor.."#u#l"..LinkItemColor..strHipeLink.."#i[".. MemH64(itemlink[2]) .."]#l#u#W"..DownColor)
					ItemTbl[itemlink[2]] = itemlink[1]
				end
			end
		end
	end
	return str,ItemTbl
end

local function SetPos(wnd, x, y)
	local Rect = CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x+uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y+uHeight
	end
	wnd:SetLogicRect(Rect)
end

function CreateQuestTraceBack(Parent)
	local Wnd = CQuestTraceBackWnd:new()
	Wnd:CreateFromRes("QuestTrackBackWnd",Parent)
	--Wnd.m_QuestBeginIndex = 0  --追踪任务面板上起始任务对应任务列表的索引
	--Wnd.m_QuestEndIndex = 0
	Wnd.m_ActionBeginIndex = 0  --追踪玩法面板上起始任务对应任务列表的索引
	Wnd.m_ActionEndIndex = 0
	Wnd.m_AreaName = nil --表示追踪某个区域的任务
	Wnd.m_TrackType = 1 ---追踪类型，1代表追踪任务，2代表追踪玩法，默认1
	Wnd:SetIsBottom(true)
	Wnd.m_NowActionTrackTbl = {}
	--Wnd.m_NowQuestTrackTbl = {}	--任务追踪信息的tbl   self.m_NowQuestTrackTbl[任务名][任务对象名].Num(值为需要的数目).TrackInfo(追踪面板显示的信息
	Wnd.m_EndActionNotifyTickTbl = {}
	Wnd.m_WaitingLevelActionName = {}
	Wnd.m_ItemTbl = {}
	Wnd.m_MaxPage = 0
	Wnd.m_NowPage = 0
	Wnd.m_TrackTypeName = m_ZlTblName
	--Wnd:InitTrack()
	
	local rt = CFRect:new()
	Wnd.m_TrackListCtrl:GetLogicRect(rt)
	
	local rt_1 = CFRect:new()
	Wnd.m_NextPageBtn:GetLogicRect(rt_1)
	local width1 = Wnd.m_NextPageBtn:GetWndOrgWidth()
	local height1 = Wnd.m_NextPageBtn:GetWndOrgHeight()
	rt_1.bottom = rt.bottom
	rt_1.top = rt_1.bottom - height1
	rt_1.right = rt.right
	rt_1.left = rt_1.right - width1
	Wnd.m_NextPageBtn:SetLogicRect(rt_1)
	
	local rt_2 = CFRect:new()
	Wnd.m_LastPageBtn:GetLogicRect(rt_2)
	local width2 = Wnd.m_LastPageBtn:GetWndOrgWidth()
	local height2 = Wnd.m_LastPageBtn:GetWndOrgHeight()
	rt_2.bottom = rt.bottom
	rt_2.top = rt_2.bottom - height2
	rt_2.right = rt_1.left
	rt_2.left = rt_2.right - width2
	Wnd.m_LastPageBtn:SetLogicRect(rt_2)
	
	return Wnd
end

function CQuestTraceBackWnd:OnChildCreated()
	self.m_TrackListCtrl	= self:GetDlgChild("TrackListCtrl")
	
	self.m_QuestBtn = {}
	self.m_QuestText = {}
	for i=1, 5 do
		self.m_QuestBtn[i] = self:GetDlgChild("QuestBtn"..i)
		self.m_QuestText[i] = self:GetDlgChild("QuestText"..i)
		self.m_QuestBtn[i]:ShowWnd(false)
		self.m_QuestText[i]:ShowWnd(false)
	end
	
	self.m_LastPageBtn		= self:GetDlgChild("LastPageBtn")
	self.m_NextPageBtn		= self:GetDlgChild("NextPageBtn")
	
	self.m_LastPageBtn:ShowWnd(false)
	self.m_NextPageBtn:ShowWnd(false)
	--self.m_LastPageBtn:SetMouseOverDescAfter("上翻")
	--self.m_NextPageBtn:SetMouseOverDescAfter("下翻")
end

function ClearEndActionNotifyTick()
--print("function ClearEndActionNotifyTick()")
	for ActionName, TickTbl in pairs(g_GameMain.m_QuestTraceBack.m_EndActionNotifyTickTbl) do
		for i = 1,#(TickTbl) do
			UnRegisterTick(TickTbl[i])
		end
	end
	g_GameMain.m_QuestTraceBack.m_EndActionNotifyTickTbl = nil
end

function CQuestTraceBackWnd:EndNotifyAction(Tick, ActionName,EndInfo)
	UnRegisterTick(self.m_EndActionNotifyTickTbl[ActionName][1])
	table.remove(self.m_EndActionNotifyTickTbl[ActionName],1)
	table.remove(self.m_NowActionTrackTbl[ActionName],1)
	self.m_WaitingLevelActionName[ActionName] = nil
	if self:IsShow() and self.m_TrackType == 2 then 
		self:ShowActionTraceList()
	end
	-----------------------
	--活动结束时,显示滚动信息
	if EndInfo then
		DisplayMsg(2,EndInfo)
		SysRollAreaMsg(EndInfo)
	end
	-----------------------
end

function CQuestTraceBackWnd:CheckIsInActionTrackTbl(ActionName,starthour,startmin)
	for i , timetbl in pairs(self.m_NowActionTrackTbl[ActionName]) do
		if timetbl[1] == starthour and timetbl[2] == startmin then
			return true
		end
	end
	return false
end

function CQuestTraceBackWnd:AddActionTrack(ActionName,starthour,startmin)
	if not self.m_NowActionTrackTbl[ActionName] then
		self.m_NowActionTrackTbl[ActionName] = {}
	end
	
	if not self:CheckIsInActionTrackTbl(ActionName,starthour,startmin) then
		table.insert(self.m_NowActionTrackTbl[ActionName],{starthour,startmin})
		if self:IsShow() and self.m_TrackType == 2 then
			self:ShowActionTraceList()
		end
	end
end

function CQuestTraceBackWnd:CalculateAddMinTime(ihour,imin,AddMin)

	local endhour = math.floor((imin+AddMin)/60)+ihour
	local endmin = (imin+AddMin)%60
	
	if endhour >= 24 then
		endhour = endhour%24
	end
	
	return endhour,endmin
end

function CQuestTraceBackWnd:GetNotifyTime(AdvanceNotifyMin,starthour,startmin,dismin)
	local nowtime = os.time()
	local nowdate = os.date("*t")
	local nowhour = nowdate.hour
	local nowmin = nowdate.min
	
	nowdate.hour = starthour
	nowdate.min = startmin
	local starttime = os.time(nowdate)
	local addtime = (AdvanceNotifyMin+dismin)*60
	local notifytime = starttime-AdvanceNotifyMin*60
	if nowtime < notifytime or nowtime >= (notifytime+addtime) then
		local notifydate = os.date("*t",notifytime)
		return 1,{notifydate.hour,notifydate.min}
	end
	return 2,notifytime+addtime-nowtime
end

local function GetIntNumber(Number)
	if Number > 0 then
		return math.floor(Number)
	else
		--math.ceil(Number)
		return math.floor(Number)
	end
end

function CQuestTraceBackWnd:GetChangeNotifyTime(DayTbl,startHour,startMin,AdvanceTime)
	local newtime = startHour * 60 + startMin
	newtime = newtime - AdvanceTime
	local iDayNum = 0
	
	local Hour = GetIntNumber(newtime / 60)
	local Min = newtime - (Hour * 60)
	iDayNum = GetIntNumber(Hour / 24)
	Hour = Hour - iDayNum * 24
	
	if Hour < 0 then
		local CountMin = 24*60 + (Hour * 60 + Min)
		Hour = GetIntNumber(CountMin / 60)
		Min = CountMin - (Hour * 60)
	end
	
	local iDayTbl = nil
	if DayTbl and IsTable(DayTbl) and iDayNum ~= 0 then
		iDayTbl = {}
		for i=1, #(DayTbl) do
			local currDay = DayTbl[i] + iDayNum
			local WeekNum = GetIntNumber(currDay / 7)
			if currDay <= 0 then
				currDay = ((-7 * WeekNum) + currDay) + 7
			else
				currDay = currDay - (7 * WeekNum)
			end
			currDay = currDay==0 and 7 or currDay
			table.insert(iDayTbl, currDay)
		end
	else
		iDayTbl = DayTbl
	end
	
	return iDayTbl, {Hour,Min}
end

local function BeforeEndNotifyAction(notifyInfo)
	for Name, _ in pairs(g_GameMain.m_FbActionPanel.m_RemindAction)do
		if Name == ActionName then
			if notifyInfo then
				DisplayMsg(2,notifyInfo)
				SysRollAreaMsg(notifyInfo)
			end
		end
	end
end

local function StartNotifyAction(Arg1,Arg2)
	local ActionName = nil
	local endTickTime = 0
	local starthour = 0
	local startmin = 0
	local notifyInfo = nil
	local endInfo = nil
	local checkInfo = nil
	if Arg2 then
		ActionName = Arg2[1]
		endTickTime = Arg2[2]
		starthour = Arg2[3]
		startmin = Arg2[4]
		notifyInfo = Arg2[5]
		endInfo = Arg2[6]
	else
		ActionName = Arg1[1]
		local AdvanceNotifyMin = FbActionDirect_Common(ActionName, "AdvanceNotifyMin")
		local ContinualMin = FbActionDirect_Common(ActionName, "ContinualMin")
		endTickTime = (ContinualMin+AdvanceNotifyMin)*60*1000
		starthour = Arg1[2]
		startmin = Arg1[3]
		notifyInfo = Arg1[4]
		endInfo = Arg1[5]
		checkInfo = Arg1[6]
		-----------------------
		--活动开始时,显示滚动信息
		if notifyInfo then
			DisplayMsg(2,notifyInfo)
			SysRollAreaMsg(notifyInfo)
		end
		-----------------------
		
		-- 活动开始前提示 g_GameMain.m_FbActionPanel.m_RemindAction libing
		for Name, _ in pairs(g_GameMain.m_FbActionPanel.m_RemindAction)do
			if Name == ActionName then
				--print(Name.."马上开始，速度报名！")
				if checkInfo then
					DisplayMsg(2,checkInfo)
					SysRollAreaMsg(checkInfo)
				end
			end
		end
		
	end
	
	if not g_GameMain.m_QuestTraceBack.m_EndActionNotifyTickTbl[ActionName] then
		g_GameMain.m_QuestTraceBack.m_EndActionNotifyTickTbl[ActionName] = {}
	end
	g_GameMain.m_QuestTraceBack:AddActionTrack(ActionName,starthour,startmin)
	local tick = RegClassTick("EndActionNotifyTick", g_GameMain.m_QuestTraceBack.EndNotifyAction, endTickTime, g_GameMain.m_QuestTraceBack,ActionName,endInfo)
	table.insert(g_GameMain.m_QuestTraceBack.m_EndActionNotifyTickTbl[ActionName],tick)
end

--活动滚动信息的显示
local function ShowActionInfoFun(WeekDayTbl,starthour,startmin,ShowInfo)
	local function ShowInfoFun(Info)
		-----------------------
		--活动开始时,显示滚动信息
		if Info then
			DisplayMsg(2,Info)
			SysRollAreaMsg(Info)
		end
		-----------------------
	end
	if ShowInfo then
		local date = {time = {starthour..":"..startmin},wday = WeekDayTbl}
		g_AlarmClock:AddTask("滚动信息的显示", date, ShowInfoFun,nil,ShowInfo)
	end
end

--参数:IntervalTime(如果大于0,表示提前多少时间;如果小于0,表示落后多长时间发消息)
function CQuestTraceBackWnd:ActionProgressInfo(DateTbl, StarHour, StarMin, CountTime, IntervalTime, ShowInfo)
	if IntervalTime and IntervalTime ~= 0
		and IntervalTime < CountTime and (IntervalTime > (-1)*CountTime) then
		
		local times = GetIntNumber(CountTime/IntervalTime)
		times = times>0 and times or (times*(-1))
		
		local localDayTbl = nil
		local localTimeTbl = nil
		if IntervalTime > 0 then--活动开始前的提示
			for i=1,(times-1) do
				localDayTbl, localTimeTbl = self:GetChangeNotifyTime(DateTbl.wday,StarHour,StarMin,(CountTime-(IntervalTime * i)))
				ShowActionInfoFun(localDayTbl,localTimeTbl[1],localTimeTbl[2],ShowInfo)
			end
			if (IntervalTime * times) < CountTime then
				localDayTbl, localTimeTbl = self:GetChangeNotifyTime(DateTbl.wday,StarHour,StarMin,(CountTime-(IntervalTime * times)))
				ShowActionInfoFun(localDayTbl,localTimeTbl[1],localTimeTbl[2],ShowInfo)
			end
		else--活动开始后的提示
			for i=1,(times-1) do
				localDayTbl, localTimeTbl = self:GetChangeNotifyTime(DateTbl.wday,StarHour,StarMin,(IntervalTime * i))
				ShowActionInfoFun(localDayTbl,localTimeTbl[1],localTimeTbl[2],ShowInfo)
			end
			if (IntervalTime * times * -1) < CountTime then
				localDayTbl, localTimeTbl = self:GetChangeNotifyTime(DateTbl.wday,StarHour,StarMin,(IntervalTime * times))
				ShowActionInfoFun(localDayTbl,localTimeTbl[1],localTimeTbl[2],ShowInfo)
			end
		end
		
	end
end

function CQuestTraceBackWnd:InitNotifyActionTick()
	if g_GameMain.m_ShowTraceInfo then
		return
	else
		g_GameMain.m_ShowTraceInfo = true
	end
		
	for _, ActionName in pairs(FbActionDirect_Common:GetKeys()) do
		local ContinualMin = FbActionDirect_Common(ActionName, "ContinualMin")
		local CheckInfoTbl = GetCfgTransformValue(true,"Lan_FbActionDirect_Common", MemH64(ActionName), "CheckAdvanceNotifyInfo")
		local NotifyInfoTbl = GetCfgTransformValue(true,"Lan_FbActionDirect_Common", MemH64(ActionName), "AdvanceNotifyInfo")
		local BeforeEndInfoTbl = GetCfgTransformValue(true,"Lan_FbActionDirect_Common", MemH64(ActionName), "BeforeEndInfo")
		local StarInfoTbl = GetCfgTransformValue(true,"Lan_FbActionDirect_Common", MemH64(ActionName), "ActionStarInfo")
		local StarProcessInfoTbl = GetCfgTransformValue(true,"Lan_FbActionDirect_Common", MemH64(ActionName), "StarProcessInfo")
		local EndInfoTbl = GetCfgTransformValue(true,"Lan_FbActionDirect_Common", MemH64(ActionName), "ActionEndInfo")
		local DateTbl = GetCfgTransformValue(true,"FbActionDirect_Common", ActionName, "StartTime")
		local IntervalTime =GetCfgTransformValue(false,"FbActionDirect_Common", ActionName, "IntervalTime")
		
		local TimeTbl = DateTbl.time
		if not TimeTbl or not IsTable(TimeTbl) then
			--全天开放的活动,在进行中的滚动信息的提示
			if IntervalTime[2] and IsNumber(IntervalTime[2]) then
				local strInfo = nil
				if StarProcessInfoTbl[1] then
					strInfo = StarProcessInfoTbl[1][1]
				end
				if strInfo then
					self:ActionProgressInfo(DateTbl,0,0, 1440, (-1)*IntervalTime[2], strInfo)
				end
			end
			-- 不限时的活动
			g_GameMain.m_FbActionPanel:AddActionInfo(ActionName,0, 0, 23, 59)
		else
			
			for i = 1 , #(TimeTbl) do
				local timestr = TimeTbl[i]
				local strlen = string.len(timestr)
				local colon_i = string.find(timestr,":")
				local starthour = tonumber(string.sub(timestr,1,colon_i-1))
				local startmin = tonumber(string.sub(timestr,colon_i+1,strlen))
				local endhour,endmin = self:CalculateAddMinTime(starthour,startmin,ContinualMin)
				
				local checkInfo = nil
				local notifyInfo = nil
				local starInfo = nil
				local starProcessInfo = nil
				local beforeEndInfo = nil
				local endInfo = nil
				
				if CheckInfoTbl[i] then
					checkInfo = CheckInfoTbl[i][1]
				end
				if NotifyInfoTbl[i] then
					notifyInfo = NotifyInfoTbl[i][1]
				end
				if StarInfoTbl[i] then
					starInfo = StarInfoTbl[i][1]
				end
				if StarProcessInfoTbl[i] then
					starProcessInfo = StarProcessInfoTbl[i][1]
				end
				if BeforeEndInfoTbl[i] then
					beforeEndInfo = BeforeEndInfoTbl[i][1]
				end
				if EndInfoTbl[i] then
					endInfo = EndInfoTbl[i][1]
				end
				
				--=活动开始前的消息=============================================
				--活动开始的滚动信息的提示
				ShowActionInfoFun(DateTbl.wday,starthour,startmin,starInfo)
				--活动进行中的滚动信息的提示
				if IntervalTime[2] and IsNumber(IntervalTime[2]) then
					self:ActionProgressInfo(DateTbl,starthour,startmin, ContinualMin, (-1)*IntervalTime[2], starProcessInfo)
				end
				--============================================================
				
				
				--=活动开始前的消息=============================================
				--第一次提示活动要开始的消息(提前发的活动信息)
				local AdvanceNotifyMin = FbActionDirect_Common(ActionName, "AdvanceNotifyMin")
				
				local iDayTbl, iTimeTbl = self:GetChangeNotifyTime(DateTbl.wday,starthour,startmin,AdvanceNotifyMin)
				local date = {time = {iTimeTbl[1]..":"..iTimeTbl[2]},wday = iDayTbl}
				g_AlarmClock:AddTask("每日线上活动提示！", date, StartNotifyAction,nil,{ActionName,starthour,startmin,notifyInfo,endInfo,checkInfo},nil)
				
				--=====活动结束前的消息====活动信息面板libing===============================================

				local BeforeEndNotifyMin = FbActionDirect_Common(ActionName, "BeforeEndNotifyMin")
				
				local iEndDayTbl, iEndTimeTbl = self:GetChangeNotifyTime(DateTbl.wday,endhour,endmin,BeforeEndNotifyMin)
				local date = {time = {iEndTimeTbl[1]..":"..iEndTimeTbl[2]},wday = iEndDayTbl}
				g_AlarmClock:AddTask("活动结束前消息提示！", date, BeforeEndNotifyAction,nil,beforeEndInfo)

				
				--多次提示活动消息(提前发的活动信息)
				if IntervalTime[1] and IsNumber(IntervalTime[1]) then
					self:ActionProgressInfo(DateTbl,starthour,startmin, AdvanceNotifyMin, IntervalTime[1], notifyInfo)
				end
				--============================================================
				
				local IsBreak = false
				if iDayTbl and IsTable(iDayTbl) and next(iDayTbl) then
					local nowtime = os.time()
					local nowdate = os.date("*t",nowtime)
					local weekday = ((nowdate.wday == 1) and 7 or nowdate.wday-1)--今天
					
					local iCommpare = iDayTbl[1] - DateTbl.wday[1]
					
					local TempDay = weekday
					if iCommpare < 0 then
						TempDay = ((weekday == 1) and 7 or weekday-1)--昨天
					elseif iCommpare > 0 then
						TempDay = ((weekday == 7) and 1 or weekday+1)--明天
					end
					
					for i=1,#(iDayTbl) do
						if iDayTbl[i] == weekday or iDayTbl[i] == TempDay then
							IsBreak = true
							break
						end
					end
				else
					IsBreak = true
				end
				
				if IsBreak then
					local type,arg = self:GetNotifyTime(AdvanceNotifyMin,starthour,startmin,ContinualMin)
					if type ~= 1 then
						--StartNotifyAction(nil,{ActionName,arg*1000,starthour,startmin,notifyInfo,endInfo})
					end
				end
				
				-- 增加到活动信息面板 libing
				g_GameMain.m_FbActionPanel:AddActionInfo(ActionName,starthour, startmin, endhour, endmin, DateTbl.wday)
			end
			
		end
	end
end

local function CreateQuestNameItemWnd(Parent, questname, width)
	local Wnd = CQuestNameItemWnd:new()
	Wnd:CreateFromRes( "QuestTrackNameWnd", Parent )
	Wnd:SetIsBottom(true)
	Wnd:SetWndWidth(width)
	
	Wnd.m_QuestNameBtn = Wnd:GetDlgChild("QuestNameBtn")
	Wnd.m_QuestName = questname
	
	Wnd.m_RetQuestRecordBtn = Wnd:GetDlgChild("RetQuestRecordBtn")
	local width1 = Wnd.m_RetQuestRecordBtn:GetWndOrgWidth()
	
	Wnd.m_RetToAreaWndBtn = Wnd:GetDlgChild("RetToAreaWndBtn")
	local width2 = Wnd.m_RetToAreaWndBtn:GetWndOrgWidth()
	
	local rt = CFRect:new()
	Parent.m_TrackListCtrl:GetLogicRect(rt)
	
	local rt_1 = CFRect:new()
	Wnd.m_RetQuestRecordBtn:GetLogicRect(rt_1)
	rt_1.right = rt.right - 2
	rt_1.left = rt_1.right - width1
	Wnd.m_RetQuestRecordBtn:SetLogicRect(rt_1)
	Wnd.m_RetQuestRecordBtn:SetMouseOverDescAfter(ToolTipsText(509))
	Wnd.m_RetQuestRecordBtn:ShowWnd(false)
	
	
	local rt_2 = CFRect:new()
	Wnd.m_RetToAreaWndBtn:GetLogicRect(rt_2)
	rt_2.right = rt_1.left - 2
	rt_2.left = rt_2.right - width2
	Wnd.m_RetToAreaWndBtn:SetLogicRect(rt_2)
	Wnd.m_RetToAreaWndBtn:SetMouseOverDescAfter(ToolTipsText(508))
	Wnd.m_RetToAreaWndBtn:ShowWnd(false)
	
	Wnd.m_QuestNameBtn:SetWndWidth(width)
	Wnd.m_QuestNameBtn:SetWndHeight(QuestNameHeight)
	
	Wnd:ShowWnd(false)
	return Wnd
end

function CQuestNameItemWnd:SetText(str, questlevel)
	local questname = self.m_QuestName
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	if g_AllMercenaryQuestMgr[questname] then
		self.m_QuestNameBtn:SetWndTextColor(Flag,YbColor)
	elseif g_DareQuestMgr[questname] then
		self.m_QuestNameBtn:SetWndTextColor(Flag,TzColor)
	elseif g_ActionQuestMgr[questname] then
		self.m_QuestNameBtn:SetWndTextColor(Flag,WfColor)
	else
		--任务的等级太低了,就用灰色
		if g_MainPlayer:CppGetLevel() - questlevel >= 3 then
			self.m_QuestNameBtn:SetWndTextColor(Flag,TempColor)
			str = GetStaticTextClient(8339)..str
		else
			self.m_QuestNameBtn:SetWndTextColor(Flag,ZxColor)
		end
	end
	self.m_QuestNameBtn:SetWndText(str)
end

local function CreateQuestTrackInfoItemWnd(Parent, questname, width)
	local Wnd = CQuestTrackInfoItemWnd:new()
	Wnd:CreateFromRes( "QuestTrackBackItemWnd", Parent )
	Wnd:SetIsBottom(true)
	Wnd.m_QuestInfoText = Wnd:GetDlgChild("QuestInfoText")
	Wnd.m_QuestInfoText:SetStyle(0x40000008)
	local InfoTextWidth = Wnd.m_QuestInfoText:GetWndOrgWidth()
	Wnd.m_QuestInfoText:SetAutoWidth(InfoTextWidth, InfoTextWidth)
	Wnd.m_QuestName = questname
	Wnd:SetWndWidth(width)
	Wnd:ShowWnd(false)
	return Wnd
end

function CQuestTrackInfoItemWnd:SetText(str)
	self.m_QuestInfoText:SetWndText(str)
	local textheight = self.m_QuestInfoText:GetWndOrgHeight()
	self:SetWndHeight(textheight)
end

--显示有那几个功能的按扭
function CQuestTraceBackWnd:GetShowBtnWnd()
	local ShowTbl = {false,false,false,false}
	local QusetInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
	if QusetInfoTbl then
		for QuestName,_ in pairs(QusetInfoTbl) do
			
			local QuestType = m_ZxTblName
			if g_AllMercenaryQuestMgr[QuestName] then
				QuestType = m_YbTblName
				ShowTbl[2] = m_YbTblName
			elseif g_DareQuestMgr[QuestName] then
				QuestType = m_TzTblName
				ShowTbl[3] = m_TzTblName
			elseif g_ActionQuestMgr[QuestName] then
				QuestType = m_WfTblName
				ShowTbl[4] = m_WfTblName
			else
				ShowTbl[1] = m_ZxTblName
			end
			
		end
	end
	self:IsShowBtnWnd(ShowTbl)
end

--显示按扭
function CQuestTraceBackWnd:IsShowBtnWnd(ShowTbl)
	local btnNum = 1
	for i=1, #(ShowTbl) do
		local TblName = ShowTbl[i]
		if TblName then
			btnNum = btnNum + 1
			self.m_QuestText[btnNum]:SetWndText(TblName)
			self.m_QuestText[btnNum].m_TextName = TblName
			self.m_QuestBtn[btnNum]:ShowWnd(true)
			self.m_QuestText[btnNum]:ShowWnd(true)
		end
	end
	--如果有一个按扭显示,就显示"总览"按扭
	if btnNum >= 1 then
		self.m_QuestText[1]:SetWndText(m_ZlTblName)
		self.m_QuestText[1].m_TextName = m_ZlTblName
		self.m_QuestBtn[1]:ShowWnd(true)
		self.m_QuestText[1]:ShowWnd(true)
	else
		self.m_TrackTypeName = m_ZlTblName
		self.m_QuestBtn[1]:ShowWnd(false)
		self.m_QuestText[1]:ShowWnd(false)
	end
	--不显示的按扭
	for i=btnNum+1, 5 do
		self.m_QuestBtn[i]:ShowWnd(false)
		self.m_QuestText[i]:ShowWnd(false)
	end
end

local function SortTypeFun(QuestTbl)
	local tbl = {{},{},{},{}}
	for i=1, #(QuestTbl) do
		if g_AllMercenaryQuestMgr[QuestTbl[i]] then
			table.insert(tbl[1], QuestTbl[i])
		elseif g_DareQuestMgr[QuestTbl[i]] then
			table.insert(tbl[4], QuestTbl[i])
		elseif g_ActionQuestMgr[QuestTbl[i]] then
			table.insert(tbl[3], QuestTbl[i])
		else
			table.insert(tbl[2], QuestTbl[i])
		end
	end
	
	QuestTbl = tbl[1]
	for i=2, #(tbl) do
		for j=1, #(tbl[i]) do
			table.insert(QuestTbl, tbl[i][j])
		end
	end
	return QuestTbl
end

function CQuestTraceBackWnd:ReorderItemTbl()
	local ItemTbl = {}
	local TrackTbl = {}
	local ShowTbl = {false,false,false,false}
	local SortType = self.m_SortType
	
	local QusetInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
	if QusetInfoTbl then
		for QuestName, _ in pairs(QusetInfoTbl) do
			if g_AllMercenaryQuestMgr[QuestName] then
				ShowTbl[2] = m_YbTblName
			elseif g_DareQuestMgr[QuestName] then
				ShowTbl[3] = m_TzTblName
			elseif g_ActionQuestMgr[QuestName] then
				ShowTbl[4] = m_WfTblName
			else
				ShowTbl[1] = m_ZxTblName
			end
			table.insert(TrackTbl, QuestName)
		end
	end
	
	for i=1, #(TrackTbl) do	--按时间排序
		for k=i+1, #(TrackTbl) do
			local questname1 = TrackTbl[i]
			local questname2 = TrackTbl[k]
			local accepttime1 = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname1].m_AcceptTime
			local accepttime2 = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname2].m_AcceptTime
			local QuestName = ""
			if accepttime1 > accepttime2 then
				QuestName = TrackTbl[i]
				TrackTbl[i] = TrackTbl[k]
				TrackTbl[k] = QuestName
			end
		end
	end
	
	if SortType == 1 then--按等级排序
		for i=1, #(TrackTbl) do
			for j=i+1, #(TrackTbl) do
				local QuestName1 = TrackTbl[i]
				local QuestName2 = TrackTbl[j]
				local Subject1 = Quest_Common( QuestName1, "任务等级" )
				local Subject2 = Quest_Common( QuestName2, "任务等级" )
				local Level1 = g_RetNoneMercSentenceParse(Subject1)
				local Level2 = g_RetNoneMercSentenceParse(Subject2)
				local QuestName = ""
				if Level1 > Level2 then
					QuestName = TrackTbl[i]
					TrackTbl[i] = TrackTbl[j]
					TrackTbl[j] = QuestName
				end
			end
		end
	elseif SortType == 2 then--按类型排序
		TrackTbl = SortTypeFun(TrackTbl)
	elseif SortType == 3 then--按区域排序
		local AreaTbl = {}
		for i=1, #(TrackTbl) do
			local QuestName = TrackTbl[i]
			if Quest_Common(QuestName, "所属区域")
				and Quest_Common(QuestName, "所属区域", "1")
				and Quest_Common(QuestName, "所属区域", "1") ~= ""
				then
					local area = Quest_Common(QuestName, "所属区域", "1")
					if not AreaTbl[area] then
						AreaTbl[area] = {}
					end
					table.insert(AreaTbl[area], QuestName)
			else
				if not AreaTbl["NoneArea"] then
					AreaTbl["NoneArea"] = {}
				end
				table.insert(AreaTbl["NoneArea"], QuestName)
			end
		end
		
		TrackTbl = {}
		for _, tempTbl in pairs(AreaTbl) do
			if tempTbl and next(tempTbl) then
				tempTbl = SortTypeFun(tempTbl)
				for i=1, #(tempTbl) do
					table.insert(TrackTbl, tempTbl[i])
				end
			end
		end
	end
	
	for i=1, table.maxn(self.m_ItemTbl) do
		if self.m_ItemTbl[i] then
			local QuestName = self.m_ItemTbl[i].m_QuestName
			ItemTbl[QuestName] = self.m_ItemTbl[i]
		end
	end
	self.m_ItemTbl = {}
	for i=1, #(TrackTbl) do
		local QuestName = TrackTbl[i]
		table.insert(self.m_ItemTbl, ItemTbl[QuestName])
	end
	
	self:IsShowBtnWnd(ShowTbl)
end

--在任务记录面板上点击“任务追踪”按钮
function CQuestTraceBackWnd:ClickTrackBtn(QuestName)
	for i=1, table.getn(self.m_ItemTbl) do
		if self.m_ItemTbl[i].m_QuestName == QuestName then
			if self.m_ItemTbl[i].m_Page >= 0 then
				self.m_ItemTbl[i].m_Page = -1	--设成-1的值之后除非再次点击否则不追踪
			else
				self.m_ItemTbl[i].m_Page = 1	--随便设个大于0的值之后重新排序分页
			end
			break
		end
	end
	self:UpdateItemTbl(nil, QuestName)
end

--重新排序任务列表
function CQuestTraceBackWnd:ResetSortQuestList()
	if IsFunction(self.IsShow) and self:IsShow() then
		if self.m_TrackType == 1 then
			self:UpdateItemTbl(true)
		end
	end
end

function CQuestTraceBackWnd:AcceptQuest(QuestName)
	self:CreateQuestItem(QuestName)
	self:GetShowBtnWnd()
end

function CQuestTraceBackWnd:QuestVarNumChange(QuestName)
	if not self:IsShow() then
		return
	end
	self:UpdateItemTbl(nil, QuestName)
end

function CQuestTraceBackWnd:ShowTrackByQuestName(QuestName)
	self.m_TrackTypeName = m_ZlTblName
	self:UpdateItemTbl(nil, QuestName)
end

--删除任务追踪面板中的任务
function CQuestTraceBackWnd:RemoveFromQuestTbl(QuestName)
	self:DeleteQuestItem(QuestName)
	self:UpdateItemTbl(true, QuestName)
	self:GetShowBtnWnd()
end

function CQuestTraceBackWnd:CreateQuestItem(QuestName)
	local itemwidth = self.m_TrackListCtrl:GetWndOrgWidth()
	local tbl = {}
	tbl.m_NameItem = CreateQuestNameItemWnd(self, QuestName, itemwidth)
	tbl.m_InfoItem = CreateQuestTrackInfoItemWnd(self, QuestName, itemwidth)
	tbl.m_Height = 0
	tbl.m_Page = 1
	tbl.m_QuestName = QuestName
	table.insert( self.m_ItemTbl, tbl )
	
	self:UpdateItemTbl(true, QuestName)
end

function CQuestTraceBackWnd:DeleteQuestItem(QuestName)
	for i=1, table.getn(self.m_ItemTbl) do
		if self.m_ItemTbl[i].m_QuestName == QuestName then
			self.m_ItemTbl[i].m_NameItem:ShowWnd(false)
			self.m_ItemTbl[i].m_InfoItem:ShowWnd(false)
			self.m_ItemTbl[i].m_NameItem = nil
			self.m_ItemTbl[i].m_InfoItem = nil
			self.m_ItemTbl[i].m_Page = nil
			self.m_ItemTbl[i].m_Height = nil
			self.m_ItemTbl[i].m_QuestName = nil
			self.m_ItemTbl[i] = nil
			break
		end
	end
end

function CQuestTraceBackWnd:SetItemTbl(ItemNum)
	local tbl = self.m_ItemTbl[ItemNum]
	local QuestName = tbl.m_QuestName
	local needinfo = g_QuestNeedMgr[QuestName] or g_BuffQuestMgr[QuestName]
	local queststr = TrackInfoColorStr
	local namestr = ""
	local playerlevel = g_MainPlayer:CppGetLevel()
	local questlevel = 1
	local questtype = Quest_Common(QuestName, "任务类型")
	
	if needinfo ~= nil then
		local Keys = Quest_Common:GetKeys(QuestName)
		local SortQuestVarMgr = g_SortQuestVarMgr[QuestName]
		for NeedIndex=1, table.getn(Keys) do
			local NeedName = Keys[NeedIndex]
			
			if string.find(NeedName, "需求") then
				if NeedName ~= "交任务状态需求" then
					for VarIndex=1, #(SortQuestVarMgr[NeedName]) do
						local VarName = SortQuestVarMgr[NeedName][VarIndex]
						--这里获得当前任务变量的数目
						if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName] then
							local num = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName][VarName].DoNum
							local NeedNum = needinfo[VarName].Num
							if num < NeedNum then
								local AchieveStateStr = "#R" .. num .."/" .. NeedNum .."  #r"
								local trackstr = needinfo[VarName].TrackInfo
								if IsTable(trackstr) then
									trackstr = needinfo[VarName].TrackInfo[1]
								end
								queststr = queststr..trackstr..AchieveStateStr
							end
						end
					end
				else
					local BuffQuestMgr = g_BuffQuestMgr[QuestName]
					if BuffQuestMgr ~= nil then
						for buffname,Tbl in pairs(BuffQuestMgr) do
							local TrackInfo = Tbl.TrackInfo
							local NeedNum = Tbl.Num
							if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
								local num = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName][buffname].DoNum
								if num < NeedNum then
									queststr = queststr .. TrackInfo .."#R" .. num .."/" .. NeedNum .."  #r"
								end
							end
						end
					end
				end
			end
			
		end
	end
	
	local TempStr, TempQuestLink = SetKeyHypeLink(QuestName, queststr, TrackInfoColorStr)
	queststr = TempStr
	for KeyName, Tbl in pairs(TempQuestLink) do
		if not self.m_QuestHypeLink[QuestName..KeyName] then
			self.m_QuestHypeLink[QuestName..KeyName] = Tbl
		end
	end
	
	local name = QuestName
	local lannamestr = ""
	if questtype == 10 then
		local tbl = GetCfgTransformValue(true, "Quest_Common", QuestName, "环属性", "1")
		local loopname = tbl[1]
		local loopnum = g_GameMain.m_QuestRecordWnd.m_LoopTbl[loopname]
		name = g_LoopQuestMgr[loopname][1][1].QuestName
		lannamestr = g_GetLanQuestName(name) .. GetStaticTextClient(8344, loopnum)
	else
		lannamestr = g_GetLanQuestName(name)
	end
		
	if g_AllMercenaryQuestMgr[name] then
		local level2 = Quest_Common(name, "接任务最小佣兵等级")
		if level2 and level2~="" then
			level2 = g_GetMercenaryLevelLanName(level2)
			namestr = "("..level2..")"
		end
	end
	local level1 = Quest_Common(name, "任务等级")
	questlevel = g_RetNoneMercSentenceParse(level1)
	namestr = namestr.."("..questlevel..")"
	
	if g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName].m_State == QuestState.failure
		or questtype == 9 then
		namestr = namestr .. lannamestr .. "(" .. GetStaticTextClient(8311)..")"--已失败
	else
		local canfinish = true
		local playerquestvar = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName]
		if playerquestvar then
			for varname, v in pairs(playerquestvar) do
				if v.DoNum<v.NeedNum then
					canfinish = false
					break
				end
			end
		else
			canfinish = false
		end
		
		if canfinish then
			local HyperLink = Quest_Common(QuestName, "结束任务方式", "HyperLink")
			local endername = Quest_Common(QuestName, "结束任务方式", "Arg")
			local type = Quest_Common(QuestName, "结束任务方式", "Subject")
			namestr = namestr .. lannamestr
			local showName = endername
			if type == "Npc" then
				showName = GetNpcDisplayName(endername)
			elseif type == "Obj" then
				showName =  GetIntObjDispalyName(endername)
			end
			
			if HyperLink ~= "" then
				local LinkTbl = {}
				LinkTbl[1] = QuestPos_Common(HyperLink, "SceneName")
				LinkTbl[2] = QuestPos_Common(HyperLink, "PosX")
				LinkTbl[3] = QuestPos_Common(HyperLink, "PosY")
				LinkTbl[4] = QuestPos_Common(HyperLink, "Mark")
				self.m_QuestHypeLink[QuestName..endername] = LinkTbl--完成任务
				queststr = TrackInfoColorStr..GetStaticTextClient(8310)..LinkNpcBaseColor.."#u#l"..LinkNpcColor..showName.."#i[".. MemH64(endername) .."]#l#u#W#r"..TrackInfoColorStr
			else
				queststr = TrackInfoColorStr..GetStaticTextClient(8310)..showName.."#r"
			end
			if g_AllMercenaryQuestMgr[QuestName] and playerlevel > 5 and endername == "登记人杰弗里斯" then
				local strSkillName = "返回树屋酒吧"
				queststr = queststr .. "("..GetStaticTextClient(8330)..LinkSkillBaseColor.."#u#l"..LinkSkillColor..GetStaticTextClient(8331).."#i["..MemH64(strSkillName).."]#l#u#W"..GetStaticTextClient(8332)..")#r"
				self.m_QuestSkillLink[strSkillName] = 1
			end
			
		else
			namestr = namestr .. lannamestr
			local Str, TempItem = SetItemHypeLink(QuestName, queststr, TrackInfoColorStr)
			queststr = Str
			for tItemName, tType in pairs(TempItem) do
				self.m_QuestUseItem[tItemName] = tType
			end
		end
	end
	
	tbl.m_NameItem:SetText(namestr, questlevel)
	tbl.m_InfoItem:SetText(queststr)
	tbl.m_Height = QuestNameHeight + tbl.m_InfoItem.m_QuestInfoText:GetWndOrgHeight()
end

--IsOrder标识是否重新排序
--QuestName有合法值时显示这个任务排序后所在页，为nil时显示第一页
function CQuestTraceBackWnd:UpdateItemTbl(IsReorder, QuestName)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if IsReorder then
		self:ReorderItemTbl()
	end
	
	for i=1, table.getn(self.m_ItemTbl) do
		if self.m_ItemTbl[i].m_Page >= 0 then		-- -1代表不追踪
			local name = self.m_ItemTbl[i].m_QuestName
			local OpenTblName = m_ZxTblName
			if g_AllMercenaryQuestMgr[name] then
				OpenTblName = m_YbTblName
			elseif g_DareQuestMgr[name] then
				OpenTblName = m_TzTblName
			elseif g_ActionQuestMgr[name] then
				OpenTblName = m_WfTblName
			end
			if self.m_TrackTypeName == m_ZlTblName or self.m_TrackTypeName == OpenTblName then
				self.m_ItemTbl[i].m_Page = 1	--随便设个大于0的值下面重新分页
			else
				self.m_ItemTbl[i].m_Page = 0	--设成0的值之后不显示
			end
		end
	end
	
	self:InitTrack()
	self.m_MaxPage = 0
	local MaxHeight = self.m_TrackListCtrl:GetWndOrgHeight() - self.m_LastPageBtn:GetWndOrgHeight()
	local height = 0
	local page = 1
	local showpage = self.m_NowPage
	for i=1, table.getn(self.m_ItemTbl) do
		if self.m_ItemTbl[i].m_Page > 0 then		--0代表这个任务暂不显示，-1代表不追踪
			self:SetItemTbl(i)
			height = height + self.m_ItemTbl[i].m_Height
			if height > MaxHeight then
				page = page + 1
				height = self.m_ItemTbl[i].m_Height
			end
			self.m_ItemTbl[i].m_Page = page
			self.m_MaxPage = page
			if QuestName == self.m_ItemTbl[i].m_QuestName then
				showpage = page
			end
		end
	end
	
	if self.m_MaxPage == 0 and self.m_TrackTypeName ~= m_ZlTblName then
		self.m_TrackTypeName = m_ZlTblName
		self:UpdateItemTbl(nil, QuestName)
	else
		if showpage > self.m_MaxPage then
			showpage = self.m_MaxPage
		end
		self:ShowQuestTraceList(showpage)
	end
end

function CQuestTraceBackWnd:InitTrack()
	self.m_CheckMouseLeaveWnd = nil
	self.m_CheckMouseLeaveState = nil
	self.m_QuestHypeLink = {}
	self.m_QuestUseItem = {}
	self.m_QuestSkillLink = {}--技能名和技能等级
end

--显示任务追踪面板中的所有追踪的任务
function CQuestTraceBackWnd:ShowQuestTraceList(ShowPage)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if self.m_ShowNot then
		return
	end
	
	if self.m_MaxPage == 0 then
		self.m_NowPage = 0
		self.m_TrackTypeName = m_ZlTblName
		self:ShowWnd(false)
		return
	end
	
	self.m_NowPage = ShowPage
	self.m_TrackType = 1
	local itemwidth = self.m_TrackListCtrl:GetWndOrgWidth()
	self.m_TrackListCtrl:InsertColumn(0, itemwidth)
	
	local itemnum = 0
	for i=1, table.getn(self.m_ItemTbl) do
		if self.m_ItemTbl[i].m_Page == ShowPage then
			local item = nil
			local rt = CFRect:new()
			self.m_TrackListCtrl:InsertItem(itemnum, QuestNameHeight)
			item = self.m_TrackListCtrl:GetSubItem(itemnum, 0)
			--item:SetStyle(0x60080000)
			item:GetLogicRect(rt)
			self.m_ItemTbl[i].m_NameItem:SetLogicRect(rt)
			self.m_ItemTbl[i].m_NameItem:ShowWnd(true)
			itemnum = itemnum+1
			
			self.m_TrackListCtrl:InsertItem(itemnum, self.m_ItemTbl[i].m_InfoItem.m_QuestInfoText:GetWndOrgHeight())
			item = self.m_TrackListCtrl:GetSubItem(itemnum, 0)
			item:GetLogicRect(rt)
			self.m_ItemTbl[i].m_InfoItem:SetLogicRect(rt)
			self.m_ItemTbl[i].m_InfoItem:ShowWnd(true)
			itemnum = itemnum+1
		else
			self.m_ItemTbl[i].m_NameItem:ShowWnd(false)
			self.m_ItemTbl[i].m_InfoItem:ShowWnd(false)
		end
	end
	
	self.m_LastPageBtn:ShowWnd(false)
	self.m_NextPageBtn:ShowWnd(false)
	if self.m_MaxPage > 1 then
		if ShowPage == 1 then
			self.m_NextPageBtn:ShowWnd(true)
		elseif ShowPage == self.m_MaxPage then
			self.m_LastPageBtn:ShowWnd(true)
		else
			self.m_NextPageBtn:ShowWnd(true)
			self.m_LastPageBtn:ShowWnd(true)
		end
	end
	
	self.m_TrackListCtrl:DeleteAllItem()
	self:ShowWnd(true)
end

function CQuestTraceBackWnd:AddExpUpdateActionNotify()
	if not self:IsShow() and self.m_TrackType == 2 then
		return
	end
	local PlayerLevel = g_MainPlayer:CppGetLevel()
	for ActionName, _ in pairs(self.m_WaitingLevelActionName) do
		local MinLevel = FbActionDirect_Common(ActionName, "MinLevel")
		local MaxLevel = FbActionDirect_Common(ActionName, "MaxLevel")
		if PlayerLevel>=MinLevel and PlayerLevel<=MaxLevel then
			self:ShowActionTraceList(true)
			break
		end
	end
end

--显示追踪玩法
function CQuestTraceBackWnd:ShowActionTraceList(bManualShow)
	self.m_TrackType = 2
	self:InitTrack()
	local itemwidth = self.m_TrackListCtrl:GetWndOrgWidth()
	self.m_TrackListCtrl:InsertColumn(0,itemwidth)
	local itemnum = 0
	
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	local PlayerLevel = g_MainPlayer:CppGetLevel()
	local PlayerCam = g_MainPlayer:CppGetBirthCamp()
	for ActionName, ActionTbl in pairs(self.m_NowActionTrackTbl) do
		local IsNotify = true
		local MinLevel = FbActionDirect_Common(ActionName, "MinLevel")
		local MaxLevel = FbActionDirect_Common(ActionName, "MaxLevel")
		self.m_WaitingLevelActionName[ActionName] = true
		if (PlayerLevel>=MinLevel and PlayerLevel<=MaxLevel) then
			self.m_WaitingLevelActionName[ActionName] = nil
			local JoinCountTimes = FbActionDirect_Common(ActionName, "MaxTimes") or ""
			local ContinualMin = FbActionDirect_Common(ActionName, "ContinualMin")
			local StartPlace = FbActionDirect_Common(ActionName, "StartPlace")
			StartPlace = GetCfgTransformValue(true,"FbActionDirect_Common", ActionName,"StartPlace")
			local StartNpc = nil
			for i = 1,#(StartPlace) do
				local posId = StartPlace[i][2]
				local PosX, PosY, SceneName = GetScenePosition(posId)
				local SceneCfg = Scene_Common[SceneName]
				if SceneCfg and (SceneCfg.Camp == 0 or SceneCfg.Camp == "" or PlayerCam == SceneCfg.Camp) then
					local lan_NpcName = StartPlace[i][1]
					if Npc_Common(lan_NpcName) then
						lan_NpcName = GetNpcDisplayName(lan_NpcName)
					elseif IntObj_Common(lan_NpcName) then
						lan_NpcName = GetIntObjDispalyName(lan_NpcName)
					end
					StartNpc = LinkNpcBaseColor.."#u#l"..LinkNpcColor..lan_NpcName.."#i[".. MemH64(StartPlace[i][1]) .."]#l#u#W"..TrackInfoColorStr
					local tbl = {SceneName, PosX, PosY}
					self.m_QuestHypeLink[StartPlace[i][1]] = tbl
					break
				end
			end
			
			if StartNpc then
				local PlayerStr = "  "..GetStaticTextClient(8312)..StartNpc.."#r"
				if JoinCountTimes ~= "" then
					JoinCountTimes = "  ("..GetStaticTextClient(8000,JoinCountLimit_Server(JoinCountTimes, "Count"))..")#r"
				else
					JoinCountTimes = "  ("..GetStaticTextClient(8001)..")#r"--不限制次数
				end
				
				for i = 1,#(ActionTbl) do
					local starthour = tonumber(ActionTbl[i][1])
					local startmin = tonumber(ActionTbl[i][2])
					local endhour,endmin = self:CalculateAddMinTime(starthour,startmin,ContinualMin)
					
					if starthour < 10 then
						starthour = "0"..starthour
					end
					
					if startmin < 10 then
						startmin = "0"..startmin
					end
					
					if endhour < 10 then
						endhour = "0"..endhour
					end
					
					if endmin < 10 then
						endmin = "0"..endmin
					end
					
					local lan_ActionName = g_GetFbActionNameLanStr(ActionName)
					
					local infostr = TrackInfoColorStr..lan_ActionName.."("..starthour..":"..startmin.."--"..endhour..":"..endmin..")".."#r"..PlayerStr..JoinCountTimes
					self:CreateTrackInfoItem(itemnum,itemwidth,infostr,nil)
					itemnum = itemnum+1
				end
			end
		end
	end
	
	self.m_LastPageBtn:ShowWnd(false)
	self.m_NextPageBtn:ShowWnd(false)
	self:ShowWnd(true)
end

--追踪上一页
function CQuestTraceBackWnd:TrackLastPage()
	if self.m_TrackType == 1 then
		self:ShowQuestTraceList(self.m_NowPage-1)
	else
		self:ShowActionTraceList()
	end
end

--追踪下一页
function CQuestTraceBackWnd:TrackNextPage()
	if self.m_TrackType == 1 then
		self:ShowQuestTraceList(self.m_NowPage+1)
	else
		self:ShowActionTraceList()
	end
end

--任务完成窗口事件响应
function CQuestTrackInfoItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == RICH_CLICK )	then
  	local value = self.m_QuestInfoText:GetChooseStrAttr()
  	local QuestName = self.m_QuestName
  	local linktbl = nil
  	if g_GameMain.m_QuestTraceBack.m_TrackType == 2 then
			linktbl = g_GameMain.m_QuestTraceBack.m_QuestHypeLink[value]
		else
			linktbl = g_GameMain.m_QuestTraceBack.m_QuestHypeLink[QuestName..value]
		end
		local itemtype = g_GameMain.m_QuestTraceBack.m_QuestUseItem[value]
		local SkillLevel = g_GameMain.m_QuestTraceBack.m_QuestSkillLink[value]
		
		if linktbl then
			PlayerAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])
		elseif itemtype then
			local nRoomIndex, nPos = g_GameMain.m_BagSpaceMgr:FindItemBySpace(g_StoreRoomIndex.PlayerBag, itemtype,value)
			if nRoomIndex ~= nil then
				g_GacUseItem(nil, itemtype, value, nRoomIndex, nPos, nil)
			end
		elseif SkillLevel then
			if IsCppBound(g_MainPlayer) then
				g_MainPlayer:FightSkill(value, SkillLevel)
			end
		end
	end
end

function CQuestNameItemWnd:TransferAreaAndRecordBtn(bShow)
	if bShow then
		--local width = self.m_RetToAreaWndBtn:GetWndOrgWidth() + self.m_RetQuestRecordBtn:GetWndOrgWidth() + 4
		--self.m_QuestNameBtn:SetWndWidth( self:GetWndOrgWidth() - width )
		self.m_RetToAreaWndBtn:ShowWnd(true)
		self.m_RetQuestRecordBtn:ShowWnd(true)
		self.m_RetToAreaWndBtn:SetTransparent(0.1)
		self.m_RetQuestRecordBtn:SetTransparent(0.1)
		self.m_RetToAreaWndBtn:SetTransparentObj(400,false)
		self.m_RetQuestRecordBtn:SetTransparentObj(400,false)
	else
		--self.m_QuestNameBtn:SetWndWidth( self:GetWndOrgWidth() )
		self.m_RetToAreaWndBtn:SetTransparentObj(400,true)
		self.m_RetQuestRecordBtn:SetTransparentObj(400,true)
	end
end

--任务完成窗口事件响应
function CQuestNameItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_RetToAreaWndBtn then
			g_GameMain.m_AreaInfoWnd:OpenAreaWndByQuest(self.m_QuestName)
		elseif Child == self.m_RetQuestRecordBtn then
--			g_GameMain.m_QuestRecordWnd:InitQuestRecordWnd(self.m_QuestName)
			local Wnd = g_GameMain.m_HandBookWnd
			if Wnd:IsShow() and Wnd.m_QuestName == self.m_QuestName then
				Wnd:ShowWnd(false)
				return
			else
				Wnd:InitHandBookWndByQuest(self.m_QuestName)
			end
		end
	elseif uMsgID == BUTTON_MOUSEMOVE and Child == self.m_QuestNameBtn then
		g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveState = true
		if g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveWnd then
			if g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveWnd ~= self then
				g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveWnd:TransferAreaAndRecordBtn(false)
			else
				return
			end
		end
		g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveWnd = self
		self:TransferAreaAndRecordBtn(true)
	elseif uMsgID == WM_KILLFOCUS and Child == self.m_QuestNameBtn then
		self:TransferAreaAndRecordBtn(false)
	end
end

function CQuestTraceBackWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		for i=1, 5 do
			if Child == self.m_QuestBtn[i] then
				self.m_TrackTypeName = self.m_QuestText[i].m_TextName
				self:UpdateItemTbl()
				return
			end
		end
		
		if Child == self.m_LastPageBtn then
			self:TrackLastPage()
		elseif Child == self.m_NextPageBtn then
			self:TrackNextPage()
		end
	end
end

function CQuestTraceBackWnd:OnSized()
	if self:IsShow() and IsCppBound(g_MainPlayer) then
		if(1 == self.m_TrackType) then
			self:ShowQuestTraceList(self.m_NowPage)
		elseif(2 == self.m_TrackType) then
			self:ShowActionTraceList(true)
		end
	end
end

--刷新一下任务列表
function CQuestTraceBackWnd:ReFreshQuestList()
	if g_GameMain.m_QuestTraceBack:IsShow() then
		if g_GameMain.m_QuestTraceBack.m_TrackType == 1 then
			g_GameMain.m_QuestTraceBack:UpdateItemTbl()
		else
			g_GameMain.m_QuestTraceBack:ShowActionTraceList(true)
		end
	end
end

function CQuestNameItemWnd:CheckMouseLeaveWnd(posCursor)
	local rt = CFRect:new()
	self:GetWndRect(rt)
	if posCursor.y < rt.top 
		or posCursor.y > rt.bottom 
		or posCursor.x < rt.left 
		or posCursor.x > rt.right then
		self:TransferAreaAndRecordBtn(false)
		g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveState = nil
		g_GameMain.m_QuestTraceBack.m_CheckMouseLeaveWnd = nil
	end
end

function OpenQuestTraceBack()
	if g_GameMain.m_QuestTraceBack:IsShow() then
		g_GameMain.m_QuestTraceBack:ShowWnd(false)
		g_GameMain.m_QuestTraceBack.m_ShowNot = true
	else
		g_GameMain.m_QuestTraceBack.m_ShowNot = nil
		if g_GameMain.m_QuestTraceBack.m_TrackType == 1 then
			g_GameMain.m_QuestTraceBack:UpdateItemTbl()
		else
			g_GameMain.m_QuestTraceBack:ShowActionTraceList(true)
		end
	end
end

function Gas2Gac:RetIsShowQuestTraceBack(Conn,IsShow)
--	if IsShow then
--		--打开任务追踪面板
--		if g_GameMain.m_QuestTraceBack.m_TrackType == 1 then
--			g_GameMain.m_QuestTraceBack:ShowQuestTraceList()
--		else
--			g_GameMain.m_QuestTraceBack:ShowActionTraceList(true)
--		end
--	else
--		--关闭任务追踪面板
--		if g_GameMain.m_QuestTraceBack:IsShow() then
--			g_GameMain.m_QuestTraceBack:ShowWnd(false)
--		end
--	end
end
