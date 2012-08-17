drop database if exists yb_cis;
create database if not exists yb_cis;

use yb_cis;
############################
#
# 账号信息服务器
#
############################

# 活动类型
create table tbl_activity_type
(
 at_ActiveID int unsigned not null,  # 活动ID
 at_AwardItem varchar(128) not null, # 奖励物品
 at_AwardType int unsigned not null, # 奖励类型
 at_MutexID int unsigned not null,   # 互斥ID
 at_StartTime datetime not null,     # 开始时间
 at_EndTime datetime not null,       # 结束时间

 primary key(at_ActiveID)
 )engine=innodb DEFAULT CHARSET=latin1;

# 用户账号信息
create table tbl_user_info
(
 ui_UserID int unsigned not null,    # 账号ID
 at_ActiveID int unsigned not null,  # 活动ID

 foreign key(at_ActiveID) references tbl_activity_type(at_ActiveID) on update cascade
 )engine=innodb DEFAULT CHARSET=latin1;

# 奖励领取历史
create table tbl_award_history
(
 ah_HistoryID bigint unsigned not null auto_increment, # 奖励领取历史ID
 ui_UserID int unsigned not null,                      # 账号ID
 ah_ServerID int unsigned not null,                    # 已经领取了奖品的服务器ID

 primary key(ah_HistoryID)
 )engine=innodb DEFAULT CHARSET=latin1;


create index idx_History on tbl_award_history(ui_UserID, ah_ServerID);



create table tbl_user_static
(
	us_uId					int	unsigned	not	null auto_increment,
	us_sName				varchar(96)		not	null,
	us_uVipType				tinyint unsigned	not	null default 0,	#vip标识;0-非vip,1-vip
	
	primary	key(us_uId),
	unique key(us_sName)
)engine=innodb;

create table tbl_char_static
(
	cs_uId					bigint unsigned		not	null auto_increment,
	us_uId					int	unsigned			not	null,
	cs_uSex					tinyint	unsigned	not	null,	#性别
	cs_uClass				tinyint	unsigned	not	null,	#职业
	cs_uCamp				tinyint	unsigned	not	null,			#阵营
	cs_dtCreateDate 		datetime not null					, #角色创建时间
	cs_uServerId			int unsigned not null,				  #所在网关id
	cs_uState				tinyint unsigned not null default 1,    	#角色状态
	
	primary	key(cs_uId),
	foreign	key(us_uId)	references tbl_user_static(us_uId) on	update cascade
)engine=innodb;


create table tbl_char
(
	cs_uId					bigint	unsigned	not	null,
	c_sName					char(60) not null,
	c_sNameFormer			char(80) not null,
	
	primary	key(cs_uId),
	unique key(c_sName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


