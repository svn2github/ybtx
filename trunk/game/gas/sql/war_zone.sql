##战区表
create table tbl_war_zone		# table of war zone
(
	wz_uId			int unsigned     	not null, #sequence id
	sc_uId			bigint	unsigned	not	null,  #scene id that war zone was in
	wz_dtCreateTime		datetime          not null,  	#create time
	primary key(wz_uId),
	foreign	key(sc_uId)      references tbl_scene(sc_uId) 		on	update cascade on	delete cascade
)engine=innodb;


##帮会驻地表
create table tbl_war_zone_station	# war zone info of tongs
(
	t_uId			bigint 	unsigned	not null,	#tong identifier
	wz_uId 			int unsigned  		not null,	#war zone identifier
	wzs_uIndex		int unsigned		not null,	#war zone station index
	wzs_dtOccupyTime	datetime          	not null,  	#create time
	primary key(wz_uId, wzs_uIndex),
	unique key(t_uId),
	foreign	key(t_uId)        references tbl_tong(t_uId) 		on	update cascade on	delete cascade,
	foreign	key(wz_uId)        references tbl_war_zone(wz_uId) 		on	update cascade on	delete cascade
)engine=innodb;

create table tbl_station_transfer_time	#tong transer station time
(
	t_uId			bigint 	unsigned	not null,		#tong id
	stt_lastTime		datetime 		not null,  		#last transfer time
	foreign	key(t_uId)	        references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;


##花语平原哨所所属表
create table tbl_war_zone_sentry	#war zone sentry
(
	wz_uId		int unsigned  			not null,		#war zone index
	wzs_uIndex	tinyint unsigned  		not null,		#sentry index
	wzs_uCamp	tinyint unsigned 		not null,		#camp that belongs to
	primary key(wz_uId, wzs_uIndex),
	foreign	key(wz_uId)	        references tbl_war_zone(wz_uId) 		on	update cascade on	delete cascade
)engine=innodb;
