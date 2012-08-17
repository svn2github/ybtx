
drop table if exists tbl_item_store_room_lock;
create table tbl_item_store_room_lock	
(
	cs_uId		bigint 	unsigned	not null,		
	isrl_uTime	int 	unsigned not null,			
	isrl_dtLockTime datetime  not null,

	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

drop table tbl_char_server_lastleve;
drop table tbl_char_last_levelup;