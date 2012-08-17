gac_gas_require "framework/common/HotUpdateRequire"

etc_gas_require "GasConfig"
gas_require "dbtransaction/CallDbTransDef"

engine_require "common/Loader/loader"
engine_require "common/Module/Module"
engine_require "common/Value/ValueTable"

gas_require "framework/main_frame/ThreadAppGasHandler"

--‘ÿ»Î≈‰÷√±Ì
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "activity/quest/QuestMgr"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
engine_require "common/Misc/TypeCheck"
gac_gas_require "shortcut/ShortcutCommon"
gac_gas_require "character/Character"
gas_require "log_mgr/CLogMgr"
gas_require "dbtransaction/DbBoxDef"


g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()

function StartUp()
	local handler = CScriptThreadHandler:new()
	g_ThreadApp:SetHandler(handler)
	
	collectgarbage("collect")
end

function CleanUp()
	g_DbChannelMgr:Release()
	g_DbChannelMgr = nil
	
	g_ThreadApp:SetHandler(nil)
	
	CValueTable.Clear()
	CDbResultSet.Clear()
	ClearLoadedCfgTbl()
end

