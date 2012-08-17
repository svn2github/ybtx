CTongForageSeekWnd = class(SQRDialog)
cfg_load "tong/TongAutoSeek_Common"
lan_load "tong/Lan_TongAutoSeek_Common"
local AutoTrackColorStr = "#c"..g_ColorMgr:GetColor("自动寻路")

local StationAndPos = {}
StationAndPos["111"] = true
StationAndPos["121"] = true
StationAndPos["122"] = true
StationAndPos["123"] = true
StationAndPos["211"] = true
StationAndPos["221"] = true
StationAndPos["222"] = true
StationAndPos["223"] = true
StationAndPos["311"] = true
StationAndPos["321"] = true
StationAndPos["322"] = true
StationAndPos["323"] = true


local function SetWndRect(ParentWnd,ChildWnd)
	local rt = CFRect:new()
	ParentWnd:GetLogicRect(rt)
	
	local rt_1 = CFRect:new()
	ChildWnd:GetLogicRect(rt_1)
	local width = rt_1.right-rt_1.left
	local height = rt_1.bottom-rt_1.top
	rt_1.right = rt.right - 10
	rt_1.left = rt_1.right - width
	ChildWnd:SetLogicRect(rt_1)
end


function CTongForageSeekWnd:Ctor(parent)
	self:CreateFromRes("ForageSeekWnd", parent)
	self.m_LinkInfo = self:GetDlgChild("LinkInfo")
	self:GetWnd()
end

function CTongForageSeekWnd:GetWnd()
	SetWndRect(g_GameMain,self)
	g_GameMain.m_ForageSeekWnd = self
	self:ShowWnd(true)
	return self 
end


function CTongForageSeekWnd:OpenPanel(stationId,warZoneId)
	self:DrawCountInfoList(stationId,warZoneId)
end

function CTongForageSeekWnd:CreatePanel(sceneType, stationId,sceneName,posx, posy, tongId,warZoneId)
	self:CreateCountInfoList(sceneType, stationId,sceneName, posx, posy, tongId,warZoneId)
end

function CTongForageSeekWnd:DrawCountInfoList(stationId,warZoneId)
	self:InsertSeekInfo(stationId,warZoneId)
	self:ShowWnd(true)
end

function CTongForageSeekWnd:CreateCountInfoList(sceneType, stationId,sceneName, posx, posy, tongId,warZoneId)
	self:CreatetSeekInfo(sceneType, stationId,sceneName, posx, posy, tongId,warZoneId)
	self:ShowWnd(true)
end

function CTongForageSeekWnd:DeleteWnd()
	self.m_LinkInfo:DeleteAllItem()
	self.m_LinkInfo:InsertColumn(0, 265)
end

local function GetDisplayName(index,warZoneId)
	local Index = MemH64(index .. warZoneId)
	--print("Index", Index)
	local sDisplayName	= Lan_TongAutoSeek_Common(Index, "displayName") 
	return sDisplayName
end

function CTongForageSeekWnd:CreatetSeekInfo(sceneType, stationId,sceneName, posx, posy, tongId,warZoneId)
	local ItemCount1 = self.m_LinkInfo:GetItemCount()
	self.m_LinkInfo:InsertItem(ItemCount1, 25)  --行高
	local Item1 = self.m_LinkInfo:GetSubItem(ItemCount1, 0)
	local itemWnd1 = CSeekInfoWnd:new(Item1)
	local str = GetStaticTextClient(320001)
	if sceneType == 7 then
		local index = stationId..""
		local displayName = GetDisplayName(index,warZoneId)
		local Index = index..","..warZoneId
		str = AutoTrackColorStr.."#u#l"..displayName.."#i[".. MemH64(Index) .."]#l#u#W"
	else
		str = AutoTrackColorStr.."#u#l"..str.."#i[".. MemH64(tongId) .."]#l#u#W"
	end
	itemWnd1.m_Info:SetWndText(str)
	Item1.m_Wnd = itemWnd1
end

function CTongForageSeekWnd:InsertSeekInfo(index,warZoneId)

	local ItemCount1 = self.m_LinkInfo:GetItemCount()
	self.m_LinkInfo:InsertItem(ItemCount1, 25)  --行高
	local Item1 = self.m_LinkInfo:GetSubItem(ItemCount1, 0)
	local itemWnd1 = CSeekInfoWnd:new(Item1)
	local displayName = nil
	local Index = index..","..warZoneId
	displayName = GetDisplayName(index,warZoneId)
	local str = AutoTrackColorStr.."#u#l"..displayName.."#i[".. MemH64(Index) .."]#l#u#W"
	itemWnd1.m_Info:SetWndText(str)
	Item1.m_Wnd = itemWnd1
end

function CTongForageSeekWnd.OnDestroyed()
	
	g_GameMain.m_ForageSeekWnd:ShowWnd(false)

end


