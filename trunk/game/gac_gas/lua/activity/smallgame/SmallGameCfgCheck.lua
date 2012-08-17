cfg_load "smallgame/SmallGame_Common"

g_SmallGameStrTbl = {}

local function TransferSmallGameCommon()
	for _, k in pairs(SmallGame_Common:GetKeys()) do
		local v = SmallGame_Common(k)
		g_SmallGameStrTbl[k] = {}
		g_SmallGameStrTbl[k].ScriptArg = loadstring("return {" .. (v("ScriptArg") or "").."}")()
		g_SmallGameStrTbl[k].QuestResult = loadstring("return {" .. (v("QuestResult") or "").."}")()
		g_SmallGameStrTbl[k].SuccArg = loadstring("return {" .. (v("SuccArg") or "").."}")()
		g_SmallGameStrTbl[k].SuccEfxFile = loadstring("return {" .. (v("SuccEfxFile") or "").."}")()
	end
end

TransferSmallGameCommon()
