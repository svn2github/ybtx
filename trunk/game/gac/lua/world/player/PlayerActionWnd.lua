gac_require "world/player/PlayerActionWndInc"
--cfg_load_single "action/FaceAction_Common"
--
local function ShowParam(Wnd, Rect)
	--Wnd.m_BtnTbl = {}
--	for i = 1, table.maxn(FaceAction_Common) do
--		local param = WndCreateParam:new()
--		param.uStyle = 0x40400000
--		param.width = 34
--		param.height = 34
--		param:SetParentWnd(Wnd)
--		param.font_size = 14
--		if (i%2) == 1 then
--			param.x = 20
--			param.y = 35 + (math.ceil(i/2) - 1 ) * 52 
--			Rect.bottom = param.y + 34
--		elseif (i%2) == 0 then
--			param.x = 100 
--			param.y = 35 + (math.ceil(i/2) - 1 ) * 52 
--			Rect.bottom = param.y + 34 
--		end
--		
--		local ActionBtn = SQRButton:new()
--		ActionBtn:Create(param)
--		ActionBtn.m_Icon = FaceAction_Common[i].SmallIcon
--		ActionBtn.m_FacialAction = FaceAction_Common[i].FaceActionName
--		ActionBtn.m_ActionName = FaceAction_Common[i].StartActionName
--		ActionBtn.m_Special = FaceAction_Common[i].StartSpecialName
--		ActionBtn.m_ContinuanceSpecial = FaceAction_Common[i].ContinuanceSpecialName
--		ActionBtn.m_Continuance = FaceAction_Common[i].ContinuanceActionName
--		ActionBtn.m_IsActive = true
--		g_LoadIconFromRes(ActionBtn.m_Icon , ActionBtn, -2, IP_ENABLE, IP_CLICKDOWN)
--		ActionBtn:SetMouseOverDescAfter(FaceAction_Common[i].Tooltip)
--		table.insert(Wnd.m_BtnTbl, ActionBtn)
--	end
end
--
function CreatePlayerActionWnd(Parent)
	local Wnd = CPlayerActionWnd:new()
	Wnd:CreateFromRes("WndAction", Parent)
	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
	local Rect = CFRect:new()
	local ParentRect = CFRect:new()
	ShowParam(Wnd, Rect)
	Wnd:GetLogicRect(ParentRect)
	ParentRect.bottom = ParentRect.top + Rect.bottom + 10
	Wnd:SetLogicRect(ParentRect)
	return Wnd
end

function CPlayerActionWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
--		if Child == self.m_CloseBtn then
--			self:ShowWnd(false)
--		else
--			for i = 1, table.maxn(FaceAction_Common) do
--				if Child == self.m_BtnTbl[i] and g_MainPlayer then
--					self:ShowWnd(false)
--					local Target = g_MainPlayer.m_LockCenter.m_LockingObj or g_MainPlayer.m_LockCenter.m_LockingIntObj or g_MainPlayer
--					local IsMoving = g_MainPlayer:IsMoving()
--					if not Target then
--						MsgClient(3100)
--						return
--					end
--					if IsMoving then
--						MsgClient(3101)
--						return
--					end
--					if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
--						MsgClient(3102)   ---不能在战斗状态下使用动作表情
--						return
--					else 
--						g_MainPlayer:DoFacialAction(Target:GetEntityID(), Child.m_FacialAction, Child.m_ActionName, Child.m_Continuance, Child.m_Special, Child.m_IsActive, Child.m_ContinuanceSpecial)
--					
--					end
--				end
--			end
--		end
	end
end

