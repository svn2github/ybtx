gac_require "activity/npc_dialog_box/NpcDialogBoxInc"
--gac_require "activity/npc_dialog_box/SpecialNpc_DialogBox"
cfg_load "npc/Npc_Common"

cfg_load "npc/NpcShowTask_Common"
lan_load "npc/Lan_NpcShowTask_Common"

function CreateNpcDlg(partent)
	local wnd = CNpcDialogBox:new()
	wnd:Init(partent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:InitFunLink()
	return wnd
end

function CNpcDialogBox:Init(partent)
	self:CreateFromRes("NpcDialogBox", partent)
end

function CNpcDialogBox:OnChildCreated()
	self.m_NpcName              = self:GetDlgChild("NpcName")
	self.m_NpcDialogBoxCloseBTn = self:GetDlgChild("NpcDialogBoxCloseBTn")
	self.m_NpcBTnBye            = self:GetDlgChild("NpcBTnBye")
	self.m_NpcBoxContent        = self:GetDlgChild("NpcBoxContent")
	self:DrawListPanel()
end

function CNpcDialogBox:DrawListPanel()
	local panel = self.m_NpcBoxContent
	panel:DeleteAllItem()
	panel:InsertColumn(0, 265)
	local tbl = {}
	for i = 0, 2 do
		panel:InsertItem(i, 30)
		table.insert(tbl, panel:GetSubItem(i, 0))
	end
	self.m_DisItem   = self:CreateNpcDialogRichTextItem(tbl[1])
	tbl[1]:SetStyle(0x64040000)
	self.m_FunItem   = self:CreateNpcFuncListCtrlItem(tbl[2], self)
	self.m_QuestItem = self:CreateNpcDialogBoxListCtrlItem(tbl[3], self)
end

--重新排版
function CNpcDialogBox:ReTypeset()
	if(self.m_NpcBoxContent) then --重新设定各个Item的高度
		local zoomSize = self:GetRootZoomSize()
		self.m_NpcBoxContent:SetItemHeight(self.m_DisItem:GetWndHeight()/zoomSize, 0)
		self.m_NpcBoxContent:SetItemHeight(self.m_FunItem:GetWndHeight()/zoomSize, 1)
		self.m_NpcBoxContent:SetItemHeight(self.m_QuestItem:GetWndHeight()/zoomSize, 2)
	end
end

--回调函数
function CNpcDialogBox:OnSized()
	self:ReTypeset()
end

--显示描述
function CNpcDialogBox:SetDiscriptionText(NpcName)
	local showContext = ""
	local cfg = g_NpcShowContentCfg[NpcName]
	if cfg and cfg["对话框"] then
		local ContentTbl = cfg["对话框"]["ContentTbl"]
		local n = math.random(1, table.maxn(ContentTbl))
		showContext = ContentTbl[n][1]
	end
	if("" ~= showContext) then
		showContext = g_QuestInfoTextTransfer(showContext)
	end
	self.m_DisItem.m_RichText:SetWndText("#c543705" .. showContext)
	--self.m_DisItem.m_RichText:SetWndText(showContext)
	return showContext
end

function CNpcDialogBox:InitNpcContent(EntityID,sType,TargetName, funcnametbl, cantakequesttbl,canfinishquesttbl, uBuildingTongID)
	self.TextType      = {}
	self.QuestNameList = {}
	self.m_NpcNameText = TargetName
	self.m_ShowDlgListWnd = nil
	
	local ShowName = nil
	if sType == "Npc" then
		ShowName = GetNpcDisplayName(TargetName)
	else
		ShowName = GetIntObjDispalyName(TargetName)
	end
	self.m_NpcName:SetWndText(ShowName)
	
	local showContext = self:SetDiscriptionText(TargetName)
	local funcNum,ShowFirstFun,IsShowList,IsSkipNpcDlg,TriggerType = self.m_FunItem:DrawListItem(funcnametbl, uBuildingTongID)
	self.m_FuncNum = funcNum
	local takeorfinishquestnum,doingquestnum = self.m_QuestItem:DrawListItem(sType,TargetName, cantakequesttbl, canfinishquesttbl, funcNum)
	if (1 == takeorfinishquestnum+funcNum and 0 == doingquestnum 
		and not g_MainPlayer.m_IsShowListWnd and not IsShowList) or ShowFirstFun then --如果Npc只存在一个功能  就直接跳进功能项
		if IsSkipNpcDlg then
			Gac2Gas:RenoSpecNpc(g_Conn, EntityID, TriggerType)
		else
			self:RunLinkFun(self.TextType[1], 0)
		end
		return
	elseif(0 == takeorfinishquestnum+funcNum+doingquestnum) then
		if showContext == nil or showContext == "" then
			return
		else --头顶冒泡
			local Target = CCharacterFollower_GetCharacterByID(EntityID)
			if Target and (Target.m_HeadSignWnd == nil ) then
				if not Target.m_NpcHeadUpDialog then
					local bIsCreate = NpcHeadUpDialog(Target)
					if not bIsCreate then
						return
					end
				end
				local Wnd = Target.m_NpcHeadUpDialog
				Wnd.m_NpcHeadUpDialogWnd:SetAutoWidth(120, 120)
				NpcHeadUpDialogShow(Wnd,showContext)
			end
			return
		end
	end
	
	self.m_ShowDlgListWnd = true
	
	self:ReTypeset()
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

--NpcDialogBox消息响应
function CNpcDialogBox:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	
	if(uMsgID == BUTTON_LCLICK) then
		if ( Child == self.m_NpcBTnBye or Child == self.m_NpcDialogBoxCloseBTn) then
			self:ShowWnd(false)
		end
	end
end

----------------------------------------------------------------------------------------------
function CNpcDialogBox:CreateNpcDialogRichTextItem(parent)
	local wnd = CNpcDialogRichTextItem:new()
	wnd:CreateFromRes("TakeTaskRichTextItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CNpcDialogRichTextItem:OnChildCreated()
	self.m_RichText = self:GetDlgChild("RichText")
	self.m_RichText:SetAutoWidth(257, 257)
end

---------------------------
function CNpcDialogBox:CreateNpcDialogBoxListCtrlItem(parent, fatherPanel)
	local wnd = CNpcDialogBoxListCtrlItem:new()
	wnd:CreateFromRes("NpcDialogBoxListCtrlItem", parent)
	wnd:ShowWnd(true)
	wnd.m_FatherPanel = fatherPanel
	wnd.m_List = wnd:GetDlgChild("List")
	return wnd
end

function CNpcDialogBoxListCtrlItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == ITEM_LBUTTONCLICK) then
		local Index = self.m_FatherPanel.TextType[uParam1 + 1 + self.m_FatherPanel.m_FuncNum]
		if not Index then
			LogErr("CNpcDialogBoxListCtrlItem中Index不存在","npc的名字是:"..self.m_FatherPanel.m_NpcNameText.." 场景的名字是:"..g_GameMain.m_SceneName)
			return 
		end
		self.m_FatherPanel:RunLinkFun(Index, uParam1)
		self.m_FatherPanel:ShowWnd(false)
	end
end

function CNpcDialogBoxListCtrlItem:DrawListItem(sType, TargetName, cantakequesttbl, canfinishquesttbl, funcNum)
	self.m_List:DeleteAllItem()
	self.m_List:InsertColumn(0, 265)
	
	local itemnum = 0
	local canFinishQuestNum = #canfinishquesttbl
	local canTakeQuestNum   = #cantakequesttbl
	for i = 1, canFinishQuestNum do --可提交任务
		local canfinishquest = canfinishquesttbl[i]
		local level1 = Quest_Common(canfinishquest, "任务等级")
		level1 = g_RetNoneMercSentenceParse(level1)
		local str = "("..level1..")"..g_GetLanQuestName(canfinishquest)
		table.insert(self.m_FatherPanel.QuestNameList,canfinishquest)
		self.m_List:InsertItem(itemnum, 30)
		local item = self.m_List:GetSubItem(itemnum, 0)
		local wnd = CreateNpcDialogBoxListCtrlItemListItem(item)
		wnd:ShowWnd(true)
		wnd.m_Text:SetWndText(GetStaticTextClient(1008) .. ":" .. str)
		local icon = SQRDialog:new()
		icon:CreateFromRes("QuestTypePic1", wnd.m_Icon)
		icon:ShowWnd( true )
		item.m_WndTbl = {wnd,icon}
		self.m_FatherPanel.TextType[i+funcNum] = self.m_FatherPanel.DlgTextType["提交任务"]
		itemnum = itemnum + 1
	end
	
	for i = 1, canTakeQuestNum do --可接取任务
		local cantakequest = cantakequesttbl[i]
		local level1 = Quest_Common( cantakequest, "任务等级" )
		level1 = g_RetNoneMercSentenceParse(level1)
		local str = "("..level1..")"..g_GetLanQuestName(cantakequest)
		table.insert(self.m_FatherPanel.QuestNameList, cantakequest)
		self.m_List:InsertItem(itemnum, 30)
		local item = self.m_List:GetSubItem(itemnum, 0)
		local wnd = CreateNpcDialogBoxListCtrlItemListItem(item)
		wnd.m_Text:SetWndText(GetStaticTextClient(1003) .. ":".. str)
		local icon = SQRDialog:new()
		icon:CreateFromRes("QuestTypePic2", wnd.m_Icon)
		icon:ShowWnd(true)
		item.m_WndTbl = {wnd,icon}
		self.m_FatherPanel.TextType[canFinishQuestNum+i+funcNum] = self.m_FatherPanel.DlgTextType["接取任务"]
		itemnum = itemnum + 1
	end
	
	local doingquestnum = 0
	if(g_WhereFinishQuestMgr[sType][TargetName]) then --进行中任务
		for i, p in pairs(g_WhereFinishQuestMgr[sType][TargetName]) do
			local statemgr = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[p]
			if statemgr ~= nil and statemgr.m_State == 1 and CQuest.CheckCanFinishQuest(p) == false then
				doingquestnum = doingquestnum+1
				
				--答题任务.末完成的任务 
				if g_QuestNeedMgr[p] then
					local questneed = g_QuestNeedMgr[p]["答题"]
					if questneed ~= nil then
						local Flag1 = IMAGE_PARAM:new(SM_BS_BK, IP_DISABLE)
						self.m_List:InsertItem(itemnum, 30)
						local item1 = self.m_List:GetSubItem(itemnum, 0)
						local wnd1 = CreateNpcDialogBoxListCtrlItemListItem(item1)
						--item1:EnableWnd(false)
						wnd1.m_Text:SetWndTextColor(Flag1, 0xff999999)
						wnd1.m_Text:SetWndText(GetStaticTextClient(1022))
						self.m_FatherPanel.TextType[canFinishQuestNum+canTakeQuestNum+doingquestnum+funcNum] = self.m_FatherPanel.DlgTextType["答题"]
						item1.m_WndTbl = {wnd1}
						itemnum = itemnum + 1
					end
				end
				
				local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_DISABLE)
				self.m_List:InsertItem(itemnum, 30)
				local item = self.m_List:GetSubItem(itemnum, 0)
				local wnd = CreateNpcDialogBoxListCtrlItemListItem(item)
				item:EnableWnd(false)
				wnd.m_Text:SetWndTextColor(Flag, 0xff999999)
				local level = Quest_Common(p, "任务等级")
				level = g_RetNoneMercSentenceParse(level)
				local str = "("..level..")"..g_GetLanQuestName(p)
				wnd.m_Text:SetWndText(GetStaticTextClient(1023) .. ":" .. str)
				local icon = SQRDialog:new()
				icon:CreateFromRes("QuestTypePic3", wnd.m_Icon)
				icon:ShowWnd(true)
				item.m_WndTbl = {wnd,icon}
				itemnum = itemnum + 1
			end
		end
	end
	
	local rect = CFRect:new()
	self:GetLogicRect(rect)
	rect.bottom = rect.top + itemnum*30
	self.m_List:SetLogicRect(rect)
	self:SetLogicRect(rect)
	return canFinishQuestNum+canTakeQuestNum,doingquestnum
end
---------------------------------
function CreateNpcDialogBoxListCtrlItemListItem(parent)
	local wnd = CNpcDialogBoxListCtrlItemListItem:new()
	wnd:CreateFromRes("NpcDialogBoxListCtrlItemListItem", parent)
	wnd:ShowWnd(true)
	wnd.m_Text = wnd:GetDlgChild("Text")
	wnd.m_Icon = wnd:GetDlgChild("Icon")
	return wnd
end

--NPC功能栏-------------------------
function CNpcDialogBox:CreateNpcFuncListCtrlItem(parent, fatherPanel)
	local wnd = CNpcFuncListCtrlItem:new()
	wnd:CreateFromRes("NpcDialogBoxListCtrlItem", parent)
	wnd:ShowWnd(true)
	wnd.m_FatherPanel = fatherPanel
	wnd.m_List = wnd:GetDlgChild("List")
	return wnd
end

function CNpcFuncListCtrlItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == ITEM_LBUTTONCLICK) then
		local item = self.m_List:GetSubItem(uParam1, 0)
		if item.m_Special then
			item.m_Wnd:RunSpecialFun()
			self.m_FatherPanel:ShowWnd(false)
			return
		end
		local Index = self.m_FatherPanel.TextType[uParam1 + 1]
		self.m_FatherPanel:RunLinkFun(Index, uParam1)
		self.m_FatherPanel:ShowWnd(false)
	end
end

local NoneShowFun = {
	["无"] = true,
	["任务NPC"] = true,
	["佣兵指引登记"] = true,
}

function CNpcFuncListCtrlItem:DrawListItem(funcnametbl, uBuildingTongID)
	local funcnum = 0
	local IsSpecial = false
	local IsSkipNpcDlg = false
	local OpenFirstFun = false
	local TriggerType = nil
	self.m_List:DeleteAllItem()
	self.m_List:InsertColumn(0, 265)
	local rect = CFRect:new()
	self:GetLogicRect(rect)
	if funcnametbl and funcnametbl ~= "" then
		funcnametbl = loadstring("return {" .. funcnametbl.."}")()
		
		if( 0 ~= uBuildingTongID ) then
			local nPlayerTongID = g_MainPlayer.m_Properties:GetTongID()
			if ( nPlayerTongID == uBuildingTongID ) then
				if ( type(funcnametbl[1]) == "table" ) then
					funcnametbl = funcnametbl[1]
				end
			else
				if ( type(funcnametbl[1]) == "table" ) then
					funcnametbl = funcnametbl[2]
				else
					return 0
				end
			end
		end
		
		for i=1, #(funcnametbl) do
			local funcname = funcnametbl[i]
			if funcname == "佣兵指引登记" then
				if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
					if g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn then
						self.m_List:InsertItem(funcnum, 30)
						fundesc = self.m_FatherPanel.ENpcShowDlg(MemH64(funcname), "NpcShowDlg")
						self.m_FatherPanel.TextType[funcnum+1] = self.m_FatherPanel.DlgTextType[funcname]
						local item = self.m_List:GetSubItem(funcnum, 0)
						local wnd = CreateNpcDialogBoxListCtrlItemListItem(item)
						wnd.m_Text:SetWndText(fundesc)
						item.m_Wnd = wnd
						funcnum = funcnum + 1
						OpenFirstFun = true
						break
					end
				end
			end
		end
		
		if not IsSpecial and not OpenFirstFun then
			for i = 1,#(funcnametbl) do
				local funcname = funcnametbl[i]
				
				--if funcname ~= "特殊NPC" then
					if(not NoneShowFun[funcname] and self.m_FatherPanel.DlgTextType[funcname]) then
						self.m_List:InsertItem(funcnum, 30)
						fundesc = self.m_FatherPanel.ENpcShowDlg(MemH64(funcname), "NpcShowDlg")
						self.m_FatherPanel.TextType[funcnum+1] = self.m_FatherPanel.DlgTextType[funcname]
						local item = self.m_List:GetSubItem(funcnum, 0)
						local wnd = CreateNpcDialogBoxListCtrlItemListItem(item)
						if not wnd then
							return
						end
						wnd.m_Text:SetWndText(fundesc)
						item.m_Wnd = wnd
						if funcname == "技能升级学习Npc" then
							local icon = SQRDialog:new()
							icon:CreateFromRes("SkillLearnPic", wnd.m_Icon)
							icon:ShowWnd(true)
							wnd.m_Icon:ShowWnd(true)
							item.m_WndTbl = {wnd,icon}
						end
						funcnum = funcnum + 1
					end
				--end
				
				--if funcname == "佣兵指引定级" or funcname == "佣兵指引领特权" then
				--	IsSpecial = true
				--end
							
				if funcname == "特殊NPC" then
					if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingObj then
						IsSpecial = true
						
						local npc = g_MainPlayer.m_LockCenter.m_LockingObj
						local npcName = npc.m_Properties:GetCharName()
						local TargetEntityID = npc:GetEntityID()
						
						local NpcShowTaskCommonKey = NpcShowTask_Common:GetKeys()
						for j, index in pairs(NpcShowTaskCommonKey) do
							if index == npcName then
								local NpcShowTaskCommon = NpcShowTask_Common(npcName) 
								local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
								for i, _ in pairs(NpcShowTaskIndex) do
									local str = i..""
									local Data = NpcShowTaskCommon(str, "TaskType")
									local triggerType = NpcShowTaskCommon(str, "TriggerType")
									if Data == "Renovate" then
										local index = npcName .. triggerType
										local displayName = Lan_NpcShowTask_Common(MemH64(index), "TriggerTypeShowName")
										local funType = Data
										TriggerType = triggerType
										self.m_List:InsertItem(funcnum, 30)
										local item = self.m_List:GetSubItem(funcnum, 0)
										local skipNpcDlg = NpcShowTaskCommon(str, "SkipNpcDlg")
										if skipNpcDlg == 1 then
											IsSkipNpcDlg = true
										end
										item.m_Special = true
										local wnd = CSpecialNpc_DialogBox:new(item,displayName,funType,TriggerType,TargetEntityID)
										item.m_Wnd = wnd
										funcnum = funcnum + 1
									elseif Data == "Transmit" then
										if not g_GameMain.m_SpecialNpcWnd then
											g_GameMain.m_SpecialNpcWnd = CSpecialNpcWnd:new(g_GameMain)
										end
										g_GameMain.m_SpecialNpcWnd:InitWnd()
										break
									end
								end
							end
						end
					end
				end
			end
		end
		
	end
	rect.bottom = rect.top + 30*funcnum
	self.m_List:SetLogicRect(rect)
	self:SetLogicRect(rect)
	
	local IsShowList = nil
	if not IsSkipNpcDlg and IsSpecial and funcnum == 1 then
		IsShowList = true
	end
	return funcnum, OpenFirstFun, IsShowList, IsSkipNpcDlg, TriggerType
end

--------------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------------------------------
function RetShowFuncNpcOrObjTalkWnd(uEntityID, uBuildingTongID)
	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end
	
	local sTargetName = nil
	local sType = nil
	local NpcObj = CCharacterFollower_GetCharacterByID(uEntityID)
	if NpcObj then
		sTargetName = NpcObj.m_Properties:GetCharName()
		sType = "Npc"
	else 
		local IntObjClient = CIntObjClient_GetIntObjByID(uEntityID)
		if IntObjClient then
			sTargetName = IntObjClient.m_Properties:GetCharName()
			sType = "Obj"
		end
	end
	if sTargetName==nil then
		return
	end
	
	local canquest = CQuest.QueryQuest(sType,sTargetName,uEntityID) --可接的任务tbl
	local canfinishquest = CQuest.QueryFinishQuest(sType,sTargetName,uEntityID)
	local funcnametbl = nil --Npc固定功能类型
	if sType == "Npc" then
		funcnametbl = Npc_Common(sTargetName,"FuncName")
	end
--	g_GameMain.m_NpcDlg.m_TargetEntityID = uEntityID
	g_GameMain.m_NpcDlg:ShowWnd(false)
	g_GameMain.m_NpcDlg:InitNpcContent(uEntityID,sType,sTargetName, funcnametbl, canquest,canfinishquest, uBuildingTongID)
	
--	local Target = g_MainPlayer.m_LockCenter.m_LockingObj
	if sType == "Npc" then
		NpcObj:DoRespondActionBegin(g_MainPlayer)
	end
end
