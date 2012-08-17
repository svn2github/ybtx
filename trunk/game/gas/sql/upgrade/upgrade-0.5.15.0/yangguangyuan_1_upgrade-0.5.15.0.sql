create table tbl_cpu_status
(
	s_uId							tinyint unsigned     	not null,  
	cs_uMaxThreadUsage	float	unsigned			not null,
	cs_uAllUsage				float unsigned			not null,
	primary key(s_uId),
	foreign	key(s_uId)	references tbl_server(s_uId) on	update cascade on delete cascade
)engine=innodb;