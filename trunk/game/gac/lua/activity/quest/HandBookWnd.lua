gac_gas_require "framework/common/CMoney"

cfg_load "area/AreaInfo_Client"
lan_load "quest/Lan_QuestHandBookDescription_Client"

CHandBookWnd = class (SQRDialog)
local CHandBookTabItem = class(SQRDialog)
local CHandBookAreaItem = class(SQRDialog)
local CHandBookQuestItem = class(SQRDialog)
local CHandBookQuestAwardItem = class(SQRDialog)
local CHandBookGridCut = class(SQRDialog)
local CHandBookNameBtnRichText = class(SQRDialog)

local TextColor = g_ColorMgr:GetColor("佣兵手册文字")
local AreaTblColor = g_ColorMgr:GetColor("佣兵手册区域标签文字颜色")
local m_IsAllQuestBrowse = nil
local m_IsMerenaryQuestType = nil

local m_FirstTblName = GetStaticTextClient(1180)--任务总览(右侧显示的名字)
local m_ZxTblName = GetStaticTextClient(1181)--主线任务
local m_YbTblName = GetStaticTextClient(1182)--佣兵任务
local m_TzTblName = GetStaticTextClient(1183)--挑战任务
local m_WfTblName = GetStaticTextClient(1184)--玩法任务

local function CreateHandBookNameBtnRichText(Parent,sQuestName,TabType)
	local Wnd = CHandBookNameBtnRichText:new()
	Wnd:CreateFromRes("HandBookNameBtnRichText",Parent)
	Wnd:ShowWnd( true )
	Wnd:SetStyle(0x60000000)
	Wnd.m_RichText = Wnd:GetDlgChild("m_RichText")
	Wnd.m_RichText:SetStyle(0x65000000)
	
	if TabType == 1 then--如果是区域名称
		Wnd.m_RichText:SetWndText(TextColor..sQuestName)
	else
		local areaName = nil
		if Quest_Common(sQuestName, "所属区域") then
			areaName = Quest_Common(sQuestName, "所属区域", "1")
		end
		local strInfo = g_GetLanQuestName(sQuestName)
		if areaName and areaName ~= "" then
			strInfo = strInfo .. "#r(".. GetAreaDispalyName(areaName) ..")"
		end
		Wnd.m_RichText:SetWndText(TextColor..strInfo)
	end
	
	Wnd.m_sName = sQuestName
	local rt_1 = CFRect:new()
	Parent:GetLogicRect(rt_1)
	Wnd:SetLogicRect(rt_1)
	Wnd.m_RichText:SetLogicRect(rt_1)
	return Wnd
end

--创建手册两边的标签按扭
local function CreateHandBookTabItem(Parent,itemwidth,sQuestName,TabType)
	local Wnd = CHandBookTabItem:new()
	Wnd.m_TabType = TabType
	if TabType == 1 then
		Wnd:CreateFromRes("HandBookAreaTabItem",Parent)
	else
		Wnd:CreateFromRes("HandBookQuestTabItem",Parent)
	end
	Wnd:ShowWnd( true )
	Wnd:SetStyle(0x40000000)
	Wnd:SetWndWidth(itemwidth)
	Wnd.m_NameCheckBtn = Wnd:GetDlgChild("NameCheckBtn")
	Wnd.m_NameCheckBtn.m_NameText = CreateHandBookNameBtnRichText(Wnd.m_NameCheckBtn,sQuestName,TabType)
	return Wnd
end

local function CreateHandBookAreaItem(Parent,itemwidth,TextStr)
	local Wnd = CHandBookAreaItem:new()
	Wnd:CreateFromRes("HandBookAreaItem",Parent)
	Wnd:ShowWnd( true )
	Wnd:SetStyle(0x40000000)
	Wnd:SetWndWidth(itemwidth)
	Wnd.m_InfoRichText = Wnd:GetDlgChild("InfoRichText")
	Wnd.m_InfoRichText:SetStyle(0x40000008)
	Wnd.m_InfoRichText:SetAutoWidth(itemwidth,itemwidth)
	Wnd.m_InfoRichText:SetWndText(TextColor..TextStr)
	return Wnd
end

local function CreateHandBookQuestItem(Parent,itemwidth,TypeStr,TextStr)
	local Wnd = CHandBookQuestItem:new()
	Wnd:CreateFromRes("HandBookQuestItem",Parent)
	Wnd:ShowWnd( true )
	Wnd:SetStyle(0x40000000)
	Wnd:SetWndWidth(itemwidth)
	Wnd.m_QuestInfoType = Wnd:GetDlgChild("QuestInfoType")
	Wnd.m_QuestInfoType:SetWndText(TypeStr)
	Wnd.m_QuestInfoText = Wnd:GetDlgChild("QuestInfoText")
	Wnd.m_QuestInfoText:SetStyle(0x40000008)
	Wnd.m_QuestInfoText:SetAutoWidth(itemwidth,itemwidth)
	Wnd.m_QuestInfoText:SetWndText(TextColor..TextStr)
	return Wnd
end

local function CreateHandBookQuestResAwardItem(Parent,IsSelItem)
	local Wnd = CHandBookQuestAwardItem:new()
	Wnd:CreateFromRes("HandBookQuestResAwardItem",Parent)
	Wnd:ShowWnd( true )
	Wnd.m_TypeText = Wnd:GetDlgChild("TypeText")
	Wnd:SetStyle(0x40000000)
	
	if IsSelItem then
		Wnd.m_TypeText:SetWndText(GetStaticTextClient(8300))--可选物品奖励：
	else
		Wnd.m_TypeText:SetWndText(GetStaticTextClient(8301))--固定物品奖励：
	end
	
	for i = 1,4 do
		Wnd["m_Res"..i] = Wnd:GetDlgChild("Res"..i)
		local Picturewnd = CHandBookGridCut:new()
		Picturewnd:CreateFromRes("CommonGridCut", Wnd["m_Res"..i])
		Picturewnd:ShowWnd( true )
		Wnd["m_Res"..i].m_Picture = Picturewnd
		Wnd["m_Res"..i]:ShowWnd(false)
	end
	return Wnd
end

--物品奖励那块功能GUI的初始化
local function CreateHandBookQuestAwardItem(Parent)
	local Wnd = CHandBookQuestAwardItem:new()
	Wnd:CreateFromRes("HandBookQuestAwardItem",Parent)
	Wnd:ShowWnd( true )
	Wnd:SetStyle(0x40000000)
	Wnd.m_ExpPic = Wnd:GetDlgChild("ExpPic")
	Wnd.m_SoulPic = Wnd:GetDlgChild("SoulPic")
	Wnd.m_MoneyPic = Wnd:GetDlgChild("MoneyPic")
	Wnd.m_BindMoneyPic = Wnd:GetDlgChild("BindMoneyPic")
	Wnd.m_TicketPic = Wnd:GetDlgChild("TicketPic")
	Wnd.m_MercIntegralPic = Wnd:GetDlgChild("MercIntegralPic")
	Wnd.m_ProfferPic = Wnd:GetDlgChild("ProfferPic")
	Wnd.m_BindMercTicketPic = Wnd:GetDlgChild("BindMercTicketPic")
	Wnd.m_DevelopmentPic = Wnd:GetDlgChild("DevelopmentPic")
	
	Wnd.m_ExpNum = Wnd:GetDlgChild("ExpNum")
	Wnd.m_SoulNum = Wnd:GetDlgChild("SoulNum")
	Wnd.m_MoneyNum = Wnd:GetDlgChild("MoneyNum")
	Wnd.m_BindMoneyNum = Wnd:GetDlgChild("BindMoneyNum")
	Wnd.m_TicketNum = Wnd:GetDlgChild("TicketNum")
	Wnd.m_MercIntegralNum = Wnd:GetDlgChild("MercIntegral")
	Wnd.m_ProfferNum = Wnd:GetDlgChild("ProfferNum")
	Wnd.m_BindMercTicketNum = Wnd:GetDlgChild("BindMercTicketNum")
	Wnd.m_DevelopmentNum = Wnd:GetDlgChild("DevelopmentNum")
	return Wnd
end

------佣兵手册面板的初始化------
function CHandBookWnd:Ctor()
	self:CreateFromRes("HandBookWnd", g_GameMain)
	self.m_AreaNameTabList = self:GetDlgChild("AreaNameTabList")
	self.m_AreaInfoList = self:GetDlgChild("AreaInfoList")
	self.m_QuestNameTabList = self:GetDlgChild("QuestNameTabList")
	self.m_QuestInfoList = self:GetDlgChild("QuestInfoList")
	self.m_QuestNameText = self:GetDlgChild("QuestNameText")
	self.m_QuestLevelText = self:GetDlgChild("QuestLevelText")
	self.m_QuestAreaText = self:GetDlgChild("QuestAreaText")
	self.m_AreaNameText = self:GetDlgChild("AreaNameText")
	self.m_AreaPitureText = self:GetDlgChild("AreaPitureText")
	self.m_QuestTrackBtn = self:GetDlgChild("QuestTrackBtn")
	self.m_QuestShareBtn = self:GetDlgChild("QuestShareBtn")
	self.m_QuestGiveUpBtn = self:GetDlgChild("QuestGiveUpBtn")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	--self.m_SceneLastArea = {}
	--self.m_AreaLastQuest = {}
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

--快捷键打开区域面板
function CHandBookWnd:BeOpenWnd()
	if self:IsShow() then
		self:ShowWnd(false)
	else
		if self.m_QuestName then
			if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[self.m_QuestName] then
				self:InitHandBookWndByQuest(self.m_QuestName)
				return
			else
				self.m_QuestName = nil
			end
		end
		self:InitHandBookWndByScene(g_GameMain.m_SceneName)
		if self.m_QuestName then
			self:ShowWnd(true)
			self:SetFocus()
		else
			local QuestInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
			if QuestInfoTbl and next(QuestInfoTbl) then
				for QuestName,_ in pairs(QuestInfoTbl) do
					self:InitHandBookWndByQuest(QuestName)
					break
				end
			else
				MsgClient(3044)--你现在没有接取任何任务
			end
		end
	end
end

function CHandBookWnd:InsertNameToList(CtrlList, ListName, ListType)
	local itemnum = CtrlList:GetItemCount()
	CtrlList:InsertItem(itemnum, 22)
	local item = CtrlList:GetSubItem(itemnum, 0 )
	local NameWnd = CreateHandBookTabItem(item,itemwidth,ListName,ListType)
	local ItemHeight = NameWnd:GetWndOrgHeight()
	item.m_TblWnd = NameWnd
	CtrlList:SetItemHeight(ItemHeight, itemnum)
end

function CHandBookWnd:ClearWndInfo()
	self.m_AreaNameTabList:DeleteAllItem()
	self.m_AreaInfoList:DeleteAllItem()
	self.m_QuestNameTabList:DeleteAllItem()
	self.m_QuestInfoList:DeleteAllItem()
	self.m_AreaNameText:SetWndText("")
	self.m_QuestNameText:SetWndText("")
	self.m_AreaPitureText:SetWndText("")
	self.m_QuestLevelText:SetWndText("")
	self.m_QuestAreaText:SetWndText("")
	self.m_AreaPitureText:ShowWnd(false)
	self.m_QuestName = nil
	
	local itemwidth = self.m_AreaNameTabList:GetWndOrgWidth()
	self.m_AreaNameTabList:InsertColumn(0,itemwidth)
	
	--self:InsertNameToList(self.m_AreaNameTabList,m_FirstTblName,1)
	
	local ShowTbl = {false,false,false,false}
	
	local QuestInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
	if QuestInfoTbl and next(QuestInfoTbl) then
		for QuestName,_ in pairs(QuestInfoTbl) do
			if g_AllMercenaryQuestMgr[QuestName] then
				ShowTbl[2] = m_YbTblName
			elseif g_DareQuestMgr[QuestName] then
				ShowTbl[3] = m_TzTblName
			elseif g_ActionQuestMgr[QuestName] then
				ShowTbl[4] = m_WfTblName
			else
				ShowTbl[1] = m_ZxTblName
			end
		end
	end
	for i=1, #(ShowTbl) do
		local TblName = ShowTbl[i]
		if TblName then
			self:InsertNameToList(self.m_AreaNameTabList,TblName,1)
		end
	end
end

function CHandBookWnd:ClearNoneWndListTbl(QuestName)
	local AreaList = self.m_AreaNameTabList
	local QuestList = self.m_QuestNameTabList
	
	--1.先删除要去掉的任务
	local itemnum = QuestList:GetItemCount()
	for i=1, itemnum do
		local item = QuestList:GetSubItem(i-1,0)
		local WndText = item.m_TblWnd.m_NameCheckBtn.m_NameText.m_sName
		if WndText == QuestName then
			QuestList:DeleteItem(i)
			break
		end
	end
	
	--2.看这个大类中是否还有任务
	local itemnum = QuestList:GetItemCount()
	if itemnum > 0 then--有任务
		
		local areaitemnum = AreaList:GetItemCount()
		local SelAreaName = m_FirstTblName
		for i=1, areaitemnum do
			local item = AreaList:GetSubItem(i-1,0)
			if item.m_TblWnd.m_NameCheckBtn:GetCheck() then
				SelAreaName = item.m_TblWnd.m_NameCheckBtn.m_NameText.m_sName
				break
			end
		end
		self:InitHandBookWndByAllQuest(SelAreaName)
		
	else--没有任务了
		self:ShowWnd(false)
--		local QuestInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
--		if QuestInfoTbl then
--			--删除掉无用的左侧标签
--			local QuestType = m_ZxTblName
--			if g_AllMercenaryQuestMgr[QuestName] then
--				QuestType = m_YbTblName
--			elseif g_DareQuestMgr[QuestName] then
--				QuestType = m_TzTblName
--			elseif g_ActionQuestMgr[QuestName] then
--				QuestType = m_WfTblName
--			end
--			
--			local areaitemnum = AreaList:GetItemCount()
--			for i=1, areaitemnum do
--				local item = AreaList:GetSubItem(i-1,0)
--				local DelAreaName = item.m_TblWnd.m_NameCheckBtn.m_NameText.m_sName
--				if DelAreaName == QuestType then
--					AreaList:DeleteItem(i)
--					break
--				end
--			end
--			self:InitHandBookWndByAllQuest(m_FirstTblName)
--		else
--			self:ShowWnd(false)
--		end
		
	end
end

function CHandBookWnd:InitQuestNameTabList(QuestTbl)
	self.m_QuestNameTabList:DeleteAllItem()
	local itemwidth = self.m_QuestNameTabList:GetWndOrgWidth()
	self.m_QuestNameTabList:InsertColumn(0,itemwidth)
	for i = 1,#(QuestTbl) do
		local QuestName = QuestTbl[i]
		self:InsertNameToList(self.m_QuestNameTabList,QuestName,2)
	end
end

local function GetQuestHandBookAreaDescription(questName)
	if Lan_QuestHandBookDescription_Client(MemH64(questName)) == nil then
		return ""
	else
		return Lan_QuestHandBookDescription_Client(MemH64(questName), "AreaDescription")
	end
end

local function GetQuestHandBookQuestPostil(questName)
	if Lan_QuestHandBookDescription_Client(MemH64(questName)) == nil then
		return ""
	else
		return Lan_QuestHandBookDescription_Client(MemH64(questName), "QuestPostil")
	end
end

local function SetShowWnd(Wnd, NumWnd, Num)
	if Num ~= 0 then
		NumWnd:SetWndText(Num) 
		Wnd:ShowWnd(true)
		NumWnd:ShowWnd(true)
	else
		Wnd:ShowWnd(false)
		NumWnd:ShowWnd(false)
	end
end

local function SortTypeFun(QuestTbl)
	local tbl1 = {}
	local tbl2 = {}
	for i=1, #(QuestTbl) do
		tempQuestName = QuestTbl[i]
		if g_AllMercenaryQuestMgr[tempQuestName] then
			table.insert(tbl1,tempQuestName)
		else
			table.insert(tbl2,tempQuestName)
		end
	end
	
	QuestTbl = tbl1
	for i=1, #(tbl2) do
		table.insert(QuestTbl,tbl2[i])
	end
	return QuestTbl
end

--按接任务时间进行追踪排序
function CHandBookWnd:ResetQuestTblOrder(QuestTbl)
	local tbl = {}
	local QuestStateTbl = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl
	for i = 1, #(QuestTbl) do
		if table.getn(tbl) > 0 then
			local AcceptTime_i = QuestStateTbl[QuestTbl[i]].m_AcceptTime
			for j = table.getn(tbl), 1 , -1 do
				local AcceptTime_j = QuestStateTbl[tbl[j]].m_AcceptTime
				if AcceptTime_i > AcceptTime_j then
					for n = table.getn(tbl), j+1, -1 do
						tbl[n+1] = tbl[n]
					end
					tbl[j+1] = QuestTbl[i]
					break
				elseif j == 1 then
					for n = table.getn(tbl), 1, -1 do
						tbl[n+1] = tbl[n]
					end
					tbl[1] = QuestTbl[i]
				end
			end
		else
			table.insert(tbl,QuestTbl[i])
		end
	end
	
	local SortType = self.m_SortType
	local tempQuestName = nil
	if SortType == 1 then--按等级排序
		for i=1, #(tbl) do
			for j=i+1, #(tbl) do
				local Subject1 = Quest_Common( tbl[i], "任务等级")
				local Subject2 = Quest_Common( tbl[j], "任务等级")
				local Level1 = g_RetNoneMercSentenceParse(Subject1)
				local Level2 = g_RetNoneMercSentenceParse(Subject2)
				if Level1 > Level2 then
					tempQuestName = tbl[i]
					tbl[i] = tbl[j]
					tbl[j] = tempQuestName
				end
			end
		end
	elseif SortType == 2 then--按类型排序
		tbl = SortTypeFun(tbl)
	elseif SortType == 3 then--按区域排序
		local AreaTbl = {}
		for i=1, #(tbl) do
			tempQuestName = tbl[i]
			if Quest_Common(tempQuestName, "所属区域")
				and Quest_Common(tempQuestName, "所属区域", "1")
				and Quest_Common(tempQuestName, "所属区域", "1") ~= ""
				then
					local AreaName = Quest_Common(tempQuestName, "所属区域", "1")
					if not AreaTbl[AreaName] then
						AreaTbl[AreaName] = {}
					end
					table.insert(AreaTbl[AreaName],tempQuestName)
			else
				if not AreaTbl["NoneArea"] then
					AreaTbl["NoneArea"] = {}
				end
				table.insert(AreaTbl["NoneArea"],tempQuestName)
			end
		end
		
		tbl = {}
		for _,tempTbl in pairs(AreaTbl) do
			if tempTbl and next(tempTbl) then
				tempTbl = SortTypeFun(tempTbl)
				for i=1, #(tempTbl) do
					table.insert(tbl,tempTbl[i])
				end
			end
		end
	end
	
	return tbl
end

function CHandBookWnd:InitHandBookWndByScene(SceneName)
	if Scene_Common[SceneName] == nil or SceneArea_Common_Shell(SceneName) == nil then
		return
	end
	
	local QuestTbl = {}
	local AreaTbl = g_SceneAreaList[SceneName]
	for AreaName, v in pairs(AreaTbl) do
		if g_AreaQuestMgr[AreaName] then
			
			for i , QuestName in pairs(g_AreaQuestMgr[AreaName]) do
				if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
					if not g_AllMercenaryQuestMgr[QuestName] then
						table.insert(QuestTbl,QuestName)
					end
				end
			end
			
		end
	end
	
	if next(QuestTbl) then
		local TempQuest = QuestTbl[1]
		self:InitHandBookWndByQuest(TempQuest)
	end
end

--修改区域信息的功能
function CHandBookWnd:InitAreaInfo(AreaName)
	self.m_AreaNameText:SetWndText(GetAreaDispalyName(AreaName))
	
	local FileName = ""
	if AreaInfo_Client(AreaName) then
		FileName = AreaInfo_Client(AreaName, "AreaPicture")
	end
	if FileName ~= "" then
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE
		local DefaultImage = WND_IMAGE_LIST:new()
		local TempFile = g_ImageMgr:GetImagePath(10005) ..FileName..".tex"
		DefaultImage:AddImage( self.m_AreaPitureText:GetGraphic(), -1,  TempFile, nil, CFPos:new(), "0xffffffff", 15 ) 	
		self.m_AreaPitureText:SetWndBkImage( Flag, DefaultImage )
		self.m_AreaPitureText:ShowWnd(true)
	end
	
	local AreaDescriptionTbl = {}
	if g_AreaQuestMgr[AreaName] then
		for i , QuestName in pairs(g_AreaQuestMgr[AreaName]) do
			if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
				if QuestHandBookDescription_Client(QuestName)
					and GetQuestHandBookAreaDescription(QuestName) ~= "" then
					local str = GetQuestHandBookAreaDescription(QuestName)
					table.insert(AreaDescriptionTbl,str)
				end
			end
		end
	end
	
	if #(AreaDescriptionTbl) > 0 then
		local itemwidth = self.m_AreaInfoList:GetWndOrgWidth()
		self.m_AreaInfoList:InsertColumn(0,itemwidth)
		local itemnum = 0
		
		for i = 1, #(AreaDescriptionTbl) do
			local TextStr = AreaDescriptionTbl[i]
			self.m_AreaInfoList:InsertItem(itemnum, 22)
			local item = self.m_AreaInfoList:GetSubItem(itemnum, 0 )
			local InfoWnd = CreateHandBookAreaItem(item,itemwidth,TextStr)
			local ItemHeight = InfoWnd:GetWndOrgHeight()
			item.m_TblWnd = InfoWnd
			self.m_AreaInfoList:SetItemHeight(ItemHeight, itemnum)
			itemnum = itemnum+1
		end
	end
end

function CHandBookWnd:InitQuestAwardInfo(itemnum,itemwidth,QuestName)
	if not IsCppBound(g_MainPlayer) then
		return 0
	end
	
	self.m_QuestInfoList:InsertItem(itemnum,22)
	local item = self.m_QuestInfoList:GetSubItem(itemnum,0)
	local AwardItem = CreateHandBookQuestAwardItem(item)
	
	local AwardInfo = g_GetQuestAwardInfo(QuestName)
	SetShowWnd(AwardItem.m_ExpPic, AwardItem.m_ExpNum, AwardInfo.Exp)
	SetShowWnd(AwardItem.m_SoulPic, AwardItem.m_SoulNum, AwardInfo.Soul)
	SetShowWnd(AwardItem.m_MoneyPic, AwardItem.m_MoneyNum, 0 == AwardInfo.Money and 0 or g_MoneyMgr:ChangeMoneyToString(AwardInfo.Money, EGoldType.GoldCoin))
	SetShowWnd(AwardItem.m_BindMoneyPic, AwardItem.m_BindMoneyNum, 0 == AwardInfo.BindMoney and 0 or g_MoneyMgr:ChangeMoneyToString(AwardInfo.BindMoney, EGoldType.GoldBar))
	SetShowWnd(AwardItem.m_TicketPic, AwardItem.m_TicketNum, AwardInfo.BindTicket)
	SetShowWnd(AwardItem.m_MercIntegralPic, AwardItem.m_MercIntegralNum, AwardInfo.MercIntegral)
	SetShowWnd(AwardItem.m_ProfferPic, AwardItem.m_ProfferNum, AwardInfo.TongProffer)
	SetShowWnd(AwardItem.m_BindMercTicketPic, AwardItem.m_BindMercTicketNum, AwardInfo.BindMercTicket)
	SetShowWnd(AwardItem.m_DevelopmentPic, AwardItem.m_DevelopmentNum, AwardInfo.Development)
	
	local ItemHeight = AwardItem:GetWndOrgHeight()
	item.m_TblWnd = AwardItem
	self.m_QuestInfoList:SetItemHeight(ItemHeight, itemnum)
	itemnum = itemnum+1
	
	if Quest_Common(QuestName, "物品奖励") then
		local mustaward = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Subject")
		local selectaward = GetCfgTransformValue(true, "Quest_Common", QuestName, "物品奖励", "Arg")
--		if #(mustaward) > 0 or #(selectaward) > 0 then
--			ItemHeight = AwardItem:GetWndOrgHeight()
--			AwardItem.m_MustResAward:ShowWnd(true)
--			AwardItem.m_SelectResAward:ShowWnd(true)
--		end
		if g_DareMercenaryQuestMgr[QuestName] and g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName] then
			local YbItemIndex = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName].m_YbItemIndex
			if YbItemIndex and YbItemIndex ~= 0 and mustaward[YbItemIndex] then
				local TempItemTbl = {}
				TempItemTbl = mustaward[YbItemIndex]
				mustaward = {}
				mustaward[1] = TempItemTbl
			else
				mustaward = nil
			end
		end
		
		if mustaward and #(mustaward) > 0 then
			self.m_QuestInfoList:InsertItem(itemnum,22)
			local item = self.m_QuestInfoList:GetSubItem(itemnum,0)
			local AwardItem = CreateHandBookQuestResAwardItem(item, false)
			local ItemHeight = AwardItem:GetWndOrgHeight()
			item.m_TblWnd = AwardItem
			self.m_QuestInfoList:SetItemHeight(ItemHeight, itemnum)
			itemnum = itemnum+1
--			AwardItem.m_MustResAward:ShowWnd(true)
			for i = 1, 4 do
				if mustaward[i] then
					AwardItem["m_Res"..i]:ShowWnd(true)
					local picwnd = AwardItem["m_Res"..i].m_Picture
					local itemtype = mustaward[i][1]
					local itemname = mustaward[i][2]
					local sName = itemname
    			if g_ItemInfoMgr:IsSoulPearl(itemtype) then
    				sName = g_SoulPearlSentenceParse(sName, QuestName)
    				itemname = g_ItemInfoMgr:GetSoulpearlInfo(sName)
    			end
					local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
					g_LoadIconFromRes(SmallIcon, picwnd, -1, IP_ENABLE, IP_ENABLE)
					picwnd:SetWndText(mustaward[i][3])
					--tooltips 显示 正常显示
					g_Tooltips:LoadIconFromRes(AwardItem["m_Res"..i],SmallIcon)
					g_SetWndMultiToolTips(AwardItem["m_Res"..i],itemtype,sName,0)
				end
			end
		end
			
		if selectaward and #(selectaward) > 0 then
			self.m_QuestInfoList:InsertItem(itemnum,22)
			local item = self.m_QuestInfoList:GetSubItem(itemnum,0)
			local AwardItem = CreateHandBookQuestResAwardItem(item, true)
			local ItemHeight = AwardItem:GetWndOrgHeight()
			item.m_TblWnd = AwardItem
			self.m_QuestInfoList:SetItemHeight(ItemHeight, itemnum)
			itemnum = itemnum+1
			for i = 1, 4 do
				if selectaward[i] then
					AwardItem["m_Res"..i]:ShowWnd(true)
					local picwnd = AwardItem["m_Res"..i].m_Picture
					local itemtype = selectaward[i][1]
					local itemname = selectaward[i][2]
					local sName = itemname
					if g_ItemInfoMgr:IsSoulPearl(itemtype) then
						itemname = g_ItemInfoMgr:GetSoulpearlInfo(itemname)
					end
					local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemtype,itemname,"SmallIcon")
					g_LoadIconFromRes(SmallIcon, picwnd, -1, IP_ENABLE, IP_ENABLE)
					picwnd:SetWndText(selectaward[i][3])
					--tooltips 显示 正常显示
					g_Tooltips:LoadIconFromRes(AwardItem["m_Res"..i],SmallIcon)
					g_SetWndMultiToolTips(AwardItem["m_Res"..i],itemtype,sName,0)
				end
			end
		end
	end
	
	return itemnum
end

function CHandBookWnd:InitQuestDescription(itemnum,itemwidth,QuestName)
	local descriptionstr = g_GetLanQuestInfo(QuestName.."任务描述")
	descriptionstr = g_QuestInfoTextTransfer(descriptionstr)
	self.m_QuestInfoList:InsertItem(itemnum,22)
	local item = self.m_QuestInfoList:GetSubItem(itemnum,0)
	local DescriptionItem = CreateHandBookQuestItem(item,itemwidth,GetStaticTextClient(8307),"    "..descriptionstr)--任务描述：
	local ItemHeight = DescriptionItem:GetWndOrgHeight()
	item.m_TblWnd = DescriptionItem
	self.m_QuestInfoList:SetItemHeight(ItemHeight, itemnum)
	itemnum = itemnum+1
	return itemnum
end

function CHandBookWnd:InitQuestTarget(itemnum,itemwidth,QuestName)
	local targetdecription = g_GetLanQuestInfo(QuestName.."任务目标描述")
	targetdecription = g_QuestInfoTextTransfer(targetdecription)
	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
		local str = ""
		if g_QuestNeedMgr[QuestName] then
			for varname ,infotbl in pairs(g_QuestNeedMgr[QuestName]) do
				local num = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName][varname].DoNum
				local TrackStr = infotbl.TrackInfo
				--if IsTable(TrackStr) then
				--	TrackStr = infotbl.TrackInfo[1]
				--end
				str = str.."--"..TrackStr .." "..num.."/"..infotbl.Num.."#r"
			end
		end
		
		local BuffQuestMgr = g_BuffQuestMgr[QuestName]
		if BuffQuestMgr then
			for buffname,Tbl in pairs(BuffQuestMgr) do
				local TrackStr = Tbl.TrackInfo
				local NeedNum = Tbl.Num
				local num = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName][buffname].DoNum
				str = str .. "--" .. TrackStr .." " .. num .."/" .. NeedNum .."#r"
			end	
		end
--		targetdecription = targetdecription..str
	end
	
	local TempStr, TempQuetLink = g_SetKeyHypeLink(QuestName, targetdecription, TextColor)
	targetdecription = TempStr
	for KeyName, Tbl in pairs(TempQuetLink) do
		if not self.m_QuestHypeLink[KeyName] then
			self.m_QuestHypeLink[KeyName] = Tbl
		end
	end
	
	local TempStr, TempItem = g_SetItemHypeLink(QuestName, targetdecription, TextColor)
	targetdecription = TempStr
	
	self.m_QuestInfoList:InsertItem(itemnum,22)
	local item = self.m_QuestInfoList:GetSubItem(itemnum,0)
	local DescriptionItem = CreateHandBookQuestItem(item,itemwidth,GetStaticTextClient(8308),"    "..targetdecription)--任务目标：
	local ItemHeight = DescriptionItem:GetWndOrgHeight()
	item.m_TblWnd = DescriptionItem
	self.m_QuestInfoList:SetItemHeight(ItemHeight, itemnum)
	itemnum = itemnum+1
	return itemnum
end

function CHandBookWnd:InitQuestPostil(itemnum,itemwidth,QuestName)
	if QuestHandBookDescription_Client(QuestName)
		and GetQuestHandBookQuestPostil(QuestName) ~= "" then
		local QuestPostil = GetQuestHandBookQuestPostil(QuestName)
		self.m_QuestInfoList:InsertItem(itemnum,22)
		local item = self.m_QuestInfoList:GetSubItem(itemnum,0)
		local QuestPostilItem = CreateHandBookQuestItem(item,itemwidth,GetStaticTextClient(8309),"    "..QuestPostil)--工会批注：
		local ItemHeight = QuestPostilItem:GetWndOrgHeight()
		item.m_TblWnd = QuestPostilItem
		self.m_QuestInfoList:SetItemHeight(ItemHeight, itemnum)
		itemnum = itemnum+1
	end
	return itemnum
end

function CHandBookWnd:ClearAreaInfo()
	self.m_AreaNameText:SetWndText("")
	self.m_AreaPitureText:SetWndText("")
	self.m_AreaPitureText:ShowWnd(false)
	self.m_AreaInfoList:DeleteAllItem()
end

function CHandBookWnd:InitQuestInfo(QuestName)
	self:ClearAreaInfo()
	
	self.m_QuestInfoList:DeleteAllItem()
	self.m_QuestHypeLink = {}
	self.m_QuestAreaText:SetWndText("")
	
	if QuestName == nil then
		return
	end
	
	self.m_QuestNameText:SetWndText(GetStaticTextClient(1029) .. ": " ..g_GetLanQuestName(QuestName))
	local QuestLevel = g_RetNoneMercSentenceParse( Quest_Common(QuestName, "任务等级") )
	self.m_QuestLevelText:SetWndText(GetStaticTextClient(1030) .. ": "..QuestLevel)
	local QuestArea = ""
	if Quest_Common(QuestName, "所属区域") then
		QuestArea = Quest_Common(QuestName, "所属区域", "1")
	end
	if QuestArea and QuestArea ~= "" then
		self.m_QuestAreaText:SetWndText(GetStaticTextClient(1031) .. ": " ..GetAreaDispalyName(QuestArea))
	end
	
	local itemwidth = self.m_QuestInfoList:GetWndOrgWidth()
	self.m_QuestInfoList:InsertColumn(0,itemwidth)
	local itemnum = 0
	itemnum = self:InitQuestAwardInfo(itemnum,itemwidth,QuestName)
	itemnum = self:InitQuestDescription(itemnum,itemwidth,QuestName)
	itemnum = self:InitQuestTarget(itemnum,itemwidth,QuestName)
	itemnum = self:InitQuestPostil(itemnum,itemwidth,QuestName)
	
	self.m_QuestName = QuestName
	
	self:SelectNameTabItem(self.m_QuestNameTabList,QuestName)
end

function CHandBookWnd:SelectNameTabItem(NameTabList,sName)
	local itemnum = NameTabList:GetItemCount()
	
	for i=1, itemnum do
		local item = NameTabList:GetSubItem(i-1,0)
		local WndText = item.m_TblWnd.m_NameCheckBtn.m_NameText.m_sName
		if WndText == sName then
			item.m_TblWnd.m_NameCheckBtn:SetCheck(true)
		else
			item.m_TblWnd.m_NameCheckBtn:SetCheck(false)
		end
	end
end

function CHandBookWnd:AddQuest(QuestName)
	if IsFunction(self.IsShow) and self:IsShow() then
		self:InitHandBookWndByQuest(self.m_QuestName)
	end
end

function CHandBookWnd:DeleteQuest(QuestName)
	if IsFunction(self.IsShow) and self:IsShow() then
		if QuestName == self.m_QuestName then
			self.m_QuestName = nil
			self:ClearNoneWndListTbl(QuestName)
		else
			self:InitHandBookWndByQuest(self.m_QuestName)
		end
	end
end

function CHandBookWnd:InitHandBookWndByAllQuest(SelTblName)
	self:SelectNameTabItem(self.m_AreaNameTabList,SelTblName)
	local QuestTbl = {}
	local QuestInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
	if QuestInfoTbl then
		for QuestName,_ in pairs(QuestInfoTbl) do
			local QuestType = m_ZxTblName
			if g_AllMercenaryQuestMgr[QuestName] then
				QuestType = m_YbTblName
			elseif g_DareQuestMgr[QuestName] then
				QuestType = m_TzTblName
			elseif g_ActionQuestMgr[QuestName] then
				QuestType = m_WfTblName
			end
			if SelTblName == m_FirstTblName or SelTblName == QuestType then
				table.insert(QuestTbl,QuestName)
			end
		end
	end
	
	if next(QuestTbl) then
		QuestTbl = self:ResetQuestTblOrder(QuestTbl)
		self:InitQuestNameTabList(QuestTbl)
		local QuestName = QuestTbl[1]
		self:InitQuestInfo(QuestName)
		if Quest_Common(QuestName, "所属区域") then
			local AreaName = Quest_Common(QuestName, "所属区域", "1")
			if AreaName and AreaName ~= "" then
				self:InitAreaInfo(AreaName)
			end
		end
	else
		self:ShowWnd(false)
	end
end

function CHandBookWnd:InitHandBookWndByQuest(QuestName)
	self:ClearWndInfo()
	if not IsCppBound(g_MainPlayer) then
		return
	end
	--标记当前的标签为选中状态
	local OpenTblName = m_ZxTblName
	if g_AllMercenaryQuestMgr[QuestName] then
		self:SelectNameTabItem(self.m_AreaNameTabList,m_YbTblName)
		OpenTblName = m_YbTblName
	elseif g_DareQuestMgr[QuestName] then
		self:SelectNameTabItem(self.m_AreaNameTabList,m_TzTblName)
		OpenTblName = m_TzTblName
	elseif g_ActionQuestMgr[QuestName] then
		self:SelectNameTabItem(self.m_AreaNameTabList,m_WfTblName)
		OpenTblName = m_WfTblName
	else
		self:SelectNameTabItem(self.m_AreaNameTabList,m_ZxTblName)
	end
	
	local QuestTbl = {}
	local QuestInfoTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
	if QuestInfoTbl then
		for QuestName,_ in pairs(QuestInfoTbl) do
			local QuestType = m_ZxTblName
			if g_AllMercenaryQuestMgr[QuestName] then
				QuestType = m_YbTblName
			elseif g_DareQuestMgr[QuestName] then
				QuestType = m_TzTblName
			elseif g_ActionQuestMgr[QuestName] then
				QuestType = m_WfTblName
			end
			if OpenTblName == QuestType then
				table.insert(QuestTbl,QuestName)
			end
		end
	end
	
	if next(QuestTbl) then
		QuestTbl = self:ResetQuestTblOrder(QuestTbl)
		self:InitQuestNameTabList(QuestTbl)
		self:InitQuestInfo(QuestName)
		if Quest_Common(QuestName, "所属区域") then
			local AreaName = Quest_Common(QuestName, "所属区域", "1")
			if AreaName and AreaName ~= "" then
				self:InitAreaInfo(AreaName)
			end
		end
	end
	
	if not self:IsShow() then
		self:ShowWnd(true)
		self:SetFocus()
	end
end

function CHandBookWnd:ShareQuest(QuestName)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	local TeamID = g_MainPlayer.m_Properties:GetTeamID()
	if TeamID == 0 then
		MsgClient(3007)
		return
	end
	
	if Quest_Common(QuestName, "能否共享") ~= 1 then
		MsgClient(3004)
		return
	end
	
	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
		Gac2Gas:ShareQuest(g_Conn, QuestName)
	else
		MsgClient(3006)
	end
end

function CHandBookWnd:GiveUpQuest(QuestName)
	if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
		GacGiveDoingQuest(QuestName)
	end
end

--任务完成窗口事件响应
function CHandBookWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
		elseif Child == self.m_QuestTrackBtn then
			if self.m_QuestName ~= nil
				and g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[self.m_QuestName] then
				g_GameMain.m_QuestTraceBack:ClickTrackBtn(self.m_QuestName)
			end
		elseif Child == self.m_QuestShareBtn then
			if self.m_QuestName ~= nil then
				self:ShareQuest(self.m_QuestName)
			end
		elseif Child == self.m_QuestGiveUpBtn then
			if self.m_QuestName ~= nil then
				self:GiveUpQuest(self.m_QuestName)
			end
		end
	end
end

function CHandBookTabItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_CLICKDOWN then
		if Child == self.m_NameCheckBtn then
			local FatherWnd = g_GameMain.m_HandBookWnd
			local sQuestName = self.m_NameCheckBtn.m_NameText.m_sName
			if self.m_TabType == 1 then 
				FatherWnd:InitHandBookWndByAllQuest(sQuestName)
			else
				FatherWnd:InitQuestInfo(sQuestName)
				if Quest_Common(sQuestName, "所属区域") then
					local AreaName = Quest_Common(sQuestName, "所属区域", "1")
					if AreaName and AreaName ~= "" then
						FatherWnd:InitAreaInfo(AreaName)
					end
				end
			end
		end
	end
end
