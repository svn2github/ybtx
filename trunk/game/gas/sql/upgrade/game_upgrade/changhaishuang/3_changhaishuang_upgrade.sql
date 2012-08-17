create table tbl_char_welfare
(
	cs_uId		bigint unsigned		not	null,
	cw_sBuffName varchar(96) not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;