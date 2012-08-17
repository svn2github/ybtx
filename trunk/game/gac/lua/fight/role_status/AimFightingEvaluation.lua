CAimFightingEvaluation  = class( SQRDialog )

--创建战斗力评定面板
function CAimFightingEvaluation:Ctor()
	self:CreateFromRes("AimFightingEvaluationWnd", g_GameMain.m_AimPlayerManageWnd)
end

function CAimFightingEvaluation:OnChildCreated()	
	self.m_RatingValueInfoTbl = {}
	for i = 1,7 do
		local RatingValueWnd			= self:GetDlgChild("RatingValueWnd" .. i)
		table.insert(self.m_RatingValueInfoTbl,RatingValueWnd)
	end
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_PlayerName = self:GetDlgChild("PlayerName")
	self.m_PlayerName:ShowWnd(false)
	self.m_CloseBtn:ShowWnd(false)
	local Rt = CFRect:new()
	g_GameMain.m_AimPlayerManageWnd.m_ActionCheckBtnTbl[1]:GetLogicRect(Rt)
	Rt.top = Rt.bottom + 3
	Rt.bottom = Rt.top + self:GetWndOrgHeight()
	Rt.right = Rt.left +  self:GetWndOrgWidth()
	self:SetLogicRect(Rt)
end

function CAimFightingEvaluation.RetAimFightingEvaluationInfoInfo(uCharID,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
	if not IsCppBound( g_GameMain.m_AimStatusWnd.m_Target) or g_GameMain.m_AimStatusWnd.m_Target.m_uID ~= uCharID then
		return 
	end
	local wnd = g_GameMain.m_AimFightingEvaluationWnd
	wnd.m_uTotalPoint = uTotalPoint
	local uRatingValue1 = string.format("%.2f",uRatingValue1)
	local uRatingValue2 = string.format("%.2f",uRatingValue2)
	local uRatingValue3 = string.format("%.2f",uRatingValue3)
	local uRatingValue4 = string.format("%.2f",uRatingValue4)
	local uTotalPoint = string.format("%.2f",uTotalPoint)
	wnd.m_RatingValueInfoTbl[1]:SetWndText(uRatingValue1)
	wnd.m_RatingValueInfoTbl[2]:SetWndText(uRatingValue2)
	wnd.m_RatingValueInfoTbl[3]:SetWndText(uRatingValue3)
	wnd.m_RatingValueInfoTbl[4]:SetWndText(uRatingValue4)
	wnd.m_RatingValueInfoTbl[5]:SetWndText(uSuitPoint)
	wnd.m_RatingValueInfoTbl[6]:SetWndText(uIntensifyPoint)
	wnd.m_RatingValueInfoTbl[7]:SetWndText(uTotalPoint)
	if g_GameMain.m_AimStatusWnd.m_Target then
		local NpcLevel = g_GameMain.m_AimStatusWnd.m_Target:CppGetLevel()
		local TipStr = CEquipAssessWnd.GetEquipAssessTipsStr(1,NpcLevel,g_GameMain.m_AimStatusWnd)
		wnd.m_RatingValueInfoTbl[7]:SetMouseOverDescAfter(TipStr)
	end	
	if g_GameMain.m_AimStatusWnd.m_FightingBtn then
		local str = GetStaticTextClient(4045) .. uTotalPoint
		g_GameMain.m_AimStatusWnd.m_FightingBtn:SetWndText(str)
	end
end


