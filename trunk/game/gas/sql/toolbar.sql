################################################################
#       用户配置收取消息的面板
#       
################################################################
#聊天信息面板信息
create table tbl_msgpanel	#settings of message panel
(
	cs_uId			bigint unsigned 	not null,   		#role identifier
	panel_name		varchar(36)		not null,		#panel name
	panel_position		tinyint			not null,		#panel position
	action			tinyint			not null,		#set or unset
	primary key (cs_uID, panel_position),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

#面板中被订制或者被取消订制的频道
create table tbl_msgchannel	#channels that be setted or unsetted
(
	cs_uID				bigint unsigned 	not null,   	#role identifier
	panel_position		tinyint				not null,	#panel position
	mc_uChannelId		tinyint unsigned	not null,		#panel name
	mc_sAction			tinyint unsigned	not null,	#action: choose or unchoose
	primary key (cs_uID, panel_position, mc_uChannelId),
	foreign key (cs_uID, panel_position) references tbl_msgpanel (cs_uID, panel_position) on update cascade on delete cascade
)engine=innodb;


################################################################
#		角色快捷栏存储表
################################################################

create table tbl_shortcut	#shot cut
(
	cs_uId						bigint 	unsigned		not	null,		#玩家ID
	sc_uPos						tinyint unsigned  	not null,			#快捷栏格子位置
	sc_Type						tinyint	unsigned		not null,		#格子的属性类型：技能、物品、
	sc_Arg1						varchar(96)			not null,			#技能名称、物品名称
	sc_Arg2						bigint	unsigned		not null,		#技能等级、物品大类
	sc_Arg3						bigint	unsigned		not null,		#快捷栏ID
	
	primary	key(cs_uId, sc_uPos, sc_Arg3),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	delete cascade on update cascade
)engine=innodb;
