cfg_load "scene/DaTaoShaTransport_Server"
--cfg_load "scene/YbEducateActionTransport_Server"
cfg_load "trigger/RandomGame_Server"
--cfg_load_single "trigger/TriggerEvent_Server"

g_DaTaoShaPointMgr = {}

local function DaTaoShaCfg()
	for _, i in pairs(DaTaoShaTransport_Server:GetKeys()) do
		local tbl = DaTaoShaTransport_Server(i)
		g_DaTaoShaPointMgr[i] = {}
		g_DaTaoShaPointMgr[i].BornPoint = loadstring("return " .. (tbl("BornPoint") or ""))()
		g_DaTaoShaPointMgr[i].IntoRoomPoint = loadstring("return " .. (tbl("IntoRoomPoint") or ""))()
		g_DaTaoShaPointMgr[i].OutRoomPoint = loadstring("return " .. (tbl("OutRoomPoint") or ""))()
	end
end

--local function YbEducateActCfg()
--	local multiIndex = { } 
--	local mt = { 
--		__index = function(t, k)
--			 return rawget(t, k) or multiIndex[k]
--		end,
--		__newindex = function() 
--			error("can't not write a read-only table")
--		end,
--	}
--	for k,v in pairs(YbEducateActionTransport_Server) do
--		rawset(v, "TrapPoint", loadstring("return " .. (v.TrapPoint or ""))())
--		rawset(v, "DynamicPoint", loadstring("return " .. (v.DynamicPoint or ""))())
--		rawset(v, "SwitchPoint", loadstring("return " .. (v.SwitchPoint or ""))())
--		multiIndex[k] = v
--	end
--	setmetatable(YbEducateActionTransport_Server, mt)
--end

g_RandomGame = {}
local function RandomGameCfg()
	for _,i in pairs(RandomGame_Server:GetKeys()) do
		local info = RandomGame_Server(i)
		local tbl = {}
		tbl["Subject"] = loadstring("return {" .. (info("Subject") or "").."}")()
		tbl["SelectTrigger"] = loadstring("return {" .. (info("SelectTrigger") or "").."}")()
		tbl["OtherTrigger"] = loadstring("return {" .. (info("OtherTrigger") or "").."}")()
		tbl["HintTrigger"] = loadstring("return {" .. (info("HintTrigger") or "").."}")()
		tbl["SubjectNum"] = info("SubjectNum")
		tbl["Type"] = info("Type")
		tbl["Index"] = info("Index")
		g_RandomGame[i] = tbl
	end
end

local function TriggerEventCfg()
	for k,v in pairs(TriggerEvent_Server) do
		rawset(v, "Arg", loadstring("return {" .. (v.Arg or "").."}")())
	end
end

local function LoadFbCfg()
	DaTaoShaCfg()
	--YbEducateActCfg()
	RandomGameCfg()
--	TriggerEventCfg()
end

LoadFbCfg()