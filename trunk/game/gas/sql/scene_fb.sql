################################################################
#		场景副本信息
################################################################

#杀怪类副本，记录Boss状态
#create table tbl_scene_boss_state
#(
#	 sc_uId							bigint	unsigned	not null,
#	 sbs_uBossId					tinyint unsigned 	not null,
#	 sbs_uBossState				tinyint unsigned 	not null,
#	 
#	 primary key (`sc_uId`, `sbs_uBossId`),
#	 foreign key (`sc_uId`) references tbl_scene (`sc_uId`) on update cascade on delete cascade
#)engine=innodb;

create table tbl_char_dataoshapoint		#dataosha dungeon
(
	cs_uId			bigint unsigned		not null,		#role identifier
	cs_uWinNum		bigint	unsigned	not null default 0,	#win times
	cs_uLoseNum		bigint	unsigned	not null default 0,	#lose times
	cs_uRunNum		bigint	unsigned	not null default 0,	#flee times
	cs_udataoshapoint	bigint	unsigned	not null default 0,	#total marks
	cd_uInFbTimes		bigint	unsigned	not null default 0,	#total take times
	cd_dDate 		datetime 		not null default 0,	#last join game time
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_char_dataoshaaccum		#datasha dungeon statistic
(
	cs_uId			bigint unsigned		not null,		#role identifier
	cs_uUpWeekWinNum	bigint	unsigned	not null default 0,	#win times of last week
	cs_uUpWeekLoseNum	bigint	unsigned	not null default 0,	#lose times of last week
	cs_uUpWeekRunNum	bigint	unsigned	not null default 0,	#flee times of last week
	cs_uUpWeekPoint 	bigint	unsigned	not null default 0,	#total marks of last week
	cs_dUpDate 		datetime not null,#比赛时间			#last time joining game of last week
	cs_uCurrWeekWinNum	bigint	unsigned	not null default 0,	#win times of this week
	cs_uCurrWeekLoseNum	bigint	unsigned	not null default 0,	#lose times of this week
	cs_uCurrWeekRunNum	bigint	unsigned	not null default 0,	#flee times of this week
	cs_uCurrWeekPoint 	bigint unsigned		not null default 0,	#total marks of this week
	cs_dCurrDate 		datetime not null,#比赛时间			#last time joining game of this week
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#插入角色积分赛的积分信息
create table tbl_char_jifensaipoint		#jifensai dungeon
(
	cs_uId			bigint unsigned		not null,		#role identifier
	cs_uType		tinyint	unsigned	not null,		#game type: 
										#  1: 2-players
										#  2: 3-players
										#  3: 5-players
	cs_uWinNum		int	unsigned	not null default 0,	#win times
	cs_uLoseNum		int	unsigned	not null default 0,	#lose times
	cs_uRunNum		int	unsigned	not null default 0,	#flee times
	cs_uPoint		int	unsigned	not null default 20000, #marks for waiting while getting start to join games
	cj_uInFbTimes	int	unsigned		not null default 0,	#total take times
	cj_dDate 		datetime 		not null default 0,	#last time of joing game
	cj_uKillNum		int	unsigned	not null default 0,	#killed number
	cj_uDeadNum		int	unsigned	not null default 0,	#dead times
	primary key (cs_uId, cs_uType),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#插入角色积分赛的积分累计信息
create table tbl_char_jifensaiaccum
(
	cs_uId				bigint unsigned		not null,
	cs_uType				tinyint	unsigned	not null,#1表示2人赛,2表示3人赛,3表示5人赛
	cs_uUpWeekWinNum		int	unsigned		not null default 0,
	cs_uUpWeekLoseNum		int	unsigned		not null default 0,
	cs_uUpWeekRunNum		int	unsigned		not null default 0,
	cs_uUpWeekPoint int	unsigned		not null default 0,#上周的荣誉积分
	cs_dUpDate 			datetime not null,#比赛时间
	cs_uCurrWeekWinNum		int	unsigned		not null default 0,
	cs_uCurrWeekLoseNum		int	unsigned		not null default 0,
	cs_uCurrWeekRunNum		int	unsigned		not null default 0,
	cs_uCurrWeekPoint int	unsigned		not null default 0,#本周的荣誉积分
	cs_dCurrDate 			datetime not null,#比赛时间
	primary key (cs_uId, cs_uType),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#插入角色积分赛的积分信息
create table tbl_fbvar_award
(
	fa_Name			varchar(32) 			not null,
	fa_Date			datetime not null,#奖励的时间
	primary	key(fa_Name)
)engine=innodb;

#区域副本任务 完成情况记录表
#create table tbl_area_fb_quest
#(
#	cs_uId								bigint unsigned		not null,
#	aq_uAcceptCount 			tinyint unsigned  not null,#当天(相对而言)领取的区域副本任务次数 不能超过2次
#	aq_uFinishCount				tinyint unsigned  not null,#当天提交的区域副本任务次数 不能超过2次
#	aq_dtLastAcceptTime 	datetime,									 #当天最后一次领取区域副本任务时间
#	aq_dtLastFinishTime 	datetime,									 #当天最后一次提交区域副本任务时间
#	primary key(cs_uId),
#	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
#)engine=innodb;

#奖励积分
create table tbl_area_fb_point
(
	cs_uId								bigint unsigned				not	null,
	afp_uPointType				bigint	unsigned			not	null,           #积分类型 1.自然积分 2.暗黑积分 3.破坏积分 4.大逃杀积分 5.积分赛积分 6.护甲积分
	afp_uPoint						bigint	unsigned			not	null default 0, #积分
	
	primary	key(cs_uId,afp_uPointType),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#副本活动参加次数
create table tbl_activity_count
(
	cs_uId								bigint unsigned		not	null,
	ac_sActivityName			varchar(32) 			not null,
	ac_dtTime							datetime					not null,
	key(cs_uId),
	key(ac_sActivityName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

#副本活动参加历史总次数和成功次数，不清空
create table tbl_activity_history_count
(
	cs_uId								bigint unsigned		not	null,
	ac_sActivityName			varchar(32) 			not null,
	ac_HistoryTimes				bigint unsigned		not null default 0,
	ac_SuccessfullTimes		bigint unsigned		not null default 0,  # 成功次数
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

#参加活动的总次数
create table tbl_activity_in_count
(
	cs_uId								bigint unsigned		not	null,
	aic_sActivityName			varchar(32) 			not null,
	aic_uInTimes					bigint unsigned		not null,
	primary key (cs_uId, aic_sActivityName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

# 记录累积活动最后一次参加的日期和那天累积的次数
create table tbl_activity_store_data
(
	cs_uId                bigint unsigned   not null,
	asd_sActivityName     varchar(32) 			not null,
	asd_dtTime            datetime					not null,
	asd_uStoreTimes       tinyint unsigned 	not null default 0,
	key(cs_uId),
	key(asd_sActivityName),
	foreign	key(cs_uId) references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

#享受额外奖励的次数
create table tbl_extra_reward_count
(
	cs_uId								bigint unsigned		not	null,
	erc_sActivityName			varchar(32) 			not null,
	erc_dtTime						datetime					not null,
	key(cs_uId),
	key(erc_sActivityName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

#修行塔已得到的关卡卡牌
create table tbl_merc_educate_card
(
	cs_uId								bigint unsigned		not	null,
	mec_uCardId	tinyint	unsigned			not null,
	
	primary key(cs_uId, mec_uCardId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;


#副本进入人员记录, 如果再次进入该副本将跳过所有检测
create table tbl_char_entered_scene
(
	sc_uId	bigint	unsigned	not null,
	cs_uId	bigint unsigned		not	null,
	primary key(sc_uId, cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign key(sc_uId) references tbl_scene(sc_uId) on update cascade on delete cascade
)engine = innodb;



#活动房间
create table tbl_action_match_list
(
	cs_uId	bigint unsigned		not	null,		#角色id
	aml_sActionName	varchar(32) 		not null,  	#报名的活动名
	aml_uLvStep	tinyint unsigned  	not null,	#等级段
	aml_uGroupId bigint unsigned 		not null, #用来分组的id,id相同的要分在一组
	aml_uWaitingTeammate tinyint unsigned  	not null default 0, #等待队友确认进场，为0时不进行后面的配队
	unique key(cs_uId, aml_sActionName),
	key(aml_sActionName, aml_uLvStep),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

#活动房间
create table tbl_action_room
(
	ar_uId 		bigint unsigned		not null auto_increment,
	ar_sActionName	varchar(32) 		not null,  	#报名的活动名
	ar_uState	tinyint unsigned 	not null,	#房间所处的状态, 0:排队状态(高速中还没创建场景), 1:高速中已经创建场景, 2:比赛中不可进人
	ar_uLvStep	tinyint unsigned  	not null,	#等级段
	sc_uId		bigint unsigned, 			#活动分配的场景id
	primary key(ar_uId),					
	foreign key(sc_uId) references tbl_scene(sc_uId) on update cascade on delete cascade
)engine = innodb;

#活动队伍,  区别于普通队伍, 虽然需要组队报名的活动, 活动队伍成员和普通队伍成员刚开始时候一样,但普通队伍发生改变时候不会影响活动队伍成员
create table tbl_action_team
(
	at_uId		bigint unsigned		not null		auto_increment,
	cap_sName       varchar(32),                 			 #队长名
	ar_uId		bigint	unsigned		not null,		#活动房间,为空的时候说明还没有分配房间,属于排队阶段
	at_uTeamLevel	int	unsigned		not null default 0,#队伍的等级
	at_uOtherInfo	int	unsigned		not null default 0,#现在为帮会的ID
	at_uTempPoint	bigint unsigned		not null default 0,#队伍用,临时的积分
	at_uIndex tinyint unsigned 		not null default 0,#确定队伍在Fb里位置的标识
	primary key(at_uId),
	foreign key(ar_uId) references tbl_action_room(ar_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_action_team_member		#队伍活动成员表
(
	cs_uId	bigint unsigned		not	null,		#角色id
	at_uId	bigint unsigned		not null,		#活动队伍表
	atm_uState	tinyint unsigned 	not null default 0, #角色状态  0代表还未准备, 1代表已经准备好, 2代表已进场
	primary key(cs_uId, at_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign key(at_uId) references tbl_action_team(at_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_warn_value		#反挂机警告值表
(
	cs_uId	bigint unsigned		not	null,		#角色id
	w_uValue smallint unsigned not null default 0,	#警告值
	w_dtTime datetime not null default 0,	#处罚时间
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

create table tbl_action_xianxie		#鲜血试炼场得分记录
(
	cs_uId	bigint unsigned		not	null,		#角色id
	ax_uWin tinyint not null default 0,		#胜利
	ax_uScore tinyint not null default 0,		#队伍分
	ax_dtTime datetime not null default 0,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;

create table tbl_scene_dragoncave   #佣兵小队龙穴活动副本列表
(
  t_uId 	bigint 	unsigned	not	null,    #佣兵小队id  
  sc_uId	bigint	unsigned	not	null,	 	#scene idenfier
  primary key(sc_uId),
  foreign key(t_uId) references tbl_tong(t_uId) on update cascade on delete cascade,
	foreign key(sc_uId) references tbl_scene(sc_uId) on delete cascade
)engine = innodb;

create table tbl_tong_dragoncave   #龙穴活动副本已完成佣兵小队角色列表
(
  cs_uId 	bigint	unsigned	not	null,    #角色id
  sc_sFinishTime datetime  not null,	#活动完成时间
	primary key(cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_tong_dragoncavetong   #龙穴活动副本已完成佣兵小队列表
(
  t_uId	 bigint	unsigned	not	null, #佣兵小队id
  sc_sFinishTime datetime  not null,	#活动完成时间
	primary key(t_uId),
	foreign key(t_uId) references tbl_tong(t_uId) on update cascade on delete cascade
)engine = innodb;