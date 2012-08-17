CNpcSkillLearnWnd = class(SQRDialog)

----------------------------------点击天赋转职npc-------------------------------------------
function CNpcSkillLearnWnd:Ctor()
	self:CreateFromRes("NpcSkillLearnWnd",g_GameMain)
	self.m_LearnSeriesBtn = self:GetDlgChild("LearnSeriesBtn")
	self.m_ClearTalentBtn = self:GetDlgChild("ClearTalentBtn")
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

local function ClearAllTalent(g_GameMain,nIndex)
	if(nIndex == MB_BtnOK) then
		Gac2Gas:ClearAllGenius(g_Conn)
	end
	g_GameMain.m_NpcSkillLearnWnd.m_MsgBox = nil
	return true
end

function CNpcSkillLearnWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_LearnSeriesBtn then
			self:ShowWnd(false)
			g_GameMain.m_SelectSeriesWnd:ShowWnd(true)
			g_GameMain.m_SelectSeriesWnd:ShowFun()
			local AutoCloseWnd = CAutoCloseWnd:new()
			AutoCloseWnd:AutoCloseWnd(g_GameMain.m_SelectSeriesWnd)
		elseif Child == self.m_ClearTalentBtn then
			--角色死亡,不可以洗天赋
			if not g_MainPlayer:CppIsAlive() then
				MsgClient(1604)
				return
			end
			if g_MainPlayer:IsInBattleState() then
				MsgClient(1607)
				return
			end

			local ConsumeItemName = ""
			local ConsumeItemNumber = 0
			local ConsumeItemFlag = false
			local ConsumeItemTbl = GetClearTalentConsumeItem(g_MainPlayer:CppGetLevel())
			for j,ItemTbl in pairs(ConsumeItemTbl["ConsumeItem"]) do 
				local ItemNum = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace( g_StoreRoomIndex.PlayerBag, g_ItemInfoMgr:GetBasicItemBigID(), ItemTbl[1] )
				if ItemNum >= ItemTbl[2] then
					ConsumeItemName = g_ItemInfoMgr:GetItemLanInfo(g_ItemInfoMgr:GetBasicItemBigID(),ItemTbl[1],"DisplayName")
					ConsumeItemNumber = ItemTbl[2]
					ConsumeItemFlag = true
					break
				end
			end
		
			local MsgBoxMsgStr = ""
			local MsgBoxMsgErrStr = ""
			if ConsumeItemFlag then
				if ConsumeItemNumber > 0 then
					MsgBoxMsgStr = MsgBoxMsg(12008,ConsumeItemName,ConsumeItemNumber)
				end
			else
				MsgBoxMsgErrStr =  MsgBoxMsg(12007)
			end
				
			local ConsumeMoneyFlag = false
			local c_money = CMoney:new()
        	local MoneyStr = c_money:ChangeMoneyToString(ConsumeItemTbl["ConsumeMoney"])
			local allMoney =  tonumber(g_MainPlayer:GetBindMoney()) + tonumber(g_MainPlayer:GetMoney())
			if allMoney >= ConsumeItemTbl["ConsumeMoney"] then
				ConsumeMoneyFlag = true
				if ConsumeItemTbl["ConsumeMoney"] > 0 then
					MsgBoxMsgStr = MsgBoxMsgStr .. " " .. MsgBoxMsg(12010,MoneyStr) 
				end
			else
				MsgBoxMsgErrStr = MsgBoxMsgErrStr .. MsgBoxMsg(12009,MoneyStr)
			end
			
			local ConsumeSoulFlag = false
			if g_MainPlayer.m_PlayerSoulNum >= ConsumeItemTbl["ConsumeSoul"] then
				ConsumeSoulFlag = true
				if ConsumeItemTbl["ConsumeSoul"] > 0 then 
					MsgBoxMsgStr = MsgBoxMsgStr .. " " .. MsgBoxMsg(12012,ConsumeItemTbl["ConsumeSoul"])
				end
				
			else
				MsgBoxMsgErrStr = MsgBoxMsgErrStr .. MsgBoxMsg(12011,ConsumeItemTbl["ConsumeSoul"])
			end
	
			if ConsumeItemFlag and ConsumeMoneyFlag and  ConsumeSoulFlag then 
				if MsgBoxMsgStr == "" then
					self.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(12013,MsgBoxMsgStr), BitOr(MB_BtnOK,MB_BtnCancel),ClearAllTalent,g_GameMain)	
				else					
					self.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(12002,MsgBoxMsgStr), BitOr(MB_BtnOK,MB_BtnCancel),ClearAllTalent,g_GameMain)	
				end
			else					
				self.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsgErrStr, MB_BtnOK)	
			end
		end
	end
end

function CNpcSkillLearnWnd:SetBtnState(IsLearn)
	if IsLearn  then
		self.m_LearnSeriesBtn:EnableWnd(false)
		self.m_ClearTalentBtn:EnableWnd(true)
	else
		self.m_LearnSeriesBtn:EnableWnd(true)
		self.m_ClearTalentBtn:EnableWnd(false)				
	end
end

function CNpcSkillLearnWnd:OpenSelf()
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

-------------------------------洗天赋---------------------------------------
--清楚技能快捷栏上的数据信息
function CNpcSkillLearnWnd.ClearToolBarTalentSkill(Tbl)
	for j = 1, #(Tbl) do
		g_GameMain.m_BeastPoseWnd:ClearSkillInfo(g_GameMain.m_MainSkillsToolBar.m_PieceTbl,Tbl[j])
	end
	g_GameMain.m_BeastPoseWnd:UpdateBeastPosToolBar()
end

--洗天赋
function CNpcSkillLearnWnd.ClearAllGenius()
	g_GameMain.m_NpcSkillLearnWnd:SetBtnState(false)
	g_GameMain.m_CreateTalentSkillPartWnd.m_IsShow = false

	for i = 1, #(g_GameMain.m_LearnCommonSkillWnd.m_TalentSkillItemTbl) do
		g_GameMain.m_LearnCommonSkillWnd.m_TalentSkillItemTbl[i].m_IsInit = false
	end
	
	CNpcSkillLearnWnd.ClearToolBarTalentSkill(g_GameMain.m_SelectSeriesWnd.m_TalentSkillNameTbl)

	Gac2Gas:SaveShortCutEnd(g_Conn)
	g_GameMain.m_NpcSkillLearnWnd.m_SkillSeriesKind = 0
	g_GameMain.m_SelectSeriesWnd.m_ConsumeTalentNumber = 0
	g_GameMain.m_SelectSeriesWnd.m_SeriesInit = nil
	g_GameMain.m_SelectSeriesWnd:SetTalentNumber()
	g_GameMain.m_SelectSeriesWnd:SetSeriesIcon()
	g_GameMain.m_LeadLeanSkillWnd:RemoveTalentSkill()
	g_GameMain.m_LeadLeanSkillWnd:InitData()
	g_GameMain.m_CreateTalentSkillPartWnd:SetDefaultInfo()
	g_GameMain.m_SelectSeriesWnd:ShowWnd(false)
	UpdateSkillAllWndRect()
	g_GameMain.m_LearnedSkillShowWnd:ClearAllGenius()
end