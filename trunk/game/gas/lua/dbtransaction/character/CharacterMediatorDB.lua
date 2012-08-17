gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gas_require "world/player/CreateServerPlayerInc"
cfg_load "appellation/DuelContWinAppellation_Common"
gac_gas_require "activity/scene/AreaFbSceneMgr"


cfg_load "scene/SceneProperty_Server"
cfg_load "scene/PkValue_Common"
gac_gas_require "scene_mgr/SceneCfg"
gac_gas_require "activity/quest/MercenaryLevelCfg"

cfg_load "player/LevelExp_Common"
cfg_load "fb_game/FbActionDirect_Common"

gac_gas_require "framework/common/CMoney"
local g_MoneyMgr = CMoney:new()
local apcall = apcall
local LogErr = LogErr
local apcall = apcall

local SceneProperty_Server = SceneProperty_Server
local PkValue_Common = PkValue_Common
local Scene_Common = Scene_Common
local g_AreaFbLev = g_AreaFbLev
local DuelContWinAppellation_Common = DuelContWinAppellation_Common
local FightSkillKind 		= FightSkillKind
local g_ItemInfoMgr		= CItemInfoMgr:new()
local os = os
local StmtContainer 		= class()
local g_StoreRoomIndex	= g_StoreRoomIndex
local QuestState 		= QuestState
local MaxLevel			= g_TestRevisionMaxLevel
local g_MercenaryLevelTbl = g_MercenaryLevelTbl
local AreaFb_Common = AreaFb_Common
local FbActionDirect_Common = FbActionDirect_Common
local LevelExp_Common = LevelExp_Common
local uint32 = uint32
local event_type_tbl = event_type_tbl

local StmtDef =
{
	"Player_GetCharCreateInfo",
	[[
	select 
	tbl_char.c_sName,

	tbl_char_static.us_uId,
	tbl_char_static.cs_sHair,
	tbl_char_static.cs_sHairColor,
	tbl_char_static.cs_sFace,
	tbl_char_static.cs_uScale,
	tbl_char_static.cs_uSex,
	tbl_char_static.cs_uClass,
	tbl_char_static.cs_uCamp,


	tbl_char_basic.cb_uLevel,
	tbl_char_basic.cb_uMoneyType,


	tbl_char_experience.cs_uLevelExp,

	tbl_char_status.cs_uCurHP,
	tbl_char_status.cs_uCurMP,
	tbl_char_status.cs_uPK,
	
	tbl_char_basic.cb_uMercenaryLevel,
	tbl_char_integral.cs_uLevelIntegral
	
	from
	tbl_char,
	tbl_char_static,
	tbl_char_basic, 
	tbl_char_experience,
	
	tbl_char_status,
	tbl_char_integral
	
	where 
	tbl_char.cs_uId = ? and
	tbl_char_static.cs_uId = ? and
	tbl_char_basic.cs_uId = ? and 
	tbl_char_experience.cs_uId = ? and 
	tbl_char_status.cs_uId = ? and
	tbl_char_static.cs_uId = ? and
	tbl_char_integral.cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef =
{
	"Player_GetCharMoneyInfo",
	[[
	select 
		cm_uMoney,
		cm_uBindingMoney,
		cm_uBindingTicket
	from 
		tbl_char_money
	where 
		cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"Player_GetMainScenePositionInfo",
	[[
	select 
		tbl_scene.sc_uId,
		tbl_scene.sc_sSceneName,
		tbl_scene.sc_uProcess,
		tbl_char_position.cp_uPosX, 
	 	tbl_char_position.cp_uPosY,
	 	tbl_scene.sc_uServerId
	from
		tbl_char_position,
		tbl_scene
	where
		tbl_char_position.cs_uId = ? and
		tbl_char_position.sc_uId = tbl_scene.sc_uId
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
	"Player_GetFbScenePositionInfo",
	[[
	select
		tbl_scene.sc_uId,
		tbl_scene.sc_sSceneName,
		tbl_scene.sc_uProcess,
		tbl_char_fb_position.cfp_uPosX, 
		tbl_char_fb_position.cfp_uPosY,
		tbl_scene.sc_uServerId
	from
		tbl_char_fb_position,
		tbl_scene
	where
		tbl_char_fb_position.cs_uId = ? and
		tbl_char_fb_position.sc_uId = tbl_scene.sc_uId
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=		--删除玩家副本信息
{
	"Player_DelFbPos",
	"delete from tbl_char_fb_position  where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=		--插入玩家副本信息
{
	"Player_InsertFbPos",
	"insert into tbl_char_fb_position(cs_uId, sc_uId, cfp_uPosX, cfp_uPosY) values(?,?,?,?)"
} 
DefineSql( StmtDef, StmtContainer )

-- 根据char id 更新位置【副本(单人副本除外)】
local StmtDef=		--更新玩家副本信息
{
	"Player_SaveFbPos",
	"replace into tbl_char_fb_position(cs_uId, sc_uId, cfp_uPosX, cfp_uPosY) values(?,?,?,?)"
}
DefineSql( StmtDef, StmtContainer )
-- 根据char id 更新位置【场景】
local StmtDef=
{
	"Player_SavePos",
	"update tbl_char_position set sc_uId = ?, cp_uPosX = ?, cp_uPosY = ? where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"SetChangeToPos",
	"insert ignore into tbl_char_change_position(cs_uId, sc_uId, ccp_uPosX, ccp_uPosY) values(?,?,?,?)"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"DleChangeToPos",
	"delete from tbl_char_change_position where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"GetChangeToPosInfo",
	[[
	select 
		p.sc_uId, p.ccp_uPosX, p.ccp_uPosY, s.sc_uType
	from 
		tbl_char_change_position as p, tbl_scene as s 
	where 
		p.cs_uId = ? and
		p.sc_uId = s.sc_uId
	]]
}
DefineSql( StmtDef, StmtContainer )

--获取所有战斗相关物品信息
local StmtDef=
{
	"CreatePlayer_GetFightItemInfo",
	[[
	select
		b.is_uType, b.is_sName, a.is_uId
	from 
		tbl_item_in_grid as a 
		left join
		tbl_item_static as b
		on
			a.is_uId = b.is_uId
	where
		a.cs_uId = ? and b.is_uType = 3;
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{ 
	"Player_GetCharBagInfo",
	[[
	select 
	a.is_uId,a.ibiu_uBagSlotIndex,a.ibiu_uRoomIndex,b.is_uType,b.is_sName,ifnull(c.isb_bIsbind,0)
	from 
	tbl_item_bag_in_use as a left join tbl_item_static as b on a.is_uId=b.is_uId
	left join tbl_item_is_binding as c  on b.is_uId = c.is_uId
	where a.cs_uId=?;
	]]
}
DefineSql( StmtDef, StmtContainer )

--技能冷却时间读取
local StmtDef=
{
	"Player_OnInitSkillCoolDownTime",
	[[
	select 
		cd_sName,
		cd_dCoolDownLeftTime,
		unix_timestamp(now())-unix_timestamp(cd_dCoolDownSaveTime),
		cd_bSwitchEquipSkill
	from 
		tbl_cool_down where cs_uId=?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"Player_OnFbSceneType",
	[[
		select s.sc_uType from tbl_char_fb_position as fp, tbl_scene as s 
		where fp.cs_uId = ? and fp.sc_uId = s.sc_uId
	]]
}
DefineSql( StmtDef, StmtContainer )

--技能冷却时间保存
local StmtDef=
{
	"Player_OnSaveSkillCoolDownTimeToDB",
	[[ replace into tbl_cool_down(cs_uId,cd_sName,cd_dCoolDownLeftTime,cd_dCoolDownSaveTime,cd_bSwitchEquipSkill) values(?,?,?,now(),?) ]]
}
DefineSql( StmtDef, StmtContainer )
--根据角色ID删除登录角色的技能冷却时间
local StmtDef=
{
	"Player_OnDeleteSkillCoolDownTime",
	"delete from tbl_cool_down where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

--Select AureMagic
local StmtDef=
{
	"SelectAureMagic",
	"select am_sAureMagicName,am_uSkillLevel,am_uSkillName,am_uAttrackType from tbl_aure_magic where cs_uId=?"
}
DefineSql( StmtDef, StmtContainer )
--AureMagic Save To DB
local StmtDef=
{
	"SaveAureMagic",
	"replace into tbl_aure_magic(cs_uId,am_sAureMagicName,am_uSkillLevel,am_uSkillName,am_uAttrackType) values(?,?,?,?,?)"
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"DeleteAureMagicDBData",
	"delete from tbl_aure_magic where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

--MagicState Save To DB
local StmtDef=
{
	"SaveMagicState",
	[[
		insert into tbl_char_magicstate set
			cs_uId = ?, 
			cms_uMagicName = ?, 
			cms_uMagicType = ?, 
			cms_uMagicTime = ?, 
			cms_uMagicRemainTime = ?, 
			cms_uCount = ?, 
			cms_uProbability = ?, 
			cms_uMagicStoreObjID1 = ?, 
			cms_uMagicStoreObjID2 = ?, 
			cms_uMagicStoreObjID3 = ?,
			cms_uSkillLevel = ?,
			cms_uSkillName = ?,
			cms_uAttrackType = ?, 
			cms_bIsDot = ?, 
			cms_bFromEqualsOwner = ?, 
			cms_uServantName = ?,
			cso_uServantID = ?
	]]
} 
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"SaveMagicStoreObj",
	[[
		insert into tbl_char_magic_obj set
			cs_uId = ?,
			cmo_uMagicStoreObjID = ?, 
			cmo_uMagicCountID = ?, 
			cmo_uMagicOperaterID = ?, 
			cmo_uData1 = ?, 
			cmo_uData2 = ?, 
			cmo_uServantName = ?,
			cso_uServantID = ?
	]]
} 
DefineSql( StmtDef, StmtContainer )
--StateDataDB读取
local StmtDef=
{
	"_SelectPlayerState",
	[[
		select 
			cms_uMagicName, 
			cms_uMagicType, 
			cms_uMagicTime, 
			cms_uMagicRemainTime, 
			cms_uCount, 
			cms_uProbability, 
			cms_uMagicStoreObjID1, 
			cms_uMagicStoreObjID2, 
			cms_uMagicStoreObjID3, 
			cms_uSkillLevel, 
			cms_uSkillName, 
			cms_uAttrackType, 
			cms_bIsDot, 
			cms_bFromEqualsOwner, 
			cms_uServantName,
			cso_uServantID
		from 
			tbl_char_magicstate
		where
			cs_uId = ? and cms_uServantName = ""
	]]
} 
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_SelectPlayerStateStoreObj",
	[[
		select 
			cmo_uMagicStoreObjID, 
			cmo_uMagicCountID, 
			cmo_uMagicOperaterID, 
			cmo_uData1, 
			cmo_uData2, 
			cmo_uServantName,
			cso_uServantID
		from 
			tbl_char_magic_obj
		where
			cs_uId = ? and cmo_uServantName = ""
	]]
} 
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_SelectServantState",
	[[
		select 
			cms_uMagicName, 
			cms_uMagicType, 
			cms_uMagicTime, 
			cms_uMagicRemainTime, 
			cms_uCount, 
			cms_uProbability, 
			cms_uMagicStoreObjID1, 
			cms_uMagicStoreObjID2, 
			cms_uMagicStoreObjID3, 
			cms_uSkillLevel, 
			cms_uSkillName, 
			cms_uAttrackType, 
			cms_bIsDot, 
			cms_bFromEqualsOwner, 
			cms_uServantName,
			cso_uServantID
		from 
			tbl_char_magicstate
		where
			cs_uId = ? and cms_uServantName != ""
	]]
} 
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"_SelectServantStateStoreObj",
	[[
		select 
			cmo_uMagicStoreObjID, 
			cmo_uMagicCountID, 
			cmo_uMagicOperaterID, 
			cmo_uData1, 
			cmo_uData2, 
			cmo_uServantName,
			cso_uServantID
		from 
			tbl_char_magic_obj
		where
			cs_uId = ? and cmo_uServantName != ""
	]]
} 
DefineSql( StmtDef, StmtContainer )

--清空清除指定char_id 的所有State操作（包括自己召唤兽的）
local StmtDef=
{
	"DeleteMagicState",
	[[
		delete from tbl_char_magicstate where cs_uId = ? and cms_uServantName =""
	]]
}
DefineSql( StmtDef, StmtContainer )
local StmtDef=
{
	"DeleteStateStoreObj",
	[[
		delete from tbl_char_magic_obj where cs_uId = ? and cmo_uServantName = ""
	]]
}
DefineSql( StmtDef, StmtContainer )

--清除主角所有召唤兽信息
local StmtDef=
{
	"DeleteAllServantMagicState",
	[[
		delete from tbl_char_magicstate where 
		cs_uId = ? and cms_uServantName != ""
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"DeleteAllServantStateStoreObj",
	[[
		delete from tbl_char_magic_obj where 
		cs_uId = ? and cmo_uServantName != ""
	]]
}
DefineSql( StmtDef, StmtContainer )

--易变值保存
local StmtDef=
{
	"Player_OnSaveToDB",
	"update tbl_char_status set cs_uCurHP = ?, cs_uCurMP = ? where cs_uId = ?"
}
DefineSql( StmtDef, StmtContainer )


local StmtDef=
{
	"DbSetCamp",
	[[
	update tbl_char_static set cs_uCamp = ? where cs_uId = ?;
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef=
{
	"_GetPlayerCamp",
	[[
	select cs_uCamp from tbl_char_static  where cs_uId = ?;
	]]
}
DefineSql( StmtDef, StmtContainer )

--[[升级保存]]--
local StmtDef=
{
	"Player_SaveLevel",
	[[
	update 
		tbl_char_basic
		join 
		tbl_char_experience 
		on 
			tbl_char_basic.cs_uId = tbl_char_experience.cs_uId	
	set 
		tbl_char_basic.cb_uLevel = ?, 
		tbl_char_experience.cs_uLevelExp = ?
	where
		tbl_char_basic.cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

--修改玩家经验
local StmtDef=
{
	"_UpdatePlayerExperience",
	[[
	update tbl_char_experience set cs_uLevelExp = ? where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

--[[升佣兵等级保存]]--
local StmtDef=
{
	"Player_SaveMercenaryLevel",
	[[
	update 
		tbl_char_basic 
		join 
		tbl_char_integral 
		on 
			tbl_char_basic.cs_uId = tbl_char_integral.cs_uId	
	set 
		tbl_char_basic.cb_uMercenaryLevel = ?, 
		tbl_char_integral.cs_uLevelIntegral = ?
	where
		tbl_char_basic.cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef =
{
	"_GetPlayerLevel",
	"select cb_uLevel from tbl_char_basic where cs_uId = ? "
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_GetCharLevelExp",
	[[
	select
		tbl_char_basic.cb_uLevel,
		tbl_char_experience.cs_uLevelExp,
		tbl_char_basic.cb_uMercenaryLevel,
		tbl_char_integral.cs_uLevelIntegral
	from
		 tbl_char_basic,
		 tbl_char_experience,
		 tbl_char_integral
	where
		tbl_char_basic.cs_uId = ? and
		tbl_char_experience.cs_uId = ? and
		tbl_char_integral.cs_uId = ?
	]]
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_GetPlayerPos",
	"select cp_uPosX,cp_uPosY from tbl_char_position where cs_uId = ? and sc_uId = ?"
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_GetPlayerSceneNamePos",
	[[
	select
		tbl_scene.sc_sSceneName,
		tbl_char_position.cp_uPosX,
		tbl_char_position.cp_uPosY
	from
		tbl_char_position,tbl_scene
	where
		tbl_char_position.sc_uId = tbl_scene.sc_uId	and	cs_uId = ?
	]]
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_GetPlayerRePos",
	"select sc_uId,cp_uPosX,cp_uPosY from tbl_char_position where cs_uId = ?"
}
DefineSql( StmtDef , StmtContainer )

local StmtDef =
{
	"_GetPlayerFbPos",
	"select cfp_uPosX,cfp_uPosY from tbl_char_fb_position where cs_uId = ? and sc_uId = ?"
}
DefineSql( StmtDef , StmtContainer )

--插入角色下线信息
local StmtDef =
{
	"PlayerLogoutSave",
	"update tbl_char_onlinetime set co_dtLastLogOutTime = now() where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--角色下线删除在线信息
local StmtDef =
{
	"_DelCharOnlineInfo",
	"delete from tbl_char_online where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef =
{
	"_DelCharLefttime",
	"delete from tbl_char_lefttime where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--保存角色登录时间
local StmtDef = 
{
	"UpdateCharLoginTime",
	"update tbl_char_onlinetime set co_dtLastLoginTime = now() where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--保存角色上线信息
local StmtDef = 
{
	"_SaveCharOnlineInfo",
	"replace into tbl_char_online(cs_uId,co_uOnServerId) values(?,?)"
}
DefineSql(StmtDef,StmtContainer)

--更新用户所在服务器
local StmtDef = 
{
	"UpdateUserOnServerId",
	[[
	update
		tbl_char_static as cs,tbl_user_online as uo
	set
		uo_uOnServerId = ?
	where
		cs.us_uId = uo.us_uId and cs.cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"GetUserOnServerId",
	[[
	select 
		uo.uo_uOnServerId
	from
		tbl_char_static as cs,tbl_user_online as uo
	where
		cs.us_uId = uo.us_uId and cs.cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)


--保存角色战斗信息
local StmtDef =
{
	"SavePlayerFightInfo",
	"update tbl_char_fight_info set cfi_uKillNpcCount = cfi_uKillNpcCount + ? , cfi_uKillPlayerCount = cfi_uKillPlayerCount + ? ,cfi_uPlayerOnTimeTotal = cfi_uPlayerOnTimeTotal + ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--保存角色死亡次数
local StmtDef =
{
	"SavePlayerDeadTimes",
	"update tbl_char_fight_info set cfi_uDeadTimes = cfi_uDeadTimes + ?  where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--角色offline时间查询
local StmtDef = 
{
	"_GetOffLineTime",
	"select unix_timestamp(co_dtLastLoginTime)-unix_timestamp(co_dtLastLogOutTime) from tbl_char_onlinetime where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

--非战斗数值 初始化
local StmtDef = 
{
	"InitNonCombatValue",
	"insert into tbl_char_non_combat values(?,?,?)"
}
DefineSql(StmtDef,StmtContainer)
-- 灵感值
local StmtDef = 
{
	"AddInspiration",
	"update tbl_char_non_combat set cnc_uInspiration = cnc_uInspiration + ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"GetInspiration",
	"select cnc_uInspiration from tbl_char_non_combat where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

-- 预存经验
local StmtDef = 
{
	"AddPrepExp",
	"update tbl_char_non_combat set cnc_uPrepExp = cnc_uPrepExp + ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"GetPrepExp",
	"select cnc_uPrepExp from tbl_char_non_combat where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)


local StmtDef =
{
	"GetServantData",
	[[
		select 
			cso_sServantResName,
			cso_uServantLevel,
			cso_uServantAIType,
			cso_uServantType,
			cso_uCurHP,
			cso_uCurMP,
			cso_uLeftTime,
			cso_uServantID
		from 
			tbl_char_servant where cs_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef =
{
	"SaveServantData",
	[[
		insert into tbl_char_servant set 
		cs_uId = ?,
		cso_sServantResName = ?,
		cso_uServantLevel = ?,
		cso_uServantAIType = ?,
		cso_uServantType = ?,
		cso_uCurHP = ?,
		cso_uCurMP = ?,
		cso_uLeftTime = ?,
		cso_uServantID = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"DeleteAllServantDBData",
	"delete from tbl_char_servant where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"updatePkSwitchState",
	"update tbl_char_status set cs_uPK = ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"updateDuelWinCount",
	"update tbl_char_status set cs_uDuelWinCount = ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"GetCharNameById",
	"select c_sName from tbl_char where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

---------------------------玩家PK挑战计数-------------------------------
local StmtDef=
{
	"GetUserNameByCharId",
	[[
		select
			us.us_sName
		from
			tbl_user_static as us
		where
			us_uId = (select us_uId from tbl_char_static where cs_uId = ?)
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"AddPkInfo",
	"insert into tbl_char_pk (cs_uId, cp_uMin, cp_uMax, cp_dtTime) values (?, ?, ?, now())"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetPkInfo",
	"select count(*) from tbl_char_pk where cs_uId = ? and cp_uMin = ? and cp_uMax = ? and date(cp_dtTime) = date(now())"
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"ClearPkInfo",
	"delete from tbl_char_pk"
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"GetDelaySaveInfo",
	"select s_uId,  dsi_uDataId, unix_timestamp(dsi_dtAddTime) from tbl_delay_save_info where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"InsertDelaySaveInfo",
	"replace into tbl_delay_save_info(cs_uId, s_uId, dsi_uDataId, dsi_dtAddTime) values(?,?,?, now())"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"DeleteDelaySaveInfo",
	"delete from tbl_delay_save_info where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"AddPkFlagInfo",
	"replace into tbl_char_pk_flag (cs_uId, cpf_uId, cpf_uFlagTime) values (?, ?, ?)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetPkFlagInfo",
	"select cpf_uFlagTime from tbl_char_pk_flag where cs_uId = ? and cpf_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"UpdatePkFlagInfo",
	"update tbl_char_pk_flag set cpf_uFlagTime = ? where cs_uId = ? and cpf_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"ClearPkFlagInfo",
	"delete from tbl_char_pk_flag"
}
DefineSql(StmtDef,StmtContainer)


---------------------------------------------------------***------------------------------------------------------------
local CharacterMediatorDB = CreateDbBox(...)

local function AddPkFlagInfo(attackerId, beAttackedId, time)
	StmtContainer.AddPkFlagInfo:ExecStat(attackerId, beAttackedId, time)
end

local function UpdatePkFlagInfo(time, attackerId, beAttackedId)
	StmtContainer.UpdatePkFlagInfo:ExecStat(time, attackerId, beAttackedId)
end

local function GetPkFlagInfo(attackerId, beAttackedId)

	local result = StmtContainer.GetPkFlagInfo:ExecStat(attackerId, beAttackedId)
	if result:GetRowNum() <= 0 then
		AddPkFlagInfo(attackerId, beAttackedId, os.time())
		return true
	end
	local time = result:GetData(0,0)
	UpdatePkFlagInfo(os.time(), attackerId, beAttackedId)
	if os.time() - time >= 180 then
		return true
	end
	return false
end

local function GetCharNameById(charId)
	local charNameRet = StmtContainer.GetCharNameById:ExecStat(charId)
	if charNameRet:GetRowNum() > 0 then
		return charNameRet:GetData(0,0)
	end
end

local function PlayerOnFbPos(charId)
	local result = StmtContainer.Player_OnFbSceneType:ExecStat(charId)
	local sceneType = nil
	if result:GetRowNum() > 0 then
		sceneType = result:GetData(0,0)
	end
	return sceneType
end

local function updatePlayerPkValue(data)
	local pkValue = data["PkValue"] 
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local result
	local res = AreaFbPointDB.AddAreaFbPointByType(data["char_id"],pkValue,7,data["scene_name"],nil,event_type_tbl["Pk状态杀死玩家加积分"])
	if not res then
		return 
	end
	local uTotal = AreaFbPointDB.GetPointStatement(data["id"],7)
	if -pkValue + uTotal < 0 then
		if uTotal == 0 then
			pkValue = 0 
			result = true
		else
			result = AreaFbPointDB.AddAreaFbPointByType(data["id"], -uTotal,7,data["scene_name"],nil,event_type_tbl["Pk状态被杀死减积分"])
		end
	else
		result = AreaFbPointDB.AddAreaFbPointByType(data["id"], -pkValue,7,data["scene_name"],nil,event_type_tbl["Pk状态被杀死减积分"])
	end
	if not result then
		CancelTran()
		return 
	end
	
	if res and result then
		for _, index in pairs(PkValue_Common:GetKeys()) do
			if data["Value"] >= PkValue_Common(index, "PkMinValue") and data["Value"] < PkValue_Common(index, "PkMaxValue") then
				local tempRes = StmtContainer.GetPkInfo:ExecStat(data["char_id"], PkValue_Common(index, "PkMinValue"), PkValue_Common(index, "PkMaxValue"))
				local count = tempRes:GetData(0,0)
				if count > 0 then
					return {res, false,data["id"]}
				else
					StmtContainer.AddPkInfo:ExecStat(data["char_id"], PkValue_Common(index, "PkMinValue"), PkValue_Common(index, "PkMaxValue"))
					return {res, true,data["id"]}
				end
			end
		end
		return {res, false, data["id"]}
	end
end

local function ClearPkInfo()
	StmtContainer.ClearPkInfo:ExecStat()
end

local function ClearPkFlagInfo()
	StmtContainer.ClearPkFlagInfo:ExecStat()
end

local function UpdatePkState(data)
	if data["PkSwitch"] == true then
		data["PkSwitch"] = 1
	else
		data["PkSwitch"] = 0
	end
	StmtContainer.updatePkSwitchState:ExecSql("", data["PkSwitch"], data["char_id"])
end

local function GetCharCreateInfo(data)
	local char_id = data["char_id"]
	local result = StmtContainer.Player_GetCharCreateInfo:ExecStat(char_id,char_id,char_id,char_id,char_id,char_id,char_id)
	return result
end

local function GetCharMoneyInfo(data)
	local char_id = data["char_id"]
	local result = StmtContainer.Player_GetCharMoneyInfo:ExecStat(char_id)
	return result
end


--根据角色id获得其帮会id
local function GetPlayerTongID(PlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tong_id,tong_name,tong_pos = tong_box.GetTongID(PlayerID)
	return tong_id,tong_name,tong_pos
end

local function GetTongType(PlayerID)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(PlayerID)
	return tong_box.GetTongType(uTongId)
end

local function GetPlayerArmyCorpsName(PlayerID)
	local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	local armyCorps_id,armyCorps_name = ArmyCorps_box.GetArmyCorpsIDAndNameDB(PlayerID)
	return armyCorps_id,armyCorps_name
end

local function GetPlayerArmyCorpsPos(PlayerID)
	local ArmyCorps_box = RequireDbBox("ArmyCorpsBasicDB")
	return ArmyCorps_box.GetPositionById(PlayerID)
end

function CharacterMediatorDB.CopyChangeToPos(data)
	local char_id = data["char_id"]
	local result = StmtContainer.GetChangeToPosInfo:ExecSql("nnnn", char_id)
	if result:GetRowNum()> 0 then
		local sceneId, x, y, sceneType = result:GetData(0,0),result:GetData(0,1),result:GetData(0,2),result:GetData(0,3)
		if SceneProperty_Server(sceneType,"PositionTable") == "Main" then
			StmtContainer.Player_SavePos:ExecSql("", sceneId, x, y, char_id)
			StmtContainer.Player_DelFbPos:ExecSql( "", char_id)
		else
			StmtContainer.Player_SaveFbPos:ExecSql("", char_id, sceneId, x, y)
		end
		StmtContainer.DleChangeToPos:ExecSql("", char_id)
	end
end

function CharacterMediatorDB.GetMainScenePositionInfo(data)
	local char_id = data["char_id"]
	local result = StmtContainer.Player_GetMainScenePositionInfo:ExecSql('ns[32]nnnn', char_id)
	local mainInfo = result:GetRow(0)
	result:Release()
	return mainInfo
end

function CharacterMediatorDB.GetFbScenePositionInfo(data)
	local char_id = data["char_id"]
	local CurServerId = data["CurServerId"]
	local ChangeType = data["ChangeType"]
	local result = StmtContainer.Player_GetFbScenePositionInfo:ExecSql('ns[32]nnnn', char_id)
	if result:GetRowNum() > 0 then
		local fbInfo = result:GetRow(0)
		local sceneId = fbInfo[1]
		local sceneName = fbInfo[2]
		local serverId = fbInfo[6]
		result:Release()
		
		local IsInFb = true
		if ChangeType == 0 then
			local TeamSceneMgrDB = RequireDbBox("TeamSceneMgrDB")
			IsInFb = TeamSceneMgrDB.CheckPlayerInFbScene(char_id, sceneId)
		end
		if not IsInFb then
			CharacterMediatorDB.DelFbPosition({["char_id"]=char_id})
			return
		else
			
			if serverId == 0 then --帮会副本需要在副本内上线, 没有场景也要创建
				local SceneMgrDB = RequireDbBox("SceneMgrDB")
				SceneMgrDB.SetSceneServer(sceneId, CurServerId)
				fbInfo[6] = CurServerId
			end
			return fbInfo
			
		end
		
	end
	result:Release()
end

local function GetCharLevelExp(char_id)
	local result = StmtContainer._GetCharLevelExp:ExecSql('nnnn',char_id,char_id,char_id)
	return result
end

local function GetCharPos(data)
	local res = nil
	local res1 = StmtContainer._GetPlayerPos:ExecSql('nn',data["char_id"],data["SceneId"])
	if res1:GetRowNum() ~= 0 then
		res = res1
	else
		res = StmtContainer._GetPlayerFbPos:ExecSql('nn',data["char_id"],data["SceneId"])
	end
	if res:GetRowNum() ~= 0 then
		local posx, posy = res:GetData(0,0),res:GetData(0,1)
		res:Release()
		return posx, posy
	else
		return nil,nil
	end
end

local function GetCharRePos(data)
	local res = StmtContainer._GetPlayerRePos:ExecSql('nnn',data["char_id"])
	if res:GetRowNum() ~= 0 then
		local sceneId, posx, posy = res:GetData(0,0),res:GetData(0,1),res:GetData(0,2)
		res:Release()
		return {sceneId, posx, posy}
	else
		return nil,nil,nil
	end
end

local function GetCharNamePos(data)
	local res = StmtContainer._GetPlayerSceneNamePos:ExecSql('s[32]nn',data["char_id"])
	if res:GetRowNum() ~= 0 then
		local SceneName, posX, posY = res:GetData(0,0),res:GetData(0,1),res:GetData(0,2)
		res:Release()
		return {SceneName, posX, posY}
	else
		return nil
	end
end

local function GetCharBagInfo(data)
	local char_id = data["char_id"]
	local result = StmtContainer.Player_GetCharBagInfo:ExecStat(char_id)
	return result
end

local function GetItemInfo(data)
	local char_id = data["char_id"]
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	return item_bag_box.InitBagItem(char_id)
end

local function GetSkillCoolDownTime(data)
	local char_id = data["char_id"]
	local result = StmtContainer.Player_OnInitSkillCoolDownTime:ExecStat( char_id )
	return result
end
local function DeleteSkillCoolDownTime(data)
	local char_id = data["char_id"]
	StmtContainer.Player_OnDeleteSkillCoolDownTime:ExecSql( "", char_id)
end
local function GetAureMagicData(data)
	local char_id = data["char_id"]
	local result = StmtContainer.SelectAureMagic:ExecSql( "s[32]ns[32]n", char_id )
	return result
end
local function DeleteAureMagicData(data)
	local char_id = data["char_id"]
	StmtContainer.DeleteAureMagicDBData:ExecSql( "", char_id)
end
------------------------------------------------------------------------------------------------------------------------


local function GetPlayerStateData(data)
	local char_id = data["char_id"]
	local result = StmtContainer._SelectPlayerState:ExecStat(char_id)
	return result
end
local function GetPlayerStateStoreObjData(data)
	local char_id = data["char_id"]
	local result = StmtContainer._SelectPlayerStateStoreObj:ExecStat(char_id)
	return result
end
local function GetServantStateData(data)
	local char_id = data["char_id"]
	local result = StmtContainer._SelectServantState:ExecStat(char_id)
	return result
end
local function ServantStateStoreObjData(data)
	local char_id = data["char_id"]
	local result = StmtContainer._SelectServantStateStoreObj:ExecStat(char_id)
	return result
end
local function DeleteStateFromDB(data)
	local char_id = data["char_id"]
	StmtContainer.DeleteMagicState:ExecSql( "", char_id)
	StmtContainer.DeleteStateStoreObj:ExecSql( "", char_id)
end

local function GetUserNameByCharId(char_id)
	local res = StmtContainer.GetUserNameByCharId:ExecStat(char_id)
	return res:GetData(0,0)
end

-- 下线时获取角色信息
local StmtDef = 
{
	"_SelectLogOutCharInfo",
	[[
		select 
			cs.us_uId, cs.cs_uId, cs.cs_uClass, cb.cb_uLevel,cm.cm_uMoney, cm.cm_uBindingMoney, ce.cs_uLevelExp
		from 
			tbl_char_static as cs, tbl_char_basic as cb, tbl_char_experience ce,tbl_char_money cm
		where
			cs.cs_uId = cb.cs_uId and cb.cs_uId = ce.cs_uId and ce.cs_uId = cm.cs_uId
			and cs.cs_uId = ?
	]]
	
}
DefineSql(StmtDef, StmtContainer)

function CharacterMediatorDB.LogLogout(char_id)
	local charName = GetCharNameById(char_id)
	if not charName then
		return
	end
	StmtContainer.PlayerLogoutSave:ExecStat(char_id)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		StmtContainer._DelCharOnlineInfo:ExecStat(char_id)
		if g_DbChannelMgr:LastAffectedRowNum() > 0 then
			StmtContainer._DelCharLefttime:ExecStat(char_id)
			Db2GasCall("CharLogout", {char_id, charName})
			local char_info = StmtContainer._SelectLogOutCharInfo:ExecStat(char_id)
			local data = {char_info(1,1),char_info(1,2),char_info(1,3),char_info(1,4),char_info(1,5),char_info(1,6),char_info(1,7)}
			char_info:Release()
			Db2CallBack("EProtocolLogOut",data, GetUserNameByCharId(char_id))
		end
	end
end

local function OnSaveSceneToDB(data)
	local char_id	= data["char_id"]
	local posCur_x	= data["posCur_x"]
	local posCur_y	= data["posCur_y"]
	local scene_id	= data["scene_id"]
	local function fun()
		StmtContainer.Player_SavePos:ExecSql( "", scene_id,  posCur_x, posCur_y, char_id )
	end
	if not apcall(fun) then
		LogErr("保存坐标异常", "场景id: " .. scene_id .. "  坐标(" .. tostring(posCur_x) .. "," .. tostring(posCur_y) .. ")")
		return false
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end


local function IsSceneExist(sceneId)
	local ret = StmtContainer.FindScene:ExecStat(sceneId)
	return ret:GetData(0,0) > 0
end

local function OnSaveFBToDB(data)
	if not IsSceneExist(data["scene_id"]) then
		CharacterMediatorDB.DelFbPosition(data)
		return
	end
	local char_id	= data["char_id"]
	local scene_id	= data["scene_id"]
	assert(scene_id ~= 0)
	local posCur_x	= data["posCur_x"]
	local posCur_y	= data["posCur_y"]
	local function fun()
		StmtContainer.Player_SaveFbPos:ExecSql( "", char_id, scene_id,  posCur_x, posCur_y)
		if data["scene_type"] == 25 then
			local SceneMgrDB = RequireDbBox("SceneMgrDB")
			local sceneName = SceneMgrDB._GetSceneNameById(scene_id)
			local mainSceneId = SceneMgrDB._GetNormalSceneIdByName(Scene_Common[sceneName].BasicName)
			StmtContainer.Player_SavePos:ExecStat(mainSceneId,  posCur_x, posCur_y, char_id )
		end
	end
	if not apcall(fun) then
		LogErr("保存坐标异常", "场景Type: " .. data["scene_type"] .. "  坐标(" .. tostring(posCur_x) .. "," .. tostring(posCur_y) .. ")")
		return false
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

local StmtDef = {
	"FindScene",
	"select count(*) from tbl_scene where sc_uId = ?"
}
DefineSql( StmtDef, StmtContainer )

local function OnSavePosToDB(data)
	data["ModifyScene"] = SceneProperty_Server(data["scene_type"],"SavePosType")
	
	local scene_type = data["scene_type"]
	local savePosType = SceneProperty_Server(data["scene_type"],"SavePosType")
	local positionTable = SceneProperty_Server(data["scene_type"],"PositionTable")
	
	if positionTable == "Main" then   --场景
		OnSaveSceneToDB(data)
	else	--其他副本
		if savePosType == "Delete" then--删除副本场景中的坐标
			CharacterMediatorDB.DelFbPosition(data)
		elseif savePosType == "Modify" then--修改副本场景中的坐标
			OnSaveFBToDB(data)
		end
	end
end

local function OnSaveToDB(data)
	local char_id 	= data["char_id"]
	local char_hp	= data["char_hp"]
	local char_mp	= data["char_mp"]
--	if char_hp == 0 then
--		char_hp =1
--	end
	if char_mp == 0 then
		char_mp =1
	end
	StmtContainer.Player_OnSaveToDB:ExecSql( "", char_hp, char_mp, char_id )
end

local function DelAllValidateInfo(data)
	local uCharID = data["char_id"]
	local RevertValidateBox = RequireDbBox("RevertValidateDB")
	RevertValidateBox.DelAllValidateInfoByCharID(uCharID)
end

--小队
local function DealWithTeamInfoLogout(data)
		local uCharID = data["char_id"]
		local team = RequireDbBox("GasTeamDB")
		local TeamID = team.GetTeamID(uCharID)
		local uTeamCapatinID = team.GetCaptain(TeamID)
		--将队长交给别人
		local tblMembers = team.GetTeamMembers(TeamID)
		local new_cap = uCharID   --新队长的id
		if (uCharID == uTeamCapatinID) then
			new_cap = team.GetSomeMemberOnlineByTeamID(TeamID,uCharID)
			if new_cap ~= uCharID then
				team.SetCaptain(new_cap,TeamID)
			end
		end
		local RevertValidateBox = RequireDbBox("RevertValidateDB")
		local validateInfo = RevertValidateBox.GetValidateInfoByInviter(uCharID,1)
		local ex = RequireDbBox("Exchanger")
		local char_name = ex.getPlayerNameById(uCharID)
		local new_cap_name = ex.getPlayerNameById(new_cap)
		local tbl = {}
		tbl["team_id"] = TeamID
		tbl["team_cap"] = new_cap
		tbl["team_members"] = tblMembers
		tbl["validateInfo"] = validateInfo
		tbl["char_name"] = char_name
		tbl["new_cap_name"] = new_cap_name
		return tbl
end


local function SaveStateToDBBegin(char_id,stateVec,storedObjVec,servantID)
--	local char_id 		= data["char_id"]
--	local stateVec 		= data["StateData"] or {}
--	local storedObjVec 	= data["StateStoreObjData"] or {}
	local id = servantID or 0
	local num = #(stateVec)
	for i =1, num do
		StmtContainer.SaveMagicState:ExecSql( "", char_id, stateVec[i][1], stateVec[i][2], stateVec[i][3], stateVec[i][4] ,stateVec[i][5], stateVec[i][6], stateVec[i][7],
		stateVec[i][8], stateVec[i][9], stateVec[i][10], stateVec[i][11], stateVec[i][12], stateVec[i][13], stateVec[i][14], stateVec[i][15], id )
	end
	
	local num = #(storedObjVec)
	for i =1, num do
		StmtContainer.SaveMagicStoreObj:ExecSql( "", char_id, storedObjVec[i][1], storedObjVec[i][2], storedObjVec[i][3], storedObjVec[i][4], storedObjVec[i][5], storedObjVec[i][6], id )
	end
end

--保存死亡次数,杀怪数和杀人
local function SaveCharFightInfo(data)
	local char_id = data["char_id"]
	local killNpcCount = data["killNpcCount"]
	local killPlayerCount = data["killPlayerCount"]
	local playerOnTimeTotal = data["playerOnTimeTotal"] 
	StmtContainer.SavePlayerFightInfo:ExecSql("",killNpcCount or 0 ,killPlayerCount or 0,playerOnTimeTotal or 0 ,char_id)
end

--下线保存战斗力总分
local function SaveFightingEvaluation(data)
	local FightingEvaluationDB		= RequireDbBox("FightingEvaluationDB")
	local param = {["charId"] = data["char_id"],["uTotalPoint"] = data["uTotalPoint"]}
	FightingEvaluationDB.SaveFightingEvaluation(param)
end

--保存角色死亡次数
function CharacterMediatorDB.SaveCharDeadTimes(data)
	local deadTimes = data["deadTimes"]
	local SceneName = data["SceneName"]
	local KillerId = data["KillerId"]
	local BeAttackerId = data["BeAttackerId"]
	local IsKillByNpc = data["IsKillByNpc"]
	local NpcName = data["NpcName"]
	local BeKilledByNpc = data["BeKilledByNpc"]
	local LogMgr = RequireDbBox("LogMgrDB")
	

	if IsKillByNpc then
		local uCharLevel = CharacterMediatorDB.GetPlayerLevel(BeAttackerId)
		LogMgr.SavePlayerKilledByNpc(NpcName,BeAttackerId, SceneName,uCharLevel)	
	else
		if KillerId then
			local attackerLevel = CharacterMediatorDB.GetPlayerLevel(KillerId)
			local beAttackerLevel = CharacterMediatorDB.GetPlayerLevel(BeAttackerId)
			LogMgr.SavePlayerFightInfo( KillerId, BeAttackerId, SceneName,attackerLevel,beAttackerLevel )
		end
	end
		
	StmtContainer.SavePlayerDeadTimes:ExecSql("",deadTimes,BeAttackerId)
	if BeKilledByNpc  then
    	local EquipDurabilityDB = RequireDbBox("EquipDurabilityDB")
    	local para = {["CharID"]= BeAttackerId, ["AttackTimes"]=0, ["BeAttackTimes"]=0, ["DeadFlag"] = true}
        local suc, result = EquipDurabilityDB.UpdateEquipDurability(para)
        return suc, result
   end
end

local function SaveSkillCoolDownTimeToDB(data)
	local char_id 				= data["char_id"]
	local skillCoolDownDataTbl	= data["skillCoolDownData"] or {}
	local num = #(skillCoolDownDataTbl)
	for i =1, num do
		StmtContainer.Player_OnSaveSkillCoolDownTimeToDB:ExecSql( "", char_id, skillCoolDownDataTbl[i][1], skillCoolDownDataTbl[i][2], skillCoolDownDataTbl[i][4] )
	end			
end

function CharacterMediatorDB.SaveAureMagicToDB(data)
	local char_id			= data["char_id"]
	local AureMagicDataTbl	= data["AureMagicData"] or {}
	local num = #(AureMagicDataTbl)
	for i =1, num do
		StmtContainer.SaveAureMagic:ExecSql( "", char_id, AureMagicDataTbl[i][1], AureMagicDataTbl[i][2], AureMagicDataTbl[i][3], AureMagicDataTbl[i][4] )
	end	
end

local function SaveServantToDB(data)
	local char_id		= data["char_id"]
	local ServantDataTbl	= data["ServantData"] or {}
	for i=1, #(ServantDataTbl) do
		local servantID,DataTbl = ServantDataTbl[i][1], ServantDataTbl[i][2]
		StmtContainer.SaveServantData:ExecSql("", char_id, DataTbl[1][1],DataTbl[1][2],DataTbl[1][3],DataTbl[1][4],DataTbl[1][5],DataTbl[1][6],DataTbl[1][7],servantID)
		SaveStateToDBBegin(data["char_id"],DataTbl[2],DataTbl[3],servantID)
	end
end

local function GetServantDataFromDB(data)
	local char_id = data["char_id"]
	local ServantData = {}
	ServantData[1] 	= StmtContainer.GetServantData:ExecSql("s[64]nnn", char_id)
	ServantData[2]	= GetServantStateData(data)
	ServantData[3] 	= ServantStateStoreObjData(data)
	return ServantData
end

local function DeleteAllServantData(data)
	local char_id = data["char_id"]
	StmtContainer.DeleteAllServantDBData:ExecSql( "", char_id)
	StmtContainer.DeleteAllServantMagicState:ExecSql("", char_id)
	StmtContainer.DeleteAllServantStateStoreObj:ExecSql("", char_id)
end

local function GetJfsPoint(data)
	local char_id	= data["char_id"]
	local JiFenSaiDB = RequireDbBox("JiFenSaiFbDB")
	return JiFenSaiDB.getPlayerPoint(char_id)
end
local function CChannelInit(data)
	local char_id	= data["char_id"]
	local channel_db = RequireDbBox("ChannelDB")
	return channel_db.SelectAllForbidChannel(char_id)
end

--根据角色ID获得其所在的队伍ID
local function GetPlayerTeamID(PlayerID)
	local team_box = RequireDbBox("GasTeamDB")
	return team_box.GetTeamID(PlayerID)
end

--得到装备信息
local function InitEquip(char_id)
	local g_EquipDbMgr = RequireDbBox("EquipMgrDB")
	local AllInfo = g_EquipDbMgr.GetPlayerEquipData(char_id)
	local EquipIntensifyDB = RequireDbBox("EquipIntensifyDB")
	local SuitInfoTbl = EquipIntensifyDB.GetSuitAttributeByPlayerID(char_id)
    local  EquipAdvanceDB = RequireDbBox("EquipAdvanceDB")
	local jingLingSkillInfo = EquipAdvanceDB.GetJingLingSkillInfo(char_id)
	local Tbl = 
	{
		["AllInfo"] = AllInfo,
		["SuitInfoTbl"] =  SuitInfoTbl, 
		["JingLingSkillInfo"] = jingLingSkillInfo
	}
	return Tbl
end

--得到普攻技能
local function GetCommonlyFightSkill(char_id)
	local FightSkillDB = RequireDbBox("FightSkillDB")
	local result = FightSkillDB.Dbs_LoadCommonlyFightSkill(char_id)
	local row = result:GetRowNum()
	if row == 0 then
		return 0
	end
	local name = result:GetData(0,0)
	local level = result:GetData(0,1)
	local kind 	= result:GetData(0,2)
	return  {name,level,kind}
end

--得到所有技能（除普攻）
local function g_GetFightSkill(char_id)
	local FightSkillDB = RequireDbBox("FightSkillDB")
	local skills = FightSkillDB.Dbs_LoadFightSkill(char_id)
	return skills
end

--得到快捷栏信息
local function GetShortcutInfo(char_id)
	local ShortCutDB = RequireDbBox("ShortcutDB")
	local shortCutRes =  ShortCutDB.GetShortcutInfo(char_id)
	return shortCutRes
end

--得到所有召唤兽的名字列表
local function g_GetAllServantNameTbl(char_id)
	local FightSkillDB = RequireDbBox("FightSkillDB")
	local NameRet = FightSkillDB.Dbs_SelectAllServantName(char_id)
	return NameRet
end

--得到聊天面板信息
local function GetAllPanel(data)
	local channel_db = RequireDbBox("ChannelDB")
	return channel_db.GetAllPanel(data)
end

--获取所有面板频道信息
local function GetAllChanelPanelInfo(data)
	local channel_db = RequireDbBox("ChannelDB")
	return channel_db.GetAllChanelPanelInfo(data)
end

local function TakeItemofStone(data)
	local lStoneSqr = RequireDbBox("StoneMgrDB")
	local tempdata = {}
	tempdata["player_id"] = data["char_id"]
	tempdata["tbl_new_items"] = data["tbl_HighStoneID"]
	lStoneSqr.TakeItem(tempdata)
end

----------------------------------------------------------------------------------------------------------
----@brief 上线加载关于阵法的一些信息
--local StmtDef = {
--			"QueryBattleShape",
--			--查询阵型信息
--			[[ 
--				select bs_uId,bs_sName,bs_uPos1, bs_uPos2,bs_uPos3, bs_uPos4,bs_uPos5,bs_uFlag  from tbl_battle_shape where cs_uId=?
--			]]
--}
--	DefineSql( StmtDef , StmtContainer )
--	
--	
--local StmtDef = {
--	"QueryBattleBookInfoByPlayerId",--查询阵型信息
--	[[
--		select
--			b.is_uId, b.ib_uPos1, b.ib_uPos2, b.ib_uPos3, b.ib_uPos4, b.ib_uPos5
--		from 
--			tbl_item_battlebook b, tbl_item_in_grid isr
--		where 
--			b.is_uId = isr.is_uId and isr.cs_uId = ?
--	]]
--}
--	DefineSql( StmtDef, StmtContainer )
--	
--local StmtDef = {
--			"QueryArrayBattle",
--			--查询列阵信息
--			[[ 
--				select bs_uId,ba_uLoc1,ba_uLoc2,ba_uLoc3,ba_uLoc4,ba_uLoc5 from tbl_battle_array where t_uId=?
--			]]
--}
--	DefineSql( StmtDef , StmtContainer )
--	
--local StmtDef = {
--			"QueryBattleSkil",
--			--查询阵法技能
--			[[ 
--				select bsi_uPos, bsi_sName from tbl_battle_skill_icon where cs_uId = ?
--			]]
--}
--	DefineSql( StmtDef , StmtContainer )
--	
----上线时加载阵法的所有信息
--
--local function LoadBattleInfo(PlayerID)
--	assert(IsNumber(PlayerID))
--	--向客户端发送所有阵型信息
--	local query_result = StmtContainer.QueryBattleShape:ExecSql("ns[32]nnnnnn",PlayerID)
--	
--	local teamID = GetPlayerTeamID(PlayerID)
--	local team_box = RequireDbBox("GasTeamDB")
--	local uCapatinID =  team_box.GetCaptain(teamID)
--	local query_list = nil
--	local bookinfo_list = nil
--
--	--向客户端发送玩家的所有列阵信息
--	if (PlayerID == uCapatinID) then
--		query_list = StmtContainer.QueryArrayBattle:ExecSql("nnnnnn",teamID)
--	end
--	
--	--向客户端发送阵法书的动态信息
--	local goods_list = StmtContainer.QueryBattleBookInfoByPlayerId:ExecSql("nnnnnn",PlayerID)
--	
--	--向客户端发送阵法技能栏的存储信息
--	local skill_list = StmtContainer.QueryBattleSkil:ExecSql("ns[32]",PlayerID)
--
--	local battletbl = {uCapatinID,query_result,query_list,goods_list,skill_list}
--	return battletbl
--end

--------------------
--上线通知小队和团队成员刷新面板
local function LoadTeamInfo(uCharID)
		local team = RequireDbBox("GasTeamDB")
		local AppTeamListDB = RequireDbBox("GasAppTeamListDB")
		
		local ATList = AppTeamListDB.judgeAppTeamList(uCharID)
		
		
		local TeamID = team.GetTeamID(uCharID)
		
		--得到小队所有成员和该小队队长
		local uCapatinID =  team.GetCaptain(TeamID)
		local eAM = team.GetAssignMode(TeamID) or 0
		local AuctionStandard = team.GetAuctionStandard(TeamID) or 0
		local AuctionBasePrice = team.GetAuctionBasePrice(TeamID) or 0
		local nIndex = 0
		local member_online = team.GetSomeMemberOnlineByTeamID(TeamID,uCharID)
		if uCapatinID ~= 0 and uCapatinID ~= uCharID and member_online == uCharID then
			--说明已经加入小队，且不是队长，且是第一个上线的成员，则该成员就转移为队长
			team.SetCaptain(uCharID,TeamID) --转移队长
			uCapatinID = uCharID
			nIndex = 1
		end
		
		local mark_box = RequireDbBox("GasTeamMarkDB")
		local MarkInfo = mark_box.GetAllMarkInfo(TeamID)
		
		local tbl = {}
		tbl["team_id"] = TeamID
		tbl["team_cap"] = uCapatinID
		tbl["eAM"] = eAM
		tbl["AuctionStandard"] = AuctionStandard
		tbl["AuctionBasePrice"] = AuctionBasePrice
		tbl["MarkInfo"] = MarkInfo
		tbl["nIndex"] = nIndex
		tbl["AppTeamList"] = ATList
		
		return tbl
end
------------------------------------------------------------------------------------------------------------------------------------
--下线通知好友
local function NotifyFriendOffline(uCharID)
	local FriendDB = RequireDbBox("FriendsDB")
	local friends = FriendDB.SelectOffLineFriendsInfo(uCharID)
	return friends
end

--下线通知好友群里面的成员
local function NotifyFriendGroupOffline(uCharID)
	local FriendGroupDB = RequireDbBox("FriendGroupDB")
	local groupfriends = FriendGroupDB.GetOnlineGroupMembers(uCharID)
	return groupfriends
end

local function InitAssociationOnline(uCharID,uChangeType)
	local tbl = {}
	local FriendDB		= RequireDbBox("FriendsDB")
	local FriendGroupDB	= RequireDbBox("FriendGroupDB")
	
	tbl.friendRequest			= FriendDB.SendOfflineFriendRequest(uCharID)	--上线后发送离线加为好友请求
	tbl.addGroupRequest			= FriendGroupDB.SendAddToGroupRequest(uCharID)	--上线后发送加为好友群的离线请求
	tbl.addGroupRequestToManger	= FriendGroupDB.SendAddToGroupRequestToManger(uCharID)
	tbl.msgtbl					= FriendDB.SelectOfflineMsg(uCharID)
	tbl.gmMsgTbl = FriendDB.SelectOfflineGMMsg(uCharID)
	FriendDB.DeletOfflineFriendRequest(uCharID)--上线后加为好友请求显示完毕后删除
	if uChangeType == 0 then
		tbl.friends					= FriendDB.SelectOffLineFriendsInfo(uCharID)	--上线通知好友
		tbl.groupfriends			= FriendGroupDB.GetOnlineGroupMembers(uCharID)			--上线通知好友群里面的成员
	end
	return tbl
end

---------------------------------------------------------------------------------------------------
local StmtDef = {
	"_SelectPlayerSoulNum",
	[[ select cs_uSoulNum from tbl_char_soul where cs_uId = ?]] 
}
DefineSql(StmtDef, StmtContainer)
--上线通知角色身上的魂池的魂值
local function NotifyPlayerSoulNum(uCharID)
	local playersoul = 0
	local playersoulset = StmtContainer._SelectPlayerSoulNum:ExecSql("n",uCharID)
	if playersoulset and playersoulset:GetRowNum() > 0 then
		playersoul = playersoulset:GetData(0,0)
	end
	return playersoul
end

local StmtDef = {
	"_UpdatePlayerSoulNum",
	[[ update tbl_char_soul set cs_uSoulNum = cs_uSoulNum + ? where cs_uId = ? and cs_uSoulNum + ? >= 0]] 
}
DefineSql(StmtDef, StmtContainer)
function CharacterMediatorDB.UpdateCharSoul(char_id,nSoulNum)
	StmtContainer._UpdatePlayerSoulNum:ExecStat(nSoulNum,char_id,nSoulNum)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		local GMCompenate = RequireDbBox("GMCompenate")
		return GMCompenate.AddUserEverydaySoulAndExp(char_id,nSoulNum,2)
	end
end

----------------------------------------------------------------------------------------------------

local StmtDef = {
	"_SelectBurstSoulTime",
	[[ select cs_uBurstSoulTimes from tbl_char_soul where cs_uId = ?]] 
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = {
	"_UpdateBurstSoulTime",
	[[ update tbl_char_soul set cs_uBurstSoulTimes = ? where cs_uId = ?]] 
}
DefineSql(StmtDef, StmtContainer)

local function GetBurstSoulTime(uCharID)
	local playersoulset = StmtContainer._SelectBurstSoulTime:ExecSql("n",uCharID)
	if playersoulset and playersoulset:GetRowNum() > 0 then
		return playersoulset:GetData(0,0)
	end
	return 0
end

local function SaveBurstSoulTime(uCharID,uValue)
	local playersoulset = StmtContainer._UpdateBurstSoulTime:ExecSql("",uValue,uCharID)
end

function CharacterMediatorDB.OnSaveBurstSoulTime(data)
	SaveBurstSoulTime(data["char_id"],data["BurstSoulTime"])
end
--------------------------------------------------------------------------------------------------------
--上线后发送玩家身上的积分
local StmtDef = {
	"_SelectPlayerPoint",
	[[ select afp_uPointType, afp_uPoint from tbl_area_fb_point where cs_uId = ?]] 
}
DefineSql(StmtDef, StmtContainer)

function CharacterMediatorDB.GetPlayerLevel(nCharID)
	local tbl = StmtContainer._GetPlayerLevel:ExecSql('n',nCharID)
	if tbl:GetRowNum() > 0 then
		return tbl:GetNumber(0,0)
	else
		return 0
	end
end

--上线后发送玩家身上的积分
local function ReturnPlayerPoint(uCharID)
	local playe_point = {}
	local playerpointset = StmtContainer._SelectPlayerPoint:ExecStat(uCharID)
	if playerpointset:GetRowNum() > 0 then
		for i = 1,playerpointset:GetRowNum() do
			playe_point[playerpointset:GetData(i-1,0)] = playerpointset:GetData(i-1,1)
		end 
	end
	
	local CTongBasicBox  = RequireDbBox("GasTongBasicDB")
	local para = {["uPlayerID"] = uCharID}
	local profferpoint = CTongBasicBox.GetProffer(para) 
	playe_point[#(playe_point)+1] = profferpoint
	
	--队贡
	local TeamProffer = CTongBasicBox.GetTongProfferByCharID(uCharID)
	playe_point[#(playe_point)+1] = TeamProffer
	
	return playe_point
end

--上线后发送玩家身上可领取的粮草数
local function ReturnForageNum(uCharID)
	local playe_point = {}
	local data = {}
	data["charId"] = uCharID
	
	local TongForageDB = RequireDbBox("GasTongForageDB")
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local flag, forageNum = TongForageDB.GetForageInfo(data)
	if not flag then
		local tong_id = CTongBasicBox.GetTongID(uCharID)
		if tong_id ~= 0 then
			TongForageDB.AddForageNum(data)
			forageNum = 50
		end
	end
	return forageNum
end

--------------------------------------------- Player Item操作--------------------------------------------------------------------
function CharacterMediatorDB.HaveItemCount(data)
	local nCharID = data["nCharID"]
	local nItemType = data["nItemType"]
	local nIndex = data["nIndex"]
	local nCount = data["nCount"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local uHaveCount = g_RoomMgr.GetItemCount(nCharID, nItemType,nIndex)
	if uHaveCount < nCount then
		return false
	end
	return true
end

function CharacterMediatorDB.AddItem(data)
	local char_id		= data["char_id"]
	local nType		= data["nType"]
	local sName		= data["sName"]
	local nCount		= data["nCount"]
	local CheckQuestNeed = data["CheckQuestNeed"]
	local createType	= data["createType"]
	local uEventId		= data["uEventId"]
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local checkres = RoleQuestDB.CheckNeedItemNum(char_id, nType, sName, CheckQuestNeed)
	if not checkres[1] then
		return checkres[2], nil
	end
	local params= {}
	params.nCharID = char_id
	params.nStoreMain = g_StoreRoomIndex.PlayerBag
	params.nBigID = nType
	params.nIndex = sName
	params.nCount = nCount
	params.createType = createType
	params.uEventId = uEventId
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local itemTbl = g_RoomMgr.AddItem(params)
	--data["itemTbl"] = itemTbl
	--local additemQuestVarTbl = RoleQuestDB.AddQuestItemVar(data)
	--return itemTbl,additemQuestVarTbl
	return itemTbl
end

function CharacterMediatorDB.DeleteItem(data) -- Delete Item By ID
	local char_id = data["char_id"]
	local item_id = data["item_id"]
	local lRoomMgr = RequireDbBox("GasRoomMgrDB")
	local pos_info = lRoomMgr.GetRoomIndexAndPosByItemId(char_id, item_id)
	if pos_info == nil then
		CancelTran()
		return false, nil
	end
	lRoomMgr.DelItemByID(char_id, item_id,event_type_tbl["物品使用"])
	if g_DbChannelMgr:LastAffectedRowNum() == 0 then
		CancelTran()
		return false, nil
	end
	local item_type, item_name = lRoomMgr.GetItemType(item_id)
	return true, pos_info
end

function CharacterMediatorDB.DelItem(data) --Delete nCount Item By Name 
	local nCharID = data["nCharID"]
	local nItemType = data["nItemType"]
	local nIndex = data["nIndex"]
	local nCount = data["nCount"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local uHaveCount = g_RoomMgr.GetItemCount(nCharID, nItemType,nIndex)
	if uHaveCount < nCount then
		return false
	end
	local res = g_RoomMgr.DelItem(nCharID,nItemType,nIndex,nCount,nil,event_type_tbl["物品使用"])
	return res
end
-----------------------------------------------  NonCombatValue  ---------------------------------
function CharacterMediatorDB.InitNonCombatValue(uCharID, uInspirationValue, uPrepExpValue)
	StmtContainer.InitNonCombatValue:ExecSql("",uCharID, uInspirationValue, uPrepExpValue)
end

function CharacterMediatorDB.AddInspiration(char_id, uPoint)
	local result = StmtContainer.GetInspiration:ExecSql("n",char_id)
	local row = result:GetRowNum()
	if row == 0 or result:GetData(0,0) + uPoint < 0 then
		return
	elseif result:GetData(0,0) + uPoint > 2000 then
		uPoint = 2000 - result:GetData(0,0)
	end
	StmtContainer.AddInspiration:ExecSql("",uPoint,char_id)
end

function CharacterMediatorDB.GetInspiration(uCharID)
	local result = StmtContainer.GetInspiration:ExecSql("n",uCharID)
	local row = result:GetRowNum()
	if row == 0 then
		return 0
	end
	local InspirationValue = result:GetData(0,0)
	return  InspirationValue
end

function CharacterMediatorDB.AddPrepExp(char_id, uPoint)
	local result = StmtContainer.GetPrepExp:ExecSql("n",char_id)
	local row = result:GetRowNum()
	if row == 0 or result:GetData(0,0) + uPoint < 0 then
		return
	end
	StmtContainer.AddPrepExp:ExecSql("",uPoint,char_id)
end

function CharacterMediatorDB.GetPrepExp(uCharID)
	local result = StmtContainer.GetPrepExp:ExecSql("n",uCharID)
	local row = result:GetRowNum()
	if row == 0 then
		return 0
	end
	local PrepExpValue = result:GetData(0,0)
	return  PrepExpValue
end

local function UpdateNonCombat(char_id)
	local OffLineTimeRet 	= StmtContainer._GetOffLineTime:ExecSql("n",char_id)
	local Lev_ExpRet 		= StmtContainer._GetCharLevelExp:ExecSql('nnnn',char_id,char_id,char_id)
	if OffLineTimeRet:GetRowNum() == 0 or Lev_ExpRet:GetRowNum() == 0 then
		return
	end
	local OffLineTime 	= OffLineTimeRet:GetData(0,0)
	if OffLineTime <= 0 then
		return
	end
	local Level 		= Lev_ExpRet:GetData(0,0)
	local uInspirationValue 	= math.floor( OffLineTime*10/3600 ) --以后可以用道具改变增长速度
	local uPrepExpValue 	= math.floor( 0.5*(50+Level*3)*360*OffLineTime/3600 )
	
	local result = StmtContainer.GetInspiration:ExecSql("n",char_id)
	local row = result:GetRowNum()
	if row == 0 then
		return
	elseif result:GetData(0,0) + uInspirationValue > 2000 then
		uInspirationValue = 2000 - result:GetData(0,0)
	end
	StmtContainer.AddInspiration:ExecSql("",uInspirationValue,char_id)
	StmtContainer.AddPrepExp:ExecSql("",uPrepExpValue,char_id)
end

function CharacterMediatorDB.GetInspirationExp(char_id, uAddExp)
	local InspirationRet = StmtContainer.GetInspiration:ExecSql("n",char_id)
	local row = InspirationRet:GetRowNum()
	if row == 0 then
		return 0
	end
	local InspirationValue = InspirationRet:GetData(0,0)
	if InspirationValue/1000 < math.random(0, 1) or InspirationValue < 2 then
		return 0
	end
	local PrepExpRet = StmtContainer.GetPrepExp:ExecSql("n",char_id)
	local row = PrepExpRet:GetRowNum()
	if row == 0 then
		return 0
	end
	local PrepExpValue = PrepExpRet:GetData(0,0)
	local InspirationExp = 0	--额外经验
	InspirationExp = math.floor(uAddExp)*(2+math.floor(InspirationValue/500))
	if PrepExpValue < 1 then
		return 0			--提示玩家，预存经验不足
	elseif PrepExpValue < InspirationExp then
		InspirationExp = PrepExpValue
	end
	local uDelInspiration = 0-(2+math.floor(InspirationValue/500)) --灵感值小于2时，概率触发的话，会数据越界
	StmtContainer.AddInspiration:ExecSql("",uDelInspiration,char_id)
	StmtContainer.AddPrepExp:ExecSql("",-InspirationExp,char_id)
	return InspirationExp
end
--------------------------------------------------------------------------------------------------------
function CharacterMediatorDB.AddLevel(data)
	local char_id	= data["char_id"]
	local char_level	= data["char_level"]
	local char_exp	= data["char_exp"]
	local nAddExp	= data["nAddExp"]
	local addExpType = data["addExpType"]
	local uEventId = data["uEventId"]
	local LimitLevel
	
	--佣兵等级限制人物等级
	--local MercenaryLevelDB = RequireDbBox("MercenaryLevelDB")
	--local param = {}
	--param["CharId"] = char_id
	--local MercenaryLevel = MercenaryLevelDB.GetMercenaryLevelInfo(param)
	--local FinishAwards = MercenaryLevelDB.GetMercenaryLevelAward(param)
	--MercenaryLevel = MercenaryLevel or 0
	--if FinishAwards["开放等级"] then
	--	if g_MercenaryLevelTbl[MercenaryLevel]["特权"]["开放等级"] then
	--		LimitLevel = g_MercenaryLevelTbl[MercenaryLevel]["特权"]["开放等级"].Arg
	--	else
	--		LimitLevel = MaxLevel
	--	end
	--else
	--	if MercenaryLevel==0 or g_MercenaryLevelTbl[MercenaryLevel-1]["特权"]["开放等级"] then
	--		LimitLevel = (MercenaryLevel==0 and 10) or g_MercenaryLevelTbl[MercenaryLevel-1]["特权"]["开放等级"].Arg
	--	else
	--		LimitLevel = MaxLevel
	--	end
	--end
	
	LimitLevel = MaxLevel--math.min(LimitLevel, MaxLevel)
	if char_level > LimitLevel then
		char_level = LimitLevel
	end
	
	local Level = CharacterMediatorDB.GetPlayerLevel(char_id)
	if nAddExp == 0 and char_level == Level then
		return
	end
	local GMCompenate = RequireDbBox("GMCompenate")
	if not GMCompenate.AddUserEverydaySoulAndExp(char_id,nAddExp,1) then
		CancelTran()
		return
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	if char_level == Level then
		StmtContainer._UpdatePlayerExperience:ExecStat(char_exp,char_id)
		if g_DbChannelMgr:LastAffectedRowNum() == -1 then
			error("加经验失败，最后影响行数是-1")
		end
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("加经验失败")
		end
	else
		StmtContainer.Player_SaveLevel:ExecSql( "",char_level,char_exp,char_id)
		if g_DbChannelMgr:LastAffectedRowNum() == -1 then
			error("加经验失败，最后影响行数是-1")
		end
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			error("加经验失败")
		end
		if char_level >= 20 then
			local parameter = {}
			parameter["charId"] = data["char_id"]
			local CTongForageBox = RequireDbBox("GasTongForageDB")
			CTongForageBox.AddForageNum(parameter)
		end
		--角色升级记录log
		g_LogMgr.PlayerLevelUpLog( char_id, char_level)
	end
	
	--记录加经验log
	local succ = g_LogMgr.LogPlayerAttribute( char_id, char_level, nAddExp,addExpType,char_exp,uEventId )
	if not succ then
		error("记录加经验log失败")
	end
	
	if char_level ~= Level then
		--清除决斗信息
		local minLevelKeys = DuelContWinAppellation_Common:GetKeys()
		for _, minLevel in pairs(minLevelKeys) do
			if char_level == tonumber(minLevel) then
				StmtContainer.updateDuelWinCount:ExecStat(0, char_id)
				return
			end
		end
	end
	
end

function CharacterMediatorDB.AddMercenaryLevel(data)
	local char_id	= data["char_id"]
	local char_mercLevel	= data["char_mercLevel"]
	local char_integral	= data["char_mercIntegral"]
	StmtContainer.Player_SaveMercenaryLevel:ExecSql( "",char_mercLevel,char_integral,char_id)
end

--得到等级，经验，佣兵等级，佣兵积分
function CharacterMediatorDB.GetCharLevelExpDB(data)
	return GetCharLevelExp(data["char_id"])
end

function CharacterMediatorDB.SetCamp(data)
	local char_id	= data["char_id"]
	local camp	= data["camp"]
	StmtContainer.DbSetCamp:ExecSql("",camp,char_id)
end

function CharacterMediatorDB.GetCamp(char_id)
	local res = StmtContainer._GetPlayerCamp:ExecSql("n",char_id)
	return res:GetNumber(0,0)
end

function CharacterMediatorDB.GetCampById(data)
	local charId = data["charId"]
	local res = StmtContainer._GetPlayerCamp:ExecSql("n",charId)
	return res:GetData(0,0)
end

function CharacterMediatorDB.GetCharPosDB(data)
	return GetCharPos(data)
end

function CharacterMediatorDB.GetCharReScenePos(data)
	return GetCharRePos(data)
end

function CharacterMediatorDB.GetCharSceneNamePos(data)
	return GetCharNamePos(data)
end

function CharacterMediatorDB.OnSaveFightPropertyTick(data)
	DeleteStateFromDB(data)
	SaveStateToDBBegin(data["char_id"],data["StateData"],data["StateStoreObjData"],0)
	SaveCharFightInfo(data)
end

function CharacterMediatorDB.OnSaveSceneToDB(data)
	return OnSaveSceneToDB(data)
end

function CharacterMediatorDB.OnSaveFBToDB(data)
	return OnSaveFBToDB(data)
end

function CharacterMediatorDB.ExitAndGoBack(data)
	OnSaveToDB(data)
	OnSaveSceneToDB(data)
end

function CharacterMediatorDB.SetChangeToPos(data)
	local char_id	= data["char_id"]
	local posCur_x	= data["posCur_x"]
	local posCur_y	= data["posCur_y"]
	local scene_id	= data["scene_id"]
	StmtContainer.SetChangeToPos:ExecSql( "", char_id, scene_id,  posCur_x, posCur_y)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function CharacterMediatorDB.DelFbPosition(data)
	StmtContainer.Player_DelFbPos:ExecSql( "", data["char_id"])
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end
-------------------------------------------------------------------------
function CharacterMediatorDB.CreateServerPlayer(data)
	local char_id = data["char_id"]
	local state = ""
	
	local DelaySaveRet = StmtContainer.GetDelaySaveInfo:ExecStat(char_id)
	
	if DelaySaveRet:GetRowNum() > 0 then --有延时保存的数据还没存盘
--		print"有延时保存的数据还没存盘"
		local serverId, dataId, addTime  = DelaySaveRet:GetData(0,0), DelaySaveRet:GetData(0,1), DelaySaveRet:GetData(0,2)
		local elapseTime = os.time() - addTime
		if data["DelayDataId"] then --本服务器有其他服务器传过来的数据
--			print"本服务器有其他服务器传过来的数据"
			if data["DelayDataId"] ~= dataId or data["SrcServerId"] ~= serverId then --本服务器的消息是失效的
--				print("数据id 不同 本服务器的消息是失效的", data["DelayDataId"], dataId, data["SrcServerId"], serverId)
				Db2GasCall("ForceSaveDelayData", {char_id}, serverId)
				return "ForceSave"
			end
			
		else
--			print"本服务器 无! 其他服务器传过来的数据"
			if elapseTime > 120 then --超时
				local addDate = os.date("%Y-%m-%d %H:%M:%S",addTime)
				LogErr("角色切场景数据延迟存盘超时, 可能是有服务器宕机", "数据所在服务器:" .. serverId .. " 数据创建时间:" .. addDate)
				StmtContainer.DeleteDelaySaveInfo:ExecStat(char_id)
			else
				Db2GasCall("ForceSaveDelayData", {char_id}, serverId)
			end	
			return "ForceSave"
		end

	else
--		print"无! 延时保存的数据还没存盘"
		if data["DelayDataId"] then
--			print"本服务器有其他服务器传过来的数据,  失效" 
			state = "DisabledData"
		end
	end
	
	local RoleQuestDB 				= RequireDbBox("RoleQuestDB")
	local NoviceDirectDB 			= RequireDbBox("NoviceDirectDB")
	local GameSettingDB 			= RequireDbBox("GameSettingDB")
	local SceneMgrDb 				= RequireDbBox("SceneMgrDB")
	local GasAppellationAndMoralDB 	= RequireDbBox("GasAppellationAndMoralDB")
	local CollectMercCardDB 		= RequireDbBox("CollectMercCardDB")
	local UseSpecialItemDB 			= RequireDbBox("UseSpecialItemDB")
	local GatherLiveSkillDB 		= RequireDbBox("GatherLiveSkillDB")
	local ActivityCountDbBox 		= RequireDbBox("ActivityCountDB")
	local MercenaryLevelDB 			= RequireDbBox("MercenaryLevelDB")
	local KeyMapDB 					= RequireDbBox("KeyMapDB")
	local LiveSkillDB 				= RequireDbBox("LiveSkillBaseDB")
	local DirectDB					= RequireDbBox("DirectDB")
	local StoneDB 					= RequireDbBox("StoneMgrDB")
	local ExpOrSoulStorageDB		= RequireDbBox("ExpOrSoulStorageDB")
	local GasTongbuildingDB 		= RequireDbBox("GasTongbuildingDB")
	local ItemBagLockDB				= RequireDbBox("ItemBagLockDB")
	local GasFightingEvaluationDB = RequireDbBox("FightingEvaluationDB")
	local RecruitPlayerDB = RequireDbBox("RecruitPlayerDB")
	local ActivityCountDB		= RequireDbBox("ActivityCountDB")
	local TongTeachDB              = RequireDbBox("TongTeachDB")
	local UseVIPItemDB              = RequireDbBox("UseVIPItemDB")
	
	CharacterMediatorDB.CopyChangeToPos(data)
	local result = {}
	
	result["CharInfo"] 			= GetCharCreateInfo(data)
	result["MainSceneInfo"] 	= CharacterMediatorDB.GetMainScenePositionInfo(data)
	result["FbSceneInfo"] 		= CharacterMediatorDB.GetFbScenePositionInfo(data)
	result["ShortcutInfo"] 		= GetShortcutInfo(char_id)
	result["MoneyInfo"]					= GetCharMoneyInfo(data)
	if not (data["ChangeType"] == 1 and data["DelayDataId"] and state == "") then --切场景才不要查询
		result["ItemInfo"] 					= GetItemInfo(data)
		result["BagInfo"] 					= GetCharBagInfo(data)
		-- 活动提示面板
		result["ActivityCountTbl"]	 		= ActivityCountDbBox.GetAllActivityCount(data)
		result["AllPanel"]					= GetAllPanel(data)
		result["AllChanelPanelInfo"]		= GetAllChanelPanelInfo(data)
		result["AllAppellationInfo"]		= GasAppellationAndMoralDB.GetCharAllAppellationInfo(data)
		result["QuestAwardItemIndex"] 		= RoleQuestDB.GetAllQuestAwardItemSql(char_id)
		result["QuestLoopTbl"] 				= RoleQuestDB.GetLoopQuestState(char_id)
		result["QusetSortSetting"] 		= GameSettingDB.GetOneSettingInfo(char_id,7)
		result["QuestVarTbl"]			= RoleQuestDB.GetAllQuestVar(char_id)
		result["QuestAllTbl"]			= RoleQuestDB.GetAllQuestSql(char_id)
		result["TongType"]						= GetTongType(char_id)
		result["forageNum"] 			= ReturnForageNum(char_id)
		result["StoneHoleInfo"]				= StoneDB.GetAllHoleOpened(char_id)
		result["skillCoolDownData"]		= GetSkillCoolDownTime(data)
		result["AureMagicData"]				= GetAureMagicData(data)
		result["StateData"]						= GetPlayerStateData(data)
		result["StateStoreObjData"] 	= GetPlayerStateStoreObjData(data)
		result["BurstSoulTime"] 			= GetBurstSoulTime(data["char_id"])
		result["AcceleratorKeyTbl"] 	= KeyMapDB.GetAllKeyMap(data)
		result["ServantData"]					= GetServantDataFromDB(data)
		result["IsShowChatWnd"]  = NoviceDirectDB.GetPlayerFirstTimeCount(data["char_id"],"世界频道聊天")
		result["IsShowCHItemChatWnd"]  = NoviceDirectDB.GetPlayerFirstTimeCount(data["char_id"],"传声频道发消息消耗物品")
		result["IsShowCHMoneyChatWnd"]  = NoviceDirectDB.GetPlayerFirstTimeCount(data["char_id"],"传声频道发消息扣钱")
		local UISettingDB				= RequireDbBox("UISettingDB")
		result["UISettingInfo"]			= UISettingDB._GetUISettingInfoDB(data["account_id"])
		result["DirectInfo"]			= DirectDB.GetPlayerAllDirectInfo(data["char_id"])
		result["DirectAwardInfo"]		= DirectDB.GetPlayerAllDirectAward(data["char_id"])
		result["teamInfo"] 				= LoadTeamInfo(char_id)
		result["ArmyCorpsPos"] = GetPlayerArmyCorpsPos(char_id)
		local NpcShopDbBox              =   RequireDbBox("NpcShopDB")
		result["NpcShopBuyTipInfo"]     =  NpcShopDbBox.GetShopTipFlagInfo(data["char_id"]) 
		result["TongBuildTbl"]					=		GasTongbuildingDB.GetAllBuildName(char_id)
		result["recruitFlag"] = RecruitPlayerDB.GetRecruitCharInfo(data)
		result["createTime"] = ActivityCountDB.GetCharacterCreateData(char_id)
	end
	
	if data["ChangeType"] == 1 and not result["QuestAllTbl"] then
		result["QuestAllTbl"]			= RoleQuestDB.GetDoingQuestSql(char_id)
	end
	result["EquipEffectInfo"] = GasFightingEvaluationDB.SelectEquipEffectDB(data)
	result["ActivationBottle"]		= ExpOrSoulStorageDB.QueryEnabledSoulBottle(char_id)
	result["IsForbidLogin"] 		= false
	result["EquipInfo"]				= InitEquip(char_id)
	result["StoneInlayedInfo"]= StoneDB.GetAllInlayedHolesInfo(char_id)
	result["now_panel"]				= StoneDB.GetNowPanel(char_id)
	result["CommonlyFightSkill"] 	= GetCommonlyFightSkill(char_id)
	result["FightSkill"] 			= g_GetFightSkill(char_id)
	result["ServantNameInfo"]		= g_GetAllServantNameTbl(char_id)
	result["TeamID"]				= GetPlayerTeamID(char_id)
	result["TongID"],result["TongName"],result["TongPos"]		= GetPlayerTongID(char_id)
	result["CChannelInitRes"]		= CChannelInit(data)
	--result["battletbl"] 			= LoadBattleInfo(char_id)
	result["playersoul"] 			= NotifyPlayerSoulNum(char_id)
	result["playe_point"] 			= ReturnPlayerPoint(char_id)
	result["FirstTimeInfo"] 		= NoviceDirectDB.GetPlayerFirstTimeInfo(char_id)
	result["playerAppellationInfo"] = GasAppellationAndMoralDB.GetPlayerAppellationInfo(data)
	result["MercCardInfo"] 			= CollectMercCardDB.QueryAllMercCard(char_id)
	result["SpecialItemCoolDown"] 	= UseSpecialItemDB.GetItemCoolDownInfo(char_id)
	result["FlowerInfo"]			= GatherLiveSkillDB.QueryFlowerInfo(data["char_id"])
	--result["MLRes"], result["MLAwards"] = MercenaryLevelDB.InitMercenaryLevel(char_id)
	result["AssociationInfo"] 		= InitAssociationOnline(char_id,data["ChangeType"])
	result["LiveskillHavingLearn"] 	= LiveSkillDB.GetAllSkillsInfo(data["char_id"])
	result["ArmyCorpsID"],result["ArmyCorpsName"] = GetPlayerArmyCorpsName(char_id)
	result["ItemLockTime"] = ItemBagLockDB.JudgeItemBagLock(data)
	result["TongTechInfo"]  = TongTeachDB.InitTongTechByCharID(char_id)
	result["VipEffectInfo"] = UseVIPItemDB.GetVIPEffect(data)
	if data["ChangeType"] ~= 1 then
		local ClientMsgDB = RequireDbBox("ClientMsgDB")
		ClientMsgDB.ClearRecordById(char_id)
	end
	return state, result
end

local StmtDef = 
{
	"_GetPlayerCount",
	"select count(*) from tbl_user_online where uo_uOnServerId = ?"
}
DefineSql( StmtDef , StmtContainer )

local StmtDef = 
{
	"_GetOnlineCharByServerId",
	"select cs_uId from tbl_char_online where co_uOnServerId = ?"
}
DefineSql( StmtDef , StmtContainer ) 

function CharacterMediatorDB.OnSaveAllPlayerJobEnd(data)
	local res = StmtContainer._GetOnlineCharByServerId:ExecStat(g_CurServerId)
	for i = 0, res:GetRowNum() -1 do
		CharacterMediatorDB.LogLogout(res:GetData(i,0))
	end
end

function CharacterMediatorDB.GetUserOnServerId(CharId)
	local res = StmtContainer.GetUserOnServerId:ExecStat(CharId)
	if res:GetRowNum() > 0 then
		return res:GetData(0,0)
	end
end

function CharacterMediatorDB.CharUpdateOnServerPos(data)
	local CharId = data["char_id"]
	StmtContainer.DeleteDelaySaveInfo:ExecStat(CharId)
 	StmtContainer._SaveCharOnlineInfo:ExecStat(CharId, g_CurServerId)
 	local oldServerId = CharacterMediatorDB.GetUserOnServerId(CharId)
 	StmtContainer.UpdateUserOnServerId:ExecSql("", g_CurServerId, CharId)
	local LoginQueueDB = RequireDbBox("LoginQueueDB")
--	LoginQueueDB.UpdateWaitQueue(data)
	local LogoutSql = RequireDbBox("LogoutServerDB")
	local FriendGroupDB				= RequireDbBox("FriendGroupDB")
 	local friendGroupIdTbl = FriendGroupDB.GetFriendGroupId(CharId)
 	local team = RequireDbBox("GasTeamDB")
	return team.GetTeamID(CharId), oldServerId,friendGroupIdTbl
end

local function LoginGameInitAssociationInfo(uCharID)
	local tbl = {}
	local FriendDB		= RequireDbBox("FriendsDB")
	local FriendGroupDB	= RequireDbBox("FriendGroupDB")
	
	tbl.friendClassList = FriendDB.GetAllFriendsClass(uCharID)	
	tbl.friendsInfo = FriendDB.SelectFriendInfo(uCharID)		
	local tblTemp = {["uPlayerID"] = uCharID}
	tbl.playerTbl = FriendDB.SelectPersonalInfo(tblTemp)
	tbl.friendGroupIdTbl = FriendGroupDB.GetFriendGroupId(uCharID)
	return tbl
end


--离线经验相关
local StmtDef =
{
	"_InSertPlayerToOffLineExpTbl",
	"insert into tbl_char_off_line_exp(cs_uId, cm_uOfflineExp) values(?,?)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_GetPlayerOffLineExp",
	"select cm_uOfflineExp from tbl_char_off_line_exp where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateOffLineExp",
	"update tbl_char_off_line_exp set cm_uOfflineExp = ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_InsertOffLineExchangeDayLimitTbl",
	[[
		insert into 
			tbl_char_off_line_exchage_limit_value
		set cs_uId = ?,
			cd_sLastExChangeDataTime = ?,
			cm_uAfflatusValue = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_GetCurDayOffLineExchangeValues",
	"Select cm_uAfflatusValue from tbl_char_off_line_exchage_limit_value where cs_uId = ? and cd_sLastExChangeDataTime = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_DeletePlayerOffLineExpInfo",
	"delete from tbl_char_off_line_exchage_limit_value where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local function UpdatePlayerOffLineExp(char_id, uIncreaseOffLineExp)
	local CurOffLineExpRet 	= StmtContainer._GetPlayerOffLineExp:ExecStat(char_id)
	local uCurOffLineExp = 0
	--如果不存在这个玩家则插入该玩家
	if CurOffLineExpRet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerToOffLineExpTbl:ExecStat(char_id, 0)
	else
		uCurOffLineExp = CurOffLineExpRet:GetNumber(0,0)		
	end
	
	local tbl = StmtContainer._GetPlayerLevel:ExecStat(char_id)
	if tbl:GetRowNum() == 0 then 
		return
	end
	local uLevel = tbl:GetNumber(0,0)
	local UseVIPItemDB              = RequireDbBox("UseVIPItemDB")
	local param = {["char_id"] = char_id}
	local vipEffectInfo = UseVIPItemDB.GetVIPEffect(param)
	if vipEffectInfo > 0 then
		uIncreaseOffLineExp = uIncreaseOffLineExp * 2
	end
	--不能超过最大离线经验值
	local uMaxOffLineExp		= LevelExp_Common(uLevel, "OffLineExpLimit")
	local uTotalOffLineExp 		= uIncreaseOffLineExp + uCurOffLineExp
	if uTotalOffLineExp > uMaxOffLineExp then
		uTotalOffLineExp = uMaxOffLineExp
	end
	StmtContainer._UpdateOffLineExp:ExecStat(uTotalOffLineExp, char_id)	
end

local function AddPlayerOffLineExp(char_id)

	--小于一小时不计算
	local OffLineTimeRet 	= StmtContainer._GetOffLineTime:ExecStat(char_id)
	if OffLineTimeRet:GetRowNum() == 0 or OffLineTimeRet:GetNumber(0,0) < 60*60 then
		return
	end
	local uOffLineTime = uint32(OffLineTimeRet:GetNumber(0,0)/3600) --转换成小时

	local tbl = StmtContainer._GetPlayerLevel:ExecStat(char_id)
	if tbl:GetRowNum() == 0 then 
		return
	end
	local uLevel = tbl:GetNumber(0,0)
	
	--离线经验增加公式：（玩家当前等级*3+50）*360*离线时间*0.3  单位是小时
	local uIncreaseOffLineExp 	= (uLevel*3+50)*360*uOffLineTime*0.3
	UpdatePlayerOffLineExp(char_id, uIncreaseOffLineExp)
end

function CharacterMediatorDB.AddOffLineExpValue(data)
	local char_id 			= data["char_id"]
	local uOffLineExpValue	= data["uOffLineExpValue"]
	UpdatePlayerOffLineExp(char_id, uOffLineExpValue)
end

--灵感值相关
local StmtDef =
{
	"_InSertPlayerAfflatusValueTbl",
	"insert into tbl_char_afflatus_value(cs_uId, cm_uAfflatusValue) values(?,?)"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef = 
{
	"_GetPlayerAfflatusValue",
	"select cm_uAfflatusValue from tbl_char_afflatus_value where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_UpdateAfflatusValue",
	"update tbl_char_afflatus_value set cm_uAfflatusValue = ? where cs_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

function CharacterMediatorDB.AddPlayerAfflatusValue(data)
	local char_id 			= data["char_id"]
	local uAfflatusValue 	= data["uAfflatusValue"]
	local LevelResSet = StmtContainer._GetPlayerLevel:ExecStat(char_id)
	if LevelResSet:GetRowNum() == 0 then 
		return false
	end
	local uLevel = LevelResSet:GetNumber(0,0)
	local uMaxValueLimit = LevelExp_Common(uLevel, "AfflatusValue")
	
	local uCurAfflatusValue = 0
	local CurAfflatusValueSet = StmtContainer._GetPlayerAfflatusValue:ExecStat(char_id)
	if CurAfflatusValueSet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerAfflatusValueTbl:ExecStat(char_id, 0)
	else
		uCurAfflatusValue = CurAfflatusValueSet:GetNumber(0,0)
	end
	
	if uCurAfflatusValue == uMaxValueLimit then
		return false,350009
	end
	local uTotalValue = uCurAfflatusValue + uAfflatusValue
	if uTotalValue > uMaxValueLimit then
		uTotalValue = uMaxValueLimit
	end
	StmtContainer._UpdateAfflatusValue:ExecStat(uTotalValue, char_id)
	return true
end

local function GetAfflatusValue_OffLineExp_MoneyInfo(data)
	local char_id = data["char_id"]
	local uCurOffLineExp = 0
	local uCurAfflatusValue = 0
	
	local CurOffLineExpRet = StmtContainer._GetPlayerOffLineExp:ExecStat(char_id)
	if CurOffLineExpRet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerToOffLineExpTbl:ExecStat(char_id, 0)
	else
		uCurOffLineExp = CurOffLineExpRet:GetNumber(0,0)
	end
	
	local CurAfflatusValueSet = StmtContainer._GetPlayerAfflatusValue:ExecStat(char_id)
	if CurAfflatusValueSet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerAfflatusValueTbl:ExecStat(char_id, 0)
	else
		uCurAfflatusValue = CurAfflatusValueSet:GetNumber(0,0)
	end
	
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local uMoney = MoneyManagerDB.GetMoney(char_id)
	
	local ResTbl = {}
	ResTbl["uCurOffLineExp"] 		= uCurOffLineExp
	ResTbl["uCurAfflatusValue"]		= uCurAfflatusValue
	ResTbl["uMoney"]				= uMoney
	
	return ResTbl
end

--离线经验，灵感值，金钱操作相关
function CharacterMediatorDB.GetAfflatusValue_OffLineExp_MoneyInfo(data)
	local ResTbl = GetAfflatusValue_OffLineExp_MoneyInfo(data)
	return ResTbl
end

function CharacterMediatorDB.GetOffLineExp(char_id)
	local uCurOffLineExp = 0						--当前离线经验
	
	--获取当前的总离线经验
	local CurOffLineExpRet = StmtContainer._GetPlayerOffLineExp:ExecStat(char_id)
	if CurOffLineExpRet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerToOffLineExpTbl:ExecStat(char_id, 0)
	else
		uCurOffLineExp = CurOffLineExpRet:GetNumber(0,0)
	end
	return uCurOffLineExp
end

function CharacterMediatorDB.ExChangeAfflatusValue(data)
	local char_id = data["char_id"]
	local uCurOffLineExp = 0						--当前离线经验
	local uCurAfflatusValue = 0						--当前灵感值
	local uCurExchangedOffLineExpToday = 0			--今天与已经交换的灵感值
	
	--获取当前的总离线经验
	local CurOffLineExpRet = StmtContainer._GetPlayerOffLineExp:ExecStat(char_id)
	if CurOffLineExpRet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerToOffLineExpTbl:ExecStat(char_id, 0)
	else
		uCurOffLineExp = CurOffLineExpRet:GetNumber(0,0)
	end
	
	-- 获取当前总的灵感值
	local CurAfflatusValueSet = StmtContainer._GetPlayerAfflatusValue:ExecStat(char_id)
	if CurAfflatusValueSet:GetRowNum() == 0 then
		StmtContainer._InSertPlayerAfflatusValueTbl:ExecStat(char_id, 0)
	else
		uCurAfflatusValue = CurAfflatusValueSet:GetNumber(0,0)
	end
	
	--获取今天已经交换的离线经验
	local CurExchangedOffLineSet = StmtContainer._GetCurDayOffLineExchangeValues:ExecStat(char_id, os.date("%x"))
	if CurExchangedOffLineSet:GetRowNum() ~= 0 then
		uCurExchangedOffLineExpToday = CurExchangedOffLineSet:GetNumber(0,0)
	end
	
	--获取当前金钱
	local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
	local uCurMoney = MoneyManagerDB.GetMoney(char_id)				--当前money
	
	local uExchgAfflatusValue 	= data["ExchgValue"]						--需要交换的灵感值
	local uExChgOffLineExp 		= 5000*uExchgAfflatusValue					--需要交换的离线经验
	local uExChgMoney			= 100*uExchgAfflatusValue					--需要消耗的金钱	
	
	local ResTbl = {}
	
	local LevelResSet = StmtContainer._GetPlayerLevel:ExecStat(char_id)
	if LevelResSet:GetRowNum() == 0 then 
		return
	end
	local uLevel = LevelResSet:GetNumber(0,0)
	local uMaxOffLineExpLimit	= LevelExp_Common(uLevel, "MaxOffLineExpOneDay")
	
	if uExchgAfflatusValue > uCurAfflatusValue then
		ResTbl["bSuccess"] 	= false
		ResTbl["ErrMsg"] 	= 350003
		return ResTbl
	elseif uExChgOffLineExp > uCurOffLineExp then
		ResTbl["bSuccess"]	= false
		ResTbl["ErrMsg"]	= 350001
		return ResTbl	
	elseif uExChgMoney > uCurMoney then
		ResTbl["bSuccess"] 	= false
		ResTbl["ErrMsg"]	= 350002
		return ResTbl
	elseif (uCurExchangedOffLineExpToday + uExChgOffLineExp) > uMaxOffLineExpLimit then
		ResTbl["bSuccess"] 	= false
		ResTbl["ErrMsg"]	= 350008
		return ResTbl
	end
	
	local uLeftOffLineExp	 		= uCurOffLineExp - uExChgOffLineExp
	local uLeftAfflatusValue		= uCurAfflatusValue - uExchgAfflatusValue
	local uLeftMoney				= uCurMoney - uExChgMoney
	local LogMgr = RequireDbBox("LogMgrDB")
	local uEventId = LogMgr.ExpChangeLog()
	local MoneyManagerDB=	RequireDbBox("MoneyMgrDB")
	local fun_info = g_MoneyMgr:GetFuncInfo("Afflatus")
	local bFlag,uMsgID = MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["ExChgMoney"],char_id, -uExChgMoney,uEventId)
	if not bFlag then
		ResTbl["bSuccess"] 	= false
		ResTbl["ErrMsg"]	= uMsgID
		return ResTbl
	end
	
	StmtContainer._UpdateOffLineExp:ExecStat(uLeftOffLineExp, char_id)
	StmtContainer._UpdateAfflatusValue:ExecStat(uLeftAfflatusValue, char_id)
	
	--记录今天已经交换的离线经验
	StmtContainer._DeletePlayerOffLineExpInfo:ExecStat(char_id)
	StmtContainer._InsertOffLineExchangeDayLimitTbl:ExecStat(char_id, os.date("%x"), uCurExchangedOffLineExpToday + uExChgOffLineExp)
	
	--低速添加经验
	local AddExpTbl	= {}
	AddExpTbl["char_id"]			= char_id
	AddExpTbl["addExp"]				= uExChgOffLineExp
	AddExpTbl["uEventId"]			= uEventId
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local uCurLevel, uCurExp = RoleQuestDB.AddExp(AddExpTbl)
	
	ResTbl["bSuccess"] 				= true
	ResTbl["uLeftOffLineExp"] 		= uLeftOffLineExp
	ResTbl["uLeftAfflatusValue"]	= uLeftAfflatusValue
	ResTbl["uLeftMoney"]			= uLeftMoney
	ResTbl["uCurLevel"]				= uCurLevel
	ResTbl["uCurExp"]				= uCurExp
	ResTbl["uAddExp"]				= uExChgOffLineExp
	ResTbl["uExChgMoney"]		= uExChgMoney
	return ResTbl
end

-----------------------------------------------------------------------------
--角色既然场景后获取同步信息
function CharacterMediatorDB.CharLoginGame(data)
	local result = {}
	local CharId = data["char_id"]
	StmtContainer.DeleteDelaySaveInfo:ExecStat(CharId)
	apcall(DeleteSkillCoolDownTime,data)
	apcall(DeleteStateFromDB,data)
	apcall(DeleteAureMagicData,data)
	apcall(UpdateNonCombat,CharId)
	StmtContainer.UpdateCharLoginTime:ExecStat(CharId)
 	StmtContainer._SaveCharOnlineInfo:ExecStat(CharId,g_CurServerId)
	
 	Db2GasCall("CharLogin", {CharId, GetCharNameById(CharId)})
 	
 	-- 更新玩家的离线经验
 	AddPlayerOffLineExp(CharId)
 	
 	--修改一下用户的在线服务ID
	result["OldServerId"] = CharacterMediatorDB.GetUserOnServerId(CharId)
	StmtContainer.UpdateUserOnServerId:ExecSql("", g_CurServerId, CharId)
	
	local LoginQueueDB = RequireDbBox("LoginQueueDB")
--	LoginQueueDB.UpdateWaitQueue(data)
	
	local LogoutSql				= RequireDbBox("LogoutServerDB")
 	result["AssociationInfo"]	= LoginGameInitAssociationInfo(CharId)
 	
 	local SystemMsgDB			= RequireDbBox("SystemMsgDB")
	result["SysteMsgTbl"]		= SystemMsgDB.SendSystemMsg(CharId)
	
 	local LogMgr				= RequireDbBox("LogMgrDB")
	LogMgr.LogCharLogin(data["char_id"],data["char_level"])
	
	local team					= RequireDbBox("GasTeamDB")
	result["TeamID"]			= team.GetTeamID(CharId)
	
	local ActivityCountDB		= RequireDbBox("ActivityCountDB")
	result["GuideInfo"]			= ActivityCountDB.GetGuideInfo(data)
	result["CanJoinAction"] = ActivityCountDB.GetCanJoinAction(data)
	return result
end

--死亡复活时删除复活石道具
function CharacterMediatorDB.DelRebornItem(data)
		local nCharID = data["nCharID"]
		local nItemType,sItemName = 1,"复活石"
		local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
		local uHaveCount = g_RoomMgr.GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,nCharID, nItemType,sItemName)
		if uHaveCount <= 0 then
			return false
		end
		--删除材料物品
	local res = g_RoomMgr.DelItem(nCharID,nItemType,sItemName,1,nil,event_type_tbl["复活石使用"])
	return res
end

------------------------------------------------------------
local function SaveKillNpcSoulAndExp(data)
	local char_id		= data["char_id"]
	local soulCount = data["soulCount"]
	local expCount =  data["KillNpcExpCount"]
	local uSoulBottleValue = data["uSoulBottleValue"]
	
	if soulCount > 0 then
		local param = {["soulCount"] = soulCount,["PlayerId"] = char_id,["addSoulType"] = data["addSoulType"]}
		local g_EquipMgr= RequireDbBox("EquipMgrDB")
		g_EquipMgr.ModifyPlayerSoul(param)
	end
	
	if expCount > 0 then
		local param = {}
		param["PlayerTempExp"] = {}
		param["PlayerTempExp"][char_id] = expCount
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		RoleQuestDB.TempMemberExpAdd(param)
	end
	
	if uSoulBottleValue > 0 then
		local ExpOrSoulStorageDB			= RequireDbBox("ExpOrSoulStorageDB")
		ExpOrSoulStorageDB.StorageSoulValueByCount(data)
	end
end

---------------------------------------------------------------

--此函数将来重构完会被注释掉
function CharacterMediatorDB.LogoutImmediatelySaveData(data)
	local char_id		= data["char_id"]
	local account_id	= data["account_id"]
	if not char_id then
		return nil
	end
	
	apcall(SaveKillNpcSoulAndExp,data)
	
--	local UseSpecialItemDB = RequireDbBox("UseSpecialItemDB")
--	apcall(UseSpecialItemDB.SaveSpecialItemCoolDown,char_id, data["SpecialItemCoolDown"])
	apcall(OnSavePosToDB,data)
	if not data["ChangeToServer"] then --切场景下线不存离线状态
		CharacterMediatorDB.LogLogout(char_id)
		DelAllValidateInfo(data)
		local NpcShopDB = RequireDbBox("NpcShopDB")
		NpcShopDB.LogoutDelSoldItemList(data)
		if not data["is_server_shutdown"] then
			local LogMgr = RequireDbBox("LogMgrDB")
			LogMgr.SaveCharLogoutPosition(data["char_id"],data["scene_logic_name"],data["area_name"],data["posCur_x"], data["posCur_y"])
		end
	end
	apcall(SaveTruckInfo,data)
--	apcall(UpdatePkState,data)
end

function CharacterMediatorDB.LogoutDelaySaveData(data)
	local char_id		= data["char_id"]
	if data["DelayDataId"] then --延迟保存的数据 检测是否最新的
		local DelaySaveRet = StmtContainer.GetDelaySaveInfo:ExecStat(char_id)
		if DelaySaveRet:GetRowNum() <= 0 then
--			print "过期数据"
			return
		end
		local serverId, dataId  = DelaySaveRet:GetData(0,0), DelaySaveRet:GetData(0,1)
		if serverId ~= g_CurServerId or dataId ~= data["DelayDataId"] then
--			print ("过期数据2", serverId , g_CurServerId , dataId , data["DelayDataId"])
			return
		end
	end
--	print"延迟保存数据有效"
	StmtContainer.DeleteDelaySaveInfo:ExecStat(char_id)
	
	--在下面添加存盘代码
	apcall(UpdatePkState,data)	
	local UseSpecialItemDB = RequireDbBox("UseSpecialItemDB")
	apcall(UseSpecialItemDB.SaveSpecialItemCoolDown,char_id, data["SpecialItemCoolDown"])
	apcall(SaveCharFightInfo,data)
	apcall(SaveBurstSoulTime,data["char_id"],data["BurstSoulTime"])
	apcall(DeleteSkillCoolDownTime,data)
	apcall(DeleteAureMagicData,data)
	apcall(SaveSkillCoolDownTimeToDB,data)
	apcall(SaveAureMagicToDB,data)
	apcall(DeleteStateFromDB,data)
	apcall(SaveStateToDBBegin,data["char_id"],data["StateData"],data["StateStoreObjData"],0)
	apcall(TakeItemofStone,data)
	apcall(OnSaveToDB,data)
	apcall(SaveServantInfo,data)
	apcall(SaveFightingEvaluation,data)
end

function CharacterMediatorDB.GetLogoutData(data)
	local char_id	= data["char_id"]
	local result = {}
	if not data["ChangeToServer"] then --切场景下线不存离线状态
		result["teamInfo"] = DealWithTeamInfoLogout(data)
		result["TongID"] = GetPlayerTongID(char_id)
	end
	if data["DelayDataId"] then
--	print ("数据库记录延迟数据Id " .. data["DelayDataId"])
		StmtContainer.InsertDelaySaveInfo:ExecStat(char_id, g_CurServerId, data["DelayDataId"])
	else
		StmtContainer.DeleteDelaySaveInfo:ExecStat(char_id)
	end
	
	result["allFriends"] = NotifyFriendOffline(data["char_id"])
	result["groupfriends"] = NotifyFriendGroupOffline(data["char_id"])
	local FriendGroupDB = RequireDbBox("FriendGroupDB")
	result["friendGroupIdTbl"] = FriendGroupDB.GetFriendGroupId(char_id)
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	result["LeaveSceneQuestFailure"] = RoleQuestDB.LeaveSceneSetQuestFailure(data)
	local TongTeachDB              = RequireDbBox("TongTeachDB")
	result["TongTechInfo"]  = TongTeachDB.InitTongTechByCharID(char_id)
	result["char_id"] = char_id
	return result
end

function CharacterMediatorDB.LogoutSave(data)
	local result = CharacterMediatorDB.GetLogoutData(data["GetLogoutData"])
	if data["Immediately"] then
		CharacterMediatorDB.LogoutImmediatelySaveData(data["Immediately"])
	end
	if data["Delay"] then
		CharacterMediatorDB.LogoutDelaySaveData(data["Delay"])
	end
	return result
end

function CharacterMediatorDB.SaveServantInfo(data)
	local char_id		= data["char_id"]
	if not char_id then
		return nil
	end
	--先清除掉数据库中所有的召唤兽信息再重新存储
	DeleteAllServantData(data)
	SaveServantToDB(data)
end

function CharacterMediatorDB.SaveTruckInfo(data)
	if not data["TruckName"] or not data["Resource"] then
		return
	end
	local LoginQueueDB = RequireDbBox("CTongTruckBox")
	LoginQueueDB.SaveTruckInfo(data)
end

function CharacterMediatorDB.ResumeServantByTransfered(data)
	local char_id		= data["char_id"]
	if not char_id then
		return nil
	end
	return GetServantDataFromDB(data)
end

local StmtDef=		--更改玩家支付方式，现在中用于npc商店中
{
	"_PlayerMoneyType",
	"update tbl_char_basic set  cb_uMoneyType =? where cs_uId = ? "
}
DefineSql( StmtDef, StmtContainer )
--设置玩家的支付方式（目前只是在npc商店使用），用绑定的金钱或不绑定的金钱
function CharacterMediatorDB.SetMoneyType(data)
	local uCharId	= data["CharID"]
	local moneyType = data["MoneyType"]
	assert(IsNumber(uCharId))
	assert(IsNumber(moneyType))
	StmtContainer._PlayerMoneyType:ExecSql('', moneyType, uCharId)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return true
	else
		return false
	end
end

--活动离场删除相关的多组物品
function CharacterMediatorDB.DelActionItem(data)
	local nCharID = data["nCharID"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local result = {}
	for _, v in pairs(data["itemTbl"]) do
		local nItemType = v[1]
		local nIndex = v[2]
		local uHaveCount = g_RoomMgr.GetItemCount(nCharID, nItemType,nIndex)
		if uHaveCount > 0 then
			local res = g_RoomMgr.DelItem(nCharID,nItemType,nIndex,uHaveCount,nil,event_type_tbl["任务品或者道具物品删除"])
			table.insert(result , res)
		end
	end
	return result
end

local function SetResult(OneRet,nRoom,nPos,nCount,nBigID,nIndex)
	assert(IsTable(OneRet))
	OneRet.m_nRoomIndex = nRoom
	OneRet.m_nPos = nPos
	OneRet.m_nCount = nCount
	OneRet.m_nBigID = nBigID
	OneRet.m_nIndex = nIndex
	return OneRet
end

local function DelAndAddItem(data)
	local ConsumeItemTbl,AddItemsTbl
	local PlayerId = data["PlayerId"]
	local ConsumeTbl = data["ConsumeTbl"]
	local sceneName = data["sceneName"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if ConsumeTbl ~= nil then
		ConsumeItemTbl = {}
		for i,v in pairs(ConsumeTbl) do
			local result = {}
			for j = 1, #v do
				local DelRes = g_RoomMgr.DelItemByID(PlayerId,v[j],event_type_tbl["任务品或者道具物品删除"])
				if DelRes then
					table.insert(result,v[j])
				else
					return false,"物品删除失败"
				end
			end
			table.insert(ConsumeItemTbl,SetResult(result,v.nRoomIndex,v.nPos,v.nCount))
		end
	end
	
	local ItemsTbl = data["AddItemsTbl"]
	if ItemsTbl ~= nil then
		AddItemsTbl = {}
		local removeindex = {}
		local param = {}
		param["char_id"] = PlayerId
		for i=1, table.getn(ItemsTbl) do
			param["nType"]	= ItemsTbl[i][1]
			param["sName"] 	= ItemsTbl[i][2]
			param["nCount"]	= ItemsTbl[i][3]
			param["sceneName"]	= sceneName
			param["createType"]	= event_type_tbl["道具产出"]
			local res1 = CharacterMediatorDB.AddItem(param)
			if IsNumber(res1) then
				CancelTran()
				return false,res1
			end
			local Tbl = {}
			Tbl["AddItemRes"] = res1
			Tbl["nType"] = param["nType"]
			Tbl["sName"] = param["sName"]
			Tbl["nCount"] = param["nCount"]
			Tbl["Index"] = i
			table.insert(AddItemsTbl,Tbl)
		end
	end
	return true,ConsumeItemTbl,AddItemsTbl
end

--删除一种物品添加另一种物品
function CharacterMediatorDB.DelAndAddItem(data)
	return DelAndAddItem(data)
end

function CharacterMediatorDB.GetItems(data)
	local AddItemsTbl = {}
	local PlayerId 	= data["PlayerId"]
	local ItemsTbl	= data["AddItemsTbl"]
	if ItemsTbl ~= nil then
		local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
		local param = {}
		param["char_id"] = PlayerId
		for i=1, table.getn(ItemsTbl) do
			param["nType"]	= ItemsTbl[i][1]
			param["sName"] 	= ItemsTbl[i][2]
			param["nCount"]	= ItemsTbl[i][3]
			param["sceneName"]	= data["sceneName"]
			param["createType"]	= data["createType"]
			local res1 = CharacterMediatorDB.AddItem(param)
			if IsNumber(res1) then
				CancelTran()
				return false,res1
			end
			local Tbl = {}
			Tbl["AddItemRes"] = res1
			Tbl["nType"] = param["nType"]
			Tbl["sName"] = param["sName"]
			Tbl["nCount"] = param["nCount"]
			Tbl["Index"] = i
			table.insert(AddItemsTbl,Tbl)
		end
	end
	return true,AddItemsTbl
end

function CharacterMediatorDB.ClearPkInfo()
	ClearPkInfo()
end

function CharacterMediatorDB.ClearPkFlagInfo()
	ClearPkFlagInfo()
end

function CharacterMediatorDB.KillPlayerDropObj(param)
--	local DropRate	= data["DropRate"]
	local result		= {}
	local result2		= {}
	local bSucc
	local g_RoomMgr	= RequireDbBox("GasRoomMgrDB")
	local g_EquipMgr= RequireDbBox("EquipMgrDB")
	local PlayerSoulNum = NotifyPlayerSoulNum(param["PlayerId"])
	
	if PlayerSoulNum > 0 then
		local soulCount = PlayerSoulNum - param["soulCount"]
		if soulCount < 0 then
			param["soulCount"] = -PlayerSoulNum
		else
			param["soulCount"] = -param["soulCount"]					--减魂
		end
		param["addSoulType"] = 65
		bSucc,result["SoulRet"] 	= g_EquipMgr.ModifyPlayerSoul(param)
		result["soulCount"] = -param["soulCount"]
	
		param["PlayerId"]	= param["AddPlayerId"]
		param["soulCount"]= -param["soulCount"]							--加魂
		param["addSoulType"] = 66
		bSucc,result2["SoulRet"]	= g_EquipMgr.ModifyPlayerSoul(param)
		result2["soulCount"]= param["soulCount"]
	end
	
	return result, result2
end

function CharacterMediatorDB.updatePlayerPkValue(data)
	return updatePlayerPkValue(data)
end

function CharacterMediatorDB.CheckIsCanInvite(data)
	local SceneType = data["SceneType"]
	local PlayerId 	= data["nCharID"]
	local InvitedID = data["InvitedID"]
	local SceneName = data["SceneName"]
	local QuestName = data["QuestName"]
	local SceneID   = data["SceneID"]
	
	if SceneType == 5 then
		local JoinActionDB = RequireDbBox("JoinActionDB")
		if not JoinActionDB.CheckWarnValue(InvitedID, PlayerId) then
			return false
		end
		
		local Level = CharacterMediatorDB.GetPlayerLevel(InvitedID)
		if Level < g_AreaFbLev[SceneName] then
			return false,191027, {SceneName, g_AreaFbLev[SceneName]}
		else
			
			local SceneMgrDB = RequireDbBox("SceneMgrDB")
			if SceneMgrDB.IsEnteredScene(InvitedID, SceneID) then  --进入过就不用检测了
				return true
			end
			
			local ActivityCountDB = RequireDbBox("ActivityCountDB")
			--local LimitType = AreaFb_Common(SceneName, "LimitType")
			local ActionName = AreaFb_Common(SceneName, "ActionName")
			--local LimitType = FbActionDirect_Common(ActionName, "MaxTimes")
			--local isCan, count = ActivityCountDB.CheckFbActionCount(InvitedID, "地下城探险")
			local isCan, count = ActivityCountDB.CheckFbActionCount(InvitedID, ActionName)
			if isCan then
				return true
			else
				--return false,191026, {count,"地下城探险"}
				return false,191026, {count, ActionName}
			end
		end
	elseif SceneType == 12 then
		return true
	elseif SceneType == 16 then
		if QuestName then
			local param = {}
			param["sQuestName"] = QuestName
			param["uCharId"] = InvitedID
			local RoleQuestDB = RequireDbBox("RoleQuestDB")
			local IsSucceed = RoleQuestDB.CheckQuestSatisfy(param)
			if IsSucceed then
				return true
			else
				return false,3056,{QuestName}
			end
		end
	elseif SceneType == 21 then
		--以后可能要再加个等级判断
		local TongBasicBoxDB = RequireDbBox("GasTongBasicDB")
		local TongID = TongBasicBoxDB.GetTongID(PlayerId)
		local InvitedTongID = TongBasicBoxDB.GetTongID(InvitedID)
		if TongID ~= 0 and TongID == InvitedTongID then
			local tongLev = TongBasicBoxDB.GetTongLevel(InvitedTongID)
			local ActivityCountDB = RequireDbBox("ActivityCountDB")
			local isCan, count = ActivityCountDB.CheckFbActionCount(InvitedID, "佣兵团刷怪本", tongLev)
			if isCan then
				return true
			else
				return false,191026, {count,"佣兵团刷怪本"}
			end
		else
			return false,191034
		end
	end
	return false,191032
end

function CharacterMediatorDB.GetCharInfoByID(CharIDTbl)
	if CharIDTbl and IsTable(CharIDTbl) then
		local Str = 
		[[ 
			select 
			tbl_char.cs_uId,
			tbl_char.c_sName,
			tbl_char_static.cs_uClass,
			tbl_char_basic.cb_uLevel,
			tbl_char_static.cs_uCamp
            from 
            tbl_char,
            tbl_char_static,
            tbl_char_basic	
            where		
			tbl_char_static.cs_uId = tbl_char.cs_uId and
			tbl_char_basic.cs_uId = tbl_char.cs_uId and 
			tbl_char.cs_uId in
		]]
		Str  =  Str .. "(" .. table.concat(CharIDTbl,",") .. ");"
		local _,res = g_DbChannelMgr:TextExecute(Str)
		return res
	end
end

function CharacterMediatorDB.GetPlayerStateData(data)
	return GetPlayerStateData(data)
end

--邀请队友进入副本时消耗道具
--function CharacterMediatorDB.DelInviteItem(data)
--	local nCharID = data["nCharID"]
--	local SceneName = data["SceneName"]
--	local InvitedID = data["InvitedID"]
--	local nItemType,sItemName = 1,"疾传石"
--	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
--	local LoginServerSql = RequireDbBox("LoginServerDB")
--	local uHaveCount = g_RoomMgr.GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,nCharID, nItemType,sItemName)
--	if uHaveCount <= 0 then
--		return 191029
--	end
--
--	if not LoginServerSql.IsPlayerOnLine(InvitedID) then
--		return 191033
--	end
--	
--	local res = g_RoomMgr.DelItem(nCharID,nItemType,sItemName,1,nil,SceneName,9)
--	if IsTable(res) then
--		local IsSucceed, MsgID, strMsgTbl = CheckIsCanInvite(data)
--		if not IsSucceed then
--			CancelTran()
--			return MsgID,strMsgTbl
--		end
--		return res
--	else
--		CancelTran()
--		return 191032
--	end
--end

function CharacterMediatorDB.IsSceneExist(data)
	local sceneId = data["sceneId"]
	return IsSceneExist(sceneId)
end

function CharacterMediatorDB.GetPkFlagInfo(data)
	local attackerId = data["attackerId"]
	local beAttackedId = data["beAttackedId"]
	local flag = GetPkFlagInfo(attackerId, beAttackedId)
	return flag
end


function CharacterMediatorDB.PlayerOnFbPos(data)
	local charId = data["charId"]
	local sceneType = PlayerOnFbPos(charId)
	return sceneType
end


SetDbLocalFuncType(CharacterMediatorDB.GetCharInfoByID)
SetDbLocalFuncType(CharacterMediatorDB.SetMoneyType)
SetDbLocalFuncType(CharacterMediatorDB.OnSaveFightPropertyTick)
SetDbLocalFuncType(CharacterMediatorDB.DelRebornItem)
SetDbLocalFuncType(CharacterMediatorDB.SaveCharDeadTimes)
SetDbLocalFuncType(CharacterMediatorDB.AddItem)
SetDbLocalFuncType(CharacterMediatorDB.DelActionItem)
SetDbLocalFuncType(CharacterMediatorDB.DeleteItem)
SetDbLocalFuncType(CharacterMediatorDB.DelItem)
SetDbLocalFuncType(CharacterMediatorDB.HaveItemCount)
SetDbLocalFuncType(CharacterMediatorDB.KillPlayerDropObj)
SetDbLocalFuncType(CharacterMediatorDB.updatePlayerPkValue)
SetDbLocalFuncType(CharacterMediatorDB.GetPkFlagInfo)
SetDbLocalFuncType(CharacterMediatorDB.SaveServantInfo)
SetDbLocalFuncType(CharacterMediatorDB.DelAndAddItem)
SetDbLocalFuncType(CharacterMediatorDB.CreateServerPlayer)

return CharacterMediatorDB
