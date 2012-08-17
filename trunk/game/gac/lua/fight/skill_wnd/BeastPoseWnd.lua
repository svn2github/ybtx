gac_require "fight/skill_wnd/BeastPoseWndInc"
cfg_load "skill/SkillPart_Common"

local MainSkillWnd = 1
local MasterSkillWnd = 2
local AddMasterSkillWnd = 3

function CBeastPoseWnd:Ctor(Parent)
	self:CreateFromRes("BeastPoseWnd",Parent)
end 

function CBeastPoseWnd:OnChildCreated()
	self.m_BgBtnTbl = {}
	for i = 1, 3 do
		local Btn = self:GetDlgChild("PoseBtn" .. i)
		table.insert(self.m_BgBtnTbl,Btn)
	end
end

function CBeastPoseWnd:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if uMsgID == BUTTON_LCLICK then
		for i = 1,3 do
			if Child == self.m_BgBtnTbl[i] then
				g_MainPlayer:FightSkill( self.m_BgBtnTbl[i].m_sSkillName, self.m_BgBtnTbl[i].m_uSkillLevel )
			end
		end
	end 
end

function CBeastPoseWnd:SetSkillBtnPicture(skillIcon,skillName,skillLevel,index)
	g_LoadIconFromRes(skillIcon, self.m_BgBtnTbl[index],-1,IP_ENABLE, IP_UNCHECK_CLICKDOWN)
	self.m_BgBtnTbl[index]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(skillName,skillLevel))
	self.m_BgBtnTbl[index].m_sSkillName = skillName
	self.m_BgBtnTbl[index].m_uSkillLevel = skillLevel
	self.m_BgBtnTbl[index]:EnableWnd(true)
end

function CBeastPoseWnd:UpdateSkillBtnPicture(skillLevel)
	for i = 1,3 do 
		if self.m_BgBtnTbl[i].m_sSkillName then
			if self.m_BgBtnTbl[i].m_uSkillLevel ~= skillLevel then
				self.m_BgBtnTbl[i].m_uSkillLevel = skillLevel
				self.m_BgBtnTbl[i]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(self.m_BgBtnTbl[i].m_sSkillName,skillLevel))
			end
		end
	end
end

local function DeletePieceInfo(Wnd)
	local tblShortcutPiece = Wnd:GetTblShortcutPiece()
	for i = 1 ,10 do
		local Piece = tblShortcutPiece[i]
		Piece.m_SkillBtn:SetMouseOverDescAfter("")
		Piece.m_SkillBtnNum:SetWndText("")
		Piece.m_SkillStateBtn:ShowWnd(false)	
		Piece:SetIndex(nil)
		g_WndMouse:ClearIconWnd( Piece.m_SkillBtn)
		Piece:UnRegisterSelfTick()
		Piece:UnRegisterSelfCommTick()
		Piece:SetState(EShortcutPieceState.eSPS_None)
		Piece:DelFlashAttention()
	end
end

local function DrawBeastPoToolBar(wnd,Pos,Arg1,Arg2,type)
	if( type == EShortcutPieceState.eSPS_Skill ) then		--表示放的是技能
		g_GameMain.m_MainSkillsToolBar:DrawSkillWnd(wnd, Pos, type, Arg1, Arg2)
	elseif( type == EShortcutPieceState.eSPS_Item ) then	--表示放的是物品
		g_GameMain.m_MainSkillsToolBar:DrawItemWnd(wnd,Pos,type,Arg1,Arg2)
	elseif( type == EShortcutPieceState.eSPS_Array ) then	--表示放的是阵法技能
		g_GameMain.m_MainSkillsToolBar:DrawArrayWnd(wnd, Pos, type, Arg1, Arg2)
	end
end

function CBeastPoseWnd:ClearSkillInfo(skillInfoTbl,skillName)
	for i,v in pairs(skillInfoTbl) do
		for j,k in pairs(v) do
			local Pos = j
			local Arg1 = k[2]
			if Arg1 == skillName then
				Gac2Gas:SaveShortCut(g_Conn, Pos, EShortcutPieceState.eSPS_None, "11", 1, i) 
				skillInfoTbl[i][Pos] = nil
			end
		end	
	end
end

function CBeastPoseWnd:UpdatePosToolBar(SkillName,SkillLevel)
	local tbl = g_GameMain.m_MainSkillsToolBar.m_PieceTbl
	for i,v in pairs(tbl) do
		for j,k in pairs(v) do
			if k[2] == SkillName then
				v[j] = {EShortcutPieceState.eSPS_Skill, SkillName, SkillLevel}
				Gac2Gas:SaveShortCut(g_Conn, j, EShortcutPieceState.eSPS_Skill, SkillName, SkillLevel, i) 
			end 
		end	
	end
end

function CBeastPoseWnd:InitRightPos()
	local wnd = g_GameMain.m_MainSkillsToolBar
	local uCurrentPage = wnd.m_CurrentPage
	for i,v in pairs(wnd.m_PieceTbl) do
		local temp_wnd = nil
		if i == MainSkillWnd then
			temp_wnd = g_GameMain.m_MainSkillsToolBar.m_WndSkill
		elseif i == MasterSkillWnd then
			temp_wnd = g_GameMain.m_MasterSkillArea.m_WndSkill
		elseif i == AddMasterSkillWnd then
			temp_wnd = g_GameMain.m_AddMasterSkillArea.m_WndSkill
		end
		for j,k in pairs(v) do
			local Pos = j
			local type = k[1]
			local Arg1 = k[2]
			local Arg2 = k[3]			
			if uCurrentPage == 1 and Pos < 11 then
				DrawBeastPoToolBar(temp_wnd,Pos,Arg1,Arg2,type)
			elseif uCurrentPage == 2 and Pos > 10 and Pos < 21 then
				Pos = Pos - 10
				DrawBeastPoToolBar(temp_wnd,Pos,Arg1,Arg2,type)
			elseif uCurrentPage == 3 and Pos > 20 then
				Pos =  Pos - 20
				DrawBeastPoToolBar(temp_wnd,Pos,Arg1,Arg2,type)
			end
			g_GameMain.m_MainSkillsToolBar:ResetCoolTimerBtn(Arg1,true)
		end	
	end
end

function CBeastPoseWnd:UpdateBeastPosToolBar()
	local wnd1 = g_GameMain.m_MainSkillsToolBar.m_WndSkill
	local wnd2 = g_GameMain.m_MasterSkillArea.m_WndSkill
	local wnd3 = g_GameMain.m_AddMasterSkillArea.m_WndSkill

	DeletePieceInfo(wnd1)
	DeletePieceInfo(wnd2)
	DeletePieceInfo(wnd3)
	self:InitRightPos()
end


local MagicWarriorTbl = {
							["闪电武器"] = 1,
							["火焰武器"] = 2,
							["冰霜武器"] = 3,
							}
							
local OrcWarriorTbl = {
							["黑豹姿态"] = 1,
							["猛犸姿态"] = 2,
							["雄狮姿态"] = 3
							}
												
function CBeastPoseWnd:InitPosSkillIcon(uClass)
	--魔剑士
	if uClass == EClass.eCL_MagicWarrior then
		self:ShowWnd(true)
		for i,v in pairs(MagicWarriorTbl) do 
			local SmallIcon = SkillPart_Common(i,"SkillSmallIcon")
			g_LoadGrayIconFromRes(SmallIcon, self.m_BgBtnTbl[v], -2, IP_ENABLE, IP_UNCHECK_CLICKDOWN)
			self.m_BgBtnTbl[v]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(i,1))
			self.m_BgBtnTbl[v]:EnableWnd(false)
		end
	--兽人战士
	elseif uClass == EClass.eCL_OrcWarrior then
		self:ShowWnd(true)
		for i,v in pairs(OrcWarriorTbl) do 
			local SmallIcon = SkillPart_Common(i,"SkillSmallIcon")
			g_LoadGrayIconFromRes(SmallIcon, self.m_BgBtnTbl[v], -2, IP_ENABLE, IP_UNCHECK_CLICKDOWN)
			self.m_BgBtnTbl[v]:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(i,1))
			self.m_BgBtnTbl[v]:EnableWnd(false)
		end
	end	
end

