
################################################################
#
#		数据中心部分的表格
#		
#		注意：游戏逻辑代码只能添加或者删除数据中心部
#					分的表格数据，但绝不应该读取这些数据的内容
#
################################################################


################################################################
#		事件记录
################################################################

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

create table tbl_item_id
(
	ii_uId	bigint unsigned	not	null auto_increment,

	primary	key(ii_uId)
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

