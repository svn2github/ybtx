###G:/ybtxver/online_stable/code/game/gas/sql/upgrade/log_upgrade/changhaishuang/1_changhaishuang_log.sql###
create table tbl_log_join_activity
(
 le_uId	 bigint unsigned 	not null,
 lcs_uId bigint unsigned 	not null,
 lts_uId int unsigned	not null,
 
 key(le_uId),
 key(lcs_uId),
 key(lts_uId)
)engine=innodb;

insert into `tbl_log_normal_delete` values('tbl_log_join_activity');


