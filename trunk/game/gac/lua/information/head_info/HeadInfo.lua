gac_require "information/head_info/HeadInfoInc"
cfg_load "int_obj/IntObj_Common"

------头顶信息窗口类------
function CreateHeadInfo(heigth)
	local wnd = CHeadInfo:new()
	wnd:CreateFromRes("HeadInformation", g_GameMain)
	wnd:SetRenderWndHeight(heigth)
	return wnd
end

function CHeadInfo:OnChildCreated()
	self.m_RichText	= self:GetDlgChild("RichText")
	self.m_Btn		= self:GetDlgChild("Btn")
	self.m_RichText:SetRowDist(0)
	self.m_RichText:SetAutoWidth(20, 320)
end

function CHeadInfo:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_CLICKDOWN) then
		if(Child == self.m_Btn and g_MainPlayer and IsCppBound(g_MainPlayer)) then
			g_MainPlayer:OnLButtonClickObj(self.m_Character)
		end
	end
end

function CHeadInfo:UpdateHeadInfoWnd(Character, sPKValue, sPKState, armyCorpsName, tongName, name, title, color)
	self.m_Character = Character
	Character:GetRenderObject():AddChild(self, eLinkType.LT_SKELETAL, "Bip01 Head")
	self:SetStyle(0x60000000)
	self:SetTransparent(1)
	self:ShowWnd(true)
	
	tbl = {sPKState, sPKValue, armyCorpsName, tongName, name, title}
	local str = ""
	for i = 1, #tbl do
		if( "" ~= tbl[i] ) then
			str = str .. tbl[i] .. "#r"
		end
	end
	
	self.m_Color			= color
	self.m_TextWithoutColor	= str
	self.m_RichText:SetWndText("#e" .. color .. str)
	self:ShowControl(Character)
end

function CHeadInfo:ShowControl(Character)
	self.m_Btn:ShowWnd(false)
	self:ShowWnd(true)
	local CharName = Character.m_Properties:GetCharName()
	local type = Character.m_Properties:GetType()
	if(type == ECharacterType.Player) then
		
	elseif(type == ECharacterType.Npc) then
		if(g_MainPlayer.m_LockCenter.m_LockingObj == Character) then
			self:TempColor()
		else
			local camp = Character:CppGetCamp()
			if( g_GameMain.m_CharacterInSyncMgr:IsNeedShowNameNpc(camp) ) then
				self:ShowWnd(false)
				self:ShowWndForQuestNeed(CharName, type)
			end
		end
	elseif(type == ECharacterType.IntObj) then
		local TriggerAction = IntObj_Common(CharName, "TriggerAction")
		if("可拾取的Obj" == TriggerAction) then
			local rt = CFRect:new()
			self.m_RichText:GetLogicRect(rt)
			self:SetLogicRect(rt)
			rt.left		= rt.left - 3
			rt.right	= rt.right + 3
			rt.top		= rt.top - 2
			rt.bottom	= rt.bottom + 2
			self.m_Btn:SetLogicRect(rt)
			self:SetStyle(0x40000000)
			self.m_Btn:ShowWnd(true)
		elseif "读条采集物品Obj保留" == TriggerAction
			or "读条采集物品Obj消失" == TriggerAction
			or "小游戏" == TriggerAction then
				self:ShowWnd(false)
				self:ShowWndForQuestNeed(CharName, type)
		end
	end
end

function CHeadInfo:TempColor()
	local type	= self.m_Character.m_Properties:GetType()
	if( type == ECharacterType.Player and not self.m_Character:IsBattleHorse() ) then
		if( g_GameMain.m_CharacterInSyncMgr:IsEnemyPlayer(self.m_Character) ) then
			local color = g_ColorMgr:GetColor("敌对目标玩家")
			self.m_bTempState = true
			self.m_RichText:SetWndText("#e" .. color .. self.m_TextWithoutColor)
		end
	elseif( type == ECharacterType.Npc ) then
		if( g_GameMain.m_CharacterInSyncMgr:IsEnemyNPC(self.m_Character) ) then
			local nOwnerEntityID = self.m_Character:GetExpOwnerID()
			local bRed = ( 0 == nOwnerEntityID ) or ( nOwnerEntityID == g_MainPlayer:GetEntityID() )
			if(not bRed) then
				local tbl = g_GameMain.m_TeamBase.m_tblTeamMemberInfo
				for i, v in ipairs(tbl) do
					if(nOwnerEntityID == v[4]) then
						bRed = true
						break
					end
				end
			end
			local color = bRed and g_ColorMgr:GetColor("敌对目标NPC") or g_ColorMgr:GetColor("敌对非归属目标NPC")
			self.m_bTempState = true
			self.m_RichText:SetWndText("#e" .. color .. self.m_TextWithoutColor)
		end
	end
end

function CHeadInfo:SelfColor()
	if(self.m_bTempState) then
		self.m_bTempState = false
		self.m_RichText:SetWndText("#e" ..self.m_Color .. self.m_TextWithoutColor)
	end
end

function CHeadInfo:ShowWndForQuestNeed(CharName, Type)
	if self:IsShow() then
		return
	end
	
	if Type == ECharacterType.Npc then
		local ShowName = Npc_Common(CharName, "Title")
		for QuestName, _ in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
			if (g_QuestShowNpcNameMgr[CharName] and g_QuestShowNpcNameMgr[CharName][QuestName])
				or (g_QuestShowNpcNameMgr[ShowName] and g_QuestShowNpcNameMgr[ShowName][QuestName]) then
					self:ShowWnd(true)
					return
			end
		end
	elseif Type == ECharacterType.IntObj then
		local ShowName = IntObj_Common(CharName, "DisplayName")
		for QuestName, _ in pairs(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo) do
			if (g_QuestShowNpcNameMgr[CharName] and g_QuestShowNpcNameMgr[CharName][QuestName])
				or (g_QuestShowNpcNameMgr[ShowName] and g_QuestShowNpcNameMgr[ShowName][QuestName]) then
					self:ShowWnd(true)
					return
			end
		end
	end
end
