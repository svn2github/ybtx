gac_require "fight/role_status/PlayerFightInfoInc"
gac_require "framework/texture_mgr/DynImageMgr"
cfg_load "image_res/NetworkDelay_Client"

--创建战斗提示信息面板
function CreatePlayerFightInfoWnd( Parent )
	local Wnd = CPlayerFightInfoWnd:new()
	Wnd:CreateFromRes( "FightInfoWnd", Parent )
	Wnd.m_FightInfoShowWnd = Wnd:GetDlgChild("FightInfoShowWnd")

	return Wnd
end

local AllTime = 4000
function CPlayerFightInfoWnd:AddMsg(Msg)
	if Msg == self.m_FightInfoShowWnd:GetWndText() and self.m_FightInfoShowWnd:IsShow() then
		return
	end

	if Msg then
		self:SetTransparent(1)
		self.m_FightInfoShowWnd:SetWndText(Msg)
		self.m_FightInfoShowWnd:ShowWnd(true)
		self:ShowWnd(true)
	end
	self:SetTransparentObj(AllTime,true)
end

-----------------------------------------下面是创建显示网络状态面板-----------------------------------------
--创建显示网络状态面板
function CreateNetworkDelayTimeWnd(Parent)
	local Wnd = CNetworkDelayTimeWnd:new()
	Wnd:CreateFromRes("NetworkDelayTimeWnd",g_GameMain.m_MainSkillsToolBar)
	Wnd:ShowWnd(true)
	return Wnd
end

local function ChangeNetDelayImage(wnd)
	local info	= NetworkDelay_Client(g_GameMain.m_NetDelayImageFlag)
	local rect	= CFRect:new()
	rect.left	= info("ImgX")
	rect.top	= info("ImgY")
	rect.right	= rect.left + info("ImgWidth")
	rect.bottom	= rect.top + info("ImgHeight")
	g_DynImageMgr:AddImageNeedCut(wnd, info("ImgPath"), IP_ENABLE, rect, CFPos:new())	
end

function GetDelayTime()
	if (not g_GameMain) then
		return
	end
	local wnd = g_GameMain.m_NetworkDelayTimeWnd
	if (not wnd) then
		return
	end
	if (not g_Conn) or (not IsCppBound(g_Conn)) then
		return 
	end
	
	--设置延迟时间
	local delayedTime	= math.max(g_Conn:GetLatency(), 0)
	wnd:SetMouseOverDescAfter(GetStaticTextClient(12003, delayedTime))
	
	--更新延迟图标
	local curNetDelayImageFlag = 1	-- delayedTime <= 200
	if delayedTime > 200 and delayedTime <= 500 then
		curNetDelayImageFlag = 2
	elseif delayedTime > 500 then
		curNetDelayImageFlag = 3
	end
	if g_GameMain.m_NetDelayImageFlag ~= curNetDelayImageFlag then
		g_GameMain.m_NetDelayImageFlag = curNetDelayImageFlag
		ChangeNetDelayImage(wnd)
	end
end
