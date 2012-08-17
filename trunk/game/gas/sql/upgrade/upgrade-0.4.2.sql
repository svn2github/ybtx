#玩家已经得到的成就
create table tbl_char_direct
(
	cs_uId										bigint unsigned not null,
	cd_sDirectName		        varchar(192) not	null,
	cd_uState									tinyint not null default 0,
	primary	key(cs_uId, cd_sDirectName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

#玩家的成就状态表
create table tbl_char_direct_state
(
	cs_uId		         				bigint unsigned not null,
	cds_sDirectName		        varchar(192) not	null,
	cds_uCount	 	 							tinyint unsigned not null,
	primary	key(cs_uId, cds_sDirectName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

#玩家的成就奖励表
create table tbl_char_direct_award
(
	cs_uId		         				bigint unsigned not null,
	cda_sAwardName		        varchar(192) not	null,
	cda_uState	 	 						tinyint unsigned not null,
	primary	key(cs_uId, cda_sAwardName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=Innodb;

alter table tbl_log_player_logout_position modify column lplp_sArgNameName varchar(128) not	null;


#帮会资源点临时信息表
create table tbl_tong_rob_resource_temp	 #tong resource info
(
	
	trrt_uId    	     bigint unsigned		not null,  			#tong id
	trrt_nCampId      bigint unsigned		not null,  			#tong camp type
	trrt_sObjName     varchar(96)    		not null,   #application place
	primary key(trrt_uId ,trrt_sObjName),
	foreign	key(trrt_uId) references tbl_tong(t_uId) on update cascade on	delete cascade
)engine=innodb;

#帮会资源点报名信息存储表
create table tbl_tong_rob_res_time	 #tong resource info
(
	
	trrti_uId    	     bigint unsigned		not null,  			#tong id
	trrti_dtSignTime        DateTime          not null,     #signup time
	trrti_uServerId    bigint unsigned		not null,  			#current server id
	primary key(trrti_uId ,trrti_dtSignTime),
	foreign	key(trrti_uId) references tbl_tong(t_uId) on update cascade on	delete cascade
)engine=innodb;



create table tbl_long_string_value1
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;


#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common1
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common1		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value2
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common2
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common2		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value3
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common3
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common3		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;

create table tbl_long_string_value4
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common4
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common4		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value5
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common5
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common5		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;

create table tbl_long_string_value6
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common6
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common6		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;

create table tbl_long_string_value7
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common7
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common7		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;

create table tbl_long_string_value8
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common8
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common8		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value9
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common9
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common9		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value10
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common10
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common10		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value11
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common11
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common11		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value12
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common12
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common12		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;

create table tbl_long_string_value13
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common13
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common13		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value14
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common14
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common14		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


create table tbl_long_string_value15
(
	ac_uId			bigint unsigned		not	null,
	lsv_sValue	varchar(256),
	primary key(ac_uId)
)engine=memory;

#参数表,   通过ac_acOwner的关联 可以组合成任意lua 的table 结构 
#  (基础类型目前 有 字符, 数字)
create table tbl_arg_common15
(
	ac_uId					bigint unsigned		not	null auto_increment,
	ac_sKey					varchar(32),
	ac_nKey					int,
	ac_sValue				varchar(32),
	ac_nValue				double,
	ac_acOwner			bigint unsigned,
	primary key(ac_uId)
)engine=memory;


#服务器消息表
create table tbl_msg_common15		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	mc_bIsReady				tinyint	unsigned not null,	#if message can be removed
	mc_dtAddTime 			timestamp not null default now(), #time that message be added
	primary key(mc_uId)	
)engine=innodb;


drop table tbl_long_string_value;
drop table tbl_arg_common;
drop table tbl_msg_common;
