gac_require "setting/key_map/KPmapInc" 
gac_require "setting/key_map/KPmapSelectEmbed" 
gac_require "setting/key_map/KPmap_Cfg" 
gac_require "setting/key_map/KeyFunction"
cfg_load "key_map/KeyMap_Common"
cfg_load "area/Area_CameraRule_Client"
cfg_load "toolbar/Funcbar_Client"


--默认情况下两者相同
function CKPmap:Ctor(Parent)
	self:CreateFromRes( "KPmap", Parent )
	self:GetDlgChild("command"):EnableWnd(false)
	self:GetDlgChild( "default1" ):EnableWnd(false)
	self:GetDlgChild( "default2" ):EnableWnd(false)
	self.mSkillItems = self:GetDlgChild( "SkillItems" )
	self.SkillKeyWnd = {}
	self.mSkillItems:DeleteAllItem()
	self.mSkillItems:InsertColumn( 0, 480 )
	self.AllKeyTbl = {}
	self.KeyTable = {}
	self.MoveCtrlKey_Tbl = {}
	self.WASDkey = {{"",""},{"",""},{"",""},{"",""}}
	self.MoveDirTable = {"UP","DOWN","LEFT","RIGHT"}
	self:DrawKPmapWnd()
	self:RegisterDefaultKey()
	--self:ShowWnd( true )
end

function CreateKPmap(Parent)
	local Wnd = CKPmap:new(Parent)
	return Wnd
end

local KeyMapClientTbl =  --索引和快捷栏上的功能按钮对应
{
	["角色"] = "打开/关闭主角属性面板",
	["背包"] = "打开/关闭背包",
	["好友"] = "好友",
	["任务"] = "打开佣兵手册",
	["技能"] = "打开技能综合面板",
	["地图"] = "打开区域系统面板",
	["设置"] = "打开系统面板",
	["菜单"] = "系统菜单"
}


function CKPmap:DrawKPmapWnd()
	self.mSkillItems:DeleteAllItem()
	self.SkillKeyWnd = {}
	self.mSkillItems:InsertColumn( 0, 480 )
	local uCount = KeyMap_Common:GetHeight()
	for i = 1,uCount do
		self.mSkillItems:InsertItem(i-1, 36)
		local embedvalue = KPmapSelectEmbed:new()
		local item = self.mSkillItems:GetSubItem( i-1, 0 )
		embedvalue:CreateFromRes( "Skillkey", item) 
		embedvalue:ShowWnd( true )
		local itemvalue = item:GetDlgChild( "Skillkey" )
		itemvalue:SetMouseWheel(true)
		self:CopySkillKey2Wnd( itemvalue, KeyMap_Common(i), Lan_KeyMap_Common(i,"DefaultKey1"), Lan_KeyMap_Common(i,"DefaultKey2") )
		table.insert(self.SkillKeyWnd, itemvalue)
	end
end

function CKPmap:DrawTextKPmapWndItem()
	local uCount = KeyMap_Common:GetHeight()
	for i = 1,uCount do
		self:CopySkillKey2Wnd( self.SkillKeyWnd[i], KeyMap_Common(i), Lan_KeyMap_Common(i,"DefaultKey1"), Lan_KeyMap_Common(i,"DefaultKey2") )
	end
end

function CKPmap:CopySkillKey2Wnd( wnd, Cfg, DefaultKey1, DefaultKey2 )
	wnd:SetKeyFuncName( Cfg("Index"), tostring(Cfg("Name")) )
	wnd:SetKeyWord(1,DefaultKey1)
	wnd:GetDlgChild("oneKey"):SetMouseWheel(true)
	wnd:SetKeyWord(2,DefaultKey2)
	wnd:GetDlgChild( "otherKey"):SetMouseWheel(true)
end

function CKPmap:ChangeSkillKey()--更改设置函数
	for k, v in pairs(self.KeyTable) do
		for j=1, #(self.SkillKeyWnd) do
			if( self.SkillKeyWnd[j].m_FuncName == k )then	 --找到需要更改的位置
				if(v[1])then
					self.SkillKeyWnd[j]:SetKeyWord(1,v[1])
				end
				if(v[2])then
					self.SkillKeyWnd[j]:SetKeyWord(2,v[2])
				end
				break
			end
		end
	end
end
function CKPmap:ChangeSettings()
	local count = {}
	local uCount = KeyMap_Common:GetHeight()
	for i=1, #(KPmapSelectEmbed.NowSkillKey) do
		local flag = true
		local NewValue = tostring(KPmapSelectEmbed.NowSkillKey[i][2])
		for k, v in pairs(self.KeyTable) do
			if(KPmapSelectEmbed.NowSkillKey[i][1] == k)then
				if v[KPmapSelectEmbed.NowSkillKey[i][3]] then
					flag = false
				end
				if(NewValue == v[1] and KPmapSelectEmbed.NowSkillKey[i][3] == 1 ) or 
					(NewValue == v[2] and KPmapSelectEmbed.NowSkillKey[i][3] == 2 ) then
					table.insert(count,i)
					break
				end
			end
		end
		if(flag)then
			for k=1,uCount do
				if(KeyMap_Common(k,"Name") == KPmapSelectEmbed.NowSkillKey[i][1]) then
					if(KPmapSelectEmbed.NowSkillKey[i][3]==1 and tostring(Lan_KeyMap_Common(k,"DefaultKey1")) == NewValue ) or 
						(KPmapSelectEmbed.NowSkillKey[i][3]==2 and tostring(Lan_KeyMap_Common(k,"DefaultKey2")) == NewValue ) then
						table.insert(count,i)
						break
					end
				end
			end
		end
	end
	table.sort(count)
	for i=1,#(count) do  --删除没有改变的
		table.remove(KPmapSelectEmbed.NowSkillKey,#(count)-i+1)
	end

	for i=1,#(KPmapSelectEmbed.NowSkillKey) do
		local NewValue = tostring(KPmapSelectEmbed.NowSkillKey[i][2])
		local updateflag = true
		for k=1,uCount do
			if(KeyMap_Common(k,"Name") == KPmapSelectEmbed.NowSkillKey[i][1]) then
				if(KPmapSelectEmbed.NowSkillKey[i][3]==1 and tostring(Lan_KeyMap_Common(k,"DefaultKey1")) == NewValue ) or 
					(KPmapSelectEmbed.NowSkillKey[i][3]==2 and tostring(Lan_KeyMap_Common(k,"DefaultKey2")) == NewValue ) then
					--判断新设置的值是否和静态文件的值一样,一样时通知服务端重数据库删除这项设置
					Gac2Gas:SetKeyMap(g_Conn,KPmapSelectEmbed.NowSkillKey[i][1],NewValue,KPmapSelectEmbed.NowSkillKey[i][3], false)
					updateflag = false
				end
				break
			end
		end
		if(updateflag)then
			Gac2Gas:SetKeyMap(g_Conn,KPmapSelectEmbed.NowSkillKey[i][1],NewValue,KPmapSelectEmbed.NowSkillKey[i][3], true)
		end
	end
	KPmapSelectEmbed.NowSkillKey = {}
end

function CKPmap:SetCancel()
	self:RegisterAccelKeyKey()
	KPmapSelectEmbed.NowSkillKey = {}
	KPmapSelectEmbed.NowSelectedWnd = nil
	self:DrawTextKPmapWndItem()
	self:ChangeSkillKey()
end

function CKPmap:SetDefault()
	local uCount = KeyMap_Common:GetHeight()
	for j = 1,#(KPmapSelectEmbed.NowSkillKey) do
		for i=1, uCount do  --先比较KeyTable和KeyMap_Common的不同
			if( KPmapSelectEmbed.NowSkillKey[j][1] == KeyMap_Common(i,"Name") and  KPmapSelectEmbed.NowSkillKey[j][3] == 1)then
				table.insert(KPmapSelectEmbed.NowSkillKey,{KeyMap_Common(i,"Name"),tostring(Lan_KeyMap_Common(i,"DefaultKey1")),1})
				self.SkillKeyWnd[i]:SetKeyWord(1,Lan_KeyMap_Common(i,"DefaultKey1"))
			end
			if(KPmapSelectEmbed.NowSkillKey[j][1] == KeyMap_Common(i,"Name")  and KPmapSelectEmbed.NowSkillKey[j][3] == 2) then
				table.insert(KPmapSelectEmbed.NowSkillKey,{KeyMap_Common(i,"Name"),tostring(Lan_KeyMap_Common(i,"DefaultKey2")),2})
				self.SkillKeyWnd[i]:SetKeyWord(2,Lan_KeyMap_Common(i,"DefaultKey2"))
			end
		end
	end
	KPmapSelectEmbed.NowSkillKey = {}
	for k, v in pairs(self.KeyTable) do
		for i=1, uCount do  --先比较KeyTable和KeyMap_Common的不同
			if( k == KeyMap_Common(i,"Name") and v[1])then
				table.insert(KPmapSelectEmbed.NowSkillKey,{KeyMap_Common(i,"Name"),tostring(Lan_KeyMap_Common(i,"DefaultKey1")),1})
				self.SkillKeyWnd[i]:SetKeyWord(1,Lan_KeyMap_Common(i,"DefaultKey1"))
			end
			if( k == KeyMap_Common(i,"Name") and v[2]) then
				table.insert(KPmapSelectEmbed.NowSkillKey,{KeyMap_Common(i,"Name"),tostring(Lan_KeyMap_Common(i,"DefaultKey2")),2})
				self.SkillKeyWnd[i]:SetKeyWord(2,Lan_KeyMap_Common(i,"DefaultKey2"))
			end
		end
	end
end

function CKPmap:OnClose()
	self:ShowWnd( false )
	self:SetCancel()
end

function CKPmap:IsMovingByKeyboard()
	if #(self.MoveCtrlKey_Tbl) == 0 then
		return false
	else
		return true
	end
end

function CKPmap:SaveMoveKey(key)
	local MoveKey = self:GetWASDkey()
	for n =1,#(MoveKey) do
		if MoveKey[n][1] == tostring(key) or MoveKey[n][2] == tostring(key) then
			for j = 1, #(self.MoveCtrlKey_Tbl) do
				if self.MoveCtrlKey_Tbl[j] == key then
					return false
				end
			end
			table.insert(self.MoveCtrlKey_Tbl,key)
			return true
		end
	end
	return false
end
function CKPmap:ReMoveMoveKey(key)
	for j = 1, #(self.MoveCtrlKey_Tbl) do
		if self.MoveCtrlKey_Tbl[j] == key then
			table.remove(self.MoveCtrlKey_Tbl,j)
			return true
		end
	end
	return false
end
function CKPmap:GetMoveDirection()
	local MoveKey = self:GetWASDkey()
	local MoveDirection = {}
	local Direction = 0
	local ret = ""
	local count = #(self.MoveCtrlKey_Tbl)
	for i=1, count do
		for j=1,#(MoveKey)do
			if tostring(self.MoveCtrlKey_Tbl[i]) == MoveKey[j][1] or tostring(self.MoveCtrlKey_Tbl[i]) == MoveKey[j][2] then
				table.insert(MoveDirection,j)
			end
			if tostring(self.MoveCtrlKey_Tbl[count]) == MoveKey[j][1] or tostring(self.MoveCtrlKey_Tbl[count]) == MoveKey[j][2] then
				Direction = j
			end
		end
	end

	if #(MoveDirection) == 1 then
		if MoveDirection[1] == 1 then
			ret = EDirectionWord.Up
		elseif MoveDirection[1] == 2 then
			ret = EDirectionWord.Down
		elseif MoveDirection[1] == 3 then
			ret = EDirectionWord.Left
		elseif MoveDirection[1] == 4 then
			ret = EDirectionWord.Right
		end
		return ret
	end
	
	for n=1,#(MoveDirection) do
		if MoveDirection[n] == 1 then
			if Direction == 3 then
				ret = EDirectionWord.LeftUp
			elseif Direction == 4 then
				ret = EDirectionWord.RightUp
			elseif Direction == 2 then
				ret = EDirectionWord.Down
			end
		elseif MoveDirection[n] == 2 then
			if Direction == 3 then
				ret = EDirectionWord.LeftDown
			elseif Direction == 4 then
				ret = EDirectionWord.RightDown
			elseif Direction == 1 then
				ret = EDirectionWord.Up
			end
		elseif MoveDirection[n] == 3 then
			if Direction == 1 then
				ret = EDirectionWord.LeftUp
			elseif Direction == 2 then
				ret = EDirectionWord.LeftDown
			elseif Direction == 4 then
				ret = EDirectionWord.Right
			end
		elseif MoveDirection[n] == 4 then
			if Direction == 1 then
				ret = EDirectionWord.RightUp
			elseif Direction == 2 then
				ret = EDirectionWord.RightDown
			elseif Direction == 3 then
				ret = EDirectionWord.Left
			end
		end
	end
	return ret
end

function CKPmap:GetWASDkey()
	return self.WASDkey
end

function CKPmap:ReSetWASDkey()
	self.WASDkey = {{"",""},{"",""},{"",""},{"",""}}
	for k, v in pairs(self.KeyTable) do
		if k == self.MoveDirTable[1] then
			if v[1] then
				self.WASDkey[1][1] = v[1]
			end
			if v[2] then
				self.WASDkey[1][2] = v[2]
			end
		elseif k == self.MoveDirTable[2] then
			if v[1] then
				self.WASDkey[2][1] = v[1]
			end
			if v[2] then
				self.WASDkey[2][2] = v[2]
			end
		elseif k == self.MoveDirTable[3] then
			if v[1] then
				self.WASDkey[3][1] = v[1]
			end
			if v[2] then
				self.WASDkey[3][2] = v[2]
			end
		elseif k == self.MoveDirTable[4] then
			if v[1] then
				self.WASDkey[4][1] = v[1]
			end
			if v[2] then
				self.WASDkey[4][2] = v[2]
			end
		end
	end
	for n =1, #(self.WASDkey) do
		local uCount = KeyMap_Common:GetHeight()
		if self.WASDkey[n][1] == "" then
			for i = 1, uCount do
				local str = {self.MoveDirTable[1],self.MoveDirTable[2],self.MoveDirTable[3],self.MoveDirTable[4]}   --改动配置表也要更改这里
				if KeyMap_Common(i,"Name") == str[n] then
					self.WASDkey[n][1] = tostring(Lan_KeyMap_Common(i,"DefaultKey1"))
					break
				end
			end
		end
		if self.WASDkey[n][2] == "" then
			for i = 1, uCount do
				local str = {self.MoveDirTable[1],self.MoveDirTable[2],self.MoveDirTable[3],self.MoveDirTable[4]}   --改动配置表也要更改这里
				if KeyMap_Common(i,"Name") == str[n] then
					self.WASDkey[n][2] = tostring(Lan_KeyMap_Common(i,"DefaultKey2"))
					break
				end
			end
		end
	end
end

function CKPmap:RegisterAccelKeyKey()
	self:UnRegisterAccelKeyKey()
	if self:IsShow() then
		return
	end
	for j=1, #(self.SkillKeyWnd) do
		self:RegisterKey( self.SkillKeyWnd[j].m_OneKeyValue )
		self:RegisterKey( self.SkillKeyWnd[j].m_OtherKeyValue )
	end
	self:RegisterDefaultKey()
end

function CKPmap:UnRegisterAccelKeyKey()
	for j=1, #(self.SkillKeyWnd) do
		self:UnRegisterKey( self.SkillKeyWnd[j].m_OneKeyValue )
		self:UnRegisterKey( self.SkillKeyWnd[j].m_OtherKeyValue )
	end
	self:UnRegisterDefaultKey()
end

function CKPmap:RegisterDefaultKey()
	self:RegisterKey(VK_CONTROL.."+"..VK_NUMPAD9)
	self:RegisterKey(VK_CONTROL.."+"..VK_NUMPAD5)
	self:RegisterKey(VK_CONTROL.."+"..VK_NUMPAD2)
end

function CKPmap:UnRegisterDefaultKey()
	self:UnRegisterKey(VK_CONTROL.."+"..VK_NUMPAD9)
	self:UnRegisterKey(VK_CONTROL.."+"..VK_NUMPAD5)
	self:UnRegisterKey(VK_CONTROL.."+"..VK_NUMPAD2)
end

function CKPmap:RegisterKey( command )
	local i, j = string.find(command, "+")
	local key1 = ""
	local key2 = ""
	if i ~= nil and j ~= nil then
		key1 = string.sub(command, 0, i-1)
		key2 = string.sub(command, j+1, string.len(command))
	end
	--( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey, uint32 wParam, uint32 lParam )
	if tonumber(key1) == VK_CONTROL then
		g_GameMain:RegisterAccelKeyCommand( true, false, false, tonumber(key2), tonumber(key2), tonumber(key2) )
	elseif tonumber(key1) == VK_MENU then
		g_GameMain:RegisterAccelKeyCommand( false, false, true, tonumber(key2), tonumber(key2), tonumber(key2) )
	elseif tonumber(key1) == VK_SHIFT then
		g_GameMain:RegisterAccelKeyCommand( false, true, false, tonumber(key2), tonumber(key2), tonumber(key2) )
	end
end

local KeyFunctionTbl = {}
local function DoFunctionByName(FuncName)
	if FuncName and FuncName ~= "" then
		if not IsFunction(KeyFunctionTbl[FuncName]) then
			KeyFunctionTbl[FuncName] = loadstring("return " .. FuncName)
		end
		KeyFunctionTbl[FuncName]()
	end
end

function CKPmap:OnCommand( wParam, lParam )
	g_GameMain.m_SysSetting.m_KPmap:SaveToAllKeyTbl(wParam)
	if self:KeyIsExist(VK_CONTROL) then
		local FuncName = GetFunctionNameByChar( VK_CONTROL.."+"..wParam )
		DoFunctionByName(FuncName)
		if wParam == VK_NUMPAD9 then   --按下小键盘9打开GM指令面板。
			OpenGMConsole()
		elseif wParam == VK_NUMPAD5 then
			TurnCameraTop()
		elseif wParam == VK_NUMPAD2 then
			TurnCameraBottom()
		end
	elseif self:KeyIsExist(VK_MENU) then
		local FuncName = GetFunctionNameByChar( VK_MENU.."+"..wParam )
		DoFunctionByName(FuncName)
	elseif self:KeyIsExist(VK_SHIFT) then
		local FuncName = GetFunctionNameByChar( VK_SHIFT.."+"..wParam )
		DoFunctionByName(FuncName)
	end
end

function CKPmap:UnRegisterKey( command )
	local i, j = string.find(command, "+")
	local key1 = ""
	local key2 = ""
	if i ~= nil and j ~= nil then
		key1 = string.sub(command, 0, i-1)
		key2 = string.sub(command, j+1, string.len(command))
	end
	--( bool bCtrl, bool bShift, bool bAlt, uint8 VirKey )
	if tonumber(key1) == VK_CONTROL then
		g_GameMain:UnRegisterAccelKeyCommand(true, false, false, tonumber(key2))
	elseif tonumber(key1) == VK_MENU then
		g_GameMain:UnRegisterAccelKeyCommand(false, false, true, tonumber(key2))
	elseif tonumber(key1) == VK_SHIFT then
		g_GameMain:UnRegisterAccelKeyCommand(false, true, false, tonumber(key2))
	end
end

function CKPmap:ReturnAllKeyMaps( KeyName, KeyValue, KeyPostion )
	if not self.KeyTable[KeyName] then
		self.KeyTable[KeyName] = {}
	end
	self.KeyTable[KeyName][KeyPostion] = KeyValue
end

function CKPmap:KeyIsExist(ukey)
	for i =1, #(self.AllKeyTbl) do
		if (self.AllKeyTbl[i] == ukey) then
			return true
		end
	end
	return false
end

function CKPmap:SaveToAllKeyTbl(ukey)
	for i =1, #(self.AllKeyTbl) do
		if (self.AllKeyTbl[i] == ukey) then
			return
		end
	end
	table.insert(self.AllKeyTbl, ukey)
end

function CKPmap:ReMoveFromAllKeyTbl(ukey)
	for i =1, #(self.AllKeyTbl) do
		if (self.AllKeyTbl[i] == ukey) then
			table.remove(self.AllKeyTbl,i)
			return
		end
	end
end

function CKPmap:MoveEndStep()
	if #(self.MoveCtrlKey_Tbl) == 0 then
		if g_MainPlayer and IsCppBound(g_MainPlayer) then
--			local CurPos = CFPos:new()
--			g_MainPlayer:GetPixelPos(CurPos)
--			local dist
--			if self.m_dir == EDirectionWord.Up or self.m_dir == EDirectionWord.Down or self.m_dir == EDirectionWord.Left or self.m_dir == EDirectionWord.Right then
--				dist = math.abs(CurPos.x - self.m_StartPos.x) * 1.414
--			elseif  self.m_dir == EDirectionWord.LeftDown or self.m_dir == EDirectionWord.RightUp  then
--				dist = math.abs(CurPos.x - self.m_StartPos.x)
--			elseif self.m_dir == EDirectionWord.LeftUp or self.m_dir == EDirectionWord.RightDown then
--				dist = math.abs(CurPos.y - self.m_StartPos.y)
--			end
--			if dist and dist < EUnits.eGridSpanForObj and
--				not g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_ForbitMove) then
--				local Dest = CFPos:new()
--				local leftDist = EUnits.eGridSpanForObj - dist
--				Dest = self:CalDestPos(CurPos, leftDist)
--				local curSpeed = g_MainPlayer:GetCurSpeed()
--				g_MainPlayer:MoveToInAStarPath(Dest, curSpeed, EFindPathType.eFPT_HypoAStar, EBarrierType.eBT_LowBarrier, 0 ,100)
--			elseif dist then
--			if dist then
			if self.m_dir then
				g_MainPlayer:SetMovingByKeyboard(false)
				g_MainPlayer:ActionTurn_UnRegisterTick()
				g_MainPlayer:MoveByGlide_UnRegisterTick()
				g_MainPlayer:StopMoving()
			end
			self.m_dir = nil
		end
	else
		self:Moving()
	end
end

function CKPmap:Moving(bForce)
	local Direction = g_GameMain.m_SysSetting.m_KPmap:GetMoveDirection()
	if g_MainPlayer == nil then
		return
	end
	
	if g_MainPlayer:IsPassive() then
		g_MainPlayer:CancelCoerceMove(g_MainPlayer:GetEntityID())
	end
	
	self.m_StartPos = CFPos:new()
	g_MainPlayer:GetPixelPos(self.m_StartPos)
	if Direction == "" then
		--正常处理
		return
	elseif Direction == EDirectionWord.Up and (self.m_dir ~= EDirectionWord.Up or bForce) then
		MoveUp(bForce)
		self.m_dir = EDirectionWord.Up
	elseif Direction == EDirectionWord.Down and (self.m_dir ~= EDirectionWord.Down or bForce) then
		MoveDown(bForce)
		self.m_dir = EDirectionWord.Down
	elseif Direction == EDirectionWord.Left and (self.m_dir ~= EDirectionWord.Left or bForce) then
		MoveLeft(bForce)
		self.m_dir = EDirectionWord.Left
	elseif Direction == EDirectionWord.Right and (self.m_dir ~= EDirectionWord.Right or bForce) then
		MoveRight(bForce)
		self.m_dir = EDirectionWord.Right
	elseif Direction == EDirectionWord.LeftUp and (self.m_dir ~= EDirectionWord.LeftUp or bForce) then
		MoveLeftUp(bForce)
		self.m_dir = EDirectionWord.LeftUp
	elseif Direction == EDirectionWord.LeftDown and (self.m_dir ~= EDirectionWord.LeftDown or bForce) then
		MoveLeftDown(bForce)
		self.m_dir = EDirectionWord.LeftDown
	elseif Direction == EDirectionWord.RightUp and (self.m_dir ~= EDirectionWord.RightUp or bForce) then
		MoveRightUp(bForce)
		self.m_dir = EDirectionWord.RightUp
	elseif Direction == EDirectionWord.RightDown and (self.m_dir ~= EDirectionWord.RightDown or bForce) then
		MoveRightDown(bForce)
		self.m_dir = EDirectionWord.RightDown
	else
		return
	end
	g_MainPlayer:SetMovingByKeyboard(true)
	g_MainPlayer:MoveOnTime_UnRegisterTick()
	g_MainPlayer:RunToMouse_UnRegisterTick()
end

function CKPmap:ClearAllKeyTbl()
	self.AllKeyTbl = {}
	self.MoveCtrlKey_Tbl = {}
end

-- 按键激发事件
function CKPmap:AllKeySet(wDownUp,wParam)
	-- 按键按下
	if g_MainPlayer and g_MainPlayer.m_ChangeCamera then
		Gac2Gas:CancelChangeCameraPos(g_Conn)
		return
	end
	
	if (wDownUp == WM_KEYDOWN or wDownUp == WM_SYSKEYDOWN) then
		if( g_GameMain.m_SysSetting.m_KPmap:IsShow() )then --按键映射时,任何键在主界面都不响应
			if KPmapSelectEmbed.NowSelectedWnd then
				KPmapSelectEmbed.NowSelectedWnd:OnChangeKeyDown( wDownUp, wParam )
				return true
			elseif(wParam == VK_ESCAPE )then
				g_GameMain.m_SysSetting.m_KPmap:OnClose()
			end
		elseif  g_GameMain.m_ExitGame:IsShow() then
			local FuncName = GetFunctionNameByChar(wParam)
			if(FuncName ~= "OpenSysPanel()")then --含义: 和OpenSysPanel()的按键不同
				return true
			end
		end
		
		if(wParam == VK_ESCAPE and g_MainPlayer and IsCppBound(g_MainPlayer) )then --特定功能,不可设置
			local state, context = g_WndMouse:GetCursorState()
			if state ~= ECursorState.eCS_Normal then
				g_WndMouse:ClearCursorSpecialState()
			end
			if g_GameMain.m_skillProgress_Load:IsShow() then
				g_MainPlayer:CancelCastingProcess()
				return true
			end
			--按下ESC, 特殊窗口(即不属于互斥范畴的)的处理,方法要改成灵活的
			if( GroundSelector:IsActive() ) then
				GroundSelector:CancelSelectGround()
				return true
			elseif g_GameMain.m_XLProgressWnd.m_IsShowWnd or g_GameMain.m_XLProgressWnd.m_IsButtonDown then
				g_GameMain.m_XLProgressWnd:StopXLFromProgress()
				return true
			elseif g_GameMain.m_UseItemProgressWnd:IsShow() then
				Gac2Gas:UseItemEnd(g_Conn, "")
				return true
			elseif g_GameMain.m_ProgressBarWnd:IsShow() or g_GameMain.m_NormalProgressBarWnd:IsShow() then
				Gac2Gas:BreakPlayerActionLoading(g_Conn)
				return true
			elseif g_GameMain.m_ResourceProgressWnd:IsShow() then
				Gac2Gas:StopLoadProgress(g_Conn)
				return true
			elseif g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= ""	then
				if g_GameMain.m_HoldThingType == "抱" then
					g_MainPlayer:StopMoving()
					Gac2Gas:ActiveBehavior(g_Conn)
				else
					Gac2Gas:QuitFitoutState(g_Conn)
				end
				return true
			elseif g_GameMain.m_PanelMenu and g_GameMain.m_PanelMenu:IsShow() then
				g_GameMain.m_PanelMenu:ShowWnd(false)
				return true
			elseif g_GameMain.m_ToLoginMsgBox and g_GameMain.m_ToLoginMsgBox:IsShow() then
				g_GameMain.m_ToLoginMsgBox:Destroy()
				return true
			elseif g_GameMain.m_PkSwitchMsgWnd and g_GameMain.m_PkSwitchMsgWnd:IsShow() then
				g_GameMain.m_PkSwitchMsgWnd:Destroy()
				return true
			elseif g_GameMain.m_ExitGameMsgBox and g_GameMain.m_ExitGameMsgBox:IsShow() then
				g_GameMain.m_ExitGameMsgBox:EscExitGame()
				return true
			elseif g_GameMain.m_LiveSkillFlower and g_GameMain.m_LiveSkillFlower:IsShow() then
				g_GameMain.m_FlowerMiniWnd:ShowWnd(true)
				g_GameMain.m_LiveSkillFlower:ShowWnd(false)
				return true
			elseif g_GameMain.m_AddNewWnd and g_GameMain.m_AddNewWnd:IsShow() then
				g_GameMain.m_AddNewWnd:ShowWnd(false)
				return true
			elseif( g_MainPlayer.m_LockCenter.m_LockingObj or g_GameMain.m_TargetInfo:IsShow() or g_MainPlayer.m_LockCenter.m_LockingIntObj) then
				g_MainPlayer:UnLockObj()
				g_MainPlayer:UnLockIntObj()
				return true
			elseif (g_GameMain.m_CreateChatWnd.m_ChatSendArea:IsShow()) then
				g_GameMain.m_CreateChatWnd.m_ChatSendArea:OnEscMsg()
				return true
			elseif(g_GameMain.m_GMConsole:IsShow())then
				g_GameMain.m_GMConsole:ShowWnd(false)
				return true
			elseif g_GameMain.m_RoleLevelAttentionWnd and (g_GameMain.m_RoleLevelAttentionWnd:IsShow())then
				g_GameMain.m_RoleLevelAttentionWnd:ShowWnd(false)
				return true
			elseif g_GameMain.m_ChatFilter then
				for i = 1, table.maxn(g_GameMain.m_ChatFilter) do
					if (g_GameMain.m_ChatFilter[i]:IsShow()) then
						if g_GameMain.m_ChatFilter[i]:IsShow() == true then
							g_GameMain.m_ChatFilter[i]:ShowWnd(false)
							return true
						end
					end
				end
			end
		end
		
		local keyIsExist = g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(wParam)
		if keyIsExist then
			return
		end
		g_GameMain.m_SysSetting.m_KPmap:SaveToAllKeyTbl(wParam)
		local Existflag = g_GameMain.m_SysSetting.m_KPmap:SaveMoveKey(wParam)
		if(Existflag) then
			self:Moving() -- 键盘移动
			return
		end
	end
	
	-- 按键释放
	if(wDownUp == WM_KEYUP or wDownUp == WM_SYSKEYUP) then
		--截屏
		if( g_GameMain.m_SysSetting.m_KPmap:IsShow() )then --按键映射时,任何键在主界面都不响应
			if KPmapSelectEmbed.NowSelectedWnd then
				KPmapSelectEmbed.NowSelectedWnd:OnChangeKeyDown( wDownUp, wParam )
				return true
			end
		end
		if (wParam == VK_SNAPSHOT) then
			local screenshotPath = CRenderSystemClient_Inst():Snapshot()
			MsgClient(8003, screenshotPath)
			return true
		end
		g_GameMain.m_SysSetting.m_KPmap:ReMoveFromAllKeyTbl(wParam)
		
		local bSucc = g_GameMain.m_SysSetting.m_KPmap:ReMoveMoveKey(wParam)
		if g_GameMain.m_SysSetting.m_KPmap:GetHaveEditActive() then   --有输入焦点, 清空AllKeyTbl
			g_GameMain.m_SysSetting.m_KPmap:ClearAllKeyTbl()
		end
		if bSucc then
			self:MoveEndStep()
		end
		if wParam == VK_NUMPAD5 then
			UnRegisterTurnTopCameraTick()
		elseif wParam == VK_NUMPAD2 then
			UnRegisterTurnBottomCameraTick()
		elseif wParam == VK_CONTROL then
			UnRegisterTurnTopCameraTick()
			UnRegisterTurnBottomCameraTick()
		end
		local KeyUpFuncName = GetKeyUpFunctionName(wParam)
		DoFunctionByName(KeyUpFuncName)
		return 
	end
	
	for i = 1,#(KP_CombinationFuncTbl) do
		local kp_tbl = KP_CombinationFuncTbl[i][1]
		for j=1,#(kp_tbl) do
			local keyIsExist = g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(kp_tbl[j])
			if not keyIsExist then
				break
			end
			if j == #(kp_tbl) then
				DoFunctionByName(KP_CombinationFuncTbl[i][2])
			end
		end
	end

	--根据按键映射 查找对应函数(静态配置表) 执行
	if (g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(VK_CONTROL) or g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(VK_SHIFT) or g_GameMain.m_SysSetting.m_KPmap:KeyIsExist(VK_MENU)) then
		if(wParam ~= VK_CONTROL and wParam ~= VK_SHIFT and wParam ~= VK_MENU)then
			return--如果"CONTROL","SHIFT","MENU"被按下时,单键消息忽略
		end
	end
	local FuncName = GetFunctionNameByChar(wParam)
	DoFunctionByName(FuncName)
end
----------------------RPC函数-------------------
function Gas2Gac:ReturnAllKeyMaps( Conn, KeyName, KeyValue, KeyPostion )
	g_GameMain.m_SysSetting.m_KPmap:ReturnAllKeyMaps( KeyName, KeyValue, KeyPostion )
end

function GetDisPlayNameByName(keyName)
	local i, j = string.find(keyName, "+")
	if i ~= nil and j ~= nil then
		local key1 = tonumber(string.sub(keyName, 0, i-1))
		local key2 = tonumber(string.sub(keyName, j+1, string.len(keyName)))
		if Lan_KeyName_Client(key1,"CombinedKeyDisplayName") and Lan_KeyName_Client(key1,"CombinedKeyDisplayName") ~= "" then
			key1 = Lan_KeyName_Client(key1,"CombinedKeyDisplayName")
		elseif Lan_KeyName_Client(key1, "KeyDisplayName") then
			key1 = Lan_KeyName_Client(key1, "KeyDisplayName")
		end
		if Lan_KeyName_Client(key2,"CombinedKeyDisplayName") and Lan_KeyName_Client(key2,"CombinedKeyDisplayName") ~= "" then
			key2 = Lan_KeyName_Client(key2,"CombinedKeyDisplayName")
		elseif Lan_KeyName_Client(key2, "KeyDisplayName") then
			key2 = Lan_KeyName_Client(key2, "KeyDisplayName")
		end
		keyName = key1.."+"..key2
	elseif keyName == NONEKey then
		keyName = GetStaticTextClient(1109)
	elseif Lan_KeyName_Client(tonumber(keyName),"KeyDisplayName") then
		keyName = Lan_KeyName_Client(tonumber(keyName),"KeyDisplayName")
	else
		LogErr("违反规定的键值:"..keyName)
	end
	return keyName
end

function Gas2Gac:ReturnAllKeyMapsEnd(Conn)
	g_GameMain.m_SysSetting.m_KPmap:ChangeSkillKey()
	local wndMainBar	= g_GameMain.m_MainSkillsToolBar
	
	local function GetKeyNameByKeyTbl(k,v)
		local keyName = v[1]
		if keyName then
			keyName = tostring(keyName)
		end
		if v[2] and keyName == NONEKey then
			keyName = v[2]
		end
		if keyName == nil then
			local uCount = KeyMap_Common:GetHeight()
			for j=1, uCount do
				if( KeyMap_Common(j,"Name") == k )then
					keyName = tostring(Lan_KeyMap_Common(j,"DefaultKey1"))
				end
			end
		end
		if keyName == nil then --可能配置表里删除了
			return ""
		end
		return GetDisPlayNameByName(keyName)
	end
	
	for j = 1, 10 do
		wndMainBar:SetKey(j,g_ShortCutKeyNametbl[1][j])
		g_GameMain.m_MasterSkillArea:SetKey(j,g_ShortCutKeyNametbl[2][j])
		g_GameMain.m_AddMasterSkillArea:SetKey(j,g_ShortCutKeyNametbl[3][j])		
		for k, v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
			local Name = GetKeyNameByKeyTbl(k,v)
			if(k=="快捷键"..j)then
				wndMainBar:SetKey(j,Name)
			end
			if (k=="扩展技能栏左-技能"..j) then
				g_GameMain.m_MasterSkillArea:SetKey(j,Name)
			end
			if (k=="扩展技能栏右-技能"..j) then
				g_GameMain.m_AddMasterSkillArea:SetKey(j,Name)
			end
		end
	end
	
	for j = 1,6 do
		g_GameMain.m_TempSkill:SetKey(j,g_TempSkillShortCutKeyNameTbl[j])
		for k,v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
			local Name = GetKeyNameByKeyTbl(k,v)
			if k == "临时技" .. j then
				g_GameMain.m_TempSkill:SetKey(j,Name)
			end
		end
	end
	
	for i, v in ipairs(g_GameMain.m_tblFunctionBar) do
		for j, w in ipairs(v.m_tblFunctionBarBtn) do
			local nTooltipsID = Funcbar_Client( tostring(i), tostring(j), "DefaultTooltips" )
			w.m_Btn:SetMouseOverDescAfter( GetStaticTextClient(nTooltipsID) )
			for k, x in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
				for l, y in pairs(KeyMapClientTbl) do
					local sFunction = Funcbar_Client( tostring(i), tostring(j), "Function" )
					if(sFunction == l and k == y) then
						local nTooltipsID = Funcbar_Client( tostring(i), tostring(j), "Tooltips" )
						local Name = GetKeyNameByKeyTbl(k, x)
						w.m_Btn:SetMouseOverDescAfter( GetStaticTextClient(nTooltipsID, Name) )
					end
				end
			end
		end
	end
	
	g_GameMain.m_SysSetting.m_KPmap:RegisterAccelKeyKey()
	g_GameMain.m_SysSetting.m_KPmap:ReSetWASDkey()
	SetEvent( Event.Test.SendAllKeyMapsEnd,true,true)
end

function Gas2Gac:ReturnSetKeyMap( Conn, succ)
	if(succ) then
		g_GameMain.m_SysSetting.m_KPmap.KeyTable = {}
		g_GameMain.m_SysSetting.m_KPmap:DrawTextKPmapWndItem()
		Gac2Gas:GetAllKeyMaps(g_Conn)
	end
	SetEvent( Event.Test.SetKeyMapEnd,true,succ)
end

--提供接口  主界面在OnAccelerator中   调用获得对应的函数名
function GetFunctionNameByChar(msg)
	local uCount = KeyMap_Common:GetHeight()
	for k, v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
		if(v[1] == tostring(msg) or v[2] == tostring(msg))then
			for i=1,uCount do
				if(k == KeyMap_Common(i,"Name") )then
					return KeyMap_Common(i,"Function")
				end
			end
		end
	end
	local FunctionName = nil
	local KeyName = nil
	local KeyPos = nil
	for k=1,uCount do
		if( Lan_KeyMap_Common(k,"DefaultKey1") == tostring(msg) )then
			FunctionName = KeyMap_Common(k,"Function")
			KeyName = KeyMap_Common(k,"Name")
			KeyPos = 1
			break
		end
		if( Lan_KeyMap_Common(k,"DefaultKey2") == tostring(msg) )then
			FunctionName = KeyMap_Common(k,"Function")
			KeyName = KeyMap_Common(k,"Name")
			KeyPos = 2
			break
		end
	end
	for k, v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
		if(k == KeyName )then
			if(v[KeyPos])then
				return nil
			end
		end
	end
	return FunctionName
end

--根绝设置的快捷键获得对应按键抬起执行函数名称
function GetKeyUpFunctionName(msg)
	local uCount = KeyMap_Common:GetHeight()
	for k, v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
		if(v[1] == tostring(msg) or v[2] == tostring(msg))then
			for i=1,uCount do
				if(k == KeyMap_Common(i,"Name") )then
					return KeyMap_Common(i,"ExecuteKeyUp")
				end
			end
		end
	end
	
	local FunctionName = nil
	local KeyName = nil
	local KeyPos = nil
	for k=1,uCount do
		if( Lan_KeyMap_Common(k,"DefaultKey1") == tostring(msg) )then
			FunctionName = KeyMap_Common(k,"ExecuteKeyUp")
			KeyName = KeyMap_Common(k,"Name")
			KeyPos = 1
			break
		end
		if( Lan_KeyMap_Common(k,"DefaultKey2") == tostring(msg) )then
			FunctionName = KeyMap_Common(k,"ExecuteKeyUp")
			KeyName = KeyMap_Common(k,"Name")
			KeyPos = 2
			break
		end
	end
	for k, v in pairs(g_GameMain.m_SysSetting.m_KPmap.KeyTable) do
		if(k == KeyName )then
			if(v[KeyPos])then
				return nil
			end
		end
	end
	return FunctionName
end

function CKPmap:CalDestPos(CurPos, leftDist)
	local DestPos = CFPos:new()
	if self.m_dir == EDirectionWord.Left then
		DestPos.x = CurPos.x - leftDist * 0.707
		DestPos.y= CurPos.y + leftDist * 0.707
	elseif self.m_dir == EDirectionWord.Right then
		DestPos.x = CurPos.x + leftDist * 0.707
		DestPos.y = CurPos.y - leftDist * 0.707
	elseif self.m_dir == EDirectionWord.Up then
		DestPos.x = CurPos.x + leftDist * 0.707
		DestPos.y = CurPos.y + leftDist * 0.707
	elseif self.m_dir == EDirectionWord.Down then
		DestPos.x = CurPos.x - leftDist * 0.707
		DestPos.y = CurPos.y - leftDist * 0.707
	elseif self.m_dir == EDirectionWord.LeftDown then
		DestPos.x = CurPos.x - leftDist
		DestPos.y = CurPos.y
	elseif self.m_dir == EDirectionWord.RightDown then
		DestPos.x = CurPos.x
		DestPos.y = CurPos.y - leftDist
	elseif self.m_dir == EDirectionWord.LeftUp then
		DestPos.x = CurPos.x
		DestPos.y = CurPos.y + leftDist
	elseif self.m_dir == EDirectionWord.RightUp then
		DestPos.x = CurPos.x + leftDist
		DestPos.y = CurPos.y
	end

	return DestPos
end

function CKPmap:OpenPanel(bFlag)
	if(bFlag) then
		self:UnRegisterAccelKeyKey()
	else
		self:RegisterAccelKeyKey()
	end
	self:ShowWnd(bFlag)
end


--按小键盘*显示或者隐藏小地图
function OpenSmallMapWnd()
	g_GameMain.m_NavigationMap:ShowWnd(not g_GameMain.m_NavigationMap:IsShow())
	g_GameMain.m_SmallMapBG:ShowWnd(not g_GameMain.m_SmallMapBG:IsShow())
end

--按小键盘“+”键放大小地图
function SmallMapWndZoomIn()
	g_GameMain.m_NavigationMap.m_nScale = g_GameMain.m_NavigationMap.m_nScale + 0.1*g_GameMain.m_NavigationMap.m_nScale
	if(g_GameMain.m_NavigationMap.m_nScale > 0.6) then
		g_GameMain.m_NavigationMap.m_nScale = 0.6
	end
	g_GameMain.m_SmallMapBG.m_Static:SetScale(g_GameMain.m_NavigationMap.m_nScale)
end

--按小键盘“-”键缩小小地图
function SmallMapWndZoomOut()
	g_GameMain.m_NavigationMap.m_nScale = g_GameMain.m_NavigationMap.m_nScale - 0.1*g_GameMain.m_NavigationMap.m_nScale
	if(g_GameMain.m_NavigationMap.m_nScale < 0.4) then
		g_GameMain.m_NavigationMap.m_nScale = 0.4
	end
	g_GameMain.m_SmallMapBG.m_Static:SetScale(g_GameMain.m_NavigationMap.m_nScale)
end
