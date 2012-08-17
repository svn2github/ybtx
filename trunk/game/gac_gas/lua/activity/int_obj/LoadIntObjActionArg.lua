cfg_load "int_obj/IntObj_Common"
cfg_load "int_obj/ObjDropItemTeam_Server"
cfg_load "int_obj/TempBagObj_Common"
g_ObjActionArgTbl = {}
AddCheckLeakFilterObj(g_ObjActionArgTbl)
g_ObjDropNum = 50
g_ObjTeamNum = 50

local function LoadIntObjActionArg()
	for _, ObjName in pairs(IntObj_Common:GetKeys()) do
		local p = IntObj_Common(ObjName)
		--------配置表中:OBJ名字中空格的检测---------
		--if string.find(ObjName," ") then
		--	print("配置表IntObj_Common中Obj【"..ObjName.."】中含有空格,请去掉!")
		--	ErrorNum = ErrorNum + 1
		--end
		--------------------------------------------
		if p("ActionArg") ~= "" then
			local str = p("ActionArg")
			str = "{" .. str .. "}"
			g_ObjActionArgTbl[ObjName] = assert(loadstring("return " .. str))()
		end
	end
end

LoadIntObjActionArg()

--local function Load_ObjRandomDropItem_Server()
--	for _ , i in pairs(ObjRandomDropItem_Server:GetKeys()) do
--		local p = ObjRandomDropItem_Server(i)
--		for j = 1, g_ObjDropNum do
--			local Level = p("DropTeamLevel"..j)
--			if Level and Level ~= "" then
--				
--				GetCfgTransformValue(false, "ObjRandomDropItem_Server", i, "DropTeamLevel"..j)
--				
--				Level = loadstring("return " .. Level)()
--				rawset( p, "DropTeamLevel"..j, Level)
--			end
--		end
--	end
--end
--
--Load_ObjRandomDropItem_Server()

--local function TransferObjTable()
--	for _ , i in pairs(ObjDropItemTeam_Server:GetKeys()) do
--		local p = ObjDropItemTeam_Server(i)
--		for j = 1, g_ObjTeamNum do
--			if p["ItemType"..j] == "Npc" then
--				local itemname = p["ItemName"..j]
--				if itemname and itemname ~= "" and Npc_Common(itemname) == nil then
--					
--					GetCfgTransformValue(true,"ObjDropItemTeam_Server","见习佣兵","挑战条件")
--					itemname = assert(loadstring("return {" .. itemname.."}"))()
--					rawset( p, "ItemName"..j, itemname)
--				end
--			end
--		end
--	end
--end

--TransferObjTable()

--local function LoadTempBagObj_Common()
--	for _, i in pairs(TempBagObj_Common:GetKeys()) do
--		local v = TempBagObj_Common(i)
--		
--		local Part = GetCfgTransformValue(false,"TempBagObj_Common", i, "Part")
--		local Compose = GetCfgTransformValue(false,"TempBagObj_Common", i, "Compose")
--		local AreaLimit = GetCfgTransformValue(false,"TempBagObj_Common", i, "AreaLimit")
--		local Effect = GetCfgTransformValue(false,"TempBagObj_Common", i, "Effect")
--	
--		rawset( v, "Part", Part)
--		rawset( v, "Compose", Compose)
--		rawset( v, "AreaLimit", AreaLimit)
--		rawset( v, "Effect", Effect)
--	end
--end
--LoadTempBagObj_Common()
