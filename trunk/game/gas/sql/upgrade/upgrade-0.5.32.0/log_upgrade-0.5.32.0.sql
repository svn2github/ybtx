#记录角色经验、魂系数变化log
create table tbl_log_char_exp_soul_modulus
(
	lcs_uId	bigint unsigned	not	null,
	lcesm_uModulusType tinyint unsigned not null,#系数类型 1-魂系数 2-经验系数
	lcesm_uModulusValue float unsigned not null,#系数值
	lcesm_dtChangeTime datetime not null,#系数值
	
	key(lcs_uId)
)engine=innodb;

 alter table tbl_log_tong_building_event add column ltbe_uBuildingId bigint unsigned		not null;
 alter table tbl_log_tong_building_event add column ltbe_uAddParameter float unsigned		not null;
 
 
 #佣兵小队抢夺资源点报名
create table tbl_log_tong_resource_sig_up
(
	le_uId	bigint unsigned		not	null,
	lts_uId int unsigned	not null, #佣兵小队Id
	ltrsu_uExecutorId bigint unsigned		not null, #执行者
	ltrsu_uExploit bigint not null,
	ltrsu_sObjName varchar(96) not null,
	
	key(le_uId),
	key(lts_uId),
	key(ltrsu_uExecutorId)
 )engine=innodb;	
