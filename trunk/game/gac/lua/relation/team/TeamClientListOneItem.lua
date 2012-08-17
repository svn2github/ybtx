gac_require "relation/team/TeamClientListOneItemInc"
gac_require "relation/team/TeamClientList"

function CreatTeamListOneItemWnd(Parent)
	local wnd = CTeamListOneItem:new()
	wnd:InitTeamListOneItemWnd(Parent)
	return wnd	
end

function CTeamListOneItem:OnChildCreated()
	  self.m_Text1 = self:GetDlgChild("text1") 
	  self.m_Text2 = self:GetDlgChild("text2")          
	  self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")    
	  self.m_CancelBtn = self:GetDlgChild("CancelBtn")      
	  self.m_CloseBtn = self:GetDlgChild("CloseBtn")
end

function CTeamListOneItem:InitTeamListOneItemWnd(Parent)
	self:CreateFromRes("TeamListOneItem", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, {g_GameMain.m_TeamAppUnderList, g_GameMain.m_TeamAppActList, g_GameMain.m_TeamAppTaskList })
 	self.m_Text1:SetMaxTextLenght(100)
end
--改变按键名称
function CTeamListOneItem:ChangeBtnState(nType)
	if 1 == nType then
		g_GameMain.m_TeamAppUnderList.m_TeamListJoinBtn.m_state = false
		g_GameMain.m_TeamAppUnderList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26002)) --“取消报名”
	elseif 2 == nType then      
		g_GameMain.m_TeamAppActList.m_TeamListJoinBtn.m_state = false
		g_GameMain.m_TeamAppActList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26002)) --“取消报名”
	elseif 3 ==	nType then
		g_GameMain.m_TeamAppTaskList.m_TeamListJoinBtn.m_state = false
		g_GameMain.m_TeamAppTaskList.m_TeamListJoinBtn:SetWndText(GetStaticTextClient(26002)) --“取消报名”
	end	    
end

--修改按键描述
function Gas2Gac:ChangeBtnState(Conn,nType)
			g_GameMain.m_TeamListOneItem:ChangeBtnState(nType)
end


--窗口事件监听器
function CTeamListOneItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )                             
	if(uMsgID == BUTTON_LCLICK )	then
		if Child == self.m_CancelBtn then
				self:ShowWnd(false)
		elseif Child == self.m_CloseBtn then
	  		self:ShowWnd(false)
	  	elseif Child == self.m_ConfirmBtn then
			local nType = g_GameMain.m_TeamListOneItem.state
			local sMsg = string.gsub(self.m_Text1:GetWndText(),"\n",";")
			if string.len(sMsg)>100 then
			else
				Gac2Gas:joinTeamList(g_Conn,nType,sMsg)
				self:ShowWnd(false)
					if 1 == nType then
						g_GameMain.m_TeamApplicationListMini.m_Ugt = 0  
						g_GameMain.m_TeamApplicationListMini:SurUgTime()  --迷你窗体 tick
					elseif 2 == nType then      
						g_GameMain.m_TeamApplicationListMini.m_Actt = 0 
						g_GameMain.m_TeamApplicationListMini:SurActTime()  --迷你窗体 tick
					elseif 3 ==	nType then
						g_GameMain.m_TeamApplicationListMini.m_Taskt = 0
						g_GameMain.m_TeamApplicationListMini:SurTaskTime()  --迷你窗体 tick
				end	    
					g_GameMain.m_TeamApplicationListMini:ShowWnd(true)   -- mini窗体
			end
		end
	end
end
