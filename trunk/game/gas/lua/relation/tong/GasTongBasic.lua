gac_gas_require "framework/text_filter_mgr/TextFilterMgr"

local tong_box = "GasTongBasicDB"
local textFltMgr = CTextFilterMgr:new()

CTongBasic = class()

function CTongBasic.PreChangeTongName(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
		if IsNumber(result) then
			MsgToConn(Conn,result)
			return
		else
			Gas2Gac:ReturnPreChangeTongName(Conn)
		end
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "PreChangeTongName", CallBack, parameters)
end

--【帮会改名】
function CTongBasic.ChangeTongName(Conn, name)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	--去掉前后空格
	if( not textFltMgr:IsValidMsg(name) ) then
		MsgToConn(Conn,9198)
		return
	end
	local sTongName = textFltMgr:RemoveTab1(name)

	--回调函数【高速逻辑】
	local function CallBack(result,tong_id)
		if IsNumber(result) then
			MsgToConn(Conn,result)
			return
		else
			CTongBasic.SetCharTongInfo(uPlayerID,tong_id)
		end
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.sTongName	= sTongName
	CallAccountManualTrans(Conn.m_Account, tong_box, "ChangeTongName", CallBack, parameters)
end
--【申请创建帮会】
function CTongBasic.RequestCreateTong(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"佣兵小队创建处")	then
		return
	end
	--回调函数【高速逻辑】
	local function CallBack(MsgID)
		if IsNumber(MsgID) then
			MsgToConn(Conn,MsgID)
			return
		end
		Gas2Gac:ReturnRequestCreateTong(Conn)
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "RequestCreateTong", CallBack, parameters)
end

--【创建帮会】
function CTongBasic.CreateTong(Conn, name,purpose)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"佣兵小队创建处")	then
		return
	end
	--去掉前后空格
	if( not textFltMgr:IsValidMsg(name) ) then
		MsgToConn(Conn,9198)
		return
	end
	if( not textFltMgr:IsValidMsg(purpose) ) then
		MsgToConn(Conn,9199)
		return
	end
	local sTongName = textFltMgr:RemoveTab1(name)
	local sTongPorpose = textFltMgr:RemoveTab1(purpose)
	if string.len(sTongName) == 0 then
		--"名称不能为空"
		MsgToConn(Conn,9176 )
		return
	end
	if string.len(sTongName) > 32 then
		--"帮会名称长度不能大于32"
		MsgToConn(Conn,9177)
		return
	end
	if string.len(sTongPorpose) > 1024 then
		--"帮会宗旨长度不能大于512"
		MsgToConn(Conn,9178)
		return
	end
	local function _Local_CreateTong(tong_id)
		--回调函数【高速逻辑】
		local function CallBack(bFlag,uTongID,regist_money)
			if not Player then return end
			if IsNumber(bFlag) then
				MsgToConn(Conn,bFlag)
				return
			else
				MsgToConn(Conn,9002)
				CTongBasic.AddTongMember(uPlayerID,uTongID,sTongName,0,"")
				InitNewTongMemberPanel(uPlayerID)
				Gas2Gac:ReturnPosChanged(Conn ,g_TongMgr:GetPosIndexByName("团长"))
				Gas2Gac:ReturnCreateTong(Conn)
				--任务事件需求加计数
				AddVarNumForTeamQuest(Player, "完成建团", 1)
				AddVarNumForTeamQuest(Player, "完成入团", 1)
			end
		end
		local parameters = {}
		parameters.uPlayerID	= uPlayerID
		parameters.sTongName	= sTongName
		parameters.sTongPorpose	= sTongPorpose
		parameters.tong_camp	= Player:CppGetBirthCamp()
		CallAccountManualTrans(Conn.m_Account, tong_box, "CreateTong", CallBack, parameters)
	end
	local function CreateGroupFailed(return_code)
		
	end
	_Local_CreateTong()
	--g_EProtocol:CreateGroup({uPlayerID,sTongName,1},{_Local_CreateTong,CreateGroupFailed})
end

function CTongBasic.ReturnGetTongInfo(Conn,info)
	local bWarState = info.bWarState
  local str = "无战"
  if bWarState then
  	str = "挑战"
  end
  if g_WarZoneMgr.m_Warring then
  	str = "战争"
  end
  info.nWarState = g_TongMgr.m_tblChallengeState[str] --战争状态
	Gas2Gac:ReturnGetTongInfo(Conn,info.tong_id,info.tong_name,info.leader_name,
				info.member_num,info.upper_member,info.tong_level,
				info.tong_honor,info.tong_exploit,info.tong_money,
				info.tong_resource,info.upper_resource,info.tong_purpose,
				info.nWarState,info.warZoneId,info.stationId,info.tong_type,
				info.tong_developdegree,info.tong_permitUpLevel,info.tong_resName)
end

--【获得帮会信息】
function CTongBasic.GetTongInfo(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	--回调函数【高速逻辑】
	local function CallBack(info)
		if next(info) then
			CTongBasic.ReturnGetTongInfo(Conn,info)
		end
	end
	
	local data = {}
	data.uPlayerID	= uPlayerID
	CallDbTrans(tong_box, "GetTongInfoDB", CallBack, data)
end

--【获得申请信息】
function CTongBasic.GetTongRequestInfo(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID	
	--回调函数【高速逻辑】
	local function CallBack(info,proffer_info)
		Gas2Gac:ReturnGetTongRequestInfoBegin(Conn)
		for i =1 ,info:GetRowNum() do
			Gas2Gac:ReturnGetTongRequestInfo(Conn,info(i,1),info(i,2),info(i,3),info(i,4),info(i,5),proffer_info[info(i,1)])
		end
		Gas2Gac:ReturnGetTongRequestInfoEnd(Conn,g_TongMgr.m_nMaxRequestNum)
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallDbTrans(tong_box, "GetRequestInfoRPC", CallBack, parameters)
end

--【获得帮会所有成员信息】
function CTongBasic.GetTongMemberInfo(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID

	local function CallBack(info, uMaxRequestNum,exploit_info)
		Gas2Gac:ReturnGetTongMemberInfoStart(Conn)
		for i =1 ,info:GetRowNum() do
			local id,pos,level,class,name,bOnline,proffer,proffer_expend,tong_proffer,outLineTime,nowTime = info(i,1),info(i,2),info(i,3),info(i,4),
						info(i,5),info(i,6),info(i,7),info(i,8),info(i,9),info(i,10),info(i,11)
			local exploit = exploit_info[id] or 0
			local online = 1
			if bOnline == 0 then 
				online = 2			--玩家是否在线，1是在线，2不在线
			else
				outLineTime = nowTime
			end
			Gas2Gac:ReturnGetTongMemberInfo(Conn,id,name,pos,level,class,proffer,proffer_expend,tong_proffer,exploit,online,outLineTime)
		end
		Gas2Gac:ReturnGetTongMemberInfoEnd(Conn, uMaxRequestNum)
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallDbTrans(tong_box, "GetTongMember", CallBack, parameters)
end

--【获得日志信息（根据日志类型）】
function CTongBasic.GetTongLog(Conn, type)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID	
	local function CallBack(info)
		local row = info:GetRowNum()
		for i =1 ,row do
			Gas2Gac:ReturnGetTongLog(Conn, info(i,5), info(i,4), info(i,3))
		end
		Gas2Gac:ReturnGetTongLogEnd(Conn)
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.uLogType	= type
	CallDbTrans(tong_box, "GetLogsInfoRPC", CallBack, parameters)
end

--【修改帮会宗旨】
function CTongBasic.ChangeTongPurpose(Conn, sTongPorpose)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if( not textFltMgr:IsValidMsg(sTongPorpose) ) then
		MsgToConn(Conn,9199)
		return
	end
	local purpose = textFltMgr:RemoveTab1(sTongPorpose)
	if string.len(purpose) > 1024 then
		MsgToConn(Conn,9178)
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID)
		if IsNumber(uMsgID) then
			MsgToConn(Conn, uMsgID)
		end
		Gas2Gac:ReturnChangeTongPurpose(Conn,bFlag)
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.purpose = purpose
	CallAccountManualTrans(Conn.m_Account, tong_box, "ChangeTongPurpose", CallBack, parameters, uTongID)
end

--【修改职位】
function CTongBasic.ChangePos(Conn, target_id, old_position, new_position)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	--判断修改的是否是自己的职位
	if uPlayerID == target_id then
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID, posFlag, charId)
		if IsNumber(uMsgID) then
			MsgToConn(Conn, uMsgID)
		end
		if bFlag then
			MsgToConn(Conn,9159)
		end
		Gas2Gac:ReturnChangePos(Conn, bFlag)
		Gas2GacById:ReturnPosChanged(target_id,new_position)
		if posFlag == false then
			Gas2GacById:RetChangeInfoWnd(charId)
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_id	= target_id
	parameters.old_position	= old_position
	parameters.new_position	= new_position
	CallAccountManualTrans(Conn.m_Account, tong_box, "UpdatePos", CallBack, parameters, uTongID)
end

--【卸任】
function CTongBasic.TongResign(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID, posFlag, charId)
		if not bFlag and IsNumber(uMsgID) then
			MsgToConn(Conn,uMsgID)
			return
		end
		Gas2Gac:ReturnTongResign(Conn, bFlag)
		if bFlag then
			Gas2Gac:ReturnPosChanged(Conn ,g_TongMgr:GetPosIndexByName("团员"))
			if not posFlag then
				Gas2GacById:RetChangeInfoWnd(charId)
			end
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "DismissPosDB", CallBack, parameters, uTongID)
end

--【开除】
function CTongBasic.KickOutOfTong(Conn, target_id)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if target_id == uPlayerID then return end
	
	--回调函数【高速逻辑】
	local function CallBack(tblRetRes,target_name,TongID)
		if not tblRetRes then return end
		if IsNumber(tblRetRes) then
			if target_name then 
				MsgToConn(Conn, tblRetRes,target_name)
			else
				MsgToConn(Conn, tblRetRes)
			end
			Gas2Gac:ReturnKickOutOfTong(Conn,false)
		else
			local tong_members = g_GasTongMgr:GetMembers(TongID)
			for m =1,#tong_members do
				local member_id = tong_members[m]
				if member_id ~= target_id then
					MsgToConnById(member_id, 9325,target_name)
				end
			end
			MsgToConnById(target_id,9326)
			CTongBasic.DelTongMember(target_id,TongID)
			Gas2GacById:ReturnBeOutOfTong(target_id)			
			Gas2GacById:ReturnPosChanged(target_id ,0)
			-- 玩法组，中地图上帮会成员图标更新
			if Player and IsCppBound(Player) then
				CGasTongmatePosMgr.PlayerStopGetTongmatePos(Player)
			end
			Gas2GacById:StopSendTongmatePos(target_id)
			CGasTongmatePosMgr.StopSendLeavedTongmatePos(TongID, target_id)
			Gas2Gac:ReturnKickOutOfTong(Conn,true)
			-- 玩法组，龙穴活动，成员被开除则T出副本
			for server_id in pairs(g_ServerList) do
				Gas2GasAutoCall:KickOutOfCave(GetServerAutoConn(server_id),target_id)
			end
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_id	= target_id
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, tong_box, "DropMemberRPC", CallBack, parameters, uTongID)
end

--【退出帮会】
function CTongBasic.LeaveTong(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	--回调函数【高速逻辑】
	local function CallBack(tblRetRes,tong_id)
		if not tblRetRes then return end
		if IsNumber(tblRetRes) then
			MsgToConn(Conn, tblRetRes)
			return
		else
			MsgToConn(Conn, 9113)
			local scene = Player.m_Scene
			local sceneType = scene.m_SceneAttr.SceneType
			if sceneType == 26 or sceneType == 28 then
				local sceneName = Player.m_MasterSceneName
				local pos = Player.m_MasterScenePos 
				ChangeSceneByName(Player.m_Conn,sceneName,pos.x,pos.y)
			end
			CTongBasic.DelTongMember(uPlayerID,tong_id)			
			MsgToConn(Conn,9186)
			Gas2Gac:ReturnBeOutOfTong(Conn)
			InitNewTongMemberPanel(uPlayerID)
			-- 玩法组，中地图上帮会成员图标更新
			CGasTongmatePosMgr.PlayerStopGetTongmatePos(Player)
			Gas2GacById:StopSendTongmatePos(uPlayerID)
			CGasTongmatePosMgr.StopSendLeavedTongmatePos(tong_id, uPlayerID)
			--g_EProtocol:DeleteGroup({100001,uPlayerID})
		end
	end
	
	local parameters = {}
	parameters.uPlayerID = uPlayerID
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, tong_box, "LeaveTong", CallBack, parameters, uTongID)
end

--【帮会中有权限者响应申请【批准or拒绝】
function CTongBasic.BeJoinTong(Conn, target_id, bBlooen)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uPlayerID == target_id then return end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID,tong_id,tong_name,target_name,ArmyCorpsId,ArmyCorpsName,tempFlag)
		if not bFlag then
			if IsNumber(uMsgID) and uMsgID ~= 9376 then
				MsgToConn(Conn, uMsgID)
				else
				local tong_name = Player.m_Properties:GetTongName()
				MsgToConnById(target_id,uMsgID,tong_name)	
			end
		else
			MsgToConnById(target_id,uMsgID,tong_name)
			MsgToConn(g_GasTongMgr:GetTongMsgSender(tong_id),9375,target_name)
--			local tong_id = uMsgID
			if bBlooen then
--				if tempFlag then
--					Gas2GacById:RetInsertFbActionToQueue(target_id,"抢夺资源点",false,0)
--				end
				CTongBasic.AddTongMember(target_id,tong_id,tong_name,ArmyCorpsId,ArmyCorpsName)				
				Gas2GacById:ReturnJoinInTong(target_id)
				-- 玩法组，创建获取帮会成员位置的Tick
				CGasTongmatePosMgr.CreateGetTongmatePosTick(tong_id,target_id)
				--任务事件需求加计数
				for ServerId, _ in pairs(g_ServerList) do
					Gas2GasAutoCall:AddVarNumForTeamQuest(GetServerAutoConn(ServerId), target_id, "完成入团", 1)
				end
			end
		end
		Gas2Gac:ReturnBeJoinTong(Conn,bFlag)
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_id	= target_id
	parameters.bBlooen	= bBlooen
	 
	CallAccountManualTrans(Conn.m_Account, tong_box, "ResponseRequest", CallBack, parameters, uTongID)
end

--【显示所有帮会的名称和人数】
function CTongBasic.GetAllTongSomeInfo(Conn)
	local function CallBack(bFlag,tbl,uMsgID)
		if bFlag then
			for i=1,#tbl do
				if tbl[i][3] ~= tbl[i][8] then
					Gas2Gac:ReturnGetAllTongSomeInfo(Conn,tbl[i][1],tbl[i][2],tbl[i][3],tbl[i][4],tbl[i][5],tbl[i][6],tbl[i][7],tbl[i][8],tbl[i][9])
				end
			end
			Gas2Gac:ReturnGetAllTongSomeInfoEnd(Conn)
		else
			MsgToConn(Conn,uMsgID)
		end
	end
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "GetAllTong", CallBack,parameters)
end

--【查询输入的帮会是否存在】
function CTongBasic.SearchTong(Conn,sName)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local function CallBack(bFlag, uMsgID)
		if not bFlag then
			MsgToConn(Conn,uMsgID)
		else
			Gas2Gac:ShowFoundTong(Conn,sName)
		end
	end
	local parameters = {}
	parameters.sName = sName
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "SearchTongDB", CallBack,parameters)
end

--【获得所有未加入佣兵小队的玩家信息】
function CTongBasic.GetAllPlayerSomeInfo(Conn)
	local function CallBack(tbl)
		for i=1,#tbl do
			Gas2Gac:ReturnGetAllPlayerSomeInfo(Conn,tbl[i][1],tbl[i][2],tbl[i][3],tbl[i][4],tbl[i][5],tbl[i][6],tbl[i][7])
		end
		Gas2Gac:ReturnGetAllPlayerSomeInfoEnd(Conn)
	end
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "GetAllPlayer", CallBack,parameters)
end

--【查询输入的玩家是否存在】
function CTongBasic.SearchPlayer(Conn,sName)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local function CallBack(bFlag, uMsgID)
		if not bFlag then
			MsgToConn(Conn,uMsgID)
		else
			Gas2Gac:ShowFoundPlayer(Conn,sName)
		end
	end
	local parameters = {}
	parameters.sName = sName
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, tong_box, "SearchPlayerDB", CallBack,parameters)
end

--【申请加入帮会】
function CTongBasic.RequestJoinTong(Conn,tong_id)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if Player.m_Properties:GetTongID() ~= 0 then
		MsgToConn(Conn, 9009)
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID,ViceCaptainIdRet, CaptainIdRet)
		if IsNumber(uMsgID) then
			MsgToConn(Conn, uMsgID)
		end
		if bFlag then
			local row1 = ViceCaptainIdRet:GetRowNum()
			if row1 > 0 then
				for n = 1, row1 do
					local ViceCaptainId = ViceCaptainIdRet(n,1)
					Gas2GacById:OnRequestJoinInTong(ViceCaptainId)
				end
			end
			local row2 = CaptainIdRet:GetRowNum()
			if row2 > 0 then
				local CaptainId = CaptainIdRet(1,1)
				Gas2GacById:OnRequestJoinInTong(CaptainId)
			end
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.tong_id	= tong_id
	parameters.tong_camp	= Player:CppGetBirthCamp()
	CallAccountManualTrans(Conn.m_Account, tong_box, "RequestJoinTong", CallBack, parameters)
	
end

function CTongBasic.RequestJoinTongByName(Conn,target_name)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if Player.m_Properties:GetTongID() ~= 0 then
		MsgToConn(Conn, 9009)
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID)
		if IsNumber(uMsgID) then
			MsgToConn(Conn, uMsgID)
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_name	= target_name
	parameters.tong_camp	= Player:CppGetBirthCamp()
	CallAccountManualTrans(Conn.m_Account, tong_box, "RequestJoinTongByName", CallBack, parameters)
end

--【推荐某人加入帮会】
function CTongBasic.RecommendJoinTong(Conn, target_name, sInfor)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if string.len(sInfor) > 100 then
		MsgToConn(Conn, 9136)
		return
	end
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID,sMsg)
		if (not bFlag) and IsNumber(uMsgID) then
			if sMsg then
				MsgToConn(Conn, uMsgID,sMsg)
			else
				MsgToConn(Conn, uMsgID)
			end
		end
		if bFlag then
			local tong_name = sMsg
			local target_id = uMsgID
			Gas2GacById:ReseiveRecommendJoinTong(target_id, uPlayerID, Player.m_Properties:GetCharName(),tong_name)
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_name	= target_name
	parameters.sInfor	= sInfor
	
	CallAccountManualTrans(Conn.m_Account, tong_box, "RecommendJoinTong", CallBack, parameters, Player.m_Properties:GetTongID())
end

--[响应引荐]
function CTongBasic.ResponseBeRecommendJoinTong(Conn, inviterId, bFlag)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if not bFlag then
		--拒绝
		MsgToConnById(inviterId,9303,Player.m_Properties:GetCharName())
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(RetMsg,inviter_name)
		if IsNumber(RetMsg) then
			MsgToConn(Conn, RetMsg)
		end
		if RetMsg == true then
			MsgToConnById(inviterId,9304,Player.m_Properties:GetCharName())
			MsgToConn(Conn, 9313,inviter_name)
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.inviterId	= inviterId	
	CallAccountManualTrans(Conn.m_Account, tong_box, "ResponseRecommendJoinTong", CallBack, parameters)
end


--【邀请】
function CTongBasic.InviteJoinTong(Conn, target_id)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local uPlayerID = Player.m_uID
	local player_name = Player.m_Properties:GetCharName()

	local function CallBack(bFlag,uMsgID,sMsg)
		if (not bFlag) and IsNumber(uMsgID) then
			if sMsg then
				MsgToConn(Conn, uMsgID,sMsg)
			else
				MsgToConn(Conn, uMsgID)
			end
			return
		end
		if bFlag then
			local tong_name = uMsgID
			Gas2GacById:ReseiveInviteJoinTong(target_id,uPlayerID,player_name,tong_name)
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_id	= target_id
	CallAccountManualTrans(Conn.m_Account, tong_box, "InviteJoinTongDB", CallBack, parameters, Player.m_Properties:GetTongID())
end
--【邀请】
function CTongBasic.InviteJoinTongByName(Conn, target_name)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local uPlayerID = Player.m_uID
	local player_name = Player.m_Properties:GetCharName()

	local function CallBack(bFlag,uMsgID,sMsg,uTargetID)
		if (not bFlag) and IsNumber(uMsgID) then
			if sMsg then
				MsgToConn(Conn, uMsgID,sMsg)
			else
				MsgToConn(Conn, uMsgID)
			end
			return
		end
		if bFlag then
			local tong_name = uMsgID
			Gas2GacById:ReseiveInviteJoinTong(uTargetID,uPlayerID,player_name,tong_name)
		end
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.target_name	= target_name
	CallAccountManualTrans(Conn.m_Account, tong_box, "InviteJoinTongByNameDB", CallBack, parameters, Player.m_Properties:GetTongID())
end

--【响应邀请】
function CTongBasic.ResponseBeInviteToTong(Conn, inviter_id,bBlooean)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local sCharName = Player.m_Properties:GetCharName()
	if not bBlooean then
		MsgToConnById(inviter_id,9322,Player.m_Properties:GetCharName())
		Gas2GacById:ReseiveResponseBeInviteToTong(inviter_id,Player.m_Properties:GetCharName(),false)
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgID,tong_id,tong_name,ArmyCorpsId,ArmyCorpsName)
		if not bFlag and IsNumber(uMsgID) then
			MsgToConn(Conn, uMsgID)
			return
		end
		if bFlag  then
			Gas2GacById:ReseiveResponseBeInviteToTong(inviter_id,Player.m_Properties:GetCharName(),bBlooean)
			if bBlooean then
				MsgToConn(Conn,9110)
				local tong_members = g_GasTongMgr:GetMembers(tong_id)
				for m =1,#tong_members do
					local member_id = tong_members[m]
					if member_id ~= uPlayerID then
						MsgToConnById(member_id,9321,sCharName)
					end
				end
				CTongBasic.AddTongMember(uPlayerID,tong_id,tong_name,ArmyCorpsId,ArmyCorpsName)
				InitNewTongMemberPanel(uPlayerID)
				Gas2GacById:ReturnJoinInTong(uPlayerID)
				-- 玩法组，创建获取帮会成员位置的Tick
				CGasTongmatePosMgr.CreateGetTongmatePosTick(tong_id,uPlayerID,inviter_id)
				--任务事件需求加计数
				AddVarNumForTeamQuest(Player, "完成入团", 1)
			end
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.inviter_id	= inviter_id
	CallAccountManualTrans(Conn.m_Account, tong_box, "ResponseRecruitJoinTongDB", CallBack, parameters)
end

local AutoChangeCaptainsTime = 36*60*60
local AutoChangeCaptainsModulus = 12*60*60


--如果因为特殊原因导致佣兵小队无队长，佣兵团无团长，这里处理
function CTongBasic.CheckAndChangeTongCaptain()
	local data = {}
	data["uTime"] = AutoChangeCaptainsTime
	CallDbTrans(tong_box, "CheckAndChangeTongCaptain", nil, data,"CheckAndChangeTongCaptain" )
end

--自动转移团长
function CTongBasic.AutoChangeTongCaptainTick(uTime,k)
	local function CallBack(ret)
		if g_AutoChangeTongCaptain then
			UnRegisterTick(g_AutoChangeTongCaptain)
			g_AutoChangeTongCaptain = nil
		end
		if ret then
			for i,v in pairs(ret) do
				Gas2GacById:ReturnPosChanged(v[2],g_TongMgr:GetPosIndexByName("团长"))
				Gas2GacById:ReturnPosChanged(v[5],g_TongMgr:GetPosIndexByName("团员"))
			end
		end
		function AutoChangeTongCaptain()
			local function AutoCallBack(ret)
				if ret then
					for i,v in pairs(ret) do
						Gas2GacById:ReturnPosChanged(v[2],g_TongMgr:GetPosIndexByName("团长"))
						Gas2GacById:ReturnPosChanged(v[5],g_TongMgr:GetPosIndexByName("团员"))
						if v[1] then
							CArmyCorpsBase.DealArmyCorpsPosWhenDelChar(v[1],v[2], v[3],v[4])
							if v[6] == false then
								Gas2GacById:RetChangeInfoWnd(v[7])
							end
						end
					end
				end
			end
			local data = {}
			data["uTime"] = AutoChangeCaptainsTime
			data["Modulus"] = AutoChangeCaptainsModulus
			CallDbTrans(tong_box, "AutoChangeCaptain", AutoCallBack, data,"AutoChangeTongCaptain" )
		end
		local uTickTime = 12*60*60*1000
		if uTime then
			uTickTime = uTime*1000
		end
		g_AutoChangeTongCaptain = RegisterTick("g_AutoChangeTongCaptain", AutoChangeTongCaptain,uTickTime)
	end	
	if uTime then
		AutoChangeCaptainsTime = uTime
		AutoChangeCaptainsModulus = k
	end
	local data = {}
	data["uTime"] = AutoChangeCaptainsTime
	data["Modulus"] = AutoChangeCaptainsModulus
	CallDbTrans(tong_box, "AutoChangeCaptain", CallBack, data,"AutoChangeTongCaptain" )
end

function CTongBasic.ImmediatelyCalculateTongDevelopDegree()
	local function CallBack()
	end
	CallDbTrans(tong_box, "CalculateTongDevelopDegree", CallBack, {},"CalculateTongDevelopDegree" )
end
--添加任务:没周日24:00点清空发展度
function CTongBasic.SettleAccountsForTongDevelopDegree()
	local function SettleAccounts()
		CTongBasic.ImmediatelyCalculateTongDevelopDegree()
	end
	g_AlarmClock:AddTask("CalculateTongDevelopDegree", {wday = {7},time = {"24:00"}}, SettleAccounts)
end

function CTongBasic.ImmediatelySendMailToTongCaptain()
	CallDbTrans(tong_box,"SendMailToTongCaptainDB",nil,{},"SendMailToTongCaptain")
end

--每周日晚上7点给佣兵小队等级不小于2级的队长发送奖励物品
function CTongBasic.SendAwardToTongCaptain()
	local function SendMailToTongCaptain()
		CTongBasic.ImmediatelySendMailToTongCaptain()
	end
	g_AlarmClock:AddTask("SendMailToTongCaptain", {wday = {7},time = {"19:00"}}, SendMailToTongCaptain)
end

----------------------------------------多服间广播消息--------------------------------------
function CTongBasic.SetCharTongInfo(char_id,tong_id)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:SetCharTongInfo(GetServerAutoConn(server_id),server_id,char_id,tong_id)
	end
end

function CTongBasic.AddTongMember(uPlayerID,uTongID,sTongName,ArmyCorpsID,ArmyCorpsName)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:AddTongMember(GetServerAutoConn(server_id),server_id,uPlayerID,uTongID,sTongName,ArmyCorpsID,ArmyCorpsName)
		Gas2GasAutoCall:InitNewPanel(GetServerAutoConn(server_id),target_id)
	end
end

function CTongBasic.DelTongMember(uPlayerID,uTongID)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:DelTongMember(GetServerAutoConn(server_id),server_id,uPlayerID,uTongID)
		Gas2GasAutoCall:InitNewPanel(GetServerAutoConn(server_id),target_id)
	end
end

function CTongBasic.GetTongLevel(Conn)
    local data = {}
    local Player = Conn.m_Player
    if not (Player and IsCppBound(Player) and IsCppBound(Conn)) then return end
    data.uPlayerID = Player.m_uID
    local function CallBack(tongLevel, stationLinLevel)
        Gas2Gac:ReturnTongLevel(Conn, tongLevel, stationLinLevel)
    end
    
    CallAccountManualTrans(Conn.m_Account, tong_box, "GetTongLevelByPlayerID", CallBack, data)
end

function CTongBasic.GetTongProfferCount(Conn)
    local data = {}
    local Player = Conn.m_Player
    if not (Player and IsCppBound(Player) and IsCppBound(Conn)) then return end
    data.uPlayerID = Player.m_uID
    local function CallBack(profferCount)
        Gas2Gac:ReturnTongProfferCount(Conn, profferCount)
    end
    
    CallAccountManualTrans(Conn.m_Account, tong_box, "GetProffer", CallBack, data)
end

function CTongBasic.TongTypeChanged(Conn,succ,succType,tongMember,bNeedNotify)
	if succ then
		if succType == 1 then
			if not bNeedNotify then
				return
			end
			for i =1 ,tongMember:GetRowNum() do
				local id,bOnline = tongMember(i,1),tongMember(i,6)
				if bOnline ~= 0 then 
					Gas2GacById:ShowTongMsg(id,10044)
				end
			end
		elseif succType == -1 and tongMember then
			for i =1 ,tongMember:GetRowNum() do
				local id,bOnline = tongMember(i,1),tongMember(i,6)
				if bOnline ~= 0 then 
					Gas2GacById:TongTypeChanged(id,g_TongMgr:GetTongTypeByName("战斗"))
					if bNeedNotify then
						Gas2GacById:ShowTongMsg(id,10043)
					end
				end
			end
		end
	end
end

function CTongBasic.UpTongType(Conn,uUpToType)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID

	local function CallBack(bSucc, bNotity, info, tongName)
		if bSucc then
			Gas2Gac:TongTypeChanged(Conn,uUpToType)
			if bNotity then
				local uMsgID = uUpToType == g_TongMgr:GetTongTypeByName("精英战斗") and 10042 or 0
				if uUpToType == g_TongMgr:GetTongTypeByName("精英战斗") then
					for i =1 ,info:GetRowNum() do
						local id,bOnline = info(i,1),info(i,6)
						if bOnline ~= 0 then 
							Gas2GacById:ShowTongMsg(id,uMsgID)
						end
					end
				end
				if tongName then
					for server_id in pairs(g_ServerList) do
						Gas2GasAutoCall:SendUpTongTypeMsg(GetServerAutoConn(server_id), tongName)
					end
				end	
			end
		else
			MsgToConn(Conn,info)
		end
	end
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["uUpToType"]	= uUpToType
	local uTongID = Player.m_Properties:GetTongID()
	CallAccountManualTrans(Conn.m_Account, tong_box, "UpTongType", CallBack, parameters,uTongID)
end

function CTongBasic.ChangeTongType(Conn, TongType)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID

	local function CallBack(bSucc, uMsgId, info)
		if bSucc then
			Gas2Gac:TongTypeChanged(Conn,TongType)
			local msgID = TongType == g_TongMgr:GetTongTypeByName("战斗") and 10050 or 10051
			for i =1 ,info:GetRowNum() do
				local id,bOnline = info(i,1),info(i,6)
				if bOnline ~= 0 then
					Gas2GacById:ShowTongMsg(id,msgID)
				end
			end
		else
			MsgToConn(Conn,uMsgId)
		end
	end
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["uTongType"]	= TongType
	local uTongID = Player.m_Properties:GetTongID()
	CallAccountManualTrans(Conn.m_Account, tong_box, "ChangeTongType", CallBack, parameters,uTongID)
end

function CTongBasic.ResetTongType(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID

	local function CallBack(bSucc, uMsgId)
		if bSucc then
			Gas2Gac:TongTypeChanged(Conn,g_TongMgr:GetTongTypeByName("普通"))
		elseif uMsgId then
			MsgToConn(Conn,uMsgId)
		end
	end
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	local uTongID = Player.m_Properties:GetTongID()
	CallAccountManualTrans(Conn.m_Account, tong_box, "ResetTongType", CallBack, parameters,uTongID)
end

function CTongBasic.AddTongDevelopDegree(Conn, uPoint)
	local Player = Conn.m_Player
	if not Player then return false end
	local uTongID = Player.m_Properties:GetTongID()
	if Player == 0 then return false end

	local parameters = {}
	parameters["uTongID"]	= uTongID
	parameters["uPoint"] = uPoint
	parameters["uEventType"] = event_type_tbl["任务加发展度奖励"]
	CallAccountAutoTrans(Conn.m_Account, tong_box, "AddTongDevelopDegree", nil, parameters,uTongID)
end

function CTongBasic.UpdateExploitOnKillOther(Killer, BeAttacker)
	local KillerTeamId = Killer.m_Conn.m_Player.m_Properties:GetTeamID()
	local KillerTongId = Killer.m_Conn.m_Player.m_Properties:GetTongID()
	local KillerPlayerId = Killer.m_Conn.m_Player.m_Properties:GetCharID()
	local KilledId = BeAttacker.m_Properties:GetCharID()
	local KillerCamp	= Killer:CppGetBirthCamp()
	local killerScene= Killer.m_Scene
	--if KillerTongId ~= 0 then
	local addExploit = 1
	local data = {}
	--data["uTongID"] = KillerTongId
	data["nCharID"] = KillerPlayerId
	data["nKilledId"] = KilledId
	data["uExploit"] = addExploit
	data["uKillTime"] = os.time()
	data["nEventType"] = 128
	data["MemberID"] = {}
	--local Member = {}
	local ChannelIDList = {}
	table.insert(ChannelIDList, KillerPlayerId)
	if KillerTeamId ~= 0 then
		local team_members = g_TeamMgr:GetMembers(KillerTeamId)
		for m =1,#team_members do
			local member_id = team_members[m]
			if member_id ~= KillerPlayerId then
				local tempPlayer = g_GetPlayerInfo(member_id)
				if tempPlayer and tempPlayer.m_Scene == Killer.m_Scene then
					table.insert(ChannelIDList, tempPlayer.m_AccountID)
					table.insert(data["MemberID"],member_id)
					--table.insert(Member, tempPlayer)
				end
			end
		end
	end
	local function CallBack(succ, succType, tongMember,bNeedNotify, bflag)
		if succ then
			MsgToConn(Killer.m_Conn, 9419, BeAttacker.m_Properties:GetCharName(), addExploit)
			local teamMemNum = 0
			if KillerTeamId ~= 0 then
				local team_members = g_TeamMgr:GetMembers(KillerTeamId)
				teamMemNum = #team_members
				for k = 1, #team_members do
					local member_id = team_members[k]				
					local teamPlayer = g_GetPlayerInfo(member_id)
					if not teamPlayer then
						teamMemNum = teamMemNum - 1
					else
						local memberScene = teamPlayer.m_Scene
						local tongId = teamPlayer.m_Properties:GetTongID()
						if killerScene ~= memberScene  then
							teamMemNum = teamMemNum - 1
						end
						if member_id ~= KillerPlayerId then
							MsgToConnById(member_id, 97, Killer.m_Conn.m_Player.m_Properties:GetCharName(), addExploit)	
						end
					end
				end
			else
				teamMemNum = 1
			end
			if bflag then
				if KillerCamp == ECamp.eCamp_AmyEmpire then
					g_TongBattleCountMgr.m_AmGetExploitCountNum = g_TongBattleCountMgr.m_AmGetExploitCountNum + teamMemNum
				elseif KillerCamp == ECamp.eCamp_WestEmpire then
					g_TongBattleCountMgr.m_SsGetExploitCountNum = g_TongBattleCountMgr.m_SsGetExploitCountNum + teamMemNum
				elseif KillerCamp == ECamp.eCamp_DevilIsland then
					g_TongBattleCountMgr.m_XsGetExploitCountNum = g_TongBattleCountMgr.m_XsGetExploitCountNum + teamMemNum
				end
			end
			CTongBasic.TongTypeChanged(m_Conn,succ,succType,tongMember,bNeedNotify)
		end
	end

	if BeAttacker.m_Conn.m_Account and IsCppBound(BeAttacker.m_Conn.m_Account) then
		CallAccountAutoTrans(BeAttacker.m_Conn.m_Account, "GasTongBasicDB", "UpdateExploit", CallBack, data, unpack(ChannelIDList))
	end
end

function DbCallBack:OnTongLevelUp(tblMember, nLevel)
	if nLevel == 2 then
		for nIndex = 1, #tblMember do
			local nMemberCsId = tblMember[nIndex]
			Gas2GacById:ShowTongMsg(nMemberCsId,10047)
		end
	elseif nLevel == 4 then
		for nIndex = 1, #tblMember do
			local nMemberCsId = tblMember[nIndex]
			Gas2GacById:ShowTongMsg(nMemberCsId,10057)
		end
	end
end

function DbCallBack:OnTongExploitChange(succ, succType, tongMember,bNeedNotify)
	CTongBasic.TongTypeChanged(m_Conn,succ,succType,tongMember,bNeedNotify)
end

function CTongBasic.GetTargetTongMemberInfo(Conn, tongId)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID

	local function CallBack(info, uMaxRequestNum,exploit_info)
		Gas2Gac:ReturnGetTongMemberInfoStart(Conn)
		for i =1 ,info:GetRowNum() do
			local id,pos,level,class,name,bOnline,proffer,proffer_expend,tong_proffer,outLineTime,nowTime = info(i,1),info(i,2),info(i,3),info(i,4),
						info(i,5),info(i,6),info(i,7),info(i,8),info(i,9),info(i,10),info(i,11)
			local exploit = exploit_info[id] or 0
			local online = 1
			if bOnline == 0 then 
				online = 2			--玩家是否在线，1是在线，2不在线
			else
				outLineTime = nowTime
			end
			Gas2Gac:ReturnGetTongMemberInfo(Conn,id,name,pos,level,class,proffer,proffer_expend,tong_proffer,exploit,online,outLineTime)
		end
		Gas2Gac:ReturnGetTongMemberInfoEnd(Conn, uMaxRequestNum)
	end
	local parameters = {}
	parameters["uTargetTongId"]	= tongId
	CallAccountManualTrans(Conn.m_Account, tong_box, "GetTargetTongMemberInfo", CallBack, parameters)
end


--------------------------------------------------------------------------------------------------
function Gas2GasDef:SendUpTongTypeMsg(SerConn, sTongName)
	MsgToConn(g_AllPlayerSender, 9358, sTongName)
end

function CTongBasic.SetCharTongInfo(Conn,server_id,uCharID,tong_id)
	local function CallBack(info)	
		local char_tong_info = info.m_tbl_CharTongInfo
		local Player = g_GetPlayerInfo(uCharID)
		if Player and IsCppBound(Player) then
			--通知视野内玩家
			Player:SetTongID(char_tong_info[1])
			Player.m_Properties:SetTongName(char_tong_info[2])
			Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
			Gas2Gac:UpdateHeadInfoByEntityID(Player.m_Conn, Player:GetEntityID())
			
			--进入,推出佣兵小队,佣兵团信息也需要更新
			if Player.m_uArmyCorpsID ~= info.m_ArmyCorpInfo[1] then
				Player.m_Properties:SetArmyCorpsName(info.m_ArmyCorpInfo[2])
				Player.m_uArmyCorpsID = info.m_ArmyCorpInfo[1]
				Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
				Gas2Gac:UpdateHeadInfoByEntityID(Player.m_Conn, Player:GetEntityID())
			end
		end
		local tong_info = info.m_tblTongInfo
		local TongID = tong_info.m_TongID
		local TongName = tong_info.m_TongName
		local AllMembers = tong_info.m_AllMembers
		if not TongID then return end
		local row = AllMembers:GetRowNum()
		for i = 1, row do
			local member_id = AllMembers(i,1)
			local Member = g_GetPlayerInfo(member_id)
			if Member and IsCppBound(Member) then
				Member:SetTongID(TongID)
				Member.m_Properties:SetTongName(TongName)
			end
		end
	end
	if not g_App:CallDbHalted() then
		local data = {["uCharID"] = uCharID,["server_id"] = server_id,["tong_id"] = tong_id}
		CallDbTrans("GasTongBasicDB","GetTongInfoForInit", CallBack, data)
	end
end
