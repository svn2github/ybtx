################################################################
#		新手指引信息
################################################################

#是否是第一次杀怪
#是否是第一次抱
#是否是第一次踢
#是否是第一次完美采集

create table tbl_novice_direct	#novice guid
(
	cs_uId		bigint	unsigned	not null, 	# role identifier
	nd_sFirstTime	varchar(96) 	not	null,		# first time to finish sth
	
	primary key (cs_uId, nd_sFirstTime),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;
