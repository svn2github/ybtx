gac_gas_require "relation/army_corps/ArmyCorpsMgr"

CArmyCorpsTeamInfo 			= class(SQRDialog)
CArmyCorpsTeamListItem	= class(SQRDialog)

function CArmyCorpsTeamInfo.CreateWnd(parent)
	local wnd = CArmyCorpsTeamInfo:new()
	wnd:Init(parent)
	return wnd
end

function CArmyCorpsTeamInfo:Init(parent)
	self:CreateFromRes("ArmyCorpsTeamInfo", parent)
	self.m_tblArmyCorpsMemInfo = {}
	self.m_MemberCount = 0
	self.m_MemberCountLimit = 0
end 

function CArmyCorpsTeamInfo:OnChildCreated()
	self.m_KickOutBtn			= self:GetDlgChild("KickOutBtn")
	self.m_LeaveBtn				= self:GetDlgChild("LeaveBtn")
	self.m_MemCount				= self:GetDlgChild("MemCount")
	self.m_MaxLimit				= self:GetDlgChild("MaxLimit")
	self.m_TeamMemberList	= self:GetDlgChild("TeamMemberList")
	
	self.m_tblSortBtn = {}
	self.m_tblSortBtn["Type"]= self:GetDlgChild("Type")
	self.m_tblSortBtn["Name"] = self:GetDlgChild("Name")
	self.m_tblSortBtn["AdminName"]	= self:GetDlgChild("CaptainName")
	self.m_tblSortBtn["Level"]			= self:GetDlgChild("Level")
	self.m_tblSortBtn["MemberNum"]	= self:GetDlgChild("MemberNum")
	self.m_tblSortBtn["Exploit"]		= self:GetDlgChild("Exploit")
	self.m_tblSortBtn["StationId"]	= self:GetDlgChild("Station")
	for p, v in pairs(self.m_tblSortBtn) do
		v.bFlag = true
	end
end

function CArmyCorpsTeamInfo:OpenPanel(bFlag)
	if bFlag then
		Gac2Gas:GetArmyCorpsTeamInfo(g_Conn)
	end
	self:ShowWnd(bFlag)
end

function CArmyCorpsTeamInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_KickOutBtn) then
			self:OnKickOutOfArmyCorpsBtn()
		elseif(Child == self.m_LeaveBtn) then
			self:OnLeaveArmyCorpsBtn()
		else
			for p, v in pairs(self.m_tblSortBtn) do
				if(Child == v) then
					self:SortMember(p, v.bFlag)
					v.bFlag = not v.bFlag
					break
				end
			end
		end
	end
end

function CArmyCorpsTeamInfo:UpdateArmyCorpsTeamInfoBegin(MemberCount,MemberCountLimit)
	self.m_tblArmyCorpsMemInfo = {}
	self.m_MemberCount = MemberCount
	self.m_MemberCountLimit = MemberCountLimit
end
function CArmyCorpsTeamInfo:UpdateArmyCorpsTeamInfo(tong_type,tong_name,leader_name,
														tong_level,member_num,tong_exploit,tong_id,warZoneId,stationId)
	local TeamInfoTbl = {}
	TeamInfoTbl.Type 			= tong_type
	TeamInfoTbl.Name 			= tong_name
	TeamInfoTbl.AdminName	= leader_name
	TeamInfoTbl.Level 		= tong_level
	TeamInfoTbl.MemberNum	= member_num
	TeamInfoTbl.Exploit 	= tong_exploit
	TeamInfoTbl.ID				= tong_id
	TeamInfoTbl.WarZoneId = warZoneId
	TeamInfoTbl.StationId = stationId
	table.insert(self.m_tblArmyCorpsMemInfo,TeamInfoTbl)
end

function CArmyCorpsTeamInfo:DrawArmyCorpsTeamInfo()
	local ShowMemCount = self.m_MemberCount ~= 0 and self.m_MemberCount or ""
	local ShowMemCountLimit = self.m_MemberCountLimit ~= 0 and self.m_MemberCountLimit or ""
	self.m_MemCount:SetWndText(ShowMemCount)
	self.m_MaxLimit:SetWndText(ShowMemCountLimit)
	self.m_TeamMemberList:DeleteAllItem()
	self.tblMemberItem = {}
	self.m_TeamMemberList:InsertColumn(0, self.m_TeamMemberList:GetWndOrgWidth())
	for i, v in ipairs(self.m_tblArmyCorpsMemInfo) do
		self.m_TeamMemberList:InsertItem(i-1, 30)
		local item = self.m_TeamMemberList:GetSubItem(i-1, 0)
		local memberItem = self:CreateArmyCorpsTeamListItemWnd(item)
		table.insert(self.tblMemberItem, memberItem)
		
		memberItem:SetMemListText(v.Type,v.Name,v.AdminName,v.Level,v.MemberNum,v.Exploit,v.WarZoneId,v.StationId)
	end
end


-----佣兵团的小队按照某属性排序------
function CArmyCorpsTeamInfo:SortMember(index, bFlag)
	if(bFlag) then
		table.sort(self.m_tblArmyCorpsMemInfo, function (a, b) return a[index] < b[index] end)
	else
		table.sort(self.m_tblArmyCorpsMemInfo, function (a, b) return a[index] > b[index] end)
	end
	self:DrawArmyCorpsTeamInfo()
end

function CArmyCorpsTeamInfo:CreateArmyCorpsTeamListItemWnd(parent)
	local wnd = CArmyCorpsTeamListItem:new()
	wnd:CreateFromRes("ArmyCorpsTeamListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CArmyCorpsTeamListItem:SetMemListText(Type,Name,AdminName,Level,MemberNum,Exploit,WarZoneId,StationId)
	local sShowType = g_GameMain.m_DisplayCommonObj:GetTongTypeName(Type)
	self:GetDlgChild("Label1"):SetWndText(sShowType)
	self:GetDlgChild("Label2"):SetWndText(Name)
	self:GetDlgChild("Label3"):SetWndText(AdminName)
	self:GetDlgChild("Label4"):SetWndText(Level)
	self:GetDlgChild("Label5"):SetWndText(MemberNum)
	self:GetDlgChild("Label6"):SetWndText(Exploit)
	self:GetDlgChild("Label7"):SetWndText(g_GameMain.m_TongBase:GetStationDes(WarZoneId, StationId))
end

function CArmyCorpsTeamInfo:OnKickOutOfArmyCorps(bSucc)
	if bSucc then
		Gac2Gas:GetArmyCorpsTeamInfo(g_Conn)
	end
end

------开除------
function CArmyCorpsTeamInfo:OnKickOutOfArmyCorpsBtn()
	local itemIndex = self.m_TeamMemberList:GetSelectItem(-1) + 1
	local tbl = self.m_tblArmyCorpsMemInfo
	if(not itemIndex or itemIndex < 1 or itemIndex > #tbl) then return end
	local tong_id = tbl[itemIndex].ID
	if not g_ArmyCorpsMgr:JudgePurview(g_GameMain.m_ArmyCorpsWnd.m_ArmyCorps_Pos,"KickOut") then
		MsgClient(8520)
		return
	end
	if tbl[itemIndex].AdminName == g_MainPlayer.m_Properties:GetCharName() then
		MsgClient(8523)
		return
	end
	local KickOutOfArmyCorps = function(wnd, uButton)
		if(uButton == MB_BtnOK) then
				Gac2Gas:KickOutOfArmyCorps(g_Conn, tong_id)
		end
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(16500), BitOr(MB_BtnOK, MB_BtnCancel), KickOutOfArmyCorps, g_GameMain)
end

------退团------
function CArmyCorpsTeamInfo:OnLeaveArmyCorpsBtn()
	if g_GameMain.m_TongBase.m_TongPos ~= g_TongMgr.m_tblPosInfo["团长"] then
		MsgClient(8516)
		return
	end
	if self.m_tblArmyCorpsMemInfo == 0 then
		MsgClient(8509)
		return
	end
	if (g_ArmyCorpsMgr:GetPosIndexByName("团长") == g_GameMain.m_ArmyCorpsWnd.m_ArmyCorps_Pos and 
		# self.m_tblArmyCorpsMemInfo ~= 1) then
		MsgClient(8518)
		return
	end
	local LeaveArmyCorps = function(wnd, uButton)
		if(uButton == MB_BtnOK) then
			Gac2Gas:LeaveArmyCorps(g_Conn)
		end
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(16501), BitOr(MB_BtnOK, MB_BtnCancel), LeaveArmyCorps, g_GameMain)
end

