function Gac2Gas:EquipSuperadd(Conn, itemType, itemName, itemID)
    if not IsCppBound(Conn.m_Player) then
        return 
    end

    local charID = Conn.m_Player.m_uID
    local data = {["CharID"] = charID, ["EquipType"] = itemType,
    ["EquipName"] = itemName, ["EquipID"] = itemID}
    
    local function CallBack(suc, result)
        if suc == false then
            MsgToConn(Conn, result)
        else
            local equipPart = result["EquipPart"]
            local curSuperaddPhase = result["CurSuperaddPhase"]
            local maxSuperaddPhase = result["MaxSuperaddPhase"]
            local oldSuperaddRate = result["OldSuperaddRate"]
            local MsgId = result["MsgID"]
            local superaddRate = result["SuperaddRate"]
            if equipPart > 0 and oldSuperaddRate ~= superaddRate  then
                local equipInfo = result["EquipInfo"]
                	local oldequip  = g_GetEquipMgr():GetEquipFromRet(itemID, Conn.m_Player.m_uID, itemType, itemName, equipInfo)
	                local newequip  = g_GetEquipMgr():GetEquipFromRet(itemID, Conn.m_Player.m_uID, itemType, itemName, equipInfo)
                    oldequip.m_SuperaddRate = oldSuperaddRate 
                    oldequip:DisablePropty(Conn.m_Player, equipPart)
                    newequip.m_SuperaddRate = superaddRate
		            newequip:EnablePropty(Conn.m_Player, equipPart)
		            CGasFightingEvaluation.UpdateFightingEvaluationInfo(Conn.m_Player)
            end
            MsgToConn(Conn, MsgId)
            local equipID = result["EquipID"]
            Gas2Gac:RetEquipSuperaddRate(Conn, equipID, curSuperaddPhase, superaddRate)
            local soulNum = result["total_soul"]
            Gas2GacById:ReturnModifyPlayerSoulNum(charID, soulNum)
            local superaddRate = result["SuperaddRate"]
            Gas2Gac:RetEquipSuperaddSuc(Conn)
        end
    end
    CallAccountManualTrans(Conn.m_Account, "EquipSuperaddDB", "EquipSuperadd", CallBack, data)
end