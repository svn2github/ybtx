create table tbl_week_money_exchange	
(
	cs_uId								bigint unsigned		not null,                #role identifier
	wme_uMoney						bigint	unsigned	not	null,  
	wme_dtExchangeTime		datetime          not null,  	#create time
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

create table tbl_week_money_exchange_log	
(
	cs_uId								bigint unsigned		not null,                #role identifier
	wme_uMoney						bigint	unsigned	not	null, 
	wme_dtExchangeTime		datetime          not null,  	#create time
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

create table tbl_money_exchange_cess	
(
	cs_uId							bigint 	unsigned		not null,                #role identifier
	mec_uCess						tinyint	unsigned	not	null,  
	mec_dtUpdateTime		datetime          not null,  	#create time
	unique key(cs_uId),
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;


