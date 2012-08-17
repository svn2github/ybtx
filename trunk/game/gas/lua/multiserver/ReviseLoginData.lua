engine_require "server/thread/DbResultShell"

--全部重构完这个函数将直接返回data
function GetDelaySaveData(data)
	local field = {
	["char_id"] = true,
	["PkSwitch"] = true,
	["SpecialItemCoolDown"] = true,
	["playerOnTimeTotal"] = true,
	["killNpcCount"] = true,
	["killPlayerCount"] = true,
	["BurstSoulTime"] = true,
	["skillCoolDownData"] = true,
	["AureMagicData"] = true,
	["StateData"] = true,
	["StateStoreObjData"] = true,
	["tbl_HighStoneID"] = true,
	["char_hp"] = true,
	["char_mp"] = true,
	["lastServerTime"] = true,
	["ServantData"] = true,
	["uTotalPoint"] = true
	}
	
	local tbl = {}
	for i in pairs(field) do
		tbl[i] = data[i]
	end
	return tbl
end

--全部重构完这个函数将返回空table
function GetImmediatelySaveData(data)
	local field = {
	["char_id"] = true,
	["scene_type"] = true,
	["posCur_x"] = true,
	["posCur_y"] = true,
	["scene_id"] = true,
	["scene_name"] = true,
	["scene_logic_name"] = true,
	["area_name"] = true,
	["is_server_shutdown"] = true,
	["serverId"] = true,
	--["PkSwitch"] = true,
	["ChangeToServer"] = true,
--	["SpecialItemCoolDown"] = true,
	["soulCount"] = true,
	["addSoulType"] = true,
	["KillNpcExpCount"] = true,
	["uSoulBottleValue"] = true,
	["TruckName"] = true,
	["Resource"] = true,
	}
	
	local tbl = {}
	for i in pairs(field) do
		tbl[i] = data[i]
	end
	return tbl
end

function ReviseCharInfo(loginData, charLastServerData)
	loginData["IsRevise"] = true
	loginData["SpecialItemCoolDown"] = charLastServerData["SpecialItemCoolDown"]
	loginData["PkSwitch"] = charLastServerData["PkSwitch"]
	loginData["playerOnTimeTotal"] = charLastServerData["playerOnTimeTotal"]
	loginData["killNpcCount"] 		= charLastServerData["killNpcCount"]
	loginData["killPlayerCount"] 	= charLastServerData["killPlayerCount"]
	loginData["BurstSoulTime"]		= charLastServerData["BurstSoulTime"]
	loginData["lastServerTime"] = charLastServerData["lastServerTime"]
	loginData["uTotalPoint"] = charLastServerData["uTotalPoint"]

	loginData["skillCoolDownData"]= CDbResultShell:new()
	for i, v in ipairs(charLastServerData["skillCoolDownData"]) do
		loginData["skillCoolDownData"]:InsertRow(v)
	end
	loginData["AureMagicData"]	= CDbResultShell:new()
	for i, v in ipairs(charLastServerData["AureMagicData"]) do
		loginData["AureMagicData"]:InsertRow(v)
	end
	loginData["StateData"] = CDbResultShell:new()
	for i, v in ipairs(charLastServerData["StateData"]) do
		loginData["StateData"]:InsertRow(v)
	end
	loginData["StateStoreObjData"] = CDbResultShell:new()
	for i, v in ipairs(charLastServerData["StateStoreObjData"]) do
		loginData["StateStoreObjData"]:InsertRow(v)
	end
	loginData["tbl_HighStoneID"]	= charLastServerData["tbl_HighStoneID"]
	loginData["CharInfo"] = CDbResultShell:new(loginData["CharInfo"])
	loginData["CharInfo"]:SetData(0,12,charLastServerData["char_hp"])
	loginData["CharInfo"]:SetData(0,13,charLastServerData["char_mp"])
	
	loginData["ServantData"] = {CDbResultShell:new(),CDbResultShell:new(),CDbResultShell:new()}
	for count=1, # charLastServerData["ServantData"] do
		local OneServantData = charLastServerData["ServantData"][count][2]
		loginData["ServantData"][1]:InsertRow(OneServantData[1])
		for j, p in ipairs(OneServantData[2]) do
			loginData["ServantData"][2]:InsertRow(p)
		end
		for m, q in ipairs(OneServantData[3]) do
			loginData["ServantData"][3]:InsertRow(q)
		end
	end
	
end
