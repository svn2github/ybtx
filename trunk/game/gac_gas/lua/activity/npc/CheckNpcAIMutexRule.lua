cfg_load "npc/NpcNature_Server"

--从NPC个性表中获得所有具有嗜睡个性的NPC
local function GetSleepNpcTbl()
	local tbl = {}
	for _, i in pairs(NpcNature_Server:GetKeys()) do
		if NpcNature_Server(i, "嗜睡") then
			tbl[i] = tbl[i] or true
		end
	end
	return tbl
end

local m_SleepNpc = GetSleepNpcTbl()

--具有嗜睡个性的NPC不能巡逻
local function CheckPatrol(tbl)
	local NpcName = tbl["Name"]
	if tbl["Path"] and m_SleepNpc[NpcName] then
		local err_msg = " Npc:【" .. NpcName .. "】 的巡逻状态和嗜睡个性相冲突，请查实！" 
		error(err_msg)
	end
end

function CheckNpcAIMutexRule(tbl)
	CheckPatrol(tbl)
end