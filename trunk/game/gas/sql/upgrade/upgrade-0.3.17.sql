#积分赛玩家成绩记录增加杀人次数和死亡次数，用于信息面板查看
alter table tbl_char_jifensaipoint add cj_uKillNum		bigint	unsigned	not null default 0 after cj_dDate;
alter table tbl_char_jifensaipoint add cj_uDeadNum		bigint	unsigned	not null default 0 after cj_uKillNum;


alter table tbl_user_wait_queue add uwq_uContextId bigint	unsigned	not	null after uwq_uOnServerId;

#增加召唤兽生存时间列
alter table tbl_char_servant_outline add cso_uLeftTime bigint unsigned not null after cso_uCurMP;

#队伍、帮会等回复消息验证表
#用于接收别人请求消息的验证
create table tbl_revert_validate
(
	cs_uInviter		bigint unsigned	not	null,	#邀请者
	cs_uInvitee		bigint unsigned not null, #被邀请者
	rv_uFun				tinyint unsigned not null, #所属功能
	unique key(cs_uInviter,cs_uInvitee,rv_uFun),
	foreign	key(cs_uInviter)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(cs_uinvitee)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;




#修改玩家好友表结构
alter table tbl_player_friends drop foreign key tbl_player_friends_ibfk_3;
alter table tbl_player_friends add foreign key (fc_uId,cs_uId) references tbl_friends_class(fc_uId,cs_uId) on update cascade on delete cascade;

#增加记录帐号创建时间的字段
alter table tbl_user_static add us_dtCreateTime	datetime not null;

#记录帐号最后一次登录时间和最后一次退出时间
create table tbl_user_last_login_logout_time
(
	us_uId int unsigned not null,
	ulllt_dtLoginTime datetime not null, #帐号最后一次登录时间
	ulllt_dtLogoutTime datetime not null,#帐号最后一次登出时间
	
	primary key(us_uId),
	foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade
)engine=innodb;

#服务器在线人数情况记录
create table tbl_log_service_online_num
(
		le_uId					bigint unsigned			not null,
		lson_uServerId tinyint	unsigned	not	null, #服务器id
		lson_uOnlineUserNum bigint unsigned			not null #服务器在线人数
		
)engine=innodb;

#服务器上各个场景的在线人数
create table tbl_log_service_scene_online_num
(
		le_uId					bigint unsigned			not null,
		lsson_uServerId tinyint	unsigned	not	null,
		lsson_uOnlineUserNum bigint unsigned			not null,
		lsson_sSceneName varchar(96) not null,
		lsson_uSceneType tinyint	unsigned	not	null
		
)engine=innodb;

