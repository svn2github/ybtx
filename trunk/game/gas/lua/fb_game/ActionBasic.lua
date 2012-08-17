cfg_load "fb_game/FbActionDirect_Common"
cfg_load "fb_game/MatchGame_Common"
cfg_load "tong/TongStartTime_Common"
cfg_load "fb_game/JoinCountLimit_Server"
cfg_load "fb_game/JuQingTransmit_Common"

ActionBasic = {}
ActionBasic.ActionInfoTbl = {}
local TongStartTime_Common = TongStartTime_Common
--local ObjNameAndLvStepTbl = {
--	["冰山狼穴抢夺资源点"] = 4,
--	["狂鹫峭壁抢夺资源点"] = 5,
--	["雪岩峭壁抢夺资源点"] = 6,
--	["冻土山道抢夺资源点"] = 7,
--	["龙栖道抢夺资源点"] = 8,
--	["死亡之爪巢穴抢夺资源点"] = 9,
--	["暗鳞氏族前营抢夺资源点"] = 10,
--	["暗鳞氏族祭坛抢夺资源点"] = 11,
--	["火鹫营地抢夺资源点"] = 12,
--	["尖蛇林地抢夺资源点"] = 13,
--	["枯骨墓场抢夺资源点"] = 14,
--	["灰岭丘陵抢夺资源点"] = 15,
--	["暗纹森林抢夺资源点"] = 16,
--	["冷法森林抢夺资源点"] = 17,
--	["腐化沙地抢夺资源点"] = 18,
--	["拉文纳营地抢夺资源点"] = 19,
--	["法潮洼地抢夺资源点"] = 20,
--	["河阳沙滩抢夺资源点"] = 21,
--	["巨鳌战舰抢夺资源点"] = 22,
--	["裂谷坟场抢夺资源点"] = 23,
--	["河阳盗匪营地抢夺资源点"] = 24,
--	["烈火商会营地抢夺资源点"] = 25,
--	["黑衣骑士营地抢夺资源点"] = 26,
--	["幽语废墟抢夺资源点"] = 27,
--	["河阳蒸汽坑道抢夺资源点"] = 28,
--	["北坞伐木场抢夺资源点"] = 29
--	
--}

function ActionBasic:GetActionInfo(actionName)
	if self.ActionInfoTbl[actionName] then
		return self.ActionInfoTbl[actionName]
	end
	local info = {}
	if MatchGame_Common(actionName) then
	 	info["SingleOrTeam"] = MatchGame_Common(actionName, "SingleOrTeam")
		local LimitType = MatchGame_Common(actionName, "JoinLimitType")
		if LimitType ~= "" then
			info["LimitType"] = LimitType
			info["LimitCount"] = JoinCountLimit_Server(LimitType, "Count")
			info["LimitCycle"] = JoinCountLimit_Server(LimitType, "Cycle")
		end
		info["MinLv"] = MatchGame_Common(actionName, "MinLevel")
		info["MaxLv"] = MatchGame_Common(actionName, "MaxLevel")
		
		info["MinTeams"] = MatchGame_Common(actionName, "MinTeams")
		info["MaxTeams"] = MatchGame_Common(actionName, "MaxTeams")
		info["SceneName"] = MatchGame_Common(actionName, "SceneName")
		if MatchGame_Common(actionName, "AutoTeamPlayerNumber") ~= "" then
			info["AutoTeamPlayerNumber"] = tonumber(MatchGame_Common(actionName, "AutoTeamPlayerNumber"))
		end
		local TeamType = MatchGame_Common(actionName, "TeamType")
		if TeamType ~= ""  then
			info["IsCanNoAutoTeam"] = string.sub(TeamType,-1,-1) == "1"
			info["IsCanAutoTeam"] = string.sub(TeamType,-2,-2) == "1"
			info["IsAllMemberJoin"] = string.sub(TeamType,-3,-3) == "1"
		end
		info["IsCheckItem"] = true
		
		info["WaiJoinTime"] = 20
	elseif JuQingTransmit_Common(actionName) then
		info["SingleOrTeam"] = 2
		info["MinLv"] = JuQingTransmit_Common(actionName, "Level")
		info["MaxLv"] = 150
		
		info["MinTeams"] = 1
		info["MaxTeams"] = 1
		info["SceneName"] = JuQingTransmit_Common(actionName, "SceneName")
		info["AutoTeamPlayerNumber"] = 5
		
		local TeamType = "111"
		if TeamType ~= ""  then
			info["IsCanNoAutoTeam"] = string.sub(TeamType,-1,-1) == "1"
			info["IsCanAutoTeam"] = string.sub(TeamType,-2,-2) == "1"
			info["IsAllMemberJoin"] = string.sub(TeamType,-3,-3) == "1"
		end
	elseif actionName == "角斗场" then
		info["SingleOrTeam"] = 1
		local LimitType = FbActionDirect_Common(actionName, "MaxTimes")
		if LimitType ~= "" then
			info["LimitType"] = LimitType
			info["LimitCount"] = JoinCountLimit_Server(LimitType, "Count")
			info["LimitCycle"] = JoinCountLimit_Server(LimitType, "Cycle")
		end
		info["MinLv"] = FbActionDirect_Common(actionName, "MinLevel")
		info["MaxLv"] = FbActionDirect_Common(actionName, "MaxLevel")
		
		info["MinTeams"] = 20
		info["MaxTeams"] = 40
		info["SceneName"] = "角斗场副本"
		
		info["WaiJoinTime"] = 20
	elseif actionName == "竞技场" then
		info["SingleOrTeam"] = 2
		info["IsAllMemberJoin"] = true
		local LimitType = FbActionDirect_Common(actionName, "MaxTimes")
		if LimitType ~= "" then
			info["LimitType"] = LimitType
			info["LimitCount"] = JoinCountLimit_Server(LimitType, "Count")
			info["LimitCycle"] = JoinCountLimit_Server(LimitType, "Cycle")
		end
		info["MinLv"] = FbActionDirect_Common(actionName, "MinLevel")
		info["MaxLv"] = FbActionDirect_Common(actionName, "MaxLevel")
		
		--info["MinTeams"] = 2
		--info["MaxTeams"] = 5
		info["SceneName"] = "竞技场副本"
		
		info["WaiJoinTime"] = 20
--	elseif actionName == "抢夺资源点" then  --佣兵团报名 
--		info["SingleOrTeam"] = 3
--		info["SceneName"] = "抢夺资源点"
--		info["WaiJoinTime"] = TongStartTime_Common("抢夺资源点报名时长", "OpenTime")
	else
		return
	end
	self.ActionInfoTbl[actionName] = info
	return info
end


local LvStepFunTbl = {}
function ActionBasic:GetActionLvStep(actionName, Param)
	if Param == 80 then
		Param = Param -1
	end
	if actionName == "角斗场" then --目前仅 角斗场 分等级段报名
		return math.floor( (Param - 40)/20 )
	elseif actionName == "竞技场" then
		local MatchType = 2
		return MatchType
	elseif MatchGame_Common(actionName) then
		if not LvStepFunTbl[actionName] then
			local str = "return function(L) return (" .. MatchGame_Common(actionName, "LvStep") ..") end"
			LvStepFunTbl[actionName] = loadstring(str)()
		end
		return LvStepFunTbl[actionName](Param)
	end
	return 1
end

----获得
--function ActionBasic:GetRobResActionLvStep(actionName, Param)
--	if Param then
--		for _, charId in pairs(Param) do 
--			local parameters = {}
--			parameters["charId"] = charId
--			local gasRobResDb = RequireDbBox("GasTongRobResourceDB")
--			local objName = gasRobResDb.GetObjName(parameters)
--			local ObjId = ObjNameAndLvStepTbl[objName]
--			return ObjId
--		end
--	end
--end

