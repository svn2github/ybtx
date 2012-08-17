drop table if exists tbl_cpu_status;

create table tbl_cpu_status
(
	s_uId							tinyint unsigned     	not null,  
	cs_uMaxThreadUsage	float	unsigned			not null,
	cs_uAllUsage				float unsigned			not null,
	primary key(s_uId)
)engine=memory;
