create table tbl_IBPay_log
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	tid_uSubjectID				tinyint unsigned not null,		#subject id
	cs_uId								bigint unsigned not null,			#char_id
	cs_uLevel							tinyint unsigned not	null,		#char_level
	tid_sIP								varchar(45)	not null,				#char_ip
	tid_IBCode						varchar(96)	not null,					#item_name
	tid_uBuyCount					int	unsigned not null,				#count	
	tid_uPrice						bigint	unsigned not null,				#price
	tid_dtBuyTime					datetime not null,					#pay_time
	tid_uIBCodeType				tinyint not null default 0,
	unique key(tid_uDetailID,tid_uSubjectID),
	key(cs_uId),
	key(tid_uSubjectID)
)engine=innodb;

create table tbl_money_exchange_log
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	tme_uSubjectID				tinyint unsigned not null,		#subject id
	cs_uId								bigint unsigned not null,			#char_id
	cs_uTargetID					bigint unsigned not null,			#target_id
	tme_uAmount						int	unsigned not null,				#price
	tme_dtTime						datetime not null,					#pay_time
	unique key(tid_uDetailID),
	key(cs_uId),
	key(tme_uSubjectID)
)engine=innodb;


insert into tbl_database_upgrade_record values("lihongling_1_upgrade-0.5.7.sql");
