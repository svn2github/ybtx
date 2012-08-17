gac_require "message/message_box/MsgBoxMsg"
gac_gas_require "framework/common/CMoney"

local QuestTextColor = g_ColorMgr:GetColor("任务面板文字")
local FlashColor = string.sub(g_ColorMgr:GetColor("任务面板闪字提示"), 3, -1)
g_TblQuestAward = {} --任务完成后的物品奖励

CFinishTaskWnd           			  = class(SQRDialog)
local CFinishTakeTitleItem   		= class(SQRDialog)
local CFinishTaskLabelItem      = class(SQRDialog)
local CFinishTaskRichTextItem   = class(SQRDialog)
local CFinishTaskGoodsAwardItem = class(SQRDialog)
local CFinishTaskGoodsItem      = class(SQRDialog)
--local CFinishTaskGoodsAwardRed	= class(SQRDialog)

function CFinishTaskWnd:Ctor()
	self:CreateFromRes( "TakeTask", g_GameMain )
	self.m_SelGoodsAwardIndex = nil
	self.m_SelGoodsAwardChoosed = nil
	self.m_tblIsHaveGoods = {false, false}
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CFinishTaskWnd:OnChildCreated()
	self.m_Accept    = self:GetDlgChild("Accept")
	self.m_Close     = self:GetDlgChild("Close")
	self.m_Deny      = self:GetDlgChild("Deny")
	self.m_TaskTitle = self:GetDlgChild("TaskTitle")
	self.m_ListPanel = self:GetDlgChild("ListPanel")
	self.m_Accept:SetWndText(GetStaticTextClient(1024))
	self.m_Deny:SetWndText(GetStaticTextClient(1025))
	self:DrawListPanel()
end

function CFinishTaskWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK ) then
		--如果点击了完成按钮，则退出任务领取界面
		if(Child == self.m_Accept) then
			if(self.m_SelGoodsAwardChoosed or not g_TblQuestAward or not g_TblQuestAward[2] or not g_TblQuestAward[2][1]) then
				--判定玩家背包 是否有足够的地方存放任务奖励. 如果有人改这块的代码请注意.
--				if self:CheckPlayerPackSpace(self:GetAwardCount()) then
					if g_QuestFinishGlobalID[self.m_sQuestName] then
						local GlobalID = g_QuestFinishGlobalID[self.m_sQuestName].FinishGlobalID
						Gac2Gas:FinishQuest(g_Conn,self.m_sQuestName,self.m_SelGoodsAwardIndex, GlobalID)
					end
--					self.m_SelGoodsAwardIndex = nil
--				else
----					self.m_MsgBox = MessageBox( self, MsgBoxMsg(8003), MB_BtnOK)
--						MsgClient( 3003, "包裹空间不足")			
--				end
			else
				--self.m_MsgBox = MessageBox( self, MsgBoxMsg(8001), MB_BtnOK)
				self:RemindChooseAward()
			end
		elseif(Child == self.m_Close or Child == self.m_Deny)then
			self.m_SelGoodsAwardIndex = nil
			self.m_SelGoodsAwardChoosed = nil
			self:ShowWnd(false)
		end
	end
end

function CFinishTaskWnd:VirtualExcludeWndClosed()
	self.m_SelGoodsAwardIndex = nil
	self.m_SelGoodsAwardChoosed = nil
end

function CFinishTaskWnd:DrawListPanel()
	local panel = self.m_ListPanel
	panel:DeleteAllItem()
	panel:InsertColumn(0, 276)
	local tbl = {}
	for i = 0, 5 do
		panel:InsertItem(i, 30)
		local item = panel:GetSubItem(i, 0)
		table.insert(tbl, item)
	end
	self.m_TitleItem			 = self:CreateFinishTaskTitleItem(tbl[1])
	self.m_DisItem         = self:CreateFinishTaskRichTextItem(tbl[2])
	self.m_AwardLabelItem  = self:CreateFinishTaskLabelItem(tbl[3])
	self.m_AwardItem       = self:CreateFinishTaskRichTextItem(tbl[4])
	self.m_GoodsAwardItem  = {}
	self.m_GoodsAwardItem[1] = self:CreateFinishTaskGoodsAwardItem(tbl[5], 1, self)
	self.m_GoodsAwardItem[2] = self:CreateFinishTaskGoodsAwardItem(tbl[6], 2, self)
	self.m_GoodsAwardItem[1].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1026))
	self.m_GoodsAwardItem[2].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1152))
	self.m_GoodsAwardItem[1]:EnableWnd(false)
	self.m_AwardLabelItem:SetWndText(GetStaticTextClient(1028))
end

--显示任务描述
function CFinishTaskWnd:SetDiscriptionText(QuestName)
	local Discription = ""
	local QuestInfo = g_GetLanQuestInfo(QuestName.."任务完成对话")
	if QuestInfo then
		Discription = g_QuestInfoTextTransfer(QuestInfo)
	end
	self.m_DisItem.m_RichText:SetWndText(QuestTextColor .. Discription)
end

function CFinishTaskWnd:FinishQuestReturn(QuestName)
	self.m_SelGoodsAwardIndex = nil
	self.m_SelGoodsAwardChoosed = nil
	if self:IsShow() then
		self:ShowWnd(false)
		--local sType = Quest_Common(QuestName, "结束任务方式", "Subject")
		--local sName = Quest_Common(QuestName, "结束任务方式", "Arg")
		--CQuest.CheckIsHaveContinualQuest(sType,sName)
	end
end

--显示奖励文字
function CFinishTaskWnd:SetAwardText(QuestName)
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

--显示物品
function CFinishTaskWnd:SetAwardGood(QuestName)
	g_TblQuestAward = {}
	--任务的奖励
	if Quest_Common(QuestName, "物品奖励") then
		g_TblQuestAward[1] = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Subject") --必得物品
		g_TblQuestAward[2] = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Arg")     --选择物品
	else
		g_TblQuestAward = nil
	end
	
	if g_DareMercenaryQuestMgr[QuestName] and g_TblQuestAward and g_TblQuestAward[1] then
		local TempItemTbl = {}
		local ItemIndex = self.m_ItemIndex
		self.m_ItemIndex = nil
		if ItemIndex and g_TblQuestAward[1][ItemIndex] then
			TempItemTbl = g_TblQuestAward[1][ItemIndex]
			g_TblQuestAward[1] = {}
			g_TblQuestAward[1][1] = TempItemTbl
		else
			g_TblQuestAward = nil
		end
	end
	
	self.m_tblIsHaveGoods = {false, false}
	
	if(g_TblQuestAward) then
		for i = 1, 2 do
			if(next(g_TblQuestAward[i])) then
				self.m_tblIsHaveGoods[i] = true
			end
		end
	end
	
	local PlayerInfo = {["Id"] = g_MainPlayer.m_uID
						,["Class"] = g_MainPlayer:CppGetClass()
						,["Level"] = g_MainPlayer:CppGetLevel()
						,["Sex"] = g_MainPlayer.m_Properties:GetSex()
						,["Camp"] = g_MainPlayer:CppGetBirthCamp()
						,["IsInBattleState"] = g_MainPlayer:IsInBattleState()
						,["IsInForbitUseWeaponState"] = g_MainPlayer:IsInForbitUseWeaponState()}
						
	--物品奖励
	for j = 1, 2 do
		local ShowNum = 0
		
		for i = 1, 4 do
			local goodsAwardItem = self.m_GoodsAwardItem[j]
			goodsAwardItem.m_tblGoodsAwardBack[i]:SetCheck(false)
			goodsAwardItem.m_tblGoodsAwardDis[i]:SetCheck(false)
			if not g_TblQuestAward or not g_TblQuestAward[j][i] then
				goodsAwardItem.m_tblGoodsAwardBack[i]:ShowWnd(false)
				goodsAwardItem.m_tblGoodsAwardDis[i]:ShowWnd(false)
			elseif (next(g_TblQuestAward[j][i])) then
				ShowNum = ShowNum + 1
				goodsAwardItem.m_tblGoodsAwardBack[i]:ShowWnd(true)
				goodsAwardItem.m_tblGoodsAwardDis[i]:ShowWnd(true)
				goodsAwardItem.m_tblGoodsAwardDis[i]:SetCheck(false)
				--goodsAwardItem.m_GoodsAward[i].m_RedWnd:ShowWnd(false)
				local itemtype = g_TblQuestAward[j][i][1]
				local itemname = g_TblQuestAward[j][i][2]
				local sName = itemname
				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
					sName = g_SoulPearlSentenceParse(sName, QuestName)
					itemname = g_ItemInfoMgr:GetSoulpearlInfo(sName)
				end
				local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
				g_LoadIconFromRes(SmallIcon, goodsAwardItem.m_GoodsAward[i], -1, IP_ENABLE, IP_DISABLE)
				goodsAwardItem.m_GoodsAward[i]:SetWndText(g_TblQuestAward[j][i][3])
				local DisplayName = g_ItemInfoMgr:GetItemLanInfo(itemtype,itemname,"DisplayName")
				goodsAwardItem.m_tblGoodsAwardDis[i]:SetWndText(DisplayName)
				
				g_Tooltips:LoadIconFromRes(goodsAwardItem.m_GoodsAward[i], SmallIcon)
				g_SetWndMultiToolTips(goodsAwardItem.m_tblGoodsAwardBack[i],itemtype, sName, 0)
				
				g_SetWndMultiToolTips(goodsAwardItem.m_tblGoodsAwardDis[i],itemtype, sName, 0)
				
				if g_ItemInfoMgr:IsEquip(itemtype)
					and g_ItemInfoMgr:PlayerQuestItem(PlayerInfo,itemtype,itemname) then
						goodsAwardItem.m_tblGoodsAwardStr[i]:ShowWnd(true)
						--goodsAwardItem.m_GoodsAward[i].m_RedWnd:ShowWnd(true)
				else
					goodsAwardItem.m_tblGoodsAwardStr[i]:ShowWnd(false)
				end
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
end

--重新排版
function CFinishTaskWnd:ReTypeset()
	if(self.m_ListPanel) then --重新设定各个Item的高度
		self.m_ListPanel:SetVerticalPos(0)
		self.m_ListPanel:SetItemHeight(self.m_TitleItem:GetWndOrgHeight(), 0)
		self.m_ListPanel:SetItemHeight(self.m_DisItem:GetWndOrgHeight(), 1)
		self.m_ListPanel:SetItemHeight(self.m_AwardLabelItem:GetWndOrgHeight(), 2)
		self.m_ListPanel:SetItemHeight(self.m_AwardItem:GetWndOrgHeight(), 3)
		self.m_ListPanel:SetItemHeight(self.m_tblIsHaveGoods[1] and self.m_GoodsAwardItem[1]:GetWndOrgHeight() or 1, 4)
		self.m_ListPanel:SetItemHeight(self.m_tblIsHaveGoods[2] and self.m_GoodsAwardItem[2]:GetWndOrgHeight() or 1, 5)
		self.m_GoodsAwardItem[1]:ShowWnd(self.m_tblIsHaveGoods[1])
		self.m_GoodsAwardItem[2]:ShowWnd(self.m_tblIsHaveGoods[2])
	end
end

--回调函数
function CFinishTaskWnd:OnSized()
	self:ReTypeset()
end

--显示本面板
function CFinishTaskWnd:ShowFinishQuestWnd(QuestName, TargetEntityID)
	self.m_TargetEntityID = TargetEntityID
	self:ShowWnd(true)
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
	self:SetAwardGood(QuestName)
	self:SetDiscriptionText(QuestName)
	self:SetAwardText(QuestName)
	self.m_GoodsAwardItem[2].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1152))
	self:ReTypeset()
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
	SetEvent( Event.Test.QuestReceived,true)
	g_GameMain.m_finishTask:SetFocus()
end

function CFinishTaskWnd:CheckPlayerPackSpace( GoodsAwardCount)
	local Mgr = g_GameMain.m_BagSpaceMgr
	local Space = Mgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local freecount = Space:GetFreeGridCount()
	if GoodsAwardCount > freecount then
		return false
	end
	return true
end

function CFinishTaskWnd:GetAwardCount()
	--目前选择奖励只可单选
	local ChooseAwardCount = 0 
	if #(g_TblQuestAward[2]) > 0 then
		ChooseAwardCount = 1
	end
	return #(g_TblQuestAward[1]) + ChooseAwardCount
end

-------------------------------------------------------------------------------------------------------------------
--栏标题
function CFinishTaskWnd:CreateFinishTaskLabelItem(parent)
	local wnd= CFinishTaskLabelItem:new()
	wnd:CreateFromRes( "TakeTaskLableItem", parent )
	wnd:ShowWnd(true)
	return wnd
end
----------------------------------------------------------
--任务描述栏
function CFinishTaskWnd:CreateFinishTaskRichTextItem(parent)
	local wnd= CFinishTaskRichTextItem:new()
	wnd:CreateFromRes( "TakeTaskRichTextItem", parent )
	wnd:ShowWnd(true)
	return wnd
end

function CFinishTaskRichTextItem:OnChildCreated()
	self.m_RichText = self:GetDlgChild("RichText")
	self.m_RichText:SetRowDist(6)
	self.m_RichText:SetAutoWidth(266, 266)
end

function CFinishTaskRichTextItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == RICH_CLICK) then
		if(Child == self.m_RichText) then
			local value = self.m_RichText:GetChooseStr()
			local linktbl = self.m_QuestHypeLink[value]
			if(linktbl) then
--				if(g_GameMain.m_SceneName ~= linktbl[1]) then
--					MessageBox(g_App:GetRootWnd(),MsgBoxMsg(401), MB_BtnOK)
--				else
					QuestAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])
--				end
			end
		end
	end
end

----------------------------------------------------------
--任务名栏
function CFinishTaskWnd:CreateFinishTaskTitleItem(parent)
	local wnd= CFinishTakeTitleItem:new()
	wnd:CreateFromRes( "TakeTaskTitleItem", parent )
	wnd:ShowWnd(true)
	return wnd
end

----------------------------------------------------------
--任务奖励物品栏
function CFinishTaskWnd:CreateFinishTaskGoodsAwardItem(parent, type, fatherPanel)
	--parent:SetStyle(0x404C0000)
	local wnd= CFinishTaskGoodsAwardItem:new()
	wnd.m_Type = type
	wnd.m_FatherPanel = fatherPanel
	wnd:CreateFromRes("TakeTaskGoodsAwardItem", parent)
	wnd:SetMouseWheel(true)
	wnd:ShowWnd(true)
	return wnd
end

--local function CreateRedWnd(parent)
--	local wnd= CFinishTaskGoodsAwardRed:new()
--	wnd:CreateFromRes("TakeTask_GoodsAward_red", parent)
--	wnd:ShowWnd(false)
--	return wnd
--end

function CFinishTaskGoodsAwardItem:OnChildCreated()
	self.m_Title = self:GetDlgChild("Title")
	self.m_tblGoodsAwardBack = {}
	self.m_tblGoodsAwardDis  = {}
	self.m_tblGoodsAwardStr  = {}
	self.m_GoodsAward = {}
	for i = 1, 4 do
		local backWnd = self:GetDlgChild("ItemBtn" .. i)
		backWnd:SetMouseWheel(true)
		local disWnd  = self:GetDlgChild("DisStatic" .. i)
		local strWnd = self:GetDlgChild("Static" .. i)
		strWnd:SetParent(disWnd)
		strWnd:ShowWnd(false)
		--if(1 == self.m_Type) then
			--disWnd:SetStyle(0x65480002)
		--end
		self.m_tblGoodsAwardBack[i] = backWnd
		self.m_tblGoodsAwardDis[i]  = disWnd
		self.m_tblGoodsAwardStr[i]  = strWnd
		local wnd = CFinishTaskGoodsItem:new()
		wnd:CreateFromRes("CommonGridCut", backWnd)
		wnd:ShowWnd(true)
		--wnd.m_RedWnd = CreateRedWnd(wnd)
		self.m_GoodsAward[i] = wnd
	end
end

function CFinishTaskGoodsAwardItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK ) then
		if(1 == self.m_Type) then return end
		for i = 1, 4 do
			if(Child == self.m_tblGoodsAwardBack[i]) then
				self.m_FatherPanel.m_SelGoodsAwardIndex = i
				self.m_FatherPanel.m_SelGoodsAwardChoosed = true
				self.m_tblGoodsAwardDis[i]:SetCheck(true)
				self.m_FatherPanel.m_GoodsAwardItem[2].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1152))
				break
			elseif(Child == self.m_tblGoodsAwardDis[i]) then
				self.m_FatherPanel.m_SelGoodsAwardIndex = i
				self.m_FatherPanel.m_SelGoodsAwardChoosed = true
				self.m_tblGoodsAwardBack[i]:SetCheck(true)
				self.m_FatherPanel.m_GoodsAwardItem[2].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1152))
				break
			end
		end
	end
end

--校验成功，显示任务完成窗口
function CFinishTaskWnd:RetShowFinishQuest(QuestName, ItemIndex)
	self.m_ItemIndex = ItemIndex
	self:ShowFinishQuestWnd(QuestName)
end

function CFinishTaskWnd:RemindChooseAward()
	local str = "#b000#" .. FlashColor .. "#500" .. GetStaticTextClient(1152) .. "#b"
	self.m_GoodsAwardItem[2].m_Title:SetWndText(str)
	self.m_ListPanel:SetVerticalPos(1000)
end
