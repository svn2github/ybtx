cfg_load "fb_game/JoinCountLimit_Server"
--cfg_load "fb_game/FbActionPanelReward_Common"
CFbActionPanelWnd = class(SQRDialog)

local TrackInfoColorStr = "#e000000"..g_ColorMgr:GetColor("追踪面板文字")

local JoinCountLimitTbl = {}   --存放转换好的JoinCountLimit_Server 配置表信息

for _, name in pairs(JoinCountLimit_Server:GetKeys()) do
	JoinCountLimitTbl[name] = {}
	JoinCountLimitTbl[name].CriticalTime = GetCfgTransformValue(true, "JoinCountLimit_Server", name, "CriticalTime")
	if JoinCountLimitTbl[name].CriticalTime.wday then
		if JoinCountLimitTbl[name].CriticalTime.wday == 7 then
			JoinCountLimitTbl[name].CriticalTime.wday = 1
		else
			JoinCountLimitTbl[name].CriticalTime.wday = JoinCountLimitTbl[name].CriticalTime.wday + 1
		end
	end
end

function CFbActionPanelWnd:Ctor()
-- 主面板
	self:CreateFromRes("FbActionPanelWnd", g_GameMain)
	self.m_WarnValue = self:GetDlgChild("WarnValue")
	self.m_ActionListCtrl.m_ItemTbl = {}
	self.m_AllActionInfoTbl = {}												-- 当前显示的活动列表
	self.m_RemindAction = {}														-- 选中要提醒的活动
	--self.m_SoloAction = {}

	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:InitActionInfoList()	-- 初始化LIST
	self.m_LeagueJointTims = {}	-- 记录各个联赛参加的次数
	self.m_LeagueActions = {}	-- 记录各个联赛中的活动参加的次数
	self.m_CheckShowLeague = {}			-- 记录这个联赛是否显示过
	
	self.m_WarnValue:SetMouseOverDescAfter(GetStaticTextClient(8010))
end

function OpenActionPanelWnd()
	if not g_GameMain.m_FbActionPanel:IsShow() then
		g_GameMain.m_FbActionPanel:ShowActionInfoList()
		--更新时间
		--g_GameMain.m_FbActionPanel:ShowTime()
	else
		g_GameMain.m_FbActionPanel:ShowWnd(false)
	end
end

local function AddImage(wnd, strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(0,0), "0xffffffff", 0 ) 	
	wnd:SetWndBkImage( Flag, DefaultImage )
end

function CreateRewardWnd(Parent, wndres, childwndres, TexSrc, Num)
	local Wnd = CRewardBtnWnd:new()
	Wnd:CreateFromRes(wndres, Parent )
	Wnd.m_NpcSignBtn = Wnd:GetDlgChild(childwndres)
	AddImage(Wnd.m_NpcSignBtn, TexSrc)
	
	local WndWidth = Wnd:GetWndOrgWidth() + 5
	local rt = CFRect:new()
	Wnd:GetLogicRect(rt)
	local rt_1 	= CFRect:new()
	rt_1.top 		= rt.top
	rt_1.bottom = rt.bottom
	rt_1.right 	= rt.right + WndWidth * (Num - 1) + 5
	rt_1.left 	= rt_1.right - WndWidth
	
	Wnd:SetLogicRect(rt_1)
	
	Wnd:ShowWnd(true)
	return Wnd
end

function CFbActionPanelWnd:GetAllActionTimes(State)
--	-- 如果列表为空则返回
--	if #(self.m_ActionListCtrl.m_ItemTbl) == 0 then
--		return
--	end

	-- 如果时间大于1分钟才向服务器查询
	local nowtime = os.time()
	local nowdate = os.date("*t")
	local nowday	= nowdate.wday
	local nowhour = nowdate.hour
	local nowmin = nowdate.min
	
	if self.LastCheckPanelTime then
		lastday	 = self.LastCheckPanelTime.wday
		lasthour = self.LastCheckPanelTime.hour
		lastmin	 = self.LastCheckPanelTime.min
		--print("nowday"..nowday.."  nowhour"..nowhour.."  nowmin"..nowmin)
		--print("lastday"..lastday.."  lasthour"..lasthour.."  lastmin"..lastmin)
		if nowday==lastday and nowhour==lasthour and nowmin-lastmin<1 then	
			-- 打开窗口	
			g_GameMain.m_FbActionPanel:ShowActionInfoWnd()	
			return
		end
	end
	self.LastCheckPanelTime = nowdate	
	-- 登录的话在服务端的InitCreatePlayerInfo里发消息
	if State ~= "Login" then
		Gac2Gas:GetActionCount(g_Conn)
	end
end

function CFbActionPanelWnd.SetActionCount(Conn, ActionName, Count1, Count2)
	g_GameMain.m_FbActionPanel.m_ActionTimesTbl[ActionName] = Count1
	g_GameMain.m_FbActionPanel.m_LeftActionTimesTbl[ActionName] = Count2
end

function CFbActionPanelWnd.RetShowActionInfoWnd(Conn, Hour, Min)
	if IsCppBound(g_MainPlayer) then
		if not g_GameMain.m_FbActionPanel.m_RecordUpdateTime then
			g_GameMain.m_FbActionPanel.m_RecordUpdateTime = {}
		end
		g_GameMain.m_FbActionPanel.m_RecordUpdateTime = {Hour, Min}
		g_GameMain.m_FbActionPanel:ShowActionInfoWnd()
		g_GameMain.m_FbActionPanel:UpdateTime(Hour, Min)
	end
end

--function CFbActionPanelWnd:UpdateTimes()
--	-- 更新次数
--	local ItemTbl = self.m_ActionListCtrl.m_ItemTbl
--	for i=0, #(ItemTbl) do
--		local ItemWnd = ItemTbl[i]
--		local ActionName = ItemWnd.m_ActionNameStr
--		if ActionName then
--			local JoinTimes = self.m_ActionTimesTbl[ActionName]
--			local LimitType = FbActionDirect_Common(ActionName, "MaxTimes") or ""
--			if LimitType ~= "" and JoinCountLimit_Server(LimitType) then
--				local MaxTimes = JoinCountLimit_Server(LimitType, "Count")
--				if JoinTimes ~= nil then
--					ItemWnd.m_JoinLimit:SetWndText(JoinTimes.."/"..MaxTimes)
--				end
--			end
--		end
--	end
--end

function CFbActionPanelWnd:OnChildCreated()
	self.m_ActionListCtrl = self:GetDlgChild("ActionListCtrl")
	self.m_CloseBtn			  = self:GetDlgChild("CloseBtn")
	self.m_NowTime  			= self:GetDlgChild("NowTime")
	self.m_CheckCanDo			= self:GetDlgChild("CheckCanDo")
	self.m_SelectCheckCanDo = true
	self.m_CheckCanDo:SetCheck(true)
end

function CFbActionPanelWnd:UpdateTime(nowhour, nowmin)
	if not nowhour or not nowmin then
--		local nowtime = os.time()
--		local nowdate = os.date("*t")
--		nowhour = nowdate.hour
--		nowmin = nowdate.min
		local Time = self.m_NowTime:GetWndText()
		if not Time or Time == "" then
			return
		end
		nowhour, nowmin = string.match(Time, "(%d+):(%d+)")

		nowmin = nowmin + 1
		if nowmin >= 60 then
			nowmin = 0
			nowhour = nowhour + 1
			if nowhour >= 24 then
				nowhour = 0
			end
		end
	end
	
	if IsNumber(nowhour) and nowhour < 10 then
		nowhour = "0"..nowhour
	end
	
	if IsNumber(nowmin) and nowmin < 10 then
		nowmin = "0"..nowmin
	end
	--self.m_RecordUpdateTime = {nowhour, nowmin}
	self.m_NowTime:SetWndText(nowhour..":"..nowmin)
end

local function ItemWndChangeColor(Wnd)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	local strTexture = nil
	local nowhour, nowmin = 0
	if g_GameMain.m_FbActionPanel then
		nowhour = g_GameMain.m_FbActionPanel.m_RecordUpdateTime[1]
		nowmin = g_GameMain.m_FbActionPanel.m_RecordUpdateTime[2]
	end
	
--	local nowtime = os.time()
--	local nowdate = os.date("*t")
--	local nowhour = nowdate.hour
--	local nowmin = nowdate.min
	local EndTime = string.sub(Wnd.m_ActionTime:GetWndText(), 8, 12) --  12:45 -- 89:12
	local BeginTime 		= string.sub(Wnd.m_ActionTime:GetWndText(), 1, 5)
	local EndHour 			= string.sub(EndTime, 1, 2)
	local EndMin  			= string.sub(EndTime, 4, 5)
	local EndHourNum		= tonumber(EndHour)
	local EndMinNum 		= tonumber(EndMin)
	local BeginHour 		= string.sub(BeginTime, 1, 2)
	local BeginMin		  = string.sub(BeginTime, 4, 5)
	local BeginHourNum 	= tonumber(BeginHour)
	local BeginMinNum 	= tonumber(BeginMin)
	local MinLev 				= tonumber(Wnd.m_RequiredLevel:GetWndText())
	local PlayerLev 		= g_MainPlayer:CppGetLevel()
	
	if PlayerLev >= MinLev then  -- 等级够了
		Wnd.m_IsRemindBtn:EnableWnd(true)
		if EndHourNum and EndMinNum then
			--print("EndTime:"..EndTime.."  EndHour:"..EndHour.."  min:"..min)	
			if EndHourNum>nowhour or (EndHourNum==nowhour and EndMinNum>=nowmin) then							             
				if BeginHourNum < nowhour or (BeginHourNum == nowhour and BeginMinNum < nowmin) then             
					--self.m_ActionListCtrl.m_ItemTbl[i].m_IsRemindBtn:EnableWnd(false)
					--strTexture = "guitex/tex/ybtx/gui/2ndEdition/Misc/huodong_ing.tex" 							-- 活动正在进行
					strTexture = g_ImageMgr:GetImagePath(1338) 
					Wnd.m_IsRemindBtn:EnableWnd(false)
				else
					strTexture = g_ImageMgr:GetImagePath(1339)						-- 如果还没开始	
				end
			else																																		-- 结束了的
				--self.m_ActionListCtrl.m_ItemTbl[i].m_IsRemindBtn:EnableWnd(false)
				strTexture = g_ImageMgr:GetImagePath(1339)
			end	
		else																																				-- 不限时
			strTexture = g_ImageMgr:GetImagePath(1338)	
		end
	else												-- 等级不够	
		strTexture = g_ImageMgr:GetImagePath(1339)
		Wnd.m_IsRemindBtn:EnableWnd(false)
	end
	
	-- 选中标记  只有UpdateActionInfoItem()会运行
	local ActionTimeTbl = g_GameMain.m_FbActionPanel.m_RemindAction[Wnd.m_ActionNameStr] 
	if ActionTimeTbl then				
		for i=1, #(ActionTimeTbl) do
			local Time1 = ActionTimeTbl[i][1]..":"..ActionTimeTbl[i][2]
			local Time2 = BeginHour..":"..BeginMin
			if Time1==Time2 then
				Wnd.m_IsRemindBtn:SetCheck(true)
				Wnd.m_bSelectIsRemind = true
				-- 切换背景
				strTexture = g_ImageMgr:GetImagePath(1340)
			else
				--Wnd.m_IsRemindBtn:SetCheck(false)
			end
		end
	end
	
--	-- 取消标记
--	if Wnd.m_bSelectIsRemind and not ActionTimeTbl then
--		if Wnd.m_ActionNameStr == "抱小猪选拔赛" then
--		--db()
--		end
--		Wnd.m_IsRemindBtn:SetCheck(false)
--		Wnd.m_bSelectIsRemind = nil
--	end
			
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(0, 0), "0xffffffff", 0 ) 
	Wnd:SetWndBkImage(Flag, DefaultImage)
end

function CFbActionPanelWnd:UpdateActionInfoItem()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	local strTexture = nil
	
	for i=0, #(self.m_ActionListCtrl.m_ItemTbl) do
		local Wnd = self.m_ActionListCtrl.m_ItemTbl[i]
		if Wnd then
			ItemWndChangeColor(Wnd)
			
			--	-- 取消标记
			if Wnd.m_bSelectIsRemind then
				--print("这条选中了"..Wnd.m_ActionNameStr)
				local ActionTimeTbl = self.m_RemindAction[Wnd.m_ActionNameStr] 
				if not ActionTimeTbl or (ActionTimeTbl and #(ActionTimeTbl) == 0) then
				--print("但是应该没有！！")
					Wnd.m_IsRemindBtn:SetCheck(false)
					Wnd.m_bSelectIsRemind = nil
				end
			end			
		end
	end
end

function CFbActionPanelWnd:CreateActionInfoItem(itemnum)
	self.m_ActionListCtrl:InsertItem(itemnum, 24)
	local item = self.m_ActionListCtrl:GetSubItem(itemnum, 0 )
	self.m_ActionListCtrl.m_ItemTbl[itemnum] = CFbActionPanelItemWnd:new(item)
end

-- 初始化
function CFbActionPanelWnd:InitActionInfoList()
	self.m_ListItemnum = 0		-- LIST里的数量
	self.m_ActionListCtrl:DeleteAllItem()
	self.m_ActionListCtrl.m_ItemTbl = {}
	self.m_QuestHypeLink = {}
	self.m_ActionTimesTbl = {}
	self.m_LeftActionTimesTbl = {}
	self.m_FreeTimeTick = {}		-- 不限时间的Tick
end

function CFbActionPanelWnd:CheckIsInActionInfoTbl(Tbl, ActionName,starthour,startmin)
	if not IsTable(Tbl) then
		return
	end
	
	for i , timetbl in pairs(Tbl[ActionName]) do
		if timetbl[1] == starthour and timetbl[2] == startmin then
			return true
		end
	end
	return false
end

-- 增加到表m_AllActionInfoTbl里
function CFbActionPanelWnd:AddActionInfo(ActionName, starthour, startmin, endhour, endmin, wday)
	if not self.m_AllActionInfoTbl[ActionName] then
		self.m_AllActionInfoTbl[ActionName] = {}
	end
	
	if not self:CheckIsInActionInfoTbl(self.m_AllActionInfoTbl, ActionName,starthour,startmin) then
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
		table.insert(self.m_AllActionInfoTbl[ActionName],{starthour, startmin, endhour, endmin, wday})
	end
	
--	if self:IsShow()then
--		self:ShowActionInfoList()
--	end
end

function CFbActionPanelWnd:CreateActionList()
	local itemwidth = self.m_ActionListCtrl:GetWndOrgWidth()
	self.m_ActionListCtrl:InsertColumn(0,itemwidth)
	
	for i=1, #(self.ActionInfoTbl) do
		self:CreateActionInfoItem(self.m_ListItemnum)
		self.m_ListItemnum = self.m_ListItemnum + 1
	end
end

-- 全部删除然后重新创建
function CFbActionPanelWnd:ReCreateActionList()
	self.m_ActionListCtrl:DeleteAllItem()
	self.m_ListItemnum = 0
	self.m_ActionListCtrl.m_ItemTbl = {}
	self:CreateActionList()
end

 -- 不全部删除 
function CFbActionPanelWnd:ReCreateActionListOld()
	--local DifNum = #(self.ActionInfoTbl) - self.LastActionInfoLen
	local DifNum = #(self.ActionInfoTbl) - #(self.m_ActionListCtrl.m_ItemTbl) - 1
	if DifNum > 0 then	--要增加ItemWnd
		--print("增加ItemWnd  "..DifNum)
		for i=1, DifNum do
			--print("增加第 "..self.m_ListItemnum)
			self:CreateActionInfoItem(self.m_ListItemnum)
			self.m_ListItemnum = self.m_ListItemnum + 1
		end	
	else								-- 要隐藏ItemWnd
		--print("要删除ItemWnd "..DifNum)
		local Num = #(self.m_ActionListCtrl.m_ItemTbl)
		DifNum = - DifNum
		if DifNum~= 0 then
			
--			for i=0, Num - DifNum + 1 do
--				self.m_ActionListCtrl.m_ItemTbl[Num-i]:EnableWnd(true)
--			end
	
			for i=0, DifNum - 1 do
--				self.m_ActionListCtrl.m_ItemTbl[Num-i]:EnableWnd(false)
				self.m_ActionListCtrl:DeleteItem(Num-i+1)
				self.m_ActionListCtrl.m_ItemTbl[Num-i] = nil
				self.m_ListItemnum = self.m_ListItemnum - 1
				--print("删除第 "..Num-i)
			end
		end
	end
end

function CFbActionPanelWnd:ShowActionInfoList(State)
	-- 记录活动次数,然后打开窗口
	self:GetAllActionTimes(State)
end

-- 服务器返回消息后再打开窗口
function CFbActionPanelWnd:ShowActionInfoWnd()  --
	-- 对活动信息做处理
	self:ManageActionInfo()
	
	-- 创建ItemWnd
	if self.m_ListItemnum == 0 then
		self:CreateActionList()
	end
	
	-- 当过了0点活动发生变化后 增加或者减少ItemWnd
	if #(self.m_ActionListCtrl.m_ItemTbl) + 1 ~= #(self.ActionInfoTbl) then
		self:ReCreateActionList()    
	end

	-- 将活动信息打到ItemWnd上
	self:PrintActionInfo()
	-- 更新次数
	--self:UpdateTimes()
	-- 更新活动颜色 
	self:UpdateActionInfoItem()
	
	-- 显示时间
	self:ShowTime()
	
	self:ShowWnd(true)
	self:SetFocus()
end

function CFbActionPanelWnd:ManageActionInfo()
	-- 记录self.m_AllActionInfoTbl中 等级够的活动和等级不够的活动
	local ReachLevelTbl, UnReachLevelTbl = self:SplitActionInfoTbl()
	                  
	-- 记录活动信息
	self.ActionInfoTbl = {}
	-- 初始化联赛信息
	self:InitLeagueInfo()
	
	self:ReachLevActInfo(ReachLevelTbl)		
	-- 先不显示不能做的活动
	--self:UnReachLevActInfo(UnReachLevelTbl)
end

function CFbActionPanelWnd:InitLeagueInfo()

	-- 显示前清空信息
	for League, ActionTbl in pairs(self.m_CheckShowLeague) do
		self.m_CheckShowLeague[League] = {}
	end
	for League, ActionTbl in pairs(self.m_LeagueActions) do
		self.m_LeagueActions[League] = {}
	end
	
	for ActionName, ActionTbl in pairs(self.m_AllActionInfoTbl) do
		local LeagueName = Lan_FbActionDirect_Common(MemH64(ActionName), "LeagueName")
		if LeagueName and LeagueName ~= "" then
			--limitType = FbActionDirect_Common(ActionName, "LeagueName") or ""
		
			-- 把联赛活动次数加起来
			if not self.m_LeagueActions[LeagueName] then
				self.m_LeagueActions[LeagueName] = {}	
				self.m_CheckShowLeague[LeagueName] = {}
			end
	
			if not self.m_LeagueActions[LeagueName][ActionName] then
				self.m_LeagueActions[LeagueName][ActionName] = self.m_ActionTimesTbl[ActionName]
			end
		end
	end
	
	for LeagueName, ActionNameTbl in pairs(self.m_LeagueActions) do
		local Times = 0
		for ActionName, JoinTimes in pairs(ActionNameTbl) do
			Times = Times + JoinTimes
		end
		self.m_LeagueJointTims[LeagueName] = Times
	end
	
end

function CFbActionPanelWnd:SplitActionInfoTbl()   
	local ReachLevelTbl = {}                        
	local UnReachLevelTbl = {}
	 
	if not IsCppBound(g_MainPlayer) then
		return ReachLevelTbl, UnReachLevelTbl
	end       
                           
	local nowtime = os.time()                           
	local nowdate = os.date("*t")   
	
	local PlayerLevel = g_MainPlayer:CppGetLevel()
	for ActionName, ActionTbl in pairs(self.m_AllActionInfoTbl) do
		--if ActionName == "驱邪总动员" then
			--db()
		--end
		local day = nil
		local StartHour, StartMin, EndHour, EndMin

		day = ActionTbl[1][5]        
		--local 
		local IsDaySame = nil
		
		if day then
			for i=1, #day do
				local currday = (nowdate.wday - 1) == 0 and 7 or (nowdate.wday - 1)
				if day[i] == currday  then
					IsDaySame = true
					break
				end
			end
		end
	
		-- 检查星期几
		if not day or IsDaySame then
			-- 检查要求等级
			local MinLevel = FbActionDirect_Common(ActionName, "MinLevel")
			if PlayerLevel >= MinLevel then
				table.insert(ReachLevelTbl, ActionName)
			else
				table.insert(UnReachLevelTbl, ActionName)
			end
		end
	end
	return ReachLevelTbl, UnReachLevelTbl
end

-- 按时间排序
function TblSortByTime(Tbl, AllActionInfoTbl)
	-- a[hour] = {Name1,Name2,...}
	local SameHourActionTbl = {}
	local SortByTimeTbl = {}
	for j=1, #(Tbl) do
		ActionName = Tbl[j]
		ActionTbl = AllActionInfoTbl[ActionName]	
		for i=1, #(ActionTbl) do
			local index = ActionTbl[i][1] * 100 + ActionTbl[i][2]
			local EndTime = ActionTbl[i][3] * 100 + ActionTbl[i][4]
			-- 不限时活动
			if index == 0 then
				if EndTime == 2359 then
					-- 不限时活动放到最下面
					--index = 2359
					-- 不限时活动放到最上面
					--优先级1最高
					 
					if FbActionDirect_Common(ActionName, "Priority") ~= "" then
						local num = tonumber(FbActionDirect_Common(ActionName, "Priority"))
						index = -(100-num)
					else
							
						index = -1
					end
				end
			end
			
			if not SameHourActionTbl[index] then
				SameHourActionTbl[index] = {}
			end
			table.insert(SameHourActionTbl[index], {ActionName, EndTime}) 
		end
	end
	
	--b[i] = {hour1,hour2,...} 按时间排序
	for hour in pairs(SameHourActionTbl) do 
		if IsNumber(hour) then
			table.insert(SortByTimeTbl, hour) 
		end
	end
	table.sort(SortByTimeTbl)
	
	return SameHourActionTbl, SortByTimeTbl
end

-- 按等级排序
function TalSortByLev(SameLevActionTbl)
	local UnReachLevSortByLev = {}
	
	for Lev, ActionTbl in pairs(SameLevActionTbl) do
		table.insert(UnReachLevSortByLev, Lev)
	end
	
	table.sort(UnReachLevSortByLev)
	return UnReachLevSortByLev
end

function CFbActionPanelWnd:UnReachLevActInfo(UnReachLevelTbl)
	-- 等级一样的话 按时间排

	-- 先按时间排
	local SameLevActionTbl = {}
	local SameHourAction, UnReachLevelSortByTime = TblSortByTime(UnReachLevelTbl, self.m_AllActionInfoTbl)	
	for _, hour in ipairs(UnReachLevelSortByTime ) do 
		for i=1, #(SameHourAction[hour]) do
			ActionName = SameHourAction[hour][i]
			local MinLevel = FbActionDirect_Common(ActionName, "MinLevel")
			
			if not SameLevActionTbl[MinLevel] then
				SameLevActionTbl[MinLevel] = {}
			end
			table.insert(SameLevActionTbl[MinLevel], {ActionName, hour})		
		end
	end

	-- 再按等级排
	local UnReachLevSortByLev = TalSortByLev(SameLevActionTbl)
	for _, Lev in ipairs(UnReachLevSortByLev) do
		for i=1, #(SameLevActionTbl[Lev]) do
			local data = self:GetActionData(SameLevActionTbl[Lev][i][1], SameLevActionTbl[Lev][i][2])
			self:InsertData(data)	
		end
	end
end

function CFbActionPanelWnd:ReachLevActInfo(ReachLevelTbl)
	-- 计算现在的时间	
--	local nowtime = os.time()                           
--	local nowdate = os.date("*t")                           
--	local nowhour = nowdate.hour
--	local nowmin = nowdate.min
	local nowhour, nowmin = 0, 0
	if self and self.m_RecordUpdateTime then
		nowhour = self.m_RecordUpdateTime[1]
		nowmin = self.m_RecordUpdateTime[2]
	end

	if nowhour < 10 then
		nowhour = "0"..nowhour
	end
	
	if nowmin < 10 then
		nowmin = "0"..nowmin
	end 
	
	local NowTime = nowhour * 100 + nowmin
	
	-----------
	local SameHourAction, ReachLevelSortByTime = TblSortByTime(ReachLevelTbl, self.m_AllActionInfoTbl)
	for _, BeginTime in ipairs(ReachLevelSortByTime ) do 
		for i=1, #(SameHourAction[BeginTime]) do
			-- 结束的时间
			local EndTime = SameHourAction[BeginTime][i][2]

			ActionName = SameHourAction[BeginTime][i][1]
			local data = self:GetActionData(ActionName, BeginTime)
			if data then
				local HaveChance = true
				-- 判断是否参加够了次数
				local JoinLimitStr = data["JoinLimit"]
				local TempNum = string.find(JoinLimitStr, "/") -- / 的位置
				if TempNum then
					local JoinTimes =	string.sub(JoinLimitStr, 1, TempNum - 1 )
					local JoinLimit = string.sub(JoinLimitStr, TempNum + 1, #(JoinLimitStr))
					if JoinTimes == JoinLimit then
						HaveChance = false
					end
				end
				
				if self.m_SelectCheckCanDo and (NowTime > BeginTime) and (NowTime < EndTime) and HaveChance	-- 选中只显示当前可以参加的活动
					or not self.m_SelectCheckCanDo then					--没选中
						self:InsertData(data)			
				end
			
			end
		
		end
	end
end

function CFbActionPanelWnd:InsertData(data)
	if not data then
		return
	end
	
	local nowtime = os.time()
	local nowdate = os.date("*t")
	local nowday	= nowdate.wday
	
	-- 检查今天是否有这个活动
	local IsMatchDay = true
	if data["WDay"] then
		for i, wday in ipairs(data["WDay"]) do
			if wday ~= (((nowday == 1) and 7) or nowday - 1)then 
				IsMatchDay = false
			else
				IsMatchDay = true
				break
			end
		end
	end
	
	if IsMatchDay then
		-- 如果是联赛的活动,只显示一次
		local LeagueName = data["LeagueName"]
		if LeagueName and LeagueName ~= "" then
			if not self.m_CheckShowLeague[LeagueName][data["ActionTime"]] then
				table.insert(self.ActionInfoTbl, data)
				self.m_CheckShowLeague[LeagueName][data["ActionTime"]] = true
			end
		else
			table.insert(self.ActionInfoTbl, data)	
		end
	end
end

local function GetJoinCountTimes(limitType, ActionName)
	local PlayerLev = g_MainPlayer:CppGetLevel()
	if limitType == "佣兵训练活动" then	
		if PlayerLev >= 10 and PlayerLev < 15 then
			return 1
		end
	end
	
	-- 特定的活动可以累计次数
	if JoinCountLimit_Server(limitType, "Cycle") == "store" then
		local nowDate = os.date("*t")
		local ActionCriticalTime = JoinCountLimitTbl[limitType].CriticalTime
		
--		local DaysNum = 1
--		if ActionCriticalTime.wday < nowDate.wday then
--			DaysNum = nowDate.wday - ActionCriticalTime.wday + 1
--		elseif ActionCriticalTime.wday > nowDate.wday then
--			DaysNum = nowDate.wday - ActionCriticalTime.wday + 8
--		else -- if ActionCriticalTime.wday == nowDate.wday then
--			if ActionCriticalTime.hour > nowDate.hour 
--			or(ActionCriticalTime.hour == nowDate.hour and ActionCriticalTime.min > nowDate.min) then
--				DaysNum = 7
--			end
--		end			
		
		return g_GameMain.m_FbActionPanel.m_LeftActionTimesTbl[ActionName] --JoinCountLimit_Server(limitType, "Count") --* DaysNum 
	else
		return JoinCountLimit_Server(limitType, "Count")
	end
end

function CFbActionPanelWnd:GetActionData(ActionName, hour)
	local PlayerCam = g_MainPlayer:CppGetBirthCamp()
	local DisplayName =  Lan_FbActionDirect_Common(MemH64(ActionName), "DisplayName")
	local MinLevel = FbActionDirect_Common(ActionName, "MinLevel")
	local MaxLevel = FbActionDirect_Common(ActionName, "MaxLevel")
	local PlayType = Lan_FbActionDirect_Common(MemH64(ActionName), "PlayType")
	local LeagueName = Lan_FbActionDirect_Common(MemH64(ActionName), "LeagueName")
	local PlayerLev = g_MainPlayer:CppGetLevel()
	-- 原来图片
	--local RewardType = assert(loadstring("return {" .. FbActionDirect_Common(ActionName, RewardType).."}"))()
	-- 现在显示文字
	local RewardType = Lan_FbActionDirect_Common(MemH64(ActionName), "RewardType")
	local JoinCountTimes = nil
	
	local limitType = ""
	if LeagueName and LeagueName ~= "" then
		limitType = FbActionDirect_Common(ActionName, "LeagueName") or ""
	else
		limitType = FbActionDirect_Common(ActionName, "MaxTimes") or ""
	end
	local ContinualMin = FbActionDirect_Common(ActionName, "ContinualMin")
	local StartTime = FbActionDirect_Common(ActionName, "StartTime")
	local StartPlace = FbActionDirect_Common(ActionName, "StartPlace")
	StartPlace = GetCfgTransformValue(true,"FbActionDirect_Common", ActionName, "StartPlace")
	local StartNpc = nil
	if StartPlace and next(StartPlace) then
		local posId = StartPlace[PlayerCam][2]
		local PosX, PosY, SceneName = GetScenePosition(posId)
		local lan_NpcName = StartPlace[PlayerCam][1]
		if Npc_Common(lan_NpcName) then
			lan_NpcName = GetNpcDisplayName(lan_NpcName)
		elseif IntObj_Common(lan_NpcName) then
			lan_NpcName = GetIntObjDispalyName(lan_NpcName)
		end
		StartNpc = AutoTrackColorStr.."#u#l"..lan_NpcName.."#i[".. MemH64(StartPlace[PlayerCam][1]) .."]#l#u#W"..TrackInfoColorStr
		local tbl = {SceneName, PosX, PosY}
		self.m_QuestHypeLink[StartPlace[PlayerCam][1]] = tbl
	end
	
	if StartNpc then
		local PlayerStr = StartNpc
		if limitType ~= "" and JoinCountLimit_Server(limitType) then
			JoinCountTimes = GetJoinCountTimes(limitType, ActionName)
		end
		local starthour = math.floor(hour/100)
		local startmin = math.floor(hour%100)
		local endhour, endmin = g_GameMain.m_QuestTraceBack:CalculateAddMinTime(starthour, startmin, ContinualMin)
		
		local data = {}
		if StartTime == "" then
			data["ActionTime"] = GetStaticTextClient(8002)
		else
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
			data["ActionTime"] = starthour..":"..startmin.."--"..endhour..":"..endmin
		end
		
		--local infostr = TrackInfoColorStr..ActionName.."("..starthour..":"..startmin.."--"..endhour..":"..endmin..")".."#r"..PlayerStr..JoinCountTimes
		data["ActionName"] = ActionName		
		data["DisplayName"] = DisplayName		
		data["RequiredLevel"] = MinLevel
		data["PlayType"] = PlayType
		data["RewardType"] = RewardType
		data["LeagueName"] = LeagueName

		-- 判断是否是联赛的活动
		if Lan_FbActionDirect_Common(MemH64(ActionName), "LeagueName") ~= "" then
			--Times = self.m_LeagueJointTims[LeagueName]
			data["JoinLimit"] = GetStaticTextClient(8005)
		elseif JoinCountTimes ~= nil then
			-- 检查RPC是否更新了次数
			local Times = self.m_ActionTimesTbl[ActionName]
			if Times then
				data["JoinLimit"] = Times.."/"..JoinCountTimes
			else			
				data["JoinLimit"] = "0/"..JoinCountTimes
			end
		else
			data["JoinLimit"] = GetStaticTextClient(8001)
		end
		
		data["StartPlace"] = PlayerStr
		
		if self.m_AllActionInfoTbl[ActionName][1][5] then
			data["WDay"] = self.m_AllActionInfoTbl[ActionName][1][5]
		end
		
		return data
	end
end

function CFbActionPanelWnd:PrintActionInfo()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	
	for i=1, #(self.ActionInfoTbl) do
		local Wnd = self.m_ActionListCtrl.m_ItemTbl[i - 1]
		if Wnd then
			local data = self.ActionInfoTbl[i]
			Wnd.m_ActionNameStr = data["ActionName"]
			--print(i.."  "..data["ActionName"])
			
			if data["LeagueName"] and data["LeagueName"] ~= "" then
				Wnd.m_ActionName:SetWndText(data["LeagueName"])
			else
				Wnd.m_ActionName:SetWndText(data["DisplayName"])
			end
			Wnd.m_ActionTime:SetWndText(data["ActionTime"])
			Wnd.m_RequiredLevel:SetWndText(data["RequiredLevel"])
			Wnd.m_JoinLimit:SetWndText(data["JoinLimit"])
			Wnd.m_StartPlace:SetWndText(data["StartPlace"])	
			Wnd.m_PlayType:SetWndText(data["PlayType"])
			Wnd.m_RewardType:SetWndText(data["RewardType"])
			-- 原来奖励的图片 
			-- 奖励类型FbActionPanelReward_Common
--			Wnd.m_RewardType.RewardWnd = {}
--			local Num = 1
--			for _, Type in pairs(FbActionPanelReward_Common:GetKeys()) do
--				local Tbl = FbActionPanelReward_Common(Type)
--				local IsHave = data["RewardType"][Num] -- 无、副、主
--				local Tex =  assert(loadstring("return {" ..Tbl("Tex").."}"))() 
--				local TexSrc = Tex[IsHave]
--				Wnd.m_RewardType.RewardWnd[Num] = CreateRewardWnd(Wnd.m_RewardType, "FbActionReward", "Image", TexSrc, Num)
--				Wnd.m_RewardType.RewardWnd[Num]:SetFocus()
--				Num = Num + 1
--			end
		
--			-- 选中标记
--			local tbl = self.m_RemindAction[data["ActionName"]] 
--			if tbl then				
--				for i=1, #(tbl) do
--					local Time1 = tbl[i][1]..":"..tbl[i][2]
--					local Time2 = string.sub(data["ActionTime"], 1, 5)
--					if Time1==Time2 then
--						Wnd.m_IsRemindBtn:SetCheck(true)
--						-- 切换背景
--						local strTexture = "guitex/tex/ybtx/gui/2ndEdition/Misc/huodong_check.tex"	
--						DefaultImage:AddImage(self:GetGraphic(), -1,  strTexture, nil, CFPos:new(0, 0), "0xffffffff", 0 ) 
--						self:SetWndBkImage(Flag, DefaultImage)	
--					else
--						Wnd.m_IsRemindBtn:SetCheck(false)
--					end
--				end
--			end
			
		end
	end

end


-- 这个活动是否要提醒
function CFbActionPanelWnd:IsRemind(ActionName, StartHour, StartMin)
	for Name, TimeTbl in pairs(self.m_RemindAction) do
		if ActionName == Name then
			if TimeTbl[1]==StartHour and TimeTbl[2]==StartMin then
				return true
			end 
		end
	end
end

function CFbActionPanelWnd:ShowTime()
	if self.m_UpdataTimeTick then
		UnRegisterTick(self.m_UpdataTimeTick)
		self.m_UpdataTimeTick = nil
	end
--	self:UpdateTime()
	self.m_UpdataTimeTick = RegClassTick("UpdataTimeTick",self.UpdateTime,60*1000,self)
end

-- 自动弹出面板 然后自动关闭。 State可能为 "Login":登陆游戏, "LevelUp":升级, "Exit":退出游戏
function CFbActionPanelWnd:PopupActionPanel(State)
	self:ShowActionInfoList(State)
	
	-- 退出游戏时弹
	if State == "Exit" then
		local rt = CFRect:new()
		self:GetLogicRect(rt)
		local height = self:GetWndOrgHeight()
		rt.top = 0
		rt.bottom = rt.top + height
		self:SetLogicRect(rt)
	end
	
	-- 5秒后关闭
	--self:ClosePanelTick()
end

function CFbActionPanelWnd:ClosePanelTick()
	local function ClosePanel(Tick)
		self:ShowWnd(false)
		--self:ClearAllTick()
	end
	
	if self.m_ClosePanelTick then
		UnRegisterTick(self.m_ClosePanelTick)
		self.m_ClosePanelTick = nil
	end
	self.m_ClosePanelTick = RegisterTick("ClosePanelTick", ClosePanel, 5*1000 )
end

-- ClearAllTick
function CFbActionPanelWnd:VirtualExcludeWndClosed()
	-- 自动关闭窗口的Tick
	if self.m_ClosePanelTick then
		UnRegisterTick(self.m_ClosePanelTick)
		self.m_ClosePanelTick = nil
	end
	
	-- 更新时间的Tick
	if self.m_UpdataTimeTick then
		UnRegisterTick(self.m_UpdataTimeTick)
		self.m_UpdataTimeTick = nil
	end
end
	
-- 面板
function CFbActionPanelWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK )	then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
			--self:ClearAllTick()
		elseif Child == self.m_CheckCanDo then
			if not self.m_SelectCheckCanDo then
				self.m_SelectCheckCanDo = true
				self.m_CheckCanDo:SetCheck(true)
			else
				self.m_SelectCheckCanDo = false
				self.m_CheckCanDo:SetCheck(false)
			end
			self:ShowActionInfoList()
		end
	end
end

-- 按钮
function CFbActionPanelWnd:OpenPanel()
	if not g_GameMain.m_FbActionPanel:IsShow() then
		g_GameMain.m_FbActionPanel:ShowActionInfoList()
	else
		g_GameMain.m_FbActionPanel:ShowWnd(false)
	end
end

function CFbActionPanelWnd.AddActionNum(Conn, ActionName, AddNum)
	local ActionNum = g_GameMain.m_FbActionPanel.m_ActionTimesTbl[ActionName]
	if ActionNum then
		g_GameMain.m_FbActionPanel.m_ActionTimesTbl[ActionName] = ActionNum + AddNum
	end
end

function CFbActionPanelWnd:SetWarnValue(WarnValue)
	self.m_WarnValue:SetWndText(GetStaticTextClient(8009) .. WarnValue)
end
