
################################################################
#		技能
################################################################
create table tbl_fight_skill
(
	cs_uId 			bigint unsigned 		not null,					#玩家角色ID
	fs_sName		varchar(96)			not null,				#技能名
	fs_uLevel 	tinyint unsigned		not null default 0,			#技能等级
	fs_uKind		tinyint unsigned		not null default 0, 		#普通技能or天赋技能
	primary	key( cs_uId, fs_sName,fs_uKind),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

##技能所属阶段
create table tbl_skill_layer
(
	cs_uId 			bigint unsigned 		not null,							#玩家角色ID
	fs_sName		varchar(96)					not null,							#技能名
	sl_uLayer		tinyint							not null,							#技能所属阶段
	primary	key( cs_uId, fs_sName,sl_uLayer),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

##主修系
create table tbl_skill_Series
(
	cs_uId 			bigint 	unsigned 				not null,						#玩家角色ID
	ss_uSeries  tinyint	unsigned				not null,						#主修系
	primary	key( cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

##每层对应的节点
create table tbl_skill_node
(
	cs_uId 			bigint 	unsigned 				not null,						#玩家角色ID
	sn_uNode		tinyint	unsigned				not null,						#节点
	primary	key( cs_uId,sn_uNode),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_cool_down
(
	cs_uId 					bigint unsigned 		not null,
	cd_sName					varchar(96)			not null,		#技能名
	cd_dCoolDownLeftTime		bigint				not null,		#技能开始冷却时间,既施放时间
	cd_dCoolDownSaveTime		datetime				not null,		#技能冷却保存时间
	cd_bSwitchEquipSkill		tinyint			not null,	#是否为装备技能的切换CD
	primary key(cs_uId,cd_sName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

create table tbl_aure_magic
(
	cs_uId 				bigint unsigned 	not null,
	am_sAureMagicName	char(96)			not null,
	am_uSkillLevel			bigint unsigned		not null,
	am_uSkillName			char(96)			not null,
	am_uAttrackType		bigint unsigned		not null,
	
	primary key(cs_uId,am_sAureMagicName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

create table tbl_char_magicstate
(
	cs_uId				bigint unsigned		not null,
	cms_uMagicName		char(192)			not null,
	cms_uMagicType		tinyint 	  unsigned	not null, 	#1,2,3,4 4种魔法状态
	cms_uMagicTime		bigint			not null,
	cms_uMagicRemainTime	bigint	  		not null,
	cms_uCount			bigint	  		not null,
	cms_uProbability		float				not null,
	cms_uMagicStoreObjID1	bigint	  unsigned,			#魔法效果
	cms_uMagicStoreObjID2	bigint	  unsigned,
	cms_uMagicStoreObjID3	bigint	  unsigned,
	cms_uSkillLevel			bigint unsigned		not null,
	cms_uSkillName		char(192)			not null,
	cms_uAttrackType		bigint unsigned		not null,
	cms_bIsDot			tinyint(1)			not null,
	cms_bFromEqualsOwner	tinyint(1)			not null,
	cms_uServantName		char(192),
	cso_uServantID 	  tinyint unsigned	not null default 0,
	
	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
	
)engine=innodb;

create table tbl_char_magic_obj
(
	cs_uId				bigint unsigned		not null,
	cmo_uMagicStoreObjID	bigint	unsigned		not null,
	cmo_uMagicCountID		bigint 	unsigned		not null,
	cmo_uMagicOperaterID	bigint	unsigned		not null,
	cmo_uData1			bigint				not null,
	cmo_uData2			bigint	unsigned		not null,
	cmo_uServantName		char(192),
	cso_uServantID 	  tinyint unsigned	not null default 0,
	
	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_char_servant_name
(
	cs_uId					bigint 		unsigned	not null,
	csn_sServantResName		varchar(96)	not null,		#召唤兽资源名
	csn_sServantRealName		varchar(96) collate utf8_unicode_ci not null,		#召唤兽名字 -- 玩家定义的
	primary	key( cs_uId, csn_sServantResName),
	foreign 	key(cs_uId) 	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_char_servant
(
	cs_uId					bigint 			unsigned	not null,
	cso_sServantResName		varchar(96)		not null,
	cso_uServantLevel			tinyint	unsigned			not null,
	cso_uServantAIType		tinyint	unsigned			not null,
	cso_uServantType			tinyint(1)			not null,
	cso_uCurHP				int	unsigned		not	null,
	cso_uCurMP				int	unsigned		not	null,
	cso_uLeftTime			bigint unsigned		not null,
	cso_uServantID 	  tinyint unsigned	not null default 0,
	
	key(cs_uId),
	foreign 	key(cs_uId) 	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;