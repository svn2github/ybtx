CFbActionPanelItemWnd = class (SQRDialog)

function CFbActionPanelItemWnd:Ctor(Parent)
	self:CreateFromRes( "FbActionRichTextItemWnd", Parent )
	
	self.m_ActionName = self:GetDlgChild("ActionName")
	self.m_ActionTime = self:GetDlgChild("ActionTime")
	self.m_RequiredLevel = self:GetDlgChild("RequiredLevel")
	self.m_JoinLimit = self:GetDlgChild("JoinLimit")
	self.m_StartPlace = self:GetDlgChild("StartPlace")
	self.m_IsRemindBtn = self:GetDlgChild("IsRemind")
	-- 隐藏标记
	self.m_IsRemindBtn:ShowWnd(false)
	self.m_PlayType = self:GetDlgChild("PlayType")
	self.m_RewardType = self:GetDlgChild("RewardType")	
	self.m_ActionName:SetAutoWidth(self.m_ActionName:GetWndOrgWidth(), self.m_ActionName:GetWndOrgWidth())
	self.m_ActionTime:SetAutoWidth(self.m_ActionTime:GetWndOrgWidth(), self.m_ActionTime:GetWndOrgWidth())
	self.m_RequiredLevel:SetAutoWidth(self.m_RequiredLevel:GetWndOrgWidth(), self.m_RequiredLevel:GetWndOrgWidth())
	self.m_JoinLimit:SetAutoWidth(self.m_JoinLimit:GetWndOrgWidth(), self.m_JoinLimit:GetWndOrgWidth())
	self.m_StartPlace:SetAutoWidth(self.m_StartPlace:GetWndOrgWidth(), self.m_StartPlace:GetWndOrgWidth())

	self:ShowWnd(true)
	self:SetFocus()	
end

function CFbActionPanelItemWnd:ClickSelectRemindBtn()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	
	local RemindAction = self.m_ActionNameStr			-- 表里的ActionName
	local StartTime = self.m_ActionTime:GetWndText()
	local StartHour = string.sub(StartTime, 1, 2)
	local StartMin  = string.sub(StartTime, 4, 5)
	if not self.m_bSelectIsRemind then			-- 选中
	--if not g_GameMain.m_FbActionPanel:IsRemind(RemindAction, StartHour, StartMin) then
		self.m_bSelectIsRemind = true
		self.m_IsRemindBtn:SetCheck(true)
		-- 选中的活动存到表里
		
		if g_GameMain.m_FbActionPanel.m_RemindAction[RemindAction] == nil then
			--print("创建  "..RemindAction)
			g_GameMain.m_FbActionPanel.m_RemindAction[RemindAction] = {}
		end
		table.insert(g_GameMain.m_FbActionPanel.m_RemindAction[RemindAction], {StartHour, StartMin})
		-- 切换背景
		local strTexture = g_ImageMgr:GetImagePath(1340)
		DefaultImage:AddImage(self:GetGraphic(), -1,  strTexture, nil, CFPos:new(0, 0), "0xffffffff", 0 ) 
		self:SetWndBkImage(Flag, DefaultImage)
		
		local function FreeTimeNotify(Tick, NotifyInfo)
			DisplayMsg(2,NotifyInfo)
			SysRollAreaMsg(NotifyInfo)
			UnRegisterTick(Tick)
			g_GameMain.m_FbActionPanel.m_FreeTimeTick[RemindAction] = nil
		end
		
		-- 如果是不限时活动，2小时候提示
		if not tonumber(StartHour) then
			local DisplayName = self.m_ActionName:GetWndText()
			local NotifyInfo = GetStaticTextClient(8003, DisplayName)
			g_GameMain.m_FbActionPanel.m_FreeTimeTick[RemindAction] = RegisterTick("FreeTimeTick", FreeTimeNotify, 2*60*60*1000, NotifyInfo)
			--print("2小时候提示一次！！！！！")
			
		end
					
	else																		-- 取消选中
		self.m_bSelectIsRemind = nil
		self.m_IsRemindBtn:SetCheck(false)
		
		local RemindTbl = g_GameMain.m_FbActionPanel.m_RemindAction[RemindAction]
		if RemindTbl ~= nil then
			for i=1, #(RemindTbl) do
				if RemindTbl[i][1]==StartHour and RemindTbl[i][2]==StartMin then
					--print("删除"..RemindAction.."--"..StartHour..":"..StartMin)
					table.remove(RemindTbl, i)
					break
				end
			end
		end
		
		-- 改变颜色
		ItemWndChangeColor(self)
		
		-- 撤销Tick
		if g_GameMain.m_FbActionPanel.m_FreeTimeTick[RemindAction] then
			UnRegisterTick(g_GameMain.m_FbActionPanel.m_FreeTimeTick[RemindAction])
			g_GameMain.m_FbActionPanel.m_FreeTimeTick[RemindAction] = nil
			--print("撤销Tick!")
		end
	end
end

-- Item
function CFbActionPanelItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == RICH_CLICK )	then
  	local value = self.m_StartPlace:GetChooseStrAttr()
		local linktbl = g_GameMain.m_FbActionPanel.m_QuestHypeLink[value]
		if linktbl then
			PlayerAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])   -- 寻路
		end
	elseif uMsgID == BUTTON_LCLICK then 	
		if Child == self.m_IsRemindBtn then
			self:ClickSelectRemindBtn()		-- 提醒
		end
	end
end