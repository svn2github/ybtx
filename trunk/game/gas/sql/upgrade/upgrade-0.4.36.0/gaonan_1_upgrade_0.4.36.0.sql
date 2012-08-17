##Boss争夺战玩家击杀Boss次数
create table tbl_tong_char_boss
(
	cs_uId					bigint unsigned 	not null,
	tcb_uNum				bigint unsigned		not null		default 0,
	primary key(cs_uId),
	foreign	key(cs_uId)	 references tbl_char_static(cs_uId) 	on	update cascade on delete cascade
)engine=innodb;


insert into tbl_database_upgrade_record values("gaonan_1_upgrade_0.4.36.0.sql");
