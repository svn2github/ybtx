gac_require "Relation/Team/TeamClientListInc"
gac_require "framework/texture_mgr/DynImageMgr"


function CTeamListWnd:Init(Parent)
	self:InitTeamListWnd(Parent)
	self:SetText()		
end

	--窗口初始化函数
function CTeamListWnd:InitTeamListWnd(Parent)
	self:CreateFromRes("TeamListMain", Parent)  --获取GUI框架
	self.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26001)) -- “报名”
	self.m_ApplicationInfo = {}
end  
	--绑定主窗口上部件
function CTeamListWnd:OnChildCreated()
	self.m_TeamListTitle		= self:GetDlgChild("TeamListTitle")
	self.m_TeamListJoinBtn 		= self:GetDlgChild("TeamListJoinBtn")
	self.m_TeamListInviteBtn	= self:GetDlgChild("TeamListInviteBtn")
	self.m_TeamListChlidList	= self:GetDlgChild("TeamListChlidList")
	self.m_TeamListCloseBtn		= self:GetDlgChild("TeamListCloseBtn")
	self.m_FreshBtn				= self:GetDlgChild("FreshBtn")
	self.m_ChatBtn				= self:GetDlgChild("chatBtn")
	self.m_TeamListJoinBtn.m_state = true
end

--窗口事件监听器
function CTeamListWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == ITEM_LBUTTONCLICK) then
		self.info = self.m_ApplicationInfo[uParam1+1]
	end
	if(uMsgID == BUTTON_LCLICK)	then
		local wnd = g_GameMain.m_TeamApplicationListMini 
		if Child == self.m_TeamListJoinBtn then
			if (Child.m_state) then 
				if g_MainPlayer.m_Properties:GetTeamID() ~= 0 then
					MsgClient(149) ----您已经有小队了 不能在进行排队
				else
					g_GameMain.m_TeamListOneItem.m_Text1:SetWndText("")
					g_GameMain.m_TeamListOneItem:ShowWnd(true)
					g_GameMain.m_TeamListOneItem:SetFocus()
				end
			else
			------取消报名  
			Gac2Gas:leaveList(g_Conn,self.m_nType)
				if 1 == self.m_nType then
					g_GameMain.m_TeamAppUnderList.m_TeamListJoinBtn.m_state = true
					g_GameMain.m_TeamAppUnderList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26001)) --“报名”
					if wnd.m_ShowUgTimeTick then
						UnRegisterTick(wnd.m_ShowUgTimeTick)
						wnd.m_ShowUgTimeTick = nil
						wnd.m_UgStr = ""
						wnd:setStr()  
					end
				elseif 2 == self.m_nType then
					g_GameMain.m_TeamAppActList.m_TeamListJoinBtn.m_state = true
					g_GameMain.m_TeamAppActList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26001)) --“报名”
					if wnd.m_ShowActTimeTick then
						UnRegisterTick(wnd.m_ShowActTimeTick)
						wnd.m_ShowActTimeTick = nil
						wnd.m_ActStr = ""
						wnd:setStr() 
					end
				elseif 3 == self.m_nType then
					g_GameMain.m_TeamAppTaskList.m_TeamListJoinBtn.m_state = true
					g_GameMain.m_TeamAppTaskList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26001)) --“报名”
					if wnd.m_ShowTaskTimeTick then
					 	UnRegisterTick(wnd.m_ShowTaskTimeTick)
						wnd.m_ShowTaskTimeTick = nil
						wnd.m_TaskStr = ""
						wnd:setStr() 
					end
				end
				if  (wnd.m_TaskStr == "" ) and (wnd.m_UgStr == "") and (wnd.m_ActStr == "") then
					g_GameMain.m_TeamApplicationListMini:ShowWnd( false )
				end
			end
		elseif Child == self.m_TeamListInviteBtn then
			--得到目标属性   self.tatgetID 用于组队键  调用组队方法。
			local isCap = g_GameMain.m_TeamBase.m_bCaptain
			if isCap or (0 == g_MainPlayer.m_Properties:GetTeamID()) then
				if nil ~= self.info then 
					Gac2Gas:InviteMakeTeam(g_Conn, self.info.id)
					MsgClient(98,self.info.name)--向%s发送了邀请
					self.info = nil
					else
					MsgClient(145) --请选择组队目标
				end
			else
				MsgClient(144)  --请耐心等待队长邀请队员
			end
		elseif Child == self.m_FreshBtn then
			local nNeedTime = 15
			local FreshAppListTick = function()
			local str = GetStaticTextClient(2009)
				if(0 == nNeedTime) then
					UnRegisterTick(wnd.m_FreshAppListTick)
					wnd.m_FreshAppListTick = nil
					self.m_FreshBtn:SetWndText( str )
					self.m_FreshBtn:EnableWnd(true)
					return
				end
				nNeedTime = nNeedTime - 1
				self.m_FreshBtn:SetWndText( str .. "(" .. nNeedTime .. ")" )
			end
			if( not wnd.m_FreshAppListTick ) then
				wnd.m_FreshAppListTick = RegisterTick("FreshAppListTick", FreshAppListTick, 1000)
				self.m_FreshBtn:EnableWnd(false)
				Gac2Gas:upFreshTeamList(g_Conn,self.m_nType)
			end
	  	
	  elseif Child == self.m_ChatBtn then
	  	---私聊
	  	if nil ~= self.info then 
	  	g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(self.info.id, self.info.name)
	  	self.info = nil
	  	end
	  elseif Child == self.m_TeamListCloseBtn then
	  	self:ShowWnd(false)
	  end
	end
end  

 --DrawList
function CTeamListWnd:DrawTeamList()
	self.m_TeamListChlidList:DeleteAllItem() 	
	self.m_tblTeamList = {}

	self.m_TeamListChlidList:InsertColumn(0, self.m_TeamListChlidList:GetWndOrgWidth())
	for i = 1, #self.m_ApplicationInfo do
		self.m_TeamListChlidList:InsertItem(i-1, 88)
		local item = self.m_TeamListChlidList:GetSubItem(i-1, 0)
		local teamListItem = self:CreateTeamListItem(item)
		table.insert(self.m_tblTeamList, teamListItem)
		teamListItem:SetTeamListText(self.m_ApplicationInfo[i])
	end
end

function CTeamListWnd:InsertInfoBegin()
	self.m_ApplicationInfo = {}
end

function CTeamListWnd:InsertInfo(nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
	local tbl = {}
	tbl.id		= nID
	tbl.name	= sName
	tbl.camp	= nCamp
	tbl.class 	= nClass
	tbl.level 	= nLevel
	tbl.type	= nType  
	tbl.msg   	= sMsg      
	tbl.time 	= nTime
	table.insert(self.m_ApplicationInfo, tbl)
end


--创建item窗口
function CTeamListWnd:CreateTeamListItem(Parent)
	local wnd = CTeamListItemWnd:new()
	wnd:initItem(Parent)
	wnd:ShowWnd( true )
	return wnd
end


function CTeamListItemWnd:OnChildCreated()
	self.m_TeamListChildImage			= self:GetDlgChild("TeamListChildImage")
	self.m_TeamListChildPlayerName		= self:GetDlgChild("TeamListChildPlayerName")
	self.m_TeamListChildPlayerLevel		= self:GetDlgChild("TeamListChildPlayerLevel")
	self.m_TeamListChildPlayerClass		= self:GetDlgChild("TeamListChildPlayerClass")
	self.m_TeamListChildText            = self:GetDlgChild("text")
end


function CTeamListItemWnd:initItem(Parent)
	self:CreateFromRes("TeamListChild", Parent)
end


-------------------list子窗体类
function CTeamListItemWnd:SetTeamListText(info)
	local class = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(info.class)
	self.m_PlayerId = info.id
	self.m_TeamListChildPlayerName:SetWndText(info.name)
	self.m_TeamListChildPlayerLevel:SetWndText(GetStaticTextClient(8322, info.level))     
	self.m_TeamListChildText:SetWndText(info.msg)
	self.m_TeamListChildPlayerClass:SetWndText(class)
	--self.m_TeamListChildPlayerChose3:EnableWnd(tOrF(tostring(info.type).sub(info.type,3,3)))
	local sEnableImg, sDisableImg = g_DynImageMgr:GetClassPic(g_MainPlayer:CppGetBirthCamp(), info.class)
	g_DynImageMgr:AddImageByIP(self.m_TeamListChildImage, sEnableImg, IP_ENABLE)
end



---------------------迷你窗体
function CreateTeamListMiniWnd(Parent)
	local wnd = CTeamListMiniWnd:new()		
	wnd:CreateFromRes("TeamListBM", Parent)
	wnd.m_Btn = wnd:GetDlgChild("miniBtn")
	wnd:ShowWnd(false)
	wnd.m_SurTime = 4*60*60             ---模拟在排队队列中时长 
  return wnd
end


--------------------tooltips tick
local function ShowTooltips(Wnd, str)
	if not str then
		local str = ""
	end
			if Wnd then
				Wnd:SetMouseOverDescAfter(str)
			end
end
--拼凑tooltips
function CTeamListMiniWnd:setStr()
	if self.m_ShowUgTimeTick and (self.m_SUgTime and (self.m_SUgTime < 60) and (self.m_SUgTime <= 0)) then
		if self.m_ShowUgTimeTick then
			UnRegisterTick(self.m_ShowUgTimeTick)
			self.m_ShowUgTimeTick = nil
		end
		MsgClient(146)
		self.m_UgStr = ""
	end
	if  self.m_ShowActTimeTick and (self.m_SActTime and (self.m_SActTime < 60) and (self.m_SActTime <= 0))  then
		if self.m_ShowActTimeTick then
			UnRegisterTick(self.m_ShowActTimeTick)
			self.m_ShowActTimeTick = nil
		end
		MsgClient(147)     
		self.m_ActStr = ""
  	end
  	
  	if  self.m_ShowTaskTimeTick and (self.m_STaskTime and (self.m_STaskTime< 60) and (self.m_STaskTime <= 0)) then
		if self.m_ShowTaskTimeTick then
			UnRegisterTick(self.m_ShowTaskTimeTick)
			self.m_ShowTaskTimeTick = nil
		end
		MsgClient(148)
		self.m_TaskStr = ""
	end
			
	if not self.m_UgStr then
		self.m_UgStr = ""
	end
	if not self.m_ActStr then
		self.m_ActStr = ""
	end
	if not self.m_TaskStr then
		self.m_TaskStr = ""
	end               
 	
 	ShowTooltips(self.m_Btn, GetStaticTextClient(26012,self.m_UgStr,self.m_ActStr,self.m_TaskStr))
 	
 	if ("" == self.m_UgStr)and("" == self.m_ActStr)and ("" == self.m_TaskStr) then
		g_GameMain.m_TeamApplicationListMini:ShowWnd( false )
	end
	
 			
end
--地下城报名TICK
function CTeamListMiniWnd:SurUgTime()
	if self.m_ShowUgTimeTick then
		UnRegisterTick(self.m_ShowUgTimeTick)
		self.m_ShowUgTimeTick = nil
	end
	if self.m_Ugt <=1 then
		self.m_UgStr = GetStaticTextClient(26009,4,0)
		self:setStr()
	end
	local function SurUgTimeTick()
		self.m_Ugt = self.m_Ugt+1
		self.m_SUgTime = self.m_SurTime - 60*self.m_Ugt
		local SHour = math.floor(self.m_SUgTime/3600 )
		local SMin  = math.floor((self.m_SUgTime-SHour*3600)/60)
		self.m_UgStr =  GetStaticTextClient(26009,SHour,SMin)
		self:setStr()
	end
	if self.m_ShowUgTimeTick then
		UnRegisterTick(self.m_ShowUgTimeTick)
		self.m_ShowUgTimeTick = nil
	end
	self.m_ShowUgTimeTick = RegisterTick( "SurAppTimeTick1", SurUgTimeTick, 60*1000)
end
--多人活动报名tick
function CTeamListMiniWnd:SurActTime()
	if self.m_ShowActTimeTick then
		UnRegisterTick(self.m_ShowActTimeTick)
		self.m_ShowActTimeTick = nil
	end
	if self.m_Actt <=1 then
		self.m_ActStr = GetStaticTextClient(26010,4,0)
		self:setStr()
	end
	local function SurActTimeTick()
		self.m_Actt = self.m_Actt+1
		self.m_SActTime = self.m_SurTime - 60*self.m_Actt
		local SHour = math.floor(self.m_SActTime/3600 )
		local SMin  = math.floor((self.m_SActTime-SHour*3600)/60)
		self.m_ActStr = GetStaticTextClient(26010,SHour,SMin)
		self:setStr()
	end
	if self.m_ShowActTimeTick then
		UnRegisterTick(self.m_ShowActTimeTick)
		self.m_ShowActTimeTick = nil
	end
		self.m_ShowActTimeTick = RegisterTick( "SurAppTimeTick2", SurActTimeTick, 60*1000)
end
		--佣兵任务报名tick
function CTeamListMiniWnd:SurTaskTime()
	if self.m_ShowTaskTimeTick then
		UnRegisterTick(self.m_ShowTaskTimeTick)
		self.m_ShowTaskTimeTick = nil
	end
	if self.m_Taskt <=1 then
		self.m_TaskStr = GetStaticTextClient(26011,4,0)
		self:setStr()
	end
	local function SurTaskTimeTick()
		self.m_Taskt = self.m_Taskt+1
		self.m_STaskTime = self.m_SurTime - 60*self.m_Taskt
		local SHour = math.floor(self.m_STaskTime/3600 )
		local SMin  = math.floor((self.m_STaskTime-SHour*3600)/60)
		self.m_TaskStr = GetStaticTextClient(26011,SHour,SMin)
		self:setStr()
	end
	if self.m_ShowTaskTimeTick then
		UnRegisterTick(self.m_ShowTaskTimeTick)
		self.m_ShowTaskTimeTick = nil
	end
	self.m_ShowTaskTimeTick = RegisterTick( "SurAppTimeTick3", SurTaskTimeTick, 60*1000)
end

--上线刷新minitooltips
function CTeamListMiniWnd:InsertInfo(nType, nTime)
	if nTime then
		if 1 == nType then
			g_GameMain.m_TeamAppUnderList.m_TeamListJoinBtn.m_state = false
			g_GameMain.m_TeamAppUnderList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26002)) --“取消报名”
		elseif 2 == nType then 
			g_GameMain.m_TeamAppActList.m_TeamListJoinBtn.m_state = false
			g_GameMain.m_TeamAppActList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26002)) --“取消报名”
		elseif 3 == nType then
			g_GameMain.m_TeamAppTaskList.m_TeamListJoinBtn.m_state = false
			g_GameMain.m_TeamAppTaskList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26002)) --“取消报名”
		end 
	end
	
	local tbl = {}
	tbl.type	= nType  
	tbl.times  = (self.m_SurTime/60)-math.floor((self.m_SurTime-nTime)/60)
	table.insert(self.m_MiniList, tbl)
end

function CTeamListMiniWnd:SetSurAppTime()
	if  #self.m_MiniList and #self.m_MiniList ~= 0 then
		g_GameMain.m_TeamApplicationListMini:ShowWnd( true )
	end
	
	for i = 1, #self.m_MiniList do
		local nType = self.m_MiniList[i].type
		if 1 == nType then 
			self.m_Ugt = self.m_MiniList[i].times 
			self.m_SUgTime = self.m_SurTime - 60*self.m_Ugt
			local SHour = math.floor(self.m_SUgTime/3600 )
			local SMin  = math.floor((self.m_SUgTime-SHour*3600)/60)
			self.m_UgStr =  GetStaticTextClient(26009,SHour,SMin)
			self:setStr()
			self:SurUgTime()
		elseif 2 == nType then
			self.m_Actt	= self.m_MiniList[i].times
			self.m_SActTime = self.m_SurTime - 60*self.m_Actt
			local SHour = math.floor(self.m_SActTime/3600 )
			local SMin  = math.floor((self.m_SActTime-SHour*3600)/60)
			self.m_ActStr =  GetStaticTextClient(26010,SHour,SMin)
			self:setStr()
			self:SurActTime()
		elseif 3 == nType then
			self.m_Taskt = self.m_MiniList[i].times
			self.m_STaskTime = self.m_SurTime - 60*self.m_Taskt
			local SHour = math.floor(self.m_STaskTime/3600 )
			local SMin  = math.floor((self.m_STaskTime-SHour*3600)/60)
			self.m_TaskStr =  GetStaticTextClient(26011,SHour,SMin)
			self:setStr()
			self:SurTaskTime()
		end
	end
end
----------------------------------------------RPC---------------------------------------
-----发送报名列表数据请求-------
function Gas2Gac:GetTeamListBegin(Conn,nType)
	if 1 == nType then
		g_GameMain.m_TeamAppUnderList:InsertInfoBegin()
		elseif 2 == nType then
		g_GameMain.m_TeamAppActList:InsertInfoBegin()
		elseif 3 == nType then
		g_GameMain.m_TeamAppTaskList:InsertInfoBegin()
	end
end


-----接收服务器反馈数据------
function Gas2Gac:GetTeamList(Conn, nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
	if 1 == nType then
		g_GameMain.m_TeamAppUnderList:InsertInfo(nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
		elseif 2 == nType then
		g_GameMain.m_TeamAppActList:InsertInfo(nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
		elseif 3 == nType then
		g_GameMain.m_TeamAppTaskList:InsertInfo(nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
	end
end

-----返回报名列表信息------
function Gas2Gac:GetTeamListEnd(Conn,nType)
	if 1 == nType then
		g_GameMain.m_TeamAppUnderList:DrawTeamList()
		elseif 2 == nType then
		g_GameMain.m_TeamAppActList:DrawTeamList()
		elseif 3 == nType then
		g_GameMain.m_TeamAppTaskList:DrawTeamList()
	end
end

-----（取消报名或离队等）重新刷新列表表信息----
function Gas2Gac:RenewTeamList(Conn,nType)
	Gac2Gas:getAppTeamList(Conn,nType)
end
----(上线)刷新mini窗口信息

function Gas2Gac:GetLineTeamListBegin(Conn)
	g_GameMain.m_TeamApplicationListMini.m_MiniList = {}
end

function Gas2Gac:GetLineTeamList(Conn,nType,nTime)
	g_GameMain.m_TeamApplicationListMini:InsertInfo(nType,nTime)
end

function Gas2Gac:GetLineTeamListEnd(Conn)
	g_GameMain.m_TeamApplicationListMini:SetSurAppTime()
end


---------------------------------------------------------------------------------------------------------
function CreateTeamListUnderWnd(Parent)  
	local wnd = CTeamListUnder:new()		
	wnd:Init(Parent)
	wnd.m_nType = 1
	wnd:SetText()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
  return wnd
end

function CTeamListUnder:SetText()
	self.m_TeamListTitle:SetWndText(GetStaticTextClient(26003))
end

function CreateTeamListActWnd(Parent) 
	local wnd = CTeamListAct:new()		
	wnd:Init(Parent)
	wnd.m_nType = 2
	wnd:SetText()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
  return wnd
end

function CTeamListAct:SetText()
	self.m_TeamListTitle:SetWndText(GetStaticTextClient(26004))
end

function CreateTeamListTaskWnd(Parent)
	local wnd = CTeamListTask:new()	
	wnd:Init(Parent)
	wnd.m_nType = 3
	wnd:SetText()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
  return wnd
end

function CTeamListTask:SetText()
	self.m_TeamListTitle:SetWndText(GetStaticTextClient(26005))
end