
function Gas2GasDef:PublicChatSendMsgDef(Conn, group_id,senderName,message)
	CGasFriendGroupBasic.PublicChatSendMsgDef(Conn, group_id,senderName,message)
end

function Gas2GasDef:SendPlayerChannelMsg(Conn, Character_Id, charName, uChannel_Id,sMsg, uProcess)
	CChatChannelMgr.SendPlayerChannelMsg(Conn, Character_Id, charName, uChannel_Id,sMsg, uProcess)
end

function Gas2GasDef:SendNpcChannelMsg(Conn, NpcName, uChannel_Id, sMsg, uProcess)
	CChatChannelMgr.SendNpcChannelMsg(Conn, NpcName, uChannel_Id, sMsg, uProcess)
end

function Gas2GasDef:SendArmyChannelMsg(Conn, NpcName,Character_Id, charName,uTongID,uChannel_Id,sMsg)
	CChatChannelMgr.SendArmyChannelMsg(Conn, NpcName,Character_Id, charName,uTongID,uChannel_Id,sMsg)
end


function Gas2GasDef:SendTongChannelMsg(Conn, NpcName,Character_Id, charName,uTongID,uChannel_Id,sMsg)
	CChatChannelMgr.SendTongChannelMsg(Conn, NpcName,Character_Id, charName,uTongID,uChannel_Id,sMsg)
end

function Gas2GasDef:SendIBPayInfo(Conn)
	CExchangeMoney.SendIBPayInfo(Conn)
end

function Gas2GasDef:SendMoneyExchangeInfo(Conn)
	CExchangeMoney.SendMoneyExchangeInfo(Conn)
end

function Gas2GasDef:SendEratingAuditTick(Conn)
	CExchangeMoney.SendEratingAuditTick(Conn)
end

function Gas2GasDef:CheckAreaFbState(Conn, PlayerID, ServerId, SceneId, SceneName)
	CAreaFbServer.CheckAreaFbState(Conn, PlayerID, ServerId, SceneId, SceneName)
end

function Gas2GasDef:RetAreaFbState(Conn, PlayerID, SceneName, SceneId, ServerId, IsShowInfoWnd)
	CAreaFbServer.RetAreaFbState(Conn, PlayerID, SceneName, SceneId, ServerId, IsShowInfoWnd)
end

function Gas2GasDef:SendResistanceValueToGac(Conn, SceneName, TeamID, PlayerID)
	CAreaFbServer.SendResistanceValueToGac(Conn, SceneName, TeamID, PlayerID)
end

function Gas2GasDef:SendPkValueRollMsg(SerConn, sName, pkValue)
	MsgToConn(g_AllPlayerSender, 193038, sName, pkValue)
end

function Gas2GasDef:SetCharTongInfo(Conn,server_id,uCharID,tong_id)
	CTongBasic.SetCharTongInfo(Conn,server_id,uCharID,tong_id)
end

function Gas2GasDef:SendUpTongTypeMsg(SerConn, sTongName)
	MsgToConn(g_AllPlayerSender, 9358, sTongName)
end

function Gas2GasDef:AddTongMember(Conn,server_id,uPlayerID,uTongID,sTongName,ArmyCorpsID,ArmyCorpsName)
	g_GasTongMgr:AddTongMember(uPlayerID,uTongID,sTongName)
	g_GasArmyCorpsMgr:AddArmyCorpsMember(uPlayerID,ArmyCorpsID,ArmyCorpsName)
end

function Gas2GasDef:DelTongMember(Conn,server_id,uPlayerID,uTongID)
	g_GasTongMgr:DelTongMember(uPlayerID,uTongID)
	g_GasArmyCorpsMgr:DelArmyCorpsMember(uPlayerID)
end

function Gas2GasDef:InitTeamInfo(Conn,TeamID,server_id,sType)
	CGasTeam.InitTeamInfo(Conn,TeamID,server_id,sType)
end

function Gas2GasDef:SendTeamMemberHPMP(Conn,TeamID,nCharID,nCurHP, nMaxHP, nCurMP, nMaxMP)
	 Gas2Gac:ReturnUpdateHPMPByCharID(g_TeamMgr:GetTeamMsgSender(TeamID), nCharID, nCurHP, nMaxHP, nCurMP, nMaxMP)
end

function Gas2GasDef:ResetTeamMemberLevel(Conn,player_id)
	CGasTeam.ResetTeamMemberLevel(Conn,player_id)
end

function Gas2GasDef:SendTeachPropertyInfo(Conn,TongID)
		CTongTech.SendTeachPropertyByServer(Conn,TongID)
end

function Gas2GasDef:SetArmyCorpsInfoOnLeave(Conn, uTongId, uArmyCorpsID, sArmyCorpsName)
	CArmyCorpsBase.SetArmyCorpsInfoOnLeave(Conn, uTongId, uArmyCorpsID, sArmyCorpsName)
end

function Gas2GasDef:UpdateCampExploit(SerConn)
	local function CallBack(bFlag, data)
		if bFlag then
			g_TongBattleCountMgr.m_AmGetExploitCountNum = g_TongBattleCountMgr.m_AmGetExploitCountNum - data["nAmGetExploit"]
			g_TongBattleCountMgr.m_SsGetExploitCountNum = g_TongBattleCountMgr.m_SsGetExploitCountNum - data["nSsGetExploit"]
			g_TongBattleCountMgr.m_XsGetExploitCountNum = g_TongBattleCountMgr.m_XsGetExploitCountNum - data["nXsGetExploit"]
		end
	end
	if g_WarZoneMgr.m_Warring then
		local data = {}
		data["nAmGetExploit"] = g_TongBattleCountMgr.m_AmGetExploitCountNum
		data["nSsGetExploit"] = g_TongBattleCountMgr.m_SsGetExploitCountNum
		data["nXsGetExploit"] = g_TongBattleCountMgr.m_XsGetExploitCountNum	
		CallDbTrans("GasTongBasicDB", "UpdateCampExploitDB", CallBack, data,88888)
	end
end

function Gas2GasDef:SetAllServerWarTime(Conn, time, wday, length)
	g_WarZoneMgr:SetWarTime(time, wday, length)
end

function Gas2GasDef:StartAllServerWar(Conn, length)
	g_WarZoneMgr:StartWar(length)
end

function Gas2GasDef:StopAllServerWar(Conn)
	g_WarZoneMgr:StopWar()
end

function Gas2GasDef:KickOutOfCave(Conn, PlayerId)
	CDragonCave:KickOutOfCave(PlayerId)
end

function Gas2GasDef:LeaveOutOfCave(Conn, SceneId)
	CDragonCave:LeaveOutOfCave(SceneId)
end


function Gas2GasDef:DestroyDragon(Conn, TongId, BuildingId)
	CDragonCave:DestroyDragon(TongId, BuildingId)
end

function Gas2GasDef:InitNewPanel(Conn,PlayerId)
	InitNewTongMemberPanel(PlayerId)
end
