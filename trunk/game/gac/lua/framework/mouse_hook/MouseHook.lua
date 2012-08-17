gac_require "framework/mouse_hook/MouseHookInc"
cfg_load "mouse_hook/designer_Mouse_Common"
EIconType =
{
	eIT_Prop = 0,
	eIT_FightSkill = 1,
}

-- 光标状态
ECursorState =
{
	eCS_Normal					= 0,
	eCS_PickupItem				= 1, --从物品栏拾取物品
	eCS_PickupItemShortcut		= 2, --从快捷栏拾取物品 
	eCS_PickupSkill				= 3, --从技能栏获得技能
	eCS_PickupEquip				= 4, --从装备面板获得装备
	eCS_PickupItemFromNPCShop	= 5, --从NPC商店拾取物品
	eCS_PickupItemFromTongDepot	= 6, --从帮会仓库拾取物品
	eCS_PickupBattleArrayIndex	= 7, --拖拽起阵法位置Index
	eCS_PickupBattleArrayMember	= 8, --在列阵中拖拽起小队成员
	eCS_PickupBattleArrayIcon	= 9, --拖拽起阵法图标
	eCS_PickupSplitItem			= 10, --物品拆分窗口出现，点击其确定按钮后的鼠标状态
 	eCS_EquipIdentify			= 11, --装备鉴定
 	eCS_ArmorPieceEnactment		= 12, --护甲片设定
 	eCS_EquipSmeltSoul			= 13, --装备炼化魂
 	eCS_RenewEquip				= 14, --修复装备
 	eCS_BreakItem				= 15, --分解物品
 	eCS_EquipIntenBack			= 16, --使用大重铸石
}

ECursorSkillState =
{
	eCSS_Normal					= 0,
	eCSS_Split 					= 1,
	eCSS_SelectGround 			= 2,
	eCSS_PK 					= 3,
	eCSS_PickUp					= 4, --拾取
	eCSS_Talk					= 5, --和任务npc对话
	eCSS_Buy					= 6, --从商店买物品
	eCSS_Sell					= 7, --卖物品给npcshop
	eCSS_Obstacle				= 8, --障碍
	eCSS_UseSkill				= 9, --使用技能
	eCSS_EquipIdentify			= 10, --装备鉴定
	eCSS_ArmorPieceEnactment	= 11, --护甲片设定
	eCSS_EquipSmeltSoul			= 12,
	eCSS_RenewEquip				=13,
	eCSS_BreakItem				= 14, --分解物品
	eCSS_EquipIntenBack			= 16, --使用大重铸石
}
local tblCursorFile = {}

--技能状态对应的鼠标ICON，能改鼠标图标的都是技能
tblCursorFile[ECursorSkillState.eCSS_Normal]				= designer_Mouse_Common("Normal","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_Split]					= designer_Mouse_Common("Split","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_SelectGround]			= designer_Mouse_Common("SelectGround","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_PK]					= designer_Mouse_Common("PK","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_PickUp]				= designer_Mouse_Common("PickUp","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_Talk]					= designer_Mouse_Common("Talk","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_Obstacle]				= designer_Mouse_Common("Obstacle","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_UseSkill]				= designer_Mouse_Common("UseSkill","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_EquipIdentify]			= designer_Mouse_Common("EquipIdentify","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_ArmorPieceEnactment]	= designer_Mouse_Common("ArmorPieceEnactment","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_EquipSmeltSoul]		= designer_Mouse_Common("EquipSmeltSoul","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_RenewEquip]			= designer_Mouse_Common("RenewEquip","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_BreakItem]				= designer_Mouse_Common("BreakItem","ImageIcon")
tblCursorFile[ECursorSkillState.eCSS_EquipIntenBack]		= designer_Mouse_Common("EquipIntenBack", "ImageIcon")
function CreateMouseHook()
	local hook = CMouseHook:new()
	local param = WndCreateParam:new()
	param.uStyle = 0x60800000
	param.width = 32
	param.height = 32
	param:SetParentWnd(g_App:GetRootWnd())
	param.font_size = 12
	hook:Create( param )
	hook:GetWndBkImage( IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE ) ):Release()
	hook.bCursorState = false		--鼠标指针锁定状态，true为锁定，false为没锁定
	hook:ShowWnd( false )
	hook:ResetCursorState()
	hook:ResetSkillState()
	return hook
end

function CMouseHook:OnHook( Wnd, Msg, wParam, lParam )
	
	self.m_MouseSelWnd = Wnd
	if Msg == WM_MOUSEMOVE then
		local posCursor = CFPos:new()
		g_App:GetRootWnd():GetLogicCursorPos(posCursor)
		local x, y = posCursor.x, posCursor.y
		local narrowEdge, wideEdge = 5, 27;
		local rect = CFRect:new( x - narrowEdge , y - narrowEdge, x + wideEdge, y + wideEdge )
		self:SetLogicRect(rect)
	elseif Msg == WM_MOUSELEAVE then
		if g_GameMain then
			g_GameMain:OnMouseLeave()
		end
	elseif Msg == WM_LBUTTONUP then
		if g_GameMain then
			g_GameMain:OnMLButtonUpAnyCase()
		end
	elseif Msg == WM_RBUTTONUP then
		if g_GameMain then
			g_GameMain:OnMRButtonUpAnyCase()
		end
	end
	return true
end

--获得物品的大类ID、小类ID和全局ID
function CMouseHook:GetItemBigSmallGlobalID(room, row, col)
	assert(room and row and col)
	local Mgr = g_GameMain.m_BagSpaceMgr
	local Lc = Mgr:GetSpaceRelateLc(room)
	local CurGrid = Lc:GetGridByIndex(row,col,room)
	local uBigID, uIndex = CurGrid:GetType()
	local nItemID = CurGrid:GetItem():GetItemID()
	return uBigID, uIndex, nItemID
end

function CMouseHook:SetCursorState( eState, CurContext )
	self.m_eCS = eState
	self.m_Context = CurContext
end

function CMouseHook:GetCursorState()
	return self.m_eCS, self.m_Context
end

function CMouseHook:LockCursor()
	self:UpdateCursor()
	self.bCursorState = true
end

function CMouseHook:ReleaseCursorLock()
	self:UpdateCursor()
	self.bCursorState = false
end

--锁定鼠标指针到某种状态
function CMouseHook:LockCursorToState(eState)
	self:SetCursorSkillState(eState)
	self:LockCursor()
end

--释放指针锁并刷新指针到某状态
function CMouseHook:ReleaseCursorLockAndUpate(eState)
	self:ReleaseCursorLock()
	self:SetCursorSkillState(eState)
	self:ReleaseCursorLock()
end

function CMouseHook:SetCursorSkillState(eState, CurContext)
	if self.bCursorState then
		return
	end
	self.m_eCSS = eState
	self.m_CSS_Context = CurContext
	self:SetGlobCursor( tblCursorFile[self.m_eCSS] )
end
function CMouseHook:SetWndCursorSkillState(wnd,eState, CurContext)
	if self.bCursorState then
		return
	end
	self.m_eCSS = eState
	self.m_CSS_Context = CurContext
	wnd:SetWndCursor( tblCursorFile[self.m_eCSS] )
end

function CMouseHook:GetCursorSkillState()
	return self.m_eCSS, self.m_CSS_Context
end

function CMouseHook:ResetCursorState()
	self:SetCursorState( ECursorState.eCS_Normal )
end

function CMouseHook:ResetSkillState()
	self:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
end

function CMouseHook:ClearCursorSpecialState()
	self:ReleaseCursorLock()
	self:SetCursorSkillState( ECursorSkillState.eCSS_Normal )
	self:ClearCursorAll()
end

function CMouseHook:ExChangeIconWnd( IconWnd )
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	local listIconWndImage = {}
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		listIconWndImage[i] = WND_IMAGE_LIST:new()
		if WndImage ~= nil then
			listIconWndImage[i]:AddImageFromImageList( WndImage, 0, -1 )
			WndImage:Release();
			if( self.m_listIconWndImage and self.m_listIconWndImage[i] )then
				WndImage:AddImageFromImageList( self.m_listIconWndImage[i], 0, -1 )
			end
		end
	end
	
	IP.StateMask = IP_ENABLE
	local MouseImage = self:GetWndBkImage( IP );
	MouseImage:Release()
	MouseImage:AddImageFromImageList( listIconWndImage[IP.StateMask], 0, -1 )	
	self:ShowWnd( MouseImage:GetImageCount() > 0 )
	self.m_listIconWndImage = listIconWndImage	
	self.m_IconWnd = IconWnd;
end

function CMouseHook:ExChangeTwoWndIcon( SrcWnd, DescWnd )
	--print(SrcWnd)
	--print(DescWnd)
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	
	local listIconWndImageSrc = {}
	local listIconWndImageDesc = {}
	
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImageSrc = SrcWnd:GetWndBkImage( IP )
		local WndImageDesc = DescWnd:GetWndBkImage( IP )
		
		listIconWndImageSrc[i] = WND_IMAGE_LIST:new()
		if WndImageSrc ~= nil then
			listIconWndImageSrc[i]:AddImageFromImageList( WndImageSrc, 0, -1 )
			WndImageSrc:Release()
		end
		
		listIconWndImageDesc[i] = WND_IMAGE_LIST:new()
		if  WndImageDesc ~=nil then
			listIconWndImageDesc[i]:AddImageFromImageList( WndImageDesc, 0, -1 )
			WndImageDesc:Release()
		end
		
		WndImageSrc:AddImageFromImageList( listIconWndImageDesc[i], 0, -1 )
		WndImageDesc:AddImageFromImageList( listIconWndImageSrc[i], 0, -1 )
	end
end

function CMouseHook:CopyIconFromWnd( IconWnd, WndState )
	
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	local listIconWndImage = {}
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		listIconWndImage[i] = WND_IMAGE_LIST:new()
		if WndImage ~= nil then
			listIconWndImage[i]:AddImageFromImageList( WndImage, 0, -1 )
		end
	end
	
	IP.StateMask = IP_ENABLE
	local MouseImage = self:GetWndBkImage( IP );
	MouseImage:Release()
	local CopyState = WndState or IP_ENABLE
	MouseImage:AddImageFromImageList( listIconWndImage[CopyState], 0, -1 )
	local value = MouseImage:GetImageCount()
	self:ShowWnd( MouseImage:GetImageCount() > 0 )
	self.m_listIconWndImage = listIconWndImage
	self.m_IconWnd = IconWnd
end

--鼠标上的物品是从物品栏拾取的
function CMouseHook:CopyItemIconToWnd( IconWnd )
 	if(self.m_listIconWndImage == nil) then
 		return
 	end
 	
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		if( WndImage and self.m_listIconWndImage[i] )then
			WndImage:AddImageFromImageList( self.m_listIconWndImage[i], 0, -1 )
		end
	end
end

--鼠标上的物品是从快捷栏拾取的
function CMouseHook:CopyItemIconShortcutToWnd( IconWnd )
 	if(self.m_listIconWndImage == nil) then
 		return
 	end
 	
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		if( WndImage and self.m_listIconWndImage[i] )then
			WndImage:AddImageFromImageList( self.m_listIconWndImage[i], 0, -1 )
		end
	end
	
	local state, context = self:GetCursorState()
	return context[1],context[2],context[3],context[4]
end

--函数贴图
function CMouseHook:CopySkillIconToWnd( IconWnd )
 	if(self.m_listIconWndImage == nil) then
 		return
 	end
 	
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		if( WndImage and self.m_listIconWndImage[i] )then
			WndImage:AddImageFromImageList( self.m_listIconWndImage[i], 0, -1 )
		end
	end
	
	--fs
	local state, context = self:GetCursorState()
	return context[1]
end

function CMouseHook:ClearIconWnd( IconWnd )
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE );
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		local WndImage = IconWnd:GetWndBkImage( IP )
		if WndImage ~= nil then
			WndImage:Release();
		end
	end
end

--手上的物品是从NPC商店获取的
function CMouseHook:SetPickupItemfromNPCShop(itemType,itemName, itemCount, iconWnd, WndState, payMoneyType)
  assert(itemType and itemName and itemCount)
  local PickUpItem = {itemType, itemName, itemCount, payMoneyType}
  self:SetCursorState(ECursorState.eCS_PickupItemFromNPCShop, PickUpItem) 
  self:CopyIconFromWnd(iconWnd, WndState)
end

--手上的物品是要拆分出来的
function CMouseHook:SetPickupSplitItem(room, row, col, num, iconWnd, WndState)
	assert(room and row and col and iconWnd)
	
	local PickUpItem = {room, row, col, num}
	self:SetCursorState(ECursorState.eCS_PickupSplitItem, PickUpItem)
	self:CopyIconFromWnd(iconWnd, WndState)
end

function CMouseHook:SetPickupItem(room, row, col, num, iconWnd, WndState, bagListItemWnd)
	assert(room and row and col and iconWnd)
	
	local PickUpItem = {room, row, col, num, iconWnd, bagListItemWnd}
	self:SetCursorState(ECursorState.eCS_PickupItem, PickUpItem)
	self:CopyIconFromWnd(iconWnd, WndState)
end

--从集体仓库拾取物品
function CMouseHook:SetPickupItemFromCollectiveBag(room, row, col, num, iconWnd, WndState, page)
	assert(row and col and iconWnd)
	local tbl = {room, row, col, num, page}
	self:SetCursorState(WndState, tbl)
	self:CopyIconFromWnd(iconWnd, nil)
end

function CMouseHook:SetPickupItemShortcut(uBigID, uIndex, uGlobalID, num, iconWnd, WndState)
	assert(uBigID and uIndex and uGlobalID and num and iconWnd)
	local PickUpItem = {uBigID, uIndex, uGlobalID, num}
	self:SetCursorState(ECursorState.eCS_PickupItemShortcut, PickUpItem)
	self:CopyIconFromWnd(iconWnd, WndState)
end

function CMouseHook:SetPickupSkill(fs, iconWnd, WndState)
	assert(fs and iconWnd)
	local PickUpSkill = {fs}
	self:SetCursorState(ECursorState.eCS_PickupSkill, PickUpSkill)
	self:CopyIconFromWnd(iconWnd, WndState)
end

function CMouseHook:SetPickupEquip(Item, EquipPart, iconWnd, WndState)
	assert(Item and iconWnd and EquipPart)
	local PickUpEquip = {Item,EquipPart,iconWnd} --窗体拿来设置为disable 或者 enable
	--iconWnd:SetMouseOverDescAfter("")
	self:SetCursorState(ECursorState.eCS_PickupEquip, PickUpEquip)
	self:CopyIconFromWnd(iconWnd, WndState)
end

--拖拽阵法位置
function CMouseHook:SetPickupBattleArrayIndex(Index, from, iconWnd, WndState)
	assert(Index and iconWnd)
	local tblPickUpIndex = {Index, from, iconWnd}
	self:SetCursorState(ECursorState.eCS_PickupBattleArrayIndex, tblPickUpIndex)
	self:CopyIconFromWnd(iconWnd, WndState)
end

--拖拽列阵成员
function CMouseHook:SetPickupBattleArrayMember(Index, from, Id, Name, iconWnd, WndState)
	assert(Id and Index and iconWnd)
	local tblPickUpMem = {Index, from, Id, Name, iconWnd}
	self:SetCursorState(ECursorState.eCS_PickupBattleArrayMember, tblPickUpMem)
	self:CopyIconFromWnd(iconWnd, WndState)
end

--拖拽阵法图标
function CMouseHook:SetPickupBattleArrayIcon(InfoTbl, iconWnd, WndState)
	assert(InfoTbl and iconWnd)
	local tblPickUpIcon = {InfoTbl}
	self:SetCursorState(ECursorState.eCS_PickupBattleArrayIcon, tblPickUpIcon)
	self:CopyIconFromWnd(iconWnd, WndState)
end

--准备使用装备鉴定卷轴
function CMouseHook:SetPreIdentifyEquip(InfoTbl)
	--设置鼠标状态
	self:SetCursorState(ECursorState.eCS_EquipIdentify, InfoTbl)
	--设置鼠标形状为装备鉴定形状
	self:SetCursorSkillState(ECursorSkillState.eCSS_EquipIdentify, InfoTbl)
	self:LockCursor()
end

function CMouseHook:SetUseEquipIntenBackItem(InfoTbl)
	--设置鼠标状态
	self:SetCursorState(ECursorState.eCS_EquipIntenBack, InfoTbl)
	--设置鼠标形状为装备鉴定形状
	self:SetCursorSkillState(ECursorSkillState.eCSS_EquipIntenBack, InfoTbl)
	self:LockCursor()
end

function CMouseHook:SetBreakItemState(InfoTbl)
	self:SetCursorState(ECursorState.eCS_BreakItem, InfoTbl)
	self:SetCursorSkillState(ECursorSkillState.eCSS_BreakItem, InfoTbl)
	self:LockCursor()
end

--准备使用护甲片设定
function CMouseHook:SetPreArmorPieceEnactment(InfoTbl)
	--设置鼠标状态
	self:SetCursorState(ECursorState.eCS_ArmorPieceEnactment, InfoTbl)
	--设置鼠标形状为护甲片设定形状
	self:SetCursorSkillState(ECursorSkillState.eCSS_ArmorPieceEnactment, InfoTbl)
	self:LockCursor()
end

--准备使用装备炼化魂卷轴
function CMouseHook:SetPreEquipSmeltSoul(InfoTbl)
	--设置鼠标状态
	self:SetCursorState(ECursorState.eCS_EquipSmeltSoul, InfoTbl)
	--设置鼠标形状为装备炼化魂形状
	self:SetCursorSkillState(ECursorSkillState.eCSS_EquipSmeltSoul, InfoTbl)
	self:LockCursor()
end

--修复装备
function CMouseHook:SetRenewEquip(InfoTbl)
	self:SetCursorState(ECursorState.eCS_RenewEquip, InfoTbl)
	self:SetCursorSkillState(ECursorSkillState.eCSS_RenewEquip, InfoTbl)
	self:LockCursor()
end

function CMouseHook:Clear()
	self:GetWndBkImage( IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE ) ):Release()
	self.m_listIconWndImage = nil	
	self.m_Context = nil
	self.m_ContextType = nil
	self.m_IconWnd = nil
end

function CMouseHook:ClearCursorAll()
	self:Clear()
	self:ResetCursorState()
	self:UpdateCursor()
end

function CMouseHook:GetContext()
	return self.m_Context, self.m_ContextType
end

function CMouseHook:UpdateCursor()
	self:UpdateGlobCursor(self)
end

function CMouseHook:CreateSuitableIconForWnd( IconWnd, EnableImage, MouseOverImage, DisableImage, ClickDownImage,
																							 ucEnableImage, ucMouseOverImage, ucDisableImage, ucClickDownImage)
	local listIconWndImage = {}
	listIconWndImage[IP_ENABLE]		= EnableImage
	listIconWndImage[IP_DISABLE]		= MouseOverImage or EnableImage
	listIconWndImage[IP_MOUSEOVER]	= DisableImage	 or EnableImage
	listIconWndImage[IP_CLICKDOWN]	= ClickDownImage or EnableImage
	
	listIconWndImage[IP_UNCHECK_ENABLE]		= ucEnableImage or EnableImage
	listIconWndImage[IP_UNCHECK_DISABLE]		= ucMouseOverImage or EnableImage
	listIconWndImage[IP_UNCHECK_MOUSEOVER]	= ucDisableImage	 or EnableImage
	listIconWndImage[IP_UNCHECK_CLICKDOWN]	= ucClickDownImage or EnableImage
	
	local IP = IMAGE_PARAM:new( SM_BS_BK, IP_ENABLE )
	for i = IP_ENABLE, IP_UNCHECK_CLICKDOWN do
		IP.StateMask = i
		IconWnd:SetWndBkImage( IP, listIconWndImage[i] )
	end
end

function CMouseHook:RegisterKey()
	self:RegisterAccelKeyCommand( false, false, true, VK_F4, 100, 100 )--ALT+F4关闭游戏
end

function CMouseHook:UnRegisterKey()
	self:UnRegisterAccelKeyCommand( false, false, true, VK_F4)
end

function CMouseHook:OnCommand( wParam, lParam )
	if wParam == 100 then
		if (g_GameMain) then
			g_GameMain:ExitToState( EGameState.eExitGame )
		elseif (g_Conn and g_Conn.ShutDown) then
			g_Conn:ShutDown()
			coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
		else
			coroutine.resume( g_App.MainRoutine, EGameState.eExitGame )
		end
	end
end

function CMouseHook:CheckSelWnd( Wnd)
	if self.m_MouseSelWnd ~= Wnd then
		return true
	end
	return false
end
