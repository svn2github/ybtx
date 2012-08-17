CGasFriendBasic = class()

local FriendsDB = "FriendsDB"
-----------------------------------------------------------------------------------------------------
--@brief 向客户端发送好友分组信息
function CGasFriendBasic.SendFriendClassList(Conn, friendClassList)
	Gas2Gac:ReturnGetAllFriendClassBegin(Conn)
	if 0 ~= #friendClassList then
		for i = 3, table.getn(friendClassList) do
			Gas2Gac:ReturnGetAllFriendClass(Conn, friendClassList[i][1],friendClassList[i][2])
		end
	end
	Gas2Gac:ReturnGetAllFriendClassEnd(Conn)
end
-----------------------------------------------------------------------------------------------------
--@brief 向客户端发送好友基本信息
function CGasFriendBasic.SendFriendInfo(Conn, friendInfoList)
	if 0 ~= #friendInfoList then
		for i = 1, table.getn(friendInfoList) do
			--好友组ID, 玩家ID, 好友name, 等级, 阵营, 职业, 小队人数, 所在场景, 是否在线
			Gas2Gac:ReturnGetAllFriendInfo(Conn, unpack(friendInfoList[i]))
		end
	end
	Gas2Gac:ReturnGetAllFriendInfoEnd(Conn)
end
-----------------------------------------------------------------------------------------------------
--@brief 向客户端发送好友群信息
function CGasFriendBasic.SendFriendGroupList(Conn, friendGroupList)
	Gas2Gac:ReturnGetAllFriendGroupBegin(Conn)
	if friendGroupList:GetRowNum() > 0 then
		local friendGroupTbl = friendGroupList:GetTableSet()
		for i = 1, friendGroupList:GetRowNum() do
			--好友群序号，好友群名称，好友群类型，好友群标签，好友群公告
			Gas2Gac:ReturnGetAllFriendGroup(Conn, friendGroupTbl(i,1),friendGroupTbl(i,2),
			friendGroupTbl(i,3),friendGroupTbl(i,4),friendGroupTbl(i,5),friendGroupTbl(i,6))
		end
	end
	Gas2Gac:ReturnGetAllFriendGroupEnd(Conn)
end
-----------------------------------------------------------------------------------------------------
--@brief 向客户端发送好友基本信息
function CGasFriendBasic.SendFriendInfoToClent(Conn, allFriendsInfo)
	if allFriendsInfo:GetRowNum() > 0  then
		for i = 1, allFriendsInfo:GetRowNum() do
			--玩家ID,玩家name,玩家职位标识(1:群主，2:管理员(最多只有三个)，3:普通成员),
			--玩家所在群ID,玩家是不是在线(1--在线，2--离线)
			local uOnline = 2
			if allFriendsInfo(i,5) ~= 0 then
				uOnline = 1
			end
			Gas2Gac:ReturnGetAllGroupMemInfo(Conn, allFriendsInfo(i,1),allFriendsInfo(i,2),
			allFriendsInfo(i,3),allFriendsInfo(i,4),uOnline)
		end
	end
	Gas2Gac:ReturnGetAllGroupMemInfoEnd(Conn,0)
	Gas2Gac:ReturnInitAssociationInfo(Conn)
end
-----------------------------------------------------------------------------------------------------
--@brief 刷新指定好友信息
--@param nFriendId:好友ID
function CGasFriendBasic.UpdateFriendInfo(Conn, nFriendId)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local function CallBack(result)
		if( result ) then
			if( 2 == result.classId ) then --企图刷新黑名单成员(可能是用外挂) 
				return
			end
			--好友组ID, 等级, 小队人数, 所在场景, 好友ID
			Gas2Gac:ReturnUpdateFriendInfo(Conn,
				result.classId, result.level, result.teamSize, result.sceneName, nFriendId,result.tongId)
		end
	end

	local data = {	["CharID"] = Player.m_uID,
					["FriendID"] = nFriendId}
	
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "UpdateFriendInfo", CallBack, data)
end

-----------------------------------------------------------------------------------------------------
--@brief 刷新在线好友信息
function CGasFriendBasic.UpdateOnlineFriendInfo(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	if(Player.m_nUpdateOnlineFriendTimeMark and os.time() - Player.m_nUpdateOnlineFriendTimeMark < 50 ) then
		return --刷新时间间隔小于50秒(客户端有限制,正常情况不会进入这里)
	else
		Player.m_nUpdateOnlineFriendTimeMark = os.time()
	end
	
	local function CallBack(result)
		if( next(result) ) then
			local preClassId = result[1][1]
			for i, v in ipairs(result) do --好友组ID, 玩家ID, 等级, 小队人数, 所在场景
				if(preClassId ~= v[1]) then
					Gas2Gac:ReturnUpdateOnlineFriendInfoClassEnd(Conn, preClassId)
					preClassId = v[1]
				end
				Gas2Gac:ReturnUpdateOnlineFriendInfo(Conn, unpack(v))
			end
			Gas2Gac:ReturnUpdateOnlineFriendInfoClassEnd(Conn, preClassId)
		end
	end
	
	local data = {["CharID"] = Player.m_uID}
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "UpdateOnlineFriendInfo", CallBack, data)
end
-----------------------------------------------------------------------------------------------------
--@brief 添加好友分组
--@param gf_sName:好友分组名称
function CGasFriendBasic.AddFriendClass(Conn,gf_sName)
	local player = Conn.m_Player
	if not IsCppBound(player) then
		return
	end
	
	local data = {
								["CharID"] = player.m_uID,
								["GroupName"] = gf_sName
								}
	local function callback(ret,errMsg,newClassId)
		if nil ~= errMsg and IsNumber(errMsg) then
			if errMsg == 130008 or errMsg == 130009 or errMsg == 130010 then
				MsgToConn(Conn,errMsg,gf_sName)
			else
				MsgToConn(Conn,errMsg)
			end
		end
		
		if ret then
			Gas2Gac:ReturnAddFriendClass(Conn, ret, newClassId, gf_sName)
		else
			Gas2Gac:ReturnAddFriendClass(Conn, ret, 0, gf_sName)
		end
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "AddFriendClass", callback, data)
end
-----------------------------------------------------------------------------------------------------
--@brief 精确查找好友
--@param f_name:好友name
--@param f_id:好友ID
function CGasFriendBasic.SearchPlayerAccurately(Conn,f_name,f_id)
	local data = {
								["InviteeName"] = f_name,
								["InviteeId"] = f_id
								}

	local function CallBack(result)
		if result:GetRowNum() > 0 then
			--发送的是玩家ID，玩家名称，玩家等级，是否在线
			Gas2Gac:ReturnSearchPlayer(Conn, result(1,1),result(1,2), result(1,3),result(1,4))
		end
		Gas2Gac:ReturnSearchPlayerEnd(Conn)
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "SearchPlayerAccurately", CallBack, data)
end
--------------------------------------------------------------------------------------------------------
--@brief 条件查找好友
--@param class:要查找的玩家的职业
--@param gender:要查找的玩家的性别
--@param low_level:要查找的玩家的最低等级
--@param up_level:要查找的玩家的最高等级

function CGasFriendBasic.SearchPlayerCondition(Conn,class,gender,low_level,up_level,sSceneName)

	local data = {
								["class"] = class,
								["gender"] = gender,
								["low_level"] = low_level,
								["up_level"] = up_level,
								["sSceneName"] = sSceneName
								}

	local function CallBack(result)
		if nil ~= result and #result > 0  then
			--发送的是玩家ID，玩家名称，玩家等级，是否在线
			for i = 1,#result do
				Gas2Gac:ReturnSearchPlayer(Conn, result[i][1],result[i][2], result[i][3],result[i][4])
			end
		end
		Gas2Gac:ReturnSearchPlayerEnd(Conn)
	end

	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "SearchPlayerByRequest", CallBack, data)
end
--------------------------------------------------------------------------------------------------------
--@brief 添加好友到好友组里
--@param f_id:要添加的好友ID
--@param gf_sName:好友将要被划分的好友组名
function CGasFriendBasic.AddFriendToClass(Conn,playerId,fc_id)
	local player = Conn.m_Player
	if not IsCppBound(player) then
		return
	end
	
	local data = {
								["InvitorId"] = player.m_uID,
								["InviteeId"] = playerId,
								["classId"] = fc_id
								}
	local function CallBack(succ,errorMsg)
		if succ then
			Gas2GacById:ReturnInviteToBeFriend(playerId, player.m_Properties:GetCharName(),fc_id,player.m_uID)
		else
			if nil ~= errorMsg and IsNumber(errorMsg) then
				MsgToConn(Conn,errorMsg)
			end
		end
	end
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "AddFriendToClass", CallBack, data)
end
--------------------------------------------------------------------------------------------------------
--@brief 响应添加好友
--@param InvitorName:要加玩家的ID
--@param InvitorClassName：要加玩家的组name
--@param InviteeClassName：被加玩家的组name

function CGasFriendBasic.RespondAddFriend(Conn, InvitorId,InvitorClassId,InviteeClassId)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["InviteeId"] = player.m_uID,
								["InvitorId"] = InvitorId,
								["InvitorClassId"] = InvitorClassId,
								["InviteeClassId"] = InviteeClassId
								}
	local function CallBack(succ, tblInvitor, tblInvitee)
		if not succ and tblInvitor and IsNumber(tblInvitor) then
			MsgToConn(Conn, tblInvitor)
			return
		end
		
		if succ then
			Gas2Gac:ReturnAddFriendToClass(Conn, tblInvitee.nClassId, InvitorId,
					tblInvitor.sName, tblInvitor.nLevel, tblInvitor.nCamp, tblInvitor.nClass,
					tblInvitor.nTeamSize, tblInvitor.sSceneName, tblInvitor.nBeOnline,tblInvitor.nTongId)
					
			MsgToConnById(InvitorId,5011,player.m_Properties:GetCharName())
			
			Gas2GacById:ReturnAddFriendToClass(InvitorId, tblInvitor.nClassId, player.m_uID,
					tblInvitee.sName,tblInvitee.nLevel, tblInvitee.nCamp, tblInvitee.nClass,
					tblInvitee.nTeamSize, tblInvitee.sSceneName, tblInvitee.nBeOnline,tblInvitee.nTongId)
		end
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "RequestAddFriend", CallBack, data)
end
-------------------------------------------------------------------------------------------------------------
--@brief 删除好友
--@param Player_ID：要删除玩家的ID

function CGasFriendBasic.DeleteMyFriend(Conn, Player_ID)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local function CallBack(suc, nMyClassIdInFriend, nFriendClassIdInMy)
		if( 0 ~= nFriendClassIdInMy ) then
			Gas2Gac:ReturnDeleteMyFriend(Conn, suc, Player_ID, nFriendClassIdInMy)
		end
		if suc and 0 ~= nMyClassIdInFriend then
			Gas2GacById:ReturnBeDeleteByFriend(Player_ID, player.m_uID, nMyClassIdInFriend)
		end
	end

	local data = {
							["self_id"] = player.m_uID,
							["player_id"] = Player_ID
							}
							
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "DeleteFriend", CallBack, data)
end
-------------------------------------------------------------------------------------------------------------
--@brief 将好友移至别的好友组
--@param player_id ：要移动的好友ID
--@param class_id ：将要移进的组ID

function CGasFriendBasic.ChangeToClass(Conn, player_id, newclass_id)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["CharID"]= player.m_uID, 
								["PlayerID"]= player_id, 
								["newclass_id"]= newclass_id 
								}

	local function CallBack(suc)
		Gas2Gac:ReturnChangeToClass(Conn,suc)
	end

	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "MoveFriendToClass", CallBack, data)
end
-----------------------------------------------------------------------------------------------------------------
local function PrivateChatRequest(Conn, requesterId, requesterName, succ,errMsg,object_id,content)
	if not succ then
		if Conn ~= 0 and IsNumber(errMsg) then
			MsgToConn(Conn,errMsg)
		end
	else
		if errMsg then --成功这个参数表示是否在线
			Gas2GacById:PrivateChatReceiveMsg(object_id,requesterId, requesterName,content)
		else
			if Conn ~= 0 then
				MsgToConn(Conn,130004)
			end
		end
	end
end

local function PrivateChatSendMsg(Conn,object_id,content)
	local requesterId
	if Conn ~= 0 then 
		local Player = Conn.m_Player
		if not IsCppBound(Player) then
			return
		end
		requesterId = Player.m_uID
	else --GM
		requesterId = 0	
	end
	
	
	local data = {
					["playerId"] = requesterId,
					["object_id"] = object_id,
					["text"] = content
				}
	local function callback(succ,errMsg,char_name)
		local requesterName
		if Conn == 0 then
			requesterName = "GM"
		else
			requesterName = char_name
		end
		PrivateChatRequest(Conn, requesterId, requesterName, succ,errMsg,object_id,content)
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	if Conn ~= 0 then
		CallAccountManualTrans(Conn.m_Account, FriendsDB, "PrivateChatRequest", callback, data)
	else
		CallDbTrans(FriendsDB, "PrivateChatRequest", callback, data)
	end
end

--@brief 私聊
--@param object_name:要私聊的对象name
--@param content:私聊的内容
function CGasFriendBasic.PrivateChatSendMsg(Conn,object_id,content)
	PrivateChatSendMsg(Conn,object_id,content)
end
---------------------------------------------------------------------------------------------------------
--@brief 添加黑名单
--@param player_id:要添加黑名单的玩家ID
function CGasFriendBasic.MoveToBlackList(Conn, player_id)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["CharID"]= player.m_uID, 
								["PlayerID"] = player_id
								}

	local function CallBack(suc,classId)
		Gas2Gac:ReturnMoveToBlackList(Conn, suc)
		if suc == true then
			Gas2GacById:ReturnBeDeleteByFriend(player_id, player.m_uID, classId)
		end
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "AddBlackList", CallBack, data)
end
---------------------------------------------------------------------------------------------------------
--@brief 通过玩家名称添加黑名单
--@param player_name:要添加黑名单的玩家name
function CGasFriendBasic.AddBlackListByName(Conn, player_name)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["CharID"]= player.m_uID,
								["player_name"] = player_name
								}

	local function CallBack(suc,classId,player_id)
		Gas2Gac:ReturnAddToBlackList(Conn, player_id, suc, player_name)
		if suc == true and classId ~= 0 then
			Gas2GacById:ReturnBeDeleteByFriend(player_id, player.m_uID, classId)
		end
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "AddBlackListByName", CallBack, data)
end

---------------------------------------------------------------------------------------------------------
--@brief 删除黑名单
--@param player_id:要删除的玩家ID
function CGasFriendBasic.DeleteBlackListMember(Conn, player_id)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["self_id"] = player.m_uID, 
								["player_id"] = player_id
								}

	local function CallBack(suc)
		Gas2Gac:ReturnDeleteBlackListMem(Conn, suc, player_id)
	end

	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "DeleteBlackList", CallBack, data)
end
---------------------------------------------------------------------------------------------------------
--@brief 修改好友组名称
function CGasFriendBasic.RenameFriendClass(Conn,friend_classId,new_className,old_className)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["playerId"] = player.m_uID,
								["friend_classId"] = friend_classId,
								["new_className"] = new_className,
							}
	local function CallBack(suc,errMsg)
		if not suc and nil ~= errMsg and IsNumber(errMsg) then
			if errMsg == 130020 then
				MsgToConn(Conn,errMsg,new_className)
			else
				MsgToConn(Conn,errMsg)
			end
		end
		
		Gas2Gac:ReturnRenameFriendClass(Conn,suc,friend_classId,new_className,old_className)
	end

	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "UpdateFriendClassName", CallBack, data)
end
---------------------------------------------------------------------------------------------------------
--@brief 删除好友组
--@param class_id:要删除的组ID

function CGasFriendBasic.DeleteFriendClass(Conn, class_id)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["CharID"]= player.m_uID, 
								["class_id"] = class_id
								}
	local function CallBack(suc, errorMsg)
		Gas2Gac:ReturnDeleteFriendClass(Conn, suc, class_id)
		if not suc then
			if nil ~= errorMsg and IsNumber(errorMsg) then
				MsgToConn( Conn, errorMsg)
			end
		end
	end

	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "DeleteFriendClass", CallBack, data)
end

---------------------------------------------------------------------------------------------------------
--@brief 通过玩家名字返回玩家ID
function CGasFriendBasic.GetMemberIdbyNameForPrivateChat(Conn, memberName, str)
	local data = {
								["memberName"] = memberName
							}
	local function CallBack(suc, memberId)
		if(suc and Conn.m_Player) then
			Gas2Gac:ReturnGetMemberIdbyNameForPrivateChat(Conn, memberId, memberName, str)
		end
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "GetMemberIdByName", CallBack, data)
end
---------------------------------------------------------------------------------------------------------
function CGasFriendBasic.AddFriendToClassByName(Conn,playerName,fc_id)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	local data = {
								["InvitorId"] = player.m_uID,
								["playerName"] = playerName,
								["classId"] = fc_id
								}
	local function CallBack(succ,errorMsg)
		if succ then
			Gas2GacById:ReturnInviteToBeFriend(errorMsg, player.m_Properties:GetCharName(),fc_id,player.m_uID)
		else
			if nil ~= errorMsg and IsNumber(errorMsg) then
				MsgToConn(Conn,errorMsg)
			end
		end
	end
	
	--local FriendDB = (g_DBTransDef["FriendsDB"])
	CallAccountManualTrans(Conn.m_Account, FriendsDB, "AddFriendToClassByName", CallBack, data)
end
---------------------------------------------------------------------------------------------------------
--当邀请加为好友的时候遭到拒绝，提示对方
function CGasFriendBasic.RefuseBeAddFriend(Conn,inviterId)
	local player = Conn.m_Player
	if nil == player then
		return
	end
	
	MsgToConnById(inviterId,5012,player.m_Properties:GetCharName())
end
-----------------------------------------------------------------------------------------------------
--@brief 保存心情寄语
--@param fellStatement:心情寄语
function CGasFriendBasic.ChangeShowSentence(Conn, showSen)
	local player = Conn.m_Player	
	if not IsCppBound(player) then
		return
	end	
	local data = {
						["playerId"]	= player.m_uID,
						["showSen"]		= showSen
					}
	local function callback(suc, errorId)
		if not suc then
			if IsNumber(errorId) then
				MsgToConn(Conn,errorId)
			end
		end
		Gas2Gac:ReturnChangeShowSentence(Conn, suc, showSen)
	end

	CallAccountManualTrans(Conn.m_Account, FriendsDB, "SaveShowSen", callback, data)
end
-------------------------------------------------------------------------------------------------------
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

function CGasFriendBasic.SaveAssociationPersonalInfo(Conn,fellState,fellStatement,hobby,bodyShape,personality,makeFriendState,style,detail)
	local player = Conn.m_Player	
	if not IsCppBound(player) then
		return
	end	
	local data = {
								["playerId"] = player.m_uID,
								["fellState"] = fellState,
								["fellStatement"] = fellStatement,
								["hobby"] = hobby,
								["bodyShape"] = bodyShape,
								["personality"] = personality,
								["makeFriendState"] = makeFriendState,
								["style"] = style,
								["detail"] = detail
							 }
	local function callback(suc,errorId)
		if suc then
			Gas2Gac:ReturnSaveAssociationPersonalInfo(Conn)
		else
			if IsNumber(errorId) then
				MsgToConn(Conn,errorId)
			end
		end
	end

	CallAccountManualTrans(Conn.m_Account, FriendsDB, "SavaPlayerInfo", callback, data)
end
-------------------------------------------------------------------------------------------------------
--@brief 查看个人基本信息
function CGasFriendBasic.GetAssociationPersonalInfo(Conn)
	local player = Conn.m_Player
	if not IsCppBound(player) then
		return
	end
	local data = {
								["uPlayerID"] = player.m_uID,
							 }
	local function callback(playerTbl)
		if playerTbl:GetRowNum() > 0 then
			Gas2Gac:ReturnGetAssociationPersonalInfo(Conn,playerTbl(1,1),playerTbl(1,2),playerTbl(1,3),
			playerTbl(1,4),playerTbl(1,5),playerTbl(1,6),playerTbl(1,7),playerTbl(1,8))
		end
	end

	CallAccountManualTrans(Conn.m_Account, FriendsDB, "SelectPersonalInfo", callback, data)
end

-------------------------------------------------------------------------------------------------------
--@brief 查看好友信息
--@param playerId：要查看好友的ID
function CGasFriendBasic.GetAssociationMemberInfo(Conn,playerId)
	local data = {
								["playerId"] = playerId
							 }
	local function callback(playerTbl)
		if nil ~= playerTbl and #playerTbl > 0 then
			Gas2Gac:ReturnGetAssociationMemberInfo(Conn,playerId,playerTbl[1],playerTbl[2],playerTbl[3],
				playerTbl[4],playerTbl[5],playerTbl[6],playerTbl[7],playerTbl[8],playerTbl[9],playerTbl[10],
				playerTbl[11],playerTbl[12],playerTbl[13])
		end
	end

	CallAccountManualTrans(Conn.m_Account, FriendsDB, "GetAssociationMemberInfo", callback, data)
end


