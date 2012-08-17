
#记录数据库的自增ID需要的偏移量


create table tbl_id_add_count
(
	ian_sBaseName					char(60) not null, #当前数据库
	ian_uId						tinyint	unsigned not null default 1,
	primary key(ian_sBaseName),
	unique key(ian_uId)
)engine=InnoDB;

create table tbl_army_info
(
	ian_uId						tinyint	unsigned	default 1,
	ac_uId						bigint	unsigned	default 0,
	ac_sName					varchar(100) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
	ac_uLevel					tinyint	unsigned	default 1,
	primary key(ac_uId)
)engine=InnoDB;

#存放需要重命名的帮会信息
create table tbl_tong_info
(
	ian_uId						tinyint	unsigned	default 1,
	t_uId						bigint	unsigned	default 0,
	t_sName						varchar(100) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
	t_uLevel					tinyint	unsigned	default 1,
	primary key(t_uId)
)engine=InnoDB;

create table tbl_char_info
(
	ian_uId						tinyint	unsigned	default 1,
	cs_uId						bigint	unsigned	default 0,
	c_sName						char(32) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
	cb_uLevel					tinyint	unsigned	default 1,
	primary key(cs_uId)
)engine=InnoDB;


create table tbl_user_info
(
	us_sName					varchar(100) not	null,
	us_uId						bigint unsigned not null,
	ub_uOnlineTime					bigint unsigned  not null	default	0,	 #用户在某个服务器的累计在线时间
	ub_uYuanBao					bigint	unsigned	not null  default 0, #这个账号在某个服务器上的元宝
	ub_uAgreedVer					smallint unsigned not null default 0,  #在某个服务器上用户上次同意用户协议的版本号
	
	primary	key(us_sName)
)engine=InnoDB;

