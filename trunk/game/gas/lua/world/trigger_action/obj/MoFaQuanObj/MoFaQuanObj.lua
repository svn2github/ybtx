gas_require "world/int_obj/IntObjServer"
cfg_load "int_obj/IntObj_Common"

local GetDistance = g_GetDistance
local IntObjServerMgr = g_IntObjServerMgr
local g_ObjActionArgTbl = g_ObjActionArgTbl
local IntObj_Common = IntObj_Common


local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

local function ResetQuan(Tick, data)
	local QuanObj,ObjName = data[1],data[2]
	if QuanObj.m_QuanTick then
		UnRegisterTick(QuanObj.m_QuanTick)
		QuanObj.m_QuanTick = nil
--		local ModelName = IntObj_Common(ObjName, "ModelName")
--		IntObjServerMgr:IntObjChangeModel(QuanObj, ModelName)
		IntObjServerMgr:IntObjDoAction(QuanObj,"respond02_2",1)
	end
end

function Entry.Exec(Conn, QuanObj, ObjName, GlobalID)
	if GetDistance(Conn.m_Player,QuanObj) > 6 or QuanObj.m_QuanTick then
		return 
	end
	
	local reFrashTime = IntObj_Common(ObjName, "RefreshTime")
	if reFrashTime < 0 then
		reFrashTime = 10000
	else
		reFrashTime = reFrashTime * 1000
	end
	
	local data = {QuanObj, ObjName}
	IntObjServerMgr:IntObjDoAction(QuanObj, "respond02_1", 2)
	QuanObj.m_QuanTick = RegisterTick("ResetQuanTick",ResetQuan, reFrashTime, data)
	
	--给玩家加一个BUFF
	if g_ObjActionArgTbl[ObjName] then
		local BuffSkillName = g_ObjActionArgTbl[ObjName][1]
--		print("BuffSkillName",BuffSkillName)
		-- Conn.m_Player:PlayerDoItemSkill(BuffSkillName, 1)
		QuanObj:DoIntObjSkill(BuffSkillName, 1, Conn.m_Player)
	end
--	print("幸运值11",Conn.m_Player.m_uNonCombatValue["幸运值"])
--	print("灵感值11",Conn.m_Player.m_uNonCombatValue["灵感值"])
--	print("声望值11",Conn.m_Player.m_uNonCombatValue["声望值"])
end

return Entry
