gac_gas_require "skill/SkillCommon"
lan_load "skill/Lan_ClassSeriesDesc_Common"
cfg_load "skill/ClassSeriesDesc_Common"

CSelectSeriesWnd  		= class(SQRDialog)
CSelectSeriesItemWnd	= class(SQRDialog)
CTalentItemWnd 			= class(SQRDialog)

local function AddImage(wnd, strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 0 ) 	
	wnd:SetWndBkImage( Flag, DefaultImage )
end

local function SetWndStateFun(ItemWnd,Flag)
	if Flag then
		ItemWnd.m_IsCanLearn = true
	else
		ItemWnd.m_IsCanLearn = nil
	end
	ItemWnd.m_DituStatic:EnableWnd(Flag)
	if Flag then 
		for i = 1,#(ItemWnd.m_TalentItemBtnTbl) do 
			if ItemWnd.m_TalentItemBtnTbl[i].m_IsLearned then
				ItemWnd.m_TalentItemBtnTbl[i]:DelFlashAttention() 
			else
				ItemWnd.m_TalentItemBtnTbl[i]:AddFlashInfoByName("TalentItemFlash")
			end
		end
	else
		for i = 1,#(ItemWnd.m_TalentItemBtnTbl) do 
			ItemWnd.m_TalentItemBtnTbl[i]:DelFlashAttention()
		end		
	end
end

--更新技能tooltips
local function UpdateAllSkillTips()
	if g_GameMain.m_MainSkillsToolBar then
		g_GameMain.m_MainSkillsToolBar:UpdateToolBarTips()
		g_GameMain.m_MasterSkillArea:UpdateToolBarTips()
		g_GameMain.m_AddMasterSkillArea:UpdateToolBarTips()
	end
	if g_GameMain and g_GameMain.m_CreateTalentSkillPartWnd then
		g_GameMain.m_CreateTalentSkillPartWnd:UpdateTalentSkillTips()
	end
	if g_GameMain and g_GameMain.m_CommonSkillWnd then
		g_GameMain.m_CommonSkillWnd:UpdateCommonSkillTips()
	end
end	

----------------------------------选择系别面板-----------------------------------------------
function CSelectSeriesWnd:Ctor()
	self:CreateFromRes("SelectSeriesParentWnd",g_GameMain)
	self.m_TalentStrRichText = self:GetDlgChild("TalentStrRichText")
	self.m_TalentNum = self:GetDlgChild("TalentNum")
	self.m_ReturnBtn = self:GetDlgChild("ReturnBtn")
	self.m_PosStatic = self:GetDlgChild("PosStatic")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_PosStatic:ShowWnd(false)
	self.m_ConsumeTalentNumber  = 0
	self.m_TalentSkillNameTbl = {}
	self.m_SeriesItemWndTbl = {}
	for i = 1, 3 do 
		local SeriesItemWnd = CSelectSeriesItemWnd:new()
		SeriesItemWnd:CreateFromRes("SeriesWndItem",self)
		SeriesItemWnd.m_IconStatic = SeriesItemWnd:GetDlgChild("IconStatic")
		SeriesItemWnd.m_DescRichText  = SeriesItemWnd:GetDlgChild("DescRichText")
		SeriesItemWnd.m_IconBtnBK = SeriesItemWnd:GetDlgChild("IconBtnBK") 
		SeriesItemWnd.m_IconStatic:InserBefore(SeriesItemWnd.m_IconBtnBK)
		SeriesItemWnd.m_NameRichText  = SeriesItemWnd:GetDlgChild("NameStatic")
		SeriesItemWnd.m_NameRichText:InserBefore(SeriesItemWnd.m_IconStatic)
		SeriesItemWnd.m_SelectBtn = SeriesItemWnd:GetDlgChild("SelectBtn")
		SeriesItemWnd.m_PosStatic =  SeriesItemWnd:GetDlgChild("PosStatic")
		SeriesItemWnd.m_PosStatic:ShowWnd(false)
		SeriesItemWnd.m_TalentItemTbl = {}
		for j = 1, 3 do 
			local TalentItem =  CTalentItemWnd:new()
			TalentItem:CreateFromRes("TalentItemOneWnd",SeriesItemWnd)
			TalentItem.m_DituStatic = TalentItem:GetDlgChild("DituStatic")
			TalentItem.m_DituStatic:EnableWnd(false)
			TalentItem.m_IsCanLearn = false
			TalentItem.m_TalentItemBtnTbl = {}
			TalentItem.m_TalentItemDituBtnTbl = {}
			for m = 1, 4 do 
				local IconBtn = TalentItem:GetDlgChild("IconBtn" .. m)
				table.insert(TalentItem.m_TalentItemBtnTbl,IconBtn)
				local DituBtn = TalentItem:GetDlgChild("DituBtn" .. m)
				DituBtn:InserBefore(TalentItem.m_DituStatic)
				DituBtn:EnableWnd(false)
				table.insert(TalentItem.m_TalentItemDituBtnTbl,DituBtn)
				
			end
			table.insert(SeriesItemWnd.m_TalentItemTbl,TalentItem)
		end
		
		local TalentItem = CTalentItemWnd:new()
		TalentItem:CreateFromRes("TalentItemTowWnd",SeriesItemWnd)
		TalentItem.m_DituStatic = TalentItem:GetDlgChild("DituStatic")
		TalentItem.m_DituStatic:EnableWnd(false)
		TalentItem.m_TalentItemBtnTbl = {}
		TalentItem.m_TalentItemDituBtnTbl = {}
		for j = 1 ,3 do 
			local IconBtn = TalentItem:GetDlgChild("IconBtn" .. j)
			table.insert(TalentItem.m_TalentItemBtnTbl,IconBtn)
			local DituBtn = TalentItem:GetDlgChild("DituBtn" .. j)
			DituBtn:InserBefore(TalentItem.m_DituStatic)
			DituBtn:EnableWnd(false)
			table.insert(TalentItem.m_TalentItemDituBtnTbl,DituBtn)			
		end
		table.insert(SeriesItemWnd.m_TalentItemTbl,TalentItem)
		table.insert(self.m_SeriesItemWndTbl,SeriesItemWnd)
	end
	g_ExcludeWndMgr:InitExcludeWnd(self, 2)
end

--设置天赋栏位置
function CSelectSeriesItemWnd:SetTalentItemPosition()	
	local Width = self.m_TalentItemTbl[1]:GetWndOrgWidth()
	local Height = self.m_TalentItemTbl[1]:GetWndOrgHeight()

	local TalentItemRt = CFRect:new()
	self.m_PosStatic:GetLogicRect(TalentItemRt)
	TalentItemRt.right = TalentItemRt.left + Width
	TalentItemRt.bottom = TalentItemRt.top + Height
	self.m_TalentItemTbl[1]:SetLogicRect(TalentItemRt)
	
	TalentItemRt.top = TalentItemRt.bottom + 5
	TalentItemRt.bottom = TalentItemRt.top + Height
	self.m_TalentItemTbl[2]:SetLogicRect(TalentItemRt)

	TalentItemRt.top = TalentItemRt.bottom + 5
	TalentItemRt.bottom = TalentItemRt.top + Height
	self.m_TalentItemTbl[3]:SetLogicRect(TalentItemRt)	
	
	TalentItemRt.top = TalentItemRt.bottom + 5
	TalentItemRt.bottom = TalentItemRt.top + self.m_TalentItemTbl[4]:GetWndOrgHeight()
	self.m_TalentItemTbl[4]:SetLogicRect(TalentItemRt)	
end

--设置天赋系别框位置
function CSelectSeriesWnd:SetSeriesItemPosition()
	local Width = self.m_SeriesItemWndTbl[1]:GetWndOrgWidth()
	local Height = self.m_SeriesItemWndTbl[1]:GetWndOrgHeight()
	
	local SeriesItemRt = CFRect:new() 
	self.m_PosStatic:GetLogicRect(SeriesItemRt)
	SeriesItemRt.right =  SeriesItemRt.left + Width
	SeriesItemRt.bottom = SeriesItemRt.top + Height
	
	self.m_SeriesItemWndTbl[1]:SetLogicRect(SeriesItemRt)
	self.m_SeriesItemWndTbl[1]:SetTalentItemPosition()
	
	SeriesItemRt.left = SeriesItemRt.right + 2
	SeriesItemRt.right = SeriesItemRt.left + Width
	self.m_SeriesItemWndTbl[2]:SetLogicRect(SeriesItemRt)
	self.m_SeriesItemWndTbl[2]:SetTalentItemPosition()
	
	SeriesItemRt.left = SeriesItemRt.right + 2
	SeriesItemRt.right = SeriesItemRt.left + Width
	self.m_SeriesItemWndTbl[3]:SetLogicRect(SeriesItemRt)
	self.m_SeriesItemWndTbl[3]:SetTalentItemPosition()
end

local function LoadBtnInfo(TalentBtn)
	local Icon,TooltipsText
	if TalentBtn.m_IsSkill then
		Icon = SkillPart_Common(TalentBtn.m_SkillName,"SkillSmallIcon")
		local LevelTbl  = g_PlayerLevelToSkillLevelTbl[TalentBtn.m_SkillName]
		if #(LevelTbl) > 1 then
			TooltipsText = g_Tooltips:GetWndSkillTips(TalentBtn.m_SkillName,LevelTbl[1][3])
			TalentBtn.m_SkillLevel = LevelTbl[1][3]
		else
			TooltipsText = g_Tooltips:GetWndSkillTips(TalentBtn.m_SkillName,1)
		end	
	else
		Icon = GiftSkill_Common(TalentBtn.m_SkillName,"SkillSmallIcon")
		TooltipsText = g_Tooltips:GetWndGiftSkillTips(TalentBtn.m_SkillName,10)
	end
	TalentBtn:SetMouseOverDescAfter(TooltipsText)
	TalentBtn.m_SkillSmallIcon = Icon 
	g_LoadGrayIconFromRes(Icon, TalentBtn, -2, IP_ENABLE, IP_CLICKDOWN)	
end

function CSelectSeriesWnd:InitSeriesItem(SeriesItemID,firstSeriesTbl)
	local SeriesItemWnd = self.m_SeriesItemWndTbl[SeriesItemID]
	for j = 1 ,4 do
		local TalentItem = SeriesItemWnd.m_TalentItemTbl[j]
		SetWndStateFun(TalentItem,false)
		for m = 1,#(TalentItem.m_TalentItemBtnTbl) do 
			local TalentBtn =  TalentItem.m_TalentItemBtnTbl[m]
			local DituBtn   =  TalentItem.m_TalentItemDituBtnTbl[m]
			DituBtn:EnableWnd(false)
			local SkillInfo = firstSeriesTbl[(j-1)*4 +m]
			TalentBtn.m_SkillName = SkillInfo.SkillName
			TalentBtn.m_IsSkill = (tonumber(SkillInfo.IsSkill) == 1 and true or false)
			if TalentBtn.m_IsSkill  then
				table.insert(self.m_TalentSkillNameTbl,SkillInfo.SkillName)
			end
			TalentBtn.m_IsLearned = false
			TalentBtn.m_SkillLevel = 0
			LoadBtnInfo(TalentBtn)
		end
	end
end

--得到该职业三天赋系表
local function GetTalentNameTbl(TalentTbl)
	local firstSeriesTbl = {}
	local secondSeriesTbl = {}
	local thirdSeriesTbl = {}
	local Keys = TalentTbl:GetKeys()
	for i,v in pairs(Keys) do
		local RowInfo = TalentTbl(v)
		if tonumber(RowInfo("GiftSkillSeries")) == ESkillSeriesKind.eSSK_First then	
			firstSeriesTbl[tonumber(RowInfo("ViewPosition"))] = {SkillName = v,DisplayName = RowInfo("DisplayName"),IsSkill = tonumber(RowInfo("IsSkill"))}
		elseif tonumber(RowInfo("GiftSkillSeries")) == ESkillSeriesKind.eSPS_Second then
			secondSeriesTbl[tonumber(RowInfo("ViewPosition"))] = {SkillName = v,DisplayName = RowInfo("DisplayName"),IsSkill = tonumber(RowInfo("IsSkill"))}
		elseif tonumber(RowInfo("GiftSkillSeries"))  == ESkillSeriesKind.eSPS_Third then
			thirdSeriesTbl[tonumber(RowInfo("ViewPosition"))] = {SkillName = v,DisplayName = RowInfo("DisplayName"),IsSkill = tonumber(RowInfo("IsSkill"))}
		end
	end
	assert(#(firstSeriesTbl) == 15,"天赋表填错了,天赋数不为15" )
	assert(#(secondSeriesTbl) == 15,"天赋表填错了,天赋数不为15" )
	assert(#(thirdSeriesTbl) == 15,"天赋表填错了,天赋数不为15" )	
	return firstSeriesTbl,secondSeriesTbl,thirdSeriesTbl
end

--初始化转职选择面板
function CSelectSeriesWnd:InitSeriesAllItem()
	local classId = g_MainPlayer:CppGetClass()
	GiftSkill_Common = loadGeniusfile(classId)
	Lan_GiftSkill_Common = loadGeniusfile(classId,true)
	local firstSeriesTbl,secondSeriesTbl, thirdSeriesTbl = GetTalentNameTbl(GiftSkill_Common)
	--设置数据
	self:InitSeriesItem(1,firstSeriesTbl)	
	self:InitSeriesItem(2,secondSeriesTbl)	
	self:InitSeriesItem(3,thirdSeriesTbl)
	--设置位置
	self:SetSeriesItemPosition()
end

function CSelectSeriesWnd:SetSeriesIcon()
	local ClassID = g_MainPlayer:CppGetClass()
	local SeriesIconTbl = ClassSeriesDesc_Common(ClassID)
	local SeriesKind = g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind
	local Flag = false
	if SeriesKind == nil or SeriesKind == 0 then
		Flag = false
	else
		Flag = true
	end
	
	for i = 1, 3 do
		if Flag then 
			if i == SeriesKind then
				self.m_SeriesItemWndTbl[i].m_IconBtnBK:EnableWnd(true)
				self.m_SeriesItemWndTbl[i].m_NameRichText:ShowWnd(true)
				AddImage(self.m_SeriesItemWndTbl[i].m_IconStatic ,g_ImageMgr:GetImagePath(SeriesIconTbl("SeriesIcon" .. i)))	
			else
				self.m_SeriesItemWndTbl[i].m_IconBtnBK:EnableWnd(false)
				self.m_SeriesItemWndTbl[i].m_NameRichText:ShowWnd(false)
				AddImage(self.m_SeriesItemWndTbl[i].m_IconStatic ,g_ImageMgr:GetImagePath(SeriesIconTbl("SeriesGrayIcon" .. i)))	
			end
		else
			self.m_SeriesItemWndTbl[i].m_IconBtnBK:EnableWnd(true)
			self.m_SeriesItemWndTbl[i].m_NameRichText:ShowWnd(true)	
			AddImage(self.m_SeriesItemWndTbl[i].m_IconStatic ,g_ImageMgr:GetImagePath(SeriesIconTbl("SeriesIcon" .. i)))		
		end		
	end
end

--切换GUI显示
function CSelectSeriesWnd:ShowFun()
	local Flag = false
	local SeriesKind = g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind
	if SeriesKind == nil or SeriesKind == 0 then
		Flag = false
	else
		Flag = true
	end
	for i = 1, 3 do 
		for j = 1 ,4 do
			self.m_SeriesItemWndTbl[i].m_TalentItemTbl[j]:ShowWnd(Flag)
		end
		self.m_SeriesItemWndTbl[i].m_DescRichText:ShowWnd(not Flag)
		self.m_SeriesItemWndTbl[i].m_SelectBtn:ShowWnd(not Flag)
		self.m_SeriesItemWndTbl[i]:ShowWnd(true)
	end
	self.m_TalentNum:ShowWnd(Flag)
	self.m_TalentStrRichText:ShowWnd(Flag)
	self.m_ReturnBtn:ShowWnd(Flag)
end


--初始化面板
function CSelectSeriesWnd:InitWnd()
	local ClassID = g_MainPlayer:CppGetClass()
	local SeriesDescTbl = Lan_ClassSeriesDesc_Common(ClassID)
	local SeriesIconTbl = ClassSeriesDesc_Common(ClassID)
	AddImage(self.m_SeriesItemWndTbl[1].m_IconStatic,g_ImageMgr:GetImagePath(SeriesIconTbl("SeriesIcon1")))
	AddImage(self.m_SeriesItemWndTbl[2].m_IconStatic,g_ImageMgr:GetImagePath(SeriesIconTbl("SeriesIcon2")))
	AddImage(self.m_SeriesItemWndTbl[3].m_IconStatic,g_ImageMgr:GetImagePath(SeriesIconTbl("SeriesIcon3")))
	self.m_SeriesItemWndTbl[1].m_NameRichText:SetWndText(SeriesDescTbl("SeriesName1"))
	self.m_SeriesItemWndTbl[2].m_NameRichText:SetWndText(SeriesDescTbl("SeriesName2"))
	self.m_SeriesItemWndTbl[3].m_NameRichText:SetWndText(SeriesDescTbl("SeriesName3"))
	self.m_SeriesItemWndTbl[1].m_DescRichText:SetWndText(SeriesDescTbl("SeriesDescText1"))
	self.m_SeriesItemWndTbl[2].m_DescRichText:SetWndText(SeriesDescTbl("SeriesDescText2"))
	self.m_SeriesItemWndTbl[3].m_DescRichText:SetWndText(SeriesDescTbl("SeriesDescText3"))
	self.m_SeriesItemWndTbl[1].m_SeriesID  = ESkillSeriesKind.eSSK_First
	self.m_SeriesItemWndTbl[2].m_SeriesID  = ESkillSeriesKind.eSPS_Second
	self.m_SeriesItemWndTbl[3].m_SeriesID  = ESkillSeriesKind.eSPS_Third
	local SkillSeriesKind  = g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind
	local SeriesKind = SkillSeriesKind and SkillSeriesKind or nil
	if SeriesKind and SeriesKind > 0 then
		if not self.m_SeriesInit  then
			self:InitSeriesAllItem()
			self.m_SeriesInit = true
			self:InitMainSeries(SeriesKind)
			self:SetTalentNumber()
		end
	else
		self:SetSeriesItemPosition()
		self.m_SeriesInit = nil
	end
end

--设置主修系
function CSelectSeriesWnd:InitMainSeries(SeriesKind)
	local LeftSeriesIte,RightSeriesItem = 0,0
	if SeriesKind == 1 then
		LeftSeriesIte = 2
		RightSeriesItem = 3
	elseif SeriesKind == 2 then
		LeftSeriesIte = 1
		RightSeriesItem = 3
	elseif SeriesKind == 3 then
		LeftSeriesIte = 1
		RightSeriesItem = 2		
	end
	self.m_MainSeriesItem = self.m_SeriesItemWndTbl[SeriesKind]
	self.m_LeftSeriesItem = self.m_SeriesItemWndTbl[LeftSeriesIte]
	self.m_RightSeriesItem = self.m_SeriesItemWndTbl[RightSeriesItem]
end

--初始化转职面板
function CSelectSeriesWnd:InitTreeNode(NodeNumber)
	if NodeNumber == ESkillTreeNode.eSTN_MainFirst then
		SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[1],true)
		self.m_MainSeriesItem.m_TalentItemTbl[1].m_ItemPhase =	ESkillLearnPhase.eSLP_First
		self.m_MainSeriesItem.m_TalentItemTbl[1].m_SkillTreeNode = ESkillTreeNode.eSTN_MainFirst
	elseif NodeNumber == ESkillTreeNode.eSTN_MainSecond then
		SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[2],true)
		self.m_MainSeriesItem.m_TalentItemTbl[2].m_ItemPhase =  ESkillLearnPhase.eSLP_Second
		self.m_MainSeriesItem.m_TalentItemTbl[2].m_SkillTreeNode = ESkillTreeNode.eSTN_MainSecond
	elseif NodeNumber == ESkillTreeNode.eSTN_MainThird then
		SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[3],true)
		self.m_MainSeriesItem.m_TalentItemTbl[3].m_ItemPhase = ESkillLearnPhase.eSLP_Third
		self.m_MainSeriesItem.m_TalentItemTbl[3].m_SkillTreeNode = ESkillTreeNode.eSTN_MainThird
	elseif NodeNumber == ESkillTreeNode.eSTN_MainFourth then
		SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[4],true)
		self.m_MainSeriesItem.m_TalentItemTbl[4].m_ItemPhase = ESkillLearnPhase.eSLP_Fourth
		self.m_MainSeriesItem.m_TalentItemTbl[4].m_SkillTreeNode = ESkillTreeNode.eSTN_MainFourth
	elseif NodeNumber == ESkillTreeNode.eSTN_LeftFirst then
		SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],true)
		self.m_LeftSeriesItem.m_TalentItemTbl[1].m_ItemPhase =  ESkillLearnPhase.eSLP_First 
		self.m_LeftSeriesItem.m_TalentItemTbl[1].m_SkillTreeNode =  ESkillTreeNode.eSTN_LeftFirst 
	elseif NodeNumber == ESkillTreeNode.eSTN_LeftSecond then
		SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[2],true)
		self.m_LeftSeriesItem.m_TalentItemTbl[2].m_ItemPhase =  ESkillLearnPhase.eSLP_Second 
		self.m_LeftSeriesItem.m_TalentItemTbl[2].m_SkillTreeNode =  ESkillTreeNode.eSTN_LeftSecond
	elseif 	NodeNumber == ESkillTreeNode.eSTN_RightFirst then
		SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],true)
		self.m_RightSeriesItem.m_TalentItemTbl[1].m_ItemPhase =  ESkillLearnPhase.eSLP_First 
		self.m_RightSeriesItem.m_TalentItemTbl[1].m_SkillTreeNode =  ESkillTreeNode.eSTN_RightFirst
	elseif 	NodeNumber == ESkillTreeNode.eSTN_RightSecond then	
		SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[2],true)		
		self.m_RightSeriesItem.m_TalentItemTbl[2].m_ItemPhase =  ESkillLearnPhase.eSLP_Second
		self.m_RightSeriesItem.m_TalentItemTbl[2].m_SkillTreeNode =  ESkillTreeNode.eSTN_RightSecond
	end
	self:UpdateOtherSeriesWnd()
end

function CSelectSeriesWnd:GetSeriesTalentNumber(Tbl)
	local Number = 0
	for i = 1,#(Tbl.m_TalentItemTbl) do
		for j = 1,#(Tbl.m_TalentItemTbl[i].m_TalentItemBtnTbl) do
			if Tbl.m_TalentItemTbl[i].m_TalentItemBtnTbl[j].m_IsLearned then
				Number = Number + 1
			end
		end
	end
	return Number
end

function CSelectSeriesWnd:GetSeriesTalentItemNumber(Tbl)
	local Number = 0
	for i = 1,#(Tbl) do 
		if Tbl[i].m_IsLearned then
			Number = Number + 1
		end
	end
	return Number
end

--设置副修系
function CSelectSeriesWnd:UpdateOtherSeriesWnd()
	if self.m_MainSeriesItem then 
		local MainSeriesNumber = self:GetSeriesTalentNumber(self.m_MainSeriesItem)
		local PlayerLevel = g_MainPlayer:CppGetLevel()
		local FourthPhaseState = false
		if PlayerLevel >= 45 and MainSeriesNumber >= 7 then
			FourthPhaseState = true
		end
		local LeftFirstNumber =	self:GetSeriesTalentItemNumber(self.m_LeftSeriesItem.m_TalentItemTbl[1].m_TalentItemBtnTbl)
		local RightFirstNumber = self:GetSeriesTalentItemNumber(self.m_RightSeriesItem.m_TalentItemTbl[1].m_TalentItemBtnTbl)
		local LeftSecondNumber = self:GetSeriesTalentItemNumber(self.m_LeftSeriesItem.m_TalentItemTbl[2].m_TalentItemBtnTbl)
		local RightSecondNumber = self:GetSeriesTalentItemNumber(self.m_RightSeriesItem.m_TalentItemTbl[2].m_TalentItemBtnTbl)		
		if PlayerLevel >= 30 and MainSeriesNumber >= 4 then
			if LeftFirstNumber > 0 and RightFirstNumber == 0 then
				SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],true)
				SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],false)
				if FourthPhaseState and LeftFirstNumber >= 2  then
					SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[2],true)
					self.m_LeftSeriesItem.m_TalentItemTbl[2].m_ItemPhase = ESkillLearnPhase.eSLP_Second
					SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],true)
					self.m_RightSeriesItem.m_TalentItemTbl[1].m_ItemPhase = ESkillLearnPhase.eSLP_First
					if LeftSecondNumber > 0 then
						SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[2],true)
						SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],false)
					elseif RightFirstNumber > 0 then
						SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],true)
						SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[2],false)
					end
				end	 
			elseif RightFirstNumber > 0 and LeftFirstNumber == 0 then
				SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],true)
				SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],false)
				if FourthPhaseState and RightFirstNumber >= 2 then
					SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],true)
					self.m_LeftSeriesItem.m_TalentItemTbl[1].m_ItemPhase = ESkillLearnPhase.eSLP_First
					SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[2],true)	
					self.m_RightSeriesItem.m_TalentItemTbl[2].m_ItemPhase = ESkillLearnPhase.eSLP_Second
					if LeftFirstNumber > 0 then
						SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],true)
						SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[2],false)
					elseif RightSecondNumber > 0 then
						SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[2],true)
						SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],false)
					end		
				end
			elseif RightFirstNumber > 0 and LeftFirstNumber > 0 then 
				SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],true)
				SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],true)
				SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[2],false)
				SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[2],false)								
			else
				SetWndStateFun(self.m_LeftSeriesItem.m_TalentItemTbl[1],true)
				self.m_LeftSeriesItem.m_TalentItemTbl[1].m_ItemPhase = ESkillLearnPhase.eSLP_First
				SetWndStateFun(self.m_RightSeriesItem.m_TalentItemTbl[1],true)
				self.m_RightSeriesItem.m_TalentItemTbl[1].m_ItemPhase = ESkillLearnPhase.eSLP_First
			end
		end
		self.m_RightSeriesItem.m_TalentItemTbl[1].m_SkillTreeNode = ESkillTreeNode.eSTN_RightFirst
		self.m_RightSeriesItem.m_TalentItemTbl[2].m_SkillTreeNode = ESkillTreeNode.eSTN_RightSecond
		self.m_LeftSeriesItem.m_TalentItemTbl[1].m_SkillTreeNode = ESkillTreeNode.eSTN_LeftFirst
		self.m_LeftSeriesItem.m_TalentItemTbl[2].m_SkillTreeNode = ESkillTreeNode.eSTN_LeftSecond
	end 
end

function CSelectSeriesWnd:UpdateMainSeriesWnd()
	if self.m_MainSeriesItem then 
		local ConsumeTalentNumber = self:GetSeriesTalentNumber(self.m_MainSeriesItem)
		local PlayerLevel = g_MainPlayer:CppGetLevel()
		if PlayerLevel >= 10 then
			SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[1],true)
			self.m_MainSeriesItem.m_TalentItemTbl[1].m_ItemPhase =	ESkillLearnPhase.eSLP_First
			self.m_MainSeriesItem.m_TalentItemTbl[1].m_SkillTreeNode = ESkillTreeNode.eSTN_MainFirst
		end
		
		if PlayerLevel >= 20 and ConsumeTalentNumber >= 2 then
			SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[2],true)
			self.m_MainSeriesItem.m_TalentItemTbl[2].m_ItemPhase =  ESkillLearnPhase.eSLP_Second
			self.m_MainSeriesItem.m_TalentItemTbl[2].m_SkillTreeNode = ESkillTreeNode.eSTN_MainSecond
		end
		
		if PlayerLevel >= 30 and ConsumeTalentNumber >= 4 then
			SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[3],true)
			self.m_MainSeriesItem.m_TalentItemTbl[3].m_ItemPhase = ESkillLearnPhase.eSLP_Third
			self.m_MainSeriesItem.m_TalentItemTbl[3].m_SkillTreeNode = ESkillTreeNode.eSTN_MainThird
		end
		
		if PlayerLevel >= 45 and ConsumeTalentNumber >= 7 then
			SetWndStateFun(self.m_MainSeriesItem.m_TalentItemTbl[4],true)
			self.m_MainSeriesItem.m_TalentItemTbl[4].m_ItemPhase = ESkillLearnPhase.eSLP_Fourth
			self.m_MainSeriesItem.m_TalentItemTbl[4].m_SkillTreeNode = ESkillTreeNode.eSTN_MainFourth
		end
	end
	self:UpdateOtherSeriesWnd()
end

function CSelectSeriesWnd:SelectSkillBtn(SkillName)
	local Tbl = {self.m_MainSeriesItem,self.m_LeftSeriesItem,self.m_RightSeriesItem}
	for m = 1,#(Tbl) do
		for i = 1,#(Tbl[m].m_TalentItemTbl) do
			for j = 1,#(Tbl[m].m_TalentItemTbl[i].m_TalentItemBtnTbl) do
				if Tbl[m].m_TalentItemTbl[i].m_TalentItemBtnTbl[j].m_SkillName == SkillName then
					return Tbl[m].m_TalentItemTbl[i].m_TalentItemBtnTbl[j],Tbl[m].m_TalentItemTbl[i].m_TalentItemDituBtnTbl[j]
				end
			end
		end
	end	
end

function CSelectSeriesWnd:ActiveTalentSkill(SkillName,SkillLevel)
	local classId = g_MainPlayer:CppGetClass()
	local SkillBtn,DituBtn = self:SelectSkillBtn(SkillName)
	if not SkillBtn then
		assert(SkillBtn,SkillName .. "天赋在配置表GiftSkill_Common" .. classId .. "不存在")
	end
	SkillBtn.m_IsLearned = true
	SkillBtn.m_SkillLevel = SkillLevel
	SkillBtn:DelFlashAttention()
	DituBtn:EnableWnd(true)
	if SkillBtn.m_IsSkill then
		local TooltipsText = g_Tooltips:GetWndSkillTips(SkillName,SkillLevel)
		SkillBtn:SetMouseOverDescAfter(TooltipsText)
	end
	g_RePlaceWndImage(SkillBtn.m_SkillSmallIcon, SkillBtn, 0, IP_ENABLE, IP_CLICKDOWN)
	self.m_ConsumeTalentNumber = self.m_ConsumeTalentNumber + 1
	self:UpdateMainSeriesWnd()
	self:SetTalentNumber()
end

function CSelectSeriesWnd:UpdateTalentInfo(SkillName,SkillLevel)
	local SkillBtn,DituBtn = self:SelectSkillBtn(SkillName)
	if SkillBtn then
		SkillBtn.m_SkillLevel = SkillLevel
		local TooltipsText = g_Tooltips:GetWndSkillTips(SkillName,SkillLevel)
		SkillBtn:SetMouseOverDescAfter(TooltipsText)
	end
end

function CSelectSeriesWnd:SetTalentNumber()
	local Level = g_MainPlayer:CppGetLevel()
	local allTalentNumber = 0
	if Level >= 10 then
		allTalentNumber= math.floor((Level-10)/5)+1
	end
	local curTalentNumber = self.m_ConsumeTalentNumber
	local TalentNumber = allTalentNumber - curTalentNumber
	TalentNumber = TalentNumber > 0 and TalentNumber or 0
	self.m_TalentNum:SetWndText(TalentNumber)
	if g_GameMain.m_CreateTalentSkillPartWnd then
		g_GameMain.m_CreateTalentSkillPartWnd.m_TalentNum:SetWndText(TalentNumber)
		if TalentNumber > 0 then
			g_GameMain.m_CreateTalentSkillPartWnd.m_TalentNum:AddFlashInfoByName("TalentPointFlash")
			g_GameMain.m_CreateTalentSkillPartWnd.m_OpenTalentBtn:AddFlashInfoByName("TalentPointBtnFlash")
		else
			g_GameMain.m_CreateTalentSkillPartWnd.m_TalentNum:DelFlashAttention()
			g_GameMain.m_CreateTalentSkillPartWnd.m_OpenTalentBtn:DelFlashAttention()
		end
	end
	if TalentNumber <= 0 then
		for j = 1,#(self.m_SeriesItemWndTbl) do
			local SeriesItemWnd = self.m_SeriesItemWndTbl[j]
			for i = 1,#(SeriesItemWnd.m_TalentItemTbl) do 
				local TalentItem = SeriesItemWnd.m_TalentItemTbl[i]
				for m = 1,#(TalentItem.m_TalentItemBtnTbl) do 
					local ItemBtn = TalentItem.m_TalentItemBtnTbl[m]
					if not ItemBtn.m_IsLearned then
						ItemBtn:DelFlashAttention() 
					end
				end
			end
		end
	end
end


function CSelectSeriesWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_ReturnBtn then
			self:ShowWnd(false)
			UpdateSkillAllWndRect()
			self:SetTalentNumber()
			g_GameMain.m_CommonSkillWnd:ShowWnd(true)
		elseif Child == self.m_CloseBtn then
			self:ShowWnd(false)
		end
	end
end 

function CSelectSeriesItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_SelectBtn then
			Gac2Gas:SetSeries(g_Conn,self.m_SeriesID)
		end
	end
	
end

function CTalentItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then 
		if self.m_IsCanLearn and self.m_ItemPhase and self.m_SkillTreeNode then
			Gac2Gas:AddTalentSkill(g_Conn,Child.m_SkillName,self.m_SkillTreeNode)
		end
	end
end

--学习天赋技能
function CSelectSeriesWnd.AddTalentSkill(name, level )
	--激活技能树上的天赋
	g_GameMain.m_SelectSeriesWnd:ActiveTalentSkill(name,level)
	g_GameMain.m_CreateTalentSkillPartWnd:SetTalentSkill(name,level,true)
	--更新快捷栏上技能的tooltips
	UpdateAllSkillTips()
	g_GameMain.m_ShowSkillDescWnd:SetDescInfo(name)
	local SkillInfo = SkillPart_Common(name)
	if g_GameMain.m_LearnedSkillShowWnd and SkillInfo then
		g_GameMain.m_LearnedSkillShowWnd:SetSkillBtnPicture(SkillInfo("SkillSmallIcon"),name,level,0) --ActionType为0代表学习，1代表升级
	end	
	g_GameMain.m_LeadLeanSkillWnd:InitData()
end

---- 1,技能的名称 2 ,技能的等级 3,种类（天赋 ，技能）
function CSelectSeriesWnd.RetAllFightSkill(name, level, kind )
	if kind == FightSkillKind.Talent then
		g_GameMain.m_SelectSeriesWnd:ActiveTalentSkill(name,level)		--激活技能树上的天赋
		g_GameMain.m_CreateTalentSkillPartWnd:SetTalentSkill(name,level)
		UpdateAllSkillTips() 											--更新快捷栏上技能的tooltips
	elseif kind == FightSkillKind.Skill then 							--激活技能综合面板上的公共技能
		if not string.find(name,"普通攻击") then
			g_GameMain.m_CommonSkillWnd:ActiveSkill(name,level)
		end
	elseif kind == FightSkillKind.NonFightSkill then
		g_GameMain.m_NonFightSkillWnd:ActiveNonFightSkill(name)			--激活非战斗技能
	end
end

function CSelectSeriesWnd.RetAddFightSkillEnd()
	g_GameMain.m_CommonSkillWnd.m_InitWnd = true
	g_GameMain.m_NpcSkillLearnWnd.m_InitWnd = true
	g_GameMain.m_SelectSeriesWnd:SetTalentNumber()
	g_GameMain.m_SelectSeriesWnd:SetSeriesIcon()
	if g_GameMain.m_CommonSkillWnd.IsOpenWnd then
		OpenSkillAllWnd()
		g_GameMain.m_CommonSkillWnd.IsOpenWnd = nil
	elseif g_GameMain.m_NpcSkillLearnWnd.IsOpenWnd then
		g_GameMain.m_NpcSkillLearnWnd:OpenSelf()
		g_GameMain.m_NpcSkillLearnWnd.IsOpenWnd	= nil
	end
end

function CSelectSeriesWnd.ReturnSeries(SkillSeriesKind)
	if g_GameMain.m_NpcSkillLearnWnd then
		g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind = SkillSeriesKind
	end	
	if 	g_GameMain.m_SelectSeriesWnd ~= nil then
		g_GameMain.m_SelectSeriesWnd:InitWnd()
		g_GameMain.m_SelectSeriesWnd:ShowFun()
		g_GameMain.m_SelectSeriesWnd:SetSeriesIcon()
		
		if SkillSeriesKind and SkillSeriesKind > 0 then
			g_GameMain.m_NpcSkillLearnWnd:SetBtnState(true)	
			g_GameMain.m_SelectSeriesWnd:InitTreeNode(ESkillTreeNode.eSTN_MainFirst)
			g_GameMain.m_SelectSeriesWnd:SetTalentNumber()
			local AutoCloseWnd = CAutoCloseWnd:new()
			AutoCloseWnd:StopAutoCloseWnd(g_GameMain.m_SelectSeriesWnd)
		else
			g_GameMain.m_NpcSkillLearnWnd:SetBtnState(false)	
		end
	end
end


function CSelectSeriesWnd.ReturnSkillNode(SkillTreeNode)	
	if g_GameMain.m_SelectSeriesWnd then
		g_GameMain.m_SelectSeriesWnd:InitTreeNode(SkillTreeNode)
	end
end

