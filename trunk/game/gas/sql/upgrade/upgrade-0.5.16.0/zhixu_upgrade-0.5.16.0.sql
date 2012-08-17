create table tbl_activity_in_count
(
	cs_uId					bigint unsigned		not null,
	aic_sActivityName			varchar(32) 		not null,
	aic_uInTimes				bigint unsigned		not null,
	primary key (cs_uId, aic_sActivityName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;
