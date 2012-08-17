create table tbl_war_exploit_time			#tong trunk
(       
	cs_uId		bigint unsigned 	not null,			#role
	we_uId		bigint unsigned 	not null,			#role
	we_uTime	bigint unsigned   not null,		#get exploit time
	primary key(cs_uId, we_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;