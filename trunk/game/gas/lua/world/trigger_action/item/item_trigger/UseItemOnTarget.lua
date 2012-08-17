
function UseItemEffectOnTarget(Conn, ItemInfo, Target)
	local effectName = ItemInfo.effect
	if effectName == "对目标释放技能" then
		DoSkillToTarget(Conn,ItemInfo,Target)
	elseif effectName == "替换" then
		ReplaceTarget(Conn,ItemInfo,Target)
	elseif effectName == "转换" then
		--print("转换脚本已经不存在")
	elseif effectName == "销毁" then
		DestryTarget(Conn,ItemInfo,Target)
	elseif effectName == "随机刷新" then
		local pos = GetCreatePos(Target)
		CreatOnRandomPos(Conn,ItemInfo,pos)
	elseif effectName == "放置" then
		local pos = GetCreatePos(Target)	
		CreatOnPos(Conn,ItemInfo,pos)
	elseif effectName == "放置建筑" then
		LayBuild(Conn, ItemInfo)	
	elseif effectName == "放置临时建筑" then
		LayTempBuild(Conn, ItemInfo)
	elseif effectName == "对地点释放技能" then
		local pos = GetCreatePos(Target)
		DoSkillOnPos(Conn,ItemInfo, pos)
	else
		UseItemEffectOnSelf(Conn, ItemInfo)
	end
end

--对目标释放技能
function DoSkillToTarget(Conn,ItemInfo,Target)

	local function SeccessDoSkill(Player,ItemInfo,pos,Target)		--pos没有用处 只是为了传入Target
		if Target == nil or not IsCppBound(Target) then
			UseItemEnd(Conn,ItemInfo,828)
			return
		end
		local reTarget = Player:GetTarget()
		local TargetName = Target.m_Properties:GetCharName()

		local ArgTbl = ItemInfo.Arg[1]
		local SkillName = ArgTbl[1]
		local SkillLevel = ArgTbl[2] or 1
		
		if not LockItem(Conn,ItemInfo) then			--给物品加锁
			UseItemEnd(Conn,ItemInfo,802)
			return
		end
		Player:SetTarget(Target)
		local res =	Player:PlayerDoItemSkill(SkillName,SkillLevel)
		Player:SetTarget(reTarget)								--对目标使用完技能再设回原来的目标
		if res == EDoSkillResult.eDSR_Success then
			g_MatchGameMgr:AddMatchGameCount(Player, 3, TargetName)
			UseItemOtherList(Conn,ItemInfo,Target)
		elseif res == EDoSkillResult.eDSR_ToTargetHaveBarrier then
			UseItemEnd(Conn,ItemInfo,825,TargetName,ItemInfo.ItemName)		
		elseif res == EDoSkillResult.eDSR_OutOfDistance then
			UseItemEnd(Conn,ItemInfo,826,TargetName,ItemInfo.ItemName)	
		else
			UseItemEnd(Conn,ItemInfo)					
		end
		--print("The Result of Player:PlayerDoItemSkill(SkillName,1) is "..res)
	end
	
	local function FailDoSkill(Player,ItemInfo)
		UseItemEnd(Conn,ItemInfo)
		--print"技能使用不成功"
	end
	
	BeginProgress(Conn,ItemInfo,SeccessDoSkill,FailDoSkill,nil,Target)
end

--销毁
function DestryTarget(Conn,ItemInfo,Target)

	local function SuccessDestry(Player,ItemInfo,pos,Target)
		if Target == nil or not IsCppBound(Target) then
			UseItemEnd(Conn,ItemInfo,828)
			return
		end
		local TargetTbl = ItemInfo.InfluenceTarget
		local TargetType = TargetTbl[1]
		local TargetName = TargetTbl[2]
		
		if TargetType == "Npc" then
			if not LockItem(Conn,ItemInfo) then			--给物品加锁
				UseItemEnd(Conn,ItemInfo,802)
				return
			end
			Target:SetOnDisappear(true)		
			Target = nil
			UseItemOtherList(Conn,ItemInfo)
		elseif TargetType == "Object" then
			if not LockItem(Conn,ItemInfo) then			--给物品加锁
				UseItemEnd(Conn,ItemInfo,802)
				return
			end
			g_IntObjServerMgr:Destroy(Target,true)
			UseItemOtherList(Conn,ItemInfo)
		end
	end
	
	local function FailDestry(Player,ItemInfo)
		UseItemEnd(Conn,ItemInfo)
		--print("销毁物品或npc失败")
	end
	BeginProgress(Conn,ItemInfo,SuccessDestry,FailDestry,nil,Target)
end

function ReplaceTarget(Conn,ItemInfo,Target)

	local function SuccessChange(Player,ItemInfo,pos,Target)
		if Target == nil or not IsCppBound(Target) then
			UseItemEnd(Conn,ItemInfo,828)
			return
		end
		local ChangeTbl	= ItemInfo.Arg[1]
		local ChangeType = ChangeTbl[1]
		local ChangeName = ChangeTbl[2]
		local pos = GetCreatePos(Target)
		local dir = Target:GetActionDir()
		if not LockItem(Conn,ItemInfo) then			--给物品加锁
			UseItemEnd(Conn,ItemInfo,802)
			return
		end
		if ChangeType == "Npc" then
			local npc = UseItemCreateNpc(Conn, ItemInfo.ItemName, ChangeName, pos, dir)
			if npc == nil then
				UseItemEnd(Conn,ItemInfo)
				return
			end
			Target:SetOnDisappear(true)
			UseItemOtherList(Conn,ItemInfo)
		elseif ChangeType == "Object" then
			local Obj = UseItemCreateIntObj(Conn, ChangeName, pos, dir)
			if Obj == nil then
				UseItemEnd(Conn,ItemInfo)
				return
			end
			g_IntObjServerMgr:Destroy(Target,true)
			UseItemOtherList(Conn,ItemInfo)
		end
	end

	local function FailChange(Player,ItemInfo)
		UseItemEnd(Conn,ItemInfo)
		--print"物品或npc替换失败"
	end
	BeginProgress(Conn,ItemInfo,SuccessChange,FailChange,nil,Target)
end