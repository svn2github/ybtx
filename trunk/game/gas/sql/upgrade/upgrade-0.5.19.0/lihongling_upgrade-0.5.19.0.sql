alter table tbl_item_mail_letter change column iml_uMailContext iml_uMailContext varchar(1024)	not null;

create table tbl_webshop_draw_detail
(	
	cs_uId				bigint unsigned not null,
	wdd_uRetCode	bigint	not null,	
	wdd_dtTime		datetime not null,	
	unique key(cs_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

alter table tbl_user_draw_balance add column udb_uType	tinyint unsigned not null default 1 after udb_uAmount;
alter table tbl_user_draw_balance_log add column udb_uType	tinyint unsigned not null default 1 after udbl_dtTime;