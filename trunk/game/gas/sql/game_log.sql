#drop database if exists ybtx_game_log;
#create database ybtx_game_log;
#use ybtx_game_log;

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
create table tbl_log_event
(
 le_uId				bigint unsigned			not	null auto_increment,
 le_dtDateTime		datetime				not null,
 
 primary	key(le_uId ),
 key (le_dtDateTime)
 )engine=innodb;

create table tbl_log_event_type
(
	le_uId				bigint unsigned not null,
	let_uEventType	smallint unsigned not null,
	
	key(le_uId),
	key (let_uEventType)
)engine=innodb;

################################################################
#		账号、角色、物品信息备份记录
################################################################

create table tbl_log_user_static
(
 lus_uId		int unsigned	not	null,
 lus_sName	varchar(96)		not	null,
 lus_dtCreateTime	datetime not null,			 
 
 primary	key(lus_uId),
 unique key(lus_sName)
)engine = innodb;

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
 lcs_dtCreateDate 		datetime not null					, #角色创建时间

 primary	key(lcs_uId),
 key(lus_uId)
 )engine = innodb;

#未删除的角色列表
create table tbl_log_char
(
 lcs_uId					bigint	unsigned	not	null, 
 lc_sName					char(32) collate utf8_unicode_ci not	null,

 primary	key(lcs_uId),
 key(lc_sName)
 )engine=innodb;
 
 #已被删除的角色列表。
create table tbl_log_char_deleted		#deleted roles
(
	le_uId			bigint unsigned not null,
	lcs_uId			bigint	unsigned	not	null,   	#role identifier
	lcd_sName		char(32) collate utf8_unicode_ci not null,	#role name that be used while be avtived

	primary	key(lcs_uId),
	key(le_uId),
	key(lcd_sName)
)engine=innodb;

create table tbl_log_item_static
(
 lis_uId							bigint unsigned					not	null,
 lis_uType						tinyint unsigned				not	null,		#物品类型
 lis_sName						varchar(96)							not	null,		#物品名称
 lcs_uId					bigint	unsigned	not	null, 
 lis_sCode						char(32) not null,   
 
 key(lis_sName),
 key(lcs_uId),
 primary key(lis_uId),
 key(lis_uType)
 )engine = innodb;

##物品绑定属性
create table tbl_log_item_binding    	#binding info of a item
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint 	unsigned	not	null,		#item identifier
	lib_uBindType	tinyint	unsigned	default 0, 		#item binding type
									#	0 stands for unbindinged
									#	1 stands for would be bindinged when be used
									#	2 stands for bindinged
															
	key(le_uId),
	key(lis_uId),
	key(lib_uBindType)
)engine=innodb;
################################################################
#		参与log的实体
################################################################

create table tbl_log_user
(
	le_uId					bigint unsigned			not	null,
	lus_uId					int unsigned			not null,
	
	key(le_uId),
	key(lus_uId)
)engine=innodb;


create table tbl_log_player
(
	le_uId					bigint unsigned			not	null,
	lcs_uId					bigint unsigned			not null,
	
	key(le_uId),
	key(lcs_uId)
 )engine=innodb;

create table tbl_log_player_taker
(
	le_uId					bigint unsigned			not	null,
	lcs_uId					bigint unsigned			not null,
	
	key(le_uId),
	key(lcs_uId)
)engine=innodb;


create table tbl_log_player_giver
(
	le_uId					bigint unsigned			not	null,
	lcs_uId					bigint unsigned			not null,
	
	key(le_uId),
	key(lcs_uId)
 )engine=innodb;

create table tbl_log_npc_taker
(
 le_uId					bigint unsigned			not	null,
 lnt_sNpcName		varchar(768)			not null,

 key (lnt_sNpcName),
 key(le_uId)
)engine=innodb;

create table tbl_log_npc_giver
(
	le_uId					bigint unsigned			not	null,
	lng_sNpcName		varchar(768)		not null,
	
	key (lng_sNpcName),
	key(le_uId)
)engine=innodb;

###############################################【佣兵小队相关信息表】#############################
create table tbl_log_tong_taker
(
	le_uId		bigint unsigned		not	null,
	lts_uId		int unsigned			not null, #佣兵小队Id
	
	key(le_uId),
	key(lts_uId)
)engine=innodb;

create table tbl_log_tong_giver
(
	le_uId		bigint unsigned	not	null,
	lts_uId		int unsigned		not null, #佣兵小队Id
	
	key(le_uId),
	key(lts_uId)
 )engine=innodb;

#佣兵小队基本信息表
create table tbl_log_tong_static
(
 lts_uId  int unsigned	not null, #佣兵小队Id
 lts_sName	varchar(100) collate utf8_unicode_ci not null,#名称
 lts_dtCreateTime  datetime          not null,   #create time
 lts_uCamp      tinyint					  not null,      #tong camp
 lts_uInitiatorId bigint unsigned		not null, #tong Initiator

 primary key(lts_uId),
 key(lts_uCamp)
)engine=innodb;

#佣兵小队荣誉log表
create table tbl_log_tong_honor
(
	le_uId		bigint unsigned			not	null,
	lth_uHonor bigint unsigned		not null,

	key(le_uId)
 )engine=innodb;	

#佣兵小队功勋log表
create table tbl_log_tong_exploit
(
	le_uId			bigint unsigned			not	null,
	lte_uExploit bigint unsigned		not null,
	
	key(le_uId)
 )engine=innodb;	

#佣兵小队等级log表
create table tbl_log_tong_level
(
	le_uId			bigint unsigned			not	null,
	ltl_uLevel	tinyint	unsigned not null,
	
	key(le_uId)
 )engine=innodb;	

#佣兵小队资金log表
create table tbl_log_tong_money
(
	le_uId			bigint unsigned		not	null,
	ltm_uMoney 	bigint not null,
	
	key(le_uId)
 )engine=innodb;	

#佣兵小队资源log表
create table tbl_log_tong_resource
(
	le_uId			bigint unsigned		not	null,
	ltr_uResource bigint not null ,
	
	key(le_uId)
 )engine=innodb;	


#佣兵小队发展度
create table tbl_log_tong_develop_degree
(
	le_uId			bigint unsigned		not	null,
	ltdd_uDevelopDegree bigint not null ,
	
	key(le_uId)
 )engine=innodb;	
 
 
#佣兵小队解散表
create table tbl_log_tong_break
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltb_uBreakCharId bigint unsigned		not null, #tong Initiator
	
	key(le_uId)
 )engine=innodb;	
 
 
 #佣兵小队人事事件表
create table tbl_log_tong_member_event
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltme_uEventType tinyint not null,#1-加入;2-踢出;3-退出;4-更改
	ltme_uExecutorId bigint unsigned		not null, #执行者
	ltme_uObjectId bigint unsigned		not null, #被执行者
	ltme_uJobType tinyint not null,
	
	key(le_uId),
	key(lts_uId),
	key(ltme_uEventType)
 )engine=innodb;
 
#佣兵小队科技事件表
create table tbl_log_tong_technology_event
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltte_uEventType tinyint not null,#1-开始研发;2-停止研发;3-完成研发
	ltte_uExecutorId bigint unsigned		not null, #执行者
	ltte_sName     varchar(96)         	not null , #technology name
	ltte_uLevel    tinyint unsigned     not null,  #current level
	
	key(le_uId),
	key(lts_uId),
	key(ltte_sName)
 )engine=innodb;
 
 #佣兵小队建筑事件表
create table tbl_log_tong_building_event
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltbe_sName     varchar(96)         	not null , #建筑名称
	ltbe_uLevel    tinyint unsigned     not null,  #建筑等级
	ltbe_uEventType tinyint not null,#1-放置建筑;2-拆除摧毁建筑
	ltbe_uExecutorId bigint unsigned		not null, #执行者
	ltbe_uExecutorTongId bigint unsigned		not null, #执行者佣兵小队
	ltbe_uBuildingId bigint unsigned		not null,#建筑id
	ltbe_uAddParameter float unsigned		not null,#附加参数(血量百分比/目标等级/摧毁时掉落资源)
	
	key(le_uId),
	key(lts_uId),
	key(ltbe_sName)
 )engine=innodb;
 
#佣兵小队物品制造表
create table tbl_log_tong_item_produce
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltip_sName     varchar(96) not null , #物品名称
	ltip_uEventType tinyint not null,#1-开始;2-取消 3-完成制造
	ltip_uExecutorId bigint unsigned		not null, #执行者
	
	key(le_uId),
	key(lts_uId),
	key(ltip_uEventType)
 )engine=innodb;
 
 #佣兵小队转类型表
create table tbl_log_tong_change_type
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltct_uNewType  tinyint not null, #新类型
	ltct_uOldType tinyint not null,#原类型
	ltct_uExecutorId bigint unsigned	not null, #执行者
	
	key(le_uId),
	key(lts_uId)
 )engine=innodb;
 
#佣兵小队仓库存取表
create table tbl_log_tong_depot
(
	le_uId	bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltd_uType  tinyint not null, #操作类型 1-捐献放入；2-制造放入； 3-领取
	lis_uId bigint 	unsigned	not	null,#物品id
	ltd_uDepotId  tinyint not null, #仓库id
	ltd_uExecutorId bigint unsigned	not null, #执行者
	ltd_uExecutorPosition tinyint unsigned not null,#执行者职位
	
	key(le_uId),
	key(lts_uId),
	key(ltd_uType)
 )engine=innodb;
 
#佣兵小队迁移驻地表
create table tbl_log_tong_station_move
(
	le_uId	bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltsm_uExecutorId bigint unsigned	not null, #执行者
	ltsm_uEventType tinyint unsigned	not null, #迁移类型
	ltsm_uOldWarzoneId  int  not null, 
	ltsm_uOldIndex      int not null, 
	ltsm_uNewWarzoneId int  not null, 
	ltsm_uNewIndex    int not null, 

	key(le_uId),
	key(lts_uId),
	key(ltsm_uEventType)
 )engine=innodb;
 
#佣兵小队挑战开始
create table tbl_log_tong_challenge_start
(
	le_uId	bigint unsigned		not	null,
	ltcs_uExecutorId bigint unsigned	not null, #执行者
	ltcs_uChallengeTongId  int unsigned	not null, #挑战佣兵小队Id
	ltcs_uRecoveryTongId  int unsigned	not null, #防御佣兵小队Id
	ltcs_uWarzoneId    int unsigned  not null, #战区id
	ltcs_uIndex      int unsigned 	not null, #驻地id

	key(le_uId),
	key(ltcs_uChallengeTongId),
	key(ltcs_uRecoveryTongId),
	key(ltcs_uWarzoneId)
 )engine=innodb;
 
 #佣兵小队挑战结束
create table tbl_log_tong_challenge_end
(
	ltce_uChallengeTongId  int unsigned	not null, #挑战佣兵小队Id
	ltce_uRecoveryTongId  int unsigned	not null, #防御佣兵小队Id
	ltce_uWarzoneId    int unsigned  not null, #战区id
	ltce_uIndex      int unsigned 	not null, #驻地id
	ltce_uResult   smallint unsigned	not null , #挑战胜负 0 - 失败 1 - 成功
	ltce_dtEndTime datetime	not null , #结束时间

	key(ltce_uChallengeTongId),
	key(ltce_uRecoveryTongId),
	key(ltce_uWarzoneId)
 )engine=innodb;
 ##############################【佣兵团相关信息】############################
create table tbl_log_army_corps
(
  lac_uId				bigint unsigned	not null,	
  lac_sName			varchar(100) collate utf8_unicode_ci not null,
  lac_uCreateCharId		bigint unsigned	not null,
  lac_uCamp			tinyint	not null,			
 	lac_dtCreateTime datetime not null,
	
  key(lac_uId),
  key(lac_uCamp)
)engine=innodb;

create table tbl_log_army_corps_break
(
	le_uId					bigint unsigned			not null,
  lac_uId				bigint unsigned	not null,	
  lcs_uId			bigint unsigned	not null,
	
	key(le_uId),
  key(lac_uId),
  key(lcs_uId)
)engine=innodb;

create table tbl_log_army_corps_member_event
(
	le_uId					bigint unsigned			not null,
  lac_uId				bigint unsigned	not null,	#佣兵团id
	lacme_uEventType tinyint not null,#1-加入;2-踢出;3-更改
	lacme_uExecutorId bigint unsigned		not null, #执行者
	lacme_uObjectId bigint unsigned		not null, #被执行者
	lacme_uJobType tinyint not null,#职位
	
	key(le_uId),
  key(lac_uId),
  key(lacme_uEventType)
)engine=innodb;
################################################################
#		服务器事件记录
################################################################

#服务器在线人数情况记录
create table tbl_log_service_online_num
(
 le_uId					bigint unsigned			not null,
 lson_uServerId tinyint	unsigned	not	null, #服务器id
 lson_uOnlineUserNum bigint unsigned			not null, #服务器在线人数

 key(lson_uServerId),
 key(le_uId)
 )engine=innodb;

#服务器上各个场景的在线人数
create table tbl_log_service_scene_online_num
(
 le_uId					bigint unsigned			not null,
 lsson_uServerId tinyint	unsigned	not	null,
 lsson_uOnlineUserNum bigint unsigned			not null,
 lsson_sSceneName varchar(96) not null,
 lsson_uSceneType tinyint	unsigned	not	null,
 
 key(lsson_uServerId),
 key(le_uId)
 )engine=innodb;
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
)engine=innodb;

create table tbl_log_logout
(
	le_uId					bigint unsigned			not null,
	ll_sSN					varchar(96)				not null,
	
	key(le_uId),
	key(ll_sSN)
)engine=innodb;

create table tbl_log_char_login
(
 le_uId					bigint unsigned			not null,
 lcs_uId 			bigint unsigned			not null,
 lcl_uLevel tinyint unsigned			not null,	
 
 key(le_uId),
 key(lcs_uId)
)engine=innodb;
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
 key(lcs_uId),
 key(lm_uMoneyType)
)engine=innodb;


create table tbl_log_depot_money
(
 le_uId				bigint unsigned		not	null,
 ldm_uMoney		bigint not	null,
 ldm_sCode1 char(32) not null, #存取钱后仓库钱的总量生成的
 ldm_sCode2 char(32) not null, #保持连续的

 key(le_uId)
 )engine=innodb;


create table tbl_log_item
(
 le_uId				bigint unsigned		not null,
 lis_uId				bigint unsigned		not null,

 key(le_uId),
 key(lis_uId)
)engine=innodb;

create table tbl_log_item_giver
(
 le_uId				bigint unsigned		not null,
 lis_uId				bigint unsigned		not null,
 
 key(le_uId),
 key(lis_uId)
)engine=innodb;

create table tbl_log_item_taker
(
 le_uId				bigint unsigned		not null,
 lis_uId				bigint unsigned		not null,
 
 key(le_uId),
 key(lis_uId)
)engine=innodb;

create table tbl_log_item_del
(
	le_uId				bigint unsigned		not null,
	lis_uId				bigint unsigned		not null,
	lid_sCode			char(32) not null,   #保持连续
	
	key(le_uId),
	key(lis_uId)
)engine=innodb;


create table tbl_log_experience
(
	le_uId				bigint unsigned		not	null,
	lcs_uId			bigint unsigned not null,
	le_uExp			bigint 			not null,
	le_sCode1 char(32) not null, #变化后的总量生成的
	le_sCode2 char(32) not null, #保持连续的
	
	key(le_uId),
	key(lcs_uId)
)engine=innodb;


create table tbl_log_level
(
 le_uId				bigint unsigned		not	null,
 ll_uLevel			tinyint unsigned			not null,	

 key(le_uId),
 key(ll_uLevel)
)engine=innodb;


create table tbl_log_reputation
(
	le_uId				bigint unsigned		not	null,
	lr_uExp			int unsigned			not null,
	
	key(le_uId)
)engine=innodb;

create table tbl_log_skill
(
 le_uId				bigint unsigned			not	null,
 ls_sName		varchar(96)			not null, #技能名
 ls_uLevel		tinyint unsigned not null, #技能等级	
 ls_uType tinyint unsigned not null, #0代表学习，1代表升级
 ls_uIsNonSkill tinyint unsigned not null, #0 代表是战斗技能 、1 代表非战斗技能

 key(le_uId),
 key(ls_uType),
 key(ls_uIsNonSkill)
)engine=innodb;

#寄售交易所
create table tbl_log_market(
	le_uId		bigint unsigned	not	null,
	mos_uId		bigint unsigned	not null,#订单ID
	lm_uCount	int	unsigned	not null, #物品数目
	lm_uPrice	bigint	unsigned	not null, #物品单价
	
	key(le_uId),
	key(mos_uId)
)engine=innodb;


create table tbl_log_market_item(
	le_uId		bigint unsigned	not	null,
	mos_uId		bigint unsigned	not null,#订单ID
	
	key(le_uId),
	key(mos_uId)
)engine=innodb;

#交易所求购订单表
create table tbl_log_market_buy_order_static
(			
	le_uId		bigint unsigned	not	null,
	lmos_uId				bigint unsigned		not null,		# order identifier
	lmbo_sItemName		varchar(96)			not null,		# acquired item name
	lmbo_uCount			bigint unsigned		not null,		# acquired item number
	lmbo_uPrice			bigint unsigned		not null,		# item price
	
	key(le_uId),
	key(lmos_uId),
	key(lmbo_sItemName)
)engine=innodb;	

#生活技能代工订单表
create table tbl_log_contract_manufacture_order_static(
  le_uId		bigint unsigned	not	null,
  lcmo_uId             bigint      unsigned    not null,
  lcmo_sSkillName      varchar(96)             not null,
  lcmo_sDirection      varchar(96)             not null,
  lcmo_sPrescripName   varchar(96)             not null,
  lcmo_uCMMoney        bigint      unsigned    not null,
  
  key(le_uId),
  key(lcmo_uId)
)engine = innodb;


create table tbl_log_contract_manufacture_order(
	le_uId		bigint unsigned	not	null,
  lcmo_uId             bigint      unsigned    not null,
  
  key(le_uId),
  key(lcmo_uId)
)engine = innodb;
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
)engine=innodb;
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
)engine=innodb;

#任务完成
create table tbl_log_quest_finish
(
 le_uId			bigint unsigned 	not null,
 lqf_sQuestName	varchar(96) 		not null,
 
 key(le_uId),
 key(lqf_sQuestName)
)engine=innodb;

#任务放弃
create table tbl_log_quest_giveup
(
 le_uId			bigint unsigned 	not null,
 lcs_uId			bigint unsigned 	not null,
 lqg_sQuestName	varchar(96) 		not null,

 key(le_uId),
 key(lcs_uId),
 key(lqg_sQuestName)
)engine=innodb;

#任务失败
create table tbl_log_quest_failure
(
 le_uId			bigint unsigned 	not null,
 lcs_uId			bigint unsigned 	not null,
 lqf_sQuestName	varchar(96) 		not null,

 key(le_uId),
 key(lcs_uId),
 key(lqf_sQuestName)
)engine=innodb;

################################################################
#       副本相关指令
################################################################
create table tbl_log_enter_scene
(
 le_uId			bigint unsigned 	not null,

 key(le_uId)
 )engine=innodb;

create table tbl_log_leave_scene
(
 le_uId			bigint unsigned 	not null,
 
 key(le_uId)
)engine=innodb;

create table tbl_log_create_scene
(
 le_uId			bigint unsigned 	not null,
 lcs_uSceneId	bigint unsigned 	not null,
 
 key(le_uId),
 key(lcs_uSceneId)
)engine=innodb;


create table tbl_log_join_activity
(
 le_uId	 bigint unsigned 	not null,
 lcs_uId bigint unsigned 	not null,
 lts_uId int unsigned	not null,
 
 key(le_uId),
 key(lcs_uId),
 key(lts_uId)
)engine=innodb;

#######################################################
#             matchgame相关
#######################################################
create table tbl_log_matchgame_server		#matchgame服务端流程记录
(
	lms_uRoomId  bigint unsigned not null,		#活动房间
	lms_sActionName varchar(32),							#活动名
	lms_uSceneId bigint unsigned not null,		#活动房间
	lms_uIndex tinyint unsigned not null,			#用于拆分保存超出sLog长度的记录
	lms_dtTime datetime not null default 0,		#保存时间
	lms_sLog	 varchar(1024),									#活动流程记录
	
	key(lms_uRoomId),
	key(lms_uSceneId),
	key(lms_uIndex)
)engine = innodb;

create table tbl_log_matchgame_room					#matchgame数据库Room创建记录
(
	lmr_uRoomId bigint unsigned not null,			#活动房间
	lmr_sActionName varchar(32),							#活动名
	lmr_uSceneId bigint unsigned not null,		#活动场景
	lmr_uServerId tinyint unsigned not null,	#服务器id
	lmr_dtTime datetime not null default 0,		#创建时间
	
	key(lmr_uRoomId),
	key(lmr_uSceneId),
	key(lmr_uServerId)
)engine = innodb;

create table tbl_log_matchgame_member				#matchgame数据库玩家操作记录
(
	lcs_uId			bigint unsigned not null,
	lmm_sActionName varchar(32),							#活动名
	lmm_uTeamId bigint unsigned not null,
	lmm_uRoomId	bigint unsigned not null,			#活动房间
	lmm_sFuncName varchar(16),								#操作函数
	lmm_sState varchar(32),										#返回函数
	lmm_dtTime datetime not null default 0,		#保存时间
	
	key(lcs_uId),
	key(lmm_uTeamId),
	key(lmm_uRoomId)
)engine = innodb;

create table tbl_log_xianxie				#鲜血试炼场结果记录
(
	le_uId			bigint unsigned 	not null,
	lcs_uId			bigint unsigned not null,
	lx_uWin 		tinyint not null default 0,		#胜利
	lx_uScore 	tinyint not null default 0,		#队伍分
	lx_uKillNum	tinyint not null default 0,		#杀人数
	lx_uDeadNum	tinyint not null default 0,		#死亡数
	
	key(le_uId),
	key(lcs_uId)
)engine = innodb;

#######################################################
#             专门记录gmlog的功能
#######################################################
create table tbl_log_gmcommand
(
 lg_sAccountName	varchar(96)	not null,
 lg_sIP			varchar(48)	not null,
 lg_sCallTime		timestamp	not null,
 lg_sCmdContent	varchar(384) not null
 
 )engine=innodb;

#场景信息
create table tbl_log_scene
(
 le_uId			bigint unsigned 	not null,
 ls_sSceneName varchar(96)	not null,

 key(le_uId)
)engine=innodb;

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
)engine=innodb;

#######################################################
#             装备相关信息
#######################################################

#装备强化
create table tbl_log_equip_intensify
(
 le_uId			bigint unsigned 	not null,
 lei_uLevel		tinyint unsigned 	not null,

 key(le_uId)
 )engine=innodb;

#装备升级
create table tbl_log_equip_upgrade
(
 le_uId			bigint unsigned 	not null,
 leu_uLevel		tinyint unsigned 	not null,

 key(le_uId)
 )engine=innodb;

#装备洗白
create table tbl_log_equip_intensifyback
(
 le_uId			bigint unsigned 	not null,

 key(le_uId)
 )engine=innodb;

#穿装备
create table tbl_log_equip_puton
(
 le_uId			bigint unsigned 	not null,
 lep_uEquipPart	tinyint unsigned  not null,   #装备位置

 key(le_uId)
)engine=innodb;

#脱装备
create table tbl_log_equip_putoff
(
 le_uId			bigint unsigned 	not null,

 key(le_uId)
)engine=innodb;

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
 liea_sAdvancedSoulRoot           varchar(12),
 liea_sAdvancedAttr1	            varchar(48),
 liea_sAdvancedAttr2	            varchar(48),
 liea_sAdvancedAttr3	            varchar(48),
 liea_sAdvancedAttr4	            varchar(48),

 key(le_uId),
 key(lis_uId)
)engine=innodb;

#玩家当前激活技能log表
create table tbl_log_equip_advancedActiveSkill
(
	le_uId			bigint unsigned 	not null,
	lcs_uId			bigint unsigned 	not null,
	iea_uActiveSkillIndex	int unsigned,					#玩家选择可用的技能名称索引
	
	key(le_uId),
	key(lcs_uId)
)engine=innodb;
	
	
#装备耐久
create table tbl_log_item_equip_durability
(
  le_uId			bigint unsigned 	not null,
  lis_uId          bigint unsigned not null,	#物品ID
  lied_uMaxDuraValue float unsigned,          #耐久上限值
  lied_uCurDuraValue float unsigned,         #当前耐久值
   
  key(lis_uId),
  key(le_uId)
)engine = innodb;

#防具
create table tbl_log_item_armor
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	lia_uBaseLevel		tinyint unsigned 	not null, 		#起始等级
	lia_uCurLevel        tinyint unsigned 	not null,
	lia_uUpgradeAtrrValue1	int unsigned 	not null, 			#装备产出时的属性一（升级属性1~3按在配置表中的顺序）
	lia_uUpgradeAtrrValue2	int unsigned 	not null,			#装备产出时的属性二
	lia_uUpgradeAtrrValue3	int unsigned 	not null,			#装备产出时的属性三
	lia_uStaticProValue		int unsigned 	not null,       	#静态装备属性值
	lia_uIntensifyQuality    tinyint unsigned    not null default 0, 	#装备强化资质，该值决定装备可强化的最高阶段
	lia_sIntenSoul varchar(6),
	
	key (le_uId),
	key (lis_uId)
)engine=innodb;

create table tbl_log_item_weapon
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	liw_uBaseLevel 		tinyint unsigned 	not null, 		#起始等级（打造时修改）
	liw_uCurLevel       tinyint unsigned 	not null,
	liw_uDPS			float unsigned 	not null,			#当前DPS（打造时修改）	
	liw_uAttackSpeed float unsigned 	not null default 0,			#攻击速度
	liw_uDPSFloorRate tinyint 	not null default 0,			#攻击浮动
	liw_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	liw_sIntenSoul varchar(6),
	
	key (le_uId),
	key (lis_uId)
)engine=innodb;

create table tbl_log_item_ring
(
	le_uId			bigint unsigned 	not null,
	lis_uId			bigint unsigned 	not null,		    #物品ID
	lir_uBaseLevel		tinyint unsigned	not null, 		#起始等级
	lir_uCurLevel        tinyint unsigned 	not null,
	lir_uDPS			float unsigned	not null,			    #攻击力
	lir_uStaticProValue  float unsigned	not null,			#静态装备属性值
	lir_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	lir_sIntenSoul varchar(6),
	
	key (le_uId),
	key (lis_uId)
)engine=innodb;

#盾牌
create table tbl_log_item_shield
(
	le_uId			bigint unsigned 	not null,
	lis_uId 			bigint unsigned 	not null,		#物品ID
	lis_uBaseLevel 		tinyint unsigned 	not null, 		#起始等级（打造时修改）
	lis_uCurLevel        tinyint unsigned 	not null,
	lis_uIntensifyQuality    tinyint unsigned    not null default 0,  	#装备强化资质，该值决定装备可强化的最高阶段
	lis_sIntenSoul varchar(6),
	
	key (le_uId),
	key (lis_uId)
)engine=innodb;

##装备护甲属性
create table tbl_log_item_equip_armor (
	le_uId			bigint unsigned 	not null,
	lis_uId          bigint unsigned not null,	#物品ID
	liea_sAttr		varchar(48) not null, #护甲片(名称)
	liea_uAddTimes tinyint	unsigned   not null,
	liea_uIndex tinyint	unsigned   not null,
	
	key (le_uId),
	key (lis_uId)
)engine = innodb;

##盾牌产出新增四种属性。
create table tbl_log_item_shield_Attr
(
	le_uId			bigint unsigned 	not null,
	lis_uId 			bigint unsigned not null,	#物品ID
	lisa_uAttrValue1	int unsigned ,				#装备产出时的属性一护甲值的值(以后升级用到)
	lisa_uAttrValue2	int unsigned ,				#装备产出时的属性二自然抗值的值(以后升级用到)
	lisa_uAttrValue3	int unsigned ,				#装备产出时的属性三破坏抗值的值(以后升级用到)
	lisa_uAttrValue4	int unsigned ,				#装备产出时的属性四暗黑抗值的值(以后升级用到)
	
	key (le_uId),
	key (lis_uId)
)engine=innodb;


##装备强化信息表
create table tbl_log_item_equip_intensify
(
	le_uId			bigint unsigned 	not null,
	lis_uId		bigint unsigned		not null,			#物品id
	liei_uIntensifySoulNum	int unsigned		not null default 0,#同上，不过适是用于强化的魂数目
	liei_uPreAddAttr1	varchar(48) not null default "" , 					#强化上一阶段的附加属性1（强化时修改）
	liei_uPreAddAttrValue1	int unsigned not null default 0,   			#附加属性1的值		（强化时修改）
	liei_uPreAddAttr2	varchar(48) not null default "", 					#强化上一阶段的附加属性2（强化时修改）
	liei_uPreAddAttrValue2	int unsigned not null default 0,   			#强化上一阶段的附加属性2的值（强化时修改）
	liei_uAddAttr1		varchar(48) not null default "" , 					#强化当前阶段附加属性1	（强化时修改）
	liei_uAddAttrValue1	int unsigned not null default 0,   				#强化当前阶段附加属性1的值（强化时修改）
	liei_uAddAttr2		varchar(48) not null default "", 					#强化当前阶段附加属性2	（强化时修改）
	liei_uAddAttrValue2	int unsigned not null default 0,   				#强化当前阶段附加属性2的值（强化时修改）
	liei_uIntensifyPhase	tinyint unsigned not null default 0,				#装备强化到的阶段（初始为0，到1、2、3、4）
	liei_uEuqipSuitType	tinyint unsigned not null default 0,				#装备强化出的套装类型，0表示无，1表示2件套，2表示3件套，3表示4件套，4表示六件套，5表示8件套
	liei_sSuitName		varchar(120) not null default "",					#套装名称
	liei_uIntensifyBackState tinyint unsigned not null default 0, 			#装备是否可进行重铸，0表示不能重铸，1表示可以
	liei_uIntensifyTimes      smallint unsigned not null default 0, 		#记录装备强化次数
	liei_uIntensifyBackTimes  smallint unsigned not null default 0, 		#记录装备重铸次数
	liei_uIntenTalentIndex     smallint unsigned not null default 0,        #强化到三阶段产生的额外天赋或技能名称
	
	key(le_uId),
	key(lis_uId)
)engine=innodb;


#装备追加
create table tbl_log_item_equip_superaddation(
	le_uId			bigint unsigned 	not null,
	lis_uId          bigint unsigned not null,
	lies_uCurSuperaddPhase tinyint unsigned not null,
	
	key(le_uId),
	key (lis_uId)
)engine = innodb;


##装备制作者
create table tbl_log_item_maker
(
	le_uId			bigint unsigned 	not null,
	lis_uId						bigint unsigned not null, 	#物品id
	lcs_uId 						bigint unsigned 		not null,		#制造者
	
	key(le_uId),
	key(lis_uId)
)engine=innodb;

#############装备强化4~9阶段属性表######################
create table tbl_log_item_equip_intensifyAddAttr
(
	le_uId			bigint unsigned 	not null,
  lis_uId      bigint unsigned 	not null,		                #物品ID
  liei_sAddAttr4       varchar(48)   not null default "",        					#强化4阶段附加属性
  liei_uAddAttr4Value  int unsigned  not null default 0,          				#强化4阶段附加属性值
  liei_sAddAttr5       varchar(48)   not null default "",       					#强化5阶段附加属性
  liei_uAddAttr5Value  int unsigned  not null default 0,          				#强化5阶段附加属性值
  liei_sAddAttr6       varchar(48)   not null default "",        					#强化6阶段附加属性
  liei_uAddAttr6Value  int unsigned  not null default 0,          				#强化6阶段附加属性值
  liei_sAddAttr7       varchar(48)   not null default "",        					#强化7阶段附加属性
  liei_uAddAttr7Value  int unsigned  not null default 0,          				#强化7阶段附加属性值
  liei_sAddAttr8       varchar(48)   not null default "",        					#强化8阶段附加属性
  liei_uAddAttr8Value  int unsigned  not null default 0,          				#强8四阶段附加属性值
  liei_sAddAttr9       varchar(48)   not null default "",        					#强化9阶段附加属性
  liei_uAddAttr9Value  int unsigned  not null default 0,          				#强化9阶段附加属性值
  
  key(le_uId),
  key(lis_uId)
)engine=innodb;
#######################################################
#             战斗相关信息
#######################################################
#玩家死亡事件
create table tbl_log_player_dead
(
	le_uId			bigint unsigned 	not null,
	lpd_uPlayerId	bigint unsigned 	not null,
	lpd_uLevel tinyint	unsigned not null,
	
	key(lpd_uPlayerId),
	key(le_uId)
)engine=innodb;
#玩家杀人事件
create table tbl_log_player_kill
(
	le_uId			bigint unsigned 	not null,
	lpk_uPlayerId	bigint unsigned 	not null,
	lpk_uLevel tinyint	unsigned not null,
	
	key(lpk_uPlayerId),
	key(le_uId)
)engine=innodb;
#保存怪物名称
create table tbl_log_npc_name
(
	le_uId			bigint unsigned 	not null,
	lnn_sNpcName	varchar(96) 	not null,
	
	key(le_uId),
	key(lnn_sNpcName)
)engine=innodb;

#战斗相关信息快照表
create table tbl_log_char_fight_info
(
	le_uId bigint unsigned not null,
	lcs_uId bigint unsigned not null,#玩家id
	lcfi_uLevel tinyint unsigned not null,#等级
	lcfi_uSeries  tinyint unsigned not null,#天赋系
	lcfi_uFightPoint smallint unsigned not null,#战斗力
	lcfi_uNatureResistance float unsigned	not null,#自然抗
	lcfi_uEvilResistance float unsigned	not null,#破坏抗值
	lcfi_uDestructionResistance float unsigned	not null,#暗黑抗值
	lcfi_uDefence float unsigned	not null,#护甲值
	
	key(le_uId),
	key(lcs_uId),
	key(lcfi_uLevel)
)engine = innodb;

#插旗Pk
create table tbl_log_char_pk
(
	le_uId			bigint unsigned 	not null,
	lcp_uFlagId int unsigned not null,#旗子id
	lcp_uResponsesId bigint unsigned not null,#插旗者id
	lcp_uChallengeId bigint unsigned not null,#挑战者id
	lcp_uCostTime bigint unsigned 	not null,#战斗时长
	lcp_uIsSucc tinyint unsigned not null,#胜负
	
	key(le_uId),
	key(lcp_uFlagId),
	key(lcp_uIsSucc)
)engine=innodb;

#3V3比赛\角斗场\鲜血试炼场\剧情本\地下城
create table tbl_log_team_activity
(
	le_uId bigint unsigned not null,
	lta_uSuccTeamId bigint unsigned not null,#胜方队伍id
	lta_uFailTeamId bigint unsigned not null,#失败队伍id
	lta_uCostTime bigint unsigned not null,#活动、比赛总耗时
	lta_uActivityType tinyint unsigned not null,#活动类型 1-3V3比赛\2-角斗场\3-鲜血试炼场\4-剧情本\5-地下城
	
	key(le_uId),
	key(lta_uActivityType)
)engine = innodb;
#######################################################
#             天赋相关信息
#######################################################
create table tbl_log_talent
(
	le_uId			bigint unsigned 	not null,
	lt_sTalentName varchar(96) 	not null,
	lt_uType tinyint unsigned 	not null, #1-学天赋；2洗天赋
	
	key(le_uId),
	key(lt_uType)
)engine=innodb;
#######################################################
#             复活相关信息
#######################################################
create table tbl_log_reborn_info
(
	le_uId			bigint unsigned 	not null,
	lr_uMethod 		tinyint unsigned 	not null,
	
	key(le_uId)
)engine=innodb;
#######################################################
#             合成相关信息
#######################################################
create table tbl_log_compose_info
(
	le_uId			bigint unsigned 	not null,
	
	key(le_uId)
)engine=innodb;
#######################################################
#             组队相关信息
#######################################################
create table tbl_log_enter_team
(
	le_uId			bigint unsigned 	not null,
	let_uTeamId		bigint unsigned		not null,
	
	key(le_uId)
)engine=innodb;

create table tbl_log_leave_team
(
	le_uId			bigint unsigned 	not null,
	let_uTeamId		bigint unsigned		not null,

	key(le_uId)
)engine=innodb;
#######################################################
#             活动相关信息
#######################################################

create table tbl_log_enter_activity
(
	le_uId			bigint unsigned 	not null,
	lea_uSceneId	bigint unsigned 	not null,
	lea_sName		varchar(96)			not null,
	
	key(le_uId),
	key(lea_sName)
)engine=innodb;

create table tbl_log_leave_activity
(
	le_uId			bigint unsigned 	not null,
	lea_uSceneId	bigint unsigned 	not null,
	lea_sName		varchar(96)			not null,
	
	key(le_uId),
	key(lea_sName)
)engine=innodb;

create table tbl_log_xiuxingta
(
	le_uId			bigint unsigned 	not null,
	lx_uGateId		int unsigned 		not null,
	lx_uSceneId bigint	unsigned	not	null,
	lx_uSucceedFlag tinyint unsigned 	not null,#1--成功；2--失败；3--时间到 4--放弃
	lx_uSpendTimes bigint unsigned 		not null, #每关花费的时间
	lx_sFloorId varchar(2) not null,#层id
	
	key(le_uId),
	key(lx_uSceneId),
	key(lx_uSucceedFlag),
	key(lx_sFloorId)
)engine=innodb;
#######################################################
#            社会关系信息
#######################################################
create table tbl_log_add_friend
(
	le_uId bigint unsigned not null,
	laf_uFriendId bigint unsigned not null,
	
	key(le_uId)
)engine=innodb;

create table tbl_log_delete_friend
(
	le_uId bigint unsigned not null,
	ldf_uFriendId bigint unsigned not null,
	
	key(le_uId)
)engine=innodb;

create table tbl_log_user_cheat
(
	lus_sName	varchar(96)		not	null,
	lcs_uId		bigint unsigned	not	null,
	luc_dtAppearTime datetime not null,
	luc_sSceneName varchar(96) not null,
	
	key(lus_sName),
	key(luc_dtAppearTime)
)engine=innodb;

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
	key(lcs_uId,lj_uType)
)engine=innodb;

create table tbl_log_dataosha_point
(
	le_uId bigint unsigned not null,
	ldp_uPoint bigint not null,
	
	key(le_uId)
)engine=innodb;
	

#网络延迟 
create table tbl_log_player_latency 
(        
	cs_uId          bigint unsigned         not     null, 
	pl_uLatency     int unsigned not null, 
	pl_tTime        datetime not null, 
	
	key(cs_uId), 
	key(pl_tTime) 
)engine = innodb; 

#记录玩家下线的场景名和区域名及坐标
create table tbl_log_player_logout_position 
(    
	le_uId bigint unsigned not null,    
	lcs_uId bigint unsigned not null, 
	lplp_sSceneName	varchar(96) not	null,		#scene name 
	lplp_sAreaName varchar(128) not	null, 
	lplp_uPosX	float	unsigned	not	null default 1,
	lplp_uPosY	float	unsigned	not	null default 1,
	
	key(le_uId),
	key(lcs_uId),
	key(lplp_sSceneName),
	key(lplp_sAreaName)
)engine = innodb; 
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
)engine=innodb;

##锻造熟练度log
create table tbl_log_specialize_smithing
(
	le_uId bigint unsigned not null,
	lcs_uId 						bigint unsigned 		not null,
	lss_uSkillType			tinyint unsigned		not null,			#所属技能类型、锻造或铸甲等
	lss_uType					varchar(96)						not null,		#武器熟练度类型
	lss_uSpecialize		bigint	unsigned		not null,		#熟练度大小
	
	key(lcs_uId),
	key(le_uId)
)engine=innodb;

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
)engine=innodb;
	
#种花
create table tbl_log_cultivate_flowers
(
	le_uId bigint unsigned not null,
	
	key(le_uId)
)engine = innodb;
	
#任务品拍卖过程
create table tbl_log_item_vendue
(
	le_uId bigint unsigned not null,
	
	key(le_uId)
)engine = innodb;

#任务品拍卖结果
create table tbl_log_item_vendue_end
(
	le_uId bigint unsigned not null,
	
	key(le_uId)
)engine = innodb;

#提升装备可强化上限
create table tbl_log_equip_intensify_level_up
(
	le_uId bigint unsigned not null,
	
	key(le_uId)
)engine = innodb;

#外网服务器用来记录数据库更新是否成功，值为sql文件名
create table tbl_database_upgrade_record(
		dur_sRecord varchar(64) not null,
		dur_dtTime datetime not null
)engine = innodb;

#离线经验兑换
create table tbl_log_exp_change
(
	le_uId bigint unsigned not null,
	
	key(le_uId)
)engine = innodb;

#玩家战斗力评分
create table tbl_log_char_fighting_evaluation
(
	le_uId bigint unsigned not null,
	lcs_uId			bigint unsigned		not	null,
	lcfe_uPoint	float unsigned not null,
	
	key(le_uId),
	key(lcs_uId)
)engine = innodb;

#记录角色速度变化log
create table tbl_log_char_speed
(
	lcs_sName	varchar(96)	not	null,
	lcs_uSpeed float not null,
	lcs_dtAppearTime datetime not null,
	lcs_sChangeReason varchar(96) not null,
	
	key(lcs_sName),
	key(lcs_uSpeed)
)engine=innodb;

#记录角色经验、魂系数变化log
create table tbl_log_char_exp_soul_modulus
(
	lcs_uId	bigint unsigned	not	null,
	lcesm_uModulusType tinyint unsigned not null,#系数类型 1-魂系数 2-经验系数
	lcesm_uModulusValue float unsigned not null,#系数值
	lcesm_dtChangeTime datetime not null,#系数值
	
	key(lcs_uId)
)engine=innodb;

#佣兵小队抢夺资源点报名
create table tbl_log_tong_resource_sig_up
(
	le_uId	bigint unsigned		not	null,
	lts_uId int unsigned	not null, #佣兵小队Id
	ltrsu_uExecutorId bigint unsigned		not null, #执行者
	ltrsu_uExploit bigint not null,
	ltrsu_sObjName varchar(96) not null,
	
	key(le_uId),
	key(lts_uId),
	key(ltrsu_uExecutorId)
 )engine=innodb;	


#点券基本信息log表
create table tbl_log_coupons_info(
	le_uId	bigint unsigned		not	null,
	lci_uId          bigint  unsigned		not null,
	lci_sName        varchar(96)  collate utf8_unicode_ci           not null,
	lci_uSmallIcon   int     unsigned        not null,
	lci_uPrice       int(10) unsigned        not null,
	lci_sDesc        text  collate utf8_unicode_ci       not null,
	lci_sUrl         text  collate utf8_unicode_ci        not null,
	
	key (le_uId),
	key (lci_uId)
)engine = innodb;

#玩家购买点券log信息
create table tbl_log_char_bought_coupons(
	le_uId	bigint unsigned		not	null,
	lcs_uId      bigint unsigned     not null,
	lci_uId      bigint unsigned     not null,
	lcbc_sSequenceId varchar(96)     not null, 
	
	key (le_uId),
	key(lcs_uId, lci_uId)
)engine = innodb;

#######################################################
#            删LOG相关
#######################################################

CREATE TABLE `tbl_log_normal_delete`(
	`lnd_sName` varchar(96) not	null,
	primary	key(`lnd_sName`)
)engine = innodb;

insert into `tbl_log_normal_delete` values('tbl_log_event');
insert into `tbl_log_normal_delete` values('tbl_log_event_type');
insert into `tbl_log_normal_delete` values('tbl_log_char_deleted');
insert into `tbl_log_normal_delete` values('tbl_log_user');
insert into `tbl_log_normal_delete` values('tbl_log_player');
insert into `tbl_log_normal_delete` values('tbl_log_player_taker');
insert into `tbl_log_normal_delete` values('tbl_log_player_giver');
insert into `tbl_log_normal_delete` values('tbl_log_npc_taker');
insert into `tbl_log_normal_delete` values('tbl_log_npc_giver');
insert into `tbl_log_normal_delete` values('tbl_log_tong_taker');
insert into `tbl_log_normal_delete` values('tbl_log_tong_giver');
insert into `tbl_log_normal_delete` values('tbl_log_tong_honor');
insert into `tbl_log_normal_delete` values('tbl_log_tong_exploit');
insert into `tbl_log_normal_delete` values('tbl_log_tong_level');
insert into `tbl_log_normal_delete` values('tbl_log_tong_money');
insert into `tbl_log_normal_delete` values('tbl_log_tong_resource');
insert into `tbl_log_normal_delete` values('tbl_log_tong_develop_degree');
insert into `tbl_log_normal_delete` values('tbl_log_tong_member_event');
insert into `tbl_log_normal_delete` values('tbl_log_tong_technology_event');
insert into `tbl_log_normal_delete` values('tbl_log_tong_building_event');
insert into `tbl_log_normal_delete` values('tbl_log_tong_item_produce');
insert into `tbl_log_normal_delete` values('tbl_log_tong_change_type');
insert into `tbl_log_normal_delete` values('tbl_log_tong_depot');
insert into `tbl_log_normal_delete` values('tbl_log_tong_station_move');
insert into `tbl_log_normal_delete` values('tbl_log_tong_challenge_start');
insert into `tbl_log_normal_delete` values('tbl_log_army_corps_member_event');
insert into `tbl_log_normal_delete` values('tbl_log_service_online_num');
insert into `tbl_log_normal_delete` values('tbl_log_service_scene_online_num');
insert into `tbl_log_normal_delete` values('tbl_log_login');
insert into `tbl_log_normal_delete` values('tbl_log_logout');
insert into `tbl_log_normal_delete` values('tbl_log_char_login');
insert into `tbl_log_normal_delete` values('tbl_log_money');
insert into `tbl_log_normal_delete` values('tbl_log_depot_money');
insert into `tbl_log_normal_delete` values('tbl_log_item');
insert into `tbl_log_normal_delete` values('tbl_log_item_giver');
insert into `tbl_log_normal_delete` values('tbl_log_item_taker');
insert into `tbl_log_normal_delete` values('tbl_log_experience');
insert into `tbl_log_normal_delete` values('tbl_log_level');
insert into `tbl_log_normal_delete` values('tbl_log_reputation');	
insert into `tbl_log_normal_delete` values('tbl_log_skill');
insert into `tbl_log_normal_delete` values('tbl_log_market');
insert into `tbl_log_normal_delete` values('tbl_log_market_item');
insert into `tbl_log_normal_delete` values('tbl_log_market_buy_order_static');
insert into `tbl_log_normal_delete` values('tbl_log_contract_manufacture_order_static');
insert into `tbl_log_normal_delete` values('tbl_log_contract_manufacture_order');
insert into `tbl_log_normal_delete` values('tbl_log_player_trade');
insert into `tbl_log_normal_delete` values('tbl_log_quest_create');
insert into `tbl_log_normal_delete` values('tbl_log_quest_finish');
insert into `tbl_log_normal_delete` values('tbl_log_quest_giveup');
insert into `tbl_log_normal_delete` values('tbl_log_quest_failure');
insert into `tbl_log_normal_delete` values('tbl_log_enter_scene');
insert into `tbl_log_normal_delete` values('tbl_log_leave_scene');
insert into `tbl_log_normal_delete` values('tbl_log_xianxie');
insert into `tbl_log_normal_delete` values('tbl_log_scene');
insert into `tbl_log_normal_delete` values('tbl_log_soul');
insert into `tbl_log_normal_delete` values('tbl_log_equip_intensify');
insert into `tbl_log_normal_delete` values('tbl_log_equip_upgrade');
insert into `tbl_log_normal_delete` values('tbl_log_equip_intensifyback');
insert into `tbl_log_normal_delete` values('tbl_log_equip_puton');
insert into `tbl_log_normal_delete` values('tbl_log_equip_putoff');
insert into `tbl_log_normal_delete` values('tbl_log_equip_advancedActiveSkill');
insert into `tbl_log_normal_delete` values('tbl_log_player_dead');
insert into `tbl_log_normal_delete` values('tbl_log_player_kill');
insert into `tbl_log_normal_delete` values('tbl_log_npc_name');
insert into `tbl_log_normal_delete` values('tbl_log_char_fight_info');
insert into `tbl_log_normal_delete` values('tbl_log_char_pk');
insert into `tbl_log_normal_delete` values('tbl_log_team_activity');
insert into `tbl_log_normal_delete` values('tbl_log_talent');
insert into `tbl_log_normal_delete` values('tbl_log_reborn_info');
insert into `tbl_log_normal_delete` values('tbl_log_compose_info');
insert into `tbl_log_normal_delete` values('tbl_log_enter_team');
insert into `tbl_log_normal_delete` values('tbl_log_leave_team');
insert into `tbl_log_normal_delete` values('tbl_log_enter_activity');
insert into `tbl_log_normal_delete` values('tbl_log_leave_activity');
insert into `tbl_log_normal_delete` values('tbl_log_xiuxingta');
insert into `tbl_log_normal_delete` values('tbl_log_add_friend');
insert into `tbl_log_normal_delete` values('tbl_log_delete_friend');
insert into `tbl_log_normal_delete` values('tbl_log_jifenpoint');
insert into `tbl_log_normal_delete` values('tbl_log_dataosha_point');
insert into `tbl_log_normal_delete` values('tbl_log_player_logout_position');
insert into `tbl_log_normal_delete` values('tbl_log_commerce_skill');
insert into `tbl_log_normal_delete` values('tbl_log_specialize_smithing');
insert into `tbl_log_normal_delete` values('tbl_log_expert_smithing');
insert into `tbl_log_normal_delete` values('tbl_log_cultivate_flowers');
insert into `tbl_log_normal_delete` values('tbl_log_item_vendue');
insert into `tbl_log_normal_delete` values('tbl_log_item_vendue_end');
insert into `tbl_log_normal_delete` values('tbl_log_equip_intensify_level_up');
insert into `tbl_log_normal_delete` values('tbl_log_exp_change');
insert into `tbl_log_normal_delete` values('tbl_log_char_fighting_evaluation');
insert into `tbl_log_normal_delete` values('tbl_log_tong_resource_sig_up');
insert into `tbl_log_normal_delete` values('tbl_log_coupons_info');
insert into `tbl_log_normal_delete` values('tbl_log_char_bought_coupons');
insert into `tbl_log_normal_delete` values('tbl_log_tong_break');
insert into `tbl_log_normal_delete` values('tbl_log_item_del');
insert into `tbl_log_normal_delete` values('tbl_log_join_activity');