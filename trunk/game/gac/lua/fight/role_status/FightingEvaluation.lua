CFightingEvaluation  = class( SQRDialog )


--创建战斗力评定面板
function CFightingEvaluation:Ctor()
	self:CreateFromRes("FightingEvaluationWnd", g_GameMain.m_PlayerManageWnd)
end

function CFightingEvaluation:OnChildCreated()	
	self.m_RatingValueInfoTbl = {}
	for i = 1,7 do
		local RatingValueWnd			= self:GetDlgChild("RatingValueWnd" .. i)
		table.insert(self.m_RatingValueInfoTbl,RatingValueWnd)
	end
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_CloseBtn:ShowWnd(false)
	self.m_PlayerName = self:GetDlgChild("PlayerName")
	self.m_PlayerName:ShowWnd(false)
	local Rt = CFRect:new()
	g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[1]:GetLogicRect(Rt)
	Rt.top = Rt.bottom + 3
	Rt.bottom = Rt.top + self:GetWndOrgHeight()
	Rt.right = Rt.left +  self:GetWndOrgWidth()
	self:SetLogicRect(Rt)	
end

function CFightingEvaluation:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if(Child == self.m_CloseBtn)then
			self:ShowWnd(false)
		end
	end
end

function CFightingEvaluation.RetFightingEvaluationInfoInfo(uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
	local wnd = g_GameMain.m_FightingEvaluationWnd
	local uRatingValue1 = string.format("%.2f",uRatingValue1)
	local uRatingValue2 = string.format("%.2f",uRatingValue2)
	local uRatingValue3 = string.format("%.2f",uRatingValue3)
	local uRatingValue4 = string.format("%.2f",uRatingValue4)
	local uTotalPoint = string.format("%.2f",uTotalPoint)
	wnd.m_PlayerName:SetWndText(g_MainPlayer.m_Properties:GetCharName())
	wnd.m_RatingValueInfoTbl[1]:SetWndText(uRatingValue1)
	wnd.m_RatingValueInfoTbl[2]:SetWndText(uRatingValue2)
	wnd.m_RatingValueInfoTbl[3]:SetWndText(uRatingValue3)
	wnd.m_RatingValueInfoTbl[4]:SetWndText(uRatingValue4)
	wnd.m_RatingValueInfoTbl[5]:SetWndText(uSuitPoint)
	wnd.m_RatingValueInfoTbl[6]:SetWndText(uIntensifyPoint)
	wnd.m_RatingValueInfoTbl[7]:SetWndText(uTotalPoint)
	if g_MainPlayer then
		local NpcLevel = g_MainPlayer:CppGetLevel()
		local TipStr = CEquipAssessWnd.GetEquipAssessTipsStr(1,NpcLevel,g_GameMain.m_RoleStatus)
		wnd.m_RatingValueInfoTbl[7]:SetMouseOverDescAfter(TipStr)
	end
	if g_GameMain.m_RoleStatus.m_FightingBtn then
		local str = GetStaticTextClient(4045) .. uTotalPoint
		g_GameMain.m_RoleStatus.m_FightingBtn:SetWndText(str)
	end
end

function CFightingEvaluation.RetUpdateEquipEffect(Conn,uState)
	Gac2Gas:UpdateEquipEffectToGas(Conn,uState)
end

