###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/baofuzhen/1_baofuzhen_upgrade.sql###
alter table tbl_char_servant add column cso_uServantID tinyint unsigned	not null default 0 after cso_uLeftTime;
alter table tbl_char_magicstate add column cso_uServantID tinyint unsigned	not null default 0 after cms_uServantName;
alter table tbl_char_magic_obj add column cso_uServantID tinyint unsigned	not null default 0 after cmo_uServantName;


###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/changhaishuang/3_changhaishuang_upgrade.sql###
create table tbl_char_welfare
(
	cs_uId		bigint unsigned		not	null,
	cw_sBuffName varchar(96) not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/dingdan/1_dingdan_data_upgrade.sql###
update tbl_contract_manufacture_order set cmo_tEndTime =now();


###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/hujiawen/3_hujiawen_upgrade.sql###
drop table if exists tbl_tong_battle_exploit;
drop table if exists tbl_battle_camp_exploit;
##国战声望获得，国战结束即清理一次
create table tbl_tong_battle_exploit #tong battle war exploit
(
	cs_uId							bigint unsigned		not null,								#role identifier
	be_uGetExploit			bigint unsigned		not null default 0,     #get exploit
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

##国战阵营声望获得，国战结束即清理一次
create table tbl_battle_camp_exploit #tong camp war exploit
(
	ce_uAmCampExploit			bigint unsigned		not null default 0,     #camp exploit
	ce_uSsCampExploit			bigint unsigned		not null default 0,     #camp exploit
	ce_uXsCampExploit			bigint unsigned		not null default 0,     #camp exploit
	primary	key(ce_uAmCampExploit)
)engine=innodb;


###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/niuxiaoguang/1_niuxiaoguang_upgrade.sql###
drop table tbl_gm_command;
drop table tbl_gm_command_log;


###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/quying/2_quying_upgrade.sql###
create table tbl_tong_get_proffer_time_temp
(
	cs_uId			bigint unsigned 	not null,			#role
	tgptt_sTime  Datetime  not null,                 
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

alter table tbl_tong_header_award_time add foreign key(t_uId)	references tbl_tong(t_uId) on	update cascade on delete cascade;
alter table tbl_tong_header_award_time add foreign key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade;



###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/quying/3_quying_data_upgrade.sql###
insert into tbl_tong_get_proffer_time_temp select * from tbl_tong_get_proffer_time;





###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/quying/4_quying_upgrade.sql###
drop table tbl_tong_get_proffer_time;

create table tbl_tong_get_proffer_time
(
	cs_uId			bigint unsigned 	not null,			#role
	tgpt_sTime  Datetime  not null,                 
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#记录资源点奖励数据清理时间
create table tbl_tong_res_award_time(
		rat_dtTime		DateTime        not null       	#clear time
)engine = innodb;



###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/quying/5_quying_data_upgrade.sql###
insert into tbl_tong_get_proffer_time
(select * from tbl_tong_get_proffer_time_temp where tgptt_sTime > date(now()));




###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/quying/6_quying_upgrade.sql###
drop table tbl_tong_get_proffer_time_temp;


###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/zhangzhengtao/1_zhangzhengtao_upgrade.sql###
drop table if exists tbl_hot_update_result;

create table tbl_hot_update_result(
	hur_id int unsigned not null auto_increment,
	hur_type tinyint unsigned not null,
	hur_filename varchar(96) not null,
	hur_code varchar(36) not null,
	hur_result varchar(32) not null,
	primary key(hur_id),
	unique key(hur_type, hur_filename, hur_code)
)engine = InnoDb;




###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/zhixu/1_zhixu_data_upgrade.sql###
delete from tbl_char_jifensaipoint where cs_uType <> 2;



###G:/ybtxver/online_test/code/game/gas/sql/upgrade/game_upgrade/zhixu/2_zhixu_upgrade.sql###
alter table tbl_char_jifensaipoint modify cs_uWinNum int unsigned not null default 0,
 modify cs_uLoseNum int unsigned not null default 0,
 modify cs_uRunNum int unsigned not null default 0,
 modify cs_uPoint int unsigned not null default 20000,
 modify cj_uInFbTimes int unsigned not null default 0,
 modify cj_uKillNum int unsigned not null default 0,
 modify cj_uDeadNum int unsigned not null default 0;


alter table tbl_char_jifensaiaccum modify cs_uUpWeekWinNum int unsigned not null default 0,
 modify cs_uUpWeekLoseNum int unsigned not null default 0,
 modify cs_uUpWeekRunNum int unsigned not null default 0,
 modify cs_uUpWeekPoint int unsigned not null default 0,
 modify cs_uCurrWeekWinNum int unsigned not null default 0,
 modify cs_uCurrWeekLoseNum int unsigned not null default 0,
 modify cs_uCurrWeekRunNum int unsigned not null default 0,
 modify cs_uCurrWeekPoint int unsigned not null default 0;



