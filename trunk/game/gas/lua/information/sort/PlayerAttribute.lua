gas_require "information/sort/PlayerAttributeInc"

local EquipMgrDB = "EquipMgrDB"

---------------------------目标属性面板-----------------------------------

function Gac2Gas:RequestAimInfo(Conn,CharID)
	local data = {["nCharID"] = CharID }
	local Target = g_GetPlayerInfo(CharID)
	if not IsCppBound(Target) then
		return 
	end
	
	local function CallBack(Data)
		local AllInfo = Data[1]
		for y=1, table.maxn(AllInfo) do
			local nBigID,nIndex,nItemID,eEquipPart,nItemBindingType, EquipRet = AllInfo[y]["nBigID"],AllInfo[y]["nIndex"],AllInfo[y]["nItemID"],AllInfo[y]["eEquipPart"],AllInfo[y]["nItemBindingType"],AllInfo[y]["EquipRet"]
			local equip_mgr = g_GetEquipMgr()
			local equip = equip_mgr:GetEquipFromRet(nItemID,CharID,nBigID,nIndex,EquipRet)
			
			equip:SendEquipInfoUsingConn(Conn)
			if AllInfo[y]["ItemMaker"] then
				Gas2Gac:RetItemMakerName(Conn,AllInfo[y]["ItemMaker"] ,nItemID)
			end
			Gas2Gac:RetAimEquipInfo(Conn,nBigID,nIndex,nItemID,eEquipPart,nItemBindingType)
			
		end
		local suitCountDBTbl,uDPS,uIntensifyPoint = unpack(Data[2])
		CGasFightingEvaluation.GetAimFightingEvaluation(Conn,CharID,suitCountDBTbl,uDPS,uIntensifyPoint)
		Gas2Gac:RetAimInfoEnd(Conn,CharID)	
	end	
	
	CallAccountManualTrans(Conn.m_Account, EquipMgrDB, "GetAimPlayerInfo", CallBack, data)
end