CSmallMapSelectShowWnd = class ( SQRDialog )

--------------小地图筛选功能窗口
cfg_load "smallmap/SmallMapIcon_Common"
lan_load "smallmap/Lan_SmallMapIcon_Common"
local NpcAndPlayerStrInfo = {}
local function InitNpcAndPlayerStrInfo()
    local keys = SmallMapIcon_Common:GetKeys()
	for k, v in ipairs(keys) do
		local sDisplayName = Lan_SmallMapIcon_Common(v,"DisplayName")
		local TexPath = SmallMapIcon_Common(v, "TexPath")
		local RoleType = SmallMapIcon_Common(v, "RoleType")
		NpcAndPlayerStrInfo[k] = {sDisplayName, TexPath, RoleType}
	end
end
InitNpcAndPlayerStrInfo()

function CSmallMapSelectShowWnd:Ctor(parent)
	self:CreateFromRes("SmallMapSelectShow", parent)
	self:SetIsBottom(true)
	self.m_SelectAll = self:GetDlgChild("SelectAll")
	self.m_SelectNone = self:GetDlgChild("SelectNone")
	local ListWnd  = self:GetDlgChild("SelectShowWnd")
	self.ItemListWnd = ListWnd
	local itemWidth = ListWnd:GetWndWidth()
	ListWnd:InsertColumn( 0, itemWidth)
	self.ChkBtnTbl = {}
	self.ItemListTbl = {}
	self.ChoosedItemTbl = {}
	self.ListTbl = {}
	local curSelectTypeNum = # NpcAndPlayerStrInfo
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	for i=1, curSelectTypeNum do
		ListWnd:InsertItem(i-1, 26)
		local item  = ListWnd:GetSubItem(i-1, 0)
		local itemWnd = SQRDialog:new() 
		itemWnd:CreateFromRes("SmallMapSelectShowItem", item)
		itemWnd:ShowWnd( true )
		table.insert(self.ItemListTbl, item)
		local chkBtn = itemWnd:GetDlgChild("ChkBtn")
		table.insert(self.ChkBtnTbl, chkBtn)
		local showNpcAndPlayerStrWnd = itemWnd:GetDlgChild("NpcOrPlayerType")
		showNpcAndPlayerStrWnd:SetWndText(NpcAndPlayerStrInfo[i][1])
		local imageWnd = itemWnd:GetDlgChild("Image")
		local pos = CFPos:new(0, 0)	
		local DefaultImage = WND_IMAGE_LIST:new()
		DefaultImage:AddImage( imageWnd:GetGraphic(), -1,  NpcAndPlayerStrInfo[i][2], nil, pos, "0xffffffff", 0 )
		imageWnd:SetWndBkImage( Flag, DefaultImage )
		table.insert(self.ListTbl, itemWnd)
	end

	self:InitSmallMapSelectShowData()
	g_ExcludeWndMgr:InitExcludeWnd(self, "非互斥")
end

function CSmallMapSelectShowWnd:InitSmallMapSelectShowData()
	local filePath = g_RootPath .. "var/gac/SmallMapSelectShow.txt"
	local file=CLuaIO_Open(filePath,"r")
	local curSelectTypeNum = # NpcAndPlayerStrInfo
	if file ~= nil then
		for i=1, curSelectTypeNum do	--总共有11种可选择类型
			local text = file:ReadLine()
			local boolValue = false
			if text == "true" then
				boolValue = tostring(true)
				local roleType = NpcAndPlayerStrInfo[i][3]
				g_GameMain.m_SmallMapBG.m_Static:AddShowRoleType(roleType)
			end
			self.ChkBtnTbl[i]:SetCheck(boolValue)
			table.insert(self.ChoosedItemTbl, boolValue)
		end
		
		file:Close()
	end
end

function CSmallMapSelectShowWnd:ResetSmallMapSelectShowData(choosedItem, selectOrNot)
	local filePath = g_RootPath .. "var/gac/SmallMapSelectShow.txt"
	local file=CLuaIO_Open(filePath,"r")
	local newData = nil
	local curSelectTypeNum = # NpcAndPlayerStrInfo
	if file ~= nil then
		for i=1, curSelectTypeNum do
			local text = file:ReadLine()
			if choosedItem == i then			
				text = tostring(selectOrNot)
			end
			if newData == nil then
				newData = text
			else
				if text ~= nil then
					newData = newData .. "\n" .. text
				end
			end
		end
		local writeData = string.format("%s", newData)
		local writeFile = CLuaIO_Open(filePath, "w")
		writeFile:WriteString(writeData)
		file:Close()
		writeFile:Close()
	end
end

function CSmallMapSelectShowWnd:ResetAllSmallMapSelectShowData(selectOrNot)
	local filePath = g_RootPath .. "var/gac/SmallMapSelectShow.txt"
	local curSelectTypeNum = # NpcAndPlayerStrInfo
	local newData = nil
	local setData = tostring(selectOrNot)
	for i =1, curSelectTypeNum do 
		if newData == nil then
			newData = tostring(selectOrNot)
		else
			newData = newData .. "\n" .. tostring(selectOrNot)
		end
		self.ChkBtnTbl[i]:SetCheck(selectOrNot)
		local roleType = NpcAndPlayerStrInfo[i][3]
		if selectOrNot == true then
			g_GameMain.m_SmallMapBG.m_Static:AddShowRoleType(roleType)
		else
			g_GameMain.m_SmallMapBG.m_Static:RemoveShowRoleType(roleType)
		end
	end
	local writeData = string.format("%s", newData)
	local writeFile = CLuaIO_Open(filePath, "w")
	if writeFile ~= nil then
		writeFile:WriteString(writeData)
		writeFile:Close()
	end
end

function CSmallMapSelectShowWnd:GetWhichTypeClicked()
	local selectShowWnd = g_GameMain.m_NavigationMap.m_SelectShowWnd
	local choosedItem = selectShowWnd.ItemListWnd:GetSelectItem( -1 ) + 1
	if choosedItem == 0 then
		return nil
	end
	local curSelectState = selectShowWnd.ChoosedItemTbl[choosedItem]
	local selectOrNot = not curSelectState
	selectShowWnd.ChkBtnTbl[choosedItem]:SetCheck(selectOrNot)
	selectShowWnd.ChoosedItemTbl[choosedItem] = selectOrNot
	self:ResetSmallMapSelectShowData(choosedItem, selectOrNot)
	return curSelectState, NpcAndPlayerStrInfo[choosedItem][3]
end

function CSmallMapSelectShowWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )	
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_SelectAll then
			self:ResetAllSmallMapSelectShowData(true)
		elseif Child == self.m_SelectNone then
			self:ResetAllSmallMapSelectShowData(false)
		end
		
	elseif uMsgID == ITEM_LBUTTONCLICK then
		local choosedOrNot, roleType = self:GetWhichTypeClicked()
		if choosedOrNot == nil then
			return
		end
		if choosedOrNot == false then
			g_GameMain.m_SmallMapBG.m_Static:AddShowRoleType(roleType)
		else
			g_GameMain.m_SmallMapBG.m_Static:RemoveShowRoleType(roleType)
		end
	end
end