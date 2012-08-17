##生活技能
create table tbl_commerce_skill
(
	cs_uSID						bigint unsigned					not	null auto_increment,
	cs_uId 						bigint unsigned 		not null,
	cs_uSkillType			tinyint	unsigned		not null,		##技能类型，0是默认学会的，1是可以学会多种，大于1是只能学习一种的
	cs_sSkillName			varchar(96)					not null,		#技能名称
	cs_uSkillLevel		tinyint	unsigned		not null,		#技能等级
	cs_uExperience		bigint	unsigned		not null,		#经验值
	primary	key(cs_uSID),
	unique key(cs_uId,cs_uSkillType,cs_sSkillName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

##学会的配方
create table tbl_skill_directions
(
	cs_uId 						bigint unsigned 		not null,
	sd_sName					varchar(96)					not null, ##学会的配方
	sd_sSkillName				varchar(96)					not null,  ##所属技能
	sd_sDirectionType		varchar(96)					not null,  ##配方类型
	unique key(cs_uId,sd_sSkillName,sd_sDirectionType,sd_sName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

create table tbl_directions_cool_time
(
	cs_uId 						bigint unsigned 		not null,
	sd_sName					varchar(96)					not null, ##学会的配方
	sd_sSkillName				varchar(96)					not null,  ##所属技能
	sd_sDirectionType		varchar(96)					not null,  ##配方类型
	sd_dtStartTime			datetime not null, #冷却开始时间
	unique key(cs_uId,sd_sSkillName,sd_sDirectionType,sd_sName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

##锻造熟练度
create table tbl_specialize_smithing
(
	cs_uId 						bigint unsigned 		not null,
	ss_uSkillType			tinyint unsigned		not null,			#所属技能类型、锻造或铸甲等
	ss_uType					varchar(96)						not null,		#武器熟练度类型
	ss_uSpecialize		bigint	unsigned		not null,		#熟练度大小
	unique key(cs_uId,ss_uType),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

##锻造专精
create table tbl_expert_smithing
(
	cs_uId 						bigint unsigned 		not null,		
	es_uSkillType			tinyint unsigned		not null,			#所属技能类型、锻造或铸甲等
	es_uType					varchar(96)					not null,		#专精类型
	es_uLevel					tinyint	unsigned		not null,		#专精等级
	unique key(cs_uId,es_uSkillType,es_uType),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

##花卉
create table tbl_flower
(
	cs_uId 						bigint unsigned 		not null,		#玩家ID
	f_sFlowerName			varchar(150)				not null,		#花卉名称
	f_dStartTime			datetime 						not null,		#开始种植时间
	f_dCoolDownTime		datetime						default null,		#花卉技能冷却时间
	f_uHealthPoint		int unsigned 				not null default 100,		#健康度
	f_uMaxHealth			int unsigned 				not null default 100,		#达到最高健康度
	f_uGetCount				int unsigned 				not null default 0,		#可采集次数 
	f_uLevel					int unsigned 				not null default 1,		#等级
	f_uEventId				bigint unsigned 		not null default 0,		#事件ID
	
	key(cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;