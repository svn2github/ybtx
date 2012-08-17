


################################################################
#		主角
################################################################


#角色的静态信息，主角一旦被创建，就会永远呆在这张表里面
create table tbl_char_static				#role info which will never be changed
(
	cs_uId		bigint unsigned	 not	null auto_increment,	#role identifier
	us_uId		int	unsigned not	null,	#user to which the role belongs
	cs_sHair	varchar(96)	 not	null,	#role hair style			
	cs_sHairColor	varchar(96)	 not	null,	#role hair color
	cs_sFace	varchar(96)	 not	null,	#role face style
	cs_uScale	tinyint	unsigned not	null,	#role scale rate
	cs_uSex		tinyint	unsigned not	null,	#role sex
	cs_uClass	tinyint	unsigned not	null,	#role occupation
	cs_uCamp	tinyint	unsigned not	null,	#role camp
	cs_dtCreateDate datetime not null, 		#role created time
	
	primary	key(cs_uId),
	key(cs_uCamp),
	key(cs_uClass),
	foreign	key(us_uId)	references tbl_user_static(us_uId) on	update cascade
)engine=innodb;


#未删除的角色列表
create table tbl_char		#actively roles 
(
	cs_uId		bigint	unsigned	not	null,	#role identifier
	c_sName		char(32) binary collate utf8_unicode_ci not null, #role name
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade,
	unique key(c_sName)
)engine=innodb;


#角色最后一次登录、登出记录表
create table tbl_char_onlinetime	#roles last login info
(
	cs_uId			bigint	unsigned	not	null, 	#role identifier
	co_dtLastLoginTime	datetime not null,			#role last login time
	co_dtLastLogOutTime	datetime not null,			#role last logout time
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#角色在线信息表
create table tbl_char_online	
(
	cs_uId			bigint	unsigned	not	null, 	#role identifier
	co_uOnServerId		tinyint	unsigned	not null,	#server that role is in
	
	primary key(cs_uId),
	key(co_uOnServerId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#剩余在线时间，防沉迷
create table tbl_char_lefttime	
(
	cs_uId				bigint	unsigned	not	null, 	#role identifier
	cl_dtLeftTime	bigint	unsigned	not null,
	
	unique key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


#已被删除的角色列表。
create table tbl_char_deleted		#deleted roles
(
	cs_uId			bigint	unsigned	not	null,   	#role identifier
	cd_sName		char(32) collate utf8_unicode_ci not null,	#role name that be used while be avtived
	cd_dtDelTime 		datetime not null,				#deleted time
	cd_uState		tinyint unsigned not null, 			#role view state. 
										#  1 stands for view as a deleted role
										#  2 stands for never view again
	
	primary	key(cs_uId),
	key(cd_uState),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;



create table tbl_char_basic		#role informations that would be changed during game time
(
	cs_uId			bigint unsigned		not	null,		#role identifier
	cb_uLevel		tinyint unsigned	not	null default 1,	#role level
	cb_uMercenaryLevel	tinyint unsigned	not	null default 0,	#role mercenary level
	cb_uMoneyType	tinyint	unsigned	not null default 2,		#money type when role buy sth. from npc shop

	primary	key(cs_uId),
	key (cb_uLevel),
	key (cb_uMercenaryLevel),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#玩家金钱表
create table tbl_char_money
(
	cs_uId			bigint unsigned		not	null,
	cm_uMoney		bigint unsigned	not null  default 0,		#role money 
	cm_uBindingMoney	bigint unsigned	not null	default 0,	#role binding money
	cm_uBindingTicket	bigint unsigned	not null 	default 0,	#role binding tickets
	cm_uRmbMoney	bigint unsigned	not null 	default 0,			#rmb
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_char_position		#role position of main scene
(
	cs_uId			bigint	unsigned	not	null,		#role identifier
	sc_uId			bigint	unsigned	not	null,		#scene identifier
	cp_uPosX		float	unsigned	not	null default 1, #X-axis position of main scene
	cp_uPosY		float	unsigned	not	null default 1, #Y-axis position of main scene
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade,
	foreign	key(sc_uId)	references tbl_scene(sc_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_char_fb_position	#role position of dungeon (priority is higher than tbl_char_position)
(
	cs_uId			bigint	unsigned	not	null,		#role identifier
	sc_uId			bigint	unsigned	not	null,		#scene identifier
	cfp_uPosX		float	unsigned	not	null default 1,	#X-axis position of dungeon 
	cfp_uPosY		float	unsigned	not	null default 1, #Y-axis position of dungeon
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign key(sc_uId) references tbl_scene(sc_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_char_change_position		#role change scene info
(
	cs_uId			bigint	unsigned	not	null,		#role identifier
	sc_uId			bigint	unsigned	not	null,		#scene which to be changed to
	ccp_uPosX		float	unsigned	not	null default 1, #X-axis TO BE position
	ccp_uPosY		float	unsigned	not	null default 1, #Y-axis TO BE position
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign key(sc_uId) references tbl_scene(sc_uId) on update cascade on delete cascade
)engine=innodb;


create table tbl_char_experience		#roles' experiences info
(
	cs_uId			bigint unsigned		not	null,		#role identifier
	cs_uLevelExp		bigint unsigned		not	null,		#role left experience
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#佣兵积分（升佣兵等级所用的积分）
create table tbl_char_integral
(
	cs_uId			bigint unsigned		not	null,		#role identifier
	cs_uLevelIntegral	bigint	unsigned	not	null,		#role mercanery level mark
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_char_non_combat		#role no-combat skills
(
	cs_uId			bigint unsigned		not	null,		#role identifier
	cnc_uInspiration	smallint  unsigned	not	null, 		#role inspiration mark
	cnc_uPrepExp		bigint unsigned		not	null, 		#role experience that added when off line
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


create table tbl_char_status
(
	cs_uId				bigint unsigned		not	null,
	cs_uCurHP			int	unsigned		not	null,
	cs_uCurMP			int	unsigned		not	null,
	cs_uPK 				smallint unsigned	not	null default 0,
	cs_uDuelWinCount 		int unsigned		not	null default 0,
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

# 记录玩家等级升级的时刻
create table tbl_char_levelup_time
(
	cs_uId bigint unsigned not null,
	clt_uLevelUpTime datetime not null default 0, #记录等级升级的时刻
	clt_uLevel tinyint unsigned	not null default 1, #role level,
	key(cs_uId),
	key(clt_uLevel),
	foreign	key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#玩家身上的魂值
create table tbl_char_soul		#role soul info
(
	cs_uId bigint unsigned not null,			#role identifier
	cs_uSoulNum bigint unsigned not null default 0,		#role soul number
	cs_uBurstSoulTimes bigint unsigned not null default 0,  #role used brust soul skill times

	primary key(cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


create table tbl_char_fight_info	#role fight info
(		
	cs_uId bigint unsigned not null, 			#role identifier
	cfi_uDeadTimes bigint unsigned default 0,  		#role dead times
	cfi_uKillNpcCount bigint unsigned default 0,		#role kill npc amount
	cfi_uKillPlayerCount bigint unsigned default 0,		#role kill players amount
	cfi_uPlayerOnTimeTotal bigint unsigned default 0,	#role accumulated online time

	primary key(cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#玩家当前的称谓
create table tbl_char_appellation	#role appellations
(
	cs_uId bigint unsigned not null,			#role identifier
	ca_uAppellationId1 smallint unsigned not null default 0,				#appellation1
	ca_uAppellationId2 smallint unsigned not null default 0,				#appellation2
	
	primary key(cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#玩家所有的称谓
create table tbl_char_appellationList	#role appellations defined in desiners' config file
(
	cs_uId bigint unsigned not null,			#role identifier
	ca_uAppellation int unsigned not null,				#appellation index defined in designers' config file
	
	primary key(cs_uId,ca_uAppellation),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


#玩家PK挑战次数
create table tbl_duel_count
(
	cs_uId								bigint unsigned		not	null,
	dc_uChallengerId			bigint unsigned		not null,
	dc_dtTime							datetime					not null,
	
	primary key(cs_uId,dc_uChallengerId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#玩家pk积分
create table tbl_char_pk
(
	cs_uId						bigint unsigned		not	null,
	cp_dtTime					datetime				not null,
	cp_uMin						bigint unsigned		not	null,
	cp_uMax						bigint unsigned		not	null,
	primary key(cs_uId,cp_uMin,cp_uMax),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#玩家获得pk积分
create table tbl_char_pk_flag
(
	cs_uId						bigint unsigned		not	null,
	cpf_uId						bigint unsigned		not	null,
	cpf_uFlagTime						bigint unsigned		not	null,
	primary key(cs_uId,cpf_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;



#玩家战斗力评分
create table tbl_char_fighting_evaluation
(
	cs_uId			bigint unsigned		not	null,
	cfe_uPoint	float unsigned not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家装备特效
create table tbl_char_equip_effect
(
	cs_uId			bigint unsigned		not	null,
	cee_uOrder	smallint unsigned not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家离线经验值表
create table tbl_char_off_line_exp
(
	cs_uId			bigint unsigned		not	null,
	cm_uOfflineExp		bigint unsigned	not null  default 0,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家每天离线经验领取上限
create table tbl_char_off_line_exchage_limit_value
(
	cs_uId						bigint unsigned	not	null,
	cd_sLastExChangeDataTime  	varchar(32) not null,
	cm_uAfflatusValue			bigint unsigned	not null  default 0,
	foreign	key(cs_uId)			references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家灵感值表
create table tbl_char_afflatus_value
(
	cs_uId					bigint unsigned	not	null,
	cm_uAfflatusValue		bigint unsigned	not null  default 0,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家拍卖金钱表
create table tbl_char_auction_money
(
	cs_uId			bigint unsigned		not	null,
	cam_uObjId		bigint unsigned		not	null,
	cam_uMoney		bigint unsigned	not null  default 0,		#role money 
	cam_dtAddDate datetime not null,
	
	primary	key(cs_uId,cam_uObjId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

#玩家通过招募创建角色
create table tbl_char_recruit_info
(
	cs_uId			bigint unsigned		not	null,
	cri_uRecFlag	tinyint	unsigned not	null,	#recruit flag
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#招募更新
create table tbl_char_recruit_time
(
	crt_dtTime	datetime not null
)engine = innodb;


#玩家VIP特效
create table tbl_char_vip_effect
(
	cs_uId			bigint unsigned		not	null,
	cve_dtUseTime  datetime not null,
	cve_sItemName varchar(96) not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#玩家佣兵福利奖励
create table tbl_char_welfare
(
	cs_uId		bigint unsigned		not	null,
	cw_sBuffName varchar(96) not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


