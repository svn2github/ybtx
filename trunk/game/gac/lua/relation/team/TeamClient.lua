gac_require "relation/team/TeamClientInc"
gac_require "framework/texture_mgr/DynImageMgr"

AMID2NameMap = {}

--AMID2NameMap[0] = GacMenuText(1405) --"轮流拾取"
AMID2NameMap[0] = GacMenuText(1401) --"自由拾取"
AMID2NameMap[1] = GacMenuText(1403) --"平均分配"
AMID2NameMap[2] = GacMenuText(1402) --"各自拾取"
AMID2NameMap[3] = GacMenuText(1404) --"拍卖模式"
AMID2NameMap[4] = GacMenuText(1406) --"需求分配"


AAMID2NameMap = {}

--AMID2NameMap[0] = "轮流拾取"
AAMID2NameMap[0] = GacMenuText(1501) --"灰色"
AAMID2NameMap[1] = GacMenuText(1502) --"白色"
AAMID2NameMap[2] = GacMenuText(1503) --"绿色"
AAMID2NameMap[3] = GacMenuText(1504) --"蓝色"
AAMID2NameMap[4] = GacMenuText(1505) --"紫色"
AAMID2NameMap[5] = GacMenuText(1506) --"橙色"
AAMID2NameMap[6] = GacMenuText(1507) --"黄色"
AAMID2NameMap[7] = GacMenuText(1508) --"青色"
--AAMID2NameMap[8] = GacMenuText(1509) --"黑色"
--AAMID2NameMap[9] = GacMenuText(1510) --"紫色"

DIID2NameMap = {}

DIID2NameMap[0] = GacMenuText(1601) --"无"
DIID2NameMap[1] = GacMenuText(1602) --"稀有物品"
DIID2NameMap[2] = GacMenuText(1603) --"珍稀物品"
DIID2NameMap[3] = GacMenuText(1604) --"宝贵物品"
DIID2NameMap[4] = GacMenuText(1605) --"独有物品"

local function SetPos(wnd, x, y)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x+uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y+uHeight
	end
	wnd:SetLogicRect(Rect)
end

--邀请入队的MessageBox回调函数
local function TeamInviteOnMsgBoxClick(context, index)
	local InviterID, InviterName = unpack(context)
	if(index == 1) then --确定
		if( not (g_MainPlayer.m_Properties:GetTeamID() > 0) ) then --被邀请方已经建立了队伍
			Gac2Gas:RespondInvite(g_Conn, InviterID, InviterName, true)
		end
	elseif(index == 2) then --拒绝
		Gac2Gas:RespondInvite(g_Conn, InviterID, InviterName, false)
	end
	g_GameMain.m_TeamMsgBox = nil
	return true
end

--申请入队的MessageBox回调函数
local function TeamInviteOnAppMsgBoxClick(context, index)
	local applicantID, applicantName = unpack(context)
	Gac2Gas:RespondApp(g_Conn, applicantID, applicantName, 1 == index)
	g_GameMain.m_TeamMsgBox = nil
	return true
end

------小队基类------------------------------------------------------------------------
function CreateTeamBase(Parent)
	local teamBase = CTeamBase:new()
	teamBase:Init(Parent)
	return teamBase
end

function CTeamBase:Init(Parent)
	self.m_tblTeamMark = {}
	self.m_tblTeamMemberInfo = {}
	self.m_bCaptain = false
	self.m_TeamSize = 0
	self.m_TeamID = 0
end

function CTeamBase:ShowMsgBox(nID, playerlev, class, sName, eAM)
	local string = ""
	if(eAM) then
		string = MsgBoxMsg(19, playerlev, class, sName, AMID2NameMap[eAM])
		g_GameMain.m_TeamMsgBox = MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), TeamInviteOnMsgBoxClick, {nID, sName})
	else
		string = MsgBoxMsg(21, playerlev, class, sName)
		g_GameMain.m_TeamMsgBox = MessageBox(g_GameMain, string, BitOr(MB_BtnOK,MB_BtnCancel), TeamInviteOnAppMsgBoxClick, {nID, sName})
	end
	g_GameMain.m_TeamMsgBox.m_ID = nID
end

function CTeamBase:ClearTeamShow()
	local tblWnd = g_GameMain.m_tblTeamMsgBtn
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	for i = 1, #tblWnd do
		tblWnd[i]:ShowWnd(false)
		tblTeamIcons[i]:ClearInfo()
		tblTeamIcons[i].m_BuffWnd:ClearAllBuffState()
	end
end

function CTeamBase:ClearAllTeamInfo()
	--在清空信息之前关掉视野内关注开关
	local tbl = self.m_tblTeamMemberInfo
	for i = 1, #tbl do
		local player = CCharacterFollower_GetCharacterByID(tbl[i][4])
		self:OnSetAgileValueBeCare(player, false)
		if g_MainPlayer then
			g_MainPlayer:DealHideFollowerOnTeamChanged(player, false)
		end
	end
	
	------
	self.m_bCaptain = false
	self.m_tblTeamMemberInfo = {}
	self:ClearTeamShow()
	
	--清空阵法中的小队信息
	--g_GameMain.m_BattleArrayMemPosWnd:SetMemInfo({})
	--g_GameMain.m_BattleArrayBase.m_tblBattleArrayMem = {}
	
	g_GameMain.m_AreaInfoWnd:DeleteTeammatePosWnd()			--清空区域面板的小队成员图标
end

function CTeamBase:OnSetAgileValueBeCare(Character, bFlag)
	if(not Character) then return end
		
	local target = g_MainPlayer.m_LockCenter.m_LockingObj
	if(target) then
		if(Character == target) then return end
			
		local targetType = target.m_Properties:GetType()
		if(targetType == ECharacterType.Npc or targetType == ECharacterType.Player) then
			local targetOfTarget = target.m_LockCenter.m_LockingIntObj
			if(targetOfTarget) then
				local targetOfTargetType = targetOfTarget.m_Properties:GetType()
				if(Character == targetOfTarget) then return end
			end
		end
	end
	Character:SetAgileValueBeCare(bFlag)
end

function CTeamBase:UpdateBasicInfoAll(captainId)
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	local tblTeamInfo = self.m_tblTeamMemberInfo
	for i=1, #tblTeamInfo do
		local playerId	= tblTeamInfo[i][1]
		local charName	= tblTeamInfo[i][2]
		local bOnline	= tblTeamInfo[i][3]
		local entityId	= tblTeamInfo[i][4]
		local class		= tblTeamInfo[i][5]
		local sex		= tblTeamInfo[i][6]
		local level		= tblTeamInfo[i][7]
		local player	= CCharacterFollower_GetCharacterByID(entityId)
		g_GameMain.m_tblTeamMsgBtn[i]:ShowWnd(true)
		tblTeamIcons[i]:EnableWnd(bOnline)
		tblTeamIcons[i]:SetName(charName)
		tblTeamIcons[i].m_uCharID	= playerId
		tblTeamIcons[i].m_uEntityId	= entityId
		tblTeamIcons[i].m_bOnline	= bOnline
		tblTeamIcons[i].uClass		= class
		tblTeamIcons[i].uSex		= sex
		tblTeamIcons[i]:SetHeadPic(class)
		tblTeamIcons[i]:SetMPPic(class)
		tblTeamIcons[i]:SetLevel(level)
		tblTeamIcons[i].m_WndCaptain:ShowWnd(playerId == captainId)
		
		if(player) then
			player:SetTargetBuff()
			self:OnSetAgileValueBeCare(player, true)
			g_MainPlayer:DealHideFollowerOnTeamChanged(player, true)
		end
	end
	
	local uTimeID = g_MainPlayer.m_Properties:GetTeamID()
	if self.m_TeamID == 0 and uTimeID ~= 0 then
		self.m_TeamID = uTimeID
	elseif self.m_TeamID ~= 0 and uTimeID ~= 0 and self.m_TeamID ~= uTimeID then
		self.m_TeamID = uTimeID
		CStatisticFightInfoWnd.GetWnd():OnEnterNewTeam()
	end
end

function CTeamBase:GetMemberNameByEntityId(nEntityId)
	local tblTeamInfo = self.m_tblTeamMemberInfo
	for i = 1, #tblTeamInfo do
		if ( nEntityId == tblTeamInfo[i][4] ) then
			return tblTeamInfo[i][2]
		end
	end
end

function CTeamBase:UpdateSyncInfoOneByEntityId(entityId)
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	for i = 1, #tblTeamIcons do
		if(entityId == tblTeamIcons[i].m_uEntityId) then
			tblTeamIcons[i]:UpdateSyncInfo()
			break
		end
	end
end

function CTeamBase:UpdateEntityId(nCharId, nEntityId)
	local tblTeamInfo = self.m_tblTeamMemberInfo
	for i = 1, #tblTeamInfo do
		if ( nCharId == tblTeamInfo[i][1] ) then
			tblTeamInfo[i][4] = nEntityId
			g_GameMain.m_tblTeamIcons[i].m_uEntityId = nEntityId
			break
		end
	end
end

function CTeamBase:UpdateSyncInfoAll()
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	for i = 1, #tblTeamIcons do
		tblTeamIcons[i]:UpdateSyncInfo()
	end
end

function CTeamBase:UpdateSyncInfoOneByCharId(nCharID, nCurHP, nMaxHP, nCurMP, nMaxMP)
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	for i = 1, #tblTeamIcons do
		if(nCharID == tblTeamIcons[i].m_uCharID) then
			tblTeamIcons[i]:UpdateHpAndMpByParam(nCurHP, nMaxHP, nCurMP, nMaxMP)
			break
		end
	end
end

function CTeamBase:IsTeamMember(entityId)
	if entityId == 0 then
		return false
	end
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	for i = 1, #tblTeamIcons do
		if(entityId == tblTeamIcons[i].m_uEntityId) then
			return true
		end
	end
	return false
end

function CTeamBase:GetTeammateWnd(entityId)
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	for i = 1, #tblTeamIcons do
		if(entityId == tblTeamIcons[i].m_uEntityId) then
			return tblTeamIcons[i]
		end
	end
	return nil
end

function CTeamBase:UpdateMemberLevelByID(nPlayerID, nLevel)
	local tblTeamIcons = g_GameMain.m_tblTeamIcons
	local tblTeamInfo = self.m_tblTeamMemberInfo
	for i, v in ipairs(tblTeamIcons) do
		if(v.m_uCharID == nPlayerID) then
			v:SetLevel(nLevel)
			break
		end
	end
end

function CTeamBase:UpdateBuff(DynamicId, description, DivNum, Time, RemainTime, BuffType, SmallIcon, StateName, uEntityID, InstallerID)
	local wnd = g_GameMain.m_TeamBase:GetTeammateWnd(uEntityID)
	if(wnd) then
		wnd.m_BuffWnd:UpdateBuffDebuff(DynamicId, description, DivNum, Time, RemainTime, BuffType, SmallIcon, StateName, uEntityID, InstallerID, true)
	end
end
--------------------------------------------------------------------------------------

function CreateTeamMsgBtn( parent )
	local wnd = CTeamMsgBtn:new()
	wnd:Init( parent )
	wnd:SetIsBottom(true)
	return wnd
end

function CTeamMsgBtn:Init( parent )
	self:CreateFromRes("TeamMemberMsgBtn", parent)
end

function CTeamMsgBtn:OnChildCreated()
	self.m_MsgBtn = self:GetDlgChild("MsgBtn")
end

function CTeamMsgBtn:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_RCLICK ) then
		if(self.m_MsgBtn == Child) then
			self.m_TeamIcon:InitMenu(uParam1+2, uParam2+2)
		end
	elseif( uMsgID == BUTTON_LCLICK ) then
		if(self.m_MsgBtn == Child) then
			self.m_TeamIcon:LockSelectedTeamMember()
		end
	end
end

function CTeamMsgBtn:SetPos(x, y)
	SetPos(self, x, y)
end
--------------------------------------------------------------------------------------

function CreateTeamIcon(Parent)
	local Wnd = CTeamIcon:new()
	Wnd:Init(Parent)
	Wnd:SetIsBottom(true)
	return Wnd
end

function CTeamIcon:Init(Parent)
	self:CreateFromRes("TeamMember", Parent)
	self:ShowWnd( true )
	self:ClearInfo()
end

function CTeamIcon:OnChildCreated()
	self.m_ProgressHP	= self:GetDlgChild("ProgressHP")
	self.m_ProgressMP	= self:GetDlgChild("ProgressMP")
	self.m_BtnName		= self:GetDlgChild("WndName")
	self.m_Level		= self:GetDlgChild("WndLV")
	self.m_WndFace		= self:GetDlgChild("WndFace")
	self.m_WndCaptain	= self:GetDlgChild("WndCaptain") --队长标记
	self.m_WndCaptain:ShowWnd(false)
end

function CTeamIcon:SetPos(x, y)
	SetPos(self, x, y)
end

function CTeamIcon:SetName(name)
	self.m_sName = name
	self.m_BtnName:SetWndText(name)
end

function CTeamIcon:SetHP(CurHP, MaxHP)
	self.m_ProgressHP:SetWndText( CurHP .. "/" .. MaxHP )
	self.m_ProgressHP:SetRange( MaxHP )
	self.m_ProgressHP:SetPos( CurHP )
	g_GameMain.m_CharacterInSyncMgr:ChangeHPByMe(self.m_ProgressHP)
end

function CTeamIcon:SetMP(CurMP, MaxMP)
	self.m_ProgressMP:SetWndText( CurMP .. "/" .. MaxMP )
	self.m_ProgressMP:SetRange( MaxMP )
	self.m_ProgressMP:SetPos( CurMP )
end

function CTeamIcon:SetLevel(nLevel)
	self.m_nLevel = nLevel
	self.m_Level:SetWndText(nLevel)
end

function CTeamIcon:SetHeadPic(nClass)
	local nCamp = g_MainPlayer:CppGetBirthCamp()
	local sEnableImg, sDisableImg = g_DynImageMgr:GetClassPic(nCamp, nClass)
	g_DynImageMgr:AddImageByIP(self.m_WndFace, sEnableImg, IP_ENABLE)
	g_DynImageMgr:AddImageByIP(self.m_WndFace, sDisableImg, IP_DISABLE)
end

function CTeamIcon:SetMPPic(nClass)
	if nClass == EClass.eCL_Warrior or nClass == EClass.eCL_OrcWarrior or nClass == EClass.eCL_DwarfPaladin then
		local strTex = g_ImageMgr:GetImagePath(1364)
		g_DynImageMgr:AddImageByIP(self.m_ProgressMP, strTex, IP_FILL)
	else
		local strTex = g_ImageMgr:GetImagePath(1365)
		g_DynImageMgr:AddImageByIP(self.m_ProgressMP, strTex, IP_FILL)
	end
end

function CTeamIcon:UpdateSyncInfo()
	if(0 == self.m_uEntityId) then return end
	self:UpdateHpAndMp()
	local StatisticFightInfoWnd = CStatisticFightInfoWnd.GetWnd()
	StatisticFightInfoWnd:OnBattleStateChanged(self.m_uEntityId,CTeamClientManager_GetInBattle(self.m_uEntityId))
end

function CTeamIcon:ClearInfo()
	self.m_bOnline		= false
	self.m_uCharID		= 0  --该队友的角色ID
	self.m_sName		= ""
	self.m_uClass		= 0
	self.m_uEntityId	= 0
	self.m_uSex			= 0
	self.m_ProgressHP:EndChangeDiff()
end

--更新血、魔
function CTeamIcon:UpdateHpAndMp()
	local curMp	= 0
	local maxMp	= 0

	local classStr = g_GetPlayerClassNameByID(self.uClass)
	
	if("矮人骑士" == classStr) then
		curMp = CTeamClientManager_GetEP(self.m_uEntityId)
		maxMp = CTeamClientManager_GetBaseEP(self.m_uEntityId)
	elseif("大剑士" == classStr or "兽人战士" == classStr) then
		curMp = CTeamClientManager_GetRP(self.m_uEntityId)
		maxMp = CTeamClientManager_GetBaseRP(self.m_uEntityId)
	else
		curMp = CTeamClientManager_GetMP(self.m_uEntityId)
		maxMp = CTeamClientManager_GetBaseMP(self.m_uEntityId)
	end
	
	local curHp = CTeamClientManager_GetHP(self.m_uEntityId)
	local maxHp = CTeamClientManager_GetBaseHP(self.m_uEntityId)
	
	self:UpdateHpAndMpByParam(curHp, maxHp, curMp, maxMp)
end

function CTeamIcon:UpdateHpAndMpByParam(curHp, maxHp, curMp, maxMp)
	self:SetHP(curHp, maxHp)
	self:SetMP(curMp, maxMp)
end

-------------------------------------------

------锁定点选对象------
function CTeamIcon:LockSelectedTeamMember()
	local Character = g_GameMain.m_CharacterInSyncMgr.m_tblCharacter[self.m_uEntityId]
	if(Character) then
		g_MainPlayer:LockObj(Character)
	end
end

------踢出队伍------
function CTeamIcon:OnKickoutTeam()
	Gac2Gas:RemoveTeamMember(g_Conn, self.m_uCharID)
end

------设为私聊------
function CTeamIcon:OnWhisper()
	g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(self.m_uCharID, self.m_sName)
end

------交易------
function CTeamIcon:OnTrade()
	if g_MainPlayer.m_ItemBagLock then
		MsgClient(160015)
		return 
	end
	Gac2Gas:SendTradeInvitation( g_Conn, self.m_uCharID )
end

------设为队长------
function CTeamIcon:OnSetCaptain()
	self.m_WndCaptain:ShowWnd(false)
	Gac2Gas:SetCaptain(g_Conn, self.m_uCharID)
end

------加为好友------
function CTeamIcon:OnAddFriend()
	local tblInfo	= {}
	tblInfo.id		= self.m_uCharID
	tblInfo.name	= self.m_sName
	g_GameMain.m_AssociationWnd:CreateAddToClassWnd(tblInfo, false)
end

--查看队友属性信息
local function OnSee(member_id)
	local Target = g_GetPlayerInfo(member_id)
	if not (Target and IsCppBound(Target)) then
		MsgClient(140)
		return
	end
	local Type = Target.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end	
	
	local Wnd = g_GameMain.m_AimStatusWnd
	Wnd:ClearAllInfo()
	if Wnd.m_Target ~= Target then
		Wnd.m_PlayerName:SetWndText(Target.m_Properties:GetCharName())
		local uEntityID = Target:GetEntityID()
		local viewinfoMgr = CFighterViewInfoMgr.GetWnd()
		viewinfoMgr:AddViewInfoByID(uEntityID)
		Wnd.m_Target = Target
	end
	Gac2Gas:RequestAimInfo(g_Conn,Target.m_uID)
	Gac2Gas:GetAimPlayerSoulNum(g_Conn,Target.m_uID)
end

------查看资料------
function CTeamIcon:OnSee()
	OnSee(self.m_uCharID)
end

------邀请进入副本------
function CTeamIcon:OnInviteJoinScene()
--	local nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "疾传石" )
--	if nItemCount > 0 then
		COreSceneMSG.SendInviteJoinSceneMsgBox(self.m_uCharID)
--	else
--		MsgClient(191029)
--	end
end

function CTeamIcon:OnInvitePublicArea()
	Gac2Gas:PublicAreaInvite(g_Conn, self.m_uCharID)
end

function CTeamIcon:Destroy()
	self:DestroyWnd()
end

-----Menu--------------------------------------------------------------------
function CTeamIcon:OnMenuMsg(func)
	func(self)
	self:DestroyMenu()
end

function CTeamIcon:DestroyMenu(func)
	if(self.m_Menu) then
		self.m_Menu:Destroy()
		self.m_Menu = nil
	end
end

local ShowInviteMenuScene = {
[5] = true,
[12] = true,
[16] = true,
[21] = true,
}
local function IsShowInviteMenu()
	if ShowInviteMenuScene[g_GameMain.m_SceneType] then
		return true
	end
	return false
end

local function IsShowPublicInviteMenu()
	return g_GameMain.m_SceneType == 25
end

function CTeamIcon:InitMenu( x, y )
	local isCap = g_GameMain.m_TeamBase.m_bCaptain --不是同步信息，所以要在次此设定
	self:DestroyMenu()
	local Menu = CMenu:new("TargetMenu", g_GameMain)
	local tbl = {	{GacMenuText(801),	self.OnKickoutTeam,},		--请离队伍
					{GacMenuText(802),	self.OnSetCaptain,	},		--设为队长
					{GacMenuText(803),	self.OnSee,		},			--查看资料
					{GacMenuText(804),	self.OnAddFriend,	},		--加为好友
					{GacMenuText(805),	self.OnTrade		},		--交易
					{GacMenuText(806),	self.OnWhisper		},		--私聊
					{GacMenuText(807),	self.OnInviteJoinScene},	--邀请进入副本
					{GacMenuText(808),	self.OnInvitePublicArea},}		--邀请进入公共区域副本
	if(isCap) then
		for i = 1, 8 do
			if tbl[i][1] == GacMenuText(807) then
				if IsShowInviteMenu() then
					Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
				end
			elseif tbl[i][1] == GacMenuText(808) then
				if IsShowPublicInviteMenu() then
					Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
				end	
			else
				Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
			end
		end
		Menu:EnableItem(1, self.m_bOnline)
		Menu:EnableItem(4, self.m_bOnline)
	else
		for i = 3, 8 do
			if tbl[i][1] == GacMenuText(807) then
				if IsShowInviteMenu() then
					Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
				end
			elseif tbl[i][1] == GacMenuText(808) then
				if IsShowPublicInviteMenu() then
					Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
				end				
			else
				Menu:InsertItem(tbl[i][1], self.OnMenuMsg, nil, false, false, nil, self, tbl[i][2])
			end
		end
		Menu:EnableItem(2, self.m_bOnline)
	end
		
	self.m_Menu = Menu
	self.m_Menu:SetPos(x, y)
end

-----------------------------------RPC函数----------------------------------------

--邀请者邀请组队
function Gas2Gac:ReturnInviteMakeTeam(Conn, nInviterID, sInviterName, playerlev, playerclass, eAM)

	if( g_GameMain.m_AssociationBase:IsInBlackList(InviterID) ) then
		return
	end
	if( g_GameMain.m_TeamMsgBox and g_GameMain.m_TeamMsgBox:IsShow() ) then
		if(g_GameMain.m_TeamMsgBox.m_ID == nInviterID ) then
			return
		else
			g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateTeamInvMsgMinimum(nInviterID, playerlev, playerclass, sInviterName, eAM)
		end
	else

		local class = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(playerclass)
		g_GameMain.m_TeamBase:ShowMsgBox(nInviterID, playerlev, class, sInviterName, eAM)

	end
end

--队长得到申请组队信息
function Gas2Gac:ReturnAppJoinTeam(Conn, nApplicantID, sApplicantName, playerlev, playerclass)
	if( g_GameMain.m_AssociationBase:IsInBlackList(applicantID) ) then
		return
	end
	if( g_GameMain.m_TeamMsgBox and g_GameMain.m_TeamMsgBox:IsShow() ) then
		if(g_GameMain.m_TeamMsgBox.m_ID == nInviterID ) then
			return
		else
			g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateTeamAppMsgMinimum(nApplicantID, playerlev, playerclass, sApplicantName)
		end
	else
		local class = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(playerclass)
		g_GameMain.m_TeamBase:ShowMsgBox(nApplicantID, playerlev, class, sApplicantName)
	end
end

--清除小队头像（被踢出或小队解散）
function Gas2Gac:NotifyNoTeam(Conn)
	g_GameMain.m_TeamBase:ClearAllTeamInfo()
	--更新聊天频道信息
	local Wnd = g_GameMain.m_CreateChatWnd.m_ChatSendArea
	Wnd:SetDefaultChannel(ChannelNameTbl[6])
	Wnd:SetDefaultChannel(ChannelNameTbl[8])
	
	--清除小队标记
	g_GameMain.m_TeamBase.m_tblTeamMark = {}
	g_GameMain.m_TeamMarkSignMgr:UpdateCharacterInView()
	g_GameMain.m_TeamMarkSignMgr:PlayerHeadTeamMarkShowFalse()
	
	--玩法组，队伍解散在区域面板上不再显示队友标示点
	g_GameMain.m_AreaInfoWnd:DeleteTeammatePosWnd()
	g_GameMain.m_CharacterInSyncMgr:UpdateCharacterInSync()
end

--通知更新队友头像
function Gas2Gac:UpdateTeammateIcon(Conn)
	Gac2Gas:GetTeamMembers(Conn)
end

--返回小队成员信息开始
function Gas2Gac:ReturnGetTeamMemberBegin(Conn)
	g_GameMain.m_TeamBase:ClearAllTeamInfo()
end

--返回小队成员信息
function Gas2Gac:ReturnGetTeamMember(Conn, memberId, name, bOnline, entityId, class, sex, level)
	local tblTeamInfo = g_GameMain.m_TeamBase.m_tblTeamMemberInfo
	table.insert(tblTeamInfo, {memberId, name, bOnline, entityId, class, sex, level})
end

--返回小队成员信息结束
function Gas2Gac:ReturnGetTeamMemberEnd(Conn, captainId, teamSize)
	local isCap = g_MainPlayer and g_MainPlayer.m_Properties:GetCharID() == captainId
	g_GameMain.m_TeamBase.m_bCaptain = isCap
	g_GameMain.m_TeamBase.m_TeamSize = teamSize
	
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOneType("组队申请")
	g_GameMain.m_TeamBase:UpdateBasicInfoAll(captainId)
	g_GameMain.m_TeamBase:UpdateSyncInfoAll()
	g_GameMain.m_CharacterInSyncMgr:UpdateCharacterInSync()
	g_GameMain.m_CharacterInSyncMgr:UpdateTargetColor()
end

--返回小队成员信息
function Gas2Gac:ReceiveTeamMemberEntityId(Conn, memberId, entityId)
	g_GameMain.m_TeamBase:UpdateEntityId(memberId, entityId)
end

--更新分配方式
function Gas2Gac:UpdateAssignMode(Conn, eAM)
	--修改主角头像菜单的"分配方式"项
	local str = AMID2NameMap[eAM]
	MsgClient(104, str)
	local PlayerInfo = g_GameMain.m_PlayerInfo
	local Menu = PlayerInfo.m_Menu
	PlayerInfo.m_AssignMode = eAM
	if Menu and Menu:IsShow() then
		PlayerInfo:Init_menu()
	end
end

function Gas2Gac:UpdateAuctionStandard(Conn, Standard)
	--修改主角头像菜单的"拍卖品质设定"项
	local str = AAMID2NameMap[Standard]
	MsgClient(105, str)
	local PlayerInfo = g_GameMain.m_PlayerInfo
	local Menu = PlayerInfo.m_Menu
	PlayerInfo.m_AuctionStandard = Standard
	if Menu and Menu:IsShow() then
		PlayerInfo:Init_menu()
	end
end

function Gas2Gac:UpdateAuctionBasePrice(Conn, BasePrice)
	--修改主角头像菜单的"拍卖底价设定"项
	gac_gas_require "framework/common/CMoney"
	local moneyMgr = CMoney:new()
	local str = moneyMgr:ChangeMoneyToString(BasePrice, EGoldType.GoldCoin)
	if g_GameMain.m_PlayerInfo.m_AssignMode == EAssignMode.eAM_AuctionAssign then
		MsgClient(108, str)
	end
	local PlayerInfo = g_GameMain.m_PlayerInfo
	local Menu = PlayerInfo.m_Menu
	PlayerInfo.m_AuctionBasePrice = BasePrice
	if Menu and Menu:IsShow() then
		PlayerInfo:Init_menu()
	end
end

--已经下线
function Gas2Gac:NotifyOffline(Conn, MemberID)
	--将该Player从客户端的管理器中删除
	--g_DelPlayerInfo(MemberID)
	
	--玩法组，队友下线在区域面板上不再显示队友标示点
	if(g_GameMain) then
		g_GameMain.m_AreaInfoWnd:TeammateOffline(MemberID)
	end
	
	Gac2Gas:GetTeamMembers(Conn)
end

--已经上线
function Gas2Gac:NotifyOnline(Conn, MemberID)
	Gac2Gas:GetTeamMembers(Conn)
end

--返回标记信息
function Gas2Gac:ReturnUpdateTeamMark(Conn, uMarkType, uTargetType, uTargetID)
	local tbl		= {}
	tbl.uMarkType	= uMarkType
	tbl.uTargetType	= uTargetType
	tbl.uTargetID	= uTargetID
	local tblTeamMark = g_GameMain.m_TeamBase.m_tblTeamMark
	for i = #tblTeamMark, 1, -1 do
		if( (tblTeamMark[i].uTargetType == uTargetType and tblTeamMark[i].uTargetID == uTargetID) or tblTeamMark[i].uMarkType == uMarkType ) then
			table.remove(tblTeamMark, i) --从尾部删除
		end
	end
	table.insert(tblTeamMark, tbl)
end

--返回标记信息
function Gas2Gac:ReturnUpdateTeamMarkEnd(Conn)
	g_GameMain.m_TeamMarkSignMgr:UpdateCharacterInView()
end

function Gas2Gac:ReturnUpdateHPMPByCharID(Conn, nCharID, nCurHP, nMaxHP, nCurMP, nMaxMP)
	g_GameMain.m_TeamBase:UpdateSyncInfoOneByCharId(nCharID, nCurHP, nMaxHP, nCurMP, nMaxMP)
end

function Gas2Gac:SendTeamMemberLevel(Conn, nPlayerID, nLevel)
	g_GameMain.m_TeamBase:UpdateMemberLevelByID(nPlayerID, nLevel)
end
