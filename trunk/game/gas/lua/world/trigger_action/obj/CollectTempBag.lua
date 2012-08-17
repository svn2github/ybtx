gas_require "framework/main_frame/SandBoxDef"

local IsCppBound = IsCppBound
local TempBagAddObj = TempBagAddObj
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, ServerIntObj, ObjName, GlobalID)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	if not Player.m_TempBag then
		return
		--Player.m_TempBag = CTempBag:new()
	end
	Player:CancelCastingProcess()
	Player.m_TempBag:TempBagAddObj(Conn, ServerIntObj, ObjName)
end
 
return Entry
 
