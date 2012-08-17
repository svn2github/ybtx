EProgressBreak = 
{
	ePB_Attack				= 1,		--攻击
	ePB_Move					= 2,		--移动
	ePB_OpenCSM				= 3,		--打开寄售交易所
	ePB_OpenShop			= 4,		--打开商店
	ePB_OpenMailbox		= 5,		--打开邮箱
	ePB_Esc						= 6,		--按ESC键
}


--设置玩家状态
local function ProgressSetCtrlState(Player, state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, state)
	Player:SetInGatherProcess(state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, state)
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, state)
	
	local actionState = state and EActionState.eAS_Item_Sing or EActionState.eAS_Item_Cast
	Player:SetAndSyncActionState(actionState)
end

--通用读条
--@param Time 读条时间长度 以毫秒为单位
--@param SuccFun 读条成功后调用的函数
--@param FailFun 读条被打断,失败后调用的函数
--@param data SuccFun(data) FailFun(data) 
--@return 1、正在读条中, 2、正在释放技能中, 3、拾取物品中
function CommLoadProgress(Player, Time, SuccFun, FailFun, data)
	if not IsCppBound(Player) then
		return
	end
	--读条之前的判断
	if Player.m_CommLoadProTick then
		return false, 1
	end
	if Player:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
		return false, 2
	end
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		return false, 3
	end
	
	--开始读条
	local function DoProgressTick(Tick, data)
		if not IsCppBound(Player) then
			return
		end
		Player:DelListeningWnd()
		if not data["bBreakMove"] then
			ProgressSetCtrlState(Player, false)
			Player.m_SetCtrlState = 1
		end
		Gas2Gac:NotifyStopProgressBar(Conn)
		UnRegisterTick(Player.m_CommLoadProTick)
		Player.m_CommLoadProTick = nil
		SuccFun(data)
	end
	data["FailFun"] = FailFun
	Player:CancelNormalAttack()
	if not data["bBreakMove"] then
		ProgressSetCtrlState(Player, true)
	end
	Gas2Gac:ResourceLoadProgress(Player.m_Conn, Time, data["ActionName"] or "")
	Player:AddListeningWnd()
	Player.m_CommLoadProTick = RegisterTick("LoadProgressTick", DoProgressTick, Time, data)
	return true
end

--读条被打断时调用
function CommStopLoadProgress(Player, BreakType)
	local data = Player.m_CommLoadProTick.m_TickArg[1]
	local BreakTypeTbl = data["BreakType"]
	if IsTable(BreakTypeTbl) then
		for i, v in pairs(BreakTypeTbl) do
			if BreakType == v then
				return
			end
		end
	end
	local FailFun = data["FailFun"]
	if IsFunction(FailFun) then
		FailFun(data)
	end
	if not IsCppBound(Player) then
		return
	end
	UnRegisterTick(Player.m_CommLoadProTick)
	Player.m_CommLoadProTick = nil
	Player:DelListeningWnd()
	if not data["bBreakMove"] then
		ProgressSetCtrlState(Player, false)
		Player.m_SetCtrlState = 2
	end
	Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
end

function Gac2Gas:StopLoadProgress(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	if Player.m_CommLoadProTick then
		CommStopLoadProgress(Player, EProgressBreak.ePB_Esc)
	elseif Player.m_CollectResTick then
		TongStopLoadProgress(Player)
	end
end
