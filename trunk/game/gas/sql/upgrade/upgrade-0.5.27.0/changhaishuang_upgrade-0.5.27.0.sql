alter table tbl_user_online add column uo_sMacAddress	 varchar(16) not null;

#玩家装备特效
create table tbl_char_equip_effect
(
	cs_uId			bigint unsigned		not	null,
	cee_uOrder	smallint unsigned not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;
