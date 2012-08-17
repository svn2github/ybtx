#佣兵小队发展度
create table tbl_log_tong_develop_degree
(
	le_uId			bigint unsigned		not	null,
	ltdd_uDevelopDegree bigint not null ,
	
	key(le_uId)
 )engine=innodb;