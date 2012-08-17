#登录排队人数
create table tbl_user_wait_queue_count	
(	
	uwqc_uCount	int	unsigned	not	null 

)engine = memory;


#玩家VIP特效
create table tbl_char_vip_effect
(
	cs_uId			bigint unsigned		not	null,
	cve_dtUseTime  datetime not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

drop table tbl_tong_needfire;

create table tbl_tong_needfire
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tn_uServerId tinyint unsigned	not null,
	
	primary	key(t_uId,tn_uServerId),
	foreign	key(t_uId)	 references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

alter table tbl_char_vip_effect add column cve_sItemName varchar(96) not null after cve_dtUseTime;

