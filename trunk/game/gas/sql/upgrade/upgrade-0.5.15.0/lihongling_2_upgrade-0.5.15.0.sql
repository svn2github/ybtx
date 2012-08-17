create table tbl_char_money_output						
(
	cs_uId								bigint unsigned not null,	#char_id
	caml_uMoney						bigint unsigned not null,					
	caml_dtTime						datetime not null,				
	key(cs_uId),
	key(caml_dtTime)
)engine=innodb;

create table tbl_erating_audit_log
(	ea_ID								bigint unsigned	not	null,
	ea_uSubjectID				tinyint unsigned not null, 	# type of money
	ea_uTotalOutput			bigint unsigned not null,		#total output 
	ea_uTotalConsume		bigint unsigned not null,		#total consume
	ea_uLeftMoney				bigint unsigned not null,		#total remain money
	ea_dtAuditTime			datetime not null
)engine=innodb;

create table tbl_erating_audit
(	ea_ID								bigint unsigned	not	null auto_increment,
	ea_uSubjectID				tinyint unsigned not null, 	# type of money
	ea_uTotalOutput			bigint unsigned not null,		#total output 
	ea_uTotalConsume		bigint unsigned not null,		#total consume
	ea_uLeftMoney				bigint unsigned not null,		#total remain money
	ea_dtAuditTime			datetime not null,
	key(ea_uSubjectID),
	primary key(ea_ID)
)engine=innodb;

alter table tbl_mail change column m_sContent m_sContent varchar(1024)	not null;
