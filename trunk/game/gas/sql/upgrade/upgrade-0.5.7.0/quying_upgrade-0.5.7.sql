create table tbl_tong_rob_res_log
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	trrl_sObjName	varchar(96)         	not null,			#role
	primary	key(t_uId, trrl_sObjName)
)engine=innodb;

insert into tbl_database_upgrade_record values("quying_upgrade-0.5.7.sql");