--设置玩家状态
local function ProgressSetCtrlState(Player, state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, state)
	Player:SetInGatherProcess(state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, state)
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, state)
end

--帮会战通用读条
--@param Time 读条时间长度 以毫秒为单位
--@param SuccFun 读条成功后调用的函数
--@param FailFun 读条被打断,失败后调用的函数
--@param data SuccFun(data) FailFun(data) 
function TongLoadProgress(Player, Time, SuccFun, FailFun, data)
	if Player.m_CollectResTick then
		return false, 1
	end
	if Player:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
		return false, 2
	end
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		return false, 3
	end
	local function DoProgressTick(data)
		if not IsCppBound(Player) then
			return
		end
		Player:DelListeningWnd()
		ProgressSetCtrlState(Player, false)
		Player.m_SetCtrlState = 3
		Gas2Gac:NotifyStopProgressBar(Conn)
		UnRegisterTick(Player.m_CollectResTick)
		Player.m_CollectResTick = nil
		SuccFun(data)
	end
	data["FailFun"] = FailFun
	Player:CancelNormalAttack()
	ProgressSetCtrlState(Player, true)
	Gas2Gac:ResourceLoadProgress(Player.m_Conn, Time, data["ActionName"] or "")
	Player:AddListeningWnd()
	Player.m_CollectResTick = RegisterTick("TongLoadProgressTick", DoProgressTick, Time, data)
	return true
end

--读条被打断时调用
function TongStopLoadProgress(Player)
	local data = Player.m_CollectResTick.m_TickArg[1]
	local FailFun = data["FailFun"]
	if IsFunction(FailFun) then
		FailFun(data)
	end
	if not IsCppBound(Player) then
		return
	end
	if Player.m_CollectResTick then
		UnRegisterTick(Player.m_CollectResTick)
		Player.m_CollectResTick = nil
		Player:DelListeningWnd()
		ProgressSetCtrlState(Player, false)
		Player.m_SetCtrlState = 4
		Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
	end
end

--帮会战掉落资源
--@param Scene, x, y 创建掉落的资源相应地图及坐标
--@param ResNum 掉落的资源数
--@param ResFoldLimit 每堆最大资源数
function DropResource(Scene, x, y, ResNum, ResFoldLimit)
	if not ResNum then
		LogErr("帮会资源掉落错误!","掉落的资源数量为nil")
		return
	end
	if ResNum <= 0 then
		return
	end
	local Pos = CPos:new()
	Pos.x = x / EUnits.eGridSpanForObj
	Pos.y = y / EUnits.eGridSpanForObj
	if ResFoldLimit and ResFoldLimit ~= 0 then
		while ResNum > 0 do
			ResNum = ResNum - ResFoldLimit
			local ResObj = CreateIntObj(Scene, RandomCreatPos(Pos,3), "散落的资源", nil, nil)
			if ResNum > 0 then
				ResObj.m_Properties:SetResourceCount(ResFoldLimit)
			else
				ResObj.m_Properties:SetResourceCount(ResFoldLimit+ResNum)
			end
		end
	else
		local ResObj = CreateIntObj(Scene, RandomCreatPos(Pos,1), "散落的资源", nil, nil)
		ResObj.m_Properties:SetResourceCount(ResNum)
	end
end

--创建随机坐标 pos为中心range的范围内
function RandomCreatPos(pos,range)
	local bContinue = true
	local x,y
	while bContinue do
		x = math.random(-range, range)
	 	y = math.random(-range, range)
	 	if x ~= 0 or y ~= 0 then
			bContinue = false
		end
	end
	local CreatPos = CPos:new()
	CreatPos.x = pos.x + x
	CreatPos.y = pos.y + y
	return CreatPos
end