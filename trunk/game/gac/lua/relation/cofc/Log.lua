gac_require "relation/cofc/LogInc"

--返回日志信息开始，初始化相关数据
function Gas2Gac:ReturnLogBegin(Conn)
	g_GameMain.m_CofCLogWnd.m_LogInfoTbl		=	{}				--记录商会日志信息的table
	g_GameMain.m_CofCLogWnd.m_LogInfoListTbl	=	{}					--用来存储商会日志信息列表中的listctrl中的item控件
end

--返回日志相关信息
function Gas2Gac:ReturnLog(Conn, logCotent, logType, logTime)
	--将数据库中的时间格式转换为常见的年月日形式
	local tblLogType = {"人员","经济","科技","任务","所有"}
	local tblWeekday = {"星期日","星期一","星期二","星期三","星期四","星期五","星期六"}
	
	local weekday    = tblWeekday[tonumber(os.date("%w", logTime))]
	local date       = os.date("%Y年%m月%d日(" .. weekday .. ")%X", logTime)
	local type       = tblLogType[logType + 1]

	local node			=	{}
	node.Time			=	date
	node.LogType		=	type
	node.Description	=	logCotent
	table.insert(g_GameMain.m_CofCLogWnd.m_LogInfoTbl, node)
end

--返回所有日志信息结束，在客户端显示相关信息
function Gas2Gac:ReturnLogEnd(Conn)
	g_GameMain.m_CofCLogWnd:ShowCofCLogInfo()
end


------------------------------------------------------------------------------------


--创建商会日志面板
function CreateCofCLogWnd(Parent)
	local wnd	=	CCofCLogWnd:new()
	wnd:CreateFromRes("CofCLog",Parent)
	wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 3, false, {g_GameMain.m_CofCMainWnd})
	wnd:InitCofCLogChild()
	return wnd
end

--初始化商会日志窗口中的子控件
function CCofCLogWnd:InitCofCLogChild()
	self.m_CloseBtn		=	self:GetDlgChild("Close")
	self.m_ShutDown		=	self:GetDlgChild("ShutDown")
	self.m_AllBtn		=	self:GetDlgChild("All")
	self.m_MemberBtn	=	self:GetDlgChild("Member")
	self.m_TechBtn		=	self:GetDlgChild("Tech")
	self.m_EconomyBtn	=	self:GetDlgChild("Economy")
	self.m_MissionBtn	=	self:GetDlgChild("Mission")
	self.m_LogList		=	self:GetDlgChild("LogList")
end

local LogType = 
{
["人员"] 			= 0,
["经济"]			= 1,
["科技"]			= 2,
["任务"]			= 3,
["所有"]			= 4
}

--商会日志窗口事件响应函数
function CCofCLogWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  	if(Child == self.m_CloseBtn or Child == self.m_ShutDown) then 			--关闭按钮
	  		self:ShowWnd(false)
	  	
	  	elseif(Child == self.m_AllBtn)    then 								
				Gac2Gas:GetCofCLogInfo(g_Conn, LogType["所有"])
	  	
	  	elseif(Child == self.m_MemberBtn)      then																		
				Gac2Gas:GetCofCLogInfo(g_Conn, LogType["人员"])
	  	
	  	elseif(Child == self.m_MissionBtn)       then							
				Gac2Gas:GetCofCLogInfo(g_Conn, LogType["科技"])
	  		
	  	elseif(Child == self.m_TechBtn) then						
				Gac2Gas:GetCofCLogInfo(g_Conn, LogType["任务"])
		
			elseif(Child == self.m_EconomyBtn) then	
				Gac2Gas:GetCofCLogInfo(g_Conn, LogType["经济"])
			end
	end
end

--显示所有日志信息
function CCofCLogWnd:ShowCofCLogInfo()
	local logTbl =	self.m_LogInfoTbl
	self.m_LogList:DeleteAllItem()
	for i=1, table.maxn( logTbl ) do
		if ( i == 1 ) then
		  self.m_LogList:InsertColumn( 0, self.m_LogList:GetWndWidth())
		end
		self.m_LogList:InsertItem( i-1, 25 )
		
		local logItemWnd = SQRDialog:new()
		local item = self.m_LogList:GetSubItem( i-1, 0)
		logItemWnd:CreateFromRes( "CofCLogItem", item )
		logItemWnd:ShowWnd( true )
		logItemWnd:SetStyle( 0x60000000 )
		logItemWnd:GetDlgChild("Time"):SetWndText(logTbl[i].Time)
		logItemWnd:GetDlgChild("Type"):SetWndText(logTbl[i].LogType)
		logItemWnd:GetDlgChild("Description"):SetWndText(logTbl[i].Description)
		table.insert(self.m_LogInfoListTbl, logItemWnd)
	end
end
