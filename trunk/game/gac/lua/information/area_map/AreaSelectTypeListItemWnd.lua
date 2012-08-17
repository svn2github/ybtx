CAreaSelectTypeListItemWnd = class(SQRDialog)

--CreaetMenuItem
function CAreaSelectTypeListItemWnd:Ctor(Parent,str,itemwidth,WndType)
	self:CreateFromRes("NewAreaSelectItemWnd",Parent)
	self.m_SelectBtn = self:GetDlgChild("SelectBtn")
	self.m_TypeNameText = self:GetDlgChild("TypeNameText")
	self.m_TypeNameText:SetWndText(str)
	self.m_WndType = WndType
end

--function CreateAreaSelectTypeListItem(Parent,str,itemwidth,WndType,Index)
--	local Wnd = CreaetMenuItem(Parent,str,itemwidth,WndType)
--	Wnd:ShowWnd(true)
--	
--	-- 创建子菜单
--	Wnd.m_SubMenuWnd = CreateSubMenu(Wnd, itemwidth, Index)
--	return Wnd
--end

local function SaveAreaSelect()
--print(">>>>>>>SaveAreaSelect")
	local PlayerID = g_MainPlayer.m_uID
	local AreaSelect = ""
	local SubMenuSelect = ""
	for i = 0, #(g_GameMain.m_AreaInfoWnd.NpcSelectTypeTbl) do
		if g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[g_GameMain.m_AreaInfoWnd.NpcSelectTypeTbl[i]] then
			AreaSelect = AreaSelect.."1"
		else
			AreaSelect = AreaSelect.."0"			
		end
		
		for _, MenuName in pairs(g_GameMain.m_AreaInfoWnd.NpcSubMenu[i]) do
			if g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[MenuName] then
				AreaSelect = AreaSelect.."y"
			else
				AreaSelect = AreaSelect.."n"
			end
		end	
		
	end
	--AreaSelect = AreaSelect..SubMenuSelect
	
	local strpath = g_RootPath .. "var/gac/AreaSelect"..PlayerID..".txt"
	--print("strpath"..strpath)
	local fo = assert(CLuaIO_Open(strpath, "w+"))
	local data = fo:WriteString("")
	fo:Close()
	local f = assert(CLuaIO_Open(strpath, "a"))
	f:WriteString(AreaSelect)
	--print("SaveAreaSelect>>>>AreaSelect"..AreaSelect)
	f:Close()
end

local CurrentItemText = ""
function CAreaSelectTypeListItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	--if uMsgID == BUTTON_LCLICK  then
	if uMsgID == BUTTON_MOUSEMOVE then
		local TypeName = self.m_TypeNameText:GetWndText()

		local Popup = nil
		if CurrentItemText ~= TypeName and self.m_WndType == "NpcSelect" then
			CurrentItemText = TypeName
			Popup = true
		else
			Popup = false			
		end
		
		if Child == self.m_TypeNameText then
			
			-- 是否有子菜单
			if self.m_SubMenuWnd and Popup then
				if not self.m_SubMenuWnd:IsShow() then		--显示子菜单
					-- 关掉以前的
					if g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_SelectWnd then
						g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_SelectWnd:ShowWnd(false)
					end
					self.m_SubMenuWnd:ShowWnd(true)	
					self.m_SubMenuWnd:SetFocus()
					
					g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_SelectWnd = self.m_SubMenuWnd
				else																			-- 隐藏子菜单
					self.m_SubMenuWnd:ShowWnd(false)															
					g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_SelectWnd = nil
				end
			end
		end
	end
	if uMsgID == BUTTON_LCLICK then
		local TypeName = self.m_TypeNameText:GetWndText()
		if Child == self.m_SelectBtn then
			if self.m_WndType == "NpcSelect" then 
				if not self.m_SelectBtn:GetCheck() then
					local SelectAll = Lan_NpcClassify_Common(0, "TypeName")			-- 标示所有
					if TypeName == SelectAll then
						-- 所有菜单取消选中
						for _, Wnd in pairs(g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_ItemTbl) do
							Wnd.m_SelectBtn:SetCheck(true)
							for _, SubMenuItem in pairs(Wnd.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl) do
								SubMenuItem.m_SelectBtn:SetCheck(true)
							end
						end
						
						g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl = {}
					else
						g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_ItemTbl[SelectAll].m_SelectBtn:SetCheck(false)
						g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[SelectAll] = false
					end
					self.m_SelectBtn:SetCheck(true)
					g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[TypeName] = true	
					
					-- 子菜单的按钮全部选中
					for _, SubMenuItem in pairs(self.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl) do
						SubMenuItem.m_SelectBtn:SetCheck(true)
						local MenuName = SubMenuItem.m_TypeNameText:GetWndText()
						g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[MenuName] = true
					end
					
				else
					self.m_SelectBtn:SetCheck(false)
					g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[TypeName] = false
					for _, Wnd in pairs(g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_ItemTbl) do
						Wnd.m_SelectBtn:SetCheck(false)
						for _, SubMenuItem in pairs(Wnd.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl) do
							SubMenuItem.m_SelectBtn:SetCheck(false)
						end
					end
					-- 子菜单的按钮全部取消选中
					for _, SubMenuItem in pairs(self.m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl) do
						SubMenuItem.m_SelectBtn:SetCheck(false)
						local MenuName = SubMenuItem.m_TypeNameText:GetWndText()
						g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[MenuName] = false
					end
					
				end
				if g_GameMain.m_AreaInfoWnd.m_SceneName == nil then
					return
				end
				SaveAreaSelect()
				g_GameMain.m_AreaInfoWnd:RemoveAllNpcPointBtn(g_GameMain.m_AreaInfoWnd.m_SceneName)
				g_GameMain.m_AreaInfoWnd:DisplayAllCharPointBtn(g_GameMain.m_AreaInfoWnd.m_SceneName)
				self:SetFocus()
			elseif self.m_WndType == "SubMenu" then 		-- 子菜单
				if not self.m_SelectBtn:GetCheck() then
					g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[TypeName] = true
					self.m_SelectBtn:SetCheck(true)
					
					-- 取消 标示所有
					local SelectAll = Lan_NpcClassify_Common(0, "TypeName")			-- 标示所有
					g_GameMain.m_AreaInfoWnd.m_SelectNpcSignListCtrl.m_ItemTbl[SelectAll].m_SelectBtn:SetCheck(false)
					g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[SelectAll] = false
						
					-- 如果子菜单全部选中 主菜单自动选中
					local IsAllSelect = true
					for _, SubMenuItem in pairs(self["MainMenuItem"].m_SubMenuWnd.m_SubSignListCtrl.m_SubItemTbl) do
						if not SubMenuItem.m_SelectBtn:GetCheck() then
							IsAllSelect = false
							break
						end
					end
					
					if IsAllSelect then
						self["MainMenuItem"].m_SelectBtn:SetCheck(true)
						local MenuName = self["MainMenuItem"].m_TypeNameText:GetWndText()
						g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[MenuName] = true
					end
					
				else
					g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[TypeName] = false
					--主菜单的选择也去掉
					self.m_SelectBtn:SetCheck(false)
					self["MainMenuItem"].m_SelectBtn:SetCheck(false)
					local MenuName = self["MainMenuItem"].m_TypeNameText:GetWndText()
					g_GameMain.m_AreaInfoWnd.m_NpcSignTypeSelectTbl[MenuName] = false
				end
				
				if g_GameMain.m_AreaInfoWnd.m_SceneName == nil then
					return
				end
				SaveAreaSelect()
				g_GameMain.m_AreaInfoWnd:RemoveAllNpcPointBtn(g_GameMain.m_AreaInfoWnd.m_SceneName)
				g_GameMain.m_AreaInfoWnd:DisplayAllCharPointBtn(g_GameMain.m_AreaInfoWnd.m_SceneName)
				self:SetFocus()
			end
		end			
	end
end
