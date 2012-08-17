create table tbl_delay_save_info
(
	cs_uId					bigint	unsigned	not	null,		#role identifier
	s_uId						tinyint unsigned     	not null,
	dsi_uDataId			bigint	unsigned	not	null,
	dsi_dtAddtime		timestamp not null default now(),
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(s_uId)	references tbl_server(s_uId) on	update cascade on delete cascade
)engine=innodb;


#热更新代码记录表
alter table tbl_hot_update_code add huc_filename varchar(64) after huc_type;
alter table tbl_hot_update_code drop huc_server_id;

#玩家战斗力评分
create table tbl_char_fighting_evaluation
(
	cs_uId			bigint unsigned		not	null,
	cfe_uPoint	float unsigned not null,
	
	primary key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#战斗力评分排行
create table tbl_sort_fight_evaluation					#role sorted by fight evaluation
(
	sfe_uOrder smallint unsigned not null,									#sequence id
	sfe_uUpdown tinyint not null,								#up or down
	cs_uId bigint unsigned not null,									#role ID
	sfe_uPoint float unsigned not null,			#total money
	
	primary key(cs_uId),
	key(sfe_uOrder),
	key(sfe_uUpdown),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

#按照阵营对战斗力评分排行榜
create table tbl_sort_fight_evaluation_by_camp #role sorted by fight evaluation group by camp
(
	sfebc_uOrder smallint unsigned not null,										#sequence id
	sfebc_uUpdown tinyint not null ,								#up or down
	cs_uId bigint unsigned not null,										#role ID
	
	primary key(cs_uId),
	key(sfebc_uOrder),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

##Boss争夺战佣兵团击杀Boss次数
create table tbl_tong_boss
(
	t_uId				bigint unsigned			not null,
 	tb_uNum				tinyint unsigned		not null		default 0,
  	primary key(t_uId),
	foreign	key(t_uId)		references tbl_tong(t_uId) 	on	update cascade on delete cascade
)engine=innodb;
