gac_require "information/tooltips/ToolTips"
gac_gas_require "framework/common/CMoney"

local QuestTextColor = g_ColorMgr:GetColor("任务面板文字")

CTakeTaskWnd 	    				    = class(SQRDialog)
local CTakeTaskTitleItem      = class(SQRDialog)
local CTakeTaskLabelItem      = class(SQRDialog)
local CTakeTaskRichTextItem   = class(SQRDialog)
local CTakeTaskGoodsAwardItem = class(SQRDialog)
local CTackTaskGoodsItem			= class(SQRDialog)

function CTakeTaskWnd:Ctor()
	self:CreateFromRes( "TakeTask", g_GameMain )
	self.m_tblIsHaveGoods = {false, false}
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CTakeTaskWnd:OnChildCreated()
	self.m_Accept          = self:GetDlgChild("Accept")
	self.m_Close           = self:GetDlgChild("Close")
	self.m_Deny            = self:GetDlgChild("Deny")
	self.m_TaskTitle       = self:GetDlgChild("TaskTitle")
	self.m_ListPanel       = self:GetDlgChild("ListPanel")
	self:DrawListPanel()
end

function CTakeTaskWnd:DrawListPanel()
	local panel = self.m_ListPanel
	panel:DeleteAllItem()
	panel:InsertColumn(0, 276)
	local tbl = {}
	for i = 0, 7 do
		panel:InsertItem(i, 30)
		table.insert(tbl, panel:GetSubItem(i, 0))
	end
	
	self.m_TitleItem         = self:CreateTakeTaskTitleItem(tbl[1])
	self.m_DisItem           = self:CreateTakeTaskRichTextItem(tbl[2])
	self.m_TarLabelItem      = self:CreateTakeTaskLabelItem(tbl[3])
	self.m_TarItem           = self:CreateTakeTaskRichTextItem(tbl[4])
	self.m_AwardLabelItem    = self:CreateTakeTaskLabelItem(tbl[5])
	self.m_AwardItem    		 = self:CreateTakeTaskRichTextItem(tbl[6])
	
	self.m_GoodsAwardItem = {}
	self.m_GoodsAwardItem[1] = self:CreateTakeTaskGoodsAwardItem(tbl[7], 1)
	self.m_GoodsAwardItem[2] = self:CreateTakeTaskGoodsAwardItem(tbl[8], 2)
	self.m_GoodsAwardItem[1].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1026))
	self.m_GoodsAwardItem[2].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1027))
	self.m_GoodsAwardItem[1]:EnableWnd(false)
	self.m_GoodsAwardItem[2]:EnableWnd(false)
	self.m_TarLabelItem:SetWndText(GetStaticTextClient(1033))
	self.m_AwardLabelItem:SetWndText(GetStaticTextClient(1028))
end

--任务领取窗口事件响应
function CTakeTaskWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK )	then
		--如果点击了拒绝按钮，则退出任务领取界面
		if(self.m_Deny == Child or self.m_Close == Child) then
			self:ShowWnd(false)
			--Gac2Gas:CancelTakeQuest(g_Conn)
			return
		elseif(self.m_Accept == Child) then
			local res, msgID = g_CheckQuestShowName(self.m_sQuestName)
			if not res then
				if msgID then
					MsgClient(msgID)
				end
				return
			end
			if Quest_Common(self.m_sQuestName, "触发任务方式", "Subject") == "Goods" then
				if not self.m_SceneName then
					Gac2Gas:RequestQuestByItem(g_Conn, self.m_sQuestName)
				else
					Gac2Gas:AcceptQuestJoinFb(g_Conn, self.m_sQuestName, self.m_SceneName)
				end
			else
				local GlobalID = g_QuestGiverGlobalID[self.m_sQuestName].GiverGlobalID
				Gac2Gas:AcceptQuest(g_Conn,self.m_sQuestName, GlobalID)
			end
			return
		end
	end
end

function CTakeTaskWnd:VirtualExcludeWndClosed()
	self.m_SceneName = nil
end

function CTakeTaskWnd:AcceptQuestReturn(QuestName)
	if self:IsShow() then
		self:ShowWnd(false)
		local sType = Quest_Common(QuestName, "触发任务方式", "Subject")
		local sName = Quest_Common(QuestName, "触发任务方式", "Arg")
		CQuest.CheckIsHaveContinualQuest(sType, sName)
	end
end

local ItemLevelTbl = {
	[1]={1,30},
	[2]={31,40},
	[3]={41,50},
	[4]={51,60},
	[5]={61,70},
	[6]={71,80},
}

function CTakeTaskWnd:ShowTakeQuestWnd(QuestName, TargetEntityID)
	self.m_TargetEntityID = TargetEntityID
--	self:ShowWnd(true)
	self.m_TarItem.m_QuestHypeLink = {}
	self.m_sQuestName = QuestName
	
	--任务类型
	if g_AllMercenaryQuestMgr[QuestName] then
		self.m_TaskTitle:SetWndText(GetStaticTextClient(1182))
	elseif g_DareQuestMgr[QuestName] then
		self.m_TaskTitle:SetWndText(GetStaticTextClient(1183))
	elseif g_ActionQuestMgr[QuestName] then
		self.m_TaskTitle:SetWndText(GetStaticTextClient(1184))
	else
		self.m_TaskTitle:SetWndText(GetStaticTextClient(1181))
	end
	
	--任务名称
	self.m_TitleItem:SetWndText(g_GetLanQuestName(QuestName))
	--任务目标描述
	local questTarDes = g_GetLanQuestInfo(QuestName.."任务目标描述")
	questTarDes = g_QuestInfoTextTransfer(questTarDes)
	
	local TempStr, TempQuetLink = g_SetKeyHypeLink(QuestName, questTarDes, QuestTextColor)
	TempStr = g_SetItemHypeLink(QuestName, TempStr, QuestTextColor)
	questTarDes = TempStr
	for KeyName, Tbl in pairs(TempQuetLink) do
		if not self.m_TarItem.m_QuestHypeLink[KeyName] then
			self.m_TarItem.m_QuestHypeLink[KeyName] = Tbl
		end
	end
	
	--local TempStr, TempItem = g_SetItemHypeLink(QuestName, questTarDes, "")
	--questTarDes = TempStr
	
	self.m_TarItem.m_RichText:SetWndText(QuestTextColor .. questTarDes)
	self:SetDiscriptionText(QuestName) --显示任务描述
	self:SetAwardText(QuestName)       --显示奖励文字
	
	local tblQuestInfo = {}
	if Quest_Common(QuestName, "物品奖励") then
		tblQuestInfo[1] = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Subject") --固定的物品奖励
		tblQuestInfo[2] = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Arg")     --可选择的物品奖励
	end
	
	if g_DareMercenaryQuestMgr[QuestName] and tblQuestInfo[1] then
		if IsCppBound(g_MainPlayer) then
			local Level = g_MainPlayer:CppGetLevel()
			local ItemIndex = nil
			for i=1, #(ItemLevelTbl) do
				if ItemLevelTbl[i][1] <= Level and ItemLevelTbl[i][2] >= Level then
					ItemIndex = i
					break
				end
			end
			local TempItemTbl = {}
			if ItemIndex and tblQuestInfo[1][ItemIndex] then
				TempItemTbl = tblQuestInfo[1][ItemIndex]
				tblQuestInfo = {}
				tblQuestInfo[1] = {}
				tblQuestInfo[1][1] = TempItemTbl
			else
				tblQuestInfo = {}
			end
		else
			tblQuestInfo = {}
		end
	end
	
	self.m_tblIsHaveGoods = {false, false}
	
	--物品奖励
	for j = 1, 2 do
		local ShowNum = 0
		for i = 1, 4 do
			local grid   = self.m_GoodsAwardItem[j].m_GoodsAward[i]
			local gridBg = self.m_GoodsAwardItem[j].m_tblGoodsAwardBack[i]
			local disWnd = self.m_GoodsAwardItem[j].m_tblGoodsAwardDis[i]
			if( not tblQuestInfo[j] or not tblQuestInfo[j][i] ) then
				grid:ShowWnd(false)
				gridBg:ShowWnd(false)
				disWnd:ShowWnd(false)
			elseif next(tblQuestInfo[j][i]) then
				ShowNum = ShowNum + 1
				self.m_tblIsHaveGoods[j] = true
				grid:ShowWnd(true)
				gridBg:ShowWnd(true)
				disWnd:ShowWnd(true)
				local itemtype = tblQuestInfo[j][i][1]
				local itemname = tblQuestInfo[j][i][2]
				local sName = itemname
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					sName = g_SoulPearlSentenceParse(sName, QuestName)
					itemname = g_ItemInfoMgr:GetSoulpearlInfo(sName)
				end
				local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
				g_LoadIconFromRes(SmallIcon, grid, -1, IP_ENABLE, IP_DISABLE)
				local DisplayName = g_ItemInfoMgr:GetItemLanInfo(itemtype,itemname,"DisplayName")
				disWnd:SetWndText(DisplayName)
				grid:SetWndText(tblQuestInfo[j][i][3])
				--tooltips 显示 正常显示
				g_Tooltips:LoadIconFromRes(gridBg,SmallIcon)
				g_SetWndMultiToolTips(gridBg, itemtype, sName, 0)
				g_SetWndMultiToolTips(disWnd, itemtype, sName, 0)
			end
		end
		
		if ShowNum ~= 0 then
			local rt = CFRect:new()
			local rt_1 = CFRect:new()
			self.m_GoodsAwardItem[j]:GetLogicRect(rt)
			self.m_GoodsAwardItem[j].m_tblGoodsAwardDis[ShowNum]:GetLogicRect(rt_1)
			rt.bottom = rt_1.bottom
			self.m_GoodsAwardItem[j]:SetLogicRect(rt)
		end
	end
	
	if self.m_tblIsHaveGoods[2] then
		local rt = CFRect:new()
		self.m_GoodsAwardItem[2]:GetLogicRect(rt)
		rt.bottom = rt.bottom + 50
		self.m_GoodsAwardItem[2]:SetLogicRect(rt)
	elseif self.m_tblIsHaveGoods[1] then
		local rt = CFRect:new()
		self.m_GoodsAwardItem[1]:GetLogicRect(rt)
		rt.bottom = rt.bottom + 50
		self.m_GoodsAwardItem[1]:SetLogicRect(rt)
	end
	
	self:ReTypeset() --排版
	self:ShowWnd(true)
	self:SetFocus()
	if not self.m_SceneName then
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
	end
end

--重新排版
function CTakeTaskWnd:ReTypeset()
	if(self.m_ListPanel) then --重新设定各个Item的高度
		self.m_ListPanel:SetVerticalPos(0)
		self.m_ListPanel:SetItemHeight(self.m_TitleItem:GetWndOrgHeight(), 0)
		self.m_ListPanel:SetItemHeight(self.m_DisItem:GetWndOrgHeight(), 1)
		self.m_ListPanel:SetItemHeight(self.m_TarLabelItem:GetWndOrgHeight(), 2)
		self.m_ListPanel:SetItemHeight(self.m_TarItem:GetWndOrgHeight(), 3)
		self.m_ListPanel:SetItemHeight(self.m_AwardLabelItem:GetWndOrgHeight(), 4)
		self.m_ListPanel:SetItemHeight(self.m_AwardItem:GetWndOrgHeight(), 5)
		self.m_ListPanel:SetItemHeight(self.m_tblIsHaveGoods[1] and self.m_GoodsAwardItem[1]:GetWndOrgHeight() or 1, 6)
		self.m_ListPanel:SetItemHeight(self.m_tblIsHaveGoods[2] and self.m_GoodsAwardItem[2]:GetWndOrgHeight() or 1, 7)
		self.m_GoodsAwardItem[1]:ShowWnd(self.m_tblIsHaveGoods[1])
		self.m_GoodsAwardItem[2]:ShowWnd(self.m_tblIsHaveGoods[2])
	end
end

--回调函数
function CTakeTaskWnd:OnSized()
	self:ReTypeset()
end

--显示任务描述
function CTakeTaskWnd:SetDiscriptionText(QuestName)
	local taskDiscription = ""
	local LanQuestInfo = g_GetLanQuestInfo(QuestName.."任务接取对话")
	if LanQuestInfo then
		taskDiscription = g_QuestInfoTextTransfer(LanQuestInfo)
	end
	self.m_DisItem.m_RichText:SetWndText(QuestTextColor .. taskDiscription)
end

--显示奖励
function CTakeTaskWnd:SetAwardText(QuestName)
	local _, awardString = g_GetQuestAwardInfoAndStr(QuestName)
	if awardString ~= "" then
		self.m_AwardItem.m_RichText:SetWndText(QuestTextColor .. awardString)
		self.m_AwardItem:ShowWnd(true)
		self.m_AwardLabelItem:ShowWnd(true)
	else
		self.m_AwardItem:ShowWnd(false)
		self.m_AwardLabelItem:ShowWnd(false)
	end
end

----------------------------------------------------------
--栏标题
function CTakeTaskWnd:CreateTakeTaskLabelItem(parent)
	local wnd= CTakeTaskLabelItem:new()
	wnd:CreateFromRes( "TakeTaskLableItem", parent )
	wnd:ShowWnd(true)
	return wnd
end
----------------------------------------------------------
--任务描述栏
function CTakeTaskWnd:CreateTakeTaskRichTextItem(parent)
	local wnd= CTakeTaskRichTextItem:new()
	wnd:CreateFromRes( "TakeTaskRichTextItem", parent )
	wnd:ShowWnd(true)
	return wnd
end

function CTakeTaskRichTextItem:OnChildCreated()
	self.m_RichText = self:GetDlgChild("RichText")
	--self.m_RichText:SetRowDist(6)
	self.m_RichText:SetAutoWidth(266, 266)
end

function CTakeTaskRichTextItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == RICH_CLICK) then
		if(Child == self.m_RichText) then
			local value = self.m_RichText:GetChooseStrAttr()
			local linktbl = self.m_QuestHypeLink[value]
			if(linktbl) then
--				if(g_GameMain.m_SceneName ~= linktbl[1]) then
--					MessageBox(g_App:GetRootWnd(),MsgBoxMsg(401), MB_BtnOK)
--				else
					PlayerAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])
--				end
			end
		end
	end
end
----------------------------------------------------------
--任务名栏
function CTakeTaskWnd:CreateTakeTaskTitleItem(parent)
	local wnd= CTakeTaskTitleItem:new()
	wnd:CreateFromRes( "TakeTaskTitleItem", parent )
	wnd:ShowWnd(true)
	return wnd
end

----------------------------------------------------------
--任务奖励物品栏
function CTakeTaskWnd:CreateTakeTaskGoodsAwardItem(parent, type)
	--parent:SetStyle(0x404C0000)
	local wnd= CTakeTaskGoodsAwardItem:new()
	wnd.m_Type = type
	wnd:CreateFromRes("TakeTaskGoodsAwardItem", parent)
	wnd:SetMouseWheel(true)
	wnd:ShowWnd(true)
	return wnd
end

function CTakeTaskGoodsAwardItem:OnChildCreated()
	self.m_Title = self:GetDlgChild("Title")
	self.m_tblGoodsAwardBack = {}
	self.m_tblGoodsAwardDis  = {}
	self.m_GoodsAward = {}
	for i = 1, 4 do
		local backWnd = self:GetDlgChild("ItemBtn" .. i)
		backWnd:SetMouseWheel(true)
		local disWnd  = self:GetDlgChild("DisStatic" .. i)
		--disWnd:SetStyle(0x65480002)
		local strWnd = self:GetDlgChild("Static" .. i)
		strWnd:SetParent(disWnd)
		strWnd:ShowWnd(false)
		self.m_tblGoodsAwardBack[i] = backWnd
		self.m_tblGoodsAwardDis[i]  = disWnd
		local wnd = CTackTaskGoodsItem:new()
		wnd:CreateFromRes("CommonGridCut", backWnd)
		wnd:ShowWnd(true)
		self.m_GoodsAward[i] = wnd
	end
end

--任务校验成功，显示任务领取窗口
function CTakeTaskWnd:RetShowTakeQuest(QuestName)
	self:ShowTakeQuestWnd(QuestName)
end

--远离NPC时关闭任务窗口
function CTakeTaskWnd:FarFromNPC()
	self:ShowWnd(false)
end

--佣兵挑战任务,其它队员打开任务接取面板
function CTakeTaskWnd:RetShowTakeDareQuest(QuestName, SceneName)
	self.m_SceneName = SceneName
	self:ShowTakeQuestWnd(QuestName)
end
