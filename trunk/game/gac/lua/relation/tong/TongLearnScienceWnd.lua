lan_load "tong/Lan_TongFightTech_Common"
cfg_load "tong/TongFightTech_Common"


CTongLearnScience = class( SQRDialog )
CTongLearnScienceItem = class( SQRDialog )


local function GetConsumeStr(ScienceName,CurrentLevel)
	local NextLevel = CurrentLevel+1
	if not g_TongTechMgr:HasFightTechInfo(ScienceName, tostring(NextLevel)) then
		return false,""
	end
	local ConsumeTbl = g_TongFightTechToPlayerTbl[ScienceName][NextLevel]
	local ConsumeStr, ErrStr = "",""
	--消耗魂值
	if g_MainPlayer.m_PlayerSoulNum >= ConsumeTbl.ConsumeSoul then
		ConsumeStr = " " .. ConsumeTbl.ConsumeSoul .. MsgBoxMsg(6051)
	else
		ErrStr = " " .. ConsumeTbl.ConsumeSoul .. MsgBoxMsg(6051)
	end

	--消耗金币
	local c_money = CMoney:new()
	local MoneyStr = c_money:ChangeMoneyToString(ConsumeTbl.ConsumeMoney)	
	local allMoney =  tonumber(g_MainPlayer:GetBindMoney()) + tonumber(g_MainPlayer:GetMoney())
	if allMoney >= ConsumeTbl.ConsumeMoney then
		ConsumeStr = ConsumeStr .. " " .. MsgBoxMsg(12010,MoneyStr)
	else
		ErrStr = ErrStr .. " " ..  MsgBoxMsg(12010,MoneyStr)
	end
	
	--消耗国贡
	if g_MainPlayer.m_TongProffer >= ConsumeTbl.Proffer then
		ConsumeStr = ConsumeStr .. " " .. ConsumeTbl.Proffer .. MsgBoxMsg(6052) 
	else
		ErrStr = ErrStr .. " " .. ConsumeTbl.Proffer .. MsgBoxMsg(6052) 
	end
	
	--消耗队贡
	if g_MainPlayer.m_TeamProffer >= ConsumeTbl.TeamProffer then
		ConsumeStr = ConsumeStr .. " " .. ConsumeTbl.TeamProffer ..  MsgBoxMsg(6056)
	else
		ErrStr = ErrStr .. " " .. ConsumeTbl.TeamProffer .. MsgBoxMsg(6056)
	end
	
	--消耗物品
	for i ,v in pairs(ConsumeTbl.ConsumeItemTbl) do 
		local ItemNum = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, v[1], v[2] )
		local displayName = g_ItemInfoMgr:GetItemLanInfo(v[1], v[2],"DisplayName")
		if ItemNum >= v[3] then
			ConsumeStr = ConsumeStr .. " " .. v[3] .. displayName
		else
			ErrStr = ErrStr .. " " .. v[3] .. displayName
		end
	end
	
	if ErrStr == "" then
		return true,MsgBoxMsg(6053,ConsumeStr) 
	else
		return false,MsgBoxMsg(6054,ErrStr) 
	end
end

local function GetDescStr(ScienceName,CurrentLevel)
	local NextLevel = CurrentLevel+1
	if not g_TongTechMgr:HasFightTechInfo(ScienceName, tostring(NextLevel)) then
		return ""
	end
	local ConsumeTbl = g_TongFightTechToPlayerTbl[ScienceName][NextLevel]
	local DescStr = ""
	--消耗魂值
	DescStr = " " .. ConsumeTbl.ConsumeSoul .. MsgBoxMsg(6051)

	--消耗金币
	local c_money = CMoney:new()
	local MoneyStr = c_money:ChangeMoneyToString(ConsumeTbl.ConsumeMoney)	
	DescStr = DescStr .. " " .. MsgBoxMsg(12010,MoneyStr)
	
	--消耗国贡
	DescStr = DescStr .. "     " .. ConsumeTbl.Proffer .. MsgBoxMsg(6052) 
	
	--消耗队贡
	DescStr = DescStr .. " " .. ConsumeTbl.TeamProffer ..  MsgBoxMsg(6056)
	
	--消耗物品
	for i ,v in pairs(ConsumeTbl.ConsumeItemTbl) do
		local displayName = g_ItemInfoMgr:GetItemLanInfo(v[1], v[2],"DisplayName")
		DescStr = DescStr .. " " .. v[3] .. displayName
	end	
	
	return MsgBoxMsg(6050) .. DescStr
end

function CTongLearnScience:Ctor()
	self:CreateFromRes("TongLearnScienceWnd",g_GameMain)
	self.m_List = self:GetDlgChild("List")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_List.m_ItemNumber = 0;
	self.m_ItemTbl = {}
	self.m_List:InsertColumn(0,self.m_List:GetWndOrgWidth())
	local Keys = TongFightTech_Common:GetKeys()
	for i , v in pairs(Keys) do
		self:UpdateTongScienceInfo(v,0) 
	end
	g_ExcludeWndMgr:InitExcludeWnd(self, 3, false, nil, 1)
end

--根据名称得到ItemWnd
function CTongLearnScience:GetListItemByScienceName(ScienceName)
	if self.m_ItemTbl[ScienceName] then
		return self.m_ItemTbl[ScienceName]
	end
end

--创建ItemWnd
function CTongLearnScience:CreateListItem(ScienceName)
	self.m_List:InsertItem(self.m_List.m_ItemNumber,130)
	local Item = self.m_List:GetSubItem(self.m_List.m_ItemNumber,0)
	self.m_List.m_ItemNumber = self.m_List.m_ItemNumber + 1
	
	self.m_ItemTbl[ScienceName] = CTongLearnScienceItem:new(Item)
	self.m_ItemTbl[ScienceName].m_ScienceName = ScienceName
	self.m_ItemTbl[ScienceName].m_ScienceLevel = 0
	self.m_ItemTbl[ScienceName].m_TopLevelStatic:SetWndText(0)
	local DisplayName = Lan_TongFightTech_Common( MemH64(ScienceName .. 1), "DisplayName")
	self.m_ItemTbl[ScienceName].m_ScienceNameStatic:SetWndText(DisplayName)
	local SmallIcon = TongFightTech_Common(ScienceName,"1","SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_ItemTbl[ScienceName].m_IconBtn, -1, IP_ENABLE, IP_CLICKDOWN)
	self.m_ItemTbl[ScienceName].m_IconBtn:SetMouseOverDescAfter( g_Tooltips:GetTongFightTechTips(ScienceName, 0) )
	local Str = GetDescStr(ScienceName,0)
	self.m_ItemTbl[ScienceName].m_DescStatic:SetWndText(Str)
end

--更新帮会科技信息								
function CTongLearnScience:UpdateTongScienceInfo(ScienceName,ScienceLevel)
	local Item = self:GetListItemByScienceName(ScienceName)
	if Item then
		Item.m_TopLevelStatic:SetWndText(ScienceLevel)
		Item.m_ScienceLevel = ScienceLevel
	else
		self:CreateListItem(ScienceName)
	end
end

--更新个人可以信息
function CTongLearnScience:UpdatePlayerScienceInfo(ScienceName,CurrentLevel)
	local Item = self:GetListItemByScienceName(ScienceName)
	if Item then
		Item.m_CurrentLevelStatic:SetWndText(CurrentLevel)
		Item.m_CurrentLevel = CurrentLevel
		local ConsumeItemTbl = g_TongFightTechToPlayerTbl[ScienceName][CurrentLevel].ConsumeItemTbl
		Item.m_DescStatic:SetWndText(ConsumeItemTbl[1][2])
		Item.m_IconBtn:SetMouseOverDescAfter(g_Tooltips:GetTongFightTechTips(ScienceName,CurrentLevel))
		local Str = GetDescStr(ScienceName,CurrentLevel)
		Item.m_DescStatic:SetWndText(Str)
		local DisplayName = ""		
		if TongFightTech_Common:HasNode( ScienceName, tostring(CurrentLevel+1) ) then 
			DisplayName = Lan_TongFightTech_Common( MemH64(ScienceName .. CurrentLevel+1), "DisplayName")
		else
			DisplayName = Lan_TongFightTech_Common( MemH64(ScienceName .. CurrentLevel), "DisplayName")
		end
		Item.m_ScienceNameStatic:SetWndText(DisplayName)
	else
		--是否创建待定	
	end
end

function CTongLearnScience:OpenPanel(bFlag)
	self:ShowWnd(bFlag)
	if(bFlag) then
		local Keys = TongFightTech_Common:GetKeys()
		for i , v in pairs(Keys) do
			self:UpdateTongScienceInfo(v,0) 
		end		
		g_GameMain.m_TongScience:OnGetCreatingItemInfo()
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(self)
	end
end

function CTongLearnScience:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if Child == self.m_CloseBtn and uMsgID == BUTTON_LCLICK then
		self:ShowWnd(false)
	end
end
-------------------------------ItemWnd----------------------------------------

function CTongLearnScienceItem:Ctor(Parent)
	self:CreateFromRes("TongLearnScienceItemWnd",Parent)
	self.m_IconBtn = self:GetDlgChild("IconBtn")
	self.m_ScienceNameStatic = self:GetDlgChild("ScienceNameStatic")
	self.m_CurrentLevelStatic = self:GetDlgChild("CurrentLevelStatic")
	self.m_TopLevelStatic = self:GetDlgChild("TopLevelStatic")
	self.m_DescStatic = self:GetDlgChild("DescStatic")
	self.m_LearnBtn = self:GetDlgChild("LearnBtn")
	self.m_CurrentLevelStatic:SetWndText(0)
	self.m_TopLevelStatic:SetWndText(0)
	self.m_ScienceLevel = 0
	self.m_CurrentLevel = 0
	self:ShowWnd(true)
end

local function IsLearnScience(TongLearnScience,uButton)
	if uButton == MB_BtnOK then
		Gac2Gas:LearnTongTech(g_Conn,TongLearnScience.m_ScienceName)
	end
	TongLearnScience.m_MsgBox = nil
	return true
end

function CTongLearnScienceItem:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if Child == self.m_LearnBtn and uMsgID == BUTTON_LCLICK then 
		if self.m_CurrentLevel < self.m_ScienceLevel then
			local Flag ,MsgStr = GetConsumeStr(self.m_ScienceName,self.m_CurrentLevel)
			if Flag then
				self.m_MsgBox = MessageBox(g_GameMain, MsgStr, BitOr( MB_BtnOK, MB_BtnCancel), IsLearnScience, self)
			else
				self.m_MsgBox = MessageBox(g_GameMain, MsgStr, MB_BtnOK)
			end
		else
			self.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(6055), MB_BtnOK)
		end
	end
end