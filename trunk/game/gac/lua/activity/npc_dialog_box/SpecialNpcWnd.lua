CSpecialNpcWnd = class(SQRDialog)

function CSpecialNpcWnd:Ctor(partent)
	self:CreateFromRes("SpecialNpcBox", partent)
	self.m_NpcName              = self:GetDlgChild("SpecialNpcName")
	self.m_NpcDialogBoxCloseBTn = self:GetDlgChild("SpecialNpcCloseBTn")
	self.m_NpcBTnBye            = self:GetDlgChild("SpecialNpcBTnBye")
	self.m_NpcBoxContent        = self:GetDlgChild("SpecialNpcBoxContent")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:GetWnd()
end

function CSpecialNpcWnd:GetWnd()
	self:ShowWnd(true)
	return self 
end


--消息响应
function CSpecialNpcWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_SHIFTDOWN or uMsgID == ITEM_CTRLDOWN then
		local item = self.m_NpcBoxContent:GetSubItem(uParam1, 0)
		if item.m_Special then
			item.m_Wnd:RunSpecialFun()
			self:ShowWnd(false)
			return
		end
	end
	if(uMsgID == BUTTON_LCLICK) then
		if ( Child == self.m_NpcBTnBye or Child == self.m_NpcDialogBoxCloseBTn) then
			self:ShowWnd(false)
			return
		end
	end
end


function CSpecialNpcWnd:CreateListCtrlItem_SpecialNpc_DialogBox(parent, WndName, FunType,TriggerType,TargetEntityID,TexPath)
	if not parent.m_CreateList then
		parent.m_CreateList = CCreateListCtrlItemWnd:new(parent, WndName, FunType,TriggerType,TargetEntityID,TexPath)
	else
		parent.m_CreateList:GetWnd()
	end
	return parent.m_CreateList
end


function CSpecialNpcWnd:InsertColumnSpecialInfo()
	local funcnum = 0
	self.m_tblListItemWnd = {}
	if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
		
		local Npc = g_MainPlayer.m_LockCenter.m_LockingObj
		local TargetEntityID = Npc:GetEntityID()
		local npcName = Npc.m_Properties:GetCharName()
		local NpcShowTaskCommonKey = NpcShowTask_Common:GetKeys()
		for j, index in pairs(NpcShowTaskCommonKey) do
			if index == npcName then
				local NpcShowTaskCommon = NpcShowTask_Common(npcName) 
				local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
				for i, _ in pairs(NpcShowTaskIndex) do
					local str = i..""
					local MercLevLimit = NpcShowTaskCommon(str, "MercLevLimit") 
					local LimitLevel = 100--g_MainPlayer.m_uMercenaryLevel
						--if g_GameMain.m_MercenaryLevelAward.FinishAwards["列表传送"] then
						--	LimitLevel = LimitLevel+1
						--end
					
					if type(MercLevLimit) ~= "number" then
						MercLevLimit = 0
					end
					if MercLevLimit <= LimitLevel then
						local triggerType = NpcShowTaskCommon(str, "TriggerType") 
						local index = npcName .. triggerType
						local displayName = Lan_NpcShowTask_Common(MemH64(index), "TriggerTypeShowName")
						local funName = triggerType
						local funType = NpcShowTaskCommon(str, "TaskType")  
						local texPath = NpcShowTaskCommon(str, "TexPath")
						
						local NpcBoxContent = self.m_NpcBoxContent:GetItemCount()
						self.m_NpcBoxContent:InsertItem(NpcBoxContent, 45)  --行高
						local NpcBoxContentItem = self.m_NpcBoxContent:GetSubItem(NpcBoxContent, 0)
						NpcBoxContentItem.m_Special = true
						local NpcBoxContentWnd = self:CreateListCtrlItem_SpecialNpc_DialogBox(NpcBoxContentItem,displayName,funType,funName,TargetEntityID,texPath)
						NpcBoxContentItem.m_Wnd = NpcBoxContentWnd
						funcnum = funcnum + 1
					end
				end
			end
		end
		
	end
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end


--显示Npc名字
function CSpecialNpcWnd:ResetWnd()
	
	local Npc = g_MainPlayer.m_LockCenter.m_LockingObj
	local npcName = Npc.m_Properties:GetCharName()
	local ShowName = GetNpcDisplayName(npcName)
	self.m_NpcName:SetWndText(ShowName)
	self.m_NpcBoxContent:DeleteAllItem()
	self.m_NpcBoxContent:InsertColumn(0, 265) --列宽	
	self:InsertColumnSpecialInfo()
end

function CSpecialNpcWnd:InitWnd()
	self:ResetWnd()
	self:ShowWnd(true)
end
