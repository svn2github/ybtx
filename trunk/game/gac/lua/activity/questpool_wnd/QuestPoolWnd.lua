CQuestPoolWnd = class(SQRDialog)
local CQuestPoolCheckWnd = class(SQRDialog)
local CMercAcceptQuestWnd = class(SQRDialog)
local CMercTakeTaskTitleItem = class(SQRDialog)
local CMercTakeTaskLabelItem = class(SQRDialog)
local CMercTakeTaskRichTextItem = class(SQRDialog)
local CQuestPoolGoodsItem = class(SQRDialog)
local CQuestPoolGoodsAwardItem = class(SQRDialog)
local CQuestPoolWndListItem = class(SQRDialog)
local CQuestPoolNYDWnd = class(SQRDialog)

local QuestTextColor = g_ColorMgr:GetColor("任务面板文字")

--local InfoTextColor1 = ""--g_ColorMgr:GetColor("佣兵手册文字")
--local InfoTextColor2 = g_ColorMgr:Get16Color("佣兵手册文字")

--local NormalColor = g_ColorMgr:Get16Color("任务池普通任务名")
--local DareColor = g_ColorMgr:Get16Color("任务池挑战任务名")
--local SpecialColor = g_ColorMgr:Get16Color("任务池稀有任务名")

local function ChildQuestWnd(Parent)
	local Wnd = CMercAcceptQuestWnd:new()
	Wnd:CreateFromRes("MercenaryQuestWnd",Parent)
	Wnd.m_EncouragementList = Wnd:GetDlgChild("QuestInfoList")
	Wnd.m_TypeTitle = Wnd:GetDlgChild("TypeTitle")
	Wnd.m_MercenaryLevelName = Wnd:GetDlgChild("MercenaryLevel")
	Wnd.m_OkBtn = Wnd:GetDlgChild("OkBtn")
	--Wnd.m_NYDTitle = Wnd:GetDlgChild("NYDTitle")
	--Wnd.m_NYDInfo = Wnd:GetDlgChild("NYDInfo")
	--Wnd.m_Star = Wnd:GetDlgChild("Star")
	
	Wnd.m_tblIsHaveGoods = {false, false}
	Wnd:DrawEncouragementList()--初始化奖励List中的信息
	return Wnd
end

local function CreateQuestBtn(parent)
	local Wnd = CQuestPoolCheckWnd:new()
	Wnd:CreateFromRes("MercenaryQuest_CheckWnd",parent)
	Wnd.m_Title = Wnd:GetDlgChild("TiTle")
	Wnd.m_Single = Wnd:GetDlgChild("Single")
	--Wnd.m_CheckBtn = Wnd:GetDlgChild("CheckBtn")
	
	local rt = CFRect:new()
	parent:GetLogicRect(rt)
	Wnd:SetLogicRect(rt)
	
	local width = rt.right - rt.left
	local rt_1 = CFRect:new()
	Wnd.m_Title:GetLogicRect(rt_1)
	rt_1.right = rt_1.left + width
	Wnd.m_Title:SetLogicRect(rt_1)
	--rt.top = rt.top + 50
	--rt.bottom = rt.top + (height-50)
	--Wnd.m_CheckBtn:SetLogicRect(rt)
	Wnd.m_Single:ShowWnd(false)
	Wnd:ShowWnd(true)
	
	parent.m_CheckInfo = Wnd
end

function CQuestPoolWnd:Ctor(parent)
	self:CreateFromRes("MercenaryQuestPoolWnd",parent)
	--self.m_GeneralQuestList = self:GetDlgChild("GeneralQuestList")
	--self.m_QuestInfoBtn =  self:GetDlgChild("InfoBtn")
	--self.m_AcceptQuestBtn = self:GetDlgChild("AcceptBtn")
	self.m_ExitBtn = self:GetDlgChild("CloseBtn")
	
--	self.m_Static1 = self:GetDlgChild("Static1")
--	self.m_Static2 = self:GetDlgChild("Static2")
--	self.m_Static3 = self:GetDlgChild("Static3")
--	self.m_Static1:SetWndText("")--GetStaticTextClient(8337))
--	self.m_Static2:SetWndText(GetStaticTextClient(8338))
--	self.m_Static3:SetWndText(GetStaticTextClient(8341))
	--self.m_SpecialInfoBtn =  self:GetDlgChild("SpecialInfoBtn")
	
	
	self.m_CheckBtn = {}
	--左上角
	self.m_CheckBtn[1] = self:GetDlgChild("CheckBtn1")
	CreateQuestBtn(self.m_CheckBtn[1])
	--中间
	self.m_CheckBtn[3] = self:GetDlgChild("CheckBtn2")
	CreateQuestBtn(self.m_CheckBtn[3])
	--右上角
	self.m_CheckBtn[2] = self:GetDlgChild("CheckBtn3")
	CreateQuestBtn(self.m_CheckBtn[2])
	--左下角
	self.m_CheckBtn[4] = self:GetDlgChild("CheckBtn4")
	CreateQuestBtn(self.m_CheckBtn[4])
	--右下角
	self.m_CheckBtn[5] = self:GetDlgChild("CheckBtn5")
	CreateQuestBtn(self.m_CheckBtn[5])
	
	
	
	--初始化任务的子窗口
	self.m_QuestWnd = ChildQuestWnd(parent)
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CQuestPoolWnd:GetWnd()
	local Wnd = g_GameMain.m_MercenaryQuestPoolWnd
	if not Wnd then
		Wnd = CQuestPoolWnd:new(g_GameMain)
		g_GameMain.m_MercenaryQuestPoolWnd = Wnd
	end
	return Wnd
end

--任务领取窗口事件响应
function CQuestPoolWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ExitBtn then
			self:ShowWnd(false)
--		elseif Child == self.m_QuestInfoBtn then
--			if self.m_sQuestName then
--				self.m_QuestWnd:ShowWnd(true)
--				self.m_QuestWnd:SetFocus()
--				self.m_QuestWnd:ShowQuestPoolWnd(self.m_sQuestName,self.m_iTargetID)
--			end
--		elseif Child == self.m_AcceptQuestBtn then
--			if self.m_sQuestName then
--				Gac2Gas:AcceptQuest(g_Conn,self.m_sQuestName,self.m_iTargetID)
--			end
		else
			self.m_sQuestName = Child.m_QuestName
			if self.m_sQuestName then
				self.m_QuestWnd:ShowWnd(true)
				self.m_QuestWnd:SetFocus()
				self.m_QuestWnd:ShowQuestPoolWnd(self.m_sQuestName,self.m_iTargetID)
			end
		end
	end
end

local checkQuest = {
	["新佣兵的成长之路(艾米)"] = true,
	["新佣兵的成长之路(神圣)"] = true,
	["新佣兵的成长之路(修斯)"] = true,
}

function CQuestPoolWnd:InitCheckWnd(CheckBtn, QuestName, IsXYQuest)
	local str = "#c8d5536"..g_GetLanQuestName(QuestName)
	
	CheckBtn.m_CheckInfo.m_Title:SetWndText(str)
	CheckBtn.m_QuestName = QuestName
	CheckBtn.m_CheckInfo.m_Single:ShowWnd(IsXYQuest and true or false)
	
	
	if checkQuest[QuestName] then
		CheckBtn:SetFlashInfo(IP_UNCHECK_ENABLE, "yb_quest")
	end
	
	CheckBtn:ShowWnd(true)
end

function CQuestPoolWnd:VirtualExcludeWndClosed()
	self.m_QuestWnd:ShowWnd(false)
	--RetShowHintMsgWnd()
	g_GameMain.m_GuideBtn:BeAlertQuestEmptyAlert()
end

--任务领取窗口事件响应
function CMercAcceptQuestWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn then
			local res, msgID = g_CheckQuestShowName(self.m_sQuestName)
			if not res then
				if msgID then
					MsgClient(msgID)
				end
				return
			end
			Gac2Gas:AcceptQuest(g_Conn,self.m_sQuestName,self.m_iTargetID)
		end
		self:ShowWnd(false)
	end
end

----------------------------------------------------------
function CMercAcceptQuestWnd:DrawEncouragementList()
	local panel = self.m_EncouragementList
	panel:DeleteAllItem()
	panel:InsertColumn(0, 276)
	local tbl = {}
	for i = 0, 8 do
		panel:InsertItem(i, 30)
		local item = panel:GetSubItem(i, 0)
		table.insert(tbl, item)
	end
	
	self.m_TitleItem			 = self:CreateTakeTaskTitleItem(tbl[1])
	self.m_DisLabelItem    = self:CreateTakeTaskLabelItem(tbl[2])
	self.m_DisItem         = self:CreateTakeTaskRichTextItem(tbl[3])
	self.m_AwardLabelItem  = self:CreateTakeTaskLabelItem(tbl[4])
	self.m_AwardItem       = self:CreateTakeTaskRichTextItem(tbl[5])
	self.m_GoodsAwardItem = {}
	self.m_GoodsAwardItem[1] = self:CreateGoodsAwardItem(tbl[6])
	self.m_GoodsAwardItem[2] = self:CreateGoodsAwardItem(tbl[7])
	self.m_NYDLabelItem = self:CreateTakeTaskLabelItem(tbl[8])
	self.m_NYDItem = self:CreateTakeTaskRichTextItem(tbl[9])
	
	self.m_GoodsAwardItem[1].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1026))
	self.m_GoodsAwardItem[2].m_Title:SetWndText(QuestTextColor .. GetStaticTextClient(1027))
	self.m_GoodsAwardItem[1]:EnableWnd(false)
	self.m_GoodsAwardItem[2]:EnableWnd(false)
	self.m_DisLabelItem:SetWndText(GetStaticTextClient(1035))
	self.m_AwardLabelItem:SetWndText(GetStaticTextClient(1036))
	self.m_NYDLabelItem:SetWndText(GetStaticTextClient(9001))
	
	self.m_StarWndList = self:CreateNYDWnd(self.m_NYDLabelItem)
end

----------------------------------------------------------
--任务名栏
function CMercAcceptQuestWnd:CreateTakeTaskTitleItem(parent)
	local wnd= CMercTakeTaskTitleItem:new()
	wnd:CreateFromRes( "TakeTaskTitleItem", parent )
	wnd:ShowWnd(true)
	return wnd
end

--栏标题
function CMercAcceptQuestWnd:CreateTakeTaskLabelItem(parent)
	local wnd= CMercTakeTaskLabelItem:new()
	wnd:CreateFromRes( "TakeTaskLableItem", parent )
	wnd:ShowWnd(true)
	return wnd
end

--任务描述栏
function CMercAcceptQuestWnd:CreateTakeTaskRichTextItem(parent)
	local wnd= CMercTakeTaskRichTextItem:new()
	wnd:CreateFromRes( "TakeTaskRichTextItem", parent )
	wnd.m_RichText = wnd:GetDlgChild("RichText")
	wnd.m_RichText:SetRowDist(6)
	wnd.m_RichText:SetAutoWidth(266, 266)
	wnd:ShowWnd(true)
	return wnd
end

--奖励物品栏
function CMercAcceptQuestWnd:CreateGoodsAwardItem(parent)
	--parent:SetStyle(0x404C0000)
	local wnd= CQuestPoolGoodsAwardItem:new()
	wnd:CreateFromRes("TakeTaskGoodsAwardItem", parent)
	wnd:SetMouseWheel(true)
	
	wnd.m_Title = wnd:GetDlgChild("Title")
	wnd.m_tblGoodsAwardBack = {}
	wnd.m_tblGoodsAwardDis  = {}
	wnd.m_GoodsAward = {}
	for i = 1, 4 do
		local backWnd = wnd:GetDlgChild("ItemBtn" .. i)
		backWnd:SetMouseWheel(true)
		local disWnd  = wnd:GetDlgChild("DisStatic" .. i)
		--disWnd:SetStyle(0x65480002)
		local strWnd = wnd:GetDlgChild("Static" .. i)
		strWnd:SetParent(disWnd)
		strWnd:ShowWnd(false)
		wnd.m_tblGoodsAwardBack[i] = backWnd
		wnd.m_tblGoodsAwardDis[i]  = disWnd
		local ChildWnd = CQuestPoolGoodsItem:new()
		ChildWnd:CreateFromRes("CommonGridCut", backWnd)
		ChildWnd:ShowWnd( true )
		wnd.m_GoodsAward[i] = ChildWnd
	end
	
	wnd:ShowWnd(true)
	return wnd
end

--任务难易度(星星标志)
function CMercAcceptQuestWnd:CreateNYDWnd(Parent)
	local wnd= CQuestPoolNYDWnd:new()
	wnd:CreateFromRes( "MercenaryStarWnd",Parent)
	wnd.m_StarWnd = {}
	for i = 1, 5 do
		wnd.m_StarWnd[i] = wnd:GetDlgChild("Static" .. i)
	end
	
	wnd:ShowWnd(true)
	return wnd
end

function CMercAcceptQuestWnd:ShowStarWndNum(ShowNum)
	if ShowNum >= 1 and ShowNum <= 5 then
		for i=1, 5 do
			if ShowNum >= i then
				self.m_StarWndList.m_StarWnd[i]:ShowWnd(true)
			else
				self.m_StarWndList.m_StarWnd[i]:ShowWnd(false)
			end
		end
	else
		self.m_StarWndList:ShowWnd(false)
	end
end
-------------------------------------------------------------------------------------------------


--显示任务名和佣兵等级名
function CMercAcceptQuestWnd:SetTitleName(QuestName)
	--任务类型
	if g_AllMercenaryQuestMgr[QuestName] then
		self.m_TypeTitle:SetWndText(GetStaticTextClient(1182))
	elseif g_DareQuestMgr[QuestName] then
		self.m_TypeTitle:SetWndText(GetStaticTextClient(1183))
	elseif g_ActionQuestMgr[QuestName] then
		self.m_TypeTitle:SetWndText(GetStaticTextClient(1184))
	else
		self.m_TypeTitle:SetWndText(GetStaticTextClient(1181))
	end

	local Level = Quest_Common(QuestName, "任务等级")
	local MercLevel = Quest_Common(QuestName, "接任务最小佣兵等级")
	if Level and Level~="" then
		Level = g_RetNoneMercSentenceParse(Level)
		self.m_TitleItem:SetWndText("(lv"..Level..") "..g_GetLanQuestName(QuestName))
	end
	if MercLevel and MercLevel~="" then
		MercLevel = g_GetMercenaryLevelLanName(MercLevel)
		self.m_MercenaryLevelName:SetWndText(GetStaticTextClient(1034, MercLevel))
	else
		self.m_MercenaryLevelName:SetWndText("")
	end
end

--显示任务描述
function CMercAcceptQuestWnd:SetDiscriptionText(QuestName)
	local Discription = ""
	local LanQuetInfo = g_GetLanQuestInfo(QuestName.."任务接取对话")
	if LanQuetInfo then
		Discription = g_QuestInfoTextTransfer(LanQuetInfo)
	end
	self.m_DisItem.m_RichText:SetWndText(QuestTextColor .. Discription)
end

--显示奖励文字
function CMercAcceptQuestWnd:SetAwardText(QuestName)
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
function CMercAcceptQuestWnd:SetAwardGood(QuestName)
	local TblQuestAward = {}
	--任务的奖励
	if Quest_Common(QuestName, "物品奖励") then
		TblQuestAward[1] = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Subject") --必得物品
		TblQuestAward[2] = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Arg")     --选择物品
	else
		TblQuestAward = nil
	end
	
	self.m_tblIsHaveGoods = {false, false}
	
	if TblQuestAward then
		--物品奖励
		for j = 1, 2 do
			local ShowNum = 0
			
			if TblQuestAward[j] and next(TblQuestAward[j]) then
				self.m_tblIsHaveGoods[j] = true
				for i = 1, 4 do
					local grid   = self.m_GoodsAwardItem[j].m_GoodsAward[i]
					local gridBg = self.m_GoodsAwardItem[j].m_tblGoodsAwardBack[i]
					local disWnd = self.m_GoodsAwardItem[j].m_tblGoodsAwardDis[i]
					if not TblQuestAward[j][i] then
						grid:ShowWnd(false)
						gridBg:ShowWnd(false)
						disWnd:ShowWnd(false)
						disWnd:SetCheck(false)
					elseif next(TblQuestAward[j][i]) then
						grid:ShowWnd(true)
						gridBg:ShowWnd(true)
						disWnd:ShowWnd(true)
						disWnd:SetCheck(false)
						ShowNum = ShowNum + 1
						
						local itemtype = TblQuestAward[j][i][1]
						local itemname = TblQuestAward[j][i][2]
						local sName = itemname
    				if g_ItemInfoMgr:IsSoulPearl(itemtype) then
    					itemname= g_ItemInfoMgr:GetSoulpearlInfo(itemname)
    				end
						local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
						g_LoadIconFromRes(SmallIcon, grid, -1, IP_ENABLE, IP_DISABLE)
						local DisplayName = g_ItemInfoMgr:GetItemLanInfo(itemtype,itemname,"DisplayName")
						disWnd:SetWndText(DisplayName)
						grid:SetWndText(TblQuestAward[j][i][3])
						g_Tooltips:LoadIconFromRes(grid,SmallIcon)
						g_SetWndMultiToolTips(gridBg,itemtype, sName, 0)
						g_SetWndMultiToolTips(disWnd,itemtype, sName, 0)
					end
				end
			else
				for i=1, 4 do
					local grid   = self.m_GoodsAwardItem[j].m_GoodsAward[i]
					local gridBg = self.m_GoodsAwardItem[j].m_tblGoodsAwardBack[i]
					local disWnd = self.m_GoodsAwardItem[j].m_tblGoodsAwardDis[i]
					grid:ShowWnd(false)
					gridBg:ShowWnd(false)
					disWnd:ShowWnd(false)
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
	else
		for i=1, 2 do
			for j=1, 4 do
				local grid   = self.m_GoodsAwardItem[i].m_GoodsAward[j]
				local gridBg = self.m_GoodsAwardItem[i].m_tblGoodsAwardBack[j]
				local disWnd = self.m_GoodsAwardItem[i].m_tblGoodsAwardDis[j]
				grid:ShowWnd(false)
				gridBg:ShowWnd(false)
				disWnd:ShowWnd(false)
			end
		end
	end
end

function CMercAcceptQuestWnd:SetNYDInfo(QuestName)
	local iShowNumWnd = 1
	local Discription = GetStaticTextClient(9002)
	--if IsCppBound(g_MainPlayer) and QuestMgr["接任务最小佣兵等级"] then
	--	local PlayerYbLevel = g_MainPlayer.m_uMercenaryLevel
	--	local MinLevel = 1
	--	if QuestMgr["接任务最小佣兵等级"][1].Subject then
	--		MinLevel = QuestMgr["接任务最小佣兵等级"][1].Subject
	--		for i=1, #(MercenaryLevel_Common) do
	--			if MercenaryLevel_Common[i].MercenaryLevel == MinLevel then
	--				MinLevel = i
	--				break
	--			end
	--		end
	--	end
		
	--	if PlayerYbLevel > MinLevel then
	--		iShowNumWnd = 1
	--		Discription = GetStaticTextClient(9002)
	--	elseif PlayerYbLevel == MinLevel then
	--		iShowNumWnd = 3
	--		Discription = GetStaticTextClient(9003)
	--	else
	--		iShowNumWnd = 5
	--		Discription = GetStaticTextClient(9004)
	--	end
	--end
	
	self:ShowStarWndNum(iShowNumWnd)
	Discription = g_QuestInfoTextTransfer(Discription)
	self.m_NYDItem.m_RichText:SetWndText(QuestTextColor .. Discription)
end

--重新排版
function CMercAcceptQuestWnd:ReSetEncouragementList()
	if self.m_EncouragementList then --重新设定各个Item的高度
		self.m_EncouragementList:SetVerticalPos(0)
		self.m_EncouragementList:SetItemHeight(self.m_TitleItem:GetWndOrgHeight(), 0)
		self.m_EncouragementList:SetItemHeight(self.m_DisLabelItem:GetWndOrgHeight(), 1)
		self.m_EncouragementList:SetItemHeight(self.m_DisItem:GetWndOrgHeight(), 2)
		self.m_EncouragementList:SetItemHeight(self.m_AwardLabelItem:GetWndOrgHeight(), 3)
		self.m_EncouragementList:SetItemHeight(self.m_AwardItem:GetWndOrgHeight(), 4)
		self.m_EncouragementList:SetItemHeight(self.m_tblIsHaveGoods[1] and self.m_GoodsAwardItem[1]:GetWndOrgHeight() or 1, 5)
		self.m_EncouragementList:SetItemHeight(self.m_tblIsHaveGoods[2] and self.m_GoodsAwardItem[2]:GetWndOrgHeight() or 1, 6)
		self.m_EncouragementList:SetItemHeight(self.m_NYDLabelItem:GetWndOrgHeight(), 7)
		self.m_EncouragementList:SetItemHeight(self.m_NYDItem:GetWndOrgHeight(), 8)
		
		self.m_GoodsAwardItem[1]:ShowWnd(self.m_tblIsHaveGoods[1])
		self.m_GoodsAwardItem[2]:ShowWnd(self.m_tblIsHaveGoods[2])
	end
end

--显示面板
function CMercAcceptQuestWnd:ShowQuestPoolWnd(QuestName,ObjEntityID)
	self.m_sQuestName = nil
	self.m_iTargetID = nil
	if QuestName and QuestName ~= "" and self:IsShow() then
		if Quest_Common(QuestName) then
			self.m_sQuestName = QuestName
			self.m_iTargetID = ObjEntityID
			
			self:SetTitleName(QuestName)
			self:SetDiscriptionText(QuestName)
			self:SetAwardText(QuestName)
			self:SetAwardGood(QuestName)
			self:SetNYDInfo(QuestName)
			self:ReSetEncouragementList()
		end
	end
end

--===============================================
function CQuestPoolWnd:CreateListItem(parent)
	local wnd = CQuestPoolWndListItem:new()
	wnd:CreateFromRes("MatchGameSelWndListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CQuestPoolWnd:InsertListInfo(ListCtrl,QuestName,TextColor)
	if QuestName then
		local ItemCount = ListCtrl:GetItemCount()
		ListCtrl:InsertItem(ItemCount, 25)
		local Item = ListCtrl:GetSubItem(ItemCount, 0)
		Item:SetMouseWheel(true)
		local itemWnd = self:CreateListItem(Item)
		local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
		itemWnd:SetWndTextColor(Flag,TextColor)
		
		local str = ""
		local level2 = Quest_Common(QuestName, "接任务最小佣兵等级")
		if level2 and level2~="" then
			str = str.."("..level2..")"
		end
		local level1 = g_RetNoneMercSentenceParsee( Quest_Common(QuestName, "任务等级") )
		local str = str.."("..level1..")"..g_GetLanQuestName(QuestName)
		itemWnd:SetWndText(str)
		Item.m_QuestName = QuestName
		Item.m_ItemWnd = itemWnd
	end
end
--============================================================

--初始日常任务挑战列表
function CQuestPoolWnd:InitDateQuestList(QuestTbl)
	--self.m_GeneralQuestList:DeleteAllItem()
	--self.m_GeneralQuestList:InsertColumn(0, self.m_GeneralQuestList:GetWndOrgWidth())
	
	--按区域排序
	local AreaTbl = {}
	for i=1, #(QuestTbl) do
		tempQuestName = QuestTbl[i]
		if Quest_Common(tempQuestName, "所属区域")
			and Quest_Common(tempQuestName, "所属区域", "1")
			and Quest_Common(tempQuestName, "所属区域", "1") ~= ""
			then
				local area = Quest_Common(tempQuestName, "所属区域", "1")
				if not AreaTbl[area] then
					AreaTbl[area] = {}
				end
				table.insert(AreaTbl[area],tempQuestName)
		else
			if not AreaTbl["NoneArea"] then
				AreaTbl["NoneArea"] = {}
			end
			table.insert(AreaTbl["NoneArea"],tempQuestName)
		end
	end
	local tbl = {}
	for _,tempTbl in pairs(AreaTbl) do
		if tempTbl and next(tempTbl) then
			for i=1, #(tempTbl) do
				table.insert(tbl,tempTbl[i])
			end
		end
	end
	
	for i=1, #(tbl) do
		--self:InsertListInfo(self.m_GeneralQuestList,tbl[i],NormalColor)
		self:InitCheckWnd(self.m_CheckBtn[i+3],tbl[i])
	end
end

--删除一个佣兵任务
function CQuestPoolWnd:DeleteQuestPoolQuest(QuestName)
	if IsFunction(self.IsShow) and self:IsShow() then
		if QuestName and QuestName ~= "" then
			if g_AllMercenaryQuestMgr[QuestName] then
				
				for i=1, 5 do
					if self.m_CheckBtn[i].m_QuestName == QuestName then
						self.m_CheckBtn[i]:ShowWnd(false)
						self.m_CheckBtn[i].m_CheckInfo.m_Title:SetWndText("")
						self.m_QuestWnd:ShowWnd(false)
						self.m_sQuestName = nil
						break
					end
				end
				--1.
--				local IsDelete = false
--				local ItemCount = self.m_GeneralQuestList:GetItemCount()
--				for i=0, ItemCount-1 do
--					local Item = self.m_GeneralQuestList:GetSubItem(i, 0)
--					if Item.m_QuestName == QuestName then
--						IsDelete = true
--						Item.m_ItemWnd:DestroyWnd()
--						self.m_GeneralQuestList:DeleteItem(i+1)
--						break
--					end
--				end
--				
--				--3.
--				if IsDelete and self.m_sQuestName == QuestName then
--					local ItemCount = self.m_GeneralQuestList:GetItemCount()
--					if ItemCount == 0 then
--						self:ShowWnd(false)
--						return
--					else
--						local Item = self.m_GeneralQuestList:GetSubItem(0, 0)
--						self.m_sQuestName = Item.m_QuestName
--						self.m_QuestWnd:ShowQuestPoolWnd(self.m_sQuestName,self.m_iTargetID)
--					end
--				end
				
			end
		end
	end
end

--服务端会传送过来,类型为1,6,7的任务
local m_QuestList = nil
function CQuestPoolWnd:RetSendMercQuestPool(sQuestName,iQuestType)
	if not m_QuestList then
		m_QuestList = {}
	end
	if not m_QuestList[iQuestType] then
		m_QuestList[iQuestType] = {}
	end
	table.insert(m_QuestList[iQuestType],sQuestName)
end

function CQuestPoolWnd:RetShowMercQuestPoolWndEnd(ObjEntityID, XYQuestName)
	self.m_sQuestName = nil
	self.m_iTargetID = ObjEntityID
	
	if not IsCppBound(g_MainPlayer) then
		m_QuestList = nil
		return
	end
	local Camp = g_MainPlayer:CppGetBirthCamp()
	local Level = g_MainPlayer:CppGetLevel()
	for i=1, 5 do
		self.m_CheckBtn[i]:SetCheck(false)
		self.m_CheckBtn[i]:ShowWnd(false)
		self.m_CheckBtn[i]:DelFlashInfo(IP_UNCHECK_ENABLE)
		self.m_CheckBtn[i].m_CheckInfo.m_Title:SetWndText("")
	end
	
	local QuestNum = {0,0,0}
	if m_QuestList then
		
		for index=1, 3 do
			if m_QuestList[index] and next(m_QuestList[index]) then
				--过滤一下,已经接的任务
				for i=#(m_QuestList[index]), 1, -1 do
					if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[m_QuestList[index][i]] then
						table.remove(m_QuestList[index],i)
					end
				end
			end
		end
		
		if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo then
			for QuestName,_ in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
				if g_GeneralMercenaryQuestMgr[QuestName] or g_AreaMercenaryQuestMgr[QuestName] then
					QuestNum[1] = QuestNum[1] + 1
				elseif g_TeamMercenaryQuestMgr[QuestName] then
					if g_TeamMercenaryQuestMgr[QuestName].m_QuestIsRepeat == 1 then
						local QuestType = g_TeamMercenaryQuestMgr[QuestName].m_QuestType - 4
						QuestNum[QuestType] = QuestNum[QuestType] + 1
					end
				end
			end
		end
		
		--================		
		for i=2, 3 do
			if QuestNum[i] >= 2 then
				m_QuestList[i] = nil
			elseif QuestNum[i] == 1 then
				if m_QuestList[i] and #(m_QuestList[i]) > 1 then
					local TempQuest = m_QuestList[i][1]
					m_QuestList[i] = {}
					m_QuestList[i][1] = TempQuest
				end
			end
		end
		--=================
		for i=3, 3 do--现在只有领域任务,没有剧情任务了
			if m_QuestList[i] and m_QuestList[i][1] and QuestNum[i] == 0 then
				self:InitCheckWnd(self.m_CheckBtn[2],m_QuestList[i][1])
			end
		end
	end
	
	--显示不重复的小队佣兵任务
	local ShowMinQuest = {}
	for QuestName,Tbl in pairs(g_TeamMercenaryQuestMgr) do
		if Tbl.m_CampType == Camp then
			if not Tbl.m_QuestIsRepeat or Tbl.m_QuestIsRepeat == 0 then--不可重复的小队任务
				if CQuest.CheckQuestAvavilable(QuestName) then
					table.insert(ShowMinQuest, QuestName)
				end
			end
		end
	end
	for i=1, #(ShowMinQuest)-1 do
		for j=i+1, #(ShowMinQuest) do
			local Level1 = Quest_Common( ShowMinQuest[i], "接任务最小等级")
			local Level2 = Quest_Common( ShowMinQuest[i], "接任务最小等级")
			if Level1 > Level2 then
				local Temp = ShowMinQuest[i]
				ShowMinQuest[i] = ShowMinQuest[j]
				ShowMinQuest[j] = Temp
			end
		end
	end
	if ShowMinQuest[1] and ShowMinQuest[1] ~= "" then
	--	and (QuestNum[2] + QuestNum[3] < 2) then
			
		if not g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[ShowMinQuest[1]] then
			self:InitCheckWnd(self.m_CheckBtn[1],ShowMinQuest[1],true)
--			if QuestNum[2] + QuestNum[3] == 1 then
--				self.m_CheckBtn[2]:ShowWnd(false)
--				self.m_CheckBtn[2].m_CheckInfo.m_Title:SetWndText("")
--			end
		end
		
	end
	
	--佣兵面板下面两个按扭要显示的任务========================
	--显示不重复的日常佣兵任务
	local ShowQuestList = {}
	ShowMinQuest = {}
	for QuestName,Tbl in pairs(g_GeneralMercenaryQuestMgr) do
		if Tbl.m_CampType == Camp then
			if not Tbl.m_QuestIsRepeat or Tbl.m_QuestIsRepeat == 0 then--不可重复的小队任务
				if CQuest.CheckQuestAvavilable(QuestName) then
					table.insert(ShowMinQuest, QuestName)
				end
			end
		end
	end
	for i=1, #(ShowMinQuest)-1 do
		for j=i+1, #(ShowMinQuest) do
			local Level1 = Quest_Common( ShowMinQuest[i], "接任务最小等级")
			local Level2 = Quest_Common( ShowMinQuest[i], "接任务最小等级")
			if Level1 > Level2 then
				local Temp = ShowMinQuest[i]
				ShowMinQuest[i] = ShowMinQuest[j]
				ShowMinQuest[j] = Temp
			end
		end
	end
	table.insert(ShowQuestList,ShowMinQuest[1])
	table.insert(ShowQuestList,ShowMinQuest[2])
	--区域佣兵任务
	ShowMinQuest = {}
	for QuestName,Tbl in pairs(g_AreaMercenaryQuestMgr) do
		if Tbl.m_CampType == Camp then
			local strlev = Quest_Common( QuestName, "任务等级")
			if Level == g_RetNoneMercSentenceParse(strlev) then
				if CQuest.CheckQuestAvavilable(QuestName) then
					table.insert(ShowMinQuest, QuestName)
				end
			end
		end
	end
	for i=1, #(ShowMinQuest)-1 do
		for j=i+1, #(ShowMinQuest) do
			local Level1 = Quest_Common( ShowMinQuest[i], "接任务最小等级")
			local Level2 = Quest_Common( ShowMinQuest[i], "接任务最小等级")
			if Level1 > Level2 then
				local Temp = ShowMinQuest[i]
				ShowMinQuest[i] = ShowMinQuest[j]
				ShowMinQuest[j] = Temp
			end
		end
	end
	table.insert(ShowQuestList,ShowMinQuest[1])
	table.insert(ShowQuestList,ShowMinQuest[2])
	--显示日常任务
	if m_QuestList and m_QuestList[1] and next(m_QuestList[1]) then
		for j=1, #(m_QuestList[1]) do
			table.insert(ShowQuestList,m_QuestList[1][j])
		end
	end
	if QuestNum[1] >= 3 then
		ShowQuestList = {}
	elseif QuestNum[1] == 2 and #(ShowQuestList) > 1 then
		local TempQuest = ShowQuestList[1]
		ShowQuestList = {}
		ShowQuestList[1] = TempQuest
	elseif QuestNum[1] == 1 and #(ShowQuestList) > 2 then
		local TempQuest1 = ShowQuestList[1]
		local TempQuest2 = ShowQuestList[2]
		ShowQuestList = {}
		ShowQuestList[1] = TempQuest1
		ShowQuestList[2] = TempQuest2
	end
	
	for i=1, 3 do
		if ShowQuestList[i] then
			self:InitCheckWnd(self.m_CheckBtn[i+2],ShowQuestList[i],true)
		end
	end
	
	--显示稀有任务
	if XYQuestName ~= "" then
		if not g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[XYQuestName] then
			self:InitCheckWnd(self.m_CheckBtn[5],XYQuestName, true)
		end
	end
	--=============================================================
	
		--显示类型为第5类的佣兵任务(只有一个杀吟风的任务)
--	local Camp = 1
--	if IsCppBound(g_MainPlayer) then
--		Camp = g_MainPlayer:CppGetBirthCamp()
--	end
--	if g_MercenaryQuestMgr[Camp] then
--		for MapName,Tbl in pairs(g_MercenaryQuestMgr[Camp]) do
--			if Tbl[5] and next(Tbl[5]) then
--				for i=1,#(Tbl[5]) do
--					self:InitCheckWnd(self.m_CheckBtn[1],Tbl[5][1])
--				end
--				break
--			end
--		end
--	end

	--if not g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn then
		self:ShowWnd(true)
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
		g_GameMain.m_MercenaryLevelTrace:ShowWnd(false)
	--end
	m_QuestList = nil
end



local m_TempQuestList = nil
function CQuestPoolWnd:RetSendMercTempQuestPool(sQuestName, iQuestType)
	if not m_TempQuestList then
		m_TempQuestList = {}
	end
	if not m_TempQuestList[iQuestType] then
		m_TempQuestList[iQuestType] = {}
	end
	table.insert(m_TempQuestList[iQuestType],sQuestName)
end

local function CopyTbl(Tbl)
	if not IsTable(Tbl) then
		return Tbl
	end
	local TempTbl = {}
	for i,p in pairs(Tbl) do
		TempTbl[i] = CopyTbl(p)
	end
	return TempTbl
end

function CQuestPoolWnd:RetSendMercTempQuestPoolEnd(IsServer)
	if IsServer then
		self.m_TempQuestList = {}
		self.m_TempQuestList = m_TempQuestList
		m_TempQuestList = nil
	end
	
	--玩家身上的任务个数
	local QuestNum = {0,0,0}
	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo then
		for QuestName,_ in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
			if g_GeneralMercenaryQuestMgr[QuestName] or g_AreaMercenaryQuestMgr[QuestName] then
				QuestNum[1] = QuestNum[1] + 1
			elseif g_TeamMercenaryQuestMgr[QuestName] then
				if g_TeamMercenaryQuestMgr[QuestName].m_QuestIsRepeat == 1 then
					local QuestType = g_TeamMercenaryQuestMgr[QuestName].m_QuestType - 4
					QuestNum[QuestType] = QuestNum[QuestType] + 1
				end
			end
		end
	end
	
	self.m_IsShowMercQuest = false
	
	if self.m_TempQuestList then
		
		--拷贝出一份临时的table
		local QuestList = CopyTbl(self.m_TempQuestList)
		
		--要刷出来的任务个数
		local TempQuestNum = {0,0,0}
		for index=1, 3 do
			if QuestList[index] and next(QuestList[index]) then
				--过滤一下,已经接的任务
				for i=#(QuestList[index]), 1, -1 do
					local TempQuestName = QuestList[index][i]
					if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[TempQuestName] then
						table.remove(QuestList[index],i)
					end
				end
				--统计个数
				for i=#(QuestList[index]), 1, -1 do
					local TempQuestName = QuestList[index][i]
					if g_GeneralMercenaryQuestMgr[TempQuestName] or g_AreaMercenaryQuestMgr[TempQuestName] then
						TempQuestNum[1] = TempQuestNum[1] + 1
					elseif g_TeamMercenaryQuestMgr[TempQuestName] then
						if g_TeamMercenaryQuestMgr[TempQuestName].m_QuestIsRepeat == 1 then
							local QuestType = g_TeamMercenaryQuestMgr[TempQuestName].m_QuestType - 4
							TempQuestNum[QuestType] = TempQuestNum[QuestType] + 1
						end
					end
				end
			end
		end
		
		if QuestNum[1] < 3 and TempQuestNum[1] ~= 0 and TempQuestNum[1] >= QuestNum[1] then
			self.m_IsShowMercQuest = true
		else
			for i=2, 3 do
				if TempQuestNum[i] > 0 and QuestNum[i] == 0 then
					self.m_IsShowMercQuest = true
					break
				end
			end
--			if (QuestNum[2] + QuestNum[3]) < 2 
--				and (TempQuestNum[2] + TempQuestNum[3]) > (QuestNum[2] + QuestNum[3]) then
--					self.m_IsShowMercQuest = true
--			end
		end
	end
	
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local Camp = g_MainPlayer:CppGetBirthCamp()
	local Level = g_MainPlayer:CppGetLevel()
	
	--小队佣兵任务
	local ShowMinQuest = {}
	if not self.m_IsShowMercQuest then
		for QuestName,Tbl in pairs(g_TeamMercenaryQuestMgr) do
			if Tbl.m_CampType == Camp then
				if not Tbl.m_QuestIsRepeat or Tbl.m_QuestIsRepeat == 0 then--不可重复的小队任务
					if CQuest.CheckQuestAvavilable(QuestName) then
						table.insert(ShowMinQuest, QuestName)
					end
				end
			end
		end
		if next(ShowMinQuest) then --and (QuestNum[2] + QuestNum[3]) < 2 then
			self.m_IsShowMercQuest = true
		end
	end
	
	--日常佣兵任务
	if not self.m_IsShowMercQuest then
		local ShowQuestList = {}
		ShowMinQuest = {}
		for QuestName,Tbl in pairs(g_GeneralMercenaryQuestMgr) do
			if Tbl.m_CampType == Camp then
				if not Tbl.m_QuestIsRepeat or Tbl.m_QuestIsRepeat == 0 then--不可重复的小队任务
					if CQuest.CheckQuestAvavilable(QuestName) then
						table.insert(ShowMinQuest, QuestName)
					end
				end
			end
		end
		table.insert(ShowQuestList,ShowMinQuest[1])
		table.insert(ShowQuestList,ShowMinQuest[2])
		--区域佣兵任务
		ShowMinQuest = {}
		for QuestName,Tbl in pairs(g_AreaMercenaryQuestMgr) do
			if Tbl.m_CampType == Camp then
				local strlev = Quest_Common( QuestName, "任务等级")
				if Level == g_RetNoneMercSentenceParse(strlev) then
					if CQuest.CheckQuestAvavilable(QuestName) then
						table.insert(ShowMinQuest, QuestName)
					end
				end
			end
		end
		table.insert(ShowQuestList,ShowMinQuest[1])
		table.insert(ShowQuestList,ShowMinQuest[2])
		
		if QuestNum[1] >= 3 then
			ShowQuestList = {}
		elseif QuestNum[1] == 2 and #(ShowQuestList) > 1 then
			local TempQuest = ShowQuestList[1]
			ShowQuestList = {}
			ShowQuestList[1] = TempQuest
		elseif QuestNum[1] == 1 and #(ShowQuestList) > 2 then
			local TempQuest1 = ShowQuestList[1]
			local TempQuest2 = ShowQuestList[2]
			ShowQuestList = {}
			ShowQuestList[1] = TempQuest1
			ShowQuestList[2] = TempQuest2
		end
		if next(ShowQuestList) then
			self.m_IsShowMercQuest = true
		end
	end
	--print("IsServer",IsServer,"self.m_IsShowMercQuest",self.m_IsShowMercQuest)
	if IsServer then
		g_GameMain.m_NpcHeadSignMgr:UpdateNpcAndObjInView()
	end
end
