cfg_load "fb_game/MatchGame_Common"
gas_require "fb_game/ActionBasic"
cfg_load "fb_game/JuQingTransmit_Common"
cfg_load "fb_game/PVP_ActionRule_Server"
--CallDbTrans时候统一所有都加上 活动名 频道,

local MatchGame_Common = MatchGame_Common
local JuQingTransmit_Common = JuQingTransmit_Common
local PVP_ActionRule_Server = PVP_ActionRule_Server
local ActionBasic = ActionBasic
local os = os

local StmtOperater = class()

local StmtDef = 
{
	"GetCharNameById",
	"select c_sName from tbl_char where cs_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"FindPlayer",
	[[
	select 
		at.at_uId,at.ar_uId 
	from 
		tbl_action_team_member as atm, tbl_action_team as at, tbl_action_room as ar
	where 
		atm.at_uId = at.at_uId and at.ar_uId = ar.ar_uId and atm.cs_uId = ? and ar.ar_sActionName = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"IsInMatchList",
	[[
	select 
		count(*)
	from 
		tbl_action_match_list
	where 
	 cs_uId = ?	and aml_sActionName = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"InsertMatchList",
	[[
	insert into tbl_action_match_list(cs_uId, aml_sActionName, aml_uLvStep, aml_uGroupId, aml_uWaitingTeammate) values(?,?,?,?,1)
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"DeleteMatchList",
	[[
	delete from tbl_action_match_list where cs_uId = ? and aml_sActionName = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetPlayerWaitingInfo",
	[[
	select aml_uGroupId from tbl_action_match_list where cs_uId = ? and aml_sActionName = ? and aml_uWaitingTeammate = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetWaitingGroupMember",
	[[
	select cs_uId from tbl_action_match_list where aml_sActionName = ? and aml_uGroupId = ? and aml_uWaitingTeammate = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"DeleteWaitingGroup",
	[[
	delete from tbl_action_match_list where aml_sActionName = ? and aml_uGroupId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef = 
{
	"GetWaitingCount",
	[[
	select count(*) from tbl_action_match_list where aml_sActionName = ? and aml_uLvStep = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetWaitingPlayer",
	[[
	select cs_uId,aml_uGroupId from tbl_action_match_list where aml_sActionName = ? and aml_uLvStep = ? and aml_uWaitingTeammate = 1
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"UpdateCharMatchList",
	[[
	update tbl_action_match_list set aml_uWaitingTeammate = ? where cs_uId = ? and aml_sActionName = ?
	]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef = 
{
	"CreateActionRoom",
	"insert into tbl_action_room(ar_sActionName, ar_uState, ar_uLvStep) values(?, 0, ?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"ActionRoomSetScene",
	"update tbl_action_room set sc_uId = ? where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"CreateActionTeam",
	"insert into tbl_action_team(ar_uId,cap_sName,at_uTeamLevel,at_uOtherInfo,at_uTempPoint,at_uIndex) values(?,?,?,?,?,?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetAllTeamIndex",
	"select at_uId, at_uIndex from tbl_action_team where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"InsertActionTeamMember",
	"insert into tbl_action_team_member(cs_uId, at_uId) values(?, ?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetUnfullRoom",
	[[
	select
		ar.ar_uId, count(at.at_uId)
	from
		tbl_action_room as ar left join tbl_action_team as at on(ar.ar_uId = at.ar_uId)
	where
		ar.ar_uState < 2 and
		ar.ar_sActionName = ? and
		ar.ar_uLvStep = ?
		group by ar.ar_uId
		order by ar.ar_uId
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetJfsUnUseActionRoom",
	[[
	select
		ar.ar_uId, count(at.at_uId)
	from
		tbl_action_room as ar left join tbl_action_team as at on(ar.ar_uId = at.ar_uId)
	where
		ar.ar_sActionName = ? and
		ar.ar_uLvStep = ? and
		IsNull(ar.sc_uId)
		group by ar.ar_uId
		order by ar.ar_uId
	]]
}
DefineSql(StmtDef,StmtOperater)

----查看时候有没有使用的room(room存在,没有team)
--local StmtDef = 
--{
--	"GetRobResUnUseActionRoom",
--	[[
--	select
--		ar.ar_uId, count(at.at_uId)
--	from
--		tbl_action_room as ar left join tbl_action_team as at on(ar.ar_uId = at.ar_uId)
--	where
--		ar.ar_sActionName = ? and
--		ar.ar_uLvStep = ?
--		group by ar.ar_uId
--		order by ar.ar_uId
--	]]
--}
--DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetTeamMemberNum",
	"select count(*) from tbl_action_team_member where at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetRoomTeams",
	"select at_uId from tbl_action_team where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetActionTeamMembers",
	"select cs_uId from tbl_action_team_member where at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetAllActionMembers",
	"select cs_uId from tbl_action_team_member"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetTeamById",
	"select at_uId from tbl_action_team_member where cs_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetActionTeamCaptains",
	"select cap_sName from tbl_action_team where at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"RemoveActionTeamMember",
	"delete from tbl_action_team_member where cs_uId = ? and at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)


local StmtDef =
{
	"RemoveActionRoom",
	"delete from tbl_action_room where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"FindPlayerByRoom",
	[[
	select
		at.at_uId
	from 
		tbl_action_team_member as atm, tbl_action_team as at
	where
		atm.cs_uId = ? and atm.at_uId = at.at_uId and at.ar_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"RemoveActionTeam",
	"delete from tbl_action_team where at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"ClearActionMemberOnServerStart",
	[[
	delete 
		atm 
	from 
		tbl_action_team_member as atm, tbl_char_online as co  
	where 
		atm.cs_uId = co.cs_uId and co.co_uOnServerId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"ClearMatchListOnServerStart",
	[[
	delete 
		aml 
	from 
		tbl_action_match_list as aml, tbl_char_online as co  
	where 
		(aml.cs_uId = co.cs_uId and co.co_uOnServerId = ?) 
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"ClearOffLineCharMatchList",
	[[
	delete 
	from 
		tbl_action_match_list 
	where 
		cs_uId not in (select cs_uId from tbl_char_online)
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetAllActionTeam",
	[[
	select 
		at.at_uId, count(atm.cs_uId)
	from 
		tbl_action_team as at left join tbl_action_team_member as atm on(at.at_uId = atm.at_uId)
		group by atm.at_uId
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetAllActionRoom",
	[[
	select 
		ar.ar_uId, count(at.at_uId)
	from 
		tbl_action_room as ar left join tbl_action_team as at on(ar.ar_uId = at.ar_uId)
	where
		ar.sc_uId is null
		group by at.ar_uId
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"UpdateRoomState",
	"update tbl_action_room set ar_uState = ? where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetActionRoomInfo",
	[[
	select
		ar.sc_uId, s.sc_sSceneName, s.sc_uType, s.sc_uServerId
	from 
		tbl_action_room as ar, tbl_scene as s
	where
		ar.sc_uId = s.sc_uId and ar.ar_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetRoomServerId",
	[[
	select
		tbl_scene.sc_uServerId
	from 
		tbl_action_room, tbl_scene
	where
		tbl_action_room.sc_uId = tbl_scene.sc_uId and tbl_action_room.ar_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetRoomIdByTeamId",
	"select ar_uId from tbl_action_team where at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"SetReady",
	"update tbl_action_team_member set atm_uState = 1 where cs_uId = ? and at_uId = ? and atm_uState = 0"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"SetInRoomByActionName",
	[[
		update
			tbl_action_room as ar,
			tbl_action_team as at,
			tbl_action_team_member as atm
		set
			atm.atm_uState = 2
		where
			ar.ar_sActionName = ?
			and ar.ar_uId = at.ar_uId
			and at.at_uId = atm.at_uId
			and atm.cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetMemberState",
	"select atm_uState from tbl_action_team_member where cs_uId = ? and at_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetUnReadyNum",
	"select count(*) from tbl_action_team_member where at_uId = ? and atm_uState = 0"
}
DefineSql(StmtDef,StmtOperater)

local JoinActionDB = CreateDbBox(...)


local JoinActionFuncTbl = {}
local CancelActionFuncTbl = {}

local function JoinAction_IntoUnfullRoom(actionName, args, charTbl)
	local _, teamNum, serverId = unpack(args) 
	--通知目标服添加新队伍
	if charTbl then
		local msgID = (#charTbl > 1 and 191016) or 191000
		for _, id in pairs(charTbl) do
			DbMsgToConnById(actionName, id, msgID, actionName)
			Db2GacById:RetInsertFbActionToQueue(actionName, id, actionName, false, teamNum)
		end
	end
end

local function JoinAction_StartNewRoom(actionName, args, charTbl)
	local _, roomId, serverId, allTeamMembers, teamNum = args[1],args[2],args[3],args[4],args[5],args[6]
	
	
	if charTbl then
		local msgID = (#charTbl > 1 and 191016) or 191000
		for _, id in pairs(charTbl) do
			DbMsgToConnById(actionName, id, msgID, actionName)
			Db2GacById:RetInsertFbActionToQueue(actionName, id, actionName, false, teamNum)
		end
	end
	
	for teamId, memberTbl in pairs(allTeamMembers) do
		for _, id in pairs(memberTbl) do
			Db2GacById:RetInFbActionPeopleNum(actionName, id, actionName, teamNum)
		end
	end

end

local function JoinAction_QueueUp(actionName, args, charTbl)
	local _, actionTeamId, allTeamMembers, teamNum = unpack(args)
	if charTbl then
		local msgID = (#charTbl > 1 and 191016) or 191000
		for _, id in pairs(charTbl) do
			DbMsgToConnById(actionName, id, msgID, actionName)
			Db2GacById:RetInsertFbActionToQueue(actionName, id, actionName, false, teamNum)
		end
	end
	
	--更新所有报名玩家的显示面板人数
	
	for teamId, memberTbl in pairs(allTeamMembers) do
		if actionTeamId  ~= teamId then  --新队伍不更新
			for _, id in pairs(memberTbl) do
				Db2GacById:RetInFbActionPeopleNum(actionName, id, actionName, teamNum)
			end
		end
	end
	
end

local function JoinAction_AutoMatch(actionName, args, charTbl)
	local _, retTbl = unpack(args)
	local msgID = (#charTbl > 1 and 191016) or 191000
	for _, charId in pairs(charTbl) do
		DbMsgToConnById(actionName, charId, msgID, actionName)
		Db2GacById:RetInsertFbActionToQueue(actionName, charId, actionName, false, 0)
	end
	for _,eventTbl in ipairs(retTbl) do
		JoinActionFuncTbl[eventTbl[1]](actionName, eventTbl, nil)
	end
end

local function JoinAction_WaitingMatch(actionName, args, charTbl)
	local msgID = (#charTbl > 1 and 191016) or 191000
	for _, charId in pairs(charTbl) do
		DbMsgToConnById(actionName, charId, msgID, actionName)
		DbMsgToConnById(actionName, charId, 191044, actionName)
		Db2GacById:RetInsertFbActionToQueue(actionName, charId, actionName, false, 0)
	end
end

local function CancelAction_InRoom(charId, charName, actionName, args)
	Db2GacById:RetDelQueueFbAction(actionName, charId, actionName, true)
end

local function CancelAction_DecTeamInRoom(charId, charName, actionName, args)
	Db2GacById:RetDelQueueFbAction(actionName, charId, actionName, true)
end

local function CancelAction_DecTeamInQueuer(charId, charName, actionName, args)
	local _, QueuerMembers, teamNum = unpack(args)
	Db2GacById:RetDelQueueFbAction(actionName, charId, actionName, true)
	for teamId, memberTbl in pairs(QueuerMembers) do
		for _, id in pairs(memberTbl) do
			Db2GacById:RetInFbActionPeopleNum(actionName, id, actionName, teamNum)
		end
	end
end

local function CancelAction_DecTeamInRoomAllCancel(charId, charName, actionName, args)
	local _, roomId, serverId, cancelTbl, charGroupMembers = unpack(args)
	for _,id in pairs(cancelTbl) do
		Db2GacById:RetDelQueueFbAction(actionName, id, actionName, id == charId or charGroupMembers[id])
		if id == charId then
			DbMsgToConnById(actionName, id, 191021)
		else
			DbMsgToConnById(actionName, id, 191022, charName)
		end
	end
end

local function CancelAction_DecTeamInQueuerAllCancel(charId, charName, actionName, args)
	local _, QueuerMembers, teamNum, cancelTbl, charGroupMembers = unpack(args)
	for _, id in pairs(cancelTbl) do
		Db2GacById:RetDelQueueFbAction(actionName, id, actionName, id == charId or charGroupMembers[id])
		if id == charId then
			DbMsgToConnById(actionName, id, 191021)
		else
			DbMsgToConnById(actionName, id, 191022, charName)
		end
	end
	
	for teamId, memberTbl in pairs(QueuerMembers) do
		for _, id in pairs(memberTbl) do
			Db2GacById:RetInFbActionPeopleNum(actionName, id, actionName, teamNum)
		end
	end
end

local function CancelAction_InMatchList(charId, charName, actionName, args)
	Db2GacById:RetDelQueueFbAction(actionName, charId, actionName, true)
end

local function CancelAction_AllCancelInMatchList(charId, charName, actionName, args)
	local _, charGroupMembers = unpack(args)
	for id, _ in pairs(charGroupMembers) do
		Db2GacById:RetDelQueueFbAction(actionName, id, actionName, true)
		if id == charId then
			DbMsgToConnById(actionName, id, 191021)
		else
			DbMsgToConnById(actionName, id, 191022, charName)
		end
	end
end

local function CancelAction_InQueuer(charId, charName, actionName, args)
	--如果是竞技场的话,就所有的人都取消比赛
	if actionName == "竞技场" then
		local teamMembers = args[2]
		for _,MemberId in pairs(teamMembers) do
			Db2GacById:RetDelQueueFbAction(actionName, MemberId, actionName, true)
		end
	end

	Db2GacById:RetDelQueueFbAction(actionName, charId, actionName, true)
end

JoinActionFuncTbl["IntoUnfullRoom"] = JoinAction_IntoUnfullRoom
JoinActionFuncTbl["StartNewRoom"] = JoinAction_StartNewRoom
JoinActionFuncTbl["QueueUp"] = JoinAction_QueueUp
JoinActionFuncTbl["AutoMatch"] = JoinAction_AutoMatch
JoinActionFuncTbl["WaitingMatch"] = JoinAction_WaitingMatch

CancelActionFuncTbl["DecTeamInRoom"] = CancelAction_DecTeamInRoom
CancelActionFuncTbl["DecTeamInQueuer"] = CancelAction_DecTeamInQueuer
CancelActionFuncTbl["CancelInRoom"] = CancelAction_InRoom
CancelActionFuncTbl["CancelInQueuer"] = CancelAction_InQueuer
CancelActionFuncTbl["DecTeamInRoomAllCancel"] = CancelAction_DecTeamInRoomAllCancel
CancelActionFuncTbl["DecTeamInQueuerAllCancel"] = CancelAction_DecTeamInQueuerAllCancel
CancelActionFuncTbl["CancelInMatchList"] = CancelAction_InMatchList
CancelActionFuncTbl["AllCancelInMatchList"] = CancelAction_AllCancelInMatchList

local function GetCharNameById(charId)
	local charNameRet = StmtOperater.GetCharNameById:ExecStat(charId)
	return  charNameRet(1,1)
end

local function InsertMatchList(charTbl, actionName, lvStep, groupId)
	for _, charId in pairs(charTbl) do
		StmtOperater.InsertMatchList:ExecStat(charId, actionName, lvStep, groupId)
	end
end

local function ScrambleTeam(countSort, num, maxN)
	if countSort[num] and next(countSort[num]) then
		local tbl = countSort[num][#countSort[num]]
		table.remove(countSort[num])
		return tbl
	end
	local beginN = math.min(num -1, maxN)
	for i = beginN, 1, -1 do
		local curTbl = countSort[i]
		if curTbl and next(curTbl) then
			local endV =  curTbl[#curTbl]
			table.remove(curTbl)
			local tbl = ScrambleTeam(countSort, num - i, i)
			if tbl then
				for _, v in pairs(tbl) do
					table.insert(endV, v)
				end
				return endV
			end
			table.insert(curTbl, endV)
		end
	end
end

local function ShuffleTbl(tbl)
	local n = #tbl
	if n <= 1 then
		return
	end
	local temp
	local index
	for i = 1, n do
		index = math.random(n)
		temp = tbl[index]
		tbl[index] = tbl[i]
		tbl[i] = temp
	end
end

local function GetWaitingPlayer(actionName, lvStep, memberNum)
	local result = StmtOperater.GetWaitingPlayer:ExecStat(actionName, lvStep)
	local teamTbl = {}
	local groupSort = {}  --按groupid 分类统计
	local countSort = {}  --按成员数分类统计
	for i = 0, result:GetRowNum() -1 do
		local groupId = result:GetData(i,1)
		if groupId == 0 then
			countSort[1] = countSort[1] or {}
			table.insert(countSort[1], {result:GetData(i,0)})
		else
			groupSort[groupId] = groupSort[groupId] or {}
			table.insert(groupSort[groupId], result:GetData(i,0))
		end
	end
	
	for i, v in pairs(groupSort) do
		local n =  #v
		countSort[n] = countSort[n] or {}
		table.insert(countSort[n], v)
	end
	
	--打乱顺序
	for i, v in pairs(countSort) do
		ShuffleTbl(v)
	end
	
	while true do
		local tbl = ScrambleTeam(countSort, memberNum, memberNum)
		if tbl then
			table.insert(teamTbl, tbl)
		else
			break
		end
	end
	return teamTbl
end

--获取一个能继续进人的房间, 且是较早开起的房间
local function GetUnfullRoom(actionName, actionInfo, lvStep)
	local maxTeams = actionInfo["MaxTeams"]
	if maxTeams == 1 then		--单只队伍的不重用房间
		return
	end
	local result = StmtOperater.GetUnfullRoom:ExecSql("nnn", actionName, lvStep)
	
	local num = result:GetRowNum()
	for i = 0, num - 1 do
		local teamNum = result:GetData(i, 1)
		if teamNum < maxTeams then
			local roomId = result:GetData(i, 0)
			result:Release()
			return roomId, teamNum
		end
	end
	result:Release()
end

local function CreateActionTeamSql(roomId,CapName,MaxLevel,OtherInfo,TempPoint,TeamIndex)
	StmtOperater.CreateActionTeam:ExecSql("", roomId,CapName,MaxLevel or 0,OtherInfo or 0,TempPoint or 0,TeamIndex or 0)
end

local function GetRoomServerId(roomId)
	local result = StmtOperater.GetRoomServerId:ExecSql("n",roomId)
	if result:GetRowNum() > 0 then
		local  serverId = result:GetData(0,0)
		result:Release()
		return serverId
	end
end

local function CreateActionRoom(actionName, lvStep)
	StmtOperater.CreateActionRoom:ExecSql("", actionName, lvStep)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return g_DbChannelMgr:LastInsertId()
	end
end

local function FindPlayer(charId, actionName)
	local result = StmtOperater.FindPlayer:ExecSql("n", charId, actionName)
	local actionTeamId, roomId
	if result:GetRowNum() > 0 then
		actionTeamId, roomId = result:GetData(0,0), result:GetData(0,1)
	end
	result:Release()
	return actionTeamId, roomId
end

local function IsInMatchList(charId, actionName)
	local result = StmtOperater.IsInMatchList:ExecStat(charId, actionName)
	if result and result:GetData(0,0) > 0 then
		return true
	end
end

local function CheckPlayerIsJoinAction(charId, actionName)
	if IsInMatchList(charId, actionName) or FindPlayer(charId, actionName) then
		return true
	end
	return false
end

--返回是否所有队友都准备好
local function IsAllReady(actionTeamId)
	local result = StmtOperater.GetUnReadyNum:ExecStat(actionTeamId)
	return result:GetData(0,0) == 0
end

local function IsReady(charId, actionTeamId)
	local result = StmtOperater.GetMemberState:ExecStat(charId, actionTeamId)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0) > 0
	end
end

local function IsInRoom(charId, actionTeamId)
	local result = StmtOperater.GetMemberState:ExecStat(charId, actionTeamId)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0) == 2
	end
end

local function SetReady(charId, actionTeamId)
	StmtOperater.SetReady:ExecStat(charId, actionTeamId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function JoinActionDB.SetInRoomByActionName(charId, actionName)
	JoinActionDB.SaveMatchGameMemberLog(actionName, "OnEnterRoom", "OnEnterRoom", charId, {})
	StmtOperater.SetInRoomByActionName:ExecStat(actionName, charId)
end

local function GetActionTeamMembers(actionTeamId)
	local result = StmtOperater.GetActionTeamMembers:ExecSql("n", actionTeamId)
	local memberTbl = result:GetCol(0)
	result:Release()
	return memberTbl
end


function JoinActionDB.GetTeamById(data)
	local playerId = data["uPlayerID"]
	local target_id = data["target_id"]
	local result = StmtOperater.GetTeamById:ExecStat(playerId)
	local teamId 
	if result:GetRowNum() > 0 then
		teamId = result:GetData(0,0)
		StmtOperater.InsertActionTeamMember:ExecStat(target_id, teamId)
	end
end

function JoinActionDB.GetAllActionMembers()
	local result = StmtOperater.GetAllActionMembers:ExecSql()
	local tbl = {}
	local num = result:GetRowNum()
	if num > 0 then
		for i = 1, num -1 do
			local memberTbl = result:GetData(i,0)
			table.insert(tbl,memberTbl)
		end
		result:Release()
		return tbl
	end
end

local function GetActionTeamCaptains(actionTeamId)
	local result = StmtOperater.GetActionTeamCaptains:ExecSql("n", actionTeamId)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0)
	end
end

local function GetTeamMemberNum(teamId)
	local result = StmtOperater.GetTeamMemberNum:ExecSql("", teamId)
	local num = result:GetData(0,0)
	result:Release()
	return num
end

local function GetRoomTeams(roomId)
	local result = StmtOperater.GetRoomTeams:ExecSql("n", roomId)
	local teamTbl = result:GetCol(0)
	result:Release()
	return teamTbl
end

local function GetRoomMembers(roomId)
	local teamTbl = GetRoomTeams(roomId)
	local roomMembers = {}
	for _, teamId in pairs(teamTbl) do
		roomMembers[teamId] = GetActionTeamMembers(teamId)
	end
	return roomMembers, #teamTbl
end

local function GetTeamCaptains(roomId)
	local teamTbl = GetRoomTeams(roomId)
	local roomCaptains = {}
	for _, teamId in pairs(teamTbl) do
		roomCaptains[teamId] = GetActionTeamCaptains(teamId)
	end
	return roomCaptains
end

local function GetActionRoomInfo(roomId)
	local result = StmtOperater.GetActionRoomInfo:ExecSql("nsnn",roomId)
	if result:GetRowNum() > 0 then
		local sceneId, sceneName, sceneType,serverId = result:GetData(0,0),result:GetData(0,1),result:GetData(0,2), result:GetData(0,3)
		result:Release()
		return sceneId, sceneName, sceneType, serverId
	end
end

local function GetRoomIdByTeamId(actionTeamId)
	local result = StmtOperater.GetRoomIdByTeamId:ExecSql("n",actionTeamId)
	if result:GetRowNum() > 0 then
		local roomId = result:GetData(0,0)
		result:Release()
		return roomId
	end
end



local function GetJfsUnUseActionRoom(actionName, lvStep)
	local result = StmtOperater.GetJfsUnUseActionRoom:ExecSql("n", actionName, lvStep)
	local num = result:GetRowNum()
	if num > 0 then
		for i = 0, num - 1 do
			local teamNum = result:GetData(i, 1)
			if teamNum == 0 then
				local roomId = result:GetData(i, 0)
				result:Release()
				return roomId
			end
		end
		result:Release()
	end
end

----是否有已存在但为空的房间
--local function GetRobResUnUseActionRoom(actionName, actionInfo, lvStep)
--	local result = StmtOperater.GetRobResUnUseActionRoom:ExecSql("n", actionName, lvStep)
--	local num = result:GetRowNum()
--	if num > 0 then
--		for i = 0, num - 1 do
--			local roomId = result:GetData(i, 0)
--			result:Release()
--			return roomId
--		end
--		result:Release()
--	end
--end

local function GetTeamIndexTbl(roomId, tblType)
	local res = StmtOperater.GetAllTeamIndex:ExecStat(roomId)
	if res and res:GetRowNum()>0 then
		local tbl = {}
		for i=1, res:GetRowNum() do
			if tblType then
				tbl[res:GetData(i-1, 1)] = res:GetData(i-1, 0)
			else
				tbl[res:GetData(i-1, 0)] = res:GetData(i-1, 1)
			end
		end
		return tbl
	end
end

local function GetTeamIndex(roomId)
	local tbl = GetTeamIndexTbl(roomId, true)
	if tbl then
		local index = 1
		while true do
			if not tbl[index] then
				return index
			end
			index = index + 1
		end
	else
		return 1
	end
end

local function CreateActionTeam(actionName, actionInfo, roomId, charTbl, lvStep, captainName)
	local teamIndex = GetTeamIndex(roomId)
	CreateActionTeamSql(roomId,captainName,nil,nil,nil,teamIndex)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		
		local actionTeamId = g_DbChannelMgr:LastInsertId()
		for _, id in pairs(charTbl) do
			StmtOperater.InsertActionTeamMember:ExecSql("", id, actionTeamId)
			if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
				CancelTran()
				return
			end
		end
		
		return actionTeamId, teamIndex
	else
		CancelTran()
		return
	end
end

local function AddTeamIntoRoom(teamNum, actionName, actionInfo, roomId, charTbl, lvStep, captainName, charMaxLv)
	local actionTeamId, teamIndex = CreateActionTeam(actionName, actionInfo, roomId, charTbl, lvStep, captainName)
	teamNum =  teamNum + 1
	
	local serverId = GetRoomServerId(roomId)
	if serverId then
		local data = {}
		data["actionName"] = actionName
		data["roomId"] = roomId
		data["actionTeamId"] = actionTeamId
		data["teamIndex"] = teamIndex
		data["captainName"] = captainName
		data["memberTbl"] = GetActionTeamMembers(actionTeamId)
		Db2GasCall("AddActionTeam", data , serverId)
		return {"IntoUnfullRoom", teamNum, serverId}
	end
	
	local minTeams = actionInfo["MinTeams"]
	
	if teamNum >= minTeams then
		local sceneId, serverId =  JoinActionDB.StartNewAction(roomId, actionInfo, charMaxLv)
		JoinActionDB.SaveMatchGameRoomLog(actionName, roomId, sceneId, serverId)
		local roomMembers = GetRoomMembers(roomId)
		local data = JoinActionDB.GetRoomInfoForCreateScene(actionName, roomId, roomMembers)
		Db2GasCall("CreateActionRoom", data,  serverId)
		return {"StartNewRoom", roomId, serverId, roomMembers}
	end
	return {"QueueUp", actionTeamId, GetRoomMembers(roomId)}
end

local function MatchTeam(actionInfo, actionName, lvStep)
	local memberNum = actionInfo["AutoTeamPlayerNumber"]
	local teamTbl = GetWaitingPlayer(actionName, lvStep, memberNum)
	local retTbl = {}
	if not next(teamTbl) then
		return false
	end
	
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local ex = RequireDbBox("Exchanger")
	
	local count = #teamTbl
	for i = 1, count do
		local charTbl = teamTbl[i]
		local roomId, teamNum  = GetUnfullRoom(actionName,actionInfo, lvStep)
		if not roomId then
			if count - i + 1 >= actionInfo["MinTeams"] then
				roomId = CreateActionRoom(actionName, lvStep)
				teamNum = 0
			else
				break
			end
		end
		
		local minLv = 10000
		local maxLv = 0
		local maxLvCharId
		for _,charId in pairs(charTbl) do
			--StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
			StmtOperater.UpdateCharMatchList:ExecStat(0, charId, actionName)
			local lv = CharacterMediatorDB.GetPlayerLevel(charId)
			if lv < minLv then
				minLv = lv
			end
			if lv > maxLv then
				maxLv = lv
				maxLvCharId = charId
			end
		end
		local captainName = "{Auto}" .. ex.getPlayerNameById(maxLvCharId)  -- "{Auto}" 作为功能性的标志, 含非法字符,所以不可能有名字和其相同
		table.insert(retTbl, AddTeamIntoRoom(teamNum, actionName, actionInfo, roomId, charTbl, lvStep, captainName, minLv) )
	end
	if next(retTbl) then
		return true, retTbl
	else
		return false
	end
end

local function CreateJFSActionTeam(actionName,actionInfo, charTbl, lvStep)
	local roomId = GetJfsUnUseActionRoom(actionName, lvStep)
	if not roomId then
		roomId = CreateActionRoom(actionName, lvStep)
		if not roomId then
			return
		end
	end
	
	local JiFenSaiFbDB = RequireDbBox("JiFenSaiFbDB")
	local MaxLevel,TempPoint,TongID = JiFenSaiFbDB.GetLevelAndPoint(charTbl, lvStep)
	
	CreateActionTeamSql(roomId,nil,MaxLevel,TongID,TempPoint)
	
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		
		local actionTeamId = g_DbChannelMgr:LastInsertId()
		for _, id in pairs(charTbl) do
			StmtOperater.InsertActionTeamMember:ExecSql("", id, actionTeamId)
			if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
				CancelTran()
				return
			end
		end
		
		return actionTeamId, roomId
	else
		CancelTran()
		return
	end
end

function JoinActionDB.StartNewAction(roomId, actionInfo, sceneBaseLv)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local otherArg = {}
	otherArg["m_SceneBaseLv"] = (sceneBaseLv < 10) and 0 or sceneBaseLv - 10
	
	local data = {}
	data["SceneName"] = actionInfo["SceneName"]
	data["OtherArg"] = otherArg
	local sceneId, serverId = SceneMgrDB._CreateScene(data)

	StmtOperater.ActionRoomSetScene:ExecSql("", sceneId, roomId)
	return sceneId, serverId
end

--创建抢夺资源点team并插入player
--local function CreateRobResActionTeam(actionName, actionInfo, charTbl, lvStep)
--	local roomId = GetRobResUnUseActionRoom(actionName,actionInfo, lvStep)
--	if not roomId then
--		roomId = CreateActionRoom(actionName, lvStep) --lvStep为报名ObjID
--		if not roomId then
--			return
--		end
--	end
--	CreateActionTeamSql(roomId,nil)
--	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
--		local actionTeamId = g_DbChannelMgr:LastInsertId()
--		for _, id in pairs(charTbl) do
--			StmtOperater.InsertActionTeamMember:ExecSql("", id, actionTeamId)
--			if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
--				CancelTran()
--				return
--			end
--		end
--		return actionTeamId, roomId
--	else
--		CancelTran()
--		return
--	end
--end


--local function StartNewRobResAction(roomId, actionInfo)
--	local SceneMgrDB = RequireDbBox("SceneMgrDB")
--	local data = {}
--	data["SceneName"] = actionInfo["SceneName"]
--	local sceneId, serverId = SceneMgrDB._CreateScene(data)
--	if sceneId then
--		StmtOperater.ActionRoomSetScene:ExecSql("", sceneId, roomId)
--		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
--			CancelTran()
--			return
--		end
--		return sceneId, serverId
--	end
--end

local StmtDef = 
{
	"GetJfsActionRoom",
	[[
	select
		ar_uId
	from
		tbl_action_room
	where
		ar_sActionName = "竞技场"
		and ar_uLvStep = ?
		and IsNull(sc_uId)
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetTeamsInfo",
	"select at_uId,at_uTeamLevel,at_uOtherInfo,at_uTempPoint from tbl_action_team where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetRoomType",
	"select ar_uLvStep from tbl_action_room where ar_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

--用与两人队伍比较的参数
local CompareNumTbl = {
		[1] = {3,0.05},--等级和百分比
		[2] = {3,0.1},
		[3] = {3,0.15},
   	[4] = {3,0.2},
		[5] = {3,0.25},
		[6] = {6,0.05},
		[7] = {6,0.1},
		[8] = {6,0.15},
		[9] = {6,0.2},
		[10] = {6,0.25},
		[11] = {10,0.05},
		[12] = {10,0.1},
		[13] = {10,0.15},
		[14] = {10,0.2},
		[15] = {10,0.25},
}

local function GetJfsActionTeamInfo(roomtype)
	local result = StmtOperater.GetJfsActionRoom:ExecSql("",roomtype)
	local RoomTbl = result:GetCol(0)
	result:Release()
	local TeamInfo = {}
	for _,roomId in pairs(RoomTbl) do
		local result = StmtOperater.GetTeamsInfo:ExecSql("",roomId)
		local num = result:GetRowNum()
		if num > 0 then
			for i=0,num-1 do
				local teamNum = result:GetData(i, 0)
				local MaxLevel = result:GetData(i, 1)
				local TongID = result:GetData(i, 2)
				local Point = result:GetData(i, 3)
				table.insert(TeamInfo,{teamNum,roomId,MaxLevel,Point,TongID,false})
			end
			result:Release()
		end
	end
	
	local CanMatchTeamTbl = {}
	
--	local TeamLen = #(TeamInfo)
--	if TeamLen > 15 then
--		TeamLen = 15
--	end
	
	--其它人的比较
	for i=1, #(TeamInfo)-1 do
		if not TeamInfo[i][6] then
			
			for k=i+1, #(TeamInfo) do
				if not TeamInfo[k][6] then
					
--					if math.abs(TeamInfo[i][3]-TeamInfo[k][3]) <= CompareNumTbl[TeamLen][1]
--						and math.abs(TeamInfo[i][4]-TeamInfo[k][4]) <= (TeamInfo[i][4]*CompareNumTbl[TeamLen][2]) then
					if math.abs(TeamInfo[i][4]-TeamInfo[k][4]) <= 10 then
							
						TeamInfo[i][6] = true
						TeamInfo[k][6] = true
						table.insert(CanMatchTeamTbl,{["Team1"]={TeamInfo[i][1],TeamInfo[i][2]},["Team2"]={TeamInfo[k][1],TeamInfo[k][2]}})
						
						return CanMatchTeamTbl
					end
					
				end
			end
			
		end
	end
	
	return CanMatchTeamTbl
end

local StmtDef = 
{
	"ModifyTeamRoom",
	[[
	update
		tbl_action_team
	set
		ar_uId = ?
	where
		at_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local function StartNewJfsAction(CanMatchTeamTbl, actionInfo, sceneBaseLv)
	local createRoomId = CanMatchTeamTbl["Team1"][2]
	local TeamId = CanMatchTeamTbl["Team2"][1]
	local delRoomId = CanMatchTeamTbl["Team2"][2]
	
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local otherArg = {}
	otherArg["m_SceneBaseLv"] = (sceneBaseLv < 10) and 0 or sceneBaseLv - 10
	
	local data = {}
	data["SceneName"] = actionInfo["SceneName"]
	data["OtherArg"] = otherArg
	local sceneId, serverId = SceneMgrDB._CreateScene(data)
	if sceneId then
		--修改另一队的人指向这个房间,把旧房间删除掉
		StmtOperater.ModifyTeamRoom:ExecSql("", createRoomId, TeamId)
		StmtOperater.RemoveActionRoom:ExecSql("", delRoomId)
		
		StmtOperater.ActionRoomSetScene:ExecSql("", sceneId, createRoomId)
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			CancelTran()
			return
		end
		return sceneId, serverId, createRoomId
	end
end

local StmtDef = 
{
	"GetRoomInfo",
	"select ar_sActionName from tbl_action_room where ar_uId = ? and IsNull(sc_uId)"
}
DefineSql(StmtDef,StmtOperater)

local function JfsCancelAction(roomId, teamId)
	local result = StmtOperater.GetRoomInfo:ExecSql("n", roomId)
	local num = result:GetRowNum()
	if num > 0 then
		result:Release()
		
		local teamMembers = GetActionTeamMembers(teamId)
		StmtOperater.RemoveActionTeam:ExecSql("", teamId)
		return teamMembers
	end
end

--local function RobResCancelAction(roomId, teamId)
--	local result = StmtOperater.GetRoomInfo:ExecSql("n", roomId)
--	local num = result:GetRowNum()
--	if num > 0 then
--		result:Release()
--		
--		local teamMembers = GetActionTeamMembers(teamId)
--		StmtOperater.RemoveActionTeam:ExecSql("", teamId)
--		return teamMembers
--	end
--end

local StmtDef = 
{
	"_GetCharActionName",
	[[
	select
		ar_sActionName
	from
		tbl_action_room as ar,
		tbl_action_team as at,
		tbl_action_team_member as atm
	where
		atm.at_uId = at.at_uId and
		at.ar_uId = ar.ar_uId and
		(IsNull(ar.sc_uId) or ar.sc_uId <> ?) and
		atm.cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_GetCharWaitingMatchActionName",
	[[
	select
		aml_sActionName
	from
		tbl_action_match_list
	where
		cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local function GetAllActionName(charId)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local sceneId = SceneMgrDB.GetPlayerCurScene(charId)
	local result = StmtOperater._GetCharActionName:ExecStat(sceneId, charId)
	local result2 = StmtOperater._GetCharWaitingMatchActionName:ExecStat(charId)
	local tbl = {}
	for i = 0, result:GetRowNum() -1 do
		tbl[result:GetData(i,0)] = true
	end
	for i = 0, result2:GetRowNum() -1 do
		tbl[result2:GetData(i,0)] = true
	end
	return tbl
end

--------------------------------------------------------------------------------------
function JoinActionDB.GM_SetActionTeamNum(data)
	local MultiServerDB =  RequireDbBox("MultiServerDB")
	MultiServerDB.UpdataConfServer(data["ActionName"].."MinTeams", data["MinNum"])
	MultiServerDB.UpdataConfServer(data["ActionName"].."MaxTeams", data["MaxNum"])
	MultiServerDB.UpdataConfServer(data["ActionName"].."MemberNum", data["MemberNum"])
end


--抢夺资源点做特殊处理
--function JoinActionDB:JoinAction(data)
function JoinActionDB.JoinAction(data)
	local actionName = data["ActionName"]
	local isSucceed, args, charTbl, otherTeamStart = JoinActionDB.JoinActionInner(data)
	if isSucceed then
		local state = args[1]
		--print(state)
		JoinActionDB.SaveMatchGameMemberLog(actionName, "JoinAction", state, data["CharId"], charTbl)
		
		JoinActionFuncTbl[state](actionName, args, charTbl)
		if actionName == "竞技场" then
			if otherTeamStart then
				for i=1, #(otherTeamStart) do
					local otherState = otherTeamStart[i][1]
					JoinActionFuncTbl[otherState](actionName, otherTeamStart[i])
				end
			end
		end
	else
		return args
	end
end

--function JoinActionDB:JoinActionInner(data)
function JoinActionDB.JoinActionInner(data)
	local charId = data["CharId"]
	local actionName = data["ActionName"]
	local IsAlarm = data["IsAlarm"]
	
	local actionInfo = ActionBasic:GetActionInfo(actionName)
	if actionInfo["MinTeams"] and actionInfo["MaxTeams"] then
		local MultiServerDB =  RequireDbBox("MultiServerDB")
		local dbMinTeams = MultiServerDB.GetConfServer(actionName.."MinTeams")
		if dbMinTeams then
			actionInfo["MinTeams"] = tonumber(dbMinTeams)
			actionInfo["MaxTeams"] = tonumber(MultiServerDB.GetConfServer(actionName.."MaxTeams"))
			if actionInfo["AutoTeamPlayerNumber"] then
				actionInfo["AutoTeamPlayerNumber"] = tonumber(MultiServerDB.GetConfServer(actionName.."MemberNum"))
			end
		end
	end

	local ex = RequireDbBox("Exchanger")
	local captainName = ex.getPlayerNameById(charId)
	local charTbl
	if actionInfo["SingleOrTeam"] == 1 or (actionInfo["SingleOrTeam"] == 2 and not actionInfo["IsAllMemberJoin"]) then --单人报名
		charTbl = {charId}
	elseif actionInfo["SingleOrTeam"] == 2 then--队伍报名
		local GasTeamDB = RequireDbBox("GasTeamDB")
		local teamId = GasTeamDB.GetTeamID(charId)
		if teamId == 0 then
			charTbl = {charId}
		else
			local captain = GasTeamDB.GetCaptain(teamId)
			if captain ~= charId then
				return false, {191012}
			else
				charTbl = GasTeamDB.GetTeamOnlineMembersId(teamId)
				if #charTbl == 0 then --有可能连自己都不在线了,直接返回
					return false
				end
			end
		end
--		elseif actionInfo["SingleOrTeam"] == 3 and actionName == "抢夺资源点" then --佣兵团个人报名
--			local tong_box = RequireDbBox("GasTongBasicDB")
--			local tong_id,tong_name,tong_pos = tong_box.GetTongID(charId)
--			charTbl = tong_box.GetTongOnlineCharId(tong_id)    --获得帮会在线player
--			
--			if #charTbl == 0 then
--				return false
--			end
	end
	
	for _, id in pairs(charTbl) do
		if CheckPlayerIsJoinAction(id, actionName) then --检测是否有人在该活动中
			if id == charId then
				return false, {191001, actionName}
			else
				return false, {191014, GetCharNameById(id),actionName}
			end
		end
		if not JoinActionDB.CheckWarnValue(id, charId) then
			return
		end
	end
	
	--检测,进需要门票的副本时,看每个人的身上是否有门票
	if actionInfo["IsCheckItem"] then
		local SceneMgrDB = RequireDbBox("SceneMgrDB")
		for _, id in pairs(charTbl) do
			local res, MsgInfo = SceneMgrDB.DeleteItemFromInScene(id, actionInfo["SceneName"], true)
			if not res then
				return false, MsgInfo
			end
		end
	end
	
	if JuQingTransmit_Common(actionName) then
		local JuQingDB = RequireDbBox("JuQingFbDB")
		local res, MsgInfo = JuQingDB.FbCheck(charId, actionName, charTbl, data["IsAutoTeam"])
		if not res then
			return false, MsgInfo
		end
	end
	
	--等级检测
	local averageLv = 0 -- 平均等级
	local minLv = 100000
	local captainLv
	local MinLv, MaxLv = actionInfo["MinLv"], actionInfo["MaxLv"]
	if MinLv and MaxLv then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local countLevel = 0
		for _, id in pairs(charTbl) do
			local lv = CharacterMediatorDB.GetPlayerLevel(id)
			if lv < MinLv then
				if id ~= charId then
					return false, {191009, MinLv}
				else
					return false, {191010, MinLv}
				end
			end
			if lv > MaxLv then
				if id ~= charId then
					return false, {191053, MaxLv}
				else
					return false, {191054, MaxLv}
				end
			end
			if lv < minLv then
				minLv = lv
			end
			if id == charId then
				captainLv = lv
			end
			countLevel = countLevel + lv
		end
		averageLv = math.floor(countLevel / #(charTbl))
	end
	
	
	--检测次数限制
	local LimitType = actionInfo["LimitType"]
	if LimitType then
		local ActivityCountDB = RequireDbBox("ActivityCountDB")
		local criticalTime =  ActivityCountDB.GetActivityCriticalTime(LimitType)
		for _, id in pairs(charTbl) do
			if not ActivityCountDB.CheckActivityCount(id, LimitType, criticalTime) then
				local msgId = 191019
				local charName = GetCharNameById(id)
				if actionInfo["LimitCycle"] == "week" then
					msgId = 191020
				end
				return false, {msgId, charName, actionName}
			end
		end
	end
	
	--检测能不能报名PVP
	if actionName == "战魂鲜血试炼场" and not JoinActionDB.CheckJoinPVPAction(charId) then
		return false,{191056}
	end
	
	--所有检测完后 数据库创建活动队伍
	local RetactionTeamId = nil
	local RetroomId = nil
	
	if actionName == "竞技场" then
		local lvStep = ActionBasic:GetActionLvStep(actionName, #(charTbl))
		RetactionTeamId, RetroomId = CreateJFSActionTeam(actionName, actionInfo, charTbl, lvStep)
		
		if not RetactionTeamId then
			CancelTran()
			return false
		end
		
		--每报名一次就开始找一次
		local CanMatchTeamTbl = GetJfsActionTeamInfo(lvStep)
		
		local OpenNewRoom = false
		local ServerId = nil
		for i=1, #(CanMatchTeamTbl) do
			if CanMatchTeamTbl[i]["Team1"][1]== RetactionTeamId or CanMatchTeamTbl[i]["Team2"][1]== RetactionTeamId then
				local sceneId, tempserverId, roomId =  StartNewJfsAction(CanMatchTeamTbl[i], actionInfo, averageLv)
				if sceneId then
					ServerId = tempserverId
					RetroomId = roomId
					OpenNewRoom = true
				else
					CancelTran()
					return false
				end
				table.remove(CanMatchTeamTbl, i)
				break
			end
		end
		--如果还有值的话,就说明其它人也可以比赛了
		local otherTeamStar = {}
		if next(CanMatchTeamTbl) then
			for i=1, #(CanMatchTeamTbl) do
				local sceneId, serverId, roomId =  StartNewJfsAction(CanMatchTeamTbl[i], actionInfo, averageLv)
				if sceneId then
					local roomMembers = GetRoomMembers(RetroomId)
					local data = JoinActionDB.GetRoomInfoForCreateScene(actionName, roomId, roomMembers)
					Db2GasCall("CreateActionRoom", data,  serverId)
					table.insert(otherTeamStar,{"StartNewRoom", roomId, serverId, roomMembers})
				else
					CancelTran()
					return false
				end
			end
		end
		
		
		local roomMembers = GetRoomMembers(RetroomId)
		if OpenNewRoom then
			local data = JoinActionDB.GetRoomInfoForCreateScene(actionName, RetroomId, roomMembers)
			Db2GasCall("CreateActionRoom", data,  ServerId)
			return true, {"StartNewRoom", RetroomId, ServerId, roomMembers}, charTbl ,otherTeamStar
		else
			return true, {"QueueUp", RetactionTeamId, roomMembers}, charTbl, otherTeamStar
		end
	else
		local lvStep = ActionBasic:GetActionLvStep(actionName, captainLv)
		local IsAutoTeam = data["IsAutoTeam"]
		if not IsAutoTeam then --非自动配对的活动
			local roomId, teamNum  = GetUnfullRoom(actionName,actionInfo, lvStep)
			if not roomId then
				roomId = CreateActionRoom(actionName, lvStep)
				if not roomId then
					return false
				end
				teamNum = 0
			end
			
			return true, AddTeamIntoRoom(teamNum,actionName, actionInfo, roomId, charTbl, lvStep, captainName, minLv), charTbl

		else -- 自动配对的活动
			local GasTeamDB = RequireDbBox("GasTeamDB")
			local groupId = 0
			if actionInfo["IsAllMemberJoin"] then
				groupId = GasTeamDB.GetTeamID(charId)
			end
			InsertMatchList(charTbl, actionName, lvStep, groupId)
			local isSucceed, RetTbl = MatchTeam(actionInfo, actionName, lvStep)
			if isSucceed then
				return true , {"AutoMatch", RetTbl}, charTbl
			else
				return true , {"WaitingMatch"}, charTbl
			end
		end
	end
	
	--return true, {"QueueUp", charTbl, RetactionTeamId, GetRoomMembers(RetroomId)}
end

function JoinActionDB.GetPlayerWaitingInfo(charId, actionName, waitingTeammate)
	local result = StmtOperater.GetPlayerWaitingInfo:ExecStat(charId, actionName, waitingTeammate)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0)
	end
end

function JoinActionDB.GetWaitingGroupMember(actionName, groupId, waitingTeammate)
	if groupId == 0 then
		return {}
	end
	
	local res = StmtOperater.GetWaitingGroupMember:ExecStat(actionName, groupId, waitingTeammate)
	local tbl = {}
	if res and res:GetRowNum()>0 then
		for _, id in pairs(res:GetCol(0)) do
			tbl[id] = true
		end
	end
	return tbl
end

--function JoinActionDB:CancelAction(data)
function JoinActionDB.CancelAction(data)
	local actionName = data["ActionName"]
	local charName =  data["CharName"]
	local charId = data["CharId"]
	local isSucceed, args = JoinActionDB.CancelActionInner(data)
	if isSucceed then
		local state = args[1]
		if state == 9457 then
			DbMsgToConnById(actionName, charId, 9457)
		else
			--print(state)
			JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", state, charId, {})
			CancelActionFuncTbl[state](charId, charName, actionName, args)
 		end
	elseif not (args and args[1] == "Ignore") then
		Db2GacById:RetDelQueueFbAction(actionName, charId, actionName, true) --不成功也删除客服端的窗口
		return
	end
end

--function JoinActionDB:CancelActionInner(data)
function JoinActionDB.CancelActionInner(data)
	local charId = data["CharId"]
	local actionName = data["ActionName"]
	
	JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "BeginCancel", charId, {})
	
	local groupId = JoinActionDB.GetPlayerWaitingInfo(charId, actionName, 1)
	if groupId then
		if groupId ~= 0 then
			local charGroupMembers = JoinActionDB.GetWaitingGroupMember(actionName, groupId, 1)
			JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "DeleteMatchList", charId, {})
			for id, _ in pairs(charGroupMembers) do
				if id ~= charId then
					JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "DeleteMatchList", id, {})
				end
			end
			StmtOperater.DeleteWaitingGroup:ExecStat(actionName, groupId)
			return true, {"AllCancelInMatchList", charGroupMembers}
		else
			StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
			return true, {"CancelInMatchList"}
		end
	end
	
	local actionTeamId, roomId = FindPlayer(charId, actionName)
	if not data["IsCancelAll"] and GetTeamMemberNum(actionTeamId)>0 and IsAllReady(actionTeamId) and not IsInRoom(charId, actionTeamId) then
		JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "Ignore", charId, {})
		return false, {"Ignore"}
	end
	
	if not actionTeamId then
		StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
		return false
	end
	
	local cancelTbl = GetActionTeamMembers(actionTeamId)
	local memberNum = GetTeamMemberNum(actionTeamId)
	local tong_box = RequireDbBox("GasTongBasicDB")
	
	if memberNum == 1 or actionName == "竞技场" then
		StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
		StmtOperater.RemoveActionTeamMember:ExecSql("", charId, actionTeamId)
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			return false
		end
	else --以队伍形式参加的活动
		if IsAllReady(actionTeamId) then  --开始进场了 只取消自己
			StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
			StmtOperater.RemoveActionTeamMember:ExecSql("", charId, actionTeamId)
			if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
				return false
			end
		else --还没开始进场, 大家一起取消
			local charGroupMembers = {}
			if IsInMatchList(charId, actionName) then
				charGroupMembers = JoinActionDB.GetWaitingGroupMember(actionName, JoinActionDB.GetPlayerWaitingInfo(charId, actionName, 0), 0)
			else
				for _, id in pairs(cancelTbl) do
					charGroupMembers[id] = true
				end
			end
			for _, id in pairs(cancelTbl) do
				if id ~= charId then
					if charGroupMembers[id] then
						JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "DeleteMatchList", id, {})
						StmtOperater.DeleteMatchList:ExecStat(id, actionName)
					else
						JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "UpdateCharMatchList", id, {})
						StmtOperater.UpdateCharMatchList:ExecStat(1, id, actionName)
					end
				else
					JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelAction", "DeleteMatchList", charId, {})
					StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
				end
			end
			StmtOperater.RemoveActionTeam:ExecSql("", actionTeamId)
			local serverId = GetRoomServerId(roomId)
			if serverId then
				local data = {}
				data["roomId"] = roomId 
				data["actionName"] = actionName
				data["cancelTbl"] = cancelTbl
				Db2GasCall("TeamCancelAction", data, serverId)
				return true, {"DecTeamInRoomAllCancel", roomId, serverId, cancelTbl, charGroupMembers}
			else	--在排队中, 让其他队伍更新排队队伍数 
				local QueuerMembers, queuerNum = GetRoomMembers(roomId)
				return true, {"DecTeamInQueuerAllCancel", QueuerMembers, queuerNum, cancelTbl, charGroupMembers}
			end
		end
	end
	
	if actionName == "竞技场" then
		local teamMembers = JfsCancelAction(roomId, actionTeamId)
		if teamMembers then
			return true, {"CancelInQueuer", teamMembers}
		end
	end
	
	if GetTeamMemberNum(actionTeamId) == 0 then --该队伍被解散
		StmtOperater.RemoveActionTeam:ExecSql("", actionTeamId)
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			return false
		end
		
		local serverId = GetRoomServerId(roomId)
		if serverId then
			local data = {}
			data["charId"] = charId
			data["roomId"] = roomId
			data["actionName"] = actionName
			Db2GasCall("PlayerCancelAction", data, serverId)
			return true, {"DecTeamInRoom"}
		else	--在排队中, 让其他队伍更新排队队伍数 
			local QueuerMembers, queuerNum = GetRoomMembers(roomId)
			return true, {"DecTeamInQueuer", QueuerMembers, queuerNum}
		end
	end
	
	if roomId then
		local serverId = GetRoomServerId(roomId)
		local data = {}
		data["charId"] = charId
		data["roomId"] = roomId
		data["actionName"] = actionName
		Db2GasCall("PlayerCancelAction", data, serverId)
		return true, {"CancelInRoom"}
	else
		return true, {"CancelInQueuer"}
	end
end

--function JoinActionDB:CancelAllAction(data)
function JoinActionDB.CancelAllAction(data)
	local isSucceed, charName, CancelActionTbl = JoinActionDB.CancelAllActionInner(data)
	local charId = data["CharId"]
	if isSucceed then
		for i=1, #(CancelActionTbl) do
			local actionName = CancelActionTbl[i]["ActionName"]
			local args = CancelActionTbl[i]["Args"]
			local state = args[1]
			CancelActionFuncTbl[state](charId, charName, actionName, args)
		end
	end
end

function JoinActionDB.CancelAllActionInner(data)
	--得到该玩家报的活动名
	local charId = data["CharId"]
	local ActionTbl = data["ActionTbl"]
	data["IsCancelAll"] = true
	local IsSucc,args
	local CancelAction = {}
	
	for actionName,_ in pairs(ActionTbl) do
		data["ActionName"] = actionName
		IsSucc,args = JoinActionDB.CancelActionInner(data)
		if IsSucc then
			table.insert(CancelAction, {["ActionName"] = actionName, ["Args"]=args})
		end
	end
	
	if #CancelAction > 0 then
		return true, GetCharNameById(charId), CancelAction
	else
		return false
	end
end

function JoinActionDB.GetAllActionName(data)
	return GetAllActionName(data["CharId"],data["SceneId"])
end

function JoinActionDB.DeleteActionTeam(data)
	local actionTeamId = data["ActionTeamId"]
	StmtOperater.RemoveActionTeam:ExecSql("", actionTeamId)
end

--function JoinActionDB:CancelActionByRoom(data)
function JoinActionDB.CancelActionByRoom(data)
	local charId = data["CharId"]
	local roomId = data["RoomId"]
	local actionName = data["ActionName"]
	local autoTeam = data["AutoTeam"]
	local result = StmtOperater.FindPlayerByRoom:ExecSql("n", charId, roomId)
	if result:GetRowNum() > 0 then
		local actionTeamId = result:GetData(0,0)
		StmtOperater.RemoveActionTeamMember:ExecSql("", charId, actionTeamId)
		
		if GetTeamMemberNum(actionTeamId) == 0 then --该队伍被解散
			StmtOperater.RemoveActionTeam:ExecSql("", actionTeamId)
		end
	end
	if autoTeam and actionName then
		JoinActionDB.SaveMatchGameMemberLog(actionName, "CancelActionByRoom", "DeleteMatchList", charId, {})
		StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
	end
end

function JoinActionDB.ClearTeamOnServerStartUp(serverId)
	StmtOperater.ClearMatchListOnServerStart:ExecStat(serverId)
	StmtOperater.ClearOffLineCharMatchList:ExecStat()
	StmtOperater.ClearActionMemberOnServerStart:ExecStat(serverId)
	local teamResult = StmtOperater.GetAllActionTeam:ExecSql("n")
	for i = 0, teamResult:GetRowNum() -1 do
		if teamResult:GetData(i,1) == 0 then
			StmtOperater.RemoveActionTeam:ExecSql("", teamResult:GetData(i,0))
		end
	end
	teamResult:Release()
	
	local roomResult = StmtOperater.GetAllActionRoom:ExecSql("n")
	for i = 0, roomResult:GetRowNum() -1 do
		if roomResult:GetData(i,1) == 0 then
			StmtOperater.RemoveActionRoom:ExecSql("", roomResult:GetData(i,0))
		end
	end
	roomResult:Release()
end

function JoinActionDB.GetRoomInfoForCreateScene(actionName , roomId, roomMembers)
	local JiFenSaiInfo = nil
	if actionName == "竞技场" then
		JiFenSaiInfo = {}
		
		local res = StmtOperater.GetRoomType:ExecSql("", roomId)
		JiFenSaiInfo["RoomType"] = res:GetData(0,0)
		res:Release()
		
		JiFenSaiInfo["NameTbl"] = {}
		local ExChanger = RequireDbBox("Exchanger")
		for _,teamInfo in pairs(roomMembers) do
			for _, ID in pairs(teamInfo) do
				JiFenSaiInfo["NameTbl"][ID] = ExChanger.getPlayerNameById(ID)
			end
		end
	end

	StmtOperater.UpdateRoomState:ExecSql("", 1, roomId)
	
	local sceneId, sceneName, sceneType, serverId = GetActionRoomInfo(roomId)
	assert(sceneId, "活动房间没设置场景!!")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local otherArg = SceneMgrDB.GetSceneOtherArg(sceneId)
	
	local res = {}
	res["actionName"] = actionName
	res["roomId"] = roomId
	res["sceneId"] = sceneId
	res["sceneName"] = sceneName
	res["roomMembers"] = roomMembers
	res["captainsTbl"] = GetTeamCaptains(roomId)
	res["teamIndexTbl"] = GetTeamIndexTbl(roomId)
	res["JiFenSaiInfo"] = JiFenSaiInfo
	res["otherArg"] = otherArg
	
	if JuQingTransmit_Common(actionName)
		or (MatchGame_Common(actionName) and MatchGame_Common(actionName, "GameType") == 1) then
		res["PlayerInfo"] = {}
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local FightingEvaluationDB = RequireDbBox("FightingEvaluationDB")
		for teamID,Members in pairs(roomMembers) do
			res["PlayerInfo"][teamID] = {}
			res["PlayerInfo"][teamID]["Res"] = CharacterMediatorDB.GetCharInfoByID(Members):ToTable()
			res["PlayerInfo"][teamID]["FightingEvaluation"] = FightingEvaluationDB.SelectFightingEvaluationByCharIDTbl(Members):ToTable()
		end
	end
	return res
end


function JoinActionDB.WaitPlayerEnd(data)
	local roomId = data["RoomId"]
	StmtOperater.UpdateRoomState:ExecSql("", 2, roomId)
end

--function JoinActionDB:EnterAction(data)
function JoinActionDB.EnterAction(data)
	local charId = data["CharId"]
	local actionName = data["ActionName"]
	local isSucceed, roomId, serverId, teamMembers = JoinActionDB.EnterActionInner(data)
	if not isSucceed then
		return
	end
	if isSucceed == "WaitOtherTeammate" then
		Db2GacById:WaitOtherTeammate(actionName,charId, actionName)
		JoinActionDB.SaveMatchGameMemberLog(actionName, "EnterAction", "WaitOtherTeammate", charId, {})
	else
		for _, id in pairs(teamMembers) do
			Db2GacById:ActionAllReady(actionName, id, serverId, roomId, actionName)
		end
		JoinActionDB.SaveMatchGameMemberLog(actionName, "EnterAction", "AllReady", charId, {})
	end
end

function JoinActionDB.EnterActionInner(data)
	local charId = data["CharId"]
	local actionName = data["ActionName"]
	local actionTeamId, roomId = FindPlayer(charId, actionName)
	if not roomId then
		return false
	end
	
	--local result = StmtOperater.GetTeamById:ExecStat(charId)
	--for i = 0, result:GetRowNum() -1 do
	--	if IsAllReady(result:GetData(i,0)) then
	--		return
	--	end
	--end
	
	if not SetReady(charId, actionTeamId) then
		return
	end
	if not IsAllReady(actionTeamId) then
		return "WaitOtherTeammate"
	end
	
	local memberTbl = GetActionTeamMembers(actionTeamId)
	local sceneId, sceneName, sceneType, serverId = GetActionRoomInfo(roomId)
	for _, id in pairs(memberTbl) do
		StmtOperater.DeleteMatchList:ExecStat(id, actionName)
	end

	return true, roomId, serverId, memberTbl
end

local StmtDef = 
{
	"GetAllActionByCharId",
	[[
	select
		ar.ar_sActionName, at.at_uId, at.ar_uId
	from
		tbl_action_room as ar,
		tbl_action_team as at,
		tbl_action_team_member as atm
	where
		atm.at_uId = at.at_uId and
		at.ar_uId = ar.ar_uId and
		atm.cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

function FindAllTeamUnreadyAction(charId, resultTbl)
	local result =  StmtOperater.GetAllActionByCharId:ExecStat(charId)
	if result:GetRowNum() > 0 then
		for i = 0, result:GetRowNum() - 1 do
			
			local actionName = result:GetData(i,0)
			if not string.find(actionName, "鲜血") then
				
				if MatchGame_Common(actionName) and MatchGame_Common(actionName,"AutoTeamPlayerNumber") ~= "" then -- 就考虑比赛本了 ,其他的有自己的规则
					JoinActionDB.SaveMatchGameMemberLog(actionName, "ExitTeam", "BeginExit", charId, {})
					
					local actionTeamId = result:GetData(i,1)
					local roomId = result:GetData(i,2)
					if GetTeamMemberNum(actionTeamId) > 0 and not IsAllReady(actionTeamId) then
						assert(resultTbl[actionName] == nil, "退队取消活动异常"..actionName.." "..charId.." "..actionTeamId)
						local teamMembers = GetActionTeamMembers(actionTeamId)
						local charGroupMembers = {}
						
						if IsInMatchList(charId, actionName) then
							charGroupMembers = JoinActionDB.GetWaitingGroupMember(actionName, JoinActionDB.GetPlayerWaitingInfo(charId, actionName, 0), 0)
						else
							for _, id in pairs(teamMembers) do
								charGroupMembers[id] = true
							end
						end
						
						for _, id in pairs(teamMembers) do
							if id ~= charId then
								if charGroupMembers[id] then
									JoinActionDB.SaveMatchGameMemberLog(actionName, "ExitTeam", "DeleteMatchList", id, {})
									StmtOperater.DeleteMatchList:ExecStat(id, actionName)
								else
									JoinActionDB.SaveMatchGameMemberLog(actionName, "ExitTeam", "UpdateCharMatchList", id, {})
									StmtOperater.UpdateCharMatchList:ExecStat(1, id, actionName)
								end
							else
								JoinActionDB.SaveMatchGameMemberLog(actionName, "ExitTeam", "DeleteMatchList", charId, {})
								StmtOperater.DeleteMatchList:ExecStat(charId, actionName)
							end
						end
						
						StmtOperater.RemoveActionTeam:ExecSql("", actionTeamId)
						resultTbl[actionName] = {teamMembers, charGroupMembers, roomId}
					end
				end
				
			end
		end
		
	end
end

function JoinActionDB.OnReleaseTeam(teamId)
	local GasTeamDB = RequireDbBox("GasTeamDB")
	local members = GasTeamDB.GetTeamOnlineMembersId(teamId)
	local captain = GasTeamDB.GetCaptain(teamId)
	local ActionTbl = {}
	for _, id in pairs(members) do
		FindAllTeamUnreadyAction(id, ActionTbl)
	end
	if next(ActionTbl) then
		Db2CallBack("OnReleaseTeamCancelAction", captain, ActionTbl)
	end
end

function JoinActionDB.OnLeaveTeam(charId, teamId)
	local ActionTbl = {}
	FindAllTeamUnreadyAction(charId, ActionTbl)
	if next(ActionTbl) then
		local charName = GetCharNameById(charId)
		Db2CallBack("OnLeaveTeamCancelAction", charId, charName, ActionTbl)
	end
end

function JoinActionDB.OnEnterRoom(data)
	if data["ActionName"] then
		JoinActionDB.SetInRoomByActionName(data["PlayerId"], data["ActionName"])
	end
	if data["ActivityName"] then
		local ActivityCountDB = RequireDbBox("ActivityCountDB")
		return ActivityCountDB.AddActivityCount(data)
	end
end

local StmtDef = 
{
	"_UpdateWarnValue",
	"replace into tbl_warn_value (cs_uId, w_uValue, w_dtTime) values (?, ?, from_unixtime(?))"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_GetWarnValue",
	"select w_uValue, unix_timestamp(w_dtTime) from tbl_warn_value where cs_uId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_ClearAllWarnValue",
	"delete from tbl_warn_value"
}
DefineSql(StmtDef,StmtOperater)

--JoinActionDB:UpdateWarnValue(data)
function JoinActionDB.UpdateWarnValue(data)
	local WarnTbl = data
	for PlayerId, st in pairs(WarnTbl) do
		if st.WarnValue ~= 0 then
			local WarnValue = JoinActionDB.GetWarnValue(PlayerId)
			WarnValue = (WarnValue or 0) + st.WarnValue
			StmtOperater._UpdateWarnValue:ExecStat(PlayerId, WarnValue, os.time())
			Db2GacById:SetActionPanelWarnValue("WarnValue", PlayerId, WarnValue)
		end
	end
end

function JoinActionDB.ClearWarnValue(PlayerId)
	StmtOperater._UpdateWarnValue:ExecStat(PlayerId, 0, os.time())
	Db2GacById:SetActionPanelWarnValue("WarnValue", PlayerId, 0)
end

function JoinActionDB.ClearAllWarnValue(data)
	StmtOperater._ClearAllWarnValue:ExecStat()
end

function JoinActionDB.GetWarnValue(PlayerId)
	local res = StmtOperater._GetWarnValue:ExecStat(PlayerId)
	if res and res:GetRowNum()>0 then
		return res:GetData(0,0), res:GetData(0,1)
	end
end

function JoinActionDB.CheckWarnValue(PlayerId, Captain)
	local WarnValue, Time = JoinActionDB.GetWarnValue(PlayerId)
	if WarnValue and WarnValue >= 10000 then
		local timedif = 7200 - os.difftime(os.time() , Time)
		if timedif > 0 then
			local hourdif = math.floor(timedif/3600)
			local mindif = math.floor( (timedif - hourdif*3600)/60 )
			DbMsgToConnById("WarnValue", PlayerId, 191048, hourdif, mindif)
			if Captain and Captain ~= PlayerId then
				DbMsgToConnById("WarnValue", Captain, 191055, GetCharNameById(PlayerId))
			end
			return
		else
			JoinActionDB.ClearWarnValue(PlayerId)
			--print("ClearWarnValue", PlayerId)
		end
	end
	return true
end

function JoinActionDB.SetActionPanelWarnValue(PlayerId)
	local WarnValue, Time = JoinActionDB.GetWarnValue(PlayerId)
	local timedif = 7200 - os.difftime(os.time() , Time)
	if WarnValue and WarnValue >= 10000 and timedif <= 0 then
		JoinActionDB.ClearWarnValue(PlayerId)
	else
		Db2GacById:SetActionPanelWarnValue("WarnValue", PlayerId, WarnValue)
	end
end

function JoinActionDB.SaveMatchGameRoomLog(ActionName, RoomId, SceneId, ServerId)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveMatchGameRoomLog(ActionName, RoomId, SceneId, ServerId)
end

function JoinActionDB.SaveMatchGameMemberLog(ActionName, FuncName, State, CharId, PlayerTbl)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	local TeamId, RoomId = FindPlayer(CharId, ActionName)
	
	g_LogMgr.SaveMatchGameMemberLog(CharId, ActionName, TeamId or 0, RoomId or 0, FuncName, State)
	
	for _, PlayerId in pairs(PlayerTbl) do
		if PlayerId ~= CharId then
			g_LogMgr.SaveMatchGameMemberLog(PlayerId, ActionName, TeamId or 0, RoomId or 0, FuncName, State)
		end
	end
end

local StmtDef = 
{
	"_InsertXianxieRes",
	"Insert into tbl_action_xianxie (cs_uId, ax_uWin, ax_uScore, ax_dtTime) values (?,?,?,now())"
}
DefineSql(StmtDef,StmtOperater)

function JoinActionDB.SavePVPXianxieRes(data)
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.SaveXianxieLog(data)
	
	for id, v in pairs(data) do
		local isWin, score = v[1], v[2]
		StmtOperater._InsertXianxieRes:ExecStat(id, isWin, score)
	end
end

--===========================================
--PVP用代码,检测活动让不让报名进入
local StmtDef = 
{
	"_GetXianxieRes",
	[[
		select
			cs_uId, sum(ax_uWin),sum(ax_uScore)
		from
			tbl_action_xianxie
		where
			YEAR(ax_dtTime) <= ? and MONTH(ax_dtTime) <= ? and DAYOFMONTH(ax_dtTime) < ?
		group by(cs_uId) order by sum(ax_uWin) desc, sum(ax_uScore) desc
	]]
}
DefineSql(StmtDef,StmtOperater)

--local function PvPCheckPlayer(res, RowNum, PlayerId)
--	local TempScore = 0
--	local TempPlayerId = PlayerId
--	for i= 0, res:GetRowNum()-1 do
--		if (res:GetData(RowNum,1) == res:GetData(i,1)) and (TempScore <= res:GetData(i,2)) then
--			TempPlayerId = res:GetData(i,0)
--			TempScore = res:GetData(i,2)
--		end
--	end
--	if PlayerId == TempPlayerId then
--		return true
--	else
--		return false
--	end
--end

local function GetDate_Str()
	local nowDate = os.date("*t",os.time())
	return nowDate.year, nowDate.month, nowDate.day
end

function JoinActionDB.CheckJoinPVPAction(PlayerId)
	--看是不是在日期内,是的话,才进行判断
	local year, month, day = GetDate_Str()
	local CountNum = PVP_ActionRule_Server("\"" .. year .. "-" .. month .. "-" .. day .. "\"", "Num")
	if CountNum and IsNumber(CountNum)then
		local res = StmtOperater._GetXianxieRes:ExecStat(year, month, day)
		if res and res:GetRowNum()>0 then
			--首先,如果玩家ID在范围之内,并且不是最后一个,那么
			local isTrue = false
			local iNum = nil
			for i=0, res:GetRowNum()-1 do
				if PlayerId == res:GetData(i,0) then
					iNum = i
					break
				end
			end
			
			if iNum and iNum < CountNum then
				isTrue = true
			end
			
			return isTrue
		end
	end
	return true
end

return JoinActionDB
