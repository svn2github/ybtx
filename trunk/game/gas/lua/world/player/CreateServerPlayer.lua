cfg_load "fb_game/FbActionDirect_Common"
gas_require "world/player/CreateServerPlayerInc"
gas_require "character/CharacterMediator"

-----------------------------------------------------------------
local function InitGroupMultiSender(Connection,friendGroupIdRes)
	local friendGroupIdTbl = friendGroupIdRes:GetTableSet()
	for i = 1,friendGroupIdRes:GetRowNum() do
		if not g_FriendGroupMultiMsgSenderTbl[friendGroupIdTbl(i,1)] then
			local friendGroupMsgSender = g_App:GetMultiMsgSender()
			g_FriendGroupMultiMsgSenderTbl[friendGroupIdTbl(i,1)] = friendGroupMsgSender
		end
		g_FriendGroupMultiMsgSenderTbl[friendGroupIdTbl(i,1)]:AddPipe(Connection)
	end
end

--玩家进入场景
local function _OnPlayerChangeInScene(Connection, uCharID, result)
	SaveCharLoginFuncFlow(uCharID,"_OnPlayerChangeInScene_begin")
	local function CallBack(team_id, oldServerId, friendGroupIdTbl)
		SaveCharLoginFuncFlow(uCharID,"CallBack")
		g_TemporaryTeamMgr:InitTemporaryTeamInfo(uCharID)
		CGasTeam.InitAllServerTeamInfo(team_id,uCharID)
		InitGroupMultiSender(Connection,friendGroupIdTbl)
	end
	local data = {}
	data["char_id"] = uCharID
	CallAccountAutoTrans(Connection.m_Account, "CharacterMediatorDB", "CharUpdateOnServerPos", CallBack, data, uCharID)
	
	apcall(g_PlayerInSceneFb,Connection.m_Player)
	local quest_info = CQuestNotepad:new(uCharID)
	quest_info:ChangeInScene_GetAllQuest(Connection.m_Player,result["QuestAllTbl"])
	SaveCharLoginFuncFlow(uCharID,"_OnPlayerChangeInScene_end")
end

--上线后发送系小助手消息
local function SendSystemMsg(SysteMsgTbl,Connection)
	if SysteMsgTbl:GetRowNum() > 0 then
		for i = 1,SysteMsgTbl:GetRowNum() do
			SystemFriendMsgToClient(Connection,SysteMsgTbl(i,1))
		end
	end
end

local function InitAssociationInfo(Connection,ret,uCharID)
	local AssociationInfo = ret["AssociationInfo"]
	local friendClassList	= AssociationInfo.friendClassList
	local friendsInfo		= AssociationInfo.friendsInfo
	local playerTbl			= AssociationInfo.playerTbl
	--向客户端发送好友分组信息
	CGasFriendBasic.SendFriendClassList(Connection, friendClassList)
	--向客户端发送玩家好友信息
	CGasFriendBasic.SendFriendInfo(Connection, friendsInfo)
	local friendGroupIdTbl = AssociationInfo.friendGroupIdTbl
	InitGroupMultiSender(Connection,friendGroupIdTbl)

	if playerTbl:GetRowNum() > 0 then
		Gas2Gac:ReturnGetAssociationPersonalInfo(Conn,playerTbl(1,1),playerTbl(1,2),playerTbl(1,3),
		playerTbl(1,4),playerTbl(1,5),playerTbl(1,6),playerTbl(1,7),playerTbl(1,8))
	end
	
	local groupIdRes = friendGroupIdTbl
	local groupIdTbl = {}
	if groupIdRes:GetRowNum() > 0 then
		local tblSet = groupIdRes:GetTableSet()
		for i = 1,groupIdRes:GetRowNum() do
			table.insert(groupIdTbl,tblSet(i,1))
		end
	end
	
	local data = {["CharID"] = uCharID}
	if Connection.m_Account then
		local function CallBack(result)
			local friendGroupList	= result.friendGroupList
			local allFriendsInfo	= result.allFriendsInfo
			--向客户端发送好友分组信息
			CGasFriendBasic.SendFriendGroupList(Connection, friendGroupList)
			--向客户端发送玩家好友群好友信息
			CGasFriendBasic.SendFriendInfoToClent(Connection, allFriendsInfo)
			Gas2Gac:ReturnInitFriendGroupInfoEnd(Connection)
		end
		
		local FriendsDB = "FriendsDB"
		CallAccountLoginTrans(Connection.m_Account, FriendsDB, "InitFriendGroupInfo", CallBack, data,unpack(groupIdTbl))
	end
end

local function InitGuideInfo(Conn, result)
	Gas2Gac:ReturnGetActionCountExBegin(Conn)
	for ActionName, CountsTbl in pairs(result[1]) do
		Gas2Gac:ReturnGetActionCountEx(Conn, ActionName, CountsTbl[1], CountsTbl[2])
	end
	for i = 1, result[2]:GetRowNum() do
		Gas2Gac:ReturnGetActionAllTimes( Conn, result[2]:GetData(i-1,0), result[2]:GetData(i-1,1) )
	end
	Gas2Gac:ReturnGetActionCountExEnd(Conn)
end

local function ShowMatchGameSign(Connection, playerLevel, uCharID)
	local actionNameTbl = {}
	for _, ActionName in pairs(FbActionDirect_Common:GetKeys()) do
		local IsShowPanel = FbActionDirect_Common(ActionName, "IsShowPanel")
		local MinMsgLevel = tonumber(FbActionDirect_Common(ActionName, "MinMsgLevel"))
		local MaxMsgLevel = tonumber(FbActionDirect_Common(ActionName, "MaxMsgLevel"))
		if IsShowPanel ~= "" and IsShowPanel == 1 then
			if CheckActionIsBeginFunc(ActionName) then
				if playerLevel >= MinMsgLevel and playerLevel <= MaxMsgLevel then
					table.insert(actionNameTbl, ActionName)
				end
			end
		end
	end
	return actionNameTbl
end

local function InitPlayerCanJoinAction(Connection, ret)
	if not IsCppBound(Connection) then
		return
	end
	if ret and table.getn(ret) > 0 then
		for _, canJoinAction in pairs(ret) do
			Gas2Gac:CreateMatchGameWnd(Connection, canJoinAction)
		end
	end
end

--玩家上线调用
local function _OnPlayerLoginGame(Connection, uCharID, result)
	SaveCharLoginFuncFlow(uCharID,"_OnPlayerLoginGame")
	--玩家上线副本活动处理
	apcall(g_PlayerLoginSceneFb,Connection.m_Player)
--	Connection.m_Player.m_DoingQuest = {}
	local quest_info = CQuestNotepad:new(uCharID)
	quest_info:GetAllQuest(Connection.m_Player,result["QuestAllTbl"],result["QuestVarTbl"],result["QuestAwardItemIndex"],result["QuestLoopTbl"],result["TongBuildTbl"])
	local actionNameTbl = ShowMatchGameSign(Connection, result["CharInfo"](1,10), uCharID)
	
	local function CallBack(ret)
		SaveCharLoginFuncFlow(uCharID,"CallBack(ret)")
		CGasTeam.InitAllServerTeamInfo(ret["TeamID"])
		InitAssociationInfo(Connection,ret,uCharID)
		SendSystemMsg(ret["SysteMsgTbl"],Connection)
		apcall(InitGuideInfo, Connection, ret["GuideInfo"])
		InitPlayerCanJoinAction(Connection, ret["CanJoinAction"])
	end
	local data = {}
	data["char_id"] = uCharID
	data["char_level"] = result["CharInfo"](1,10)
	data["actionInfo"] = actionNameTbl
	CallAccountAutoTrans(Connection.m_Account, "CharacterMediatorDB", "CharLoginGame", CallBack, data)
end

local function _AdultCheckOnLogin(uCharID)
	if GasConfig.SkipAdultCheck == 1 then return end
	local function CallBack(ret)
		if ret(1,1) > 0 then
				LogoutForAdultCheck(uCharID)
		end
	end
	local data = {}
	data["char_id"] = uCharID
	CallDbTrans("LoginServerDB", "GetOneCharLeftTimeInfo", CallBack,data)
end

-------------------------------------------------------------------

function ReuseServerPlayer( Connection, player, result, lastMsgIdStr)
	apcall(CCharacterMediator.ReUsePlayer,CCharacterMediator,Connection, player, result, lastMsgIdStr)
	apcall(player.DisableDirectorMoving,player,false)
	
	local char_id = player.m_uID
	local CChannelInitRes	= result["CChannelInitRes"]
	local teamInfo = result["teamInfo"]
	apcall(InitNewTongMemberPanel,char_id)
	local tong_id = result["TongID"]
	
	apcall(CChatChannelMgr.Init,char_id,CChannelInitRes)
	
	--通知小队和团队成员其上线并刷新标记信息
	apcall(CGasTeam.NotifyTeamInfoOnline,{Connection, char_id,teamInfo})
	--发送佣兵任务列表
	apcall(CRoleQuest.PlayerSendTempQuestList,Connection.m_Player)
	Connection.m_bCanCallTrans = true
	
	apcall(_OnPlayerLoginGame,Connection, char_id, result)
	g_AllConnection[char_id] = Connection
end

function CreateServerPlayer(Connection, ChangeType, uCharID, result, sceneInfo, lastMsgIdStr)
	SaveCharLoginFuncFlow(uCharID,"CreateServerPlayer_begin")
	local IsForbidLogin	= result["IsForbidLogin"]
	local CChannelInitRes	= result["CChannelInitRes"]
	if IsForbidLogin and Connection.ShutDown then
		--通知一下客户端说已经被禁止登陆了,这儿有点问题
		--如果被禁止，前面的数据库操作已经把冷却时间和buff的数据库data删除了以后处理
		Connection:ShutDown("用户被禁止登录,断开连接")
		LogErr("用户被禁止登录,断开连接","char_id:" .. uCharID)
		return
	end

	apcall(CCharacterMediator.Create,CCharacterMediator,Connection, uCharID, ChangeType, result, sceneInfo, lastMsgIdStr)
	
	apcall(g_AddPlayerInfo,uCharID, Connection.m_Player.m_Properties:GetCharName(), Connection.m_Player)
	------------------------------把正在做的任务挂到角色身上-----------------------------------

	apcall(CChatChannelMgr.Init,uCharID,CChannelInitRes)
	
	Connection.m_bCanCallTrans = true
	----
	if ChangeType == 1 then--切服登录的
		apcall(_OnPlayerChangeInScene,Connection, uCharID, result)
	else
		apcall(_OnPlayerLoginGame,Connection, uCharID, result)
			--通知小队和团队成员其上线并刷新标记信息
		apcall(CGasTeam.NotifyTeamInfoOnline,{Connection, uCharID,result["teamInfo"]})
		apcall(_AdultCheckOnLogin,uCharID)
		Gas2GacById:SendUserGameID(uCharID,Connection.m_nGameID)
	end
	apcall(CCharacterMediator.InitTongMember,Connection.m_Player,result,Connection)
	apcall(InitNewTongMemberPanel,uCharID)
	apcall(CCharacterMediator.InitTongTechInfo,Connection.m_Player,result,Connection)
	--发送佣兵任务列表
	apcall(CRoleQuest.PlayerSendTempQuestList,Connection.m_Player)
	g_AllConnection[uCharID] = Connection
	SaveCharLoginFuncFlow(uCharID,"CreateServerPlayer_end")
	UnRegisterCharLoginFuncFlowTick(uCharID)
	return Connection.m_Player
end

function DestroyServerPlayer( player, result, IsChangeScene)
	local function Init()
		if( not player or not IsCppBound(player))then
			return
		end
		
		local uCharID = player.m_uID
		local uObjID = player:GetEntityID()
		local sCharName = player.m_Properties:GetCharName()
		local Scene = player.m_Scene
		
		if IsChangeScene then
			local LeaveSceneQuestFailure = result["LeaveSceneQuestFailure"]
			if LeaveSceneQuestFailure and next(LeaveSceneQuestFailure) then
				for i=1, #(LeaveSceneQuestFailure) do
					if LeaveSceneQuestFailure[i]["DelSucc"] then
						CRoleQuest.QuestFailure_DB_Ret(player,LeaveSceneQuestFailure[i]["QuestName"])
					end
				end
			end
		end
		
		--玩家下线就不需要获取队友位置了
		apcall(CGasTeammatePosMgr.PlayerStopGetTeammatePos, player)
		
		if player.m_ActionLoadingTick then
			apcall(BreakPlayerActionLoading,player)
		end
		
		--如果玩家正在采集Obj需求要更新Obj的状态
		if player.m_CollIntObjID then
			apcall(BreakPlayerCollItem,player)
		end
		--如果玩家正在拾取Npc掉落
		if player.m_PickUpNpcID then
			apcall(BreakPlayerPickUpItem,player)
		end
		local Conn = player.m_Conn
		local function busystate_apcall()
			--如果在交易过程中下线，则通知交易对方----------------------------------------------------------
			
			if Conn then
				if player.m_Conn.busystate ~= nil and
					player.m_Conn.busystate == 1 then
					local playerTran = CPlayerTran:new(uCharID)
					playerTran:ClearTradeState(player.m_Conn)
				end
				Conn.m_Player = nil
			end
		end
		
		apcall(busystate_apcall)
		
		local function logout_apcall()
			--下线时对小队团队相关信息的判断
			if result ~= nil and IsTable(result["teamInfo"]) then
				local teamInfo = result["teamInfo"]
				CGasTeam.DealWithTeamInfoLogout(uCharID, teamInfo)
			end
			g_GasTongMgr:MemberLogout(uCharID, player.m_Properties:GetTongID())

            local charTongTechInfo = result["TongTechInfo"]
			apcall(CTongTech.InitTongTechInfo,Conn, false, charTongTechInfo)

			--社群系统下线通知好友
			local allFriendsInfo = result["allFriends"]
			if allFriendsInfo:GetRowNum() > 0 then
				local allFriendsInfoTbl = allFriendsInfo:GetTableSet()
				for i = 1, allFriendsInfo:GetRowNum() do
					--好友组Id,玩家ID
					Gas2GacById:NotifyFriendOffline(allFriendsInfoTbl(i,2), allFriendsInfoTbl(i,1),uCharID)
				end
			end
			--社群系统下线通知好友群成员
			local groupfriends = result["groupfriends"]
			if groupfriends:GetRowNum() > 0 then
				for i = 1, groupfriends:GetRowNum() do
					--玩家ID,所在群ID
					Gas2GacById:NotifyFriendGroupOffline(groupfriends(i,1),uCharID,groupfriends(i,2))
				end
			end
		end
		
		if not IsChangeScene then
			apcall(logout_apcall)
		end
		
		local TongID = result["TongID"] --player.m_Properties:GetTongID()
		if TongID and TongID > 0 then
			apcall(CGasTongmatePosMgr.PlayerStopGetTongmatePos,player)	--玩家下线就不需要获取帮会成员位置了
			apcall(CGasTongmatePosMgr.StopSendLeavedTongmatePos,TongID, uCharID) -- 通知其他帮会其他成员不显示下线成员的图标
		end
				
		apcall(CFbPlayerPosMgr.StopSendLeavedFbPlayerPos, Scene, player)	-- 玩家下线就不获取副本内玩家位置了
			
		--如果是在副本中，跟据不同，删掉场景
		Scene.m_tbl_Player[uCharID] = nil
		--g_MainSceneAreaMgr:UndateAreaPlayerInfo(Scene)
		apcall(LeaveScene,Scene, player)
		
		apcall(player.Destroy, player)	
		player = nil
		g_DelPlayerInfo(uCharID, sCharName)
		g_AllConnection[uCharID] = nil
		------------------------------------
	end
	apcall(Init)
end 

--原始的切场景调用
function NewServerPlayerChangeScene( Player)
	
	local uCharID = Player.m_uID
	
	--如果玩家正在采集Obj需求要更新Obj的状态
	if Player.m_CollIntObjID then
		BreakPlayerCollItem(Player)
	end
	--如果玩家正在拾取Npc掉落
	if Player.m_PickUpNpcID then
		BreakPlayerPickUpItem(Player)
	end
	
	--副本玩法相关
	g_PlayerChangeSceneInFb(Player)
	
	--如果在交易过程中下线，则通知交易对方--------------------------------
	if Player.m_Conn.busystate ~= nil and
		Player.m_Conn.busystate == 1 then
		local playerTran = CPlayerTran:new(uCharID)
		playerTran:ClearTradeState(Player.m_Conn)
	end
	---------------------------------------------------------------------------
		
end


--这个函数是用来保证所有的主角都执行玩destory操作后再调用EndService的时序
local function OnSaveAllPlayerJobEnd()
	local function callback()
		EndService()
	end
	CallDbTrans("CharacterMediatorDB", "OnSaveAllPlayerJobEnd", callback, {})
end

function KickAllPlayer()
	g_bServerShuttingDown = true
	for _,v in pairs(g_AllConnection) do
		if v.m_Player then
			v.m_Player.m_bDisabled = true
			local function callback()
				if v.ShutDown then
					v:ShutDown("关服")
				end
			end
			LeaveGame(v.m_Player, v.m_Player.m_AccountID, v.m_UserName, callback)
			StopUpdateOnlineTime(v.m_UpOnlineTimeTick)
		end
	end
	
	for _, o in pairs(g_AllDisabledPlayer) do
		local function callback()
			if o.m_Conn and o.m_Conn.ShutDown then
				o.m_Conn:ShutDown("关服")
			end
		end
		LeaveGame(o, o.m_AccountID, o.m_UserName, callback)
	end
	
	OnSaveAllPlayerJobEnd()
end
--获取验证码
--[[
	实现方式，生成一个4位数字的随机数
--]]


