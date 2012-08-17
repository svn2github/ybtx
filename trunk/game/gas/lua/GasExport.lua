engine_require "common/Script/ExportClass"

local ExportTbl = 
{
	{"CExchangeMoney", "commerce/agiotage/Agiotage"},
	{"CPutItemsToBag", "item/item_bag/CPutItemsToBag"},
	{"CGetItemDynInfo", "item/GetItemDynInfo"},
	{"CServerPlayerProperties", "world/player/ServerPlayerProperties"},
	{"CServerIntObjProperties", "world/int_obj/ServerIntObjProperties"},
	{"CServerNpcProperties", "world/npc/ServerNpcProperties"},
	{"CServerBattleHorseProperties", "world/npc/ServerBattleHorseProperties"},
	{"CCharacterServerCallbackHandler", "character/CharacterServerCallbackHandler"},
	{"CCharacterMediatorCallbackHandler", "character/CharacterMediatorCallbackHandler"},
	{"CCharacterDictatorCallbackHandler", "character/CharacterDictatorCallbackHandler"},
	{"CNpcEnmityQueryCallback", "world/common/CNpcEnmityQueryCallback"},
	{"CServantQueryCallback", "world/common/CServantQueryCallback"},
	{"CGameGridRadiusCallback", "world/common/QueryObjInLua"},
	{"CRoomRpcMgr", "item/item_bag/OperateRoomRpc"},
	{"CBreakItem", "item/item_bag/BreakItem"},
	{"CDepotMgr", "item/item_bag/Depot"},
	{"MailMgr", "message/mail/Mail"},
	{"UserAdvice", "message/UserAdvice/UserAdvice"},
	{"CGasCompenate", "activity/gm_compenate/GasCompenate"},
	{"CTongBasic", "relation/tong/GasTongBasic"},
	{"CTongBuilding", "relation/tong/GasTongBuilding"},
	{"CTongDepot", "relation/tong/GasTongDepot"},
	{"CTongItemCreate", "relation/tong/GasTongItemCreate"},
	{"CTongContriMoney", "relation/tong/GasTongContributeMoney"},
	{"CTongTech", "relation/tong/GasTongTechnology"},
	{"CTongWar", "relation/tong/GasTongWar"},
	{"CTeamMark", "relation/team/GasTeamMark"},
	{"CTeamAppl", "relation/team/GasTeamApp"},
	{"CGasTeam", "relation/team/GasTeam"},
	{"CLiveskillBase", "resource/productive_profession/GasLiveSkillBase"},
	{"CExpertAndPractice", "resource/productive_profession/GasExpertAndPractice"},
	{"CDirectionsMake", "resource/productive_profession/GasDirectionsMake"},
	{"CGasToolsMall", "commerce/tools_mall/GasToolsMall"},
	{"CGasSyssetting", "setting/GasSyssetting"},
	{"CKeyMapMgr", "setting/CharacterKeyMap"},
	{"CGameSetting", "setting/GasGamesetting"},
	{"CUISetting", "setting/GasUISetting"},
	--副本
	{"CScopesExploration", "fb_game/scopes_exploration/ScopesExploration"},
	{"CScenarioExploration", "fb_game/scopes_exploration/ScenarioExploration"},
	{"CDareQuestFb", "fb_game/darequest_fb/DareQuestFb"},
	{"CKillYinFengServer", "fb_game/kill_yinfeng/KillYinFengServer"},
	{"CMercenaryEducateAct", "fb_game/mercenary_educate_act/MercenaryEducateAction"},
	{"CMercenaryRoomCreate", "fb_game/mercenary_educate_act/MercenaryRoomCreate"},	
	{"CAreaFbServer", "fb_game/AreaFbServer"},
	{"CaptureTreasure", "fb_game/CaptureTreasure"},
	{"CDragonCave","fb_game/DragonCave"},
	
	{"CFbDpsServer", "fb_game/FbDps_Server"},
	
	{"CScenePkMgr", "world/scene_mgr/ScenePkMgr"},
	{"CFetchRes", "world/tong_area/FetchRes"},
	{"CStoneMgr", "item/stone/CStoneMgr"},
	{"CGasTongMarket", "relation/tong/GasTongMarket"},
	
	{"CTriggerMgr", "world/trigger_action/TriggerMgr"},
	{"CTriggerScript", "world/trigger_action/TriggerScript"},
	{"CCountTrigger", "world/trigger_action/CountTrigger"},
	{"CTimeTrigger", "world/trigger_action/TimeTrigger"},
	{"CTheaterMgr", "/world/scene_mgr/theater_mgr/Theater_Mgr"},
	{"CTheaterFun", "/world/scene_mgr/theater_mgr/TheaterFun"},
	
	{"CMercenaryLevel", "activity/MercenaryLevel"},
	
	-- 中地图
	{"CFbPlayerPosMgr", "world/area_server/FbPlayerPosMgr"},
	{"CGasTeammatePosMgr", "relation/team/GasTeammatePosMgr"},
	{"CGasTongmatePosMgr", "relation/tong/GasTongmatePosMgr"},
	
	--花卉
	{"CFlower", "resource/gather_liveskill/CultivateFlowersServer"},
	{"CPickOre", "world/trigger_action/item/use_item_script/PickOre"},
	
	{"CGasArmorPieceEnactment","item/equip/GasArmorPieceEnactment"},
	{"CGasEquipIdentify","item/equip/GasEquipIdentify"},
	{"CGasEquipSmeltSoulScroll","item/equip/GasEquipSmeltSoulScroll"},
	{"CGasAppellationAndMoral","fight/role_status/GasAppellationAndMoral"},
	{"CGasFightingEvaluation","fight/role_status/GasFightingEvaluation"},
	{"CSortMgr","information/sort/SortMgr"},
	{"CChatChannelMgr","message/channel/ChatChannelMgr"},
	
	{"CGasFriendBasic","relation/association/GasFriendBasic"},
	{"CGasAssoTongBasic","relation/association/GasAssoTongBasic"},
	{"CGasFriendGroupBasic","relation/association/GasFriendGroupBasic"},
	
	{"CGasShortcut","toolbar/shortcut/GasShortcut"},
	{"CGasBattleArrayBooks","fight/battle_array/GasBattleArrayBooks"},
	{"CExpOrSoulStorage","item/useitem/GasExpAndSoulBottle"},
	
	--非战斗行为
	{"CActiveBehavior","world/other_activity/ActiveBehavior"},
	{"CClaspBehavior","world/other_activity/nonefight_action/ClaspBehavior"},
	{"CKickBehavior","world/other_activity/nonefight_action/KickBehavior"},
	
	--佣兵团
	{"CArmyCorpsBase","relation/army_corps/GasArmyCorpsBasic"},

	
	{"CTongRobResMgr","world/tong_area/TongRobResMgr"},
	{"CSpecialNpc","world/npc/SpecialNpc"},
	{"CEssayQuestion","activity/essayquestion/EssayQuestion"},
	{"CRecruitMgr","entrance/recruit/RecruitMethodMgr"},
	
	{"CChanllengeFlagMgr","world/player/ChallengeFlag"},
	{"CTempBag","world/trigger_action/trigger_script/TempBag"},
	{"CItemBagLock","item/item_bag/ItemBagLock"},
	{"CLevelUpGas","fight/role_status/GasRoleLevelUpAttention"},
	{"CTongNeedFireActivity","relation/tong/GasTongNeedFireActivity"},
	{"CCopyMatchGame","fb_game/CopyMatchGame"},
	{"CUseVIPItem","world/trigger_action/item/use_item_script/UseVIPItem"},
	{"CTongItemUseMgr","world/tong_area/TongItemUseMgr"},
	{"CYYLoginServer","entrance/login/YYLoginServer"},
	
	{"CGasWeiBo","open_platform/weibo/GasWeiBo"},
}

SetExportClass("gas", ExportTbl)
