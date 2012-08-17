gac_require "activity/direct/DirectWndInc"
gac_require "framework/texture_mgr/DynImageMgr"
gac_gas_require "activity/direct/LoadDirectCommon"
cfg_load "player/Direct_Common"
cfg_load "player/pos_Direct_Common"
lan_load "player/Lan_Direct_Common"
lan_load "player/Lan_DirectInfo_Common"


local fl_TblStaticText = {}
fl_TblStaticText["未激活"]		= GetStaticTextClient(25001)
fl_TblStaticText["点击后达成"]	= GetStaticTextClient(25002)
fl_TblStaticText["已领取"]		= GetStaticTextClient(25003)

function CreateDirectWnd(parent)
	local wnd = CDirectWnd:new()
	wnd:Init(parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 2)
	return wnd
end

function CDirectWnd:Init(parent)
	self.m_HighLightWnd = nil
	self:CreateFromRes("DirectWnd", parent)
end

function CDirectWnd:OnChildCreated()
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_Static	= self:GetDlgChild("Static")
	self.m_tblRadio	= {self:GetDlgChild("TabRadio1"), self:GetDlgChild("TabRadio2")}
	
	self.m_tblList	= {self:CreateList(self.m_Static, 1), self:CreateList(self.m_Static, 2)}
	self.m_tblRadio[1]:SetCheck(true)
	self.m_tblRadio[2]:SetCheck(false)
	self.m_tblList[1]:ShowWnd(true)
end

function CDirectWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		else
			for i, v in ipairs(self.m_tblRadio) do
				if(Child == v) then
					self:ChangePage(i)
				end
			end
		end
	end
end

function CDirectWnd:OpenPanel(bFlag, nBigType, sActionType, sDirectName)
	if(nBigType and sActionType and sDirectName) then
		self:HighLight(nBigType, sActionType, sDirectName)
	end
	self:ShowWnd(bFlag)
end

function CDirectWnd:VirtualExcludeWndClosed()
	g_GameMain.m_DirectWnd:DelFlash()
end

function CDirectWnd:ChangePage(nPage)
	for i, v in ipairs(self.m_tblList) do
		v:ShowWnd(i == nPage)
	end
end

function CDirectWnd:HighLight(nBigType, sActionType, sDirectName)
	local BigTypeWnd	= self.m_tblList[nBigType]
	local nActionIndex	= g_DirectMgr.m_tblActionTypeTrans[sActionType]
	local ActionWnd		= BigTypeWnd.m_tblListItemWnd[nActionIndex]
	local nDirectIndex	= ActionWnd.m_tblDirectNameTrans[sDirectName]
	local DirectWnd		= ActionWnd.m_tblListItemWnd[nDirectIndex]
	self.m_tblRadio[nBigType]:SetCheck(true)
	self:ChangePage(nBigType)
	BigTypeWnd.m_List:SetVerticalPos(nActionIndex - 2)
	ActionWnd.m_List:SetVerticalPos(nDirectIndex - 3)
	ActionWnd:EnableUpBottomBtn()
	
	self:DelFlash()
	DirectWnd:AddFlashInfoByNameEx("DirectHighLight", true)
	self.m_HighLightWnd = DirectWnd
end

function CDirectWnd:DelFlash()
	if(self.m_HighLightWnd) then
		self.m_HighLightWnd:DelFlashAttention()
	end
	self.m_HighLightWnd = nil
end

function CDirectWnd:ChangeDirectState(nBigType, sActionType, sDirectName, state)
	local BigTypeWnd	= self.m_tblList[nBigType]
	local nActionIndex	= g_DirectMgr.m_tblActionTypeTrans[sActionType]
	local ActionWnd		= BigTypeWnd.m_tblListItemWnd[nActionIndex]
	if(not ActionWnd) then
		LogErr("攻略信息错误", "nBigType:" .. nBigType .. ",sActionType:" .. sActionType .. ",sDirectName:" .. sDirectName .. ",state:" .. state)
	end
	local nDirectIndex	= ActionWnd.m_tblDirectNameTrans[sDirectName]
	local DirectWnd		= ActionWnd.m_tblListItemWnd[nDirectIndex]
	DirectWnd:ChangeState(state)
	ActionWnd:CanGetDirectAwardItem()
end

----------------------------------------------------------------------------
function CDirectWnd:CreateList(parent, nBigType)
	local wnd = CDirectList:new()
	wnd:Init(parent, nBigType)
	return wnd
end

function CDirectList:Init(parent, nBigType)
	self.m_nBigType	= nBigType
	self:CreateFromRes("DirectWndList", parent)
end

function CDirectList:OnChildCreated()
	self.m_List = self:GetDlgChild("List")
end

function CDirectList:DrawList()
	self.m_List:DeleteAllItem()
	self.m_tblListItemWnd = {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	local tbl = g_DirectMgr.m_DirectInfoTbl[self.m_nBigType]
	for i, v in ipairs(tbl) do
		self.m_List:InsertItem(i-1, 245)
		local itemWnd = self:CreateListItem(self.m_List:GetSubItem(i-1, 0), v.ActionType, self)
		table.insert(self.m_tblListItemWnd, itemWnd)
		itemWnd:SetInfo(v)
	end
end

----------------------------------------------------------------------------
function CDirectList:CreateListItem(parent, sActionType, fatherWnd)
	local wnd = CDirectListItem:new()
	wnd:Init(parent, sActionType, fatherWnd)
	wnd:ShowWnd(true)
	return wnd
end

function CDirectListItem:Init(parent, sActionType, fatherWnd)
	self.m_FatherWnd	= fatherWnd
	self.m_sActionType	= sActionType
	self:CreateFromRes("DirectWndListItem", parent)
	self:DrawList()
end

function CDirectListItem:OnChildCreated()
	self.m_List			= self:GetDlgChild("List")
	self.m_UpBtn		= self:GetDlgChild("UpBtn")
	self.m_DownBtn		= self:GetDlgChild("DownBtn")
	self.m_Label		= self:GetDlgChild("Label")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_ItemIcon		= self:GetDlgChild("ItemIcon")
	self.m_ItemLabel	= self:GetDlgChild("ItemLabel")
	self.m_OkBtn:ShowWnd(false)
	self.m_VerticalScroll = self.m_List:GetVerticalScroll()
	self.m_VerticalScroll:SetCutWnd(self.m_FatherWnd.m_List)
end

function CDirectListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then
			Gac2Gas:GetDirectAwardItem(g_Conn, self.m_FatherWnd.m_nBigType, self.m_sActionType)
		elseif(Child == self.m_UpBtn) then
			local pos = self.m_VerticalScroll:GetPos()
			self.m_VerticalScroll:SetPos(pos - 1)
			self:EnableUpBottomBtn()
		elseif(Child == self.m_DownBtn) then
			local pos = self.m_VerticalScroll:GetPos()
			self.m_VerticalScroll:SetPos(pos + 1)
			self:EnableUpBottomBtn()
		end
	end
end

function CDirectListItem:EnableUpBottomBtn()
	self.m_UpBtn:ShowWnd(true)
	self.m_DownBtn:ShowWnd(true)
	if( self.m_VerticalScroll:Istop() ) then
		self.m_UpBtn:ShowWnd(false)
	end
	if(self.m_VerticalScroll:Isbottom()) then
		self.m_DownBtn:ShowWnd(false)
	end
end

function CDirectListItem:SetInfo(tblInfo)
	self.m_Label:SetWndText( Lan_DirectInfo_Common( MemH64(self.m_sActionType), "DisplayName" ) )
	local tbl = tblInfo.AwardItem[1]
	if( tbl and next(tbl) ) then
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(tbl[1], tbl[2],"SmallIcon")
		g_LoadIconFromRes(SmallIcon, self.m_ItemIcon, -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self.m_ItemIcon, tbl[1], tbl[2], 0)
		self.m_ItemIcon:SetWndText(tbl[3])
		self.m_ItemIcon:ShowWnd(true)
	else
		self.m_ItemIcon:ShowWnd(false)
	end
end

function CDirectListItem:DrawList()
	self.m_List:DeleteAllItem()
	self.m_tblDirectNameTrans	= {}
	self.m_tblListItemWnd		= {}
	self.m_List:InsertColumn(0, self.m_List:GetWndOrgWidth())
	local nActionType = g_DirectMgr.m_tblActionTypeTrans[self.m_sActionType]
	local tbl = g_DirectMgr.m_DirectInfoTbl[self.m_FatherWnd.m_nBigType][nActionType]["DirectList"]
	for i, v in ipairs(tbl) do
		self.m_tblDirectNameTrans[v] = i
		self.m_List:InsertItem(i-1, 79)
		local itemWnd = self:CreateListItemListItem(self.m_List:GetSubItem(i-1, 0), v, self)
		table.insert(self.m_tblListItemWnd, itemWnd)
		itemWnd:SetInfo()
	end
	self:EnableUpBottomBtn()
end

function CDirectListItem:CanGetDirectAwardItem()
	for i, v in pairs(self.m_tblListItemWnd) do
		if(-1 ~= v.m_nState) then
			return
		end
	end
	self.m_OkBtn:ShowWnd(true)
	self.m_ItemLabel:ShowWnd(false)
end

----------------------------------------------------------------------------
function CDirectListItem:CreateListItemListItem(parent, sDirectName, fatherWnd)
	local wnd = CDirectListItemListItem:new()
	wnd:Init(parent, sDirectName, fatherWnd)
	wnd:ShowWnd(true)
	return wnd
end

function CDirectListItemListItem:Init(parent, sDirectName, fatherWnd)
	self.m_FatherWnd	= fatherWnd
	self.m_sDirectName	= sDirectName
	self:CreateFromRes("DirectWndListItemListItem", parent)
end

function CDirectListItemListItem:OnChildCreated()
	self.m_Icon			= self:GetDlgChild("Icon")
	self.m_DoneIcon		= self:GetDlgChild("DoneIcon")
	self.m_OkBtn		= self:GetDlgChild("OkBtn")
	self.m_IconLabel	= self:GetDlgChild("IconLabel")
	self.m_RichText		= self:GetDlgChild("RichText")
end

function CDirectListItemListItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then
			Gac2Gas:FinishPlayerDirect(g_Conn, self.m_sDirectName)
		end
	elseif(uMsgID == RICH_CLICK )	then
		local value = self.m_IconLabel:GetChooseStrAttr()
		local SceneName = pos_Direct_Common(value, "SceneName")
		local PosX = pos_Direct_Common(value, "PosX")
		local PosY = pos_Direct_Common(value, "PosY")
		local Mark = pos_Direct_Common(value, "Mark")
--		print(Mark,SceneName,PosX,PosY)
		PlayerAutoTrack(Mark,SceneName,PosX,PosY)
	end
end

function CDirectListItemListItem:SetInfo()
	local icon = Direct_Common(self.m_sDirectName, "EnableIcon")
	g_LoadIconFromRes(icon, self.m_Icon, -1, IP_ENABLE, IP_ENABLE)
	icon = Direct_Common(self.m_sDirectName, "DisableIcon")
	g_LoadGrayIconFromRes(icon, self.m_Icon, -1, IP_DISABLE, IP_DISABLE)
	
	local str = Lan_Direct_Common(MemH64(self.m_sDirectName), "SimpleName")
	local Link = Direct_Common(self.m_sDirectName, "Link")
	if Link then
		if string.find(Link, "#camp#") then
			local Camp = g_MainPlayer:CppGetCamp()
			Link = string.gsub(Link, "#camp#", Camp)
		end
		str = AutoTrackColorStr.."#u#l"..str.."#i[".. MemH64(Link) .."]#l#u#W"
	end
	self.m_IconLabel:SetWndText( str )
	self:ChangeState()
end

function CDirectListItemListItem:SetBK(bFlag)
	local rect		= CFRect:new()
	local pos		= CFPos:new()
	local state		= nil
	local textColor	= ""
	if(bFlag) then
		rect.left, rect.right, rect.top, rect.bottom, pos.x, pos.y = 15, 497, 88, 173, -1, -4
		state, textColor = IP_ENABLE, "#ca49e80"
	else
		rect.left, rect.right, rect.top, rect.bottom, pos.x, pos.y = 15, 497, 174, 255, -1, 0
		state, textColor= IP_DISABLE, "#c636363"
	end
	local texStr = g_ImageMgr:GetImagePath(1335)
	g_DynImageMgr:AddImageNeedCut(self, texStr, IP_ENABLE, rect, pos)
	self.m_Icon:EnableWnd(bFlag)
	self.m_OkBtn:EnableWnd(bFlag)
	self.m_RichText:SetWndText( textColor .. Lan_Direct_Common(MemH64(self.m_sDirectName), "Description") )
end

function CDirectListItemListItem:ChangeState(state)
	self.m_nState = state
	local bDone	= -1 == state
	local CanDo	= 0 == state
	self:SetBK(CanDo or bDone)
	self.m_OkBtn:SetWndText(CanDo and fl_TblStaticText["点击后达成"] or fl_TblStaticText["未激活"])
	self.m_OkBtn:ShowWnd(not bDone)
	self.m_DoneIcon:ShowWnd(bDone)
	self:DelFlashAttention()
	
	if(bDone) then
		g_GameMain.m_DirectShowWnd:CloseOneBtn(self.m_sDirectName)
	end
end

--------------------------------------------------------------------------

function Gas2Gac:ShowNewDirectBegin(Conn)
	local tbl = g_GameMain.m_DirectWnd.m_tblList
	for i, v in ipairs(tbl) do
		v:DrawList()
	end
end

function Gas2Gac:ShowNewDirect(Conn, DirectName, state) --每条指引的状态更新 0可盖章 -1已达成
	local nBigType, sActionType = g_DirectMgr:GetDirectType(DirectName)
	if not nBigType then
		return
	end
	if state == 0 then
		g_GameMain.m_DirectShowWnd:SetDirectBtnPicture(DirectName)
	end
	g_GameMain.m_DirectWnd:ChangeDirectState(nBigType, sActionType, DirectName, state)
end

function Gas2Gac:ShowNewDirectEnd(Conn)

end

function Gas2Gac:UpdateDirectAwardInfo(Conn, sActionType)
--	print("UpdateDirectAwardInfo",ActionType)
	local nBigType			= DirectInfo_Common(sActionType, "BigType")
	local BigTypeWnd		= g_GameMain.m_DirectWnd.m_tblList[nBigType]
	local nActionIndex		= g_DirectMgr.m_tblActionTypeTrans[sActionType]
	local ActionWnd			= BigTypeWnd.m_tblListItemWnd[nActionIndex]
	ActionWnd.m_OkBtn:ShowWnd(false)
	ActionWnd.m_ItemLabel:ShowWnd(true)
	ActionWnd.m_ItemLabel:SetWndText(fl_TblStaticText["已领取"])
end