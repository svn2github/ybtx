--gac_require "relation/tong/TongShowRulesWndInc"
CShowRulesWnd = class(SQRDialog)

local ChallengeTypeTbl = {}
ChallengeTypeTbl["国战介绍"] = 160002
ChallengeTypeTbl["挑战介绍"] = 160003
ChallengeTypeTbl["资源领取介绍"] = 160004
ChallengeTypeTbl["抢夺资源点介绍"] = 160005
ChallengeTypeTbl["怪物攻城介绍"] = 160006
ChallengeTypeTbl["国贡领取介绍"] = 160007
ChallengeTypeTbl["击杀积分介绍"] = 160008


function CShowRulesWnd:Ctor(parent)
	self:CreateFromRes("DisplayRules",parent)
	self.m_ShowText = self:GetDlgChild("ShowText")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_CloseWndBtn = self:GetDlgChild("CloseWndBtn")
	self:ShowWnd(false)
end

function CShowRulesWnd:GetWnd(Type)
	if not g_GameMain.m_ChallengeIntroduce then
		g_GameMain.m_ChallengeIntroduce = CShowRulesWnd:new(g_GameMain)
	end
	local wnd = g_GameMain.m_ChallengeIntroduce
	local msg = ChallengeTypeTbl[Type]
	wnd.m_ShowText:SetWndText(GetStaticTextClient(msg)) 
	wnd:ShowWnd(true)
	return wnd	
end

--function CreateTongShowRulesWnd(parent, Type)
--	local Wnd = CShowRulesWnd:new()
--	Wnd:CreateFromRes("DisplayRules",parent)
--	Wnd.m_ShowText = Wnd:GetDlgChild("ShowText")
--	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
--	Wnd.m_CloseWndBtn = Wnd:GetDlgChild("CloseWndBtn")
--	local msgId = ChallengeTypeTbl[Type]
--	Wnd.m_ShowText:SetWndText(GetStaticTextClient(msgId)) 
--	Wnd:ShowWnd(true)
--	
--	return Wnd	
--end

function CShowRulesWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if(Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_CloseWndBtn) then
			self:ShowWnd(false)
		end
	end
end
