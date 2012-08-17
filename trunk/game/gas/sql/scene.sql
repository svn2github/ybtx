
################################################################
#		场景
################################################################

#	Scene	信息 保管，暂时只对每个	CoreScene	记录一个 uint64	作为附加信息
create table tbl_scene			#table of all scenes including dungeons 
(
	 sc_uId			bigint	unsigned	not	null	auto_increment,	 	#scene idenfier
	 sc_sSceneName		varchar(96) 		not	null,		#scene name
	 sc_uServerId		tinyint unsigned	not	null, 		#server that scene belongs to
	 sc_dtCreationDateTime	datetime		not	null,		#create time
	 sc_dtLastActiveDateTime	datetime	not	null,		
	 sc_uProcess		bigint		unsigned default 0,		
	 sc_uType		tinyint unsigned not null, 			#scene type
	 primary key(	sc_uId ),
	 key(sc_sSceneName),
	 key(sc_uProcess),
	 key(sc_uType)
)engine=innodb;

create table tbl_scene_distribute
(
	sc_sSceneName  varchar(96) 		not	null,		#scene name
	sc_uServerId	tinyint unsigned	not	null,
	primary key(sc_sSceneName)
)engine=innodb;


#场景额外参数
create table tbl_scene_arg		#scene extro info
(
	 sc_uId		bigint	unsigned 	not	null	auto_increment,
	 sa_sArgName	varchar(16) 		not	null, #变量名
	 sa_sArgValue	varchar(96),			#字符串变量值 		sa_nArgValue 和 sa_sArgValue 只有一个存在
	 sa_nArgValue	int,				#数字变量值		sa_nArgValue 和 sa_sArgValue 只有一个存在
	 primary key(	sc_uId,  sa_sArgName),
	 foreign	key(sc_uId)	references tbl_scene(sc_uId) on	update cascade on delete cascade
)engine=innodb;



# 1   艾米   2 神圣教廷  3 休斯   yy

#	初始化 tbl_scene 表, 注意	MetaScene	编号要和 Scene_Common 里的一致
#insert into
#	tbl_scene( sc_sSceneName, sc_dtCreationDateTime, sc_dtLastActiveDateTime , sc_uProcess )
#values
#	(	'轻歌林地',	now(),	now(),	0	),
#	(	'尼瑞克平原',	now(),	now(),	0	),
#	(   '雁南郡',   now(),  now(),  0 ),
#	(	'测试1',	now(),	now(),	0	),
#	(	'测试2',	now(),	now(),	0	),
#	(	'测试3',	now(),	now(),	0	),
#	(	'测试4',	now(),	now(),	0	);
