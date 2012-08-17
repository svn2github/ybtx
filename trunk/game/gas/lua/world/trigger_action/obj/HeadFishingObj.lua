gas_require "framework/main_frame/SandBoxDef"

local g_GetDistance = g_GetDistance
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, Obj, ObjName, EntityID)
	if g_GetDistance(Conn.m_Player, Obj) <= 6 then --ÅÐ¶Ï¾àÀë
		if ObjName == "Óã" then
			Gas2Gac:CheckHeadFishing(Conn, "ÄÔµç²¨µöÓã", EntityID)
		end
	end
end

return Entry