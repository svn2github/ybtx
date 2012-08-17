gac_require "information/select_obj_tooltips/SelectObjTooltipsInc"
lan_load "npc/Lan_Npc_Common"

local function SetPos(wnd, x, y)
	local Rect = CFRect:new()
	local childRect = CFRect:new()
	wnd:GetLogicRect(Rect)
	wnd.m_DescRichText:GetLogicRect(childRect)
	
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x 
		Rect.right	= x + uWidth
	end
	if(y) then
		local uHeight = childRect.bottom - childRect.top
		Rect.top	= y - uHeight
		Rect.bottom	= y
	end
	wnd:SetLogicRect(Rect)
end

function CreateSelectObjTooltips(parent)
	local wnd = CSelectObjTooltips:new()
	wnd:CreateFromRes("CharacterTooltips", parent)
	return wnd
end

function CSelectObjTooltips:OnChildCreated()
	self.m_DescRichText = self:GetDlgChild("DescRichText")
	self.m_DescRichText:SetAutoWidth(50, 200)
	self.m_DescRichText:SetMinHeight(24)
end

function CSelectObjTooltips:ShowInfo(selectObj)
	local posCursor = CFPos:new()
	g_App:GetRootWnd():GetLogicCursorPos(posCursor)
	
	local type = selectObj.m_Properties:GetType()
	local name   = selectObj.m_Properties:GetCharName()
	local tblInfo = {}
	-----------------------------------
	--大逃杀中用到
	if(1 == g_GameMain.m_SceneStateForClient) then
		if type == ECharacterType.Npc then
			local Master = selectObj:GetMaster()
			if not Master or Master ~= g_MainPlayer then
				name = GetStaticTextClient(1113)--逃亡者
			end
		else
			name = GetStaticTextClient(1113)--逃亡者
		end
	end
	------------------------------------
	
	if(type ~= ECharacterType.IntObj) then
		local camp		= selectObj:CppGetCamp()
		local level		= selectObj:CppGetLevel()
		local bFlag		= (level > g_MainPlayer:CppGetLevel()+5) and (g_MainPlayer:CppGetCamp() ~= camp)
		tblInfo[5]	= bFlag and "??级" or level .. "级"
		if(type == ECharacterType.Player) then
			tblInfo[4] = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(selectObj:CppGetClass())
			if(g_MainPlayer:CppGetCamp() ~= camp) then --不同阵营
				local tbl = {"艾米", "神圣教廷", "休斯"}
				tblInfo[2] = tbl[camp]
			end
		elseif(type == ECharacterType.Npc) then
			local honour = Lan_Npc_Common(MemH64(name),"NpcHonour")
			if("" ~= honour) then honour = string.format("<%s>", honour) end
			name = GetNpcDisplayName(name)
			tblInfo[1]	= honour
		end
	else
		local res = selectObj.m_Properties:GetResourceCount()
		if res > 0 then
			tblInfo[1] = res
		end
	end
	
	local str = name
	for i, v in pairs(tblInfo) do
		if("" ~= v) then
			str = str .. "#r" .. v
		end
	end
	local color = g_GameMain.m_CharacterInSyncMgr:GetTextColor(selectObj, type)
	self.m_DescRichText:SetWndText(color .. str)
	SetPos(self, posCursor.x, posCursor.y)
end

function CSelectObjTooltips:OnMouseMoveInGameMain()

	local SelectObjTooltipsOnTime = function ()
		self:ShowWnd( false )
		if( self.m_SelectObjTooltipsTick ) then
			UnRegisterTick( self.m_SelectObjTooltipsTick )
			self.m_SelectObjTooltipsTick = nil
		end
	end
	
	SelectObjTooltipsOnTime()
	
	if( g_CoreScene ) then
		local selectObj = CEntityClient_GetSelectedEntity(g_CoreScene)
		if( selectObj ) then
			self.m_SelectObjTooltipsTick = RegisterTick( "SelectObjTooltipsOnTime", SelectObjTooltipsOnTime, 2000 )
			local renderObj = selectObj:GetRenderObject()
			self:ShowInfo( selectObj )
			self:ShowWnd(true)
		end
	end
end
