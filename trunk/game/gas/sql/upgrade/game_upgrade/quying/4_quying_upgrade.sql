drop table tbl_tong_get_proffer_time;

create table tbl_tong_get_proffer_time
(
	cs_uId			bigint unsigned 	not null,			#role
	tgpt_sTime  Datetime  not null,                 
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


#记录资源点奖励数据清理时间
create table tbl_tong_res_award_time(
		rat_dtTime		DateTime        not null       	#clear time
)engine = innodb;
