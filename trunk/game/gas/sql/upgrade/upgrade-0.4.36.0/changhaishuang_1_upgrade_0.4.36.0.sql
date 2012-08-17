
drop table tbl_sort_jifensai_all_wintimes;
drop table tbl_sort_jifensai_all_wintimes_by_camp;
#联赛的总参赛次数排行榜
create table tbl_sort_jifensai_join_times
(
	sjjt_uOrder smallint unsigned not null,												#名次
	sjjt_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	sjjt_uJoinTimes	bigint	unsigned		not null default 0,#联赛总参赛次数
	
	key(sjjt_uOrder),
	key(sjjt_uJoinTimes),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对联赛的总参赛次数排行
create table tbl_sort_jifensai_join_times_by_camp
(
	sjjtbc_uOrder smallint unsigned not null,												#名次
	sjjtbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	key(sjjtbc_uOrder),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#佣兵小队等级相关排行
create table tbl_sort_tong_level
(
	stl_uOrder smallint unsigned not null,								
	stl_uUpdown tinyint not null,							
	t_uId				bigint unsigned		not null,	
	stl_uLevel tinyint	not null,  
	stl_uHonor bigint unsigned		not null,    
	
	primary key(t_uId),
	key(stl_uOrder),
	key(stl_uUpdown),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#按照阵营对佣兵小队等级排行
create table tbl_sort_tong_level_by_camp 
(
	stlbc_uOrder smallint unsigned not null,		
	stlbc_uUpdown tinyint not null ,					
	t_uId				bigint unsigned		not null,	
	
	primary key(t_uId),
	key(stlbc_uOrder),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#佣兵小队声望相关排行
create table tbl_sort_tong_exploit
(
	ste_uOrder smallint unsigned not null,								
	ste_uUpdown tinyint not null,							
	t_uId				bigint unsigned		not null,	
	ste_uExploit bigint unsigned	not null,	
	
	primary key(t_uId),
	key(ste_uOrder),
	key(ste_uUpdown),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#按照阵营对佣兵小队声望排行
create table tbl_sort_tong_exploit_by_camp 
(
	stebc_uOrder smallint unsigned not null,		
	stebc_uUpdown tinyint not null ,					
	t_uId				bigint unsigned		not null,	
	
	primary key(t_uId),
	key(stebc_uOrder),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#佣兵小队军资相关排行
create table tbl_sort_tong_resource
(
	str_uOrder smallint unsigned not null,								
	str_uUpdown tinyint not null,							
	t_uId				bigint unsigned		not null,	
	str_uResource bigint unsigned	not null,	
	
	primary key(t_uId),
	key(str_uOrder),
	key(str_uUpdown),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#按照阵营对佣兵小队军资排行
create table tbl_sort_tong_resource_by_camp 
(
	strbc_uOrder smallint unsigned not null,		
	strbc_uUpdown tinyint not null ,					
	t_uId				bigint unsigned		not null,	
	
	primary key(t_uId),
	key(strbc_uOrder),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#佣兵小队资金相关排行
create table tbl_sort_tong_money
(
	stm_uOrder smallint unsigned not null,								
	stm_uUpdown tinyint not null,							
	t_uId				bigint unsigned		not null,	
	stm_uMoney bigint unsigned	not null,	
	
	primary key(t_uId),
	key(stm_uOrder),
	key(stm_uUpdown),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#按照阵营对佣兵小队资金排行
create table tbl_sort_tong_money_by_camp 
(
	stmbc_uOrder smallint unsigned not null,		
	stmbc_uUpdown tinyint not null ,					
	t_uId	 bigint unsigned		not null,	
	
	primary key(t_uId),
	key(stmbc_uOrder),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#玩家军资运输排行
create table tbl_sort_char_resource					
(
	scr_uOrder smallint unsigned not null,									#sequence id
	scr_uUpdown tinyint not null,								#up or down
	cs_uId bigint unsigned not null,									#role ID
	scr_uResource float unsigned not null,			#total money
	
	primary key(cs_uId),
	key(scr_uOrder),
	key(scr_uUpdown),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对战斗力评分排行榜
create table tbl_sort_char_resource_by_camp 
(
	scrbc_uOrder smallint unsigned not null,										#sequence id
	scrbc_uUpdown tinyint not null ,								#up or down
	cs_uId bigint unsigned not null,										#role ID
	
	primary key(cs_uId),
	key(scrbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#联赛的胜利场次排行榜
create table tbl_sort_jifensai_wintimes
(
	sjw_uOrder smallint unsigned not null,												#名次
	sjw_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	sjw_uWinTimes	bigint	unsigned		not null default 0,
	
	key(sjw_uOrder),
	key(sjw_uWinTimes),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对联赛的胜利场次排行
create table tbl_sort_jifensai_wintimes_by_camp
(
	sjwbc_uOrder smallint unsigned not null,												#名次
	sjwbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	key(sjwbc_uOrder),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#联赛的杀人数量排行榜
create table tbl_sort_tong_kill_num
(
	stkn_uOrder smallint unsigned not null,												#名次
	stkn_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	stkn_uKillNum	bigint	unsigned		not null default 0,
	
	key(stkn_uOrder),
	key(stkn_uKillNum),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对联赛杀人数量排行
create table tbl_sort_tong_kill_num_by_camp
(
	stknbc_uOrder smallint unsigned not null,												#名次
	stknbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	key(stknbc_uOrder),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#资源点争夺杀人数量排行榜
create table tbl_sort_resource_kill_num
(
	srkn_uOrder smallint unsigned not null,												#名次
	srkn_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	srkn_uKillNum	bigint	unsigned		not null default 0,
	
	key(srkn_uOrder),
	key(srkn_uKillNum),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对资源点争夺杀人数量排行
create table tbl_sort_resource_kill_num_by_camp
(
	srknbc_uOrder smallint unsigned not null,												#名次
	srknbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	key(srknbc_uOrder),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#佣兵小队资源点占领次数相关排行
create table tbl_sort_tong_resource_occupy_times
(
	strot_uOrder smallint unsigned not null,								
	strot_uUpdown tinyint not null,							
	t_uId				bigint unsigned		not null,	
	strot_uOccupyTimes bigint unsigned	not null,	
	
	primary key(t_uId),
	key(strot_uOrder),
	key(strot_uUpdown),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#按照阵营对佣兵小队资源点占领次数排行
create table tbl_sort_tong_resource_occupy_times_by_camp 
(
	strotbc_uOrder smallint unsigned not null,		
	strotbc_uUpdown tinyint not null ,					
	t_uId				bigint unsigned		not null,	
	
	key(strotbc_uOrder),
	primary key(t_uId),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#佣兵小队击杀领主次数排行
create table tbl_sort_tong_kill_boss_num
(
	stkbn_uOrder smallint unsigned not null,								
	stkbn_uUpdown tinyint not null,							
	t_uId				bigint unsigned		not null,	
	stkbn_uKillNum bigint unsigned	not null,	
	
	primary key(t_uId),
	key(stkbn_uOrder),
	key(stkbn_uUpdown),
	foreign	key(t_uId) references tbl_tong(t_uId) on	update cascade on	delete cascade
)engine=innodb;

#按照阵营对佣兵小队击杀领主次数排行
create table tbl_sort_tong_kill_boss_num_by_camp 
(
	stkbnbc_uOrder smallint unsigned not null,		
	stkbnbc_uUpdown tinyint not null ,					
	t_uId				bigint unsigned		not null,	
	
	key(stkbnbc_uOrder),
	primary key(t_uId),
	foreign	key(t_uId) references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;


#玩家击杀领主次数排行
create table tbl_sort_char_kill_boss_num
(
	sckbn_uOrder smallint unsigned not null,								
	sckbn_uUpdown tinyint not null,							
	cs_uId				bigint unsigned		not null,	
	sckbn_uKillNum bigint unsigned	not null,	
	
	primary key(cs_uId),
	key(sckbn_uOrder),
	key(sckbn_uUpdown),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对玩家击杀领主次数排行
create table tbl_sort_char_kill_boss_num_by_camp 
(
	sckbnbc_uOrder smallint unsigned not null,		
	sckbnbc_uUpdown tinyint not null ,					
	cs_uId				bigint unsigned		not null,	
	
	key(sckbnbc_uOrder),
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

insert into tbl_database_upgrade_record values("changhaishuang_1_upgrade_0.4.36.0.sql");
