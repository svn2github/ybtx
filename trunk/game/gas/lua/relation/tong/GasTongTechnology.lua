gac_gas_require "relation/tong/TongTechnologyMgr"

CTongTech = class()
function CTongTech.ResetPlayerTeachProperty(Player,Result)
			if not (Player and IsCppBound(Player)) then return end
			local Conn = Player.m_Conn
			local tblTeachInfo = Result.TeachInfo
			local nTongLevel, nTongLine = Result.TongLevel,Result.TongStationLine
			local row = tblTeachInfo:GetRowNum()
			Gas2Gac:ReturnGetTongFightScienceInfoBegin(Conn, nTongLevel, nTongLine)
			for i =1,row do
				local TechName = tblTeachInfo(i,2)
				local nNowDBLevel = tblTeachInfo(i,3)
--				if nNowDBLevel > 3 then
--					nNowDBLevel = 3 
--				end
				local state,time = tblTeachInfo(i,5),tblTeachInfo(i,6)
				local nMark = state or 0
				local nTime = time or 0
				Gas2Gac:ReturnGetTongFightScienceInfo(Conn,TechName,nNowDBLevel, nMark, nTime)
			end 
			Gas2Gac:ReturnGetTongFightScienceInfoEnd(Conn)
end

function CTongTech.SendTeachPropertyByServer(Conn,TongID)
    local function CallBack(result)
        CTongTech.ResetPlayerTeachProperty(Conn.m_Player, result)
	end
	local parameters = {}
	parameters.uTongID	= TongID
	CallDbTrans("TongTeachDB", "GetTongTeachInfoDB", CallBack, parameters)
end

--【研发到时间】
function CTongTech.GetTongFightScienceInfo(Conn)
	local Player = Conn.m_Player
	if not(Player and IsCppBound(Player)) then return end

	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	
	local function CallBack(TongID)
			CTongTech.SendTeachPropertyByServer(Conn,uTongID)
	end
	
  local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.nTeachType	= 1
	CallAccountManualTrans(Conn.m_Account, "TongTeachDB", "DealWithTeachTimeInfo", CallBack, parameters, uTongID)
end

--【研发】
--RD==Research and Development
function CTongTech.UpdateTongScience(Conn, TechName,TechType)
	local Player = Conn.m_Player
	if not(Player and IsCppBound(Player)) then return end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160041)
		return 
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	local function CallBack(bFlag,Result)
			if (not bFlag) then
				return 
			end
			if IsNumber(bFlag) then
				if Result then
					MsgToConn(Conn, bFlag,Result)
				else
					MsgToConn(Conn, bFlag)
				end
				return 
			end 
			CTongTech.SendTeachPropertyByServer(Conn,uTongID)
	end
  local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.TechName	= TechName
	parameters.TechType	= TechType
	CallAccountManualTrans(Conn.m_Account, "TongTeachDB", "RDTechnologyDB", CallBack, parameters, uTongID)
end

--【取消研发】
function CTongTech.CancelUpdateScience(Conn, name,Type)
  local Player = Conn.m_Player
  if not(Player and IsCppBound(Player)) then return end
  if Player.m_ItemBagLock then
		MsgToConn(Conn,160041)
		return 
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	local function CallBack(result)
			if not result then return end
			if IsNumber(result) then
				MsgToConn(Conn, result)
				return
			end
			local tblRetRes = result.tblRetRes
			local nRes = tblRetRes.m_nRetRes
			local nMoney = tblRetRes.m_nRetMoney

			if IsNumber(nMoney) and nMoney > 0 then
				Gas2Gac:TongMoneyMsgToConn(Conn,9332,nMoney)
			end
			CTongTech.SendTeachPropertyByServer(Conn,result.uTongID)
	end
  local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.TechType	= Type
	parameters.TechName	= name
	CallAccountManualTrans(Conn.m_Account, "TongTeachDB", "CannelRDTeach", CallBack, parameters, uTongID)
end


function CTongTech.InitTongTechInfo(Conn, addOrRemoveTechEff, techInfoRet)
    local player = Conn.m_Player
    if not (player and IsCppBound(player)) then return end    

    local techEffFunc 
    if addOrRemoveTechEff == true then
        techEffFunc = player.AddEquipSuiteEff
    else
        techEffFunc = player.RemoveEquipSuiteEff
    end
    local rowCount =techInfoRet:GetRowNum()
    for i=1, rowCount do
        local techName  = techInfoRet(i, 1)
        local techLevel = techInfoRet(i, 2)
        local TalentName	= g_TongTechMgr:GetFightTechTalentName(techName, techLevel)
		local TanlentPoint	= g_TongTechMgr:GetFightTechTanlentPoint(techName, techLevel)
        techEffFunc(player, TalentName, TanlentPoint)
        Gas2GacById:RetCharLearnedTech(player.m_uID,techName,techLevel)
    end
end

function CTongTech.LearnTongTech(Conn, techName)
    local player = Conn.m_Player
    if not (player and IsCppBound(player)) then return end    
    local charID = Conn.m_Player.m_uID
    local sceneName = Conn.m_Player.m_Scene.m_SceneName
    
    local data= {["CharID"] = charID, ["sSceneName"]=sceneName, ["tongTechName"]=techName}
    local function CallBack(sucFlag, result)
        if sucFlag == false then
            MsgToConn(Conn, result) 
        else
            local curTechLevel = result["CurTechLevel"]
            local preTechLevel = result["CurTechLevel"] - 1
            if preTechLevel > 0 then 
    		    local PreTalentName	= g_TongTechMgr:GetFightTechTalentName(techName, preTechLevel)
			    local PreTanlentPoint	= g_TongTechMgr:GetFightTechTanlentPoint(techName, preTechLevel)
			    player:RemoveEquipSuiteEff(PreTalentName, PreTanlentPoint)
			end
			local TalentName	= g_TongTechMgr:GetFightTechTalentName(techName, curTechLevel)
			local TanlentPoint	= g_TongTechMgr:GetFightTechTanlentPoint(techName, curTechLevel)
			player:AddEquipSuiteEff(TalentName, TanlentPoint)
			local totalSoul = result["totalSoul"]
			Gas2GacById:ReturnModifyPlayerSoulNum(charID, totalSoul)
			Gas2GacById:RetCharLearnedTech(charID, techName,curTechLevel)
			MsgToConn(Conn, 9058) 
        end
    end
    
    CallAccountManualTrans(Conn.m_Account, "TongTeachDB", "LearnTongTech", CallBack, data)
end

