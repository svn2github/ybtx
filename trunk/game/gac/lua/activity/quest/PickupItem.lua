CPickupItemWnd = class ( SQRDialog )
local CPickupItemGridCut = class ( SQRDialog )

--物品拾取动作成功

function CreatePickupItemWnd(parent)
	local Wnd = CPickupItemWnd:new()
	Wnd:CreateFromRes("WndCollObj",parent) 
	Wnd.m_CanCollItemTbl = {}
	Wnd.m_DeleteIndexNum = 0
	Wnd.m_AutoPickUpIndex = nil
	Wnd.m_AutoPickUpOneDrop = nil
	Wnd.m_CloseButton = Wnd:GetDlgChild("WndClose")
	Wnd.m_BtnGetAllItem = Wnd:GetDlgChild("WndGetAllItem")
	Wnd.m_BtnPageUp = Wnd:GetDlgChild("LastPage")
	Wnd.m_BtnPageDown = Wnd:GetDlgChild("NextPage")

	return Wnd
end

local m_AutoPickUpObjTbl = {
			["可拾取的Obj"] = true,
			["神符"] = true,
			["临时技神符"] = true,
			["可拾取的Obj临时技"] = true,
			["拾取矿石"] = true,
			["临时包拾取OBJ"] = true,
}

function AutoPickUpDropItems(index)
--	if g_MainPlayer then
--		for targetID, Npc in pairs(g_MainPlayer.m_NpcInVisionTbl) do
--			if not Npc:CppIsAlive() and g_GetDistance(Npc, g_MainPlayer) <=3 and index ~= targetID then
--				Gac2Gas:ShowNpcDropItemWnd(g_Conn,targetID)
--				g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex = targetID
--				return
--			end
--		end
--	end
--	return
	
	if IsCppBound(g_MainPlayer) then
		local moveDis = nil
		for targetID,Obj in pairs(g_MainPlayer.m_IntObjInVisionTbl) do
			local ObjName = Obj.m_Properties:GetCharName()
			local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
			if m_AutoPickUpObjTbl[TriggerAction] then
				moveDis = GetAutoTrackDistance(Obj)
				if g_GetDistance(Obj, g_MainPlayer) <= moveDis and CheckUseCondition(Obj) then				
					if g_MainPlayer:IsMoving() and not g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
						g_MainPlayer:StopMoving()
					end
					OnLClickIntObj(Obj)
					return
				end
			end
		end
		
		if g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
			return
		end
		
		local minPos = 8
		local MoveToObjID = nil
		for targetID,Obj in pairs(g_MainPlayer.m_IntObjInVisionTbl) do
			local ObjName = Obj.m_Properties:GetCharName()
			local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
			if m_AutoPickUpObjTbl[TriggerAction] then
				local nowpos = g_GetDistance(Obj, g_MainPlayer)
				if nowpos < minPos and CheckUseCondition(Obj) then
					minPos = nowpos
					moveDis = GetAutoTrackDistance(Obj)
					MoveToObjID = targetID
				end
			end
		end
		if MoveToObjID then
			g_MainPlayer:InitAutoTracker(g_MainPlayer:GetEntityID(), MoveToObjID, moveDis, AutoTrackerHandlerForIntObj:new(), false)
		end
	end
end

function CPickupItemWnd:SetAutoPickUpOneDrop(IsAuto)
	self.m_AutoPickUpOneDrop = IsAuto
end

function CPickupItemWnd:InitDropItemWnd(DropItemTbl)
	if self.StartIndex>self.EndIndex then
		return
	end
	local GetItemWnd={"GetItemBtn1","GetItemBtn2","GetItemBtn3","GetItemBtn4"}
	local ItemNameWnd={"ItemName1","ItemName2","ItemName3","ItemName4"}
	self.m_SelectItemWnd={}
	self.m_ItemNameWnd={}
	for i=1,4 do
		table.insert(self.m_SelectItemWnd,self:GetDlgChild(GetItemWnd[i]))
		local PicWnd = CPickupItemGridCut:new()
		PicWnd:CreateFromRes("CommonGridCut", self.m_SelectItemWnd[i])
		PicWnd:ShowWnd(true)
		self.m_SelectItemWnd[i].m_Picture = PicWnd
		table.insert(self.m_ItemNameWnd,self:GetDlgChild(ItemNameWnd[i]))
	end
	for i=1,4 do
		if i+self.StartIndex-1>self.EndIndex or DropItemTbl[i+self.StartIndex-1]==nil then
			self.m_SelectItemWnd[i]:ShowWnd(false)
			self.m_ItemNameWnd[i]:ShowWnd(false)
			self.m_SelectItemWnd[i]:EnableWnd(false)
			self.m_SelectItemWnd[i]:SetWndText("")
			self.m_ItemNameWnd[i]:SetWndText("")
		else
			local ItemType,ItemName,ItemNum,CanClick = DropItemTbl[i+self.StartIndex-1][1],DropItemTbl[i+self.StartIndex-1][2],DropItemTbl[i+self.StartIndex-1][3],DropItemTbl[i+self.StartIndex-1][4]
--			print("ItemType,ItemName,ItemNum",ItemType,ItemName,ItemNum)
			local SmallIcon = g_ItemInfoMgr:GetItemInfo(ItemType,ItemName,"SmallIcon")
			local DisplayName = g_ItemInfoMgr:GetItemLanInfo(ItemType,ItemName,"DisplayName")
			local picwnd = self.m_SelectItemWnd[i].m_Picture
			g_LoadIconFromRes(SmallIcon, picwnd, -1, IP_ENABLE, IP_ENABLE)
			picwnd:SetWndText(ItemNum)
			self.m_SelectItemWnd[i]:ShowWnd(true)
			if CanClick then
				self.m_SelectItemWnd[i]:EnableWnd(true)
				self.m_ItemNameWnd[i]:SetWndText(DisplayName)
			else
				self.m_SelectItemWnd[i]:EnableWnd(false)
				self.m_ItemNameWnd[i]:SetWndText(GetStaticTextClient(8315,DisplayName))
			end
			self.m_ItemNameWnd[i]:ShowWnd(true)
		end
	end
	if self.EndIndex>4 then
		self.m_BtnPageUp:EnableWnd(true)
	else
		self.m_BtnPageUp:EnableWnd(false)
	end
	if table.getn(DropItemTbl)-self.EndIndex>0 then
		self.m_BtnPageDown:EnableWnd(true)
	else
		self.m_BtnPageDown:EnableWnd(false)
	end

	self.m_BtnGetAllItem:EnableWnd(false)
	for i = 1, table.getn(DropItemTbl) do
		if DropItemTbl[i][4] then
			self.m_BtnGetAllItem:EnableWnd(true)
			break
		end
	end
	self:ShowWnd(true)
end

function CPickupItemWnd:ChangePageItem()
	if table.getn(self.m_CanCollItemTbl)-g_GameMain.m_PickupItemWnd.StartIndex>=4 then
		self.EndIndex=g_GameMain.m_PickupItemWnd.StartIndex+3
	else
		self.EndIndex=table.getn(self.m_CanCollItemTbl)
	end
	self:InitDropItemWnd(self.m_CanCollItemTbl)
	self:ShowWnd(true)
end

function CPickupItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	local npcglobalid = self.m_NpcGlobalID
	if(uMsgID == BUTTON_LCLICK) then
		if ( Child == self.m_CloseButton) then
			Gac2Gas:CancelGetDropItems(g_Conn, npcglobalid)
			self.m_CanCollItemTbl={}
			self.m_DeleteIndexNum = 0
			self:ShowWnd(false)
			return
		elseif(Child == self.m_BtnGetAllItem) then
--			print("进行全部采集")
			Gac2Gas:SelectAllDropItems(g_Conn, npcglobalid)
			return
		elseif(Child == g_GameMain.m_PickupItemWnd:GetDlgChild("LastPage")) then
			self.StartIndex=self.StartIndex-4
			self:ChangePageItem()
			return
		elseif(Child == g_GameMain.m_PickupItemWnd:GetDlgChild("NextPage")) then
			self.StartIndex=self.StartIndex+4
			self:ChangePageItem()
			return
		elseif(Child == g_GameMain.m_PickupItemWnd:GetDlgChild("GetItemBtn1")) then
--			print("提取物品1")
			local PannelIndex = self.StartIndex
			--local GetIndex = self.m_CanCollItemTbl[PannelIndex][4]
			--print(GetIndex, "GetIndex")
			Gac2Gas:SelectSingleGridDropItem(g_Conn, PannelIndex, npcglobalid)
			return
		elseif(Child == g_GameMain.m_PickupItemWnd:GetDlgChild("GetItemBtn2")) then
--			print("提取物品2")
			local PannelIndex = self.StartIndex+1
			--local GetIndex = self.m_CanCollItemTbl[PannelIndex][4]
			--print(GetIndex, "GetIndex")
			Gac2Gas:SelectSingleGridDropItem(g_Conn, PannelIndex, npcglobalid)
			return
		elseif(Child == g_GameMain.m_PickupItemWnd:GetDlgChild("GetItemBtn3")) then
--			print("提取物品3")
			local PannelIndex = self.StartIndex+2
			--local GetIndex = self.m_CanCollItemTbl[PannelIndex][4]
			--print(GetIndex, "GetIndex")
			Gac2Gas:SelectSingleGridDropItem(g_Conn, PannelIndex, npcglobalid)
			return
		elseif(Child == g_GameMain.m_PickupItemWnd:GetDlgChild("GetItemBtn4")) then
--			print("提取物品4")
			local PannelIndex = self.StartIndex+3
			--local GetIndex = self.m_CanCollItemTbl[PannelIndex][4]
			--(GetIndex, "GetIndex")
			Gac2Gas:SelectSingleGridDropItem(g_Conn, PannelIndex, npcglobalid)
			return
		else
			return
		end
	end
	return
end

function CPickupItemWnd:RemoveOneGridItem(Grid)
	g_DelWndImage(self.m_SelectItemWnd[Grid], 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_SelectItemWnd[Grid]:ShowWnd(false)
	self.m_ItemNameWnd[Grid]:ShowWnd(false)
	self.m_BtnGetAllItem:EnableWnd(false)
	local DropItemTbl=self.m_CanCollItemTbl
	if table.getn(DropItemTbl)>0 then
		if table.getn(DropItemTbl)-self.StartIndex>=4 then
			self.EndIndex=self.StartIndex+3
		else
			self.EndIndex=table.getn(DropItemTbl)
		end
		self:InitDropItemWnd(DropItemTbl)
	end
end

function Gas2Gac:RetNpcDropItem(Conn,ItemType,ItemName,ItemNum,CanClick)
	table.insert(g_GameMain.m_PickupItemWnd.m_CanCollItemTbl,{ItemType,ItemName,ItemNum,CanClick})
end

function Gas2Gac:RetNpcDropItemEnd(Conn, GlobalID)
	local DropItemTbl=g_GameMain.m_PickupItemWnd.m_CanCollItemTbl
	g_GameMain.m_PickupItemWnd.m_NpcGlobalID = GlobalID
	if g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex 
		or g_GameMain.m_PickupItemWnd.m_AutoPickUpOneDrop then
		if DropItemTbl==nil or table.getn(DropItemTbl)==0 then
			Gac2Gas:CancelGetDropItems(g_Conn, GlobalID)
		else
			Gac2Gas:SelectAllDropItems(g_Conn, GlobalID)
		end
		return		
	end
	if DropItemTbl==nil or table.getn(DropItemTbl)==0 then
		DropItemTbl={}
		g_GameMain.m_PickupItemWnd.StartIndex=1
		g_GameMain.m_PickupItemWnd.EndIndex=1
	else
		g_GameMain.m_PickupItemWnd.StartIndex=1
		if table.getn(DropItemTbl)>4 then
			g_GameMain.m_PickupItemWnd.EndIndex=4
		else
			g_GameMain.m_PickupItemWnd.EndIndex=table.getn(DropItemTbl)
		end
	end
	g_GameMain.m_PickupItemWnd:InitDropItemWnd(DropItemTbl)	
end

function Gas2Gac:NotifyClosePickUpWnd(Conn)
	local index = g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex
	if index then
		table.remove(g_MainPlayer.m_NpcInVisionTbl, index)
		g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex = nil
		--AutoPickUpDropItems()
	end
	g_GameMain.m_PickupItemWnd:ShowWnd(false)
	g_GameMain.m_PickupItemWnd.m_CanCollItemTbl={}
	g_GameMain.m_PickupItemWnd.m_DeleteIndexNum = 0
end	

function Gas2Gac:RetSelectAllDropItems(Conn, Succ)
	local index = g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex
	if index then
		--table.remove(g_MainPlayer.m_NpcInVisionTbl, index)
		g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex = nil
		local npcglobalid = g_GameMain.m_PickupItemWnd.m_NpcGlobalID
		Gac2Gas:CancelGetDropItems(g_Conn, npcglobalid)
		g_GameMain.m_PickupItemWnd.m_CanCollItemTbl={}
		g_GameMain.m_PickupItemWnd.m_DeleteIndexNum = 0
		--AutoPickUpDropItems(index)
	elseif g_GameMain.m_PickupItemWnd.m_AutoPickUpOneDrop then
		local npcglobalid = g_GameMain.m_PickupItemWnd.m_NpcGlobalID
		Gac2Gas:CancelGetDropItems(g_Conn, npcglobalid)
		g_GameMain.m_PickupItemWnd.m_CanCollItemTbl={}
		g_GameMain.m_PickupItemWnd.m_DeleteIndexNum = 0
	end
	if not Succ then
		g_GameMain.m_PickupItemWnd.m_DeleteIndexNum = 0
		return
	end
	g_GameMain.m_PickupItemWnd.m_CanCollItemTbl={}
	g_GameMain.m_PickupItemWnd.m_DeleteIndexNum = 0
	g_GameMain.m_PickupItemWnd:ShowWnd(false)
end

function Gas2Gac:RetGetDropItemGrid(Conn,GetIndex)
	if g_GameMain.m_PickupItemWnd.m_AutoPickUpIndex then
		return
	end
	local removeindex = GetIndex - g_GameMain.m_PickupItemWnd.m_DeleteIndexNum
--	print(GetIndex ,removeindex,  "GetIndex, removeindex")
	table.remove(g_GameMain.m_PickupItemWnd.m_CanCollItemTbl,removeindex)
	if removeindex <= 4 then
		g_GameMain.m_PickupItemWnd:RemoveOneGridItem(removeindex)
	end
	g_GameMain.m_PickupItemWnd.m_DeleteIndexNum = g_GameMain.m_PickupItemWnd.m_DeleteIndexNum + 1
end

function Gas2Gac:RetGetSingleGridDropItem(Conn,PannelIndex)
	local Grid = PannelIndex%4
	if Grid == 0 then
		Grid = 4
	end
--	print(Grid ,PannelIndex,  "Grid, PannelIndex")
	--local removeindex = PannelIndex - g_GameMain.m_PickupItemWnd.m_DeleteIndexNum
	table.remove(g_GameMain.m_PickupItemWnd.m_CanCollItemTbl,PannelIndex)
	g_GameMain.m_PickupItemWnd:RemoveOneGridItem(Grid)
	--[[
	g_DelWndImage(g_GameMain.m_PickupItemWnd.m_SelectItemWnd[Grid], 1, IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_PickupItemWnd.m_SelectItemWnd[Grid]:ShowWnd(false)
	g_GameMain.m_PickupItemWnd.m_ItemNameWnd[Grid]:ShowWnd(false)
	g_GameMain.m_PickupItemWnd.m_BtnGetAllItem:EnableWnd(false)
	local DropItemTbl=g_GameMain.m_PickupItemWnd.m_CanCollItemTbl
	if table.getn(DropItemTbl)>0 then
		if table.getn(DropItemTbl)-g_GameMain.m_PickupItemWnd.StartIndex>=4 then
			g_GameMain.m_PickupItemWnd.EndIndex=g_GameMain.m_PickupItemWnd.StartIndex+3
		else
			g_GameMain.m_PickupItemWnd.EndIndex=table.getn(DropItemTbl)
		end
		g_GameMain.m_PickupItemWnd:InitDropItemWnd(DropItemTbl)
	end
	--]]
end
