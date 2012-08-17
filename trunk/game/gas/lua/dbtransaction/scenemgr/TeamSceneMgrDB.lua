
local StmtContainer = class()
local TeamSceneMgrDB = CreateDbBox(...)

local StmtDef=
{
	"GetTeamScene",
	[[
		select
			st.sc_uId, s.sc_uServerId
		from 
			tbl_scene_team as st, tbl_scene as s
		where
			st.sc_uId = s.sc_uId and
			st.t_uId = ? and
			s.sc_sSceneName = ? and
			s.sc_uProcess = ?
		order by st.sc_uId
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"DeleteTeamScene",
	"delete from tbl_scene_team where sc_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"InsertTeamScene",
	"insert ignore into tbl_scene_team(sc_uId, t_uId) values(?,?)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"UpdateTeamScene",
	"update tbl_scene_team set t_uId=?, t_uBelongID=? where sc_uId=?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetSceneType",
	"select sc_uType from tbl_scene where sc_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetCharChangeToScene",
	"select sc_uId from tbl_char_change_position where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetCharFbScene",
	"select sc_uId from tbl_char_fb_position where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"_GetSceneTeamId",
	"select t_uId,t_uBelongID from tbl_scene_team where sc_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--哪些类型的副本是队伍副本
local TeamSceneType = {
[5] = true,
[12] = true,
[15] = true,
[16] = true,
[18] = true,
[21] = true,
[23] = true,
}

function TeamSceneMgrDB.IsTeamSceneTypeByType(sceneType)
	return TeamSceneType[sceneType]
end

local function IsTeamSceneType(sceneId)
	local result = StmtContainer.GetSceneType:ExecSql("n", sceneId)
	if result:GetRowNum() == 0 then
		result:Release()
		return false
	end
	local sceneType = result:GetData(0,0)
	result:Release()
	return TeamSceneType[sceneType]
end

local function GetSceneTeamId(sceneId)
	local result = StmtContainer._GetSceneTeamId:ExecSql("n", sceneId)
	if result:GetRowNum() == 0 then
		result:Release()
		return false
	end
	local sceneTeamID, memberID = result:GetData(0,0), result:GetData(0,1)
	result:Release()
	return sceneTeamID, memberID
end


function TeamSceneMgrDB.GetPlayerCurTeamScene(charId)
	local changeToScene = StmtContainer.GetCharChangeToScene:ExecSql("n", charId)
	local sceneId
	if changeToScene:GetRowNum() > 0 then
		sceneId  = changeToScene:GetData(0,0)
		changeToScene:Release()
		if IsTeamSceneType(sceneId) then
			return sceneId
		end
		return
	end
	changeToScene:Release()
	
	local fbScene = StmtContainer.GetCharFbScene:ExecSql("n", charId)
	if fbScene:GetRowNum() > 0 then
		sceneId = fbScene:GetData(0,0)
		fbScene:Release()
		if IsTeamSceneType(sceneId) then
			return sceneId
		end
		return
	end
	fbScene:Release()
end

function TeamSceneMgrDB.CheckInFbScene(data)
	local sceneId = data["sceneId"]
	local charId = data["char_id"]
	if IsTeamSceneType(sceneId) then
		local TempTeamID = GetSceneTeamId(sceneId)
		if not TempTeamID then
			return true
		end
		local GasTeamDB = RequireDbBox("GasTeamDB")
		local teamId = GasTeamDB.GetTeamID(charId)
		if teamId == nil or teamId == 0 then
			return false
		end
		if teamId == TempTeamID then
			return true
		else
			return false
		end
	end
end

function TeamSceneMgrDB.CheckPlayerInFbScene(charId, sceneId)
	if IsTeamSceneType(sceneId) then
		local GasTeamDB = RequireDbBox("GasTeamDB")
		local teamId = GasTeamDB.GetTeamID(charId)
		if teamId == nil or teamId == 0 then
			return false
		end
		local TempTeamID = GetSceneTeamId(sceneId)
		if teamId == TempTeamID then
			return true
		else
			return false
		end
	end
	return true
end

function TeamSceneMgrDB.InsertTeamScene(sceneId, teamId)
	if teamId == 0 or teamId == nil then
		return
	end
--	if not IsTeamSceneType(sceneId) then
--		return
--	else
	StmtContainer.InsertTeamScene:ExecStat(sceneId, teamId)
end

function TeamSceneMgrDB.InsertTeamSceneCheckID(sceneId, teamId, memberID)
	if teamId == 0 or teamId == nil then
		return
	end
	local TempTeamID,ID = GetSceneTeamId(sceneId)
	if not TempTeamID then
		StmtContainer.InsertTeamScene:ExecStat(sceneId, teamId)
	elseif ID == memberID then
		StmtContainer.UpdateTeamScene:ExecStat(teamId, 0, sceneId)
	end
end

function TeamSceneMgrDB.DeleteTeamScene(sceneId)
	StmtContainer.DeleteTeamScene:ExecSql("", sceneId)
end


--获取队伍场景
function TeamSceneMgrDB.GetScene(data)
	local teamId = data["teamId"]
	if teamId == 0 then
		return 
	end
	
	local sceneName = data["SceneName"]
	local proc = data["Proc"] or 0
	local result = StmtContainer.GetTeamScene:ExecSql("nn", teamId, sceneName, proc)
	if result:GetRowNum() == 0 then
		result:Release()
		return
	end
	local sceneId, serverId = result:GetData(0,0) , result:GetData(0,1)
	result:Release()
	return sceneId, serverId  --如果有多个场景 暂时先返回第一个(即先创建的), 需要的话可以改成队长所在的副本
end

function TeamSceneMgrDB.OnCreateScene(sceneId, teamId)
	if IsTeamSceneType(sceneId) then
		TeamSceneMgrDB.InsertTeamScene(sceneId, teamId)
	end
end

function TeamSceneMgrDB.OnCreateTeam(teamId)
	local GasTeamDB = RequireDbBox("GasTeamDB")
	local members = GasTeamDB.GetTeamOnlineMembersId(teamId)
	for _, id in pairs(members) do
		local curTeamScene =  TeamSceneMgrDB.GetPlayerCurTeamScene(id)
		if curTeamScene then
			TeamSceneMgrDB.InsertTeamSceneCheckID(curTeamScene, teamId, id)
		end
	end
end

function TeamSceneMgrDB.OnReleaseTeam(teamId)
	--要返回这个队伍对应的所有的场景ID,和serverID
	--找到每一个玩家
	local GasTeamDB = RequireDbBox("GasTeamDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	
	local SceneInfo = {}
  local team_members = GasTeamDB.GetTeamMembers(teamId)
  for i= 1,#team_members do
  	local member_id = team_members[i][1]
  	SceneMgrDB.DelEnterScenePlayer(member_id)
  	local SceneId =  TeamSceneMgrDB.GetPlayerCurTeamScene(member_id)
  	if SceneId then
  		if not SceneInfo[SceneId] then
  			SceneInfo[SceneId] = {}
  		end
  		local serverId = SceneMgrDB.GetSceneServerPos(SceneId)
  		table.insert(SceneInfo[SceneId],{member_id,serverId})
  	end
  end
  
  local ExitScenePlayerInfo = {}
  for SceneID,Tbl in pairs(SceneInfo) do
  	if #(Tbl) > 1 then
  		for i=1, #(Tbl) do
  			table.insert(ExitScenePlayerInfo,Tbl[i])
  		end
  	elseif Tbl[1] then
  		--如果是一个人,就把这个场景给这个人所拥有
  		local param = {}
  		param["PlayerId"] = Tbl[1][1]
			param["SceneId"] = SceneID
  		SceneMgrDB.EnterAreaFbScene(param)
  		StmtContainer.UpdateTeamScene:ExecStat(0, Tbl[1][1], SceneID)
  	end
  end
  
  return ExitScenePlayerInfo
end

function TeamSceneMgrDB.OnAddMember(memerberID, teamId)
	local curScene =  TeamSceneMgrDB.GetPlayerCurTeamScene(memerberID)
	if curScene then
		TeamSceneMgrDB.InsertTeamSceneCheckID(curScene, teamId, memerberID)
	end
end

function TeamSceneMgrDB.OnLeaveTeam(memerberID, teamId)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local LeaveSceneId =  TeamSceneMgrDB.GetPlayerCurTeamScene(memerberID)
	SceneMgrDB.DelEnterScenePlayer(memerberID)
	
	if LeaveSceneId then
		local GasTeamDB = RequireDbBox("GasTeamDB")
		local members = GasTeamDB.GetTeamOnlineMembersId(teamId)
		for _, id in pairs(members) do
			if memerberID ~= id then
				local SceneId =  TeamSceneMgrDB.GetPlayerCurTeamScene(id)
				if SceneId and SceneId == LeaveSceneId then
					local serverId = SceneMgrDB.GetSceneServerPos(SceneId)
					return serverId	--"该副本已经不属于你."
				end
			end
		end
		
		StmtContainer.UpdateTeamScene:ExecStat(0, memerberID, LeaveSceneId)
	end
	
	return false
end

-- 检测是否有队友在SceneName里
function TeamSceneMgrDB.CheckPlayerInFbBySceneName(data)
	local teamId = data["TeamID"]
	local sceneName = data["SceneName"]
	--local tblMembers = g_TeamMgr:GetMembers(teamId)
	local GasTeamDB = RequireDbBox("GasTeamDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	
	--local SceneInfo = {}
  local team_members = GasTeamDB.GetTeamMembers(teamId)
  
  local HaveTeamMate = false
  local sceneId = nil
  for i= 1,#team_members do
  	if data["PlayerId"] ~= team_members[i][1] then
	  	local member_id = team_members[i][1]
	  	sceneId =  SceneMgrDB.GetPlayerCurScene(member_id)
	  	if SceneMgrDB._GetSceneNameById(sceneId) == sceneName then
				--print("有队友在，返回ID"..sceneId)
				HaveTeamMate = true
				break
				--return sceneId		-- 有队友在
			else
				--print("无队友在，返回场景名"..sceneName)
				HaveTeamMate = false
				--return sceneName
			end
		end
  end	
  
  if HaveTeamMate then
  	return sceneId
  else
  	return sceneName
  end
end

return TeamSceneMgrDB
