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
	
	key(le_uId),
	key(lts_uId),
	key(ltbe_sName)
 )engine=innodb;
 
 alter table tbl_log_char_login add column lcs_uId 			bigint unsigned			not null;
 alter table tbl_log_char_login add column lcl_uLevel tinyint unsigned			not null;
 
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
create table tbl_log_tong_building_move
(
	le_uId	bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltbm_uExecutorId bigint unsigned	not null, #执行者
	ltbm_uOldPosX        	float        						not null, 
	ltbm_uOldPosY        	float 				        	not null, 
	ltbm_uNewPosX        	float        						not null, 
	ltbm_uNewPosY        	float 				        	not null, 

	key(le_uId),
	key(lts_uId)
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
	le_uId	bigint unsigned		not	null,
	ltce_uChallengeTongId  int unsigned	not null, #挑战佣兵小队Id
	ltce_uRecoveryTongId  int unsigned	not null, #防御佣兵小队Id
	ltce_uWarzoneId    int unsigned  not null, #战区id
	ltce_uIndex      int unsigned 	not null, #驻地id
	ltce_uResult   smallint unsigned	not null , #挑战胜负 0 - 失败 1 - 成功
	ltce_dtEndTime datetime	not null , #结束时间

	key(le_uId),
	key(ltce_uChallengeTongId),
	key(ltce_uRecoveryTongId),
	key(ltce_uWarzoneId)
 )engine=innodb;
 
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


#装备追加
create table tbl_log_item_equip_superaddation(
	lis_uId          bigint unsigned not null,
	lies_uCurSuperaddPhase tinyint unsigned not null,
	lies_uMaxSuperaddPhase tinyint unsigned not null,
	
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

create table tbl_log_team_activity
(
	le_uId bigint unsigned not null,
	lta_uSuccTeamId bigint unsigned not null,
	lta_uFailTeamId bigint unsigned not null,
	lta_uCostTime bigint unsigned not null,
	lta_uActivityType tinyint unsigned not null,
	
	key(le_uId),
	key(lta_uActivityType)
)engine = innodb;