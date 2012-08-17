cfg_load "int_obj/IntObj_Common"
cfg_load "int_obj/ObjDropItemTeam_Server"
cfg_load "int_obj/ObjRandomDropItem_Server"
cfg_load "model_res/IntObjRes_Client"
cfg_load "smallgame/SmallGame_Common"

cfg_load "scene/Transport_Server"
cfg_load "scene/Trap_Common"
gac_gas_require "scene_mgr/SceneCfg"
gac_gas_require "activity/item/ItemUseInfoMgr"

cfg_load "liveskill/CultivateFlowers_Common"

cfg_load "item/OreAreaMap_Common"

cfg_load "fb_game/AreaFb_Common"
gac_gas_require "scene_mgr/SceneCfg"

--[[
这个文件里放置的函数是用来检测策划的填的配置文档
因为他们一切皆能填错，所以要检查，呵呵……
--]]

gac_gas_require "item/Equip/EquipCommonFunc"
gac_gas_require "item/Equip/EquipDef"
engine_require "common/Misc/TypeCheck"

cfg_load "int_obj/ObjRandomDropItem_Server"

--任务记事本 
gac_gas_require "activity/npc/CheckNpcCommon"

--装备基础表
gac_gas_require "activity/int_obj/LoadIntObjActionArg"

gac_gas_require "activity/npc/LifeOriginCommon"
--初始化区域坐标

--检测不同阵营对应的不同地图名是否存在
cfg_load "scene/SceneMapDifCamp_Common"
gac_gas_require "scene_mgr/SceneCfg"

