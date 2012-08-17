gas_require "world/tong_area/WarZoneMgr"

local function Script(Arg, Trigger, Player)
	if not IsCppBound(Player) then
		return
	end
	assert(Trigger.m_StationIndex and Trigger.m_WarZoneId, "占领Npc属性不全")
	g_WarZoneMgr:OccupyStation(Player.m_Conn, Trigger)
end

return Script