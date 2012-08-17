create table tbl_warn_value		#反挂机警告值表
(
	cs_uId	bigint unsigned		not	null,		#角色id
	w_uValue smallint unsigned not null default 0,	#警告值
	w_dtTime datetime not null default 0,	#处罚时间
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;