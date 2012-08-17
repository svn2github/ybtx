create table tbl_server_attribute
(
	s_uId							tinyint unsigned     	not null,  #server id
	sa_bCanLogin			tinyint unsigned     	not null,
	sa_bNoFbScene			tinyint unsigned     	not null,
	sa_bNoMainScene		tinyint unsigned     	not null,
	primary key(s_uId),
	foreign	key(s_uId)	references tbl_server(s_uId) on	update cascade on delete cascade
)engine=innodb;

insert into
	tbl_server_attribute(s_uId,sa_bCanLogin, sa_bNoFbScene, sa_bNoMainScene)
values
	(1,1,0,0);

create table tbl_Scene_distribute
(
	sc_sSceneName  varchar(96) 		not	null,		#scene name
	sc_uServerId	tinyint unsigned	not	null,
	primary key(sc_sSceneName)
)engine=innodb;