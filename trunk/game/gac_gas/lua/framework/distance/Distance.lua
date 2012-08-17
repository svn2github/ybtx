cfg_load "npc/Npc_Common"

function g_GetDistance(initiatice, passive)
	local pos1 = CPos:new()
	local pos2 = CPos:new()
	initiatice:GetGridPos( pos1 )
	passive:GetGridPos( pos2 )
	return initiatice:GetEntityDistInGrid(passive)
	--return math.max( math.abs( pos1.x - pos2.x ), math.abs( pos1.y - pos2.y ) )
end

function g_GetDistanceByPos(Target, pos)
	if not IsCppBound(Target) then
		return 0
	end
	local pos1 = CPos:new()
	Target:GetGridPos( pos1 )
	return math.max( math.abs( pos1.x - pos.x ), math.abs( pos1.y - pos.y ) )
end

function g_DynamicPathPos(Target, pos)
	if not IsCppBound(Target) then
		return 0
	end
	local pos1 = CPos:new()
	Target:GetGridPos( pos1 )
	local xPos = math.abs( pos1.x - pos.x )
	local yPos = math.abs( pos1.y - pos.y )
	local margin = math.sqrt(xPos * xPos + yPos * yPos)
	return margin
end


function GetReachDist(BeginPos, EndPos, fReachDist)
	--CPos DistVec = DesPos - SrcPos;
	local x_dist = EndPos.x - BeginPos.x
	local y_dist = EndPos.y - BeginPos.y
	local fDist = math.sqrt((x_dist * x_dist + y_dist * y_dist));
	local fRate = fReachDist / fDist;

	local pos = CFPos:new()
	pos.x = EndPos.x - x_dist * fRate;
	pos.y = EndPos.y - y_dist * fRate;
	return pos
end

--判断玩家和npc的距离
function DistanceJudgeBetweenNpcAndPlayer(Conn, sFuncName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return false
	end
	if Conn.LockingNpcID == nil then
		MsgToConn(Conn, 14)
		return false
	end
	local NPC = CEntityServerManager_GetEntityByID(Conn.LockingNpcID)
	if NPC == nil then
		MsgToConn(Conn, 14)
		return false
	end
	
	local nPlayerTongID = Player.m_Properties:GetTongID()
	if NPC.m_BuildingId and NPC.m_TongID ~= nPlayerTongID then
		MsgToConn(Conn, 7)
		return false
	end
	
	local NpcName = NPC.m_Properties:GetCharName()
	if Npc_Common(NpcName) == nil then
		-- "必须在相关npc旁边才可进行此操作"
		MsgToConn(Conn, 7)
		return false
	end
	local funcnametbl = Npc_Common(NpcName,"FuncName")
	if funcnametbl ~= "" then
		funcnametbl = loadstring("return {"..funcnametbl.."}")()
		for i = 1, #(funcnametbl) do
			if funcnametbl[i] == sFuncName then
				if g_GetDistance(NPC, Player) > 20 then
					-- "必须在相关npc旁边才可进行此操作"
					MsgToConn(Conn, 7)
					return false
				end
				break
			end
		end
--		if Npc_Common(NpcName, "FuncName") == FuncName then
--			if g_GetDistance(NPC, Player) > 20 then
--				-- "必须在相关npc旁边才可进行此操作"
--				MsgToConn(Conn, 7)
--				return false
--			end
--		else
--			MsgToConn(Conn, 7)
--			return false
--		end
	else
		return false
	end
	return true
end
