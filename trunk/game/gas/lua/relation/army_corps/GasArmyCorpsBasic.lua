gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gac_gas_require "relation/army_corps/ArmyCorpsMgr"

local ArmyCorps_box = "ArmyCorpsBasicDB"
local textFltMgr = CTextFilterMgr:new()

CArmyCorpsBase = class()

--【申请创建佣兵团】
function CArmyCorpsBase.RequestCreateArmyCorps(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"佣兵团创建处")	then
		return
	end
	--回调函数【高速逻辑】
	local function CallBack(MsgID)
		if IsNumber(MsgID) then
			MsgToConn(Conn,MsgID)
			return
		end
		Gas2Gac:ReturnRequestCreateArmyCorps(Conn)
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "RequestCreateArmyCorpsDB", CallBack, parameters)
end

--【创建佣兵团】
function CArmyCorpsBase.CreateArmyCorps(Conn, name,purpose)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"佣兵团创建处")	then
		return
	end

	if( not textFltMgr:IsValidMsg(name) ) then
		MsgToConn(Conn,8540)
		return
	end
	if( not textFltMgr:IsValidMsg(purpose) ) then
		MsgToConn(Conn,8541)
		return
	end
	--去掉前后空格
	local sArmyCorpsName = textFltMgr:RemoveTab1(name)
	local sArmyCorpsPorpose = textFltMgr:RemoveTab1(purpose)
	if string.len(sArmyCorpsName) == 0 then
		--"名称不能为空"
		MsgToConn(Conn,9176 )
		return
	end
	if string.len(sArmyCorpsName) > 32 then
		--"名称长度不能大于32"
		MsgToConn(Conn,9177)
		return
	end
	if string.len(sArmyCorpsPorpose) > 1024 then
		--"宗旨长度不能大于512"
		MsgToConn(Conn,9178)
		return
	end
	local uTongID = Player.m_Properties:GetTongID()
	
	local function CallBack(bFlag,uMsgId,regist_money,uArmyCorpsID)
		if not Player then return end
		MsgToConn(Conn,uMsgId)
		if bFlag then
			CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(uTongID,uArmyCorpsID,sArmyCorpsName)
			Gas2Gac:ReturnCreateArmyCorps(Conn)
			Gas2Gac:OnArmyCorpsPosChanged(Conn ,g_ArmyCorpsMgr:GetPosIndexByName("团长"))
		end
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.sArmyCorpsName	= sArmyCorpsName
	parameters.sArmyCorpsPorpose	= sArmyCorpsPorpose
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "CreateArmyCorpsDB", CallBack, parameters, uTongID)
end

function CArmyCorpsBase.GetArmyCorpsInfo(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	--回调函数【高速逻辑】
	local function CallBack(bFlag, info)
		if bFlag and next(info) then
			Gas2Gac:ReturnArmyCorpsInfo( Conn, info.Name,info.Level,info.Money,info.Purpose,
				info.AdminName,info.MemberCount,info.Exploit,info.Honor )
		end
	end
	
	local data = {}
	data["uPlayerID"]	= uPlayerID
	CallAccountAutoTrans(Conn.m_Account, ArmyCorps_box, "GetArmyCorpsInfoDB", CallBack, data)
end

function CArmyCorpsBase.ChangeArmyCorpsPurpose(Conn, sPurpose)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	if( not textFltMgr:IsValidMsg(sPurpose) ) then
		MsgToConn(Conn,8541)
		return
	end
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgId)
		if bFlag then
			Gas2Gac:OnArmyCorpsPurposeChanged( Conn, sPurpose )
		else
			MsgToConn(Conn,uMsgId)
		end
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["sPurpose"]	= sPurpose
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "ChangeArmyCorpsPurposeDB", CallBack, parameters, Player.m_uArmyCorpsID)
end

function CArmyCorpsBase.InviteJoinArmyCorps(Conn, uTargetId)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgId, sArmyCorpsName)
		if bFlag then
			Gas2GacById:OnInviteJoinArmyCorps( uTargetId, uPlayerID, Player.m_Properties:GetCharName(), sArmyCorpsName )
		end
		MsgToConn(Conn,uMsgId)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["uTargetID"]	= uTargetId
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "CanInviteJoinArmyCorpsDB", CallBack, parameters)
end

function CArmyCorpsBase.PreChangeArmyCorpsName(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(result)
		if IsNumber(result) then
			MsgToConn(Conn,result)
			return
		else
			Gas2Gac:ReturnPreChangeArmyCorpsName(Conn)
		end
	end
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "PreChangeArmyCorpsNameDB", CallBack, parameters)
end

function CArmyCorpsBase.ChangeArmyCorpsName(Conn, name)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	--去掉前后空格
	if( not textFltMgr:IsValidMsg(name) ) then
		MsgToConn(Conn,8540)
		return
	end
	local sArmyCorpsName = textFltMgr:RemoveTab1(name)

	--回调函数【高速逻辑】
	local function CallBack(result,uMsgId,tong_id,uArmyCorpsId)
		MsgToConn(Conn,uMsgId)
		if not result then
			return
		else
			CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(tong_id,uArmyCorpsId,sArmyCorpsName)
		end
	end
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["sName"]	= sArmyCorpsName
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "ChangeArmyCorpsNameDB", CallBack, parameters)
end


function CArmyCorpsBase.InviteJoinArmyCorpsByName(Conn, sTargetName)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	--回调函数【高速逻辑】
	local function CallBack(bFlag,uMsgId, sArmyCorpsName, uTargetId)
		if bFlag then
			Gas2GacById:OnInviteJoinArmyCorps( uTargetId, uPlayerID, Player.m_Properties:GetCharName(), sArmyCorpsName )
		end
		MsgToConn(Conn,uMsgId)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["sTargetName"]	= sTargetName
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "CanInviteJoinArmyCorpsByNameDB", CallBack, parameters)
end

function CArmyCorpsBase.ResponseBeInviteToArmyCorps(Conn, InviteID, bResponse)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	if not bResponse then
		MsgToConnById(InviteID, 8515, Player.m_Properties:GetCharName())
		return
	end
	local function CallBack(uArmyCorpsID)
			--回调函数【高速逻辑
			if uArmyCorpsID == 0 then
				MsgToConnById(InviteID, 8509)
			end
			local function AddCallBack(bFlag,uMsgId,uArmyCorpsId,info, uBeInviteTongID)
				if not bFlag then
					MsgToConnById(InviteID, uMsgId)
					return
				end
				if bFlag and next(info) then--ReseiveResponseBeInviteToArmyCorps
					Gas2Gac:ReturnArmyCorpsInfo( Conn, info.Name,info.Level,info.Money,info.Purpose,
						info.AdminName,info.MemberCount,info.Exploit,info.Honor )
					CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(uBeInviteTongID,uArmyCorpsId,info.Name)
				end
			end
			
			local parameters = {}
			parameters["uPlayerID"]	= InviteID
			parameters["uTargetID"]	= uPlayerID
			local uTongID = Player.m_Properties:GetTongID()
			CallAccountAutoTrans(Conn.m_Account, ArmyCorps_box, "AddTongToArmyCorpsDB", AddCallBack, parameters, uArmyCorpsID, uTongID)
	end
	local data = {}
	data["uTargetID"] = InviteID
	CallDbTrans(ArmyCorps_box, "GetArmyCorpsId", CallBack, data)
end

function CArmyCorpsBase.GetArmyCorpsTeamInfo(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(bFlag,MemberCount,MemberCountLimit,TeamInfoTbl)
		Gas2Gac:ReturnArmyCorpsTeamInfoBegin(Conn,MemberCount,MemberCountLimit)
		if bFlag then
			for i=1,#TeamInfoTbl do
				Gas2Gac:ReturnArmyCorpsTeamInfo(Conn,TeamInfoTbl[i].tong_type,TeamInfoTbl[i].tong_name,
				TeamInfoTbl[i].leader_name,TeamInfoTbl[i].tong_level,TeamInfoTbl[i].member_num,
				TeamInfoTbl[i].tong_exploit,TeamInfoTbl[i].tong_id,TeamInfoTbl[i].warZoneId, TeamInfoTbl[i].stationId)
			end
		end
		Gas2Gac:ReturnArmyCorpsTeamInfoEnd(Conn)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	CallAccountAutoTrans(Conn.m_Account, ArmyCorps_box, "GetArmyCorpsTeamInfoDB", CallBack, parameters)
end

function CArmyCorpsBase.LeaveArmyCorps(Conn)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(bFlag,uMsgId,uTongId)
		if bFlag then
			Gas2Gac:OnLeaveArmyCorps(Conn,true)
			CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(uTongId,0,"")
		else
			MsgToConn(Conn, uMsgId)
		end
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "LeaveArmyCorpsDB", CallBack, parameters, Player.m_uArmyCorpsID)
end

function CArmyCorpsBase.KickOutOfArmyCorps(Conn, tongId)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	local function CallBack(bFlag,uMsgId,BeKickOutTongMem)
		if bFlag then
			Gas2Gac:OnKickOutOfArmyCorps(Conn,true)
			for _, charId in pairs(BeKickOutTongMem) do 
				Gas2GacById:OnBeKickOutOfArmyCorps(charId)
			end
			CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(tongId,0,"")
		else
			MsgToConn(Conn, uMsgId)
		end
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["uTongId"]		= tongId
	CallAccountManualTrans(Conn.m_Account, ArmyCorps_box, "KickOutOfArmyCorpsDB", CallBack, parameters, Player.m_uArmyCorpsID)
end

function CArmyCorpsBase.DealArmyCorpsPosWhenDelChar(uArmyCorpsNewAdminId, uTongNewLeaderId, uTong_Id, MemberTong)
	if uArmyCorpsNewAdminId > 0 then
		Gas2GacById:OnArmyCorpsPosChanged(uArmyCorpsNewAdminId ,g_ArmyCorpsMgr:GetPosIndexByName("团长"))
	end
	if uArmyCorpsNewAdminId == 0 and MemberTong ~= nil then
		for i,v in pairs(MemberTong) do
			CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(v,0,"")
		end
	end
	if uTongNewLeaderId ~= 0 then
		if uTongNewLeaderId then
			Gas2GacById:ReturnPosChanged(uTongNewLeaderId,g_TongMgr:GetPosIndexByName("团长"))
		end
	end
end

function CArmyCorpsBase.ChangeArmyCorpsPos(Conn,uTargetId,uChangeToPos)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	
	if uPlayerID == uTargetId then return end
	
	local function CallBack(bFlag,uMsgId)
		if bFlag then
			if uChangeToPos == g_ArmyCorpsMgr:GetPosIndexByName("团长") then
				Gas2Gac:OnArmyCorpsPosChanged(Conn ,g_ArmyCorpsMgr:GetPosIndexByName("团员"))
			end
			Gas2GacById:OnArmyCorpsPosChanged(uTargetId ,uChangeToPos)
		else
			MsgToConn(Conn, uMsgId)
		end
	end
	
	local parameters = {}
	parameters["uPlayerID"]		 = uPlayerID
	parameters["uTargetId"]		 = uTargetId
	parameters["uChangeToPos"] = uChangeToPos
	CallAccountAutoTrans(Conn.m_Account, ArmyCorps_box, "ChangeArmyCorpsPosDB", CallBack, parameters, Player.m_ArmyCorpsID)
end



----------------------------------------多服间广播消息--------------------------------------
function CArmyCorpsBase.InitArmyCorpsInfoOnMemChange(uTongId,uArmyCorpsID,sArmyCorpsName)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:SetArmyCorpsInfoOnLeave(GetServerAutoConn(server_id),uTongId,uArmyCorpsID,sArmyCorpsName)
	end
end

function CArmyCorpsBase.SetArmyCorpsInfoOnLeave(Conn, uTongId, uArmyCorpsID, sArmyCorpsName)
	local tblMembers = g_GasTongMgr:GetMembers(uTongId)
	for i = 1, #(tblMembers) do
		local MemberId = tblMembers[i]
		if uArmyCorpsID == 0 then
			g_GasArmyCorpsMgr:DelArmyCorpsMember(MemberId)
		else
			g_GasArmyCorpsMgr:AddArmyCorpsMember(MemberId,uArmyCorpsID,sArmyCorpsName)
		end
	end
end
---------------------------------------------------------------
--function Gas2GasDef:SetCharArmyCorpsInfo(Conn,uArmyCorpsID,sArmyCorpsName)
--	local function CallBack()
--		for i=1,ArmyCorpsMemRet:GetRowNum() do
--			local uTongId = ArmyCorpsMemRet:GetNumber(i,0)
--			local tblMembers = g_GasTongMgr:GetMembers(uTongId)
--			for i = 1, #(tblMembers) do
--				local MemberId = tblMembers[i]
--				local pMember = g_GetPlayerInfo(MemberId)
--				if IsCppBound(pMember) then		
--					--pMember:SetArmyCorpsName(sArmyCorpsName)
--					pMember.m_uArmyCorpsID = uArmyCorpsID
--					Gas2Gac:UpdateHeadInfoByEntityID(pMember:GetIS(0), pMember:GetEntityID())
--					Gas2Gac:UpdateHeadInfoByEntityID(pMember.m_Conn, pMember:GetEntityID())
--				end
--			end
--		end
--	end
--	if not g_App:CallDbHalted() then
--		local parameters = {["uArmyCorpsId"] = uArmyCorpsID}
--		CallDbTrans("ArmyCorpsBasicDB","GetArmyCorpsMemberDB", CallBack, parameters)
--	end
--end
-------------------------------------------------------------
