drop table tbl_Scene_distribute;

create table tbl_scene_distribute
(
	sc_sSceneName  varchar(96) 		not	null,		#scene name
	sc_uServerId	tinyint unsigned	not	null,
	primary key(sc_sSceneName)
)engine=innodb;