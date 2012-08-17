alter table tbl_IBPay_log add column til_uRetCode	int not null default 1 after tid_uIBCodeType;

create table tbl_gm_compenate
(	
	us_uId					bigint unsigned not null,
	gc_uActivityId	bigint	not null,	
	gc_uItemName		varchar(96) not null
)engine=innodb;