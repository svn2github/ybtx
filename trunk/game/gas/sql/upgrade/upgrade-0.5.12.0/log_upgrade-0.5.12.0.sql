alter table tbl_database_upgrade_record add dur_dtTime datetime not null after dur_sRecord;
alter table tbl_log_char_static drop column lcs_uInherence;

alter table tbl_log_tong_static add column lts_dtCreateTime  datetime  not null after lts_sName;
alter table tbl_log_tong_static add column lts_uCamp  tinyint  not null after lts_dtCreateTime;
alter table tbl_log_tong_static add column lts_uInitiatorId bigint unsigned	not null after lts_uCamp;
alter table tbl_log_tong_static add key(lts_uCamp);

create table tbl_log_tong_break
(
	le_uId			bigint unsigned		not	null,
	lts_uId  int unsigned	not null, #帮会Id
	ltb_uBreakCharId bigint unsigned		not null, #tong Initiator
	
	key(le_uId)
 )engine=innodb;	
 
 
#任务品拍卖结果
create table tbl_log_item_vendue_end
(
	le_uId bigint unsigned not null,
	
	key(le_uId)
)engine = innodb;

alter table tbl_log_user_static drop column lus_sPassword;
alter table tbl_log_user_static add column lus_dtCreateTime	datetime not null;

drop table tbl_log_player_post;