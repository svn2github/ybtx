
-----------------------------------
gas_require "relation/RevertValidateMgr"
gac_gas_require "team/TeamMgr"
local DefaultTeamMode = EAssignMode.eAM_NeedAssign
local DefaultStandard = EAuctionStandard.eAS_GreenStandard
local DefaultBasePrice = 0
local RevertValidateMgr = RevertValidateMgr:new()
local StmtOperater = {}		
local GasTeamBox = CreateDbBox(...)
------------------------------------------相关sql-----------------------------------------------------
local StmtDef = {
    	"_AddTeamStaticInfo",
    	[[ 
    		insert into tbl_team_static values();
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--【得到teamid】
--ps：条件是角色id
local StmtDef = {
    	"_GetTeamId",
    	[[ 
    		select t_uId from tbl_member_team where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【获得队伍人数】
--ps：条件是角色id
local StmtDef = {
    	"_CountTeamMems",
    	[[ 
    		select count(*) from tbl_member_team where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【增加队员】
local StmtDef = {
    	"_AddTeamMember",
    	[[ replace into tbl_member_team(cs_uId, t_uId ) values(?,?) ]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除成员】
local StmtDef = {
    	"_DeleteTeamMember",
    	[[ 
    		delete from  tbl_member_team where cs_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelStaticTeamID",
    	[[ 
    		delete from  tbl_team_static where t_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【创建小队】
--ps1：小队id自增
--ps2：参数是队长的id
local StmtDef = {
    	"_AddTeam",
    	[[ 
    		insert into tbl_team(t_uId,cs_uId,t_uAssignment)  values(?,?,?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除小队】
local StmtDef = {
    	"_DeleteTeam",
    	[[ 
    		delete  from  tbl_team where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除小队所有成员】
--ps：条件：小队id
local StmtDef = {
    	"_DeleteAllTeamMembers",
    	[[ 
    		delete  from  tbl_member_team where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除小队所有标记信息】
--条件：小队id
local StmtDef = {
    	"_DeleteAllTeamMarks",
    	[[ 
    		delete  from  tbl_mark_team where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【修改队长】
--条件：新队长id、小队id
local StmtDef = {
    	"_UpdateTeamCaptain",
    	[[ 
    		 update  tbl_team set  cs_uId = ?  where t_uId = ?
    	]]
}    

DefineSql ( StmtDef, StmtOperater )

--【删除列阵信息】
--条件：小队id
local StmtDef = {
			"_DeleteArrayBattle",
			[[ 
				delete from	tbl_battle_array where t_uId = ? 
			]]
}
DefineSql ( StmtDef, StmtOperater )

--【设置分配方式】
local StmtDef = {
			"_UpdateAssignMode",
			[[ 
    		 update  tbl_team set  t_uAssignment = ?  where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

--【分配方式中拍卖模式的拍卖品质设定】 
local StmtDef = {
			"_UpdateAuctionStandard",
			[[ 
    		 update  tbl_team set  t_uAuctionStandard = ?  where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

--【分配方式中拍卖模式的拍卖底价设定】 
local StmtDef = {
			"_UpdateAuctionBasePrice",
			[[ 
    		 update  tbl_team set  t_uAuctionBasePrice = ?  where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )


local StmtDef = {
			"_GetTeamAssignAndAuction",
			[[ 
    		 select t_uAssignment,t_uAuctionStandard,t_uAuctionBasePrice from tbl_team where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

--【获得小队分配方式】
local StmtDef = {
			"_GetTeamAssignMode",
			[[ 
    		 select t_uAssignment from tbl_team where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )


--【获得小队分配方式中拍卖模式的拍卖品质 】  
local StmtDef = {
			"_GetTeamAuctionStandard",
			[[ 
    		 select t_uAuctionStandard from tbl_team where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )


--【获得小队分配方式中拍卖模式的拍卖底价 】  
local StmtDef = {
			"_GetTeamAuctionBasePrice",
			[[ 
    		 select t_uAuctionBasePrice from tbl_team where t_uId = ?
    	]]
}
DefineSql ( StmtDef, StmtOperater )

--【得到小队成员信息】
local StmtDef = {
    	"_GetTeamMembers",
    	[[ 
    		select cs_uId  from  tbl_member_team  where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetTeamOnlineMembers",
    	[[ 
    		select 
    			mt.cs_uId  
    		from  
    			tbl_member_team as mt, tbl_char_online as co 
    		where 
    			mt.t_uId = ? and co.cs_uId = mt.cs_uId
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_CountOnlineMembersByServer",
    	[[ 
    		select 
    			count(mt.cs_uId)  
    		from  
    			tbl_member_team as mt, tbl_char_online as co 
    		where 
    			mt.t_uId = ? and co.cs_uId = mt.cs_uId and co.co_uOnServerId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【得到小队成员id和名称】
local StmtDef = {
    	"_GetTeamMates",
    	[[ 
    		select		team.cs_uId,chr.c_sName, ifnull(co_uOnServerId,0), cstatic.cs_uClass,cstatic.cs_uSex,cbasic.cb_uLevel
    		from 		tbl_member_team as team,
    					tbl_char as chr,
    					tbl_char_static as cstatic,
    					tbl_char_basic as cbasic
    		left join
    				tbl_char_online as online on online.cs_uId = cbasic.cs_uId
    		where	team.cs_uId = chr.cs_uId
    		and		chr.cs_uId = cstatic.cs_uId
    		and 	cstatic.cs_uId = cbasic.cs_uId
    		and		team.t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【获得队长id】
local StmtDef = {
    	"_GetCapationByTeam",
    	[[ 
    		select  cs_uId  from  tbl_team where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【查询列阵信息(根据小队ID)】
local StmtDef = {
			"_QueryArrayBattleByTeamID",
			[[ 
				select bs_uId,ba_uLoc1,ba_uLoc2,ba_uLoc3,ba_uLoc4,ba_uLoc5 from	tbl_battle_array where t_uId=?
			]]
}
DefineSql (StmtDef,StmtOperater)

--【更新列阵信息(根据流水号和小队ID)】
local StmtDef = {
			"_UpdateArrayBattle",
			[[ 
				update tbl_battle_array set ba_uLoc1=? , ba_uLoc2=? , ba_uLoc3=? , ba_uLoc4=? , ba_uLoc5=?  where bs_uId=? and t_uId=?
			]]
}
DefineSql (StmtDef,StmtOperater)

--【得到小队中所有在线成员】
--ps：用于服务器启动时加载所有小队的信息
--ps2：返回小队id，玩家id、分配方式、拍卖品质
local StmtDef = {
			"_SelectAllMembersOnline",
			[[ 
				select   member.cs_uId from  tbl_member_team as member,tbl_char_online as online
				where 	member.cs_uId = online.cs_uId
				and 		member.cs_uId != ?
				and			member.t_uId = ?
				limit 1;
			]]
}
DefineSql (StmtDef,StmtOperater)

-------------------------------------------数据库操作相关部分------------------------------------------
function GasTeamBox.GetTeamMembersInfo(uTeamID)
	local query_list = StmtOperater._GetTeamMates:ExecStat(uTeamID)

	return query_list
end

function GasTeamBox.GetSomeMemberOnlineByTeamID(uTeamID,uPlayerID)
 	local query_list =  StmtOperater._SelectAllMembersOnline:ExecSql('n', uPlayerID,uTeamID)
 	if query_list:GetRowNum() == 0 then
 		return uPlayerID
 	else
 		return query_list:GetNumber(0,0)
 	end
end

function GasTeamBox.AddTempTeamlog(data)
 	local team_id = data.team_id
 	local members = data.member_id
 	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
  local g_LogMgr = RequireDbBox("LogMgrDB")
 	for i=1,#members do
 		local member_id = members[i]
 		local member_level = CharacterMediatorDB.GetPlayerLevel(member_id)
 		g_LogMgr.EnterTeam(member_id,team_id,member_level)
	end
end

--[[
		【功能：获得小队id
			条件：角色id】
--]]
function GasTeamBox.GetTeamID(uPlayerID)
 	local team_id =  StmtOperater._GetTeamId:ExecSql('n', uPlayerID)
 	if team_id:GetRowNum() == 0 then
 	 	team_id:Release()
 	 	return 0
 	end
  local teamID = team_id:GetNumber(0,0)
  team_id:Release()
 	return teamID
end

function GasTeamBox.CountTeamMems(nTeamID)
 	local count =  StmtOperater._CountTeamMems:ExecSql('n', nTeamID)
 	if count:GetRowNum() == 0 then
 	 	count:Release()
 	 	return 0
 	end
  local nCount = count:GetNumber(0,0)
  count:Release()
 	return nCount
end

-----------------------------------------------------------

--【增加队员】
function GasTeamBox.AddMembers(player_id, team_id)
  StmtOperater._AddTeamMember:ExecSql('', player_id, team_id)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false  
  end
  
  local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
  TeamSceneMgrDB.OnAddMember(player_id, team_id)
  
  --添加log信息
 	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
  local g_LogMgr = RequireDbBox("LogMgrDB")
 	local member_level = CharacterMediatorDB.GetPlayerLevel(player_id)
  g_LogMgr.EnterTeam(player_id, team_id,member_level)
  
  return true
end

---------------------------------------------------------------
--【根据角色id删除队员】
function GasTeamBox.DeleteMembers(player_id, team_id)
	 --添加log信息
  local g_LogMgr = RequireDbBox("LogMgrDB")
  g_LogMgr.LeaveTeam(player_id, GasTeamBox.GetTeamID(player_id))
  
  --队伍副本场景的清理
  local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
  local JoinActionDB  = RequireDbBox("JoinActionDB")
  local IsLeaveScene = TeamSceneMgrDB.OnLeaveTeam(player_id, team_id)
  JoinActionDB.OnLeaveTeam(player_id, team_id)
  local SceneMgrDB = RequireDbBox("SceneMgrDB")
  SceneMgrDB.OnLeaveTeam(player_id, team_id)
  
  StmtOperater._DeleteTeamMember:ExecSql('', player_id)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false  
  end
  return true, IsLeaveScene
end
---------------------------------------------------------------
function GasTeamBox.GetTeamStaticID(data)
	StmtOperater._AddTeamStaticInfo:ExecStat()
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return  0 
  end
	local team_id = g_DbChannelMgr:LastInsertId()
	return team_id,data
end

--【创建小队】
function GasTeamBox.AddTeam(uCharID,uTeamMode)
		local team_id = GasTeamBox.GetTeamStaticID()
  	StmtOperater._AddTeam:ExecStat(team_id,uCharID,uTeamMode)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return  0 
  	end
  	local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
  	TeamSceneMgrDB.OnCreateTeam(team_id)
  	
		return team_id
end
--------------------------------------------------------
--【删除小队】
function GasTeamBox.DeleteTeam(team_id)
  local team_members = GasTeamBox.GetTeamMembers(team_id)
  local g_LogMgr = RequireDbBox("LogMgrDB")
  for i= 1,#team_members do
  	local member_id = team_members[i][1]
  	g_LogMgr.LeaveTeam(member_id, team_id)
  end
  
  --在删除队伍之前调用队伍绑定的场景
 	local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
 	local JoinActionDB = RequireDbBox("JoinActionDB")
  local LeaveSceneTbl = TeamSceneMgrDB.OnReleaseTeam(team_id)
  JoinActionDB.OnReleaseTeam(team_id)
  local SceneMgrDB = RequireDbBox("SceneMgrDB")
  SceneMgrDB.OnReleaseTeam(team_id)
  
  --删除小队成员
 	StmtOperater._DeleteAllTeamMembers:ExecStat(team_id)
 	--删除列阵信息
 	StmtOperater._DeleteArrayBattle:ExecStat(team_id)
 	--删除小队所有标记信息
 	StmtOperater._DeleteAllTeamMarks:ExecStat(team_id)
 	--删除小队信息
  StmtOperater._DeleteTeam:ExecStat(team_id)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return
  end
  StmtOperater._DelStaticTeamID:ExecStat(team_id)
  
  return true, LeaveSceneTbl
end

------------------------------------------------------------
--【设置队长】
function GasTeamBox.SetCaptain(player_id,team_id)
		StmtOperater._DeleteArrayBattle:ExecSql('',team_id)
  	StmtOperater._UpdateTeamCaptain:ExecSql('', player_id, team_id)
  	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  		CancelTran()
  		return
  	end
  	return true
end
--------------------------------------------------------------
--【设置分配方式】
function GasTeamBox.SetAssignMode(team_id, eAM)
	StmtOperater._UpdateAssignMode:ExecSql('', eAM, team_id)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return
  end
  return true
end

--------------------------------------------------------------
--【设置分配方式中的拍卖品质】
function GasTeamBox.SetAuctionStandard(team_id, AuctionStandard)
	StmtOperater._UpdateAuctionStandard:ExecStat(AuctionStandard, team_id)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		return
	end
	return true
end
--【设置分配方式中的拍卖底价】
function GasTeamBox.SetAuctionBasePrice(team_id, AuctionBasePrice)
	StmtOperater._UpdateAuctionBasePrice:ExecStat(AuctionBasePrice, team_id)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0) then
		CancelTran()
		return
	end
	return true
end
--------------------------------------------------------------
--【获得小队分配方式】
function GasTeamBox.GetAssignMode(team_id)
	local ret = StmtOperater._GetTeamAssignMode:ExecSql('n', team_id)
  	if ret:GetRowNum() == 0  then
  		return
  	end
  	return ret:GetData(0,0)
end

function GasTeamBox.GetAssignAndAuction(team_id)
	local ret = StmtOperater._GetTeamAssignAndAuction:ExecStat(team_id)
  	if ret:GetRowNum() == 0  then
  		return 0,0,0
  	end
  	return ret:GetData(0,0),ret:GetData(0,1),ret:GetData(0,2)
end

--------------------------------------------------------------
--【获得拍卖模式的品阶设定】
function GasTeamBox.GetAuctionStandard(team_id)
	local ret = StmtOperater._GetTeamAuctionStandard:ExecSql('n', team_id)
  if ret:GetRowNum() == 0  then
  	return
  end
  return ret:GetData(0,0)
end

--【获得拍卖模式的底价设定】
function GasTeamBox.GetAuctionBasePrice(team_id)
	local ret = StmtOperater._GetTeamAuctionBasePrice:ExecStat(team_id)
  if ret:GetRowNum() == 0  then
  	return 0
  end
  return ret:GetData(0,0)
end

-------------------------------------------------------------
--【获得某小队所有成员】
--返回二维表
function GasTeamBox.GetTeamMembers(team_id)
  local query_list = StmtOperater._GetTeamMembers:ExecStat(team_id)
  local row = query_list:GetRowNum()
	local res = {}
	for i = 1, row do
		table.insert(res,{query_list(i,1)})
	end
	query_list:Release()
	return res
end

--获取某个服务器的所有在线成员
function GasTeamBox.GetOnlineMembersByServer(team_id,server_id)
	local result = StmtOperater._CountOnlineMembersByServer:ExecStat(team_id,server_id)
	return result:GetData(0,0)
end

-------------------------------------------------------------
--【通过小队id获得小队队长id】
function GasTeamBox.GetCaptain(team_id)
  	local query_list = StmtOperater._GetCapationByTeam:ExecStat(team_id )
  	if query_list:GetRowNum() == 0 then
  		query_list:Release()
  		return 0
  	end
  	local cap = query_list:GetData(0,0)
  	query_list:Release()
  	return  cap
end

--获取所以玩家在线成员id
function GasTeamBox.GetTeamOnlineMembersId(team_id)
	local result = StmtOperater._GetTeamOnlineMembers:ExecStat(team_id)
	local row = result:GetRowNum()
	local onlineMember = {}
	for i=1,row do
		table.insert(onlineMember,result(i,1))
	end
	return onlineMember
end

--【玩家离开小队时删除玩家再该小队里的所有列阵信息】
function GasTeamBox.DeletePlayerIDFromBattleArray(LeaverID, teamId)
	local result_list = StmtOperater._QueryArrayBattleByTeamID:ExecSql("nnnnnn",teamId)
	if result_list:GetRowNum() > 0 then
		for i = 0,result_list:GetRowNum()-1 do
			for j = 1,5 do
				if (result_list:GetData(i,j) == LeaverID) then
					if j == 1 then
						StmtOperater._UpdateArrayBattle:ExecSql("",null,result_list:GetData(i,j+1),result_list:GetData(i,j+2),result_list:GetData(i,j+3),result_list:GetData(i,j+4),result_list:GetData(i,0),teamId)
					elseif j == 2 then
						StmtOperater._UpdateArrayBattle:ExecSql("",result_list:GetData(i,j-1),null,result_list:GetData(i,j+1),result_list:GetData(i,j+2),result_list:GetData(i,j+3),result_list:GetData(i,0),teamId)
					elseif j == 3 then
						StmtOperater._UpdateArrayBattle:ExecSql("",result_list:GetData(i,j-2),result_list:GetData(i,j-1),null,result_list:GetData(i,j+1),result_list:GetData(i,j+2),result_list:GetData(i,0),teamId)
					elseif j == 4 then
						StmtOperater._UpdateArrayBattle:ExecSql("",result_list:GetData(i,j-3),result_list:GetData(i,j-2),result_list:GetData(i,j-1),null,result_list:GetData(i,j+1),result_list:GetData(i,0),teamId)
					elseif j == 5 then
						StmtOperater._UpdateArrayBattle:ExecSql("",result_list:GetData(i,j-4),result_list:GetData(i,j-3),result_list:GetData(i,j-2),result_list:GetData(i,j-1),null,result_list:GetData(i,0),teamId)
					end
					break
				end
			end
		end
	end
end
-----------------------------------------------------------------
--【转移队长(队长离开队伍时）】
function GasTeamBox.AutoChangeTeamCapatin(team_id,LeaverID)
	local tblMembers = GasTeamBox.GetTeamMembers(team_id) 
		
	local new_cap = LeaverID
	
	--先找在线的人
	--如果都不在线，转移给没在线的
	local new_cap = GasTeamBox.GetSomeMemberOnlineByTeamID(team_id,LeaverID)
	if new_cap == LeaverID then
		for i=1, #tblMembers  do
			local PlayerID = tblMembers[i][1]
			if(PlayerID ~= LeaverID) then
				new_cap = PlayerID
				break
			end
		end
	end
	GasTeamBox.SetCaptain(new_cap,team_id)
	return new_cap
end

-------------------------------------------------RPC调用相关操作函数---------------------------------------------------------
--【邀请组队】
function GasTeamBox.InviteTeam(parameters)
 	local uCharID = parameters.uCharID
 	local uTargetID = parameters.uTargetID
 
 	local setting_box = RequireDbBox("GameSettingDB")
 	if setting_box.GetOneSettingInfo(uTargetID,1) ~= 1 then
 		--系统设置为不可组队状态
 		return false, 166
 	end
 	local LoginServerSql = RequireDbBox("LoginServerDB")
	if not LoginServerSql.IsPlayerOnLine(uTargetID) then
		--对方不在线
		return false,110
	end
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	if CharacterMediatorDB.GetCamp(uTargetID) ~= CharacterMediatorDB.GetCamp(uCharID) then
		--阵营不同
		return false,112
	end
	
	local char_team_id = GasTeamBox.GetTeamID(uCharID)
	local target_team_id = GasTeamBox.GetTeamID(uTargetID)
	local inviterid,inviteeid = uCharID,uTargetID
	local team_cap,TeamMode = uCharID,DefaultTeamMode
	if char_team_id > 0 and target_team_id > 0 then
		--两人都已经加入队伍
		return false, 111
		
	elseif  char_team_id > 0 then
		--邀请者已经在队伍中
		local char_team_cap = GasTeamBox.GetCaptain(char_team_id)
		if char_team_cap ~= uCharID then
			--邀请者不是队长
			return false, 114
		else
			local members = GasTeamBox.GetTeamMembers(char_team_id)
			if #members >= 5 then
					return false,113
			end
			TeamMode = GasTeamBox.GetAssignAndAuction(char_team_id)
		end
	elseif target_team_id > 0 then
		--被邀请者已经加入队伍、则邀请者相当于申请加入被邀请者的队伍
		local members = GasTeamBox.GetTeamMembers(target_team_id)
		if #members >= 5 then
			return false,116
		end
		team_cap = GasTeamBox.GetCaptain(target_team_id)
		TeamMode = GasTeamBox.GetAssignMode(target_team_id) or 0
		inviteeid = team_cap
	else
		--邀请者和被邀请者都没有加入队伍,创建一个队伍
	end
	
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.AddValidateInfo(inviterid,inviteeid,RevertValidateMgr:GetFunIndexByStr("MakeTeam")) then
		return
	end
	return true,team_cap,TeamMode 
end

function GasTeamBox.InviteTeamByName(parameters)
 	local uCharID = parameters.uCharID
 	local uTargetName = parameters.uTargetName
 	local ex = RequireDbBox("Exchanger")
	local uTargetID = ex.getPlayerIdByName(uTargetName)
	if 0 == uTargetID then
		return
	end
	local bFlag,team_cap,assign_mode = GasTeamBox.InviteTeam({["uTargetID"] = uTargetID,["uCharID"] = uCharID})
	return bFlag,team_cap,assign_mode,uTargetID
end

--【响应邀请】
function GasTeamBox.RespondInvite(parameters)
 	local uCharID = parameters.uCharID
 	local InviterID = parameters.InviterID
 	local InviterName = parameters.InviterName
 	local bAccept = parameters.bAccept
 	
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.DelValidateInfo(InviterID,uCharID,RevertValidateMgr:GetFunIndexByStr("MakeTeam")) then
		return
	end
	
	local ex = RequireDbBox("Exchanger")
	local char_name = ex.getPlayerNameById(uCharID)
 	if bAccept then
 		local LoginServerSql = RequireDbBox("LoginServerDB")
		if not LoginServerSql.IsPlayerOnLine(InviterID) then
			--对方不在线
			return false,106,InviterName
		end
	else
		local ErrRes = {}
		ErrRes.CharName = char_name
		return true,ErrRes
	end
	
	--先获得小队id
	local self_team_id = GasTeamBox.GetTeamID(uCharID)
	local target_team_id = GasTeamBox.GetTeamID(InviterID)
	
	if self_team_id > 0  then
		return false,133
	end
	
	local RetData 
	local sFlag 
	if target_team_id > 0 then
		local team_cap = GasTeamBox.GetCaptain(target_team_id)  
		if team_cap ~= InviterID then
			return false,141, InviterName
		end
		local param = {}
		param.uTeamID = target_team_id
		param.uTargetID = uCharID
		RetData = GasTeamBox.AddMember(param)
		sFlag = "AddMember"
	else
		local param = {}
		param.uCharID = InviterID 
		param.uTargetID = uCharID
		RetData = GasTeamBox.CreateTeam(param)
		sFlag = "CreateTeam"
	end
	if not IsTable(RetData) then
		return false,RetData
	end
	local mark_box = RequireDbBox("GasTeamMarkDB")
	local MarkInfo = mark_box.GetAllMarkInfo(TeamID)
	
	RetData.CharName = char_name
	RetData.MarkInfo = MarkInfo
	return sFlag,RetData
end

-----------------------------------------------------------
--【响应申请】
function GasTeamBox.RespondApp(parameters)
 	local uCharID = parameters.uCharID
 	local InviterID = parameters.InviterID
 	local InviterName = parameters.InviterName
 	local bAccept = parameters.bAccept
 	
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.DelValidateInfo(InviterID,uCharID,RevertValidateMgr:GetFunIndexByStr("MakeTeam")) then
		return
	end
	
	local ex = RequireDbBox("Exchanger")
	local char_name = ex.getPlayerNameById(uCharID)
 	if bAccept then
 		local LoginServerSql = RequireDbBox("LoginServerDB")
		if not LoginServerSql.IsPlayerOnLine(InviterID) then
			--对方不在线
			return false,135
		end
	else
		local ErrRes = {}
		ErrRes.CharName = char_name
		return true,ErrRes
	end
	
	local TeamID = GasTeamBox.GetTeamID(uCharID)
	if TeamID == 0 then return end
	
	if GasTeamBox.GetTeamID(InviterID) > 0 then
		--对方已经加入了队伍
		return false,134,InviterName
	end
	
	local param = {}
	param.uTeamID = TeamID
	param.uTargetID = InviterID
	local RetData = GasTeamBox.AddMember(param)
	
	if not IsTable(RetData) then
		return false,RetData
	end
	local mark_box = RequireDbBox("GasTeamMarkDB")
	local MarkInfo = mark_box.GetAllMarkInfo(TeamID)
	
	RetData.CharName = char_name
	RetData.MarkInfo = MarkInfo

	return "AddMember",RetData
end
------------------------------------------------------------

--【创建队伍】
function GasTeamBox.CreateTeam(parameters)
	local uCharID = parameters.uCharID
	local uTargetID = parameters.uTargetID
	
	--判断创建队伍的两人是否有人已经加入了队伍
	if GasTeamBox.GetTeamID(uCharID) ~= 0 or 0 ~= GasTeamBox.GetTeamID(uTargetID) then
		return 
	end
	
	--给数据库中添加一个小队
	local TeamID = GasTeamBox.AddTeam(uCharID,DefaultTeamMode)
	if TeamID == 0 then return end

	--往数据库中的此小队添加成员
	GasTeamBox.AddMembers(uCharID,TeamID)
	GasTeamBox.AddMembers(uTargetID,TeamID)
	
	GasTeamBox.SetAuctionStandard(TeamID,DefaultStandard)
	
	local data = {}
	data.eAM = DefaultTeamMode
	data.AuctionStandard = DefaultStandard
	data.AuctionBasePrice = DefaultBasePrice
	data.uTeamID = TeamID
	return data
end

------------------------------------------------------------

--【解散队伍】
function GasTeamBox.BreakTeam(parameters)
	local uCharID = parameters.uCharID
	local uTeamID = GasTeamBox.GetTeamID(uCharID)
	if uTeamID == 0 then return end

	local captain = GasTeamBox.GetCaptain(uTeamID)
  if uCharID ~= captain then return end
 
  local team_members = GasTeamBox.GetTeamMembers(uTeamID)
  --删除小队及其成员
  local res, LeaveSceneTbl = GasTeamBox.DeleteTeam(uTeamID)
  if not res then return end
 
  local data = {}
  data.m_TeamCap = captain
  data.m_tblTeamMeb = team_members
  data.m_nTeamID = uTeamID
  data.LeaveSceneTbl = LeaveSceneTbl
  return data
end

----------------------------------------------------------

--【增加成员】
function GasTeamBox.AddMember(parameters)
	local uTeamID = parameters.uTeamID
	local uTargetID = parameters.uTargetID
	if uTeamID == 0 then 
		return 
	end
	local team_members = GasTeamBox.GetTeamMembers(uTeamID)
	
	if #team_members == 0 then
		--该小队不存在
		return 
	elseif #team_members >= 5 then
		--人数满了
		return 113
	end
	
	local team_id = GasTeamBox.GetTeamID(uTargetID)
	if team_id ~= 0 then return end

	if not GasTeamBox.AddMembers(uTargetID,uTeamID) then
		return 
	end
	
	--分配方式相关
	local eAM,AuctionStandard,AuctionBasePrice = GasTeamBox.GetAssignAndAuction(uTeamID)
	local team_members2 = GasTeamBox.GetTeamMembers(uTeamID)
	
	local team_cap = GasTeamBox.GetCaptain(uTeamID)
	local data = {}
	data.eAM = eAM
	data.AuctionStandard = AuctionStandard
	data.AuctionBasePrice = AuctionBasePrice
	data.team_members = team_members2
	data.team_cap = team_cap
	data.uTeamID = uTeamID
	return data
end

--------------------------------------------------------

--【离开队伍】
function GasTeamBox.LeaveTeam(parameters)
	local LeaverID = parameters.LeaverID
	local team_id = GasTeamBox.GetTeamID(LeaverID)
	if team_id == 0 then return end

	local uNewCap = 0
	--删除列阵信息
	GasTeamBox.DeletePlayerIDFromBattleArray(LeaverID,team_id)
	
  local LeaveSceneTbl = {}
  local res,IsLeaveScene = GasTeamBox.DeleteMembers(LeaverID,team_id)
  if not res then return end

   --人数小于2自动解散
  local tblMembers = GasTeamBox.GetTeamMembers(team_id)
  if #tblMembers < 2 then
    	local res, LeaveTbl = GasTeamBox.DeleteTeam(team_id)
		  if not res then return end
		  LeaveSceneTbl = LeaveTbl
  else
  		--队长离开
  		local captain = GasTeamBox.GetCaptain(team_id)
  		if LeaverID == captain then
	  		uNewCap = GasTeamBox.AutoChangeTeamCapatin(team_id,LeaverID)
			end
	end
 	
 	if IsLeaveScene then
		table.insert(LeaveSceneTbl,{LeaverID,IsLeaveScene})
	end
	
	local data = {}
	data.m_nNewTeamCap  = uNewCap
	data.m_tblTeamMem = tblMembers
	data.m_nTeamID = team_id
	data.LeaveSceneTbl = LeaveSceneTbl
	return data
end
------------------------------------------------------------
--【剔除队员】
function GasTeamBox.RemoveTeamMember(parameters)
	local LeaverID = parameters.LeaverID
	local uPlayerID = parameters.uPlayerID
		
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	local captain = GasTeamBox.GetCaptain(uTeamID)
  if uPlayerID ~= captain then return end
 
  if GasTeamBox.GetTeamID(LeaverID) ~= uTeamID then return end
	
	--删除列阵信息
	GasTeamBox.DeletePlayerIDFromBattleArray(LeaverID,uTeamID)
	
  local LeaveSceneTbl = {}
  local res,IsLeaveScene = GasTeamBox.DeleteMembers(LeaverID, uTeamID)
  if not res then return end
 
   --人数小于2 ，自动解散
  local tblMembers = GasTeamBox.GetTeamMembers(uTeamID)
  if #tblMembers < 2 then
    	local res, LeaveTbl = GasTeamBox.DeleteTeam(uTeamID)
		  if not res then return end
		  LeaveSceneTbl = LeaveTbl
   end
   
	if IsLeaveScene then
		table.insert(LeaveSceneTbl,{LeaverID,IsLeaveScene})
	end
	
  local ex = RequireDbBox("Exchanger")
	local leaver_name = ex.getPlayerNameById(LeaverID)
	local data = {}
	data.m_TeamCap = captain
	data.m_tblTeamMem = tblMembers
	data.m_nTeamID = uTeamID
	data.m_LeaverName = leaver_name
	data.LeaveSceneTbl = LeaveSceneTbl	
  return data
end

------------------------------------------------------

--【设置队长】
function GasTeamBox.SetCaptainRPC(parameters)
	local uTargetID = parameters.uTargetID
	local uPlayerID = parameters.uPlayerID
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	local team_members = GasTeamBox.GetTeamMembers(uTeamID)
	if GasTeamBox.GetCaptain(uTeamID) ~= uPlayerID then
		--说明设置队长的是目前不是队长 ，没有权限
		return false
	end
	local LoginServerSql = RequireDbBox("LoginServerDB")
	if not LoginServerSql.IsPlayerOnLine(uTargetID) then
		--对方不在线
		return false,110
	end
	
	--设置队长
	if not GasTeamBox.SetCaptain(uTargetID,uTeamID) then 
		return 
	end
	local ex = RequireDbBox("Exchanger")
	local cap_name = ex.getPlayerNameById(uTargetID)
	local res = {}
	res.m_tblTeamMem = team_members
	res.m_nTeamCap = cap_name
	res.m_TeamID = uTeamID
	return true,res
end

--------------------------------------------------

--【设置分配方式】
function GasTeamBox.SetAssignModeRPC(parameters)
	local eAM = parameters.eAM
	local uPlayerID = parameters.uPlayerID
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	if GasTeamBox.GetCaptain(uTeamID) ~= uPlayerID then
		--说明不是队长 ，没有权限
		return 1
	end
	if not GasTeamBox.SetAssignMode(uTeamID,eAM) then
		return 0
	end
	local team_members = GasTeamBox.GetTeamMembers(uTeamID)
	return 2,team_members,uTeamID
end

--【获得小队所有成员】
function GasTeamBox.GetTeamMembersByChar(parameters)
	local uPlayerID = parameters.uPlayerID
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	local tblRes = {}
	tblRes.uTeamID = uTeamID
	if uTeamID == 0 then
		return tblRes
	end
	local team_mates = GasTeamBox.GetTeamMembersInfo(uTeamID)
	
	--队长
	local captain = GasTeamBox.GetCaptain(uTeamID)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local PlayerLevel = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	tblRes.team_mates = team_mates
	tblRes.captain = captain
	tblRes.PlayerLevel = PlayerLevel
	return tblRes
end

function GasTeamBox.GetTeamCapAndMem(parameters)
	local TeamID = parameters.TeamID
	local server_id = parameters.server_id
	local Members = GasTeamBox.GetTeamMembers(TeamID)
	local captain = GasTeamBox.GetCaptain(TeamID)
	local online_membs = GasTeamBox.GetOnlineMembersByServer(TeamID,server_id)
	local data = {}
	data.m_AllMembers = Members
	data.m_TeamCap = captain
	data.m_OnlineMem = online_membs
	data.m_TeamID = TeamID
	return data
end

function GasTeamBox.GetPlayerLevelAndTeamID(parameters)
	local uPlayerID  = parameters.m_uPlayerID
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	local PlayerLevel = CharacterMediatorDB.GetPlayerLevel(uPlayerID)
	return PlayerLevel,uTeamID
end
--------------------------------------------------
--【分配方式中拍卖模式的拍卖品质设定】
function GasTeamBox.SetAuctionStandardRPC(parameters)
	local uPlayerID = parameters.uPlayerID
	local AuctionStandard = parameters.AuctionStandard
	
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	if GasTeamBox.GetCaptain(uTeamID) ~= uPlayerID then
		return 
	end
	if not GasTeamBox.SetAuctionStandard(uTeamID,AuctionStandard) then
		return 
	end
	local members = GasTeamBox.GetTeamMembers(uTeamID)
	return members,uTeamID
end

--【分配方式中拍卖模式的拍卖底价设定】
function GasTeamBox.SetAuctionBasePriceRPC(parameters)
	local uPlayerID = parameters.uPlayerID
	local AuctionBasePrice = parameters.AuctionBasePrice
	
	local uTeamID = GasTeamBox.GetTeamID(uPlayerID)
	if GasTeamBox.GetCaptain(uTeamID) ~= uPlayerID then
		return 
	end
	if not GasTeamBox.SetAuctionBasePrice(uTeamID,AuctionBasePrice) then
		return
	end
	local members = GasTeamBox.GetTeamMembers(uTeamID)
	return members,uTeamID
end

--【删除角色时对小队团队信息的处理】
function GasTeamBox.DealWithTeamInfoWhenDelChar(uCharID)
	local uTeamID = GasTeamBox.GetTeamID(uCharID)
	if uTeamID == 0 then
		return {}
	end
	local parameters = {}
	parameters.LeaverID	= uCharID
	parameters.uTeamID		= uTeamID
	local LeaveData = GasTeamBox.LeaveTeam(parameters)
	
	local tblRetData = {}
	tblRetData.m_nTeamID = uTeamID
	tblRetData.m_tblTeamMembers = LeaveData.m_tblTeamMem
	return tblRetData
end

SetDbLocalFuncType(GasTeamBox.GetTeamMembersByChar)
SetDbLocalFuncType(GasTeamBox.GetPlayerLevelAndTeamID)
--返回闭包
return GasTeamBox