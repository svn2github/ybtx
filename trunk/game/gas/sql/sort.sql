
##################################【玩家相关排行榜】######################################

#等级.经验排行榜
create table tbl_sort_level				#ranking board of roles' level
(
	sl_uOrder smallint unsigned not null,									#sequence id
	sl_uUpdown tinyint not null,								#up or down
	cs_uId bigint unsigned not null,									#role ID
	sl_uLevel tinyint unsigned not null default 1,		#level
	sl_uLevelExp bigint unsigned not null default 0,			#experience
	
	primary key(cs_uId),
	key(sl_uOrder),
	key(sl_uLevel),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#按照阵营对等级.经验值进行排序
create table tbl_sort_level_by_camp		#ranking board of roles' level group by camp
(
	slbc_uOrder smallint unsigned not null,									#sequence id
	slbc_uUpdown tinyint not null,								#up or down
	cs_uId bigint unsigned not null,									#role id
	
	primary key(cs_uId),
	key(slbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#不绑定金钱排行榜
create table tbl_sort_money					#role sorted by money
(
	sm_uOrder smallint unsigned not null,									#sequence id
	sm_uUpdown tinyint not null,								#up or down
	cs_uId bigint unsigned not null,									#role ID
	sm_uMoney bigint unsigned not null default 0,			#total money
	
	primary key(cs_uId),
	key(sm_uOrder),
	key(sm_uMoney),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对不绑定金钱排行榜
create table tbl_sort_money_by_camp #role sorted by money group by camp
(
	smbc_uOrder smallint unsigned not null,										#sequence id
	smbc_uUpdown tinyint not null ,								#up or down
	cs_uId bigint unsigned not null,										#role ID
	
	primary key(cs_uId),
	key(smbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#角色死亡次数排行榜
create table tbl_sort_deadtimes
(
	sd_uOrder smallint unsigned not null,										#名次
	sd_uUpdown tinyint not null,									#升降
	cs_uId bigint unsigned not null,										#玩家ID
	sd_uDeadTimes bigint unsigned	not null default 0,		#角色死亡的次数
	
	primary key(cs_uId),
	key(sd_uOrder),
	key(sd_uDeadTimes),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对角色死亡次数进行排序
create table tbl_sort_deadtimes_by_camp
(
	sdbc_uOrder smallint unsigned not null,									#名次
	sdbc_uUpdown tinyint not null,								#升降
	cs_uId bigint unsigned not null,									#玩家ID
	
	primary key(cs_uId),
	key(sdbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#角色杀怪兽排行榜
create table tbl_sort_kill_npc_count
(
	sknc_uOrder smallint unsigned not null,											#名次
	sknc_uUpdown tinyint not null,										#升降
	cs_uId bigint unsigned not null,											#玩家ID
	sknc_uKillNpcCount bigint unsigned not null default 0,	#角色杀怪兽个数
	
	primary key(cs_uId),
	key(sknc_uOrder),
	key(sknc_uKillNpcCount),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对角色杀怪兽个数进行排序
create table tbl_sort_kill_npc_count_by_camp
(
	skncbc_uOrder smallint unsigned not null,												#名次
	skncbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	primary key(cs_uId),
	key(skncbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#角色杀人排行榜
create table tbl_sort_kill_player_count
(
	skpc_uOrder smallint unsigned not null,												#名次
	skpc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	skpc_uKillPlayerCount bigint unsigned not null default 0,	#角色杀人个数
	
	primary key(cs_uId),
	key(skpc_uOrder),
	key(skpc_uKillPlayerCount),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对角色杀人个数进行排序
create table tbl_sort_kill_player_count_by_camp
(
	skpcbc_uOrder smallint unsigned not null,												#名次
	skpcbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	primary key(cs_uId),
	key(skpcbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#角色在线时间累积排行榜
create table tbl_sort_player_onlinetime
(
	spo_uOrder smallint unsigned not null,												#名次
	spo_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	spo_uPlayerOnTimeTotal bigint unsigned not null default 0,	#角色在线时间榜
	
	primary key(cs_uId),
	key(spo_uOrder),
	key(spo_uPlayerOnTimeTotal),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对角色在线时间累积进行排序
create table tbl_sort_player_onlinetime_by_camp
(
	spobc_uOrder smallint unsigned not null,												#名次
	spobc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	primary key(cs_uId),
	key(spobc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#大逃杀总参赛次数排行
create table tbl_sort_dataosha_all_jointimes
(
	sdaj_uOrder smallint unsigned not null,												#名次
	sdaj_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	sdaj_uJoinTimes	bigint	unsigned		not null default 0,#大逃杀总参赛次数
	
	primary key(cs_uId),
	key(sdaj_uOrder),
	key(sdaj_uJoinTimes),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对大逃杀总参赛次数排行
create table tbl_sort_dataosha_all_jointimes_by_camp
(
	sdajbc_uOrder smallint unsigned not null,												#名次
	sdajbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	primary key(cs_uId),
	key(sdajbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#大逃杀总胜利次数排行
create table tbl_sort_dataosha_all_wintimes
(
	sdaw_uOrder smallint unsigned not null,												#名次
	sdaw_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	sdaw_uWinTimes	bigint	unsigned		not null default 0,#大逃杀总参赛次数
	
	primary key(cs_uId),
	key(sdaw_uOrder),
	key(sdaw_uUpdown),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对大逃杀总胜利次数排行
create table tbl_sort_dataosha_all_wintimes_by_camp
(
	sdawbc_uOrder smallint unsigned not null,												#名次
	sdawbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	primary key(cs_uId),
	key(sdawbc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

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


#角斗场积分排行榜
create table tbl_sort_dataosha_point
(
	sdp_uOrder smallint unsigned not null,												#名次
	sdp_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	sdp_uPoint bigint	unsigned		not null default 0,#大逃杀积分
	
	key(sdp_uOrder),
	key(sdp_uPoint),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对角斗场的积分排行
create table tbl_sort_dataosha_point_by_camp
(
	sdpbc_uOrder smallint unsigned not null,												#名次
	sdpbc_uUpdown tinyint not null,											#升降
	cs_uId bigint unsigned not null,												#玩家ID
	
	key(sdpbc_uOrder),
	primary key (cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#战斗力评分排行
create table tbl_sort_fight_evaluation					#role sorted by fight evaluation
(
	sfe_uOrder smallint unsigned not null,									#sequence id
	sfe_uUpdown tinyint not null,								#up or down
	cs_uId bigint unsigned not null,									#role ID
	sfe_uPoint float unsigned not null,			#total money
	
	primary key(cs_uId),
	key(sfe_uOrder),
	key(sfe_uUpdown),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对战斗力评分排行榜
create table tbl_sort_fight_evaluation_by_camp #role sorted by fight evaluation group by camp
(
	sfebc_uOrder smallint unsigned not null,										#sequence id
	sfebc_uUpdown tinyint not null ,								#up or down
	cs_uId bigint unsigned not null,										#role ID
	
	primary key(cs_uId),
	key(sfebc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

####################################【以下是佣兵小队相关排行表】#######################################
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















