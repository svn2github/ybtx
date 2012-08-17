gas_require "world/scene_mgr/SceneType/YbEducateActionSceneInc"
--训练塔副本

--判断是否退出该副本，删掉场景的时间
function CYbEducateActionScene:LeaveScene()
	-- 10分钟后删除场景	
	local function CloseScene(Tick)
		if self.m_CloseYbActionTick then
			UnRegisterTick(self.m_CloseYbActionTick)
			self.m_CloseYbActionTick = nil
		end
		self:Destroy()
	end
		
	if self.m_MercenaryActionInfo.PlayerOffLine then   								-- 10分钟后删除
		if self.m_CloseYbActionTick then
			UnRegisterTick(self.m_CloseYbActionTick)
			self.m_CloseYbActionTick = nil
		end
		self.m_MercenaryActionInfo.PlayerOffLine = nil
		self.m_CloseYbActionTick = RegisterTick("CloseYBActionTick", CloseScene, 10*60*1000)	
	else																						-- 直接删除
		CloseScene(nil, self)
	end
end


function CYbEducateActionScene:OnDestroy()
	CMercenaryRoomCreate.ClearAllTick(self)
	
	if self.m_CloseYbActionTick then
		UnRegisterTick(self.m_CloseYbActionTick)
		self.m_CloseYbActionTick = nil
	end
end

function CYbEducateActionScene:OnPlayerDeadInScene(Attacker, BeAttacker)
	CMercenaryRoomCreate.PlayerDeadInYbActFb(Attacker, BeAttacker)
end

function CYbEducateActionScene:OnPlayerChangeOut(Player)
	CMercenaryEducateAct.ChangeSceneYbActFb(Player, self.m_SceneId)
end

function CYbEducateActionScene:OnPlayerChangeIn(Player)
--	if IsCppBound(Player) then
		CMercenaryEducateAct.IntoYbActFbScene(Player)
		--若其他任务有完成修行塔的事件需求，加佣兵任务计数
		CScenePkMgr.ChangePkState(Player)
		AddVarNumForTeamQuest(Player, "完成修行塔", 1)
		--AddMercenaryLevelCount(Player.m_Conn, "修行塔")
--	end
end

function CYbEducateActionScene:OnPlayerLogIn(Player)
	--加下面这个判断,是因为玩家在切多服进副本时
	--如果连接失败的话,场景上没有这个对象,说明玩家还没有进过副本
	--这种情况下,让他执行进入的接口,而不是登录的接口
	CScenePkMgr.ChangePkState(Player)
	if self.m_MercenaryActionInfo then
		CMercenaryEducateAct.PlayerLoginYbEducationAction(Player)
	else
		self:OnPlayerChangeIn(Player)
	end
end

function CYbEducateActionScene:OnPlayerLogOut(Player)
	CMercenaryEducateAct.PlayerOffLineYbActFb(Player)
end
