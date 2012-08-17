#龙穴活动龙头建筑奖励
create table tbl_building_dragon(
		bt_uId		bigint unsigned			not null,
		t_uId			bigint unsigned			not null,
		bt_buildTime		datetime			not null,
		primary key(bt_uId),
		foreign key(bt_uId)	references tbl_building_tong(bt_uId)  on update cascade on delete cascade,
		foreign key(t_uId)	references tbl_tong(t_uId)  on update cascade on delete cascade
)engine = innodb;

create table tbl_scene_dragoncave   #佣兵小队龙穴活动副本列表
(
  t_uId 	bigint 	unsigned	not	null,    #佣兵小队id  
  sc_uId	bigint	unsigned	not	null,	 	#scene idenfier
  primary key(sc_uId),
  foreign key(t_uId) references tbl_tong(t_uId) on update cascade on delete cascade,
	foreign key(sc_uId) references tbl_scene(sc_uId) on delete cascade
)engine = innodb;

create table tbl_tong_dragoncave   #龙穴活动副本已完成佣兵小队角色列表
(
  cs_uId 	bigint	unsigned	not	null,    #角色id
  sc_sFinishTime datetime  not null,	#活动完成时间
	primary key(cs_uId),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_tong_dragoncavetong   #龙穴活动副本已完成佣兵小队列表
(
  t_uId	 bigint	unsigned	not	null, #佣兵小队id
  sc_sFinishTime datetime  not null,	#活动完成时间
	primary key(t_uId),
	foreign key(t_uId) references tbl_tong(t_uId) on update cascade on delete cascade
)engine = innodb;