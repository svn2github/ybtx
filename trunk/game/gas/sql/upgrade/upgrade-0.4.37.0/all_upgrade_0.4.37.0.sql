#--------------------------更新sql代码分割线---------------------------------------

##经验瓶、魂瓶
create table tbl_item_exp_soul_bottle
(
	cs_uId 	bigint unsigned 	not null,		
	is_uId			bigint unsigned 	not null,	
	iesb_uStorageNum int unsigned not null, 
	iesb_uState tinyint unsigned not null,  #1-激活 ; 2-关闭
	iesb_uType tinyint unsigned not null,  #1-魂瓶 ; 2-经验瓶
	
	primary key (cs_uId,iesb_uState,iesb_uType),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

alter table tbl_tong_proffer change column tp_uProffer tp_uProffer	double(20,2) unsigned not null;

alter table tbl_activity_count add index (cs_uId);
alter table tbl_activity_count add index (ac_sActivityName);
alter table tbl_extra_reward_count add index (cs_uId);
alter table tbl_extra_reward_count add index (erc_sActivityName);

#--------------------------更新sql代码分割线---------------------------------------
drop table if exists tbl_hot_update_code;

###热更新的lua代码记录
#gas
create table tbl_hot_update_gas(
	hug_id		bigint 	unsigned	not null	auto_increment,
	hug_revision	varchar(32)	not null,
	hug_filename	varchar(96)	not null,
	hug_code	text,
	primary key(hug_id)
)engine = InnoDb;

#gac
create table tbl_hot_update_gac(
	hug_id		bigint 	unsigned	not null	auto_increment,
	hug_revision	varchar(32)	not null,
	hug_filename	varchar(96)	not null,
	hug_code	text,
	primary key(hug_id)
)engine = InnoDb;

#db
create table tbl_hot_update_db(
	hud_id		bigint 	unsigned	not null	auto_increment,
	hud_revision	varchar(32)	not null,
	hud_filename	varchar(96)	not null,
	hud_code	text,
	primary key(hud_id)
)engine = InnoDb;

drop table if exists tbl_user_log_code;
drop table if exists tbl_user_advice;
create table tbl_user_advice
(
	ua_uErrorCode			bigint unsigned not null 	auto_increment,	#advice code
	cs_uId						bigint unsigned 	not null,			
	ua_sIp						varchar(32) not null,
	ua_sErrorMsg			varchar(1024) not null,
	ua_nMsgType				tinyint unsigned not null,
	ua_dtTime					datetime not null,
	primary key(ua_uErrorCode),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_ui_setting
(
	us_uId							int		unsigned	not null,
	
	uis_uPlayerNameShow				tinyint	unsigned	not null,
	uis_uPlayerHonourShow			tinyint	unsigned	not null,
	uis_uPlayerTongNameShow			tinyint	unsigned	not null,
	uis_uPlayerArmyCorpsNameShow	tinyint	unsigned	not null,
	
	uis_uOtherNameShow				tinyint unsigned	not null,
	uis_uOtherHonourShow			tinyint unsigned	not null,
	uis_uOtherTongNameShow			tinyint unsigned	not null,
	uis_uOtherArmyCorpsNameShow		tinyint	unsigned	not null,
	
	uis_uNpcNameShow				tinyint unsigned	not null,
	uis_uMonsterNameShow			tinyint unsigned	not null,
	uis_uHeadPieceShow				tinyint unsigned	not null,
	uis_uLockShortcut				tinyint unsigned	not null,
	uis_uMonsterNameShowMO			tinyint unsigned	not null, #仅在鼠标悬停时显示怪物名称
	uis_uCanNotSelectFriend			tinyint	unsigned	not null,
	
	primary key(us_uId),
	foreign	key(us_uId)	references tbl_char_static(us_uId) on update cascade on delete cascade
	
)engine=innodb;

#--------------------------更新sql代码分割线---------------------------------------
insert into tbl_database_upgrade_record values ("all_upgrade_0.4.37.0.sql");
