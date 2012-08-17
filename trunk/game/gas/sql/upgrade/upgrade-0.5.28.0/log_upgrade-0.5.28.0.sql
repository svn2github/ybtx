drop table tbl_log_tong_building_move;

create table tbl_log_tong_station_move
(
	le_uId	bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #佣兵小队Id
	ltsm_uExecutorId bigint unsigned	not null, #执行者
	ltsm_uEventType tinyint unsigned	not null, #迁移类型
	ltsm_uOldWarzoneId  int  not null, 
	ltsm_uOldIndex      int not null, 
	ltsm_uNewWarzoneId int  not null, 
	ltsm_uNewIndex    int not null, 

	key(le_uId),
	key(lts_uId),
	key(ltsm_uEventType)
 )engine=innodb;