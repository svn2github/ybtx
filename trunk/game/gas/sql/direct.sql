#玩家已经得到的成就
create table tbl_char_direct
(
	cs_uId										bigint unsigned not null,
	cd_sDirectName		        varchar(192) not	null,
	cd_uState									tinyint not null default 0,
	primary	key(cs_uId, cd_sDirectName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

#玩家的成就状态表
create table tbl_char_direct_state
(
	cs_uId		         				bigint unsigned not null,
	cds_sDirectName		        varchar(192) not	null,
	cds_uCount	 	 							tinyint unsigned not null,
	primary	key(cs_uId, cds_sDirectName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

#玩家的成就奖励表
create table tbl_char_direct_award
(
	cs_uId		         				bigint unsigned not null,
	cda_sAwardName		        varchar(192) not	null,
	cda_uState	 	 						tinyint unsigned not null,
	primary	key(cs_uId, cda_sAwardName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;
