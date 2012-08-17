gac_gas_require "setting/SettingMgr"

local g_SettingMgr = g_SettingMgr or CSettingMgr:new()
--------------------------------------------------------------
local SettingBox = CreateDbBox(...)

function SettingBox.SysSettingEndDB(parameters)
	local gm_parameters		= parameters["GameSetting"]
	local kp_parameters		= parameters["KeyMapSetting"]
	local mouse_parameters	= parameters["MouseSetting"]
	local ui_parameters		= parameters["UISetting"]
	
	local mouse_ret,kp_ret,gm_ret,ui_ret
	--鼠标设置
	if mouse_parameters then
		local kp_setting = RequireDbBox("KeyMapDB")
		mouse_ret = kp_setting.SaveMouseKey(mouse_parameters)
	end
	
	--键盘设置
	if kp_parameters then
		local kp_setting = RequireDbBox("KeyMapDB")
		kp_ret = kp_setting.SetKeyMap(kp_parameters)
	end
	
	--游戏设置
	if gm_parameters then
		local gm_setting = RequireDbBox("GameSettingDB")
		gm_ret = gm_setting.GameSettingDB(gm_parameters)
	end
	
	if ui_parameters then
		local ui_setting = RequireDbBox("UISettingDB")
		ui_ret = ui_setting.SaveUISettingDB(ui_parameters)
	end
	
	return {["mouse_ret"] = mouse_ret,["kp_ret"] = kp_ret, ["gm_ret"] = gm_ret, ["ui_ret"] = ui_ret}
end

function SettingBox.MouseSettingDB(data)
	local kp_setting = RequireDbBox("KeyMapDB")
	local tbl = g_SettingMgr:GetMouseOptionByType(data["type"])
	if(not tbl) then return end
	local parameter = {}
	parameter["char_id"]		= data["char_id"]
	parameter["lockTarget"]		= tbl.lockTarget
	parameter["movekey"]		= tbl.movekey
	parameter["selectLeft"]		= tbl.selectLeft
	parameter["selectRight"]	= tbl.selectRight
	parameter["attrackLeft"]	= tbl.attrackLeft
	parameter["attrackRight"]	= tbl.attrackRight
	parameter["bShowMsgBox"]	= data["bShowMsgBox"]
	return kp_setting.SaveMouseKeyFromMsgBox(parameter)
end

SetDbLocalFuncType(SettingBox.SysSettingEndDB)
SetDbLocalFuncType(SettingBox.MouseSettingDB)
return SettingBox