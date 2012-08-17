alter table tbl_quest_loop drop foreign key tbl_quest_loop_ibfk_1;
alter table tbl_quest_loop drop primary key;
alter table tbl_quest_loop add primary key (cs_uId, ql_sLoopName);
alter table tbl_quest_loop add foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade;

drop table tbl_item_exp_soul_bottle;
#魂瓶
create table tbl_item_soul_bottle
(
	cs_uId	bigint unsigned 	not null,	
	is_uId	bigint unsigned 	not null,	
	isb_uState tinyint unsigned not null,  #激活状态 1--激活  2--关闭
	isb_uStorageNum int unsigned not null, #存储量
	
	primary key(cs_uId,is_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

#经验瓶
create table tbl_item_exp_bottle
(
	cs_uId	bigint unsigned 	not null,	
	is_uId	bigint unsigned 	not null,	
	ieb_uState tinyint unsigned not null,  #激活状态 1--激活  2--关闭
	ieb_uStorageNum int unsigned not null, #存储量
	
	primary key(cs_uId,is_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_npcshop_buy_tip(
	cs_uId			bigint(20) 	unsigned not null,
	nbt_LiuTongShopBuyTip	tinyint 	unsigned default 1, #默认提示，0为不提示	
	nbt_CommonShopBuyTip	tinyint 	unsigned default 1, #默认提示，0为不提示	

	primary key (cs_uId),
	foreign key (cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_tong_header_award_time
(
	t_uId				bigint unsigned			not null,
	cs_uId			bigint unsigned 	not null,			#role
	that_dtTime datetime not null,                 
	primary	key(t_uId,cs_uId)
)engine=innodb;

#玩家获得pk积分
create table tbl_char_pk_flag
(
	cs_uId						bigint unsigned		not	null,
	cpf_uId						bigint unsigned		not	null,
	cpf_uFlagTime					bigint unsigned		not	null,
	primary key(cs_uId,cpf_uId),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine = innodb;

drop table if exists tbl_IBPay_detail_memory;
drop table if exists tbl_user_draw_balance_memory;
drop table if exists tbl_IBPay_detail;
drop table if exists tbl_money_exchange;
drop table if exists tbl_user_draw_balance;
drop table if exists tbl_erating_detail_id;
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
	tid_uState						tinyint	not null default 0,
	unique key(tid_uDetailID,tid_uSubjectID),
	key(cs_uId),
	key(tid_uSubjectID)
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
	tme_uState						tinyint not null default 0,
	unique key(tid_uDetailID),
	key(cs_uId),
	key(tme_uSubjectID)
)engine=innodb;

create table tbl_user_draw_balance
(
	tid_uDetailID					bigint unsigned not null,	#detail id
	cs_uId								bigint unsigned not null,			#char_id
	udb_uAmount						bigint unsigned not null,
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

##金钱捐献记录,只记录当天的，每天0点清理一次
create table tbl_tong_contribute_money		#tong member exploit
(       
  cs_uId							bigint unsigned		not null,    	#role identifier
	tcm_uContriNum			bigint unsigned		not null,			#contribute number
	foreign	key (cs_uId)	references tbl_char_static(cs_uId) 	on	update cascade on	delete cascade
)engine=innodb;

#--------------------------更新sql代码分割线---------------------------------------
insert into tbl_database_upgrade_record values ("all_upgrade_0.4.40.0.sql");
