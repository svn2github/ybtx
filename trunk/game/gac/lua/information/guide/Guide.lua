gac_gas_require "areamgr/AreaMgr"
cfg_load "information/guide/GuideDirection_Client"
cfg_load "information/guide/GuideType_Client"
cfg_load "information/guide/GuideCondition_Client"
lan_load "information/guide/Lan_GuideDirection_Client"
lan_load "information/guide/Lan_GuideType_Client"

CGuide					= class(SQRDialog)
CGuideListItem			= class(SQRDialog)
CGuideListItemListItem	= class(SQRDialog)
CGuideOnTheCheck		= class(SQRDialog)
CGuideInstanceBtn		= class(SQRDialog)
CGuideOnTheCheckBtn		= class(SQRDialog)

local fl_nBigItemHeight		= 50 + 14 + 7
local fl_nSmallItemHeight	= 32
local fl_nRadioNum			= 9
local fl_nListOnePageNum	= 7

function CGuide:Ctor()
	self:CreateFromRes("Guide", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:SetMenuList()
end

function CGuide:OnChildCreated()
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_List		= self:GetDlgChild("List")
	self.m_UpBtn	= self:GetDlgChild("UpBtn")
	self.m_DownBtn	= self:GetDlgChild("DownBtn")
	self.m_tblRadio	= {}
	for i = 1, fl_nRadioNum do
		self.m_tblRadio[i] = self:GetDlgChild("Radio" .. i)
		self.m_tblRadio[i]:EnableWnd(false)
	end
	self.m_UpBtn:ShowWnd(false)--EnableWnd(false)
	self.m_DownBtn:ShowWnd(false)--EnableWnd(false)
end

function CGuide:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:OpenPanel(false)
		else
			for i, v in ipairs(self.m_tblRadio) do
				if(Child == v) then
					self:SetList(i)
				end
			end
		end
	end
end

function CGuide:OnDestroy()
	if(self.m_GuideOnTrackTick) then
		UnRegisterTick(self.m_GuideOnTrackTick)
		self.m_GuideOnTrackTick = nil
	end
end

function CGuide:VirtualExcludeWndOnClose()
	g_GameMain.m_GuideBtn:Animation(true)
end

function CGuide:RefrashRadio()
	for i, v in ipairs(self.m_tblRadio) do
		v:EnableWnd(true == g_GameMain.m_GuideData.m_tblDirectionClear[i])
	end
end

function CGuide:OpenPanel(bFlag)
	local arg = g_ParseCommandline()[3] --鲜血试炼服
	if arg and arg.gastype == 1 then
		return
	end
	
	local zoomSize	= self:GetRootZoomSize()
	local rect1		= CFRect:new()
	local rect2		= CFRect:new()
	self:GetWndRect(rect1)
	rect2.left		= rect1.left + 50*zoomSize
	rect2.right		= rect1.right + 50*zoomSize
	rect2.top		= rect1.top
	rect2.bottom	= rect1.bottom
	if(bFlag) then
		self.VirtualSpecialWndOnClose = VirtualSpecialWndOnClose
		self:ShowWnd(true)
		self:SetTransparent(0.1)
		self:SetTransparentObj(350, false)
		self:SetShapeEffect(250, rect2, rect1)
		self.m_bHiding = false
	else
		self:SetTransparent(1)
		self:SetTransparentObj(350, true)
		self:SetShapeEffect(250, rect1, rect2)
		self.m_bHiding = true
	end
	g_GameMain.m_GuideBtn:Clear()
end

function CGuide:OnShapeEffectEnded()
	if(self.m_bHiding) then
		self:ShowWnd(false)
		self.m_bHiding = false
	else
		Gac2Gas:GetActionCountEx(g_Conn)
	end
end

--设置列表listctrl控件
function CGuide:SetList(nDirection)
	self.m_List:DeleteAllItem()
	self.m_tblItemWnd = {}
	self.m_List:InsertColumn(0, self.m_List:GetWorkAreaOrgWidth())
	for i, v in ipairs( g_GameMain.m_GuideData.m_tblCondition[nDirection] ) do
		self.m_List:InsertItem(i-1, fl_nBigItemHeight)
		local item		= self.m_List:GetSubItem(i-1, 0)
		local itemWnd	= CGuideListItem:new(item, i, self)
		self.m_tblItemWnd[i] = itemWnd
		itemWnd:SetInfo(v)
	end
end

--关闭所有组的列表
function CGuide:CloseAll()
	for i = 1, #(self.m_tblItemWnd) do
		self.m_tblItemWnd[i]:ShowChildList(false)
	end
end

function CGuide:OpenForSaveHighLight(sSmallType)
	sSmallType = self.m_ForceAlertType or sSmallType
	local sBigType = GuideCondition_Client(sSmallType, "BigType")
	local tbl = g_GameMain.m_GuideData.m_tblStringIndex[sBigType]
	self.m_ForceAlertType = nil
	if(tbl) then
		self.m_tblRadio[tbl[1]]:SetCheck(true)
		self:SetList(tbl[1])
		self.m_tblItemWnd[tbl[2]]:ShowChildList(true)
		self.m_List:SetVerticalPos(tbl[2] - fl_nListOnePageNum)
		self.m_tblItemWnd[tbl[2]].m_OnTheCheck:AddFlashInfoByNameEx("DirectHighLight", true)
	end
end

function CGuide:GetMenuCheck()
	for i, v in ipairs(self.m_tblRadio) do
		if( v:GetCheck() ) then
			return i
		end
	end
end

--function CGuide:BeInTrack(SceneName, x, y)
--	local Info = CQuest.GetAvaliableMasterStrokeQuests(0)
--	for i, v in ipairs(Info) do
--		if(v[2] == SceneName and v[3] == x and v[4] == y) then
--			g_GameMain.m_CenterMsg:ShowWnd(false)
--			break
--		end
--	end
--end

function CGuide:BeginTrackTick()
	local function GuideOnTrackTick()
		if( not (g_SpanSceneAutoTrackMgr.m_AutoTrackToPointName and g_SpanSceneAutoTrackMgr.m_AutoTrackToPointName ~= "") ) then
			g_GameMain.m_GuideBtn:BeAlertQuestEmptyAlert()
			UnRegisterTick(self.m_GuideOnTrackTick)
			self.m_GuideOnTrackTick = nil
		end
	end
	if(self.m_GuideOnTrackTick) then
		UnRegisterTick(self.m_GuideOnTrackTick)
		self.m_GuideOnTrackTick = nil
	end
	self.m_GuideOnTrackTick = RegisterTick("GuideOnTrackTick", GuideOnTrackTick, 20000)
end

function CGuide:SetMenuList()
	for i, v in ipairs( GuideDirection_Client:GetKeys() ) do
		local sDisplayDirection = Lan_GuideDirection_Client(MemH64(v), "DisplayDirection")
		self.m_tblRadio[i]:SetWndText(sDisplayDirection)
	end
	self.m_tblRadio[1]:SetCheck(true)
end

--***************************************************************************************

--list第一级------------------------------------------------------------------------------------------------------
function CGuideListItem:Ctor(parent, index, fatherWnd)
	self.m_fatherWnd = fatherWnd
	self.m_nItemIndex = index --自己在list中的序列
	self:CreateFromRes("GuideListItem", parent)
	self:SetMouseWheel(true)
	self:ShowWnd( true )
end

function CGuideListItem:OnChildCreated()
	self.m_TitleCheck	= self:GetDlgChild("TitleCheck")
	self.m_ChildList	= self:GetDlgChild("ChildList")
	self.m_LeftSide		= self:GetDlgChild("LeftSide")
	self.m_RightSide	= self:GetDlgChild("RightSide")
	self.m_OnTheCheck	= CGuideOnTheCheck:new(self.m_TitleCheck)
	self.m_TitleCheck:SetMouseWheel(true)
	self.m_ChildList:SetMouseWheel(true)
	self.m_LeftSide:ShowWnd(false)
	self.m_RightSide:ShowWnd(false)
end

function CGuideListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_TitleCheck) then
			local bFlag = self.m_ChildList:IsShow()
			self.m_fatherWnd:CloseAll()
			self:ShowChildList(not bFlag)
		end
	end
end

function CGuideListItem:ShowChildList(bFlag)
	local height = bFlag and #(self.m_tblChildItemWnd)*fl_nSmallItemHeight or 0
	self.m_TitleCheck:SetCheck(bFlag)
	self.m_ChildList:ShowWnd(bFlag)
	self.m_fatherWnd.m_List:SetItemHeight(height+fl_nBigItemHeight, self.m_nItemIndex-1)
end

function CGuideListItem:SetWndHeight(wnd, height)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	Rect.bottom = Rect.top + height
	wnd:SetLogicRect(Rect)
end

function CGuideListItem:SetInfo(tblInfo)
	local bDone		= g_GameMain.m_GuideData.m_tblBeDone[tblInfo.sType][1]
	local bClear	= g_GameMain.m_GuideData.m_tblBeDone[tblInfo.sType][2]
	local bLast		= 1 == GuideCondition_Client(tblInfo.sType, "Last")
	
	if(bDone and g_GameMain.m_GuideWnd.m_sSmallTypeForHighLight == tblInfo.sType) then
		g_GameMain.m_GuideWnd.m_sSmallTypeForHighLight = nil
	end
	
	-------------------------------------------------------------------------------------------------
	local lanInfo = Lan_GuideType_Client( MemH64(tblInfo.sDirection .. tblInfo.sType) )
	local sDisplayType	= lanInfo("DisplayType")
	local sDisplayState	= lanInfo("DisplayState")
	local sDescription	= lanInfo("Description")
	local icon			= GuideType_Client(tblInfo.sDirection, tblInfo.sType, "Icon")
	local nface			= GuideType_Client(tblInfo.sDirection, tblInfo.sType, "Face")
	local sFace			= g_DynImageMgr:GetFaceImage(nface)
	local sDisFace		= g_DynImageMgr:GetDisableFaceImage(nface)
	
	if("剧情任务" == tblInfo.sType) then
		sDisplayState = GetStrAccordWildcard(sDisplayState, CountMasterStrokeQuest())
	elseif("跑环任务（已开放）" == tblInfo.sType) then
		local nCur, nTotal = 0, 0
		local tbl = CQuest.GetLoopQuestState()
		for i, v in ipairs(tbl) do
			if( string.find(v[1], "答题") ) then
				sTimes = v[2] .. "/" .. v[3]
				break
			end
		end
		sDisplayState = GetStrAccordWildcard(sDisplayState, sTimes)
	else
		local sActionCount = g_GameMain.m_GuideData.m_tblActionCount[tblInfo.sType]
		local _, num = string.gsub(sDisplayState, "%%s", "%%s")
		if(1 == num) then
			sDisplayState = GetStrAccordWildcard(sDisplayState, sActionCount or "0/0")
		end
	end
	
	self.m_sDirection	= tblInfo.sDirection
	self.m_sType		= tblInfo.sType
	self.m_OnTheCheck.m_Title:SetWndText(sDisplayType)
	self.m_OnTheCheck.m_State:SetWndText(sDisplayState)
	self.m_OnTheCheck.m_Description:SetWndText(sDescription)
	g_DynImageMgr:AddImageByIP(self.m_OnTheCheck.m_Icon, icon, IP_ENABLE)
	
	self.m_LeftSide:ShowWnd(bLast and bClear)
	self.m_RightSide:ShowWnd(bLast and bClear)
	
	if(sFace and "" ~= sFace and sDisFace and "" ~= sDisFace) then
		g_DynImageMgr:AddImageByIP(self.m_OnTheCheck.m_Face, sFace, IP_ENABLE)  
		g_DynImageMgr:AddImageByIP(self.m_OnTheCheck.m_Face, sDisFace , IP_DISABLE)
		self.m_OnTheCheck.m_Face:ShowWnd(true)
	end
	-------------------------------------------------------------------------------------------------
	
	self:EnableWnd(bClear)
	self:SetChildList(tblInfo)
	self:ShowChildList(false)
end

function CGuideListItem:SetChildList(tblInfo)
	self.m_ChildList:DeleteAllItem()
	self.m_tblChildItemWnd = {}
	self.m_ChildList:InsertColumn(0, self.m_ChildList:GetWorkAreaOrgWidth())
	
	for i, v in ipairs(tblInfo) do
		self.m_ChildList:InsertItem(i-1, fl_nSmallItemHeight)
		local item		= self.m_ChildList:GetSubItem(i-1, 0)
		local itemWnd	= CGuideListItemListItem:new(item, self, i)
		self.m_tblChildItemWnd[i] = itemWnd
		itemWnd:SetInfo(v)
	end
	
	local height = #(self.m_tblChildItemWnd)*fl_nSmallItemHeight
	self:SetWndHeight(self.m_ChildList, height)
	self:SetWndHeight(self, height+fl_nBigItemHeight)
end

--check上面的信息
function CGuideOnTheCheck:Ctor(parent)
	self.m_Parent = parent
	self:CreateFromRes("GuideOnTheCheck", parent)
	self:ShowWnd(true)
end

function CGuideOnTheCheck:OnChildCreated()
	self.m_Icon			= self:GetDlgChild("Icon")
	self.m_Title		= self:GetDlgChild("Title")
	self.m_State		= self:GetDlgChild("State")
	self.m_Face			= self:GetDlgChild("Face")
	self.m_Description	= self:GetDlgChild("Description")
	self.m_VideoBtn		= CGuideOnTheCheckBtn:new(self)
	self.m_Face:ShowWnd(false)
end

--list第二级---------------------------------------------------------------------------------------------------
function CGuideListItemListItem:Ctor(parent, belongToWnd, index)
	self.m_childItemIndex	= index
	self:CreateFromRes("GuideListItemlistItem", parent)
	self:SetMouseWheel(true)
	self:ShowWnd(true)
end

function CGuideListItemListItem:OnChildCreated()
	self.m_Content	= self:GetDlgChild("Content")
	self.m_Btn		= CGuideInstanceBtn:new(self)
	self.m_Content:SetMouseWheel(true)
	self.m_Btn:SetMouseWheel(true)
	self.m_Btn:ShowWnd(false)
end

function CGuideListItemListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID  == RICH_CLICK) then
		if(Child == self.m_Content) then
			local str		= Child:GetChooseStr()
			local value		= Child:GetChooseStrAttr()
			local tbl		= {}
			SplitString(value, ",", tbl)
			if("跳转" == tbl[1]) then
				g_GameMain.m_GuideWnd:HighLight(tbl[2])
			elseif("技能学习寻路" == tbl[1]) then
				local nCamp = g_GameMain.m_PlayerInfoTbl.m_PlayerBirthCamp
				local pointList = g_GameMain.m_LeadLeanSkillWnd:GetPositionList(nCamp)
				PlayerAutoTrackNearestPoint(pointList)
				g_GameMain.m_GuideWnd:BeginTrackTick()
				g_GameMain.m_GuideWnd:OpenPanel(false)
			elseif("寻路" == tbl[1]) then
				PlayerAutoTrack( tbl[2], tbl[3], tonumber(tbl[4]), tonumber(tbl[5]) )
				g_GameMain.m_GuideWnd:BeginTrackTick()
				g_GameMain.m_GuideWnd:OpenPanel(false)
			elseif("技能" == tbl[1]) then
				if( IsCppBound(g_MainPlayer) ) then
					g_MainPlayer:FightSkill(tbl[2], 1)
					g_GameMain.m_GuideWnd:OpenPanel(false)
				end
			end
		end
	elseif(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Btn) then
			self:BtnFunc()
		end
	end
end

function CGuideListItemListItem:BtnInstanceFunc()
	local nSceneType = Scene_Basic_Common_Shell(self.m_FuncParam, "SceneType")
	g_GameMain.m_GuideWnd:OpenPanel(false)
	if( g_MainPlayer:IsInBattleState() ) then
		MsgClient(410023)
		return
	end
	if(2 == nSceneType) then
		Gac2Gas:EnterFbSceneFromWnd(g_Conn, self.m_FuncParam)
	elseif(13 == nSceneType) then
		Gac2Gas:JoinMatchGame(g_Conn, self.m_FuncParam, true)
	elseif(14 == nSceneType) then
		Gac2Gas:JoinYbEducateAction(g_Conn, self.m_FuncParam)
	elseif(20 == nSceneType) then
		Gac2Gas:JoinDaTaoSha(g_Conn)
	end
end

function CGuideListItemListItem:BtnOpenPanelFuc()
	if("积分" == self.m_FuncParam) then
		g_GameMain.m_PlayerManageWnd:ShowWnd(true)
		g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[3]:SetCheck(true)
		g_GameMain.m_JiFenDetailWnd:ShowWnd(true)
		g_GameMain.m_JiFenDetailWnd:UpdateJiFenWnd()
		g_GameMain.m_RoleStatus:ShowWnd(false)
		g_GameMain.m_FightingEvaluationWnd:ShowWnd(false)
		g_GameMain.m_PlayerManageWnd:SetFocus()
		Gac2Gas:GetTongProfferCount(g_Conn)
	end
end

function CGuideListItemListItem:SetInfo(info)
	local color = g_ColorMgr:GetColor("攻略文字")
	if(IsTable(info)) then
		self.m_Btn:ShowWnd(true)
		if("开启面板" == info[1]) then
			self.m_Btn.m_Btn:SetWndText(info[3])
			self.m_FuncParam	= info[2]
			self.BtnFunc		= self.BtnOpenPanelFuc
		elseif("副本" == info[2][1]) then
			self.m_Btn.m_Btn:SetWndText(GetStaticTextClient(25004))
			self.m_FuncParam	= info[2][2]
			self.BtnFunc		= self.BtnInstanceFunc
			self.m_Content:SetWndText(color .. info[1])
		end
	else
		self.m_Content:SetWndText(color .. info)
	end
end

--条件横条上的按钮--------------------------------------------------------------------------------------
function CGuideInstanceBtn:Ctor(fatherWnd)
	self.m_FatherWnd = fatherWnd
	self:CreateFromRes("GuideInstanceBtn", fatherWnd.m_Content)
end

function CGuideInstanceBtn:OnChildCreated()
	self.m_Btn = self:GetDlgChild("Btn")
end

function CGuideInstanceBtn:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Btn) then
			self.m_FatherWnd:BtnFunc()
		end
	end
end

--项目上的按钮--------------------------------------------------------------------------------------
function CGuideOnTheCheckBtn:Ctor(parent)
	self:CreateFromRes("GuideOnTheCheckBtn", parent)
end

function CGuideOnTheCheckBtn:OnChildCreated()
	self.m_Btn = self:GetDlgChild("Btn")
end

function CGuideOnTheCheckBtn:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Btn) then
			
		end
	end
end

