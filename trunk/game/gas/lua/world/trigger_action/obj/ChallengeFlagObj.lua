gas_require "framework/main_frame/SandBoxDef"
cfg_load "appellation/DuelContWinAppellation_Common"

local DuelContWinAppellation_Common = DuelContWinAppellation_Common
local EFighterCtrlState = EFighterCtrlState
local g_GetDistance = g_GetDistance
local IsCppBound = IsCppBound
local MsgToConn = MsgToConn
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, Obj, ObjName, GlobalID)

	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local Player = Conn.m_Player
	local Creator = Obj.m_Creator
	if not IsCppBound(Creator) then
		return
	end
	
	if Creator == Player then
		return
	else
		if g_GetDistance(Obj, Player) > 6 then 
			MsgToConn(Conn, 7)
			return
		end
		if Creator:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) then
			MsgToConn(Conn, 193509, Creator.m_Properties:GetCharName())
			return
		end
		if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InDuel) then
			MsgToConn(Conn, 193510)
			return
		end
		if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
			MsgToConn(Conn, 193516)
			return
		end
		if Creator:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
			MsgToConn(Conn, 193517)
			return
		end
		if IsCppBound(Obj.m_Challenger) then
			MsgToConn(Conn, 193508, Creator.m_Properties:GetCharName(), Obj.m_Challenger.m_Properties:GetCharName())
			return
		end
		if IsCppBound(Player.m_ChallengeObj) then
			MsgToConn(Conn, 193511)
			return
		end
		
		Obj.m_AddCount = true
		local charLevel = Creator:CppGetLevel()
		local ChallengeLevel = Player:CppGetLevel()
		local minLevelKeys = DuelContWinAppellation_Common:GetKeys()
		local min, max = 0
		for _, minLevel in pairs(minLevelKeys) do
			local maxLevelKeys = DuelContWinAppellation_Common(minLevel):GetKeys()
			for _, maxLevel in pairs(maxLevelKeys) do
				if charLevel >= tonumber(minLevel) and charLevel <= tonumber(maxLevel) then
					if ChallengeLevel < tonumber(minLevel) then
						Obj.m_AddCount = false
						min = tonumber(minLevel)
						max = tonumber(maxLevel)
						break;
					elseif ChallengeLevel > tonumber(maxLevel) then
						Obj.m_AddCount = true
						min = tonumber(minLevel)
						max = tonumber(maxLevel)
						break;
--						MsgToConn(Conn, 193521, tonumber(minLevel), tonumber(maxLevel))
--						return
					end
				end
			end
		end
		if min == 0 and max == 0 then
			Gas2Gac:ShowChallengeMessage(Conn, GlobalID, Creator.m_Properties:GetCharName())
		else
			Gas2Gac:ShowChallengeAdvertMessage(Conn, GlobalID, Creator.m_Properties:GetCharName(), charLevel, min, max)
		end
	end
end

return Entry
