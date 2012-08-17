gac_require "toolbar/servant_skill_area/servant_skillInc"
gac_require "toolbar/short_cut/ShortcutInc"
cfg_load "skill/ServantSkill_Common"
cfg_load "skill/SkillPart_Common"
lan_load "skill/Lan_SkillPart_Common"

local MaxNumber = 6 
function CreateServantSkill(Parent)
	local Wnd = CServantSkill:new()
	Wnd:CreateFromRes("ServantSkillWnd",Parent)
	Wnd.m_ServantSkillList = ClassCast(CShortcut,Wnd:GetDlgChild("ServantSkillList"))
	Wnd.m_ServantSkillList:Init(MaxNumber, false, false,0)
	for i = 1,MaxNumber do
		local Piece = Wnd.m_ServantSkillList.m_tblShortcutPiece[i]
		Piece.m_SkillBtn:SetWndText("")
	end 
	return Wnd
end

--技能栏事件响应
function CServantSkill:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == ITEM_LBUTTONCLICK and Child == self.m_ServantSkillList then
		local idx = uParam1+1
		local Piece = self.m_ServantSkillList.m_tblShortcutPiece[idx] 
		if Piece:GetState() == EShortcutPieceState.eSPS_Skill then
			local fs = Piece:GetSkillContext()
			g_MainPlayer:FightSkill( fs.SkillName, tbl.Level)
		end
	end
end

-- 得到召唤兽的技能
local function GetServantSkill(ServantName)
	local Keys = ServantSkill_Common:GetKeys()
	for i ,p in pairs (Keys) do
		if p == ServantName then
			local RowInfo = ServantSkill_Common(p)
			return RowInfo("SkillName")
		end
	end	
	return nil
end

--添加技能
function CServantSkill:AddServantSkill(ServantName)
	local skillName = GetServantSkill(ServantName)
	if skillName == nil then
		return
	end
	for i = 1,MaxNumber do
		local Piece = self.m_ServantSkillList.m_tblShortcutPiece[i]
		if skillName ~= "" and 
		Piece:GetState() == EShortcutPieceState.eSPS_None   then
			local tbl = {} 
			tbl.Level = 0
			tbl.SkillName  = skillName
			Piece:SetSkillContext(tbl)
			Piece:SetName(skillName)
			Piece:SetState(EShortcutPieceState.eSPS_Skill)
			local CoolTime = Piece:GetCoolTime()
			local LeftTime = g_MainPlayer:GetSkillLeftCoolDownTime(tbl.SkillName)
			Piece:RegisterSelfTick(CoolTime,CoolTime - LeftTime)	
			self:DrawWnd(Piece)
			break
		end
	end
end

--删除召唤兽技能
function CServantSkill:DelServantSkill(ServantName)
	local Piece = self:FindServantSkill(ServantName)
	if Piece == nil then
		return
	end
	Piece:SetState(EShortcutPieceState.eSPS_None)
	g_WndMouse:ClearIconWnd(Piece.m_SkillBtn)
	local pos = Piece:GetPos()
	self.m_ServantSkillList:GetSubItem(0,pos-1):SetMouseOverDescAfter("")
	self:MoveServantSkill(pos)
end

--查找召唤兽对应的技能
function CServantSkill:FindServantSkill(ServantName)
	local skillName = GetServantSkill(ServantName)
	if skillName then
		return
	end
	for i = 1 ,MaxNumber do
		local Piece = self.m_ServantSkillList.m_tblShortcutPiece[i]
		local tbl = Piece:GetSkillContext() 
		if Piece:GetState() == EShortcutPieceState.eSPS_Skill 
			and  tbl.SkillName == skillName then
			return Piece
		end
	end
end

--向前移动召唤兽技能
function CServantSkill:MoveServantSkill(pos)
	local List = self.m_ServantSkillList.m_tblShortcutPiece
	for i = pos ,MaxNumber do 	
		if List[i + 1 ]:GetState() == EShortcutPieceState.eSPS_Skill then
			List[i]:SetSkillContext(List[i + 1]:GetSkillContext())
			List[i]:SetState( EShortcutPieceState.eSPS_Skill)
			local tbl = List[i]:GetSkillContext()
			local CoolTime = List[i]:GetCoolTime()
			local LeftTime = g_MainPlayer:GetSkillLeftCoolDownTime(tbl.SkillName)
			List[i]:RegisterSelfTick(CoolTime,CoolTime - LeftTime)
			
			List[i+1]:SetState( EShortcutPieceState.eSPS_None) 
		else
			List[i]:UnRegisterSelfTick()
			List[i]:UnRegisterSelfCommTick()
			List[i].m_SkillStateBtn:ShowWnd(false)
			g_WndMouse:ClearIconWnd( List[i].m_SkillBtn)
			self.m_ServantSkillList:GetSubItem(0,i-1):SetMouseOverDescAfter("")
			if i == 1 then
				self:ShowWnd(false)
			end
			break
		end
	end
	for i = 1, MaxNumber do
		self:DrawWnd(List[i])   
	end
end

--加载图片和设置ToolTips
function CServantSkill:DrawWnd(Piece)
	if Piece:GetState() == EShortcutPieceState.eSPS_Skill then
		local tbl = Piece:GetSkillContext()
		local ItemInfo
		for i,p in pairs(SkillPart_Common) do
			if SkillPart_Common(p,"SkillName") == tbl.SkillName then
				ItemInfo = SkillPart_Common(p)	
				break
			end
		end
		g_LoadIconFromRes(ItemInfo("SkillSmallIcon"), Piece.m_SkillBtn, -1, IP_ENABLE, IP_MOUSEOVER)
		local pos = Piece:GetPos()
		self.m_ServantSkillList:GetSubItem(0,pos-1):SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(ItemInfo("SkillName"),0))
	end
end

--删除召唤兽技能
function Gas2Gac:DelServantSkill(Conn,ServantName)
--	if g_GameMain.m_ServantSkill ~= nil then
--		g_GameMain.m_ServantSkill:DelServantSkill(ServantName)
--	end
end

--添加召唤兽技能
function Gas2Gac:CreateServantSkill(Conn,ServantName)
--	if g_GameMain.m_ServantSkill == nil then
--		g_GameMain.m_ServantSkill =  CreateServantSkill(g_GameMain) 
--	end
--	if not g_GameMain.m_ServantSkill:IsShow() then
--		g_GameMain.m_ServantSkill:ShowWnd(true)
--	end
--	g_GameMain.m_ServantSkill:AddServantSkill(ServantName)
end

