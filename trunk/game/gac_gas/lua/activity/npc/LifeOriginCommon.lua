gac_gas_require "activity/npc/LifeOriginCommonInc"

g_LifeOrigin = CLifeOriginCommon:new()

function CLifeOriginCommon:AddSinglePointBornTbl(key, tbl)
	g_NpcBornMgr:AddSinglePointBornTbl(key, tbl)
end

---- 加载 NPC 动态配置（场景编辑器生成的lua文件） 
--function CLifeOriginCommon:LoadNpcDynamicSetting(file)
--	if file == "" then
--		print("该副本没有配置NPC")
--	else
--		 
--	end
--end