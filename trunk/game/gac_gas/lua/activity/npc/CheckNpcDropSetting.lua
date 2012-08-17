cfg_load "npc/NpcDropItem_Common"
cfg_load "npc/DropTeam_Server"
cfg_load "npc/NpcDropItemObj_Server"
cfg_load "npc/DropItemObjTeam_Server"

g_NpcDropObjTable = {}
g_NpcDropItemTable = {}
g_NpcDropObjTeamRate = {}
g_NpcDropItemTeamRate = {}
AddCheckLeakFilterObj(g_NpcDropObjTable)
AddCheckLeakFilterObj(g_NpcDropItemTable)
AddCheckLeakFilterObj(g_NpcDropObjTeamRate)
AddCheckLeakFilterObj(g_NpcDropItemTeamRate)

local time = GetProcessTime()

local function Load_NpcDropItemObj_Server()
	for _ , i in pairs(NpcDropItemObj_Server:GetKeys()) do
		local Info = NpcDropItemObj_Server(i)
		g_NpcDropObjTable[i] = {}
		for j = 1, 100 do
			if Info("DropTeamRate"..j) ~= nil and Info("DropTeamRate"..j) ~= "" then
				local tbl = {}
				tbl.Level = loadstring("return " .. Info("DropTeamLevel"..j))()
				tbl.DropTeam = Info("DropTeam"..j)
--				tbl.DropRate = Info["DropTeamRate"..j]
				local funStr = "local function GetProbability(NL, PL)  return (" .. Info("DropTeamRate"..j) .. ") end return GetProbability"
				tbl.DropRate = loadstring(funStr)()
				tbl.DropRateStr = Info("DropTeamRate"..j)
				table.insert(g_NpcDropObjTable[i], tbl)
			end
		end
	end
end
Load_NpcDropItemObj_Server()

local function Load_NpcDropItem_Common()
	for _ , i in pairs(NpcDropItem_Common:GetKeys()) do
		local Info = NpcDropItem_Common(i)
		for j = 1, 30 do
			if Info("DropTeamRate"..j) ~= nil and Info("DropTeamRate"..j) ~= "" then
				if g_NpcDropItemTable[i] == nil then
					g_NpcDropItemTable[i] = {}
				end
				local tbl = {}
				tbl.Level = loadstring("return " .. Info("DropTeamLevel"..j))()
				tbl.DropTeam = Info("DropTeam"..j)
--				tbl.DropRate = Info["DropTeamRate"..j]
				local funStr = "local function GetProbability(NL, PL)  return (" .. Info("DropTeamRate"..j) .. ") end return GetProbability"
				tbl.DropRate = loadstring(funStr)()
				table.insert(g_NpcDropItemTable[i], tbl)
			end
		end
	end
end
Load_NpcDropItem_Common()

local function TransNpcDropObjTeamRate()

	--for i , Info in pairs(DropItemObjTeam_Server) do
	for _, i in pairs(DropItemObjTeam_Server:GetKeys()) do
		local Info = DropItemObjTeam_Server(i)
		local AllRateSum=0
		for j=1,30 do
			local DropRate="DropRate"..j
			local Rate = Info(DropRate)
			if Rate == "" or Rate == nil then
				Rate = 0
			else
				Rate = tonumber(Rate)
			end
			AllRateSum=AllRateSum+Rate
		end
		g_NpcDropObjTeamRate[i] = AllRateSum
	end
end
TransNpcDropObjTeamRate()

local function TransNpcDropItemTeamRate()
	for _, i in pairs(DropTeam_Server:GetKeys()) do
		local Info = DropTeam_Server(i)
		local AllRateSum=0
		for j=1,30 do
			local DropRate="DropRate"..j
			local Rate = Info(DropRate)
			if Rate == "" or Rate == nil then
				Rate = 0
			else
				Rate = tonumber(Rate)
			end
			AllRateSum=AllRateSum+Rate
		end
		g_NpcDropItemTeamRate[i] = AllRateSum
	end
end
TransNpcDropItemTeamRate()

time = GetProcessTime() - time
print("转换NPC掉落物品表花费时间:"..time.."毫秒.")