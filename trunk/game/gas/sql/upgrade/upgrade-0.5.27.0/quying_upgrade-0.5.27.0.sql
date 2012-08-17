#阵营状态
create table tbl_recruit_camp
(
	rc_uCamp		tinyint unsigned	not null,   
	rc_uState	tinyint unsigned	not null,   #状态(0,1,2)  --0关闭,1普通,2重金
	primary	key(rc_uCamp)
)engine = innodb;



#玩家通过招募创建角色
create table tbl_char_recruit_info
(
	cs_uId			bigint unsigned		not	null,
	cri_uRecFlag	tinyint	unsigned not	null,	#recruit flag
	primary	key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;


#招募更新
create table tbl_char_recruit_time
(
	crt_dtTime	datetime not null
)engine = innodb;


#重金招募信息
create table tbl_recruit_info
(
	ri_uMinNum	smallint 	unsigned	not null, 
	ri_uMaxNum	smallint 	unsigned	not null 
)engine = innodb;


alter table tbl_tong_rob_resource add unique key(trr_sObjName);