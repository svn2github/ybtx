GMLanCommand = {}
GMLanCommand.Executor = {}

cfg_load "gm/GMCommand_Common"
cfg_load "equip/suit/EquipSuit_Common"
lan_load "scene/Lan_Scene_Basic_Common"
lan_load "npc/Lan_NpcTheaterName_Client"
lan_load "liveskill/Lan_LiveSkillName_Client"
lan_load "liveskill/Lan_Practiced_Common"
lan_load "npc/Lan_Npc_Common"
lan_load "skill/Lan_NonFightSkill_Common"
lan_load "skill/Lan_SkillPart_Common"
lan_load "equip/suit/Lan_EquipSuit_Common"
lan_load "fb_game/Lan_FbActionDirect_Common"
lan_load "gm/Lan_MorePointOne_Common"

local function GetIndex(sDisplayName)
	local Keys = Lan_MorePointOne_Common:GetKeys()
	for k, v in pairs(Keys) do
		if( sDisplayName == Lan_MorePointOne_Common(v,"DisplayNameForGM")) then
			return v
		end
	end
	return nil
end

local function GetRealNameInCommon(tblMH64, sDisplayName)
	if( 0 == #(tblMH64) ) then
		local mh64 = GetIndex(sDisplayName)
		if( not mh64 ) then
			return false, sDisplayName .. "dosen`t exist"
		end
		return true, MemH64ToMbcs(mh64)
	elseif( 1 == #(tblMH64) ) then
		return true, MemH64ToMbcs(tblMH64[1])
	elseif( 1 < #(tblMH64) ) then
		sError = "There are " .. #(tblMH64) .. " real name below, choose one of them and type again"
		for i, v in ipairs(tblMH64) do
			local sDisplayNameForGM = Lan_MorePointOne_Common(v, "DisplayNameForGM")
			if( sDisplayNameForGM == sDisplayName ) then
				return true, MemH64ToMbcs(v)
			end
			if( sDisplayNameForGM ) then
				sError = sError .. "#r" .. sDisplayNameForGM
			else
				sError = sError .. "#r" .. MemH64ToMbcs(v) .. "(Can`t find this word in Lan_MorePointOne_Common)"
			end
		end
		return false, sError
	end
end

--把GM指令中的显示名参数替换成真实名
--@func:将显示名传入返回对应真实名表(真实名可能是多个)
--@tVarTable:GM指令参数表
--@nTransParamIndex:需要转换的参数
--@arg:转换时需要用到的参数索引表
local function Convert(func, tVarTable, nTransParamIndex, ...)
	local arg = {...}
	local tblNeed = {}
	if(#arg > 0) then
		for i, v in ipairs(arg[1]) do
			tblNeed[i] = tVarTable[v]
		end
	else
		tblNeed[1] = tVarTable[nTransParamIndex]
	end
	local bFlag, sErrOrName = GetRealNameInCommon( func(unpack(tblNeed)), tVarTable[nTransParamIndex] )
	if(not bFlag) then
		return false, sErrOrName
	end
	tVarTable[nTransParamIndex] = sErrOrName
	return true, tVarTable
end

---------------------------------------------------------------------
local function GetLanInfoTable(sDisplayName, cfgTable)
	local tblMH64 = {}
	for k, v in pairs(cfgTable) do
		if( v.DisplayName == sDisplayName) then
			table.insert(tblMH64, k)
		end
	end
	return tblMH64
end

local function GetLanInfoTableEx(sDisplayName, cfgTable)
	local tblMH64 = {}
	local keys = cfgTable:GetKeys()
	for i, v in ipairs(keys) do
		if( cfgTable(v, "DisplayName") == sDisplayName ) then
			table.insert(tblMH64, v)
		end
	end
	return tblMH64
end
---------------------------------------------------------------------
local function GetTechRealName(sDisplayName)
	return g_TongTechMgr:GetTechMH64ByDisplayName(sDisplayName)
end

--通过物品大类和物品显示名称得到物品相关信息
local function GetItemInformation(nBigID, sDisplayName)
	return g_ItemInfoMgr:GetItemMH64ByDisplayName( nBigID, sDisplayName )	--tblMH64
end

local function GetSoulPearlInformation(sDisplayName)
	return g_ItemInfoMgr:GetItemMH64ByDisplayName( g_ItemInfoMgr:GetSoulPearlBigID(), sDisplayName )	--tblMH64
end

local function GetQuestInformation(sDisplayName)
	return g_GetQuestNameMH64ByDisplayName( sDisplayName )				--tblMH64
end

local function GetSceneInformation(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_Scene_Basic_Common)		--tblMH64
end

local function GetTheaterInformation(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_NpcTheaterName_Client)	--tblMH64
end

local function GetLiveSkillInformation(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_LiveSkillName_Client)	--tblMH64
end

local function GetLiveSkillPracticeInformation(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_Practiced_Common)		--tblMH64
end

local function GetNPCInformation(sDisplayName)
	local tblMH64 = {}
	for _, k in pairs(Lan_Npc_Common:GetKeys()) do
		if (Lan_Npc_Common(k,"Title") == sDisplayName) then
			table.insert(tblMH64, k)
		end
	end
	return tblMH64
end

local function GetNonFightSkillInformation(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_NonFightSkill_Common)
end

local function GetTalentInformation(sDisplayName)
	return GetLanInfoTable(sDisplayName, loadGeniusfile(g_MainPlayer:CppGetClass(), true))
end

local function GetTalentInformationEx(sDisplayName)
	local tblMH64 = nil
	for nClassID = 1, 9 do
		tblMH64 = GetLanInfoTable(sDisplayName, loadGeniusfile(nClassID, true))
		if( next(tblMH64) ) then
			return tblMH64
		end
	end
	return {}
end

local function GetFightSkillInformation(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_SkillPart_Common)
end

local function GetActionRealName(sDisplayName)
	return GetLanInfoTableEx(sDisplayName, Lan_FbActionDirect_Common)
end

local function GetEquipSuitInformation(sDisplayName)
	local tblMH64 = {}
	local suitNameKeys = EquipSuit_Common:GetKeys()
	for i, v in pairs (suitNameKeys) do
		local suitName = v
		local needPartsCountKeys = EquipSuit_Common:GetKeys(suitName)
		for j, w in pairs (needPartsCountKeys) do
			local needPartCount = w
			local lanIndex = MemH64(string.format("%s%s", suitName, needPartCount))
			local sMH64Name = MemH64(suitName)
			local displaySuitName = Lan_EquipSuit_Common(lanIndex, "DisplayName")
			if displaySuitName == sDisplayName then
				if( next(tblMH64) ) then
					local bFlag = true
					for i, w in ipairs(tblMH64) do
						if(w == sMH64Name) then bFlag = false break end
					end
					if(bFlag) then
						table.insert(tblMH64, sMH64Name)
					end
				else
					table.insert(tblMH64, sMH64Name)
				end
			end
		end
	end
	return tblMH64
end

--下面的指令名称要和服务端对应-----------------------------------------------------------------------

--物品相关
function GMLanCommand.Executor.additem(tVarTable)
	return Convert(GetItemInformation, tVarTable, 2, {1, 2})
end

function GMLanCommand.Executor.delitem(tVarTable)
	return Convert(GetItemInformation, tVarTable, 2, {1, 2})
end

function GMLanCommand.Executor.targetadditem(tVarTable)
	return Convert(GetItemInformation, tVarTable, 3, {2, 3})
end

function GMLanCommand.Executor.targetdelitem(tVarTable)
	return Convert(GetItemInformation, tVarTable, 3, {2, 3})
end

function GMLanCommand.Executor.AddSoulPearl(tVarTable)
	return Convert(GetSoulPearlInformation, tVarTable, 1)
end

function GMLanCommand.Executor.AddIntenEquip(tVarTable)
	local bFlag, tVarTable = Convert(GetItemInformation, tVarTable, 2, {1, 2})
	if(bFlag) then
		return Convert(GetEquipSuitInformation, tVarTable, 3)
	else
		return bFlag, tVarTable
	end
end

--任务相关
function GMLanCommand.Executor.AddQuest(tVarTable)
	return Convert(GetQuestInformation, tVarTable, 1)
end

function GMLanCommand.Executor.FinishQuest(tVarTable)
	return Convert(GetQuestInformation, tVarTable, 1)
end

function GMLanCommand.Executor.TargetAddQuest(tVarTable)
	return Convert(GetQuestInformation, tVarTable, 2)
end

function GMLanCommand.Executor.TargetFinishQuest(tVarTable)
	return Convert(GetQuestInformation, tVarTable, 1)
end

function GMLanCommand.Executor.AddLoopQuest(tVarTable)
	return Convert(GetQuestInformation, tVarTable, 1)
end

--场景相关
function GMLanCommand.Executor.ChangeScene(tVarTable)
	return Convert(GetSceneInformation, tVarTable, 1)
end

function GMLanCommand.Executor.TargetChangescene(tVarTable)
	return Convert(GetSceneInformation, tVarTable, 2)
end

function GMLanCommand.Executor.SceneMaxGrid(tVarTable)
	return Convert(GetSceneInformation, tVarTable, 1)
end

function GMLanCommand.Executor.SetTongTechLevel(tVarTable)
	return Convert(GetTechRealName, tVarTable, 1)
end

function GMLanCommand.Executor.SetCharTechLevel(tVarTable)
	return  Convert(GetTechRealName, tVarTable, 1)
end

--剧场相关
function GMLanCommand.Executor.RunTheater(tVarTable)
	return Convert(GetTheaterInformation, tVarTable, 1)
end

--生活技能相关
function GMLanCommand.Executor.AddLiveskill(tVarTable)
	return Convert(GetLiveSkillInformation, tVarTable, 1)
end

function GMLanCommand.Executor.AddLiveSkillPractice(tVarTable)
	local bFlag, tVarTable = Convert(GetLiveSkillInformation, tVarTable, 1)
	if(bFlag) then
		local bFlag, tVarTable = Convert(GetLiveSkillPracticeInformation, tVarTable, 2)
		tVarTable[2] = string.gsub(tVarTable[2], tVarTable[1], "")
		return bFlag, tVarTable
	else
		return bFlag, tVarTable
	end
end

--NPC相关
function GMLanCommand.Executor.TestNpc(tVarTable)
	return Convert(GetNPCInformation, tVarTable, 1)
end

function GMLanCommand.Executor.AddNpc(tVarTable)
	return Convert(GetNPCInformation, tVarTable, 1)
end

--非战斗技能相关
function GMLanCommand.Executor.AddNonFightSkill(tVarTable)
	return Convert(GetNonFightSkillInformation, tVarTable, 1)
end

--天赋相关
function GMLanCommand.Executor.SetTalent(tVarTable)
	return Convert(GetTalentInformation, tVarTable, 1)
end

function GMLanCommand.Executor.TargetSetTalent(tVarTable)
	return Convert(GetTalentInformationEx, tVarTable, 2)
end

--战斗技能相关
function GMLanCommand.Executor.AddSkillGM(tVarTable)
	return Convert(GetFightSkillInformation, tVarTable, 1)
end

function GMLanCommand.Executor.TargetAddSkill(tVarTable)
	return Convert(GetFightSkillInformation, tVarTable, 2)
end

function GMLanCommand.Executor.SetActionTeamNum(tVarTable)
	return Convert(GetActionRealName, tVarTable, 1)
end

-----------------------------------------------------------------------------------------------------

GMLanCommand.GMLanCommandList = {}
local Keys = GMCommand_Common:GetKeys()
for i, p in pairs( Keys ) do
	local RowInfo = GMCommand_Common(p)
	if(1 == RowInfo("Trans")) then
		local tblInfo = {}
		local func = loadstring("return GMLanCommand.Executor." .. RowInfo("Func"))()
		tblInfo[1] = RowInfo("ParamType")
		tblInfo[2] = func
		tblInfo[3] = RowInfo("Level")
		local newi = string.lower( p )
		GMLanCommand.GMLanCommandList[ newi ] = tblInfo
	end
end
GMCommand_Common = nil
