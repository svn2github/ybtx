gas_require "relation/RevertValidateMgr"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
local CFriend = {}

local CTextFilterMgr = CTextFilterMgr
local FriendNumLimit = 100
local RevertValidateMgr = RevertValidateMgr:new()

local FriendDbBox = CreateDbBox(...)

--[[服务器端玩家的好友种类：
--	1: 好友
--	2: 黑名单
--]]

-----------------------------------------------------------------------------------------------------------
--@brief 添加好友
--@param InvitorId:邀请者ID
--@param InviteeId:被加为好友的玩家ID
function FriendDbBox.AddFriendToClass(data)
	local InvitorId = data["InvitorId"]
	local InviteeId = data["InviteeId"]
	local classId = data["classId"]
	--判断是不是能将该玩家加为好友
	local succ, errorMsg = FriendDbBox.CanAddFriend(InvitorId, InviteeId)
	if (succ) then
		if (not errorMsg) then  --对方不在线，把请求信息暂时加入到数据库中
			FriendDbBox.AddOfflineFriendMgr(InvitorId, InviteeId,classId)
			return false, 130004  --"对方不在线"
		end
		return succ
	else
		return succ, errorMsg
	end
end
-----------------------------------------------------------------------------------------------------------
--@brief 添加好友通过玩家name
--@param InvitorId:邀请者ID
--@param playerName:被加为好友的玩家name
function FriendDbBox.AddFriendToClassByName(data)
	local InvitorId = data["InvitorId"]
	local playerName = data["playerName"]
	local classId = data["classId"]
	
	--判断是不是能将该玩家加为好友
	local ex = RequireDbBox("Exchanger")
	local InviteeId = ex.getPlayerIdByName(playerName)
	
	if InviteeId == 0 then
		return false, 130013 --这个玩家不存在
	end
	
	local succ, errorMsg = FriendDbBox.CanAddFriend(InvitorId, InviteeId)
	if (succ) then
		if (not errorMsg) then  --对方不在线，把请求信息暂时加入到数据库中
			FriendDbBox.AddOfflineFriendMgr(InvitorId, InviteeId,classId)
			return false, 130004  --"对方不在线"
		end
		return succ,InviteeId
	else
		return succ, errorMsg
	end
end
-----------------------------------------------------------------------------------------------------------
--@brief 响应加为好友
--@param InvitorId:邀请者ID
--@param InviteeId:被加为好友的玩家ID
function FriendDbBox.RequestAddFriend(data)
	local InviteeId = data["InviteeId"]
	local InvitorId = data["InvitorId"]
	local InvitorClassId = data["InvitorClassId"]
	local InviteeClassId = data["InviteeClassId"]
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.DelValidateInfo(InvitorId,InviteeId,RevertValidateMgr:GetFunIndexByStr("AddFriend")) then
		return false
	end
	if InvitorId == 0 then
		return false,130013 --"该玩家不存在!"
	end
	return FriendDbBox.AddFriend(InvitorId,InvitorClassId,InviteeId,InviteeClassId)
end
-----------------------------------------------------------------------------------------------------------
local StmtDef =
{
	"_GetPlayerLevelCampClass",
	[[
		select
			a.cb_uLevel,
			b.cs_uCamp,
			b.cs_uClass
		from
			tbl_char_basic a,
			tbl_char_static b
		where
			b.cs_uId = a.cs_uId and a.cs_uId = ? 
	]]
}
DefineSql( StmtDef , CFriend )

--@brief 查找玩家等级
--@param charId:邀请者ID
function FriendDbBox.GetPlayerLevelCampClass(charId)
	local tblInfo = {}
	local result = CFriend._GetPlayerLevelCampClass:ExecSql('n',charId)
	if result and result:GetRowNum() > 0 then
		table.insert( tblInfo, result:GetData(0,0) )
		table.insert( tblInfo, result:GetData(0,1) )
		table.insert( tblInfo, result:GetData(0,2) )
		result:Release()
	end
	return tblInfo
end
------------------------------------------------------------------------------------------------------------
--根据玩家ID和好友组Id查找好友组name
local StmtDef = {
		"SelectFriendClassName",
		[[	
			select fc_sName from tbl_friends_class where fc_uId = ? and cs_uId = ?  
		]]
}
DefineSql( StmtDef, CFriend )

--向好友表里添加新玩家
local StmtDef = {
		"AddNewFriend",
		[[
			replace into tbl_player_friends values(?,?,?)
		]]
}
DefineSql( StmtDef, CFriend )

local StmtDef = {
		"SelectPlayerLevel",
		[[	
			select  cb_uLevel from tbl_char_basic where cs_uId = ?
		]]
}
DefineSql( StmtDef, CFriend )

function FriendDbBox.AddFriendCommon(Id,ClassId,BeAddId)
	local g_LogMgr				= RequireDbBox("LogMgrDB")
	local SceneMgrDB			= RequireDbBox("SceneMgrDB")
	local GasTeamDB				= RequireDbBox("GasTeamDB")
	local ex					= RequireDbBox("Exchanger")
	local LoginServerSql		= RequireDbBox("LoginServerDB")
	local nLevel, nCamp, nClass = unpack( FriendDbBox.GetPlayerLevelCampClass(Id) )
	
	local classname = CFriend.SelectFriendClassName:ExecSql("",ClassId,Id)
	
	if( (not classname) or classname:GetRowNum() ) == 0 then
		ClassId = 1
		CFriend.AddNewFriend:ExecSql("",Id,BeAddId,1)
	else
		CFriend.AddNewFriend:ExecSql("",Id,BeAddId,ClassId)
	end
	g_LogMgr.AddFriend(Id,nLevel,BeAddId)
	classname:Release()
	
	local nTeamId		= GasTeamDB.GetTeamID(Id)
	local nSceneId		= SceneMgrDB.GetPlayerCurScene(Id)
	local CTongBasicBox  = RequireDbBox("GasTongBasicDB")
	local tblInfo = {}
	tblInfo.nClassId	= ClassId
	tblInfo.sName		= ex.getPlayerNameById(Id)
	tblInfo.nLevel		= nLevel
	tblInfo.nCamp		= nCamp
	tblInfo.nClass		= nClass
	tblInfo.nTeamSize	= GasTeamDB.CountTeamMems(nTeamId)
	tblInfo.sSceneName	= SceneMgrDB._GetSceneNameById(nSceneId)
	tblInfo.nBeOnline	= LoginServerSql.IsPlayerOnLine(Id) and 1 or 2
	
  local nTongID = CTongBasicBox.GetTongID(Id)
	tblInfo.nTongId = nTongID
	
	return tblInfo
end

--@brief 响应添加好友
function FriendDbBox.AddFriend(InvitorId,InvitorClassId,InviteeId,InviteeClassId)
	local data =	{	{ ["self_id"] = InvitorId, ["player_id"] = InviteeId },
						{ ["self_id"] = InviteeId, ["player_id"] = InviteeId }
					}
	for i = 1, 2 do
		if FriendDbBox.IsBlackName(InvitorId, InviteeId) then
			if not FriendDbBox.DeleteBlackList(data[i]) then
				CancelTran()
				return false
			end
		end
	end
	
	local playerInfo = FriendDbBox.GetFriendInfo(InvitorId, InviteeId)
	if playerInfo then
		if playerInfo == 1 then
			return false  --"该玩家已经在你的好友列表里"
		end
	end
	
	--classId, sName, level, camp, class, teamSize, sceneName, nBeOnline
	local tblInvitor = FriendDbBox.AddFriendCommon(InvitorId,InvitorClassId,InviteeId,InviteeClassId)
	local tblInvitee = FriendDbBox.AddFriendCommon(InviteeId,InviteeClassId,InvitorId,InvitorClassId)
	
	return true, tblInvitor, tblInvitee
end
-----------------------------------------------------------------------------------------------------------
local StmtDef = {
		"Friend_Kind",
		[[
			select fc_uId from tbl_player_friends where cs_uId = ? and pf_uFriendId = ? 
		]]
}
DefineSql( StmtDef, CFriend )
--@brief 查找好友信息
function FriendDbBox.GetFriendInfo(invitorId, inviteeId)
	local query_result = CFriend.Friend_Kind:ExecSql("n", invitorId, inviteeId)
	if query_result == nil then
		return nil
	end
	
	if (0 == query_result:GetRowNum()) then --要加为好友的玩家不在自己的好友列表里面
		query_result:Release()
		return nil
	end
	
	if query_result:GetData(0,0) ~= 2 then
		query_result:Release()
		return 1 --"该玩家已经在你的好友列表里"
	else
		query_result:Release()
		return 2 --"该玩家在你的黑名单列表里"
	end
end
---------------------------------------------------------------------------------------------------------
local StmtDef = {
		"Friend_All",
		[[
			select count(*) from tbl_player_friends	where cs_uId = ? and fc_uId = 1
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 判断能否添加好友请求
function FriendDbBox.CanAddFriend(invitorId, inviteeId)
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	if not RevertValidateBox.AddValidateInfo(invitorId,inviteeId,RevertValidateMgr:GetFunIndexByStr("AddFriend")) then
		return false
	end
	local setting_box = RequireDbBox("GameSettingDB")
 	if setting_box.GetOneSettingInfo(inviteeId,4) ~= 1 then
 		return false, 130000   --对方设置了拒绝好友邀请!
 	end
 	
	if invitorId == inviteeId then					--如果玩家要加的好友是自己
		return false, 130001 --不能将自己加为好友
	end
	
	local playerInfo = FriendDbBox.GetFriendInfo(invitorId, inviteeId)
	if playerInfo ~= nil then
		if playerInfo == 1 then
			return false,130002 --该玩家已经在你的好友列表里
		end
	end
	
	local friends = CFriend.Friend_All:ExecSql("n", invitorId)
	if friends:GetRowNum() >= FriendNumLimit then
		friends:Release()
		return false, 130003 --你的好友数目已经达到上限,不能再加好友了
	end
	friends:Release()
	local LoginServerSql = RequireDbBox("LoginServerDB")
	local isOnline = LoginServerSql.IsPlayerOnLine(inviteeId)	
	return true,isOnline
end
---------------------------------------------------------------------------------------------------------
local StmtDef={
		"Select_OfflineFriend_Mgr",
		[[
			select
				a.c_sName,
				b.fc_uId,
				b.ra_uInviter
			from
				tbl_char a,
				tbl_request_addfriend b
			where a.cs_uId = b.ra_uInviter and b.ra_uInvitee = ?
		
		]]
}
DefineSql(StmtDef, CFriend)

--@brief 好友(被邀请者离线)上线后发送离线请求
--@param inviteeId:被邀请者
function FriendDbBox.SendOfflineFriendRequest(inviteeId)
	local inviterSet = CFriend.Select_OfflineFriend_Mgr:ExecSql('s[32]nn', inviteeId)
	return inviterSet
end
---------------------------------------------------------------------------------------------------------
local StmtDef={
		"Add_OfflineFriend_Mgr",
		[[ replace into tbl_request_addfriend values(?,?,?) ]]
}
DefineSql(StmtDef, CFriend)

--@brief 将好友(被邀请者离线)请求写入到数据库
--@param inviterId:邀请者
--@param inviteeId:被邀请者
function FriendDbBox.AddOfflineFriendMgr(inviterId,inviteeId,classId)
	if inviterId == 0 or inviteeId == 0 then
		return false
	end
	
	CFriend.Add_OfflineFriend_Mgr:ExecSql('', inviterId,inviteeId,classId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

---------------------------------------------------------------------------------------------------------
--上线显示完成后把离线时的添加好友信息从数据库删除；
local StmtDef={
		"Delete_OfflineFriendRequest",
		[[
			delete from tbl_request_addfriend where ra_uInvitee = ? 
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 删除已经显示完的离线消息
function FriendDbBox.DeletOfflineFriendRequest(player_id)
	CFriend.Delete_OfflineFriendRequest:ExecSql('',player_id)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
---------------------------------------------------------------------------------------------------------
local StmtDef = {
		"_SelectBlackName",
		[[
			select fc_uId from tbl_player_friends where cs_uId = ? and pf_uFriendId  = ? 
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 检查是否在黑名单里面
function FriendDbBox.IsBlackName(invitorId, inviteeId)
	local black_info = CFriend._SelectBlackName:ExecSql("n", invitorId, inviteeId)
	if black_info == nil then
		return false
	end
	if black_info:GetRowNum() == 0 then
		black_info:Release()
		return false
	end

	if black_info:GetData(0,0) == 2 then
		black_info:Release()
		return true
	end
	return false
end
---------------------------------------------------------------------------------------------------
--@brief 条件查找好友
--@param area:要查找的玩家所在区域
--@param class:要查找的玩家的职业
--@param gender:要查找的玩家的性别 1-男；2-女
--@param low_level:要查找的玩家的最低等级
--@param up_level:要查找的玩家的最高等级
function FriendDbBox.SearchPlayerByRequest(data)
	local area = data["area"]
	local class = data["class"]
	local gender = data["gender"]
	local low_level = data["low_level"]
	local up_level = data["up_level"]
	local sSceneName = data["sSceneName"]
	local querySql = ""
	local playerTbl = {}
	local query_sql = " select a.cs_uId,a.c_sName,b.cb_uLevel from tbl_char a,tbl_char_basic b,tbl_char_static c,tbl_char_position cp,tbl_scene s,tbl_char_online co where a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId and c.cs_uId = cp.cs_uId and cp.cs_uId = co.cs_uId and cp.sc_uId = s.sc_uId "
	if nil ~= class and class ~= 0 then
		query_sql = query_sql .. " and c.cs_uClass = " .. class
	end
	
	if nil ~= gender and gender ~= 0 then
		query_sql = query_sql .. " and c.cs_uSex = " .. gender
	end
	
	if nil ~= low_level then
		query_sql = query_sql .. " and b.cb_uLevel >= " .. low_level
	end
	
	if nil ~= up_level then
		query_sql = query_sql .. " and b.cb_uLevel <= " .. up_level
	end

	if nil ~= sSceneName and sSceneName ~= "" then
		query_sql = query_sql .. " and s.sc_sSceneName = " .. "'" .. sSceneName .. "'"
	end 
	query_sql = query_sql .. " limit 80 "
	local _, query_result = g_DbChannelMgr:TextExecute(query_sql)
	if query_result == nil or query_result:GetRowNum() == 0 then
		query_result:Release()
		return playerTbl
	end
	
	local LoginServerSql = RequireDbBox("LoginServerDB")
	for i = 1,query_result:GetRowNum() do
		local playerinfo = {}
		playerinfo[1] = query_result:GetData(i-1,0)
		playerinfo[2] = query_result:GetData(i-1,1)
		playerinfo[3] = query_result:GetData(i-1,2)
		playerinfo[4] = LoginServerSql.IsPlayerOnLine(playerinfo[1])
		table.insert(playerTbl,playerinfo)
	end
	query_result:Release()
	return playerTbl
end
---------------------------------------------------------------------------------------------------
--@brief 按照玩家ID或者玩家name精确查找玩家(玩家ID优先查)
--@param playerName：要查找的玩家name
--@param playerId：要查找的玩家Id
function FriendDbBox.SearchPlayerAccurately(data)
	local playerName = data["InviteeName"]
	local playerId = data["InviteeId"]
	local playeyInfo = nil
	if(0 ~= playerId) then
		playeyInfo = FriendDbBox.SearchPlayerAccuratelyById(playerId)
	end
	if(not playeyInfo and "" ~= playerName) then
		playeyInfo = FriendDbBox.SearchPlayerAccuratelyByName(playerName)
	end
	return playeyInfo
end
------------------------------------------------------------------------------------------------------
local StmtDef = {
		"SearchPlayerByName",
		[[
		select
			a.cs_uId,
			a.c_sName,
			b.cb_uLevel,
			ifnull(c.co_uOnServerId,0)
		from
			tbl_char a,
			tbl_char_basic b
		left join
			tbl_char_online c on b.cs_uId = c.cs_uId
		where
			a.cs_uId = b.cs_uId and a.c_sName = ?
		]]
}
DefineSql( StmtDef, CFriend )
--@brief 通过玩家名称搜索玩家
--@param playerName：要查找的玩家name
function FriendDbBox.SearchPlayerAccuratelyByName(playerName)
	local ret = CFriend.SearchPlayerByName:ExecSql("nn", playerName)
	return ret
end
------------------------------------------------------------------------------------------------------
local StmtDef = {
		"SearchPlayerById",
		[[
		select
			a.cs_uId,
			a.c_sName,
			b.cb_uLevel,
			ifnull(c.co_uOnServerId,0)
		from
			tbl_char a,
			tbl_char_basic b
		left join
			tbl_char_online c on b.cs_uId = c.cs_uId
		where
			a.cs_uId = b.cs_uId and a.cs_uId = ?
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 通过玩家Id搜索玩家
--@param playerId：要查找的玩家ID
function FriendDbBox.SearchPlayerAccuratelyById(playerId)
	local ret = CFriend.SearchPlayerById:ExecSql("s[32]n", playerId)
	return ret
end
---------------------------------------------------------------------------------------------------------
--查找自己再玩家好友组的组ID
local StmtDef = {
		"Select_ClassId",
		[[
			select  fc_uId from tbl_player_friends where cs_uId = ? and pf_uFriendId = ?	
		]]
}
DefineSql( StmtDef, CFriend )

local StmtDef = {
		"Friend_delete",
		[[
			delete from tbl_player_friends where cs_uId = ? and pf_uFriendId = ?	
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 删除好友
--@param invitorId：玩家自己的ID
--@param inviteeId：玩家要删除的好友的ID
function FriendDbBox.DeleteFriend(data)
	local invitorId = data["self_id"]
	local inviteeId = data["player_id"]
	local nMyClassIdInFriend, nFriendClassIdInMy = 0, 0
	local g_LogMgr = RequireDbBox("LogMgrDB")
	--在自己的好友列表里搜索要删除的好友
	local info = FriendDbBox.GetFriendInfo(invitorId, inviteeId)
	if (not info) then
		return false
	end
	
	--在对方的好友列表里搜索自己所在的好友组ID
	local friendClassIdList = CFriend.Select_ClassId:ExecSql('',inviteeId,invitorId)
	--在自己的好友列表里搜索对方所在的好友组ID
	local myClassIdList = CFriend.Select_ClassId:ExecSql('',invitorId,inviteeId)
	
	if ( friendClassIdList and friendClassIdList:GetRowNum() > 0 ) then
		nMyClassIdInFriend = friendClassIdList:GetData(0,0)
	end
	if ( myClassIdList and myClassIdList:GetRowNum() > 0 ) then
		nFriendClassIdInMy = myClassIdList:GetData(0,0)
	end
	
	--从自己的好友列表里面将该好友删除
	CFriend.Friend_delete:ExecSql('', invitorId, inviteeId)
	g_LogMgr.DeleteFriend(invitorId,inviteeId)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		CancelTran()
		return false
	end
	
	--从对方的好友列表里面将自己删除
	if FriendDbBox.GetFriendInfo(inviteeId,invitorId) then
		CFriend.Friend_delete:ExecSql('', inviteeId,invitorId)
		g_LogMgr.DeleteFriend(inviteeId,invitorId)
		if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
			CancelTran()
			return false
		end
	end
	
	local ex = RequireDbBox("Exchanger")
	local invitee_name = ex.getPlayerNameById(inviteeId)
	
	return true, nMyClassIdInFriend, nFriendClassIdInMy
end
------------------------------------------------------------------------------------------------------------
local StmtDef = {
		"Get_Group_Exist",
		[[	select 1 from tbl_friends_class where cs_uId = ? and fc_sName = ?]]
}
DefineSql( StmtDef, CFriend)

local StmtDef = {
		"Add_Friend_Group",
		[[	insert into tbl_friends_class (fc_sName,cs_uId) values (?, ?) ]]
}
DefineSql( StmtDef, CFriend )
--@brief 添加好友分组
--@param uCharId:玩家ID
--@param groupName:群名称
--@return 返回成见成功与否

function FriendDbBox.AddFriendClass(data)
	local uCharId = data["CharID"]
	local groupName = data["GroupName"]
	
	local textFltMgr = CTextFilterMgr:new()
	groupName = textFltMgr:RemoveTab1(groupName)
	if "" == groupName then
		return false,130005 --"对不起,新建组名不能为空!"
	end
	
	if string.len(groupName)>32 then
		return false,130006 --"对不起,您输入的组名过长!"
	end
	
	local textFltMgr = CTextFilterMgr:new()
	if(not textFltMgr:IsValidName(groupName)) then
		return false,130007 --"对不起,您输入的好友组名不合法!"
	end
	
	local query_result = CFriend.Get_Friend_All_Class:ExecSql('ns[32]', uCharId)
	--这里有17当做临界值是因为好友组里面已经有好友和黑名单两个默认的好友组了。
	if query_result and query_result:GetRowNum() >= 17 then 
		query_result:Release()
		return false,130051 --"好友分组数上限是15个，超过上限了。"
	end
	
	--判断重名
	local query_result = CFriend.Get_Group_Exist:ExecSql('n', uCharId, groupName)
	if query_result == nil then
		return false
	end
	
	if (0 ~= query_result:GetRowNum()) then
		query_result:Release()
		return false,130008  -- "组已经存在,组名不能重复!"
	end
	
	query_result:Release()
	CFriend.Add_Friend_Group:ExecSql('', groupName,uCharId)
	local newClassId = g_DbChannelMgr:LastInsertId()
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true,130009,newClassId  -- "组创建成功!",g_DbChannelMgr:LastInsertId()
	else
		return false,130010 -- "组创建失败!"
	end
end
---------------------------------------------------------------------------------------------------------
--@brief  获取玩家的所有好友组名
function FriendDbBox.GetGroupCount(fc_uId,playerId)
	local grouplist = {}
	local query_result = CFriend.SelectFriendClassName:ExecSql('s[32]',fc_uId,playerId)
	if query_result == nil then
		return grouplist
	end
	
	if (0 == query_result:GetRowNum()) then
		query_result:Release()
		return grouplist
	end
	
	local num = query_result:GetRowNum()
	for i = 1, num do
		table.insert(grouplist, query_result:GetData(i-1,0))
	end
	query_result:Release()
	return grouplist
end
--------------------------------------------------------------------------------------------------------------
local StmtDef = {
		"Insert_Friend_Group",
		[[insert into tbl_friends_class (fc_uId,fc_sName,cs_uId) values (?,?, ?) ]]
}
DefineSql( StmtDef, CFriend )

function FriendDbBox.InitAddFriendGoodClassAndBlackClass(uCharId)

	if 0 == #(FriendDbBox.GetGroupCount(1,uCharId)) then
		CFriend.Insert_Friend_Group:ExecSql('', 1,"我的好友",uCharId)
	end
	if 0 == #(FriendDbBox.GetGroupCount(2,uCharId)) then
		CFriend.Insert_Friend_Group:ExecSql('', 2,"黑名单",uCharId)
	end

	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
--------------------------------------------------------------------------------------------------------------
--更新玩家好友的所在好友组
local StmtDef = {
		"Update_Friend_Class",
		[[
			update tbl_player_friends set fc_uId = ?  where cs_uId = ? and pf_uFriendId = ?
		]]
}
DefineSql( StmtDef, CFriend )

--删除玩家的好友组
local StmtDef = {
		"Delete_Friend_Class",
		[[
			delete from tbl_friends_class where fc_uId = ? and cs_uId = ?
		]]
}
DefineSql( StmtDef, CFriend )

--获取分类所有好友
local StmtDef = {
		"Selece_Group_Member",
		[[	select pf_uFriendId from tbl_player_friends where cs_uId = ? and fc_uId = ?	]]
}
DefineSql( StmtDef, CFriend )

local StmtDef={
		"Update_FriendRequestByClassId",
		[[
			update tbl_request_addfriend set fc_uId = ? where fc_uId = ?
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 删除好友组
--@param uCharId:玩家ID
--@param class_id:要删除的组ID
function FriendDbBox.DeleteFriendClass(data)
	local uCharId		=	data["CharID"]
	local class_id		=	data["class_id"]
	if nil == FriendDbBox.GetGroupCount(class_id,uCharId) then
		return false,130011 --"对不起,要删除的组已不存在!"
	end
	
	CFriend.Update_FriendRequestByClassId:ExecSql('', 1,class_id)

	local member_result = CFriend.Selece_Group_Member:ExecSql('n', uCharId, class_id)
	if ( member_result:GetRowNum() > 0 ) then
		for i = 1,member_result:GetRowNum() do
			CFriend.Update_Friend_Class:ExecSql("",1,uCharId,member_result:GetData(i-1,0))
			if g_DbChannelMgr:LastAffectedRowNum() < 1 then
				CancelTran()
				return false,130012 --"移动好友到好友组失败!"
			end
		end
	end

	member_result:Release()
	CFriend.Delete_Friend_Class:ExecSql('', class_id,uCharId)
	if g_DbChannelMgr:LastAffectedRowNum() < 1 then
		CancelTran()
		return false,130017 --"删除好友组失败!"
	end
	return true
end
-----------------------------------------------------------------------------------------------------------
--将好友移到分类
local StmtDef = {
		"Move_To_Type",
		[[	update tbl_player_friends set fc_uId = ? where cs_uId = ? and pf_uFriendId = ?	]]
}
DefineSql( StmtDef, CFriend )

--@brief 将好友移入某个好友组
--@param uCharId:玩家ID
--@param playerId：要移动的玩家ID
--@param class_id：要移动的好友组Id
function FriendDbBox.MoveFriendToClass(data)
	local uCharId = data["CharID"]
	local playerId = data["PlayerID"]
	local class_id = data["newclass_id"]
	if FriendDbBox.GetFriendInfo(uCharId, playerId) == 1 then
		local invitor_classname = CFriend.SelectFriendClassName:ExecStat(class_id,uCharId)
		if (not invitor_classname) or invitor_classname:GetRowNum() == 0 then
			return false
		end
		CFriend.Move_To_Type:ExecSql("",class_id,uCharId,playerId)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
	return false
end
-----------------------------------------------------------------------------------------------------
--上线显示完成后把离线时的添加好友信息从数据库删除；
local StmtDef={
		"Delete_OfflineFriend_Mgr",
		[[
			delete from tbl_offline_msg where om_uReciever = ? 
		]]
}
DefineSql( StmtDef, CFriend )

--上线后，显示离线时别人发送的添加好友信息
local StmtDef={
		"SelectOfflineMsg",
		[[ 
			select 
				om_uSender,c_sName,om_dtCreateTime,om_sContent 
			from 
				tbl_offline_msg,tbl_char 
			where om_uSender = cs_uId and om_uReciever = ? 
		]] 
}
DefineSql(StmtDef, CFriend)
	
--@brief 上线后显示离线消息
function FriendDbBox.SelectOfflineMsg(playerId)
	local query_list = CFriend.SelectOfflineMsg:ExecStat(playerId)
	if query_list:GetRowNum() == 0 then
		return nil
	end
	CFriend.Delete_OfflineFriend_Mgr:ExecStat(playerId)
	return query_list
end
-----------------------------------------------------------------------------------------------------
local StmtDef={
		"_SelectOfflineGMMsg",
		[[ 
			select 
				gm_uSenderId,gm_dtCreateTime,gm_sContent 
			from 
				tbl_gm_msg 
			where gm_uRecieverId = ? and gm_uReadFlag = 1
		]] 
}
DefineSql(StmtDef, CFriend)

local StmtDef={
		"_ReadOfflineGMMsg",
		[[ 
			update tbl_gm_msg set gm_uReadFlag = 0 where gm_uRecieverId = ? 
		]] 
}
DefineSql(StmtDef, CFriend)
	
--@brief 上线后显示GM离线消息
function FriendDbBox.SelectOfflineGMMsg(playerId)
	local query_list = CFriend._SelectOfflineGMMsg:ExecStat(playerId)
	if query_list:GetRowNum() == 0 then
		return nil
	end
	CFriend._ReadOfflineGMMsg:ExecStat(playerId)
	return query_list
end
-------------------------------------------------------------------------------------------------------
--更新好友标识
local StmtDef = {
		"_UpdateFriendFlag",
		[[ 
			update tbl_player_friends set fc_uId = ? where cs_uId = ? and pf_uFriendId = ? 
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 添加黑名单
function FriendDbBox.AddBlackList(data)
	local uCharId = data["CharID"]
	local playerId = data["PlayerID"]
	local classId = 0
	local g_LogMgr = RequireDbBox("LogMgrDB")
	--查看自己是不是对方的好友
	if FriendDbBox.GetFriendInfo(playerId,uCharId) == 1 then
		--在对方的好友列表里搜索自己所在的好友组ID
		local classIdList = CFriend.Select_ClassId:ExecSql("n", playerId, uCharId)
		if nil ~= classIdList or classIdList:GetRowNum() > 0 then
			classId = classIdList:GetData(0,0)
			--从对方的好友列表里面将自己删除
			CFriend.Friend_delete:ExecSql('', playerId, uCharId)
			g_LogMgr.DeleteFriend(playerId, uCharId)
			if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
				CancelTran()
				return false
			end
		end
	end
	
	--更新玩家在自己的好友列表里面的标志
	CFriend._UpdateFriendFlag:ExecSql("",2,uCharId,playerId)
	if g_DbChannelMgr:LastAffectedRowNum() <= 0 then
		if 0 == #(FriendDbBox.GetGroupCount(2,uCharId)) then
			CFriend.Insert_Friend_Group:ExecSql('', 2,"黑名单",uCharId)
		end
		CFriend.AddNewFriend:ExecSql("",uCharId,playerId,2)
	else
		g_LogMgr.DeleteFriend(uCharId, playerId)
	end
	
	return true,classId
end
-------------------------------------------------------------------------------------------------------
function FriendDbBox.AddBlackListByName(data)
	local uCharId = data["CharID"]
	local player_name = data["player_name"]
	local ex = RequireDbBox("Exchanger")
	local playerId = ex.getPlayerIdByName(player_name)
	if playerId == 0 then
		return false
	end
	local query_data = {
			["CharID"] = uCharId,
			["PlayerID"] = playerId
		}
	local suc,classId = FriendDbBox.AddBlackList(query_data)
	return suc,classId,playerId
end
-------------------------------------------------------------------------------------------------------
----@brief 删除黑名单
function FriendDbBox.DeleteBlackList(data)
	return FriendDbBox.DeleteFriend(data)
end
----------------------------------------------------------------------------------------------------------
----获取好友所有group
local StmtDef = {
		"Get_Friend_All_Class",
		[[	
			select fc_uId,fc_sName from tbl_friends_class where cs_uId = ?
		]]
}
DefineSql( StmtDef, CFriend)

--@brief  获取玩家的所有好友组名
function FriendDbBox.GetAllFriendsClass(uCharId)
	local grouplist = {}
	local query_result = CFriend.Get_Friend_All_Class:ExecSql('ns[32]', uCharId)
	if query_result == nil then
		return grouplist
	end
	
	if (0 == query_result:GetRowNum()) then
		query_result:Release()
		return grouplist
	end
	
	local num = query_result:GetRowNum()
	
	for i = 1, num do
		local groupset = {}
		groupset[1] = query_result:GetData(i-1,0)
		groupset[2] = query_result:GetData(i-1,1)
		grouplist[i] = groupset
	end
	
	query_result:Release()
	return grouplist
end
----------------------------------------------------------------------------------------------------------
local StmtDef = {
		"SelectFriendInfo",
		[[
			select
				d.fc_uId,
				c.pf_uFriendId,
				a.c_sName,
				b.cb_uLevel,
				e.cs_uCamp,
				e.cs_uClass
			from
				tbl_char a,
				tbl_char_basic b,
				tbl_player_friends c,
				tbl_friends_class d,
				tbl_char_static e
			where
				a.cs_uId = b.cs_uId and c.pf_uFriendId = b.cs_uId and c.cs_uId = d.cs_uId and c.fc_uId = d.fc_uId
				and e.cs_uId = b.cs_uId and c.cs_uId = ?
			union
			select
				fc.fc_uId,
				pf.pf_uFriendId,
				cd.cd_sName,
				cb.cb_uLevel,
				cs.cs_uCamp,
				cs.cs_uClass
			from
				tbl_char_deleted cd,
				tbl_char_basic cb,
				tbl_player_friends pf,
				tbl_friends_class fc,
				tbl_char_static cs
			where
				cd.cs_uId = cb.cs_uId and pf.pf_uFriendId = cb.cs_uId and pf.cs_uId = fc.cs_uId and pf.fc_uId = fc.fc_uId
				and cs.cs_uId = cb.cs_uId and pf.cs_uId = ?
		]]
		
}
DefineSql( StmtDef, CFriend )

--@brief 查找好友信息
function FriendDbBox.SelectFriendInfo(invitorId)
	local m_memberlist = {}
	local query_result = CFriend.SelectFriendInfo:ExecStat(invitorId,invitorId)
	
	if query_result == nil then
		return m_memberlist
	end
	
	if (0 == query_result:GetRowNum()) then --玩家的好友列表为空
		query_result:Release()
		return m_memberlist
	end
	local LoginServerSql	= RequireDbBox("LoginServerDB")
	local SceneMgrDB		= RequireDbBox("SceneMgrDB")
	local GasTeamDB			= RequireDbBox("GasTeamDB")
	local tong_box = RequireDbBox("GasTongBasicDB")
	for i=1, query_result:GetRowNum() do
		--好友组ID, 玩家ID, 好友name, 等级, 阵营, 职业, 小队人数, 所在场景, 是否在线
		local friendsInfo = {}
		friendsInfo[1]	= query_result:GetData(i-1,0)
		friendsInfo[2]	= query_result:GetData(i-1,1)
		friendsInfo[3]	= query_result:GetData(i-1,2)
		friendsInfo[4]	= query_result:GetData(i-1,3)
		friendsInfo[5]	= query_result:GetData(i-1,4)
		friendsInfo[6]	= query_result:GetData(i-1,5)
		
		local nTeamId	= GasTeamDB.GetTeamID(friendsInfo[2])
		local nSceneId	= SceneMgrDB.GetPlayerCurScene(friendsInfo[2])
		local isOnline	= LoginServerSql.IsPlayerOnLine(friendsInfo[2])
		friendsInfo[7]	= GasTeamDB.CountTeamMems(nTeamId)
		friendsInfo[8]	= SceneMgrDB._GetSceneNameById(nSceneId)
		friendsInfo[9]	= isOnline and 1 or 2
		friendsInfo[10] = tong_box.GetTongID(friendsInfo[2])	
		table.insert(m_memberlist,friendsInfo)
	end
	query_result:Release()
	return m_memberlist
end
----------------------------------------------------------------------------------------------------------
local StmtDef = {
		"UpdateFriendInfo",
		[[
			select
				c.fc_uId,
				a.cb_uLevel
			from
				tbl_char_basic a,
				tbl_player_friends b,
				tbl_friends_class c
			where
				b.cs_uId = c.cs_uId and b.fc_uId = c.fc_uId and a.cs_uId = b.pf_uFriendId and
				b.cs_uId = ? and b.pf_uFriendId = ?
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 更新指定好友信息
function FriendDbBox.UpdateFriendInfo(data)
	local nCharID		= data["CharID"]
	local nFriendID		= data["FriendID"]
	local query_result	= CFriend.UpdateFriendInfo:ExecSql("", nCharID, nFriendID)
	
	if query_result == nil then
		return nil
	end
	
	if (0 == query_result:GetRowNum()) then
		query_result:Release()
		return nil
	end
	
	local SceneMgrDB	= RequireDbBox("SceneMgrDB")
	local GasTeamDB		= RequireDbBox("GasTeamDB")
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tblFriendsInfo = {}
	--好友组ID, 等级, 小队人数, 所在场景
	tblFriendsInfo.classId	= query_result:GetData(0,0)
	tblFriendsInfo.level	= query_result:GetData(0,1)
	tblFriendsInfo.tongId = tong_box.GetTongID(nFriendID)	
		
	local nTeamId				= GasTeamDB.GetTeamID(nFriendID)
	local nSceneId				= SceneMgrDB.GetPlayerCurScene(nFriendID)
	tblFriendsInfo.teamSize		= GasTeamDB.CountTeamMems(nTeamId)
	tblFriendsInfo.sceneName	= SceneMgrDB._GetSceneNameById(nSceneId)

	query_result:Release()
	return tblFriendsInfo
end
----------------------------------------------------------------------------------------------------------
local StmtDef = {
		"UpdateOnlineFriendInfo",
		[[
			select
				c.fc_uId,
				b.pf_uFriendId,
				a.cb_uLevel
			from
				tbl_char_basic a,
				tbl_player_friends b,
				tbl_friends_class c,
				tbl_char_online d
			where
				c.fc_uId <> 2				and
				b.pf_uFriendId = a.cs_uId	and
				b.cs_uId = c.cs_uId			and
				b.fc_uId = c.fc_uId			and
				d.cs_uId = b.pf_uFriendId	and
				b.cs_uId = ?
			order by
				c.fc_uId
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 更新在线好友信息
function FriendDbBox.UpdateOnlineFriendInfo(data)
	local nCharID		= data["CharID"]
	local tblUpdateList	= {}
	local query_result	= CFriend.UpdateOnlineFriendInfo:ExecSql("", nCharID)
	
	if query_result == nil then
		return tblUpdateList
	end
	
	if (0 == query_result:GetRowNum()) then --玩家的好友列表为空
		query_result:Release()
		return tblUpdateList
	end
	
	local SceneMgrDB	= RequireDbBox("SceneMgrDB")
	local GasTeamDB		= RequireDbBox("GasTeamDB")
	local tong_box = RequireDbBox("GasTongBasicDB")
	for i=1, query_result:GetRowNum() do
		--好友组ID, 玩家ID, 等级, 小队人数, 所在场景
		local friendsInfo = {}
		friendsInfo[1]	= query_result:GetData(i-1,0)
		friendsInfo[2]	= query_result:GetData(i-1,1)
		friendsInfo[3]	= query_result:GetData(i-1,2)
		
		local nTeamId	= GasTeamDB.GetTeamID(friendsInfo[2])
		local nSceneId	= SceneMgrDB.GetPlayerCurScene(friendsInfo[2])
		friendsInfo[4]	= GasTeamDB.CountTeamMems(nTeamId)
		friendsInfo[5]	= SceneMgrDB._GetSceneNameById(nSceneId)
		friendsInfo[6]	= tong_box.GetTongID(friendsInfo[2])	
		table.insert(tblUpdateList, friendsInfo)
	end
	query_result:Release()
	return tblUpdateList
end

----------------------------------------------------------------------------------------------------------
local StmtDef = {
		"SelectOffLineFriendInfo",
		[[
			select 
				fc_uId,cs_uId
			from
				tbl_player_friends
			where
				pf_uFriendId = ?
		]]
}
DefineSql( StmtDef, CFriend )

--@brief 查找下线玩家的好友信息
function FriendDbBox.SelectOffLineFriendsInfo(playerId)
	local query_result = CFriend.SelectOffLineFriendInfo:ExecSql("nn",playerId)
	return query_result
end
-----------------------------------------------------------------------------------------------------------
function FriendDbBox.InitFriendGroupInfo(data)
	local uCharId		=	data["CharID"]
	local result = {}
	local FriendGroupDB = RequireDbBox("FriendGroupDB")
	--查找好友群信息
	result.friendGroupList = FriendGroupDB.GetAllFriendsGroup(uCharId)
	--查找玩家好友信息
	result.allFriendsInfo = FriendGroupDB.GetAllMembers(uCharId)
	
	return result
end
-----------------------------------------------------------------------------------------------------------

--保存玩家不在线的私聊信息
--发送消息玩家ID，接受消息玩家Id，小消息的时间，消息的内容
local StmtDef = {
		"SavePrivateChatMsg",
		[[
			insert into tbl_offline_msg(om_uSender,om_uReciever,om_dtCreateTime,om_sContent) values(?,?,now(),?)
		]]
}
DefineSql( StmtDef, CFriend)


--查询角色在线状态
local StmtDef =
{
	"FindPlayerOnLineState",
	"select co_uOnServerId from  tbl_char_online where cs_uId = ?"
}
DefineSql( StmtDef, CFriend)

local StmtDef = {
		"SaveGMChatMsg",
		[[
			insert into tbl_gm_msg(gm_uSenderId,gm_uRecieverId,gm_dtCreateTime,gm_sContent,gm_uReadFlag) values(?,?,now(),?,?)
		]]
}
DefineSql( StmtDef, CFriend)

--@brief 私聊
function FriendDbBox.PrivateChatRequest(data)
	local playerId = data["playerId"]
	local object_id = data["object_id"]
	local message = data["text"]
	local textFltMgr = CTextFilterMgr:new()
	if object_id == 0 then
		CFriend.SaveGMChatMsg:ExecSql("n", playerId,object_id,message,0)
		return
	elseif playerId == 0 then
		local onlineState = CFriend.FindPlayerOnLineState:ExecSql("n", object_id)
		local beOnline = false
		if onlineState:GetRowNum() > 0 then
			beOnline = true
		else
			beOnline = false
		end
		if beOnline then
			CFriend.SaveGMChatMsg:ExecSql("n", playerId,object_id,message,0)
		else
			CFriend.SaveGMChatMsg:ExecSql("n", playerId,object_id,message,1)
		end
		return beOnline, beOnline
	end
	
	local isInBlackList = FriendDbBox.IsBlackName(object_id,playerId)
	if isInBlackList then
		return false,130014 --"你在对方的黑名单中,不能私聊!"
	end
	
	local isInBlackList = FriendDbBox.IsBlackName(playerId, object_id)
	
	if isInBlackList then
		return false,130018 --"对方在你的黑名单中，不能私聊!"
	end
	
	--去掉前后空格
	message = textFltMgr:RemoveTab1(message)
	--用“**”替换所有不合法字符
	message = textFltMgr:ReplaceInvalidChar(message)
	
	local onlineState = CFriend.FindPlayerOnLineState:ExecSql("n", object_id)
	local beOnline = false
	if onlineState:GetRowNum() > 0 then
		beOnline = true
		onlineState:Release()
	else
		onlineState:Release()
		--私聊对象不在线,存离线消息
		CFriend.SavePrivateChatMsg:ExecSql("",playerId,object_id,message)
		return false,130004 --"该玩家不在线"
	end

	local ex = RequireDbBox("Exchanger")
	local char_name = ex.getPlayerNameById(playerId)
	return true, beOnline,char_name
end
-------------------------------------------------------------------------------------------------------------     		
local StmtDef = {
		"Update_Class_Name_Stmt",
		[[
			update tbl_friends_class set fc_sName = ? where fc_uId = ?
		]]
}
DefineSql (StmtDef,CFriend)

--@brief 修改组名
function FriendDbBox.UpdateFriendClassName(data)
	local playerId = data["playerId"]
	local friend_classId = data["friend_classId"]
	local new_className = data["new_className"]
	
	if friend_classId == 1 or friend_classId == 2 then
		return false,130019 --"对不起,系统默认组不能重命名!"
	end
	
	--判断重名
	local query_result = CFriend.Get_Group_Exist:ExecSql('n', playerId, new_className)
	if query_result == nil then
		return false
	end
	
	if (0 ~= query_result:GetRowNum()) then
		query_result:Release()
		return false,130020  --new_className .. "组已经存在,组名不能重复!"
	end
	
	if string.len(new_className)>32 then
		return false,130006 --"对不起,您输入的组名过长!"
	end
	
	CFriend.Update_Class_Name_Stmt:ExecSql('',new_className,friend_classId)
	local b_flag = g_DbChannelMgr:LastAffectedRowNum() > 0
	if not b_flag then
		return false,130021 --"重命名组名失败!"
	else
		return true
	end
end

-----------------------------------------------------------------------------------------------------------
--@brief 通过玩家名字得到玩家ID
function FriendDbBox.GetMemberIdByName(data)
	local memberName = data["memberName"]
	local ex = RequireDbBox("Exchanger")
	local memberId = ex.getPlayerIdByName(memberName)
	if(0 == memberId) then
		return false
	else
		return true, memberId
	end
end
----------------------------------------------------------------------------------------------------
local StmtDef = {
		"Save_FellStatement",
		[[ 
		call SaveShowSentence(?,?)
		]]
}
DefineSql (StmtDef, CFriend)
--@brief 保存心情寄语
--@param fellStatement:心情寄语
function FriendDbBox.SaveShowSen(data)
	local playerId = data["playerId"]
	local showSen = data["showSen"]

	if not playerId or playerId == 0 then
		return false
	end
	if string.len(showSen) > 32 then
		return false,130053 --输入的心情寄语过长。
	end
	
	CFriend.Save_FellStatement:ExecStat(playerId, showSen)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
----------------------------------------------------------------------------------------------------
local StmtDef = {
		"Save_PlayerInfo",
		[[ 
		replace into 
			tbl_friends_info(cs_uId,fi_uFellState,fi_sFellStatement,fi_sHobby,fi_uBodyShape,fi_uPersonality,fi_uMakeFriendState,fi_uStyle,fi_sDetail)
		values(?,?,?,?,?,?,?,?,?)
		]]
}
DefineSql (StmtDef, CFriend)
--@brief 保存群里面的个人信息
--@param fellState:心情
--@param fellStatement:心情寄语
--@param hobby:兴趣爱好
--@param bodyShape:体型
--@param personality:个性
--@param makeFriendState:交友状态
--@param style:类型
--@param first_newest:最新动态1
--@param second_newest:最新动态2
--@param third_newest:最新动态3
function FriendDbBox.SavaPlayerInfo(data)
	local playerId = data["playerId"]
	local fellState = data["fellState"]			--fell这个单词怎么用在这里
	local fellStatement = data["fellStatement"]
	local hobby = data["hobby"]
	local bodyShape = data["bodyShape"]
	local personality = data["personality"]
	local makeFriendState = data["makeFriendState"]
	local style = data["style"]
	local detail = data["detail"]
	
	if playerId == nil or playerId == 0 then
		return false
	end
	if string.len(fellStatement) > 32 then
		return false,130053 --输入的心情寄语过长。
	end
	if string.len(hobby) > 255 then
		return false,130054 --输入的兴趣爱好过长。
	end
	if string.len(detail) > 255 then
		return false,130055 --个人说明
	end
	
	CFriend.Save_PlayerInfo:ExecStat(playerId,fellState,fellStatement,hobby,bodyShape,personality,makeFriendState,style,detail)
														
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
----------------------------------------------------------------------------------------------------
local StmtDef = {
		"Select_PersonalInfo",
		[[ 
		select
			ifnull(fi_uFellState,0),ifnull(fi_sFellStatement,""),ifnull(fi_sHobby,""),ifnull(fi_uBodyShape,0),
			ifnull(fi_uPersonality,0),ifnull(fi_uMakeFriendState,0),ifnull(fi_uStyle,0),ifnull(fi_sDetail,"") 
		from 
			tbl_friends_info
		where
			cs_uId = ?
		]]
}
DefineSql (StmtDef, CFriend)

--@brief 查看个人基本信息
function FriendDbBox.SelectPersonalInfo(data)
	local playerId = data["uPlayerID"]
	if playerId == nil or playerId == 0 then
		return playerTbl
	end
	local playerInfo = CFriend.Select_PersonalInfo:ExecStat(playerId)
	return playerInfo
end
----------------------------------------------------------------------------------------------------
--查看好友的基本信息
local StmtDef = {
		"Select_PlayerInfo",
		[[ 
		select
			a.cs_uClass,b.cb_uLevel,s.sc_sSceneName
		from
			tbl_char_static a,
			tbl_char_basic b,
			tbl_char_position c,
			tbl_scene s
		where
			a.cs_uId = b.cs_uId and b.cs_uId = c.cs_uId and c.sc_uId = s.sc_uId and a.cs_uId = ?
		]]
}
DefineSql (StmtDef, CFriend)

--查看好友的佣兵团信息
local StmtDef = {
		"Select_TongInfo",
		[[ 
		select
			b.t_sName
		from
			tbl_member_tong a,
			tbl_tong b
		where
			a.t_uId = b.t_uId and a.cs_uId = ?
		]]
}
DefineSql (StmtDef, CFriend)

--@brief 查看玩家基本信息
function FriendDbBox.GetAssociationMemberInfo(data)
	local playerId = data["playerId"]
	local playerTbl = {}
	local tongName = ""
	local class = 1
	local level = 1
	local position = 1
	if playerId == nil or playerId == 0 then
		return playerTbl
	end
	
	local playerInfo = CFriend.Select_PlayerInfo:ExecStat(playerId)
	local playerTongInfo = CFriend.Select_TongInfo:ExecStat(playerId)
	
	if nil ~= playerInfo or playerInfo:GetRowNum() > 0 then
		class = playerInfo:GetData(0,0)
		level = playerInfo:GetData(0,1)
		position = playerInfo:GetData(0,2)
	end
	
	if playerTongInfo ~= nil and playerTongInfo:GetRowNum() > 0 then
		tongName = playerTongInfo:GetData(0,0)
	end
	
	playerTbl[1] = 0
	playerTbl[2] = ""
	playerTbl[3] = class
	playerTbl[4] = level
	playerTbl[5] = tongName
	playerTbl[6] = ""
	playerTbl[7] = position
	playerTbl[8] = ""
	playerTbl[9] = 0
	playerTbl[10] = 0
	playerTbl[11] = 0
	playerTbl[12] = 0
	playerTbl[13] = ""
	
	
	local playerOtherInfo = CFriend.Select_PersonalInfo:ExecStat(playerId)
	
	if nil ~= playerOtherInfo or playerOtherInfo:GetRowNum() > 0 then
		for i = 1 ,playerOtherInfo:GetRowNum() do
			playerTbl[1] = playerOtherInfo:GetData(i-1,0) or 0 
			playerTbl[2] = playerOtherInfo:GetData(i-1,1) or ""
			playerTbl[8] = playerOtherInfo:GetData(i-1,2) or ""
			playerTbl[9] = playerOtherInfo:GetData(i-1,3) or 0
			playerTbl[10] = playerOtherInfo:GetData(i-1,4) or 0
			playerTbl[11] = playerOtherInfo:GetData(i-1,5) or 0
			playerTbl[12] = playerOtherInfo:GetData(i-1,6) or 0
			playerTbl[13] = playerOtherInfo:GetData(i-1,7) or ""
		end

		return playerTbl
	end
	return playerTbl
end
-----------------------------------------------------------------------------------------------------------
SetDbLocalFuncType(FriendDbBox.AddFriendClass)
SetDbLocalFuncType(FriendDbBox.SearchPlayerAccurately)
SetDbLocalFuncType(FriendDbBox.SearchPlayerByRequest)
SetDbLocalFuncType(FriendDbBox.AddFriendToClass)
SetDbLocalFuncType(FriendDbBox.RequestAddFriend)
SetDbLocalFuncType(FriendDbBox.DeleteFriend)
SetDbLocalFuncType(FriendDbBox.MoveFriendToClass)
SetDbLocalFuncType(FriendDbBox.PrivateChatRequest)
SetDbLocalFuncType(FriendDbBox.AddBlackList)
SetDbLocalFuncType(FriendDbBox.AddBlackListByName)
SetDbLocalFuncType(FriendDbBox.DeleteBlackList)
SetDbLocalFuncType(FriendDbBox.SelectOfflineMsg)
SetDbLocalFuncType(FriendDbBox.UpdateFriendClassName)
SetDbLocalFuncType(FriendDbBox.DeleteFriendClass)
SetDbLocalFuncType(FriendDbBox.GetMemberIdByName)
SetDbLocalFuncType(FriendDbBox.AddFriendToClassByName)
return FriendDbBox
