create table tbl_log_matchgame_server		#matchgame服务端流程记录
(
	lms_uRoomId  bigint unsigned not null,		#活动房间
	lms_sActionName varchar(32),							#活动名
	lms_uSceneId bigint unsigned not null,		#活动房间
	lms_uIndex tinyint unsigned not null,			#用于拆分保存超出sLog长度的记录
	lms_dtTime datetime not null default 0,		#保存时间
	lms_sLog	 varchar(1024),									#活动流程记录
	
	key(lms_uRoomId),
	key(lms_uSceneId),
	key(lms_uIndex)
)engine = innodb;

create table tbl_log_matchgame_room					#matchgame数据库Room创建记录
(
	lmr_uRoomId bigint unsigned not null,			#活动房间
	lmr_sActionName varchar(32),							#活动名
	lmr_uSceneId bigint unsigned not null,		#活动场景
	lmr_uServerId tinyint unsigned not null,	#服务器id
	lmr_dtTime datetime not null default 0,		#创建时间
	
	key(lmr_uRoomId),
	key(lmr_uSceneId),
	key(lmr_uServerId)
)engine = innodb;

create table tbl_log_matchgame_member				#matchgame数据库玩家操作记录
(
	lcs_uId			bigint unsigned not null,
	lmm_sActionName varchar(32),							#活动名
	lmm_uTeamId bigint unsigned not null,
	lmm_uRoomId	bigint unsigned not null,			#活动房间
	lmm_sFuncName varchar(16),								#操作函数
	lmm_sState varchar(32),										#返回函数
	lmm_dtTime datetime not null default 0,		#保存时间
	
	key(lcs_uId),
	key(lmm_uTeamId),
	key(lmm_uRoomId)
)engine = innodb;