CTongForageQueryWnd = class(SQRDialog)
CInfoListItem = class(SQRDialog)
lan_load "tong/Lan_TongAutoSeek_Common"
local StationLineModulus = {} --驻地团贡奖励系数
StationLineModulus[1] = 100
StationLineModulus[2] = 200
StationLineModulus[3] = 300
StationLineModulus[4] = 400

function CTongForageQueryWnd:Ctor(partent)
	self:CreateFromRes("TongForageQueryWnd", partent)
	self.m_InfoList		= self:GetDlgChild("InfoList")
	self.m_Title			= self:GetDlgChild("Title")
	self.m_ForageBtn	= self:GetDlgChild("ForageBtn")
	self.m_XBtn	      = self:GetDlgChild("XBtn")
	self.m_TongName = self:GetDlgChild("TongName")
	--self.m_WarZoneId = self:GetDlgChild("WarZoneId")
	self.m_StationId = self:GetDlgChild("StationId")
	self.m_Num = self:GetDlgChild("Num")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
--	self.m_tblTongInfo	= {}
	self:GetWnd()
end

function CTongForageQueryWnd:GetWnd()
	g_GameMain.m_IssueFetchWnd = self
	self:ShowWnd(true)
	return self 
end


function CTongForageQueryWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_ForageBtn) then
			--客户端发消息到服务端,判断
			Gac2Gas:RetIssueFetchWnd(g_Conn)
			self:ShowWnd(false)
		end
	end
end

function CTongForageQueryWnd:InitWnd()
	self.m_InfoList:DeleteAllItem()
	self.m_InfoList:InsertColumn(0, self.m_InfoList:GetWndOrgWidth())
	self:ShowWnd(true)
end

function CTongForageQueryWnd:OpenPanel(tongName, warzone, stationId, lineId, time)
	self:DrawMemberList(tongName, warzone, stationId, lineId, time)
end

------帮会成员列表行----------------------------------------------------------------
function CTongForageQueryWnd:DrawMemberList(tongName, warzone, stationId, lineId, time)
	local ItemCount1 = self.m_InfoList:GetItemCount()
	self.m_InfoList:InsertItem(ItemCount1, 30) 
	local item = self.m_InfoList:GetSubItem(ItemCount1, 0)
	local infoItem = self:CreateTongListItemWnd(item)
	infoItem:SetTongMemListText(tongName, warzone, stationId, lineId, time)
	item.m_Wnd = infoItem
	self:ShowWnd(true)
end


------创建帮会成员列表的行窗体
function CTongForageQueryWnd:CreateTongListItemWnd(parent)
	local wnd = CInfoListItem:new()
	wnd:CreateFromRes("TongInfoList", parent)
	wnd:ShowWnd(true)
	return wnd
end

--*************************
------list的子窗体类-------
--*************************
function CInfoListItem:OnChildCreated()
	self.m_Name		= self:GetDlgChild("TongName")
	--self.m_WarZone	= self:GetDlgChild("WarZone")
	self.m_Station	= self:GetDlgChild("Station")
	self.m_Num	= self:GetDlgChild("Num")
end

function CInfoListItem:SetTongMemListText(tongName, warzone, stationId, lineId, time)
	--local warZone = GetStaticTextClient(10039, warzone)
	self.m_Name:SetWndText(tongName)
	--self.m_WarZone:SetWndText(warZone)
	local index = MemH64(stationId..""..warzone)
	local stationName = Lan_TongAutoSeek_Common(index, "displayName")
	self.m_Station:SetWndText(stationName)
	local time = time.."/"..StationLineModulus[lineId]
	self.m_Num:SetWndText(time)
end


