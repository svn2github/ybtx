gac_gas_require "relation/association/AssociationMgrInc"
gac_gas_require "scene_mgr/SceneCfg"

function CAssociationMgr:GetCanSearchSceneDisplayNameTable()
	local tbl = {}
	local keys = Scene_Basic_Common_Shell:GetKeys()
	for k=1, table.getn(keys) do
		if( 1 == Scene_Basic_Common_Shell(keys[k], "IsCanSearch") ) then
			table.insert(tbl, keys[k])
		end
	end
	return tbl
end

function CAssociationMgr:IsCanSearchScene(sceneName)
	return 1 == Scene_Basic_Common_Shell(sceneName, "IsCanSearch")
end

g_AssociationMgr = g_AssociationMgr or CAssociationMgr:new()