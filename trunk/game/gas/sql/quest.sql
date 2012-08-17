
####################################################################
##			任务相关部分
####################################################################
#任务信息表，里面包括了所有
create table tbl_quest(			#all quest info
	q_sName		varchar(96) 		not null,		#quest name
	cs_uId		bigint unsigned 	not null,		#role
	q_state		tinyint unsigned 	not null, 		#quest state, 1: on-going 2: failed, 3: reached
	q_tAcceptTime	int	unsigned	not null,			#accepted time
	q_tFinishTime	int unsigned	not null default 0,		#finished time
	q_uExpMultiple	tinyint unsigned 	not null,	#exp multiple
	q_uLimitTime	tinyint unsigned 	not null default 0,	#limit time, 1: no-limit
	primary key (cs_uId, q_sName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

create table tbl_char_quest_finish_num(		#quest number that role've finished
	cs_uId		bigint unsigned 	not null,		#role identifier
	cqfn_uFinishNum	smallint unsigned 	not null default 0, 	#quest number that role've finished
	
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;


create table tbl_quest_var(			#quest variables that notes whether quest aims be reached
	q_sName		varchar(96) 		not null,		#quest name
	cs_uId		bigint unsigned 	not null,		#role
	qv_name		varchar(384)		not null default "",	#quest variable name
	qv_num		tinyint unsigned 	not null default 0,	#quest variable number
	primary key (`cs_uId`, `q_sName`, `qv_name`),
	foreign key (`cs_uId`, `q_sName`) references tbl_quest (`cs_uId`, `q_sName`) on update cascade on delete cascade
)engine=innodb;

create table tbl_quest_tempitem(		#items that given when accept a quest
	q_sName		varchar(96) 		not null,	#quest name
	cs_uId		bigint unsigned 	not null,	#role name
	qt_ItemIndex	tinyint unsigned 	not null,	#item index be defined in designers' config file
	primary key (`cs_uId`, `q_sName`),
	foreign key (`cs_uId`, `q_sName`) references tbl_quest (`cs_uId`, `q_sName`) on update cascade on delete cascade
)engine=innodb;

create table tbl_quest_limit(			#repeatable quest
	q_sName		varchar(96) 		not null,		#quest name
	cs_uId		bigint unsigned 	not null,		#role identifier
	ql_Count	tinyint unsigned	not null default 0,	#total reached times
	ql_CountTime	datetime		not null default 0,	#last finished quest time
	ql_CDTime	datetime		not null default 0,	#cool down time
	primary key (cs_uId, q_sName),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=Innodb;

create table tbl_mercenary_level(
	cs_uId					bigint unsigned 	not null,
	ml_uStatus			tinyint unsigned 	not null default 1,
	ml_sSubject1		varchar(32)				not null default "",
	ml_sSubject2		varchar(32)				not null default "",
	ml_sSubject3		varchar(32)				not null default "",
	ml_uCount1			tinyint unsigned 	not null default 0,
	ml_uCount2			tinyint unsigned 	not null default 0,
	ml_uCount3			tinyint unsigned 	not null default 0,
	ml_uCallenge		tinyint unsigned 	not null default 0,
	primary key (cs_uId),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=Innodb;

create table tbl_mercenary_level_award(
	cs_uId					bigint unsigned 	not null,
	mla_sAward			varchar(96)				not null default "", ##玩家已领取的佣兵特权
	primary key (cs_uId, mla_sAward),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=Innodb;

create table tbl_finish_npc_talk(
	cs_uId					bigint unsigned 	not null,
	fnt_sTalkName		varchar(96)				not null default "", ##玩家已完成的交互对话
	primary key (cs_uId, fnt_sTalkName),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=Innodb;

##跑环任务环数记录
create table tbl_quest_loop(
	cs_uId					bigint unsigned 	not null,
	ql_sLoopName		varchar(96)				not null default "",
	ql_uLoopNum			tinyint unsigned 	not null default 0,
	primary key (cs_uId, ql_sLoopName),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=Innodb;

