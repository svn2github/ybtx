##服务器列表
create table tbl_server			#server list
(
	s_uId							tinyint unsigned     	not null,  #server id
	s_sClientIp				varchar(32)						not null,  #ip listened for clients to connect
	s_sServerIp				varchar(32)						not null,  #ip listened for servers to connect
	s_uClientPort			int unsigned          not null,	 #port listened for clients to connect
	s_uServerPort			int unsigned          not null,	 #port listened for servers to connect
	primary key(s_uId)
)engine=innodb;
	
create table tbl_server_attribute
(
	s_uId							tinyint unsigned     	not null,  #server id
	sa_bCanLogin			tinyint unsigned     	not null,
	sa_bNoFbScene			tinyint unsigned     	not null,
	sa_bNoMainScene		tinyint unsigned     	not null,
	primary key(s_uId)
)engine=innodb;
	
create table tbl_cpu_status
(
	s_uId							tinyint unsigned     	not null,  #server id
	cs_uMaxThreadUsage	float	unsigned			not null,
	cs_uAllUsage				float unsigned			not null,
	primary key(s_uId)
)engine=memory;

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

create table tbl_client_msg
(
	cs_uId					bigint	unsigned	not	null,		#role identifier
	cm_sTypeName		varchar(32)				not	null,
	cm_uId					int unsigned			not	null default 0,
	primary key(cs_uId, cm_sTypeName)
)engine=innodb;


create table tbl_msg_unready_mark
(
	s_uDisServer	tinyint unsigned not	null,
	s_uSrcServer 	tinyint unsigned not	null,
	mc_uId bigint unsigned		not	null,
	key(s_uDisServer),
	key(s_uSrcServer),
	key(mc_uId)
)engine=memory;

