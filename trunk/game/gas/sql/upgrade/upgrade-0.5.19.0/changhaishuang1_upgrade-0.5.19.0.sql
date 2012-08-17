
alter table tbl_char_onlinetime drop column co_uOnServerId;

#角色在线信息表
create table tbl_char_online	
(
	cs_uId			bigint	unsigned	not	null, 	#role identifier
	co_uOnServerId		tinyint	unsigned	not null,	#server that role is in
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

##########################################【下面的代码是创建ybtx_log】#####################################
drop database if exists ybtx_log;
create database ybtx_log;
use ybtx_log;

create table tbl_log_event
(
		le_uId				bigint unsigned			not	null auto_increment,
		le_dtDateTime		datetime				not null,

		primary	key(le_uId)
)engine=innodb;


create table tbl_serial_item
(
	lcs_uId	 bigint	unsigned	not	null, 
	si_sCode char(32) not null,
	
	primary key(lcs_uId)
)engine = innodb;


create table tbl_serial_money
(
	lcs_uId			bigint unsigned not null,
	sm_sCode char(32) not null,	#保持连续的
	
	primary key(lcs_uId)
)engine = innodb;


create table tbl_serial_depot_money
(
	lcs_uId			bigint unsigned not null,
	sdm_sCode char(32) not null,	#保持连续的
	
	primary key(lcs_uId)
)engine = innodb;


create table tbl_serial_item_del
(
	lcs_uId			bigint unsigned not null,
	sid_sCode char(32) not null,	#保持连续的
	
	primary key(lcs_uId)
)engine = innodb;


create table tbl_serial_experience
(
	lcs_uId			bigint unsigned not null,
	se_sCode char(32) not null,	#保持连续的
	
	primary key(lcs_uId)
)engine = innodb;


create table tbl_serial_soul
(
	lcs_uId			bigint unsigned not null,
	ss_sCode char(32) not null,	#保持连续的
	
	primary key(lcs_uId)
)engine = innodb;

create table tbl_serial_jifen
(
	lcs_uId			bigint unsigned not null,
	sj_sCode char(32) not null,	#保持连续的
	
	primary key(lcs_uId)
)engine = innodb;

create table tbl_log_channel_chat
(
 cs_uId	bigint unsigned	not	null,
 lcc_sMsg	text not null,
 lcc_dtChatTime datetime	not null,
 lcc_sChannelName varchar(96) not null,

 key(cs_uId),
 key(lcc_dtChatTime)
)engine = innodb;

insert into tbl_log_event select * from ybtx.tbl_log_event;
insert into tbl_serial_item select * from ybtx.tbl_serial_item;
insert into tbl_serial_money select * from ybtx.tbl_serial_money;
insert into tbl_serial_depot_money select * from ybtx.tbl_serial_depot_money;
insert into tbl_serial_item_del select * from ybtx.tbl_serial_item_del;
insert into tbl_serial_experience select * from ybtx.tbl_serial_experience;
insert into tbl_serial_soul select * from ybtx.tbl_serial_soul;
insert into tbl_serial_jifen select * from ybtx.tbl_serial_jifen;
