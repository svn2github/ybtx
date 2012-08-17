cfg_load "skill/SkillPart_Common"
lan_load "skill/Lan_SkillPart_Common"
cfg_load "skill/SkillTreeName_Common"
cfg_load "skill/ClearTalentConsume_Common"
lan_load "skill/Lan_SkillTreeName_Common"

ESkillSeriesKind =
{
	eSSK_First  = 1,
	eSPS_Second  = 2,
	eSPS_Third = 3,
}

---------------技能系别----------------------------------

FightSkillKind =
{
	Skill 			= 1, 	--公共技能
	Talent 			= 2, 	--天赋
	NonFightSkill 	= 3,	--非战斗技能
	NormSkill 		= 4,	--普攻技能
	TalentSkill     = 5,    --天赋技能
}

---------------技能类型----------------------------------

ESkillTreeNode =
{
	eSTN_MainFirst = 1, 
	eSTN_MainSecond = 2, 
	eSTN_MainThird = 3,
	eSTN_MainFourth = 4,
	eSTN_LeftFirst = 5,
	eSTN_LeftSecond = 6,
	eSTN_RightFirst = 7,
	eSTN_RightSecond = 8,
}
--Node排序为从上到下，从左到右

ESkillTalentSeries =
{
	eSTS_Main = 1,
	eSTS_Left = 2,
	eSTS_Right = 3,
}


---------------技能树形ID----------------------------------
ESkillLearnPhase =
{
	eSLP_First = 1, 
	eSLP_Second = 2, 
	eSLP_Third = 3,
	eSLP_Fourth = 4,	
}
---------------------技能学习阶段--------------------------------

local function GetPlayerLevelToSkillLevelTbl()
	local PlayerLevelToSkillLevelTbl = {}
	local PlayerUpSkillConsume = {}
	local PlayerLearnSkillConsume = {}
	local Keys = SkillPart_Common:GetKeys()
	for i ,v  in pairs ( Keys ) do
		if SkillPart_Common(v,"PlayerLevelToSkillLevel") ~= "" then
			local LevelTbl  = loadstring("return" .. SkillPart_Common(v,"PlayerLevelToSkillLevel"))()
			PlayerLevelToSkillLevelTbl[v] = LevelTbl
		else
			PlayerLevelToSkillLevelTbl[v] = {}
		end
		if SkillPart_Common(v,"UpSkillConsume")  ~= "" then
			local UpSkillConsumeTbl = loadstring("return" .. SkillPart_Common(v,"UpSkillConsume"))()
			PlayerUpSkillConsume[v] = UpSkillConsumeTbl
		end
		if SkillPart_Common(v,"LearnSkillConsume") ~= "" then
			local LearnSkillConsume = loadstring("return" .. SkillPart_Common(v,"LearnSkillConsume"))()
			PlayerLearnSkillConsume[v] = LearnSkillConsume
		end
	end
	return PlayerLevelToSkillLevelTbl,PlayerUpSkillConsume,PlayerLearnSkillConsume
end

g_PlayerLevelToSkillLevelTbl,g_PlayerUpSkillConsume,g_PlayerLearnSkillConsume = GetPlayerLevelToSkillLevelTbl()
AddCheckLeakFilterObj(g_PlayerLevelToSkillLevelTbl)
AddCheckLeakFilterObj(g_PlayerUpSkillConsume)
AddCheckLeakFilterObj(g_PlayerLearnSkillConsume)

function GetUpSkillConsume(SkillName,SkillLevel)
	local ConsumeTbl = {}	
	if g_PlayerUpSkillConsume[SkillName] 
		and IsTable(g_PlayerUpSkillConsume[SkillName]) 
		and #(g_PlayerUpSkillConsume[SkillName]) > 1 then
		for i = 1,#(g_PlayerUpSkillConsume[SkillName]) do
			if g_PlayerUpSkillConsume[SkillName][i][1] == SkillLevel then
				ConsumeTbl.MoneyNumber = g_PlayerUpSkillConsume[SkillName][i][2]
				ConsumeTbl.SoulNumber = g_PlayerUpSkillConsume[SkillName][i][3]
				break
			end
		end
	end
	if ConsumeTbl.MoneyNumber == nil then
		error( "升级技能出错技能名称:".. SkillName .. "等级:" .. SkillLevel)
	end
	return ConsumeTbl
end

function GetLearnSkillConsume(SkillName)
	local ConsumeTbl = {}
	ConsumeTbl.MoneyNumber = g_PlayerLearnSkillConsume[SkillName][1]
	ConsumeTbl.SoulNumber = g_PlayerLearnSkillConsume[SkillName][2]
	return ConsumeTbl
end

function GetSkillLevel(SkillName,PlayerLevel)
	local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
	assert(IsTable(LevelTbl),SkillName .. "在配置表SkillPart_Common中PlayerLevelToSkillLevel列填表错误")
	for i = 1,#(LevelTbl) do
		if PlayerLevel >= tonumber(LevelTbl[i][1]) and PlayerLevel <= tonumber( LevelTbl[i][2]) then
			if tostring(LevelTbl[i][3]) == "l" then
				return PlayerLevel ,i
			elseif tonumber( LevelTbl[i][3]) ~= nil then
				return tonumber( LevelTbl[i][3]),i
			end
		end 
	end
end

function GetNodeName(ClassId,SeriesKind)
	local NodeNameTbl = {}
	if ClassId and SeriesKind then
		ClassId = tostring(ClassId)
		SeriesKind = tostring(SeriesKind)
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"FirstPhases"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"SecondPhases"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"ThirdPhasesLeft"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"ThirdPhasesRight"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"FourthPhasesLeft"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"FourthPhasesMid"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"FourthPhasesRight"))
		table.insert(NodeNameTbl,SkillTreeName_Common(ClassId,SeriesKind,"SeriesName"))	
	end
	return NodeNameTbl
end

function GetNodeLanName(ClassId,SeriesKind)
	local NodeNameTbl = {}
	local classIDStr = tostring(ClassId)
	local seriesKindStr = tostring(SeriesKind)
	local classLimitStr = classIDStr .. seriesKindStr
	local info  = Lan_SkillTreeName_Common(MemH64(classLimitStr))
	table.insert(NodeNameTbl,info("DisplayFirstPhases"))
	table.insert(NodeNameTbl,info("DisplaySecondPhases"))
	table.insert(NodeNameTbl,info("DisplayThirdPhasesLeft"))
	table.insert(NodeNameTbl,info("DisplayThirdPhasesRight"))
	table.insert(NodeNameTbl,info("DisplayFourthPhasesLeft"))
	table.insert(NodeNameTbl,info("DisplayFourthPhasesMid"))
	table.insert(NodeNameTbl,info("DisplayFourthPhasesRight"))
	table.insert(NodeNameTbl,info("DisplaySeriesName"))

	return NodeNameTbl
end


local function  GetClearTalentConsumeFun()
	local Keys = ClearTalentConsume_Common:GetKeys()
	local Tbl = {}
	for i, v in pairs (Keys) do
		Tbl[loadstring("return " .. ClearTalentConsume_Common(v,"PlayerLevelScale"))()] = 
				{
					["ConsumeItem"] = loadstring("return " .. ClearTalentConsume_Common(v,"ConsumeItem"))(),
					["ConsumeMoney"] = ClearTalentConsume_Common(v,"ConsumeMoney"),
					["ConsumeSoul"] = ClearTalentConsume_Common(v,"ConsumeSoul"),
				}
	end
	return Tbl
end

local ClearTalentConsumeTbl = GetClearTalentConsumeFun()

function GetClearTalentConsumeItem(PlayerLevel)
	for i , v in pairs (ClearTalentConsumeTbl) do 
		if i[1] <= PlayerLevel and PlayerLevel <= i[2] then
			return v 
		end
	end
end

------------------加载天赋配置表---------------------------
function loadGeniusfile(ClassID, DisplayName)
	if ClassID == 1 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common1"
			return Lan_GiftSkill_Common1
		end
		cfg_load "skill/GiftSkill_Common1"
		return GiftSkill_Common1
	elseif ClassID == 2 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common2"
			return Lan_GiftSkill_Common2
		end
		cfg_load "skill/GiftSkill_Common2"
		return GiftSkill_Common2
	elseif ClassID == 3 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common3"
			return Lan_GiftSkill_Common3
		end
		cfg_load "skill/GiftSkill_Common3"
		return GiftSkill_Common3
	elseif ClassID == 4 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common4"
			return Lan_GiftSkill_Common4
		end
		cfg_load "skill/GiftSkill_Common4"
		return GiftSkill_Common4
	elseif ClassID == 5 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common5"
			return Lan_GiftSkill_Common5
		end
		cfg_load "skill/GiftSkill_Common5"
		return GiftSkill_Common5
	elseif ClassID == 6 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common6"
			return Lan_GiftSkill_Common6
		end
		cfg_load "skill/GiftSkill_Common6"
		return GiftSkill_Common6
	elseif ClassID == 7 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common7"
			return Lan_GiftSkill_Common7
		end
		cfg_load "skill/GiftSkill_Common7"
		return GiftSkill_Common7
	elseif ClassID == 8 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common8"
			return Lan_GiftSkill_Common8
		end
		cfg_load "skill/GiftSkill_Common8"
		return GiftSkill_Common8
	elseif ClassID == 9 then
		if DisplayName then
			lan_load "skill/Lan_GiftSkill_Common9"
			return Lan_GiftSkill_Common9
		end
		cfg_load "skill/GiftSkill_Common9"
		return GiftSkill_Common9
	end
end