##小队信息表
create table tbl_team_static
(
	t_uId										bigint unsigned		not null		auto_increment,    #team identifier
	primary key(t_uId)
)engine=innodb;

create table tbl_team	   #table of teams
(
	t_uId										bigint unsigned		not null		auto_increment,    #team identifier
  cs_uId									bigint unsigned		not null,                      #team master id
  t_uAssignment           tinyint unsigned  not null default 0,   	#assignment method
  t_uAuctionStandard      tinyint unsigned  not null default 0,     #setting of auction assiment method
  t_uAuctionBasePrice      bigint unsigned  not null default 0,
    
  primary key(t_uId),
  foreign	key (t_uId)	  references tbl_team_static(t_uId) on	update cascade on delete cascade,
  foreign	key (cs_uId)	  references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

##小队成员表
create table tbl_member_team		#teams' members
(
	cs_uId										bigint unsigned		not null,         #member identifier
  t_uId											bigint unsigned		not null,		 			#team identifier
  primary key(cs_uId), 
  key(t_uId),
  foreign	key (cs_uId)	    references tbl_char_static(cs_uId) on	update cascade on delete cascade,
  foreign	key (t_uId)	      references tbl_team(t_uId) on	update cascade on delete cascade
)engine=innodb;

##小队标记
create table tbl_mark_team		#teams' sign
(
	t_uId								bigint unsigned		not null,               #team id
  mt_uMarkType				tinyint unsigned	not null,               #mark type
	mt_uTargetType			tinyint unsigned	not null,								#character type be marked
	mt_uTargetID				bigint unsigned		not null,               #character id be marked
  primary key(t_uId, mt_uMarkType),
	unique key(t_uId,mt_uTargetType,mt_uTargetID),
  foreign	key (t_uId)	references tbl_team(t_uId)         on	update cascade on delete cascade
)engine=innodb;

##队伍场景(队伍所拥有的场景 只用于队伍副本)
create table tbl_scene_team					#teams' scene, used for teams' dungeon
(
	sc_uId								bigint	unsigned	not	null,						#scene id
	t_uId									bigint unsigned		not null,           #team id
	t_uBelongID						bigint unsigned		not null default 0,	#belong player ID
  primary key(sc_uId),
  foreign	key (sc_uId)	  references tbl_scene(sc_uId)   on	update cascade on delete cascade
)engine=innodb;


##小队组队器
create table tbl_team_list (			#list of players apply for a team 
		cs_uId			bigint unsigned		not	null,		#role identifier
		tl_nType    tinyint unsigned	not	null,
		tl_sMsg      char(128) collate utf8_unicode_ci not null,
		tl_dtTime   datetime,
		#tl_dtUGTime   datetime,
		#tl_dtActTime   datetime,
		#tl_dtTaskTime   datetime,
		primary key(cs_uId,tl_nType),
		foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade		
)engine=innodb;
