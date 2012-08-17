drop table if exists tbl_char_money_output;
create table tbl_money_everyday_output					
(	
	us_uGameID			int	unsigned not null,
	tme_uSubjectID	tinyint unsigned not null,		#subject id
	meo_uOutput			bigint unsigned not null,		#money output
	meo_uConsume		bigint unsigned not null,		#money consume
	unique key(us_uGameID,tme_uSubjectID)
)engine=innodb;
create table tbl_char_lefttime	
(
	cs_uId				bigint	unsigned	not	null, 	#role identifier
	cl_dtLeftTime	bigint	unsigned	not null,
	
	unique key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;