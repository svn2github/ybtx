lan_load "gm/Lan_GMActivity_Common"

CGMActivityWnd = class(SQRDialog) 
CGMActivityItemWnd = class(SQRDialog)

function CGMActivityWnd:Ctor()
	self:CreateFromRes("GmActivityWnd",g_GameMain)
	self.m_ActivityListWnd = self:GetDlgChild("ActivityList")
	self.m_CloseBtn = self:GetDlgChild("Close")
	self.m_tblCompenateItemInfo = {}
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end


function CGMActivityWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_CloseBtn)then
			self:ShowWnd(false)
		end
	end
end

function CGMActivityWnd:OpenWnd()
	self:SetActivityInfo()
	self:ShowWnd(true)
end

function CGMActivityWnd:ReturnGetCompensateItemInfoBegin(Conn,ActivityID) 
	ActivityID = tonumber(string.sub(tostring(ActivityID),1,2))
	self.m_tblCompenateItemInfo[ActivityID] = {}
end

function CGMActivityWnd:ReturnGetCompensateItemInfo(Conn,ActivityID,H64ItemName,item_left_num,begin_time,end_time) 
	local binding_type = 0
	local SendName = H64ItemName
	local tblItem = {}
	if not string.find(H64ItemName,"money") then
			local str_pos = string.find(H64ItemName,"&")
			if str_pos then
				binding_type = tonumber(string.sub(H64ItemName,str_pos+1,str_pos+1))
				H64ItemName = string.sub(H64ItemName,1,str_pos-1)
			end
			local H64RealName,SoulCount = g_ItemInfoMgr:GetSoulpearlInfo(H64ItemName)
			local ItemName = MemH64ToMbcs(H64RealName)
			tblItem = g_ItemInfoMgr:GetItemInfoByIndex(ItemName)
			if not tblItem or #tblItem == 0 then return end
			if SoulCount > 0 then
				tblItem[2] = tblItem[2] .. ":" .. SoulCount
			end
			local ItemInfo = g_ItemInfoMgr:GetItemFunInfo( tblItem[1], tblItem[2])
			binding_type = binding_type or ItemInfo( "BindingStyle" )
	else
		tblItem = {0,SendName}
	end
	local activity_id = tonumber(string.sub(tostring(ActivityID),1,2))
	if tblItem then
		table.insert(tblItem,SendName)
		table.insert(tblItem,item_left_num)
		table.insert(tblItem,begin_time)
		table.insert(tblItem,end_time)
		table.insert(tblItem,binding_type)
		table.insert(tblItem,ActivityID)
		self.m_tblCompenateItemInfo[activity_id] = self.m_tblCompenateItemInfo[activity_id] or {}
		table.insert(self.m_tblCompenateItemInfo[activity_id],tblItem)
	end
end

function CGMActivityWnd:ReturnActivationCodeCompensateItem(Conn,CompensateID,ItemType,ItemName,ItemCount)
	local RealActivityID = 1000000000
	local activity_id,binding_type = 10,2
	local begin_time,end_time = "",""
	local tblItem = {ItemType,ItemName,CompensateID,ItemCount,begin_time,end_time,binding_type,RealActivityID}
	self.m_tblCompenateItemInfo[activity_id] = self.m_tblCompenateItemInfo[activity_id] or {}
	table.insert(self.m_tblCompenateItemInfo[activity_id],tblItem)
end

function CGMActivityWnd:ReturnGetCompensateItemInfoEnd(Conn,ActivityID) 
	ActivityID = tonumber(string.sub(tostring(ActivityID),1,2))
	g_GameMain.m_GmCompensateItemWnd:OpenWnd(ActivityID)
end

function CGMActivityWnd:SetActivityInfo()
	self.ActivityWndTbl = {}
	self.m_NowActivityID = 0
	self.m_ActivityListWnd:DeleteAllItem()
	self.m_ActivityListWnd:InsertColumn(0, self.m_ActivityListWnd:GetWndOrgWidth())
	local keys = Lan_GMActivity_Common:GetKeys()
	for i, v in ipairs(keys ) do
		self.m_ActivityListWnd:InsertItem(i-1, 30)
		local embedvalue = CGMActivityItemWnd:new()
		local item = self.m_ActivityListWnd:GetSubItem( i-1, 0 )
		embedvalue:CreateFromRes( "GmActivityItem", item) 
		local itemvalue = item:GetDlgChild( "GmActivityItem" )
		embedvalue.ItemBtn = embedvalue:GetDlgChild( "ItemBtn" )
		embedvalue.ItemBtn:SetMouseWheel(true)
		embedvalue.ItemBtn:SetWndText(Lan_GMActivity_Common(v,"DisplayName"))
		embedvalue.ActivityID = v
		embedvalue.ItemBtn:ShowWnd(true)
		embedvalue:ShowWnd(true)
		table.insert(self.ActivityWndTbl, embedvalue)
	end
end

function CGMActivityItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2) 
	if(uMsgID == BUTTON_LCLICK ) then
		if(Child == self:GetDlgChild("ItemBtn"))then
			local WndTbl = g_GameMain.m_GmActivity.ActivityWndTbl
			for i =1,#(WndTbl) do
				if Child == WndTbl[i].ItemBtn then
						g_GameMain.m_GmActivity.m_NowActivityID = WndTbl[i].ActivityID
						if WndTbl[i].ActivityID == 1000000000 then
							Gac2Gas:GetActivationCodeCompenateItem(g_Conn)
						else
							Gac2Gas:GetCompensateItemInfo(g_Conn,g_GameMain.m_GmActivity.m_NowActivityID)
						end
						break
				end
			end
   	end       
	end
end





