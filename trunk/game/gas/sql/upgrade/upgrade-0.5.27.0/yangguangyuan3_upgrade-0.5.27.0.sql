use ybtx_log

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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common1		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common2		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common3		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common4		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common5		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common6		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common7		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common8		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common9		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common10		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common11		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common12		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common13		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common14		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
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
	primary key(ac_uId),
	index using btree(ac_acOwner)
)engine=memory;


#服务器消息表
create table tbl_msg_common15		#table of common messages
(
	mc_uId						bigint unsigned		not	null auto_increment, #sequence id
	mc_sFunName				varchar(32)				not null,	#function name to be executed
	ac_uId						bigint unsigned,						#args
	primary key(mc_uId)	
)engine=innodb;


use ybtx
