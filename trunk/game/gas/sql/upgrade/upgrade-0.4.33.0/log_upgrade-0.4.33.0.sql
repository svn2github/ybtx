#种花
create table tbl_log_cultivate_flowers
	(
	 le_uId bigint unsigned not null,
	 le_uServerId int unsigned 			not null

	)engine = innodb;
	
#任务品拍卖
create table tbl_log_item_vendue
	(
	 le_uId bigint unsigned not null,
	 le_uServerId int unsigned 			not null

)engine = innodb;