create table tbl_erating_detail_id
(
	tid_uDetailID					bigint unsigned not null 	auto_increment,	#detail id
	primary key(tid_uDetailID)
)engine=innodb;

create table tbl_IBPay_detail
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
	til_uRetCode					int not null default 1,
	unique key(tid_uDetailID,tid_uSubjectID),
	key(cs_uId),
	key(tid_uSubjectID),
	key(til_uRetCode),
	key(tid_dtBuyTime)
)engine=innodb;

create table tbl_IBPay_detail_memory
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	idm_dtSendTime				datetime not null, #msg send time
	unique key(tid_uDetailID),
	foreign	key (tid_uDetailID)	  references tbl_IBPay_detail(tid_uDetailID) on	update cascade on delete cascade
)engine=MEMORY;

create table tbl_money_exchange
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

create table tbl_user_draw_balance
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	cs_uId								bigint unsigned not null,			#char_id
	udb_uAmount						bigint unsigned not null,
	udb_uType							tinyint unsigned not null,#1 is from gac,2 is from webshop
	primary key(tid_uDetailID),
	key(cs_uId)
)engine=innodb;

create table tbl_user_draw_balance_memory
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	udbm_dtSendTime				datetime not null, #msg send time
	unique key(tid_uDetailID),
	foreign	key (tid_uDetailID)	  references tbl_user_draw_balance(tid_uDetailID) on	update cascade on delete cascade
)engine=MEMORY;

create table tbl_user_draw_balance_log
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	cs_uId								bigint unsigned not null,	#char_id
	udb_uAmount						bigint unsigned not null,
	udbl_RetCode					bigint	not null,					
	udbl_dtTime						datetime not null,
	udb_uType							tinyint unsigned not null,#1 is from gac,2 is from webshop			
	key(tid_uDetailID),
	key(cs_uId),
	key(udbl_dtTime)
)engine=innodb;

create table tbl_money_everyday_output					
(	
	us_uGameID			int	unsigned not null,
	tme_uSubjectID	tinyint unsigned not null,		#subject id
	meo_uOutput			bigint unsigned not null,		#money output
	meo_uConsume		bigint unsigned not null,		#money consume
	unique key(us_uGameID,tme_uSubjectID)
)engine=innodb;

create table tbl_erating_audit_log
(	ea_ID								bigint unsigned	not	null,
	us_uGameID					int unsigned not null,
	ea_uSubjectID				tinyint unsigned not null, 	# type of money
	ea_uTotalOutput			bigint unsigned not null,		#total output 
	ea_uTotalConsume		bigint unsigned not null,		#total consume
	ea_uLeftMoney				bigint unsigned not null,		#total remain money
	ea_dtAuditTime			datetime not null
)engine=innodb;

create table tbl_erating_audit
(	ea_ID								bigint unsigned	not	null auto_increment,
	us_uGameID					int unsigned not null,
	ea_uSubjectID				tinyint unsigned not null, 	# type of money
	ea_uTotalOutput			bigint unsigned not null,		#total output 
	ea_uTotalConsume		bigint unsigned not null,		#total consume
	ea_uLeftMoney				bigint unsigned not null,		#total remain money
	ea_dtAuditTime			datetime not null,
	key(ea_uSubjectID),
	primary key(ea_ID)
)engine=innodb;

create table tbl_webshop_draw_detail
(	
	cs_uId				bigint unsigned not null,
	wdd_uRetCode	bigint	not null,	
	wdd_dtTime		datetime not null,	
	unique key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_gm_compenate
(	
	us_uId					bigint unsigned not null,
	gc_uActivityId	bigint	not null,	
	gc_uItemName		varchar(96) not null,
	gc_uBeginTime		bigint unsigned not null
)engine=innodb;
create table tbl_user_activation_compenate
(	
	uac_uId					bigint unsigned	not	null auto_increment,
	us_uId					int unsigned not null,
	is_uType				tinyint unsigned not	null,		#item type
	is_sName				varchar(96)			not	null,		#item name
	uac_ItemCount		tinyint unsigned not null,
	primary key(uac_uId),
	foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade
)engine=innodb;


