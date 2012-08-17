lan_load "appellation/Lan_AppellationText_Common"
local GasAppellationAndMoralDB = "GasAppellationAndMoralDB"
CGasAppellationAndMoral = class()

--@brief 保存玩家称谓
--@param appellationIndex:称谓内容
function CGasAppellationAndMoral.SavePlayerAppellation(Conn,appellationId1,appellationId2)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	
	local data = {
			["char_id"] = Player.m_uID,
			["appellationId1"] = appellationId1,
			["appellationId2"] = appellationId2
			}
	local function callback(succ)
		if succ and IsCppBound(Player) then
			Player.m_Properties:SetCharAppellation1(appellationId1)
			Player.m_Properties:SetCharAppellation2(appellationId2)
			Gas2Gac:ReturnPlayerAppellationInfo(Player.m_Conn)
			--通知视野内玩家
			Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
		end
	end		
	CallAccountManualTrans(Conn.m_Account, GasAppellationAndMoralDB, "SavePlayerAppellation", callback, data)
end

--@brief 保存玩家新增称谓
--@param AppellationText:称谓内容
function CGasAppellationAndMoral.AddNewAppellation(Player, AppellationText)
	local data = {
			["char_id"] = Player.m_uID,
			["AppellationText"] = AppellationText
			}
	
	local function callback(succ,appellationId,isUsed)
		if succ then
			if IsCppBound(Player) then
				Gas2Gac:ReturnAddNewAppellation(Player.m_Conn,appellationId)
				AppellationText = Lan_AppellationText_Common(MemH64(AppellationText),"Content")
				MsgToConn(Player.m_Conn,37,AppellationText)
			end
		end
	end	
	CallAccountAutoTrans(Player.m_Conn.m_Account, GasAppellationAndMoralDB, "AddNewAppellation", callback, data)
end
