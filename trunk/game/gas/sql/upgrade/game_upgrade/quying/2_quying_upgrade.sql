create table tbl_tong_get_proffer_time_temp
(
	cs_uId			bigint unsigned 	not null,			#role
	tgptt_sTime  Datetime  not null,                 
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

alter table tbl_tong_header_award_time add foreign key(t_uId)	references tbl_tong(t_uId) on	update cascade on delete cascade;
alter table tbl_tong_header_award_time add foreign key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade;
