##############################
###           禁言         ###
##############################
create table tbl_forbid_talking (		#roles of forbid talking by channel id
	cs_uID 		bigint unsigned		not null,	#role identifier
	ft_uChannelId	tinyint unsigned	not null,	#channel that forbidded
	primary key 	(`cs_uID`, `ft_uChannelId`),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = InnoDb;

create table tbl_forbid_login(			#roles that forbid login
	cs_uID 		bigint unsigned		not null,	#role identifier
	primary key 	(`cs_uID`),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = InnoDb;

create table tbl_freeze_account(		#accounts that were freezed 
	username 	varchar(96)	not null,		#account name
	us_uGameID	int	unsigned not null,
	unique key(username,us_uGameID)
)engine = InnoDb;

create table tbl_gm_validate(			#online gm names
	username 	varchar(96)	not null,	#gm account name
	us_uGameID	int	unsigned not null,
	gv_uLevel tinyint not null,			#gm level
	unique key(username,us_uGameID)
)engine = InnoDb;

create table tbl_money_corrency_limit(		#function that limited of money corrency
	mcl_sFunName 	varchar(96)	not null,		# function name
	mcl_sModule		varchar(96)	not null,	# module that belongs to
	primary key 	(`mcl_sFunName`,`mcl_sModule`)
)engine = InnoDb;

create table tbl_jifen_use_limit(		#limited integral type
	jul_uType 	tinyint 	not null,		# integral type
	primary key 	(`jul_uType`)
)engine = InnoDb;

create table tbl_item_use_limit(		#limited items
	iul_sItemName 	varchar(96)	not null,		#item name
	primary key 	(`iul_sItemName`)
)engine = InnoDb;

create table tbl_action_closed(			#limited action
	ac_uID             bigint unsigned   not null   auto_increment,		#sequence id
	ac_sActionName     varchar(96)       not null   default "",		#action name
	primary key 	(`ac_uID`)
)engine = InnoDb;

create table tbl_quest_finish_limit(
	qfl_sQuestName 	varchar(96)	not null,
	primary key 	(`qfl_sQuestName`)
)engine = InnoDb;

create table tbl_obj_drop_limit(
	odl_sObjName 	varchar(96)	not null,
	primary key 	(`odl_sObjName`)
)engine = InnoDb;
##############################
###          热备份        ###
##############################
create table tbl_hot_update_result(
	hu_uId		bigint unsigned		not null,
	hu_serIid	bigint unsigned		not null,
	hu_state	tinyint unsigned	not null,
	hu_sMsg		text
)engine = InnoDb;
 
###热更新的lua代码记录
#gas
create table tbl_hot_update_gas(
	hug_id		bigint 	unsigned	not null	auto_increment,
	hug_revision	varchar(32)	not null,
	hug_filename	varchar(96)	not null,
	hug_code	varchar(60000) not null,
	primary key(hug_id)
)engine = InnoDb;

#gac
create table tbl_hot_update_gac(
	hug_id		bigint 	unsigned	not null	auto_increment,
	hug_revision	varchar(32)	not null,
	hug_filename	varchar(96)	not null,
	hug_code	varchar(60000) not null,
	primary key(hug_id)
)engine = InnoDb;

#db
create table tbl_hot_update_db(
	hud_id		bigint 	unsigned	not null	auto_increment,
	hud_revision	varchar(32)	not null,
	hud_filename	varchar(96)	not null,
	hud_code	varchar(60000) not null,
	primary key(hud_id)
)engine = InnoDb;

#阵营状态
create table tbl_recruit_camp
(
	rc_uCamp		tinyint unsigned	not null,   
	rc_uState	tinyint unsigned	not null,   #状态(0,1,2)  --0关闭,1普通,2重金
	primary	key(rc_uCamp)
)engine = innodb;

#重金招募信息
create table tbl_recruit_info
(	
	ri_uMinNum	smallint 	unsigned	not null, 
	ri_uMaxNum	smallint 	unsigned	not null,
	ri_uTime	smallint 	unsigned	not null 
)engine = innodb;

#tbl_web_server_conf
CREATE TABLE `tbl_web_server_conf` (
  `wsc_sName` varchar(32) NOT NULL,
  `wsc_sValue` varchar(128) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1
