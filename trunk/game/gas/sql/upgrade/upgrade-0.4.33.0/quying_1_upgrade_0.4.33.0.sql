create table tbl_tong_station_time
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tst_uStationId	bigint unsigned 	not null,			#role
	tst_uTime  bigint unsigned 	not null default 0,			#role              
	primary	key(t_uId, tst_uStationId)
)engine=innodb;

create table tbl_tong_line_time
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tlt_uLineId	bigint unsigned 	not null,			#role
	tlt_uTime  bigint unsigned 	not null default 0,			#role              
	primary	key(t_uId, tlt_uLineId)
)engine=innodb;

create table tbl_tong_draw_forage_temp
(
	tdft_uId   bigint unsigned     	not null,										#char id
	tdft_uTotal    bigint unsigned    	not null,			#forage total num
	primary	key(tdft_uId),
  foreign	key(tdft_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;

alter table tbl_tong_char_other modify column tco_uTypeId  tinyint unsigned not null;