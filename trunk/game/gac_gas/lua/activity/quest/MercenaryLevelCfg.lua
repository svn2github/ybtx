cfg_load "quest/MercenaryLevelDirect_Common"

local NameLevelMap = {
	["见习佣兵"]= 0,
	["小佣兵"]	= 1,
	["G级佣兵"] = 2,
	["F级佣兵"] = 3,
	["E级佣兵"] = 4,
	["D级佣兵"] = 5,
	["C级佣兵"] = 6,
	["B级佣兵"] = 7,
	["A级佣兵"] = 8,
	["S级佣兵"] = 9,
}
g_MercenaryLevelSortTbl ={}

local function GetMercenaryLevelTbl()
	local tbl = {}
	local LevelKeys = MercenaryLevelDirect_Common:GetKeys()
	tbl.MaxLevel = #LevelKeys
	for i=1, #LevelKeys do
		local LevelName = LevelKeys[i]
		local Level = NameLevelMap[LevelName]
		tbl[Level] = {}
		tbl[Level].LevelName = LevelName
		g_MercenaryLevelSortTbl[Level] = {}
		
		local FieldKeys = MercenaryLevelDirect_Common:GetKeys(LevelName)
		for j=1, #FieldKeys do
			local Field = FieldKeys[j]
			tbl[Level][Field] = {}
			g_MercenaryLevelSortTbl[Level][Field] = {}
			
			local SubjectKeys = MercenaryLevelDirect_Common:GetKeys(LevelName, Field)
			for k=1, #SubjectKeys do
				local Subject = SubjectKeys[k]
				if Field == "达成条件" or Field == "特权" then
					tbl[Level][Field][Subject] = {}
					if Subject == "技能" or Subject == "物品" or Subject == "行为" then
						local Keys = MercenaryLevelDirect_Common:GetKeys(LevelName, Field, Subject)
						for n=1, #Keys do
							tbl[Level][Field][Subject][n] = {}
							tbl[Level][Field][Subject][n].Object 	= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,Keys[n],"Object")	))()
							tbl[Level][Field][Subject][n].LinkPos = assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,Keys[n],"LinkPos") ))()
							tbl[Level][Field][Subject][n].Arg 		= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,Keys[n],"Arg")			))()
						end
					else
						tbl[Level][Field][Subject].Object 	= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,"Object")	))()
						tbl[Level][Field][Subject].LinkPos 	= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,"LinkPos") ))()
						tbl[Level][Field][Subject].Arg 			= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,"Arg")			))()
					end
				else
					tbl[Level][Field].Subject 	= Subject
					tbl[Level][Field].Object 		= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,"Object")	))()
					tbl[Level][Field].LinkPos 	= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,"LinkPos") ))()
					tbl[Level][Field].Arg 			= assert(loadstring("return " .. MercenaryLevelDirect_Common(LevelName,Field,Subject,"Arg")			))()
				end
				table.insert(g_MercenaryLevelSortTbl[Level][Field], Subject)
			end
		end
	end
	return tbl
end

g_MercenaryLevelTbl = GetMercenaryLevelTbl()
g_MercenaryLevelTbl.MaxLevel = #g_MercenaryLevelTbl + 1
UnloadCfgTbl("MercenaryLevelDirect_Common")
AddCheckLeakFilterObj(g_MercenaryLevelTbl)
AddCheckLeakFilterObj(g_MercenaryLevelSortTbl)
