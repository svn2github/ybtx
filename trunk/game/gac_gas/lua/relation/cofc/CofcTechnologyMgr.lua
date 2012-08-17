gac_gas_require "relation/cofc/CofcTechnologyMgrInc"
--cfg_load "cofc/CofcTechnology_Common"

--根据名称和等级获得科技信息
function CCofcTechnologyMgr:GetTechnologyInfo(nIndex)
	return CofcTechnology_Common(nIndex)
end

function CCofcTechnologyMgr:GetTechnologyInfoAll()
	return CofcTechnology_Common
end