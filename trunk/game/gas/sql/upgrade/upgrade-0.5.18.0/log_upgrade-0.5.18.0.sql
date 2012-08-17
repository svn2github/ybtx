
drop table if exists tbl_log_item_equip_superaddation ;

#×°±¸×·¼Ó
create table tbl_log_item_equip_superaddation(
	le_uId			bigint unsigned 	not null,
	lis_uId          bigint unsigned not null,
	lies_uCurSuperaddPhase tinyint unsigned not null,
	
	key(le_uId),
	key (lis_uId)
)engine = innodb;