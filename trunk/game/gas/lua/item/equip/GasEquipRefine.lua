

function Gac2Gas:EquipRefine(Conn, equipID, equipType, equipName, refineStoneType, refineStoneName)
    local charID = Conn.m_Player.m_uID
    local data  = {["CharID"] = charID, 
    ["EquipID"]= equipID, ["EquipType"]=equipType, ["EquipName"] = equipName,
    ["StoneType"] = refineStoneType, ["StoneName"] = refineStoneName,
    ["SceneName"] = Conn.m_Player.m_Scene.m_SceneName}
    
    local function CallBack(suc, result)
        if suc == false then
            local msgID = result
            MsgToConn(Conn, msgID) 
        else
	        Gas2Gac:RetEquipRefine(Conn, true)
	        MsgToConn(Conn, 188004) 
        end
    end
    
    CallAccountManualTrans(Conn.m_Account, "EquipRefineDB", "EquipRefine", CallBack, data)	
end