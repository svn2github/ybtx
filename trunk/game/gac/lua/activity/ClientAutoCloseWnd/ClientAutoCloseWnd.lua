local AutoCloseWndTbl = {}

CAutoCloseWnd = class()

local function CheckIsInTbl(Wnd)
	for _, wnd in pairs(AutoCloseWndTbl) do
		if wnd == Wnd then
			return
		end
	end
	AutoCloseWndTbl[Wnd] = Wnd
end

function CAutoCloseWnd:ClearAutoCloseWndTick()
	--print("function ClearAutoCloseWndTick()")
	for _, Wnd in pairs(AutoCloseWndTbl) do
		if Wnd.m_CheckIsAutoCloseWndTick then
--			print("ClearAutoCloseWndTick")
			UnRegisterTick(Wnd.m_CheckIsAutoCloseWndTick)
			Wnd.m_CheckIsAutoCloseWndTick = nil
		end
	end
	AutoCloseWndTbl = {}
end

function CAutoCloseWnd:StopAutoCloseWnd(Wnd)
	if Wnd.m_CheckIsAutoCloseWndTick then
		UnRegisterTick(Wnd.m_CheckIsAutoCloseWndTick)
		Wnd.m_CheckIsAutoCloseWndTick = nil
	end
	if AutoCloseWndTbl[Wnd] then
		AutoCloseWndTbl[Wnd] = nil
	end
end

local function CloseCurWnd(Wnd)
	if Wnd == g_GameMain.m_WndMainBag then
		if g_WndMouse:GetCursorState() == ECursorState.eCS_BreakItem then
				g_WndMouse:ClearCursorSpecialState()
		end
		Wnd:ShowWnd(false)
		if g_WndMouse:GetCursorState() == ECursorState.eCS_BreakItem then
					g_WndMouse:ClearCursorSpecialState()
		end
		if Wnd.m_CheckIsAutoCloseWndTick then
			UnRegisterTick(Wnd.m_CheckIsAutoCloseWndTick)
			Wnd.m_CheckIsAutoCloseWndTick = nil
		end
	else
		Wnd:ShowWnd(false)
	end
	if AutoCloseWndTbl[Wnd] then
		AutoCloseWndTbl[Wnd] = nil
	end
end

--在切场景后,要关闭的窗口
function CAutoCloseWnd:TransferCloseAutoCloseWnd()
	for _, Wnd in pairs(AutoCloseWndTbl) do
		CloseCurWnd(Wnd)
	end
	AutoCloseWndTbl = {}
end

function CAutoCloseWnd:AutoCloseWnd(Wnd)
	local function CheckFun()
		if IsCppBound(g_MainPlayer) then
			if g_MainPlayer:IsMoving() or not g_MainPlayer:CppIsAlive() then
				CloseCurWnd(Wnd)
				return
			end
			
			if not Wnd:IsShow() then--因为窗口都已经隐藏了,那就消掉Tick
				if Wnd.m_CheckIsAutoCloseWndTick then
					UnRegisterTick(Wnd.m_CheckIsAutoCloseWndTick)
					Wnd.m_CheckIsAutoCloseWndTick = nil
				end
				return
			end
			
			if Wnd.m_TargetNpcID then
				local TargetEntityID = Wnd.m_TargetNpcID
				local Npc = CCharacterFollower_GetCharacterByID(TargetEntityID)
				local IntObjClient = CIntObjClient_GetIntObjByID(TargetEntityID)
				local QuestObj = Npc or IntObjClient
				
				if not QuestObj or g_GetDistance(QuestObj, g_MainPlayer) > 6 then
					CloseCurWnd(Wnd)
				end
				
				if Npc and not Npc:CppIsAlive() then
					CloseCurWnd(Wnd)
				end
				
			end
		else
			if Wnd.m_CheckIsAutoCloseWndTick then
				UnRegisterTick(Wnd.m_CheckIsAutoCloseWndTick)
				Wnd.m_CheckIsAutoCloseWndTick = nil
			end
		end
	end
	if Wnd.m_CheckIsAutoCloseWndTick then
		UnRegisterTick(Wnd.m_CheckIsAutoCloseWndTick)
		Wnd.m_CheckIsAutoCloseWndTick = nil
	end
	Wnd.m_CheckIsAutoCloseWndTick = RegisterTick("CheckFun", CheckFun, 100)
	CheckIsInTbl(Wnd)
end
