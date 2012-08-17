utime0 = GetProcessTime()

require "GasConfig"
require "common/Loader/loader"

print(GetProcessTime()-utime0)
--require "liveskill/CheckLiveSkillCommon"
--require "liveskill/CheckCfgCultivateFlowersCommon"
--require "activity/npc/CheckNpcDropSetting"
--require "activity/npc/CheckNpcNature"
--require "activity/npc/CheckNpcFacialResponse"
--require "activity/npc/CheckNpcTheater"
--require "activity/npc/CheckNpcAIMutexRule"
--require "activity/npc/CheckNpcTriggerAction"
--require "world/trap/TrapServer"
--require "activity/trap/TrapCfgCheck"
--require "activity/smallgame/SmallGameCfgCheck"
--require "activity/item/CheckCfgItemUseCommon"
--require "activity/item/CheckOreAreaMapCommon"
--require "activity/scene/CheckOreAreaCfg"
--require "activity/npc/CheckRandomCreate"
--require "activity/quest/CheckAreaFbQuest"
--require "activity/scene/CheckTransport"
--require "activity/scene/CheckWarZoneCfg"
--require "activity/match_game/CheckMatchGameCfg"
--require "activity/CheckCfg_Common"
--require "activity/quest/QuestMgr"
----require "world/scene_mgr/SceneMgr"
--require "world/int_obj/IntObjServer"
--require "areamgr/AreaMgr"
--require "activity/scene/AreaFbSceneMgr"
--require "item/item_info_mgr/ItemInfoMgr"
--
--g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()
--
--ServerCheckCfg()
--CustomerCheckCfg()
--CheckCfgCommon()
utime1 = GetProcessTime()
require "item/item_info_mgr/ItemInfoMgr"
require "item/check_designer_setting/CheckDesignerSetting"
require "liveskill/CheckLiveSkillCommon"
print(GetProcessTime()-utime1)


g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()

utime2 = GetProcessTime()

CheckCfgInCheckDesignerSettingFile()
CheckLiveSkillCommon()

print(GetProcessTime()-utime2)


