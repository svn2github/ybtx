#佣兵小队篝火表
create table tbl_tong_needfire
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id

	primary	key(t_uId),
	foreign	key(t_uId)	 references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;

#添加木柴表
create table tbl_char_firewood		
(
	cs_uId		bigint	unsigned	not	null,	#role identifier
	cf_dtDateTime	datetime  not null,#add firewood time
	cf_uLevel tinyint unsigned not null,#firewood level
	
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;


alter table tbl_char_firewood drop column cf_uLevel;
alter table tbl_char_firewood add column cf_uAddTimes tinyint not null;
