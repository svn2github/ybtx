
##阵型信息表
create table tbl_battle_shape					#battle array shape
(
	bs_uId bigint unsigned not null auto_increment,		#sequence Idendifier
	bs_sName varchar(96) not null,				#battle array name
	cs_uId bigint unsigned    not null,			#battle array owner identifier
	bs_uPos1 tinyint	unsigned not null default 0,	#1st position
	bs_uPos2 tinyint	unsigned not null default 0,	#2nd position
	bs_uPos3 tinyint	unsigned not null default 0,	#3rd position
	bs_uPos4 tinyint	unsigned not null default 0,	#4th position
	bs_uPos5 tinyint	unsigned not null default 0,	#5th position
	bs_uFlag tinyint	unsigned not null default 0,	#flag 0 stands for non-batter-array; 1 stands for array
	primary key(bs_uId),
	foreign key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

##列阵信息表
create table tbl_battle_array					#battle array used info
(
	ba_uId bigint unsigned not null auto_increment,		#battle array used id
	bs_uId bigint unsigned not null ,			#array type
	t_uId bigint unsigned not null,				#team id
	ba_uLoc1 tinyint	unsigned default null,		#role id on 1st position
	ba_uLoc2 tinyint	unsigned default null,		#role id on 2nd position
	ba_uLoc3 tinyint	unsigned default null,		#role id on 3rd position
	ba_uLoc4 tinyint	unsigned default null,		#role id on 4th position
	ba_uLoc5 tinyint	unsigned default null,		#role id on 5th position
	primary key(ba_uId),
	foreign key(bs_uId)	references tbl_battle_shape(bs_uId) on update cascade on delete cascade,
	foreign key(t_uId)	references tbl_team(t_uId) on update cascade on delete cascade
)engine=innodb;

#阵法技能栏
create table tbl_battle_skill_icon				#battle skill
(
	cs_uId	bigint unsigned  not null,			#role id
	bsi_uPos   bigint unsigned  not null,			#skill icon position (should be in 1 to 8)
	bsi_sName  varchar(96) default null,			#skill name viewed on icon
	primary key(cs_uId,bsi_uPos),
	foreign key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade                                         
)engine=innodb;
