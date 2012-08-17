gac_gas_require "player/PlayerMgrInc"

function g_AddPlayerInfo(uCharID, uCharName, player)
	g_tblPlayerMgr[uCharID] = player
	g_tblPlayerName2ID[uCharName] = uCharID
end

function g_DelPlayerInfo(uCharID, uCharName)
	local function Init()
		g_tblPlayerMgr[uCharID] = nil
		g_tblPlayerName2ID[uCharName] = nil
	end
	apcall(Init)
end

function g_GetPlayerInfo(uCharID)
	return g_tblPlayerMgr[uCharID]
end

function g_GetPlayerInfoByName(uCharName)
	if not g_tblPlayerName2ID[uCharName] then
		return nil
	end
	return g_tblPlayerMgr[g_tblPlayerName2ID[uCharName]]
end

function g_LoadGeniusNamefun()
	for j= 1, 9 do
		local tbl = loadGeniusfile(j)
		local Keys = tbl:GetKeys()
		for i ,p in pairs(Keys) do 
			CTalentHolder_InsertName(p) 
		end
	end
	cfg_load "skill/EquipGift_Common"
	local Keys = EquipGift_Common:GetKeys()
	for m, n in pairs(Keys) do
		CTalentHolder_InsertName(n)
	end
end

function g_GetPlayerClassNameByID(ClassID)
	local tbl = {	[EClass.eCL_MagicWarrior]	= "魔剑士",
					[EClass.eCL_NatureElf]		= "法师",
					[EClass.eCL_EvilElf]		= "邪魔",
					[EClass.eCL_Priest]			= "牧师",
					[EClass.eCL_Paladin]		= "人类骑士",
					[EClass.eCL_ElfHunter]		= "精灵弓箭手",
					[EClass.eCL_Warrior]		= "大剑士",
					[EClass.eCL_OrcWarrior]		= "兽人战士",
					[EClass.eCL_DwarfPaladin]	= "矮人骑士"	}
	return tbl[ClassID]
end
