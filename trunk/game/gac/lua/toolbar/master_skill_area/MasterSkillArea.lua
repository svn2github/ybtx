gac_require "toolbar/short_cut/ShortCut"
gac_require "toolbar/master_skill_area/MasterSkillAreaInc"

function CreateMasterSkillArea(parent, nType)
	local wnd = CSkillBarExpanded:new(parent, nType)
	return wnd
end

function CSkillBarExpanded:Ctor(parent, nSkillbarIndex)
	self:CreateFromRes( "SkillBarExpanded" .. nSkillbarIndex, parent )
	self.m_WndSkill = ClassCast( CShortcut, self:GetDlgChild("List") )
	self.m_WndSkill:Init(nSkillbarIndex)
	
	local tblFlag = { [2] = {}, [3] = {} }
	tblFlag[2], tblFlag[3] = CMainSkillsToolBar:GetMasterSkillAreaState()
	self:ShowWnd(tblFlag[nSkillbarIndex])
	self:SetIsBottom(true)
	self:SetRect()
end

function CSkillBarExpanded:SetRect()
	local Rect	= CFRect:new()
	local nLong	= self.m_WndSkill.m_nPieceNumber * self.m_WndSkill.m_nGridSize
	self:GetLogicRect(Rect)
	if(self.m_WndSkill.m_bVertical) then
		Rect.bottom	= Rect.top + nLong
	else
		Rect.right	= Rect.left + nLong
	end
	self:SetLogicRect(Rect)
end

function CSkillBarExpanded:UpdateToolBarTips()
	local tblShortcutPiece = self.m_WndSkill:GetTblShortcutPiece()
	for i = 1, self.m_WndSkill.m_nPieceNumber do 
		local Piece = tblShortcutPiece[i]
		local state = Piece:GetState()
		if state == EShortcutPieceState.eSPS_Skill then
			local fs = Piece:GetSkillContext()
			local tips = g_Tooltips:GetWndSkillTips(fs.SkillName, fs.Level)
			Piece.m_SkillBtn:SetMouseOverDescAfter(tips)
			g_MainPlayer:AddLearnSkill(fs.SkillName, fs.Level)
		end
	end
end

function CSkillBarExpanded:SetKey(PieceID, Key)
	local m_tblShortcutPiece = self.m_WndSkill:GetTblShortcutPiece()
	local Piece = m_tblShortcutPiece[PieceID]
	Piece.m_SkillBtn:SetWndText(Key)
end

function CSkillBarExpanded:OnClearCommonCD()
	local tblShortcutPiece = self.m_WndSkill:GetTblShortcutPiece()
	for i = 1,#tblShortcutPiece do
		tblShortcutPiece[i]:OnClearCommonCD()
	end
end
--------------------------------------------------------

function CMasterSkillItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	local parentWnd = g_GameMain.m_MasterSkillArea
	parentWnd.m_WndSkill:OnEvent(Child, uMsgID, uParam1, uParam2, parentWnd)
end

function CAddMasterSkillItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	local parentWnd = g_GameMain.m_AddMasterSkillArea
	parentWnd.m_WndSkill:OnEvent(Child, uMsgID, uParam1, uParam2, parentWnd)
end