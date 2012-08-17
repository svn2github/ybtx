QuestState = {
	init	= 1,
	failure	= 2,
	finish	= 3
}

-----判断任务是否可接     begin
--判断人物是否满足任务的阵营需求 参数：人物阵营,任务名
function IfAcceptQuestCamp(Player, questname)
	local camp = Player:CppGetCamp()
	local camplimit = Quest_Common(questname, "阵营限制")
	if camplimit and camplimit~=0	and camplimit~=camp then
		return false
	end
	return true
end

function IfAcceptQuestClass(Player, questname)
	local classTab = {
		
		["无职业"] = EClass.eCL_NoneClass,
		["大剑士"] = EClass.eCL_Warrior,
		["魔剑士"] = EClass.eCL_MagicWarrior,
		["骑士"] = EClass.eCL_Paladin,
		["法师"] = EClass.eCL_NatureElf,
		["邪魔"] = EClass.eCL_EvilElf,
		["祭司"] = EClass.eCL_Priest,
		["矮人骑士"] = EClass.eCL_DwarfPaladin,
		["精灵弓箭手"] = EClass.eCL_ElfHunter,
		["兽人战士"] = EClass.eCL_OrcWarrior
	}
	
	local class = Player:CppGetClass()
	local subject = GetCfgTransformValue(true, "Quest_Common", questname, "职业限制")
	if subject then
		for i = 1, table.maxn(subject) do 
			for j = 1, table.maxn(subject[i]) do
				local sub = subject[i][j]
				if class == classTab[sub] then
					return true
				end
			end
		end
	else
		return true
	end
end

--判断人物是否满足接任务的最小等级 参数：人物等级,任务名
function IfAcceptQuestMinLev(Player, questname)
	local lev = Player:CppGetLevel()
	local minlevel = Quest_Common(questname, "接任务最小等级")
	if (minlevel and minlevel ~= 0 and lev < minlevel) then
		return
	end
	return true
end

function IfAcceptQuestMaxLev(Player, questname)
	local lev = Player:CppGetLevel()
	local maxlevel = Quest_Common(questname, "接任务最高等级")
	if (maxlevel and maxlevel ~= 0 and lev > maxlevel) then
		return
	end
	return true
end


--判断接任务时玩家当前所在地图是否正确
function IfAcceptQuestMap(scenename, questname)
--	if Quest_Common(questname, "触发任务方式", "HyperLink") ~= scenename then
--		return false
--	end
	return true
end
--判断交任务时玩家当前所在地图是否正确
function IfFinishQuestMap(scenename, questname)
--	if Quest_Common(questname, "结束任务方式", "HyperLink") ~= scenename then
--		return false
--	end
	return true
end
--判断人物所在区域是否满足接任务的区域 参数：人物当前位置,npc位置
function IfAcceptQuestArea(Player, Pos)

	if not IsCppBound(Player) then
		return false
	end
	local pos1 = CPos:new()
	local pos2 = Pos
	Player:GetGridPos( pos1 )
	return math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )<=5
end
-----判断任务是否可接     end

-----判断任务是否可提交     begin
--判断角色所在位置是否符合提交条件
function IfCanFinishArea(Player, Pos)
	local pos1 = CPos:new()
	local pos2 = Pos
	Player:GetGridPos( pos1 )
	return math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )<=5
end

function IsPasserbyCamp(Camp, PlayerCamp, GameCamp, PlayerGameCamp)
	if PlayerGameCamp == 0 or GameCamp == 0 then  --一方为真实阵营,比较真实阵营
		if Camp == ECamp.eCamp_Passerby or Camp == ECamp.eCamp_Specail or Camp == PlayerCamp then
			return true
		end
		return false
	else
		if GameCamp == PlayerGameCamp then
			return true
		end
		return false
	end
end
