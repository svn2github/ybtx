################################################################
#玩家提交log表
################################################################
#user_log

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

