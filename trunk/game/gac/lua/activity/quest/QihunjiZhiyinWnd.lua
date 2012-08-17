
CQihunjiZhiyinWnd = class(SQRDialog)

function CQihunjiZhiyinWnd:Ctor()
	self:CreateFromRes( "QihunjiZhiyinWnd", g_GameMain )
end

function CQihunjiZhiyinWnd:RetShowQihunjiZhiyinWnd(IsShow)
	local function OnTick()
		g_GameMain.m_QihunjiZhiyinWnd:RetShowQihunjiZhiyinWnd(false)
	end
	
	if self.m_CloseTick then
		UnRegisterTick(self.m_CloseTick)
		self.m_CloseTick = nil
	end
	
	if IsShow then
		self.m_CloseTick = RegisterTick("QihunjiZhiyinTick", OnTick, 20000)
	end
	
	self:ShowWnd(IsShow)
end

--CCharacterDirectorCallbackHandler:OnDead(uEntityID)
--PlayerMoveChangeAreaInfo()
--这俩函数里有关于区域“剑魂领域”的判断
