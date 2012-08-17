use ybtx;

alter table tbl_member_team add key(t_uId);

create table tbl_item_id
(
	ii_uId	bigint unsigned	not	null auto_increment,

	primary	key(ii_uId)
)engine=innodb;

create database if not exists ybtx_log;
use ybtx_log;

################################################################
#
#		数据中心部分的表格
#		
#		注意：游戏逻辑代码只能添加或者删除数据中心部
#					分的表格数据，但绝不应该读取这些数据的内容
#
################################################################


################################################################
#		事件记录
################################################################

create table tbl_log_event_type
(
		le_uId				bigint unsigned not null,
		let_uEventType	smallint unsigned not null,
		
		key	(le_uId),
		key (let_uEventType)
) engine=myisam;

################################################################
#		账号、角色、物品信息备份记录
################################################################

create table tbl_log_user_static
(
	lus_uId		int unsigned	not	null,
	lus_sName	varchar(96)		not	null,
	lus_sPassword	varchar(96)		not null,

	primary	key(lus_uId),
	unique key(lus_sName)
)engine = myisam;

create table tbl_log_char_static
(
	lcs_uId							bigint unsigned		not	null,
	lus_uId							int	unsigned			not	null,
	lcs_sHair						varchar(96)	not	null,	#头发
	lcs_sHairColor				varchar(96)	not	null,	#发色
	lcs_sFace						varchar(96)	not	null,	#脸型
	lcs_uScale						tinyint	unsigned	not	null,	#缩放比例
	lcs_uSex							tinyint	unsigned	not	null,	#性别
	lcs_uClass						tinyint	unsigned	not	null,	#职业
	lcs_uCamp					tinyint	unsigned	not	null,			#阵营
	lcs_uInherence				tinyint	unsigned	not	null,		#天赋,不是技能天赋
	lcs_dtCreateDate 		datetime not null					, #角色创建时间
	
	primary	key(lcs_uId),
	key(lus_uId)
)engine = myisam;

#未删除的角色列表
create table tbl_log_char
(
		lcs_uId					bigint	unsigned	not	null, 
		lc_sName					char(32) collate utf8_unicode_ci not	null,
		
		primary	key(lcs_uId),
		key(lc_sName)
)engine=myisam;

create table tbl_log_item_static
(
		lis_uId							bigint unsigned					not	null auto_increment,
		lis_uType						tinyint unsigned				not	null,		#物品类型
		lis_sName						varchar(96)							not	null,		#物品名称
		lcs_uId					bigint	unsigned	not	null, 
		lis_sCode						char(32) not null,   
		key (lis_sName),
		key(lcs_uId),
		primary	key(lis_uId)
)engine = myisam;

################################################################
#		参与log的实体
################################################################

create table tbl_log_user
(
		le_uId					bigint unsigned			not	null,
		lus_uId					int unsigned			not null,
		
		key(le_uId),
		key(lus_uId)
)engine=myisam;


create table tbl_log_player
(
		le_uId					bigint unsigned			not	null,
		lcs_uId					bigint unsigned			not null,
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;

create table tbl_log_player_taker
(
		le_uId					bigint unsigned			not	null,
		lcs_uId					bigint unsigned			not null,
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;


create table tbl_log_player_giver
(
		le_uId					bigint unsigned			not	null,
		lcs_uId					bigint unsigned			not null,
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;

create table tbl_log_npc_taker
(
		le_uId					bigint unsigned			not	null,
		lnt_sNpcName		varchar(768)			not null,
		
		key (lnt_sNpcName),
		key(le_uId)
)engine=myisam;

create table tbl_log_npc_giver
(
		le_uId					bigint unsigned			not	null,
		lng_sNpcName		varchar(768)		not null,
		
		key (lng_sNpcName),
		key(le_uId)
)engine=myisam;


###############################################【帮会相关信息表】#############################
create table tbl_log_tong_taker
(
		le_uId		bigint unsigned		not	null,
		lts_uId		int unsigned			not null, #帮会Id
		
		key(le_uId),
		key(lts_uId)
)engine=myisam;


create table tbl_log_tong_giver
(
		le_uId		bigint unsigned	not	null,
		lts_uId		int unsigned		not null, #帮会Id
		
		key(le_uId),
		key(lts_uId)
)engine=myisam;

#帮会基本信息表
create table tbl_log_tong_static
(
		lts_uId  int unsigned	not null, #帮会Id
		lts_sName	varchar(100) collate utf8_unicode_ci not null,#名称

		primary key(lts_uId)
)engine=myisam;

 #帮会荣誉log表
 create table tbl_log_tong_honor
 (
 	le_uId		bigint unsigned			not	null,
 	lth_uHonor bigint unsigned		not null,
 	
 	key(le_uId)
 )engine=myisam;	
 
 #帮会功勋log表
 create table tbl_log_tong_exploit
 (
 	le_uId			bigint unsigned			not	null,
 	lte_uExploit bigint unsigned		not null,
 	
 	key(le_uId)
 )engine=myisam;	
  
 #帮会等级log表
 create table tbl_log_tong_level
 (
 	le_uId			bigint unsigned			not	null,
 	ltl_uLevel	tinyint	unsigned not null,
 	
 	key(le_uId)
 )engine=myisam;	
 
 #帮会资金log表
 create table tbl_log_tong_money
 (
 	le_uId			bigint unsigned		not	null,
 	ltm_uMoney 	bigint not null,
 	
 	key(le_uId)
 )engine=myisam;	
 
  #帮会资源log表
 create table tbl_log_tong_resource
 (
 	le_uId			bigint unsigned		not	null,
 	ltr_uResource bigint not null ,
 	
 	key(le_uId)
 )engine=myisam;	
 
################################################################
#		服务器事件记录
################################################################

create table tbl_log_service_begin
(
		le_uId					bigint unsigned			not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_service_end
(
		le_uId					bigint unsigned			not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_service_crash
(
		le_uId					bigint unsigned			not null,
		
		key(le_uId)
)engine=myisam;

#服务器在线人数情况记录
create table tbl_log_service_online_num
(
		le_uId					bigint unsigned			not null,
		lson_uServerId tinyint	unsigned	not	null, #服务器id
		lson_uOnlineUserNum bigint unsigned			not null, #服务器在线人数
		
		key(le_uId)
)engine=myisam;

#服务器上各个场景的在线人数
create table tbl_log_service_scene_online_num
(
		le_uId					bigint unsigned			not null,
		lsson_uServerId tinyint	unsigned	not	null,
		lsson_uOnlineUserNum bigint unsigned			not null,
		lsson_sSceneName varchar(96) not null,
		lsson_uSceneType tinyint	unsigned	not	null,
		
		key(le_uId)
)engine=myisam;
################################################################
#		进出记录
################################################################

create table tbl_log_login
(
		le_uId					bigint unsigned			not null,
		ll_sIP 					varchar(45) 						not null,
		ll_sSN					varchar(96)				not null,
		
		key(ll_sIP),
		key(ll_sSN),
		key(le_uId)
)engine=myisam;

create table tbl_log_logout
(
		le_uId					bigint unsigned			not null,
		ll_sSN					varchar(96)				not null,
		
		key(ll_sSN),
		key(le_uId)
)engine=myisam;

create table tbl_log_enter_world
(
		le_uId					bigint unsigned			not null,
		lew_sIP 					varchar(45) 						not null,
		
		key(lew_sIP),
		key(le_uId)
)engine=myisam;

create table tbl_log_leave_world
(
		le_uId					bigint unsigned			not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_travel
(
		le_uId									bigint unsigned			not	null,
		lt_uFromMetaSceneId			smallint unsigned		not null,
		lt_uToMetaSceneId				smallint unsigned		not null,
		lt_uHow									tinyint unsigned		not null, #走路？传送？被踢？死亡？召唤？
		
		key(le_uId)
)engine=myisam;

create table tbl_log_change_scene
(
		le_uId					bigint unsigned			not null,
		
		key(le_uId)
)engine=myisam;

################################################################
#		虚拟财产变化记录
################################################################

create table tbl_log_money
(
		le_uId bigint unsigned		not	null,
		lcs_uId			bigint unsigned not null,
		lm_uMoney		bigint 	not null,
		lm_uMoneyType	tinyint not null,
		lm_sCode1 char(32) not null,	#变化后钱的总量生成的
		lm_sCode2 char(32) not null,	#保持连续的
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;


create table tbl_log_depot_money
(
	le_uId				bigint unsigned		not	null,
	ldm_uMoney		bigint not	null,
	ldm_sCode1 char(32) not null, #存取钱后仓库钱的总量生成的
	ldm_sCode2 char(32) not null, #保持连续的
	
	key(le_uId)
)engine=myisam;


create table tbl_log_item
(
		le_uId				bigint unsigned		not null,
		lis_uId				bigint unsigned		not null,

		key(le_uId),
		key(lis_uId)
)engine=myisam;

create table tbl_log_item_giver
(
		le_uId				bigint unsigned		not null,
		lis_uId				bigint unsigned		not null,
		
		key(le_uId),
		key(lis_uId)
)engine=myisam;

create table tbl_log_item_taker
(
		le_uId				bigint unsigned		not null,
		lis_uId				bigint unsigned		not null,
		
		key(le_uId),
		key(lis_uId)
)engine=myisam;

create table tbl_log_item_del
(
		le_uId				bigint unsigned		not null,
		lis_uId				bigint unsigned		not null,
		lid_sCode			char(32) not null,   #保持连续
		
		key(le_uId),
		key(lis_uId)
)engine=myisam;


create table tbl_log_experience
(
		le_uId				bigint unsigned		not	null,
		lcs_uId			bigint unsigned not null,
		le_uExp			bigint unsigned			not null,
		le_sCode1 char(32) not null, #变化后的总量生成的
		le_sCode2 char(32) not null, #保持连续的
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;


create table tbl_log_level
(
		le_uId				bigint unsigned		not	null,
		ll_uLevel			tinyint unsigned			not null,	
		
		key(ll_uLevel),
		key(le_uId)
)engine=myisam;


create table tbl_log_reputation
(
		le_uId				bigint unsigned		not	null,
		lr_uExp			int unsigned			not null,
		
		key(le_uId)
)engine=myisam;


create table tbl_log_skill
(
		le_uId				bigint unsigned			not	null,
		ls_sName		varchar(96)			not null, #技能名
		ls_uLevel		tinyint unsigned not null, #技能等级	
		ls_uType tinyint unsigned not null, #0代表学习，1代表升级
		ls_uIsNonSkill tinyint unsigned not null, #0 代表是战斗技能 、1 代表非战斗技能
		
		key(le_uId)
)engine=myisam;

#寄售交易所
create table tbl_log_market(
		le_uId		bigint unsigned	not	null,
		mos_uId		bigint unsigned	not null,#订单ID
		lm_uCount	int	unsigned	not null, #物品数目
		lm_uPrice	bigint	unsigned	not null, #物品单价
		
		key(le_uId)
)engine=myisam;

################################################################
#		gm 命令记录
################################################################
create table tbl_log_gm_command
(
		le_uId 				bigint unsigned not null,
		lgc_sCommand	varchar(768) not null,
		
		key(le_uId)
)engine=myisam;


################################################################
#		采样记录
################################################################

create table tbl_sample_scene_pop
(
		sc_uId						bigint unsigned			not	null,
		ssp_dtDateTime		datetime						not	null,
		ssp_uPop					smallint unsigned		not	null,
		key(sc_uId),
		key(ssp_dtDateTime)
)engine=myisam;

################################################################
#             交易与邮寄相关
################################################################
create table tbl_log_player_trade
(
		le_uId						bigint unsigned not null,
		lpt_uFromCharId		bigint unsigned not null,
		lpt_uToCharId			bigint unsigned not null,
		lpt_uMoneyType		tinyint	unsigned,
		lpt_uMoney				bigint unsigned,
		lis_uId						bigint unsigned,	
		
		key(le_uId),
		key(lpt_uFromCharId),
		key(lpt_uToCharId),
		key(lis_uId)
)engine=myisam;

create table tbl_log_player_post
(
		le_uId						bigint unsigned not null,
		lpp_uFromCharId		bigint unsigned not null,
		lpp_uToCharId			bigint unsigned not null,
		lis_uId						bigint unsigned,
		
		key(le_uId),
		key(lpp_uFromCharId),
		key(lpp_uToCharId),
		key(lis_uId)	
)engine=myisam;
################################################################
#       任务相关指令
################################################################

#任务接受
create table tbl_log_quest_create
(
		le_uId			bigint unsigned 	not null,
		lcs_uId			bigint unsigned 	not null,
		lqc_sQuestName	varchar(96) 		not null,
		
		key(le_uId),
		key(lcs_uId),
		key(lqc_sQuestName)
)engine=myisam;

#任务完成
create table tbl_log_quest_finish
(
		le_uId			bigint unsigned 	not null,
		lqf_sQuestName	varchar(96) 		not null,
		
		key(le_uId),
		key(lqf_sQuestName)
)engine=myisam;

#任务放弃
create table tbl_log_quest_giveup
(
		le_uId			bigint unsigned 	not null,
		lcs_uId			bigint unsigned 	not null,
		lqg_sQuestName	varchar(96) 		not null,
		
		key(le_uId),
		key(lcs_uId),
		key(lqg_sQuestName)
)engine=myisam;

#任务失败
create table tbl_log_quest_failure
(
		le_uId			bigint unsigned 	not null,
		lcs_uId			bigint unsigned 	not null,
		lqf_sQuestName	varchar(96) 		not null,
		
		key(le_uId),
		key(lcs_uId),
		key(lqf_sQuestName)
)engine=myisam;

################################################################
#       副本相关指令
################################################################
create table tbl_log_enter_scene
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_leave_scene
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;
#######################################################
#             专门记录gmlog的功能
#######################################################
create table tbl_log_gmcommand
(
		lg_sAccountName	varchar(96)	not null,
		lg_sIP			varchar(48)	not null,
		lg_sCallTime		timestamp	not null,
		lg_sCmdContent	varchar(384) not null
)engine=myisam;

#场景信息
create table tbl_log_scene
(
		le_uId			bigint unsigned 	not null,
		ls_sSceneName varchar(96)	not null,
		
		key(le_uId)
)engine=myisam;

#魂信息
create table tbl_log_soul
(
		le_uId			bigint unsigned 	not null,
		lcs_uId			bigint unsigned 	not null,
		ls_uSoulValue bigint not null,
		ls_sCode1 char(32) not null, #变化后钱的总量生成的
		ls_sCode2 char(32) not null, #保持连续的
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;

#######################################################
#             装备相关信息
#######################################################

#装备获得
create table tbl_log_equip_create
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#装备强化
create table tbl_log_equip_intensify
(
		le_uId			bigint unsigned 	not null,
		lei_uLevel		tinyint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#装备升级
create table tbl_log_equip_upgrade
(
		le_uId			bigint unsigned 	not null,
		leu_uLevel		tinyint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#装备销毁
create table tbl_log_equip_destroy
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#装备洗白
create table tbl_log_equip_intensifyback
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#穿装备
create table tbl_log_equip_puton
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#脱装备
create table tbl_log_equip_putoff
(
		le_uId			bigint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;

#装备进阶信息表
create table tbl_log_item_equip_advance
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	liea_uCurAdvancePhase tinyint unsigned	not null,			#装备当前进阶的阶段
	liea_uTotalAdvancedTimes int  unsigned,					#装备的总进阶次数
	liea_uSkillPieceIndex1 	tinyint	unsigned,				#技能激活项的名称1索引
	liea_uSkillPieceIndex2 	tinyint	unsigned,				#技能激活项的名称2索引
	liea_uSkillPieceIndex3 	tinyint	unsigned,				#技能激活项的名称3索引
	liea_uSkillPieceIndex4 	tinyint	unsigned,				#技能激活项的名称4索引		
	liea_uChoosedSkillPieceIndex 	tinyint unsigned,			#当前选择的技能激活项名称索引
	liea_sJingLingType               varchar(3),                 #精灵类型：火、风、地、冰、水
	liea_uAdvanceSoulNum             smallint unsigned default 0,          #注入的进阶魂数目
	liea_uAdvancedTimes              int unsigned default 0,               #进阶次数
	liea_uAdvancedAttrValue1         int unsigned,
	liea_uAdvancedAttrValue2         int unsigned,
	liea_uAdvancedAttrValue3         int unsigned,
	liea_uAdvancedAttrValue4         int unsigned,

	key(le_uId),
	key(lis_uId)
)engine=myisam;

#玩家当前激活技能log表
create table tbl_log_equip_advancedActiveSkill
(
	le_uId			bigint unsigned 	not null,
	lcs_uId			bigint unsigned 	not null,
	iea_uActiveSkillIndex	int unsigned,					#玩家选择可用的技能名称索引
	
	key(le_uId),
	key(lcs_uId)
)engine=myisam;
#######################################################
#             战斗相关信息
#######################################################
#玩家死亡事件
create table tbl_log_player_dead
(
		le_uId			bigint unsigned 	not null,
		lpd_uPlayerId	bigint unsigned 	not null,
		lpd_uLevel tinyint	unsigned not null,
		
		key(le_uId),
		key(lpd_uPlayerId),
		key(lpd_uLevel)
)engine=myisam;
#玩家杀人事件
create table tbl_log_player_kill
(
		le_uId			bigint unsigned 	not null,
		lpk_uPlayerId	bigint unsigned 	not null,
		lpk_uLevel tinyint	unsigned not null,
		
		key(le_uId),
		key(lpk_uPlayerId),
		key(lpk_uLevel)
)engine=myisam;
#保存怪物名称
create table tbl_log_npc_name
(
		le_uId			bigint unsigned 	not null,
		lnn_sNpcName	varchar(96) 	not null,
		
		key(le_uId),
		key(lnn_sNpcName)
)engine=myisam;
#######################################################
#             天赋相关信息
#######################################################
create table tbl_log_talent
(
		le_uId			bigint unsigned 	not null,
		lt_sTalentName varchar(96) 	not null,
		lt_uType tinyint unsigned 	not null, #1-学天赋；2洗天赋
		
		key(le_uId)
)engine=myisam;
#######################################################
#             复活相关信息
#######################################################
create table tbl_log_reborn_info
(
		le_uId			bigint unsigned 	not null,
		lr_uMethod 		tinyint unsigned 	not null,
		
		key(le_uId)
)engine=myisam;
#######################################################
#             合成相关信息
#######################################################
create table tbl_log_compose_info
(
		le_uId			bigint unsigned 	not null,
		lc_uItemId 		bigint unsigned		not null,
		
		key(le_uId)
)engine=myisam;
#######################################################
#             组队相关信息
#######################################################
create table tbl_log_enter_team
(
		le_uId			bigint unsigned 	not null,
		let_uTeamId		bigint unsigned		not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_leave_team
(
		le_uId			bigint unsigned 	not null,
		let_uTeamId		bigint unsigned		not null,
		
		key(le_uId)
)engine=myisam;
#######################################################
#             活动相关信息
#######################################################
create table tbl_log_activity_start
(
		le_uId			bigint unsigned 	not null,
		lea_sName		varchar(96)			not null,
		
		key(le_uId),
		key(lea_sName)
)engine=myisam;

create table tbl_log_activity_stop
(
		le_uId			bigint unsigned 	not null,
		lea_sName		varchar(96)			not null,
		
		key(le_uId),
		key(lea_sName)
)engine=myisam;

create table tbl_log_enter_activity
(
		le_uId			bigint unsigned 	not null,
		lea_uSceneId	bigint unsigned 	not null,
		lea_sName		varchar(96)			not null,
		
		key(le_uId),
		key(lea_sName)
)engine=myisam;

create table tbl_log_leave_activity
(
		le_uId			bigint unsigned 	not null,
		lea_uSceneId	bigint unsigned 	not null,
		lea_sName		varchar(96)			not null,
		
		key(le_uId),
		key(lea_sName)
)engine=myisam;

create table tbl_log_activity_mark
(
		le_uId			bigint unsigned 	not null,
		lam_uMark		varchar(96)			not null,
		
		key(le_uId)
)engine=myisam;

#######################################################
#            聊天相关信息
#######################################################

create table tbl_log_channel_used
(
		le_uId bigint unsigned not null,
		lcu_sChannelName varchar(96) not null,
		
		key(le_uId)
)engine = myisam;


create table tbl_log_channel_chat
(
		lcs_uId	bigint unsigned	not	null,
		lcc_sMsg	text not null,
		lcc_dtChatTime datetime	not null,
		lcc_sChannelName varchar(96) not null,
		lcc_uServerId tinyint	unsigned	not	null, 
		
		key(lcs_uId)
)engine = myisam;

#######################################################
#            社会关系信息
#######################################################

create table tbl_log_add_friend
(
		le_uId bigint unsigned not null,
		laf_uFriendId bigint unsigned not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_delete_friend
(
		le_uId bigint unsigned not null,
		ldf_uFriendId bigint unsigned not null,
		
		key(le_uId)
)engine=myisam;

create table tbl_log_user_cheat
(
		lus_sName	varchar(96)		not	null,
		lcs_uId		bigint unsigned	not	null,
		luc_dtAppearTime datetime not null,
		luc_sSceneName varchar(96) not null
)engine=myisam;

#######################################################
#            积分信息
#######################################################
create table tbl_log_jifenpoint
(
		le_uId bigint unsigned not null,
		lcs_uId		bigint unsigned	not	null,
		lj_uPoint bigint not null,
		lj_uType tinyint unsigned not null, #积分类型 1.自然积分 2.暗黑积分 3.破坏积分 4.大逃杀积分 5.积分赛积分 6.护甲积分
		lj_sCode1 char(32) not null, #变化后的总量生成的
		lj_sCode2 char(32) not null, #保持连续的
		
		
		key(le_uId),
		key(lcs_uId)
)engine=myisam;


#网络延迟 
create table tbl_log_player_latency 
(        
        cs_uId          bigint unsigned         not     null, 
        pl_uLatency     int unsigned not null, 
        pl_tTime        datetime not null, 
        key(cs_uId), 
        key(pl_tTime) 
)engine = myisam; 

#记录玩家下线的场景名和区域名及坐标
create table tbl_log_player_logout_position 
(    
	le_uId bigint unsigned not null,    
	lcs_uId bigint unsigned not null, 
	lplp_sSceneName	varchar(96) not	null,		#scene name 
	lplp_sArgNameName varchar(128) not	null, 
	lplp_uPosX	float	unsigned	not	null default 1,
	lplp_uPosY	float	unsigned	not	null default 1,
	
	key(le_uId), 
	key(lcs_uId),
	key(lplp_sSceneName),
	key(lplp_sArgNameName)
)engine = myisam; 
#######################################################
#            生活技能log信息
#######################################################
##生活技能log
create table tbl_log_commerce_skill
(
	le_uId bigint unsigned not null,
	lcs_uId 		bigint unsigned 		not null,
	lcs_sSkillName			varchar(96)					not null,		#技能名称
	lcs_uSkillLevel		tinyint	unsigned		not null,		#技能等级
	lcs_uExperience		bigint	unsigned		not null,		#经验值
	
	key(le_uId),
	key(lcs_uId)
)engine=myisam;

##锻造熟练度log
create table tbl_log_specialize_smithing
(
	le_uId bigint unsigned not null,
	lcs_uId 						bigint unsigned 		not null,
	lss_uSkillType			tinyint unsigned		not null,			#所属技能类型、锻造或铸甲等
	lss_uType					varchar(96)						not null,		#武器熟练度类型
	lss_uSpecialize		bigint	unsigned		not null,		#熟练度大小
	
	key(le_uId),
	key(lcs_uId)
)engine=myisam;

##锻造专精log
create table tbl_log_expert_smithing
(
	le_uId bigint unsigned not null,
	lcs_uId 						bigint unsigned 		not null,		
	les_uSkillType			tinyint unsigned		not null,			#所属技能类型、锻造或铸甲等
	les_uType					varchar(96)					not null,		#专精类型
	les_uLevel					tinyint	unsigned		not null,		#专精等级
	
	
	key(le_uId),
	key(lcs_uId)
)engine=myisam;

create table tbl_log_event
(
		le_uId				bigint unsigned			not	null auto_increment,
		le_dtDateTime		datetime				not null,

		primary	key(le_uId),
 		key (le_dtDateTime)
)engine=myisam;

use ybtx;

insert into tbl_item_id select max(lis_uId) from tbl_log_item_static;



