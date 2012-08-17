lan_load "liveskill/Lan_DirectionType_Common"
lan_load "liveskill/Lan_LiveSkill_Common"

CLiveSkillCookBaseWnd  = class(SQRDialog)
CLiveSkillMunuListItem = class(SQRDialog)

function CreateLiveSkillCookBaseWnd(parent)
	local wnd = CLiveSkillCookBaseWnd:new()
	wnd:Init(parent)
	wnd:ShowWnd(true)
	return wnd
end

function CLiveSkillCookBaseWnd:Init(Parent)
	self.m_PrescriptionName	= "" --配方名称
	self.m_tblAllMenu		= {{}, {}, {}}
	self.m_ListPage			= 3
	self:CreateFromRes("LiveSkillProduce", Parent)
end

function CLiveSkillCookBaseWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_CancelBtn		= self:GetDlgChild("CancelBtn")
	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_SkillList		= self:GetDlgChild("SkillList")
	self.m_ProdAllBtn		= self:GetDlgChild("ProdAllBtn")
	self.m_NumEdit			= self:GetDlgChild("NumEdit")
	self.m_AddBtn			= self:GetDlgChild("AddBtn")
	self.m_MinusBtn			= self:GetDlgChild("MinusBtn")
	self.m_Description		= self:GetDlgChild("Description")
	self.m_CoolDown			= self:GetDlgChild("CoolDown")
	
	self.m_tblProduct			= {}
	self.m_tblProductName		= {}
	self.m_tblProductPercent	= {}
	for i = 1, 2 do
		self.m_tblProduct[i]		= self:GetDlgChild("Product" .. i)
		self.m_tblProductName[i]	= self:GetDlgChild("ProductName" .. i)
		self.m_tblProductPercent[i]	= self:GetDlgChild("ProductPercent" .. i)
	end
	
	self.m_tblListBtn		= {}
	self.m_tblMaterial		= {}
	self.m_tblMaterialName	= {}
	self.m_tblMaterialCount	= {}
	for i = 1, 3 do
		self.m_tblListBtn[i]		= self:GetDlgChild("ListBtn" .. i)
		self.m_tblMaterial[i]		= self:GetDlgChild("Material" .. i)
		self.m_tblMaterialName[i]	= self:GetDlgChild("MaterialName" .. i)
		self.m_tblMaterialCount[i]	= self:GetDlgChild("MaterialCount" .. i)
	end
	
	self.m_NumEdit:SetMaxTextLenght(2)
	self.m_NumEdit:SetWndText(1)
	self.m_CancelBtn:ShowWnd(false)
end

--消息
function CLiveSkillCookBaseWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:OpenWnd(false)
		elseif(Child == self.m_CancelBtn) then
			Gac2Gas:LiveSkillBreakProducing(g_Conn)
		elseif(Child == self.m_OkBtn) then
			self:ProduceByProductName(false) --按照数量制造
		elseif(Child == self.m_ProdAllBtn) then
			self:ProduceByProductName(true)  --全部制造
		elseif(Child == self.m_AddBtn) then
			local num = tonumber(self.m_NumEdit:GetWndText()) or 0
			num = num < 99 and num + 1 or 99
			self.m_NumEdit:SetWndText(num)
		elseif(Child == self.m_MinusBtn) then
			local num = tonumber(self.m_NumEdit:GetWndText()) or 0
			num = num > 1 and num - 1 or 1
			self.m_NumEdit:SetWndText(num)
		else
			local tbl = self.m_tblListBtn
			for i = 1, #tbl do
				if(Child == tbl[i]) then
					self:ChangeListPage(i, self.m_ListPage)
					break
				end
			end
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		self:OnChangeMenuItem(self.m_tblAllMenu[self.m_ListPage])
	end
end

function CLiveSkillCookBaseWnd:Flush(tblInfo)
	self.m_SkillName	= tblInfo[1]
	self.m_nLevelValue	= tblInfo[2]
	self.m_tblDirection	= {}
	for i, v in ipairs(g_LiveSkillMgr.m_tblDirectionType[self.m_SkillName]) do
		self.m_tblDirection[i] = {}
		self.m_tblDirection[i].sDirection = v
		self.m_tblListBtn[i]:SetWndText( Lan_DirectionType_Common(MemH64(v), "DisplayName") )
	end
end

function CLiveSkillCookBaseWnd:UpdateInfo(tblInfo)
	self:Flush(tblInfo)
	Gac2Gas:GetProductsCanMake(g_Conn, self.m_SkillName)
end

function CLiveSkillCookBaseWnd:SetRemainCDTime(sPrescriptionName, nTime)
	if(sPrescriptionName == self.m_PrescriptionName) then
		local time = self.m_CoolDownTime - nTime
		if(time > self.m_CoolDownTime or time < 0) then
			local sTime = g_CTimeTransMgr:FormatTimeDHM(self.m_CoolDownTime)
			self.m_CoolDown:SetWndText(GetStaticTextClient(3045) .. sTime)
		else
			local sTime = g_CTimeTransMgr:FormatTimeDHM(time)
			self.m_CoolDown:SetWndText(GetStaticTextClient(3046) .. sTime)
		end
	end
end

function CLiveSkillCookBaseWnd:InsertProductNameByDirectionBegin(sExpertName, nExpertLevel)
	for i, v in ipairs(self.m_tblDirection) do
		if("专精" == v.sDirection) then
			v.sExpert	= sExpertName
			v.nLevel	= nExpertLevel
		end
	end
	for i, v in ipairs(self.m_tblAllMenu) do
		local bFlag			= "专精" == self.m_tblDirection[i].sDirection
		local sDirection	= bFlag and self.m_tblDirection[i].sExpert or self.m_tblDirection[i].sDirection
		local nLevel		= bFlag and self.m_tblDirection[i].nLevel or self.m_nLevelValue
		self.m_tblAllMenu[i] = g_LiveSkillMgr:GetProductsCanMake(self.m_SkillName, sDirection, nLevel)
	end
end

function CLiveSkillCookBaseWnd:InsertProductNameByDirection(productName)
	for i, v in ipairs(self.m_tblAllMenu) do
		local tbl = self.m_tblDirection[i]
		local sDirection = "专精" == tbl.sDirection and tbl.sExpert or tbl.sDirection
		local tblProd = g_LiveSkillMgr:GetProductInfoByName(self.m_SkillName, sDirection, productName)
		if(tblProd) then
			local tblTemp	= {}
			tblTemp.name	= productName
			tblTemp.level	= tblProd("SkillLevel")
			table.insert(self.m_tblAllMenu[i], tblTemp)
		end
	end
end

------Panel------
function CLiveSkillCookBaseWnd:ChangeListPage(newPage, oldPage)
	local dis  = newPage - oldPage
	if(0 == dis) then return end
	local sign = dis/math.abs(dis)
	local tbl = self.m_tblListBtn
	local btnHeight = tbl[1]:GetWndOrgHeight()
	self:SetPosRelatively(0, btnHeight*dis, self.m_SkillList)
	for i = newPage, oldPage+sign, -sign do
		self:SetPosRelatively(0, -sign*self.m_SkillList:GetWndOrgHeight(), sign == 1 and tbl[i] or tbl[i-sign])
	end
	self:DrawMenuList(newPage)
end

------重置某窗体的位置(参数是相对于原位置的值)------
function CLiveSkillCookBaseWnd:SetPosRelatively(x, y, wnd)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	local uWidth	= Rect.right	- Rect.left
	local uHeight	= Rect.bottom	- Rect.top
	Rect.left		= Rect.left	+ x
	Rect.top		= Rect.top	+ y
	Rect.right		= Rect.left	+ uWidth
	Rect.bottom		= Rect.top	+ uHeight
	wnd:SetLogicRect(Rect)
end

------选择item事件------
function CLiveSkillCookBaseWnd:OnChangeMenuItem(tblName)
	self:OnClearWnd()
	local itemIndex			= self.m_SkillList:GetSelectItem(-1)+1
	local tbl				= self.m_tblDirection[self.m_ListPage]
	local DirectType		= "专精" == tbl.sDirection and tbl.sExpert or tbl.sDirection
	self.m_PrescriptionName	= tblName[itemIndex].name
	local tblProd			= g_LiveSkillMgr:GetProductInfoByName(self.m_SkillName, DirectType, self.m_PrescriptionName)
	
	if(not tblProd) then
		local str = ""
		for k, v in pairs(self.m_tblDirection) do
			for l, w in pairs(v) do
				str = string.format("%s%d,%s,%s;", str, k, l, w)
			end
		end
		LogErr("生活技能配置表中没有该项或者参数有误", self.m_SkillName .. "," .. DirectType .. "," .. self.m_PrescriptionName .. "," .. self.m_ListPage .. "(" .. str .. ")")
		self.m_PrescriptionName = ""
		return
	end
	
	self.m_CoolDown:SetWndText("")
	local nCD = tblProd("CoolTime")
	if(nCD and ""~= nCD and 0 ~= nCD) then
		self.m_CoolDownTime = nCD
		Gac2Gas:GetLiveSkillProdRemainCDTime(g_Conn, self.m_SkillName, DirectType, self.m_PrescriptionName)
	end
	
	for i = 1, 2 do
		local nItemType		= tblProd("ProductType" .. i)
		if(0 == nItemType) then break end
		local sItemName		= tblProd("ProductName" .. i)
		local nSmallIcon	= g_ItemInfoMgr:GetItemInfo(nItemType, sItemName, "SmallIcon")
		local sDisplayName	= g_ItemInfoMgr:GetItemLanInfo(nItemType, sItemName, "DisplayName")
		local nQuality		= g_ItemInfoMgr:GetItemInfo(nItemType, sItemName, "Quality")
		local sProbability	= string.format("%.2f%%", tblProd("ProductPro" .. i)*100)
		g_LoadIconFromRes(nSmallIcon, self.m_tblProduct[i], -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self.m_tblProduct[i], nItemType, sItemName, 0)
		self.m_tblProductName[i]:SetWndText("#c" ..g_ColorMgr:GetColor("稀".. nQuality) .. sDisplayName)
		self.m_tblProductPercent[i]:SetWndText(sProbability)
	end
	
	self.m_tblMaterialInfo	= g_LiveSkillMgr:GetMaterialsByProduct(self.m_SkillName, DirectType, self.m_PrescriptionName)
	for i, v in ipairs(self.m_tblMaterialInfo) do
		local itemType, itemName, needNum = v[2], v[1], v[3]
		local SmallIcon		= g_ItemInfoMgr:GetItemInfo( itemType, itemName,"SmallIcon" ) --物品类型，物品名称
		local countInBag	= g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, itemType, itemName )
		local sDisplayName	= g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayName")
		local nQuality		= g_ItemInfoMgr:GetItemInfo(itemType, itemName, "Quality")
		g_LoadIconFromRes(SmallIcon, self.m_tblMaterial[i], -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self.m_tblMaterial[i],itemType, itemName, 0)
		self.m_tblMaterialCount[i]:SetWndText( countInBag .. "/" .. needNum )
		self.m_tblMaterialName[i]:SetWndText("#c" ..g_ColorMgr:GetColor("稀".. nQuality) .. sDisplayName)
	end
end

function CLiveSkillCookBaseWnd:UpdateMaterialCountShow(sProductName)
	if( "" == self.m_PrescriptionName ) then return end
	local tblMaterial = self.m_tblMaterialInfo
	for i = 1, #tblMaterial do
		local itemType, itemName, needNum = tblMaterial[i][2], tblMaterial[i][1], tblMaterial[i][3]
		if( itemName == sProductName ) then
			local countInBag = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, itemType, itemName )
			self.m_tblMaterialCount[i]:SetWndText( countInBag .. "/" .. needNum )
		end
	end
end

function CLiveSkillCookBaseWnd:GetItemShortStr()
	local strItemShort = ""
	local tblMaterial = self.m_tblMaterialInfo
	for i = 1, #tblMaterial do
		local itemType, itemName, needNum = tblMaterial[i][2], tblMaterial[i][1], tblMaterial[i][3]
		if( g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, itemType, itemName ) < needNum ) then
			local DisplayName = g_ItemInfoMgr:GetItemLanInfo( itemType, itemName, "DisplayName" )
			strItemShort = strItemShort .. " '" .. DisplayName .. "'"
		end
	end
	return strItemShort
end

function CLiveSkillCookBaseWnd:IsCanProd(nMsgID1, nMsgID2)
	if("" == self.m_PrescriptionName) then
		MsgClient(nMsgID1)
		return false
	end
	local strItemShort = self:GetItemShortStr()
	if( "" ~= strItemShort ) then
		MsgClient(nMsgID2, strItemShort)
		return false
	end
	self:ButtonEnable(false)
	return true
end

------设定该物品格子的可用状态------
function CLiveSkillCookBaseWnd:EnableBagItem(row, col, bFlag)
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc(g_StoreRoomIndex.PlayerBag)
	SrcCt:GetSubItem(row, col):EnableWnd(bFlag)
end

------清除面板------
function CLiveSkillCookBaseWnd:OnClearWnd()
	local tbl = self.m_tblMaterial
	self.m_PrescriptionName = ""
	for i = 1, 2 do
		g_DelWndImage(self.m_tblProduct[i], 1, IP_ENABLE, IP_ENABLE)
		self.m_tblProduct[i]:SetMouseOverDescAfter("")
		self.m_tblProductName[i]:SetWndText("")
		self.m_tblProductPercent[i]:SetWndText("")
	end
	for i = 1, #tbl do
		g_DelWndImage(tbl[i], 1, IP_ENABLE, IP_ENABLE)
		tbl[i]:SetMouseOverDescAfter("")
		self.m_tblMaterialCount[i]:SetWndText("")
		self.m_tblMaterialName[i]:SetWndText("")
	end
end

function CLiveSkillCookBaseWnd:ButtonEnable(bFlag)
	self.m_OkBtn:EnableWnd(bFlag)
	self.m_ProdAllBtn:EnableWnd(bFlag)
	self.m_CancelBtn:EnableWnd(not bFlag)
end

----------------------------------------------------------------------------------------------------------------
function CLiveSkillCookBaseWnd:ProduceByProductName(bAll)
	if(not bAll) then
		local num = tonumber(self.m_NumEdit:GetWndText()) or 0
		if( num <= 0) then return end
	end
	
	if(self.m_SkillName == "制药") then
		if( not self:IsCanProd(9552, 9554) ) then return end
	elseif(self.m_SkillName == "烹饪") then
		if( not self:IsCanProd(9551, 9553) ) then return end
	else
		if( not self:IsCanProd(9561, 9562) ) then return end
	end
	
	local tbl = self.m_tblDirection[self.m_ListPage]
	local DirectType = "专精" == tbl.sDirection and tbl.sExpert or tbl.sDirection
	if(bAll) then
		Gac2Gas:ProduceByProductNameAll(g_Conn, self.m_SkillName, DirectType, self.m_PrescriptionName)
	else
		local num = tonumber(self.m_NumEdit:GetWndText())
		Gac2Gas:ProduceByProductName(g_Conn, self.m_SkillName, DirectType, self.m_PrescriptionName, num )
	end
end

function CLiveSkillCookBaseWnd:GetItemTextColor(nProdLevel)
	if(nProdLevel == self.m_nLevelValue) then
		return 0xffe98e06
	elseif(nProdLevel + 1 == self.m_nLevelValue) then
		return 0xff06b304
	else
		return 0xff808080
	end
end 

----------------------------------------------------------------------------------------------------------------

-------------------------------------------------------------
function CLiveSkillCookBaseWnd:DrawMenuList(menuType)
	self.m_SkillList:DeleteAllItem()
	self.tblMenuItem = {}
	self.m_SkillList:InsertColumn(0, self.m_SkillList:GetWorkAreaOrgWidth())
	local tbl = self.m_tblDirection[menuType]
	local sDirectType = "专精" == tbl.sDirection and tbl.sExpert or tbl.sDirection
	for i, v in ipairs(self.m_tblAllMenu[menuType]) do
		self.m_SkillList:InsertItem(i-1, 30)
		local item		= self.m_SkillList:GetSubItem(i-1, 0)
		local menuItem	= self:CreateMenuListItemWnd(item)
		local nColor	= self:GetItemTextColor(v.level)
		menuItem:SetInfo(self.m_SkillName, sDirectType, nColor, v)
		table.insert(self.tblMenuItem, menuItem)
	end
	self.m_ListPage = menuType
end

------创建列表的行窗体
function CLiveSkillCookBaseWnd:CreateMenuListItemWnd(Parent)
	local Wnd = CLiveSkillMunuListItem:new()
	Wnd:CreateFromRes("LiveSkillProduceListItem", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

--*************************
------list的子窗体类-------
--*************************
function CLiveSkillMunuListItem:SetInfo(sSkillName, sDirectType, nColor, tblNameAndLevel)
	local disIndex		= MemH64( string.format("%s%s%s", sSkillName, sDirectType, tblNameAndLevel.name) )
	self:SetWndTextColor(IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE), nColor)
	self:SetMenuText(Lan_LiveSkill_Common(disIndex, "DisplayName"))
end

function CLiveSkillMunuListItem:SetMenuText(sName)
	self:SetWndText(sName)
end
--*************************
---------------------------
--*************************


---------------------------------------------------------------

function Gas2Gac:ReturnGetProductsCanMakeBegin(Conn, sExpertName, nExpertLevel)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction:InsertProductNameByDirectionBegin(sExpertName, nExpertLevel) --sExpertName, nExpertLevel)
end

function Gas2Gac:ReturnGetProductsCanMake(Conn, productName)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction:InsertProductNameByDirection(productName)
end

function Gas2Gac:ReturnLearnNewDirection(Conn,sSkillName, sDirection, sPropDirection)
	local disIndex	= MemH64( string.format("%s%s%s",sSkillName, sDirection, sPropDirection) )
	sDisplayName	= Lan_LiveSkill_Common(disIndex, "DisplayName")
	MsgClient(9531,sDisplayName)
end

function Gas2Gac:ReturnGetProductsCanMakeEnd(Conn)
	local wnd = g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction
	for i, v in ipairs(wnd.m_tblAllMenu) do
		table.sort( v, function(a, b) return a.level > b.level end )
	end
	wnd:DrawMenuList(wnd.m_ListPage)
end

------返回按照数量制造结果------
function Gas2Gac:ReturnProduceByProductName(Conn, bFlag)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction:ButtonEnable(true)
end

function Gas2Gac:ReturnGetLiveSkillProdRemainCDTime(Conn, sPrescriptionName, nTime)
	g_GameMain.m_LiveSkillProdMain.m_LiveSkillProduction:SetRemainCDTime(sPrescriptionName, nTime)
end
