gac_require "setting/key_map/KPmapInc" 
lan_load "keymap/Lan_KeyName_Client"
lan_load "keymap/Lan_KeyMap_Common"

KPmapSelectEmbed.NowSkillKey = {}  --保存新设置的按键key {{"功能名","keyvalue","pos"},{},{},{}}
KPmapSelectEmbed.clickKeyName = nil --功能名
KPmapSelectEmbed.clickDefaultKeyNumber = nil  --key 1  or  2
KPmapSelectEmbed.NowSelectedWnd = nil
function KPmapSelectEmbed:Ctor()
	
end
function KPmapSelectEmbed:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then                    --主界面Button事件
		self.ControlIsKeyDown = false
		self.ALTIsKeyDown = false
		self.SHIFTIsKeyDown = false
		if( Child == self:GetDlgChild("oneKey") ) then
			KPmapSelectEmbed.clickKeyName = self.m_FuncName
			KPmapSelectEmbed.clickDefaultKeyNumber = 1
			self:ResetKeyWord()
			self:GetDlgChild("oneKey"):SetWndText(GetStaticTextClient(1108))
			KPmapSelectEmbed.NowSelectedWnd = self
		elseif( Child == self:GetDlgChild("otherKey") ) then
			KPmapSelectEmbed.clickKeyName = self.m_FuncName
			KPmapSelectEmbed.clickDefaultKeyNumber = 2
			self:ResetKeyWord()
			self:GetDlgChild("otherKey"):SetWndText(GetStaticTextClient(1108))
			KPmapSelectEmbed.NowSelectedWnd = self
		end
	end
end
function KPmapSelectEmbed:ResetKeyWord()
	local uKeyWndIndex = nil
	local uKeyPos = nil
	for i=1,#(g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd) do
		if( g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[i]:GetDlgChild("oneKey"):GetWndText() == GetStaticTextClient(1108) )then
			uKeyWndIndex = i
			uKeyPos = 1
		elseif( g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[i]:GetDlgChild("otherKey"):GetWndText() == GetStaticTextClient(1108) )then
			uKeyWndIndex = i
			uKeyPos = 2
		end
	end
	if uKeyWndIndex == nil or uKeyPos == nil then
		return
	end
	for m =1, #(KPmapSelectEmbed.NowSkillKey) do
		if(KPmapSelectEmbed.NowSkillKey[m][1] == g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[uKeyWndIndex].m_FuncName
			 and KPmapSelectEmbed.NowSkillKey[m][3] == uKeyPos) then
			g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[uKeyWndIndex]:SetKeyWord(uKeyPos,KPmapSelectEmbed.NowSkillKey[m][2])
			return
		end
	end
	for k, v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
		if(k == g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[uKeyWndIndex].m_FuncName and v[uKeyPos]) then
			g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[uKeyWndIndex]:SetKeyWord(uKeyPos,v[uKeyPos])
			return
		end
	end
	local uCount = KeyMap_Common:GetHeight()
	for j=1, uCount do
		if( KeyMap_Common(j,"Name") == g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[uKeyWndIndex].m_FuncName )then
			local key = nil
			if uKeyPos == 1 then
				key = Lan_KeyMap_Common(j,"DefaultKey1")
			elseif uKeyPos == 2 then
				key = Lan_KeyMap_Common(j,"DefaultKey2")
			end
			g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[uKeyWndIndex]:SetKeyWord(uKeyPos,key)
			return
		end
	end
end

function KPmapSelectEmbed:OnChangeKeyDown( Msg, wParam, lParam )
	--知道 目前选中的SkillKeyWnd  和   Defaultkeynumber
	if(KPmapSelectEmbed.clickKeyName ~= nil)then
		if Msg == WM_KEYDOWN or Msg == WM_SYSKEYDOWN then
			if wParam == VK_CONTROL then
				self.ControlIsKeyDown = true
				return
			elseif wParam == VK_MENU then
				self.ALTIsKeyDown = true
				return
			elseif wParam == VK_SHIFT then
				self.SHIFTIsKeyDown = true
				return
			end
		end
		if Msg == WM_KEYUP or Msg == WM_SYSKEYUP then
			if wParam == VK_CONTROL then
				self.ControlIsKeyDown = false
			elseif wParam == VK_MENU then
				self.ALTIsKeyDown = false
			elseif wParam == VK_SHIFT then
				self.SHIFTIsKeyDown = false
			end
		end
		local ControlFlag = nil
		if self.ControlIsKeyDown then
			ControlFlag = VK_CONTROL.."+"
		elseif self.ALTIsKeyDown then
			ControlFlag = VK_MENU.."+"
		elseif self.SHIFTIsKeyDown then
			ControlFlag = VK_SHIFT.."+"
		end
		if self.m_FuncName == g_GameMain.m_SysSetting.m_KPmap.MoveDirTable[1] 
			or self.m_FuncName == g_GameMain.m_SysSetting.m_KPmap.MoveDirTable[2]
			or self.m_FuncName == g_GameMain.m_SysSetting.m_KPmap.MoveDirTable[3] 
			or self.m_FuncName == g_GameMain.m_SysSetting.m_KPmap.MoveDirTable[4] then
			ControlFlag = nil
		end
		
		Keypos = KPmapSelectEmbed.clickDefaultKeyNumber   --按下的是default1  or  default2
		if ControlFlag then
			local strKey = ControlFlag..wParam
			local i, j = string.find(strKey, "+")
			local key1 = ""
			local key2 = ""
			if i ~= nil and j ~= nil then
				key1 = string.sub(strKey, 0, i-1)
				key2 = string.sub(strKey, j+1, string.len(strKey))
			end
			if Lan_KeyName_Client( tonumber(key1),"KeyDisplayName" ) and Lan_KeyName_Client( tonumber(key2),"KeyDisplayName" ) then
				self:ChangeKPmapTbl( KPmapSelectEmbed.clickKeyName, Keypos, ControlFlag..wParam )
			end
		elseif(Lan_KeyName_Client( wParam,"KeyDisplayName" )) then
			self:ChangeKPmapTbl( KPmapSelectEmbed.clickKeyName, Keypos, wParam )
		else
			self:ResetKeyWord() --您的这个按键不能设置...
		end
	end
	KPmapSelectEmbed.clickKeyName = nil    --每次更改都需点button
	KPmapSelectEmbed.NowSelectedWnd = nil
end

function KPmapSelectEmbed:ChangeKPmapTbl( keyname, Keypos, wparam )
	if(KPmapSelectEmbed.NowSkillKey ~= nil) then
		--保存设置
		for i=1, table.getn(KPmapSelectEmbed.NowSkillKey) do
			if(KPmapSelectEmbed.NowSkillKey[i][1] == keyname)then
				if( Keypos == KPmapSelectEmbed.NowSkillKey[i][3]) then
					KPmapSelectEmbed.NowSkillKey[i][2] = wparam
				else
					break
				end
				self:DrawKPmap( keyname, Keypos, wparam )
				return
			end
		end
	end
	table.insert(KPmapSelectEmbed.NowSkillKey,{keyname,wparam,Keypos})
	self:DrawKPmap( keyname, Keypos, wparam )
end
function KPmapSelectEmbed:DrawKPmap( keyname, Keypos, wparam )
	if tostring(wparam) == NONEKey then
		return
	end
	for i=1,#(g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd) do
		--如果有按键key重复,以最后一次设置为准,前面的变成 "未设置"
		local wnd = g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[i]
		if wnd ~= self then
			if(wnd.m_OneKeyValue == tostring(wparam) )then
				wnd:GetDlgChild("oneKey"):SetWndText(GetStaticTextClient(1109))
				self:ChangeKPmapTbl( wnd.m_FuncName, 1, NONEKey )
			end
			if(wnd.m_OtherKeyValue == tostring(wparam) )then
				wnd:GetDlgChild("otherKey"):SetWndText(GetStaticTextClient(1109))
				self:ChangeKPmapTbl( wnd.m_FuncName, 2, NONEKey )
			end
		end
	end
	for j=1,#(g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd) do
		if( g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[j].m_FuncName == keyname )then
			g_GameMain.m_SysSetting.m_KPmap.SkillKeyWnd[j]:SetKeyWord(Keypos,wparam)
			break
		end
	end
end

local function GetKeyMapFuncDisplayName( index )
	if Lan_KeyMap_Common(index) then
		return Lan_KeyMap_Common(index,"DisplayName")
	else
		return nil
	end
end

local function GetKeyDisplayName( key )
--	if Lan_KeyName_Client(MemH64(key),"KeyDisplayName") then
--		return Lan_KeyName_Client(MemH64(key),"KeyDisplayName")
--	end
	key = tonumber(key)
	if Lan_KeyName_Client(key,"KeyDisplayName") then
		return Lan_KeyName_Client(key,"KeyDisplayName")
	end
	return nil
end

function KPmapSelectEmbed:SetKeyFuncName(index, FuncName)
	self:SetWndText(GetKeyMapFuncDisplayName(index))
	self.m_FuncName = FuncName
end

function KPmapSelectEmbed:SetKeyWord(KeyPos, KeyValue)
	KeyValue = tostring(KeyValue)
	local KeyName = ""
	local i, j = string.find(KeyValue, "+")
	local key1 = ""
	local key2 = ""
	if i ~= nil and j ~= nil then
		key1 = string.sub(KeyValue, 0, i-1)
		key2 = string.sub(KeyValue, j+1, string.len(KeyValue))
		local Key1Name = GetKeyDisplayName(key1)
		local Key2Name = GetKeyDisplayName(key2)
		if Key1Name == nil then
			Key1Name = key1
		end
		if Key2Name == nil then
			Key2Name = key2
		end
		KeyName = Key1Name .. "+" .. Key2Name
	elseif GetKeyDisplayName(KeyValue) then
		KeyName =  GetKeyDisplayName(KeyValue)
	else
		KeyName = KeyValue
	end

	if tostring(KeyValue) == NONEKey or KeyValue == nil or KeyValue == "" then
		KeyName = GetStaticTextClient(1109)
		KeyValue = ""
	end
	
	if(KeyPos == 1) then
		self:GetDlgChild("oneKey"):SetWndText(KeyName)
		self.m_OneKeyValue = KeyValue
	elseif(KeyPos == 2) then
		self:GetDlgChild("otherKey"):SetWndText(KeyName)
		self.m_OtherKeyValue = KeyValue
	end
end
