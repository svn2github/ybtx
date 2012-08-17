CTongContriMoney = class()
-----------------------------------------RPCº¯Êý---------------------------------------------------------
--¡¾¾èÏ×½ðÇ®¡¿
function CTongContriMoney.ContributeMoney(Conn,nNum)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160034)
		return 
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if nNum <= 0 then
		return
	end
	
	local function CallBack(bFlag,uMsgID)
			if (not bFlag) then
				if IsNumber(uMsgID) then
					MsgToConn(Conn, uMsgID)
				end
			else
				MsgToConn(Conn, 9158)
				local AddProffer = nNum*0.1
				if AddProffer > 0 then
					MsgToConn(Conn, 9353,AddProffer)
				end
			end
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	parameters["nNum"]	= nNum
	CallAccountManualTrans(Conn.m_Account, "GasTongContributeMoneyDB", "ContributeMoneyDB", CallBack, parameters, uTongID)
end

function CTongContriMoney.GetMoneyCanContribute(Conn)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160034)
		return 
	end
	local uPlayerID = Player.m_uID
	
	local function CallBack(total_money,have_money)
		Gas2Gac:ReturnGetMoneyCanContribute(Conn,total_money,have_money)
	end
	
	local parameters = {}
	parameters["uPlayerID"]	= uPlayerID
	CallAccountManualTrans(Conn.m_Account, "GasTongContributeMoneyDB", "GetMoneyCanContribute", CallBack, parameters)
end

function CTongContriMoney.ClearContributeMoneyInfoTick()
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans("GasTongContributeMoneyDB","ClearContributeMoneyInfo", nil,{})
		end
	end
	g_AlarmClock:AddTask("ClearContributeMoneyInfo", {wday = {1,2,3,4,5,6,7},time = {"00:00"}}, CallBack)
end



function CTongContriMoney.GetTongContributeMoneyInfo(Conn, eIndex)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		return
	end
	local function CallBack(max_money,sum_money)
		Gas2Gac:ReturnTongContributeMoneyInfo(Conn,eIndex,max_money,sum_money)
	end
	local parameters = {}
	parameters["uPlayerID"] = uPlayerID
	parameters["eIndex"] = eIndex
	CallAccountManualTrans(Conn.m_Account, "GasTongContributeMoneyDB", "GetTongContributeMoneyInfo", CallBack, parameters, uTongID)
end

function CTongContriMoney.ContributeMoneyChangeExp(Conn, nMoney)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		return
	end
	local function CallBack(bSucc, uMsgID, curLevel, levelExp, addExp, max_money, sum_money)
		if not bSucc then
			MsgToConn(Conn,uMsgID)
		end
		if bSucc then
			if curLevel then
				local AddExpTbl = {}
				AddExpTbl["Level"] = curLevel
				AddExpTbl["Exp"] = levelExp
				AddExpTbl["AddExp"] = addExp
				AddExpTbl["uInspirationExp"] = 0
				CRoleQuest.AddPlayerExp_DB_Ret(Player, AddExpTbl)
				MsgToConn(Conn,uMsgID,addExp)
				MsgToConn(Conn,2047,addExp)
			else
				MsgToConn(Conn,uMsgID)
			end
			Gas2Gac:ReturnContributeMoney(Conn)
		end
	end
	OnSavePlayerExpFunc({Player})
	local parameters = {}
	parameters["uPlayerID"] = uPlayerID
	parameters["uMoney"] = nMoney
	CallAccountManualTrans(Conn.m_Account, "GasTongContributeMoneyDB", "ContributeMoneyChangeExp", CallBack, parameters, uPlayerID,uTongID)
end

function CTongContriMoney.ContributeMoneyChangeSoul(Conn, nMoney)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		return
	end
	local function CallBack(bSucc, uMsgID, total_soul, uAddSoulValue, max_money, sum_money)
		if not bSucc then
			MsgToConn(Conn,uMsgID)
		else
			MsgToConn(Conn,uMsgID,uAddSoulValue)
			MsgToConn(Conn,9389,uAddSoulValue)
			Gas2Gac:ReturnModifyPlayerSoulNum(Conn,total_soul)
			Gas2Gac:ReturnContributeMoney(Conn)
		end
	end
	local parameters = {}
	parameters["uPlayerID"] = uPlayerID
	parameters["uMoney"] = nMoney
	CallAccountManualTrans(Conn.m_Account, "GasTongContributeMoneyDB", "ContributeMoneyChangeSoul", CallBack, parameters, uPlayerID,uTongID)
end

function CTongContriMoney.ContributeMoneyChangeResource(Conn, nMoney)
	local Player = Conn.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		return
	end
	local function CallBack(bSucc, uMsgID, uAddResourceValue, max_money, sum_money)
		if not bSucc then
			MsgToConn(Conn,uMsgID)
		else
			MsgToConn(Conn,uMsgID,uAddResourceValue)
			MsgToConn(Conn,9390,uAddResourceValue)
			Gas2Gac:ReturnContributeMoney(Conn)
		end
	end
	local parameters = {}
	parameters["uPlayerID"] = uPlayerID
	parameters["uMoney"] = nMoney
	CallAccountManualTrans(Conn.m_Account, "GasTongContributeMoneyDB", "ContributeMoneyChangeResource", CallBack, parameters, uPlayerID,uTongID)
end