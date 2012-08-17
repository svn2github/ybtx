################################################################
#		区域
################################################################

#	Scene	信息 保管，暂时只对每个	CoreScene	记录一个 uint64	作为附加信息
create table tbl_area	#table to record areas that whether roles have been descovered or not
(
	 cs_uId			bigint unsigned	 not	null,	#role identifier
	 a_sAreaName		varchar(64) 	not	null, 	#area name 
	 a_bDiscovery		tinyint	unsigned	not	null default 0,  	#whether the area have been discoveried by role
	 
	primary key(cs_uId,a_sAreaName),
	foreign	key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_player_area	#table of roles' current area
(
	 cs_uId				bigint unsigned	 not	null,
	 pa_sAreaName			varchar(64) 	not	null,			#area name which role current be in
	 
	primary key(cs_uId),
	foreign	key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;
