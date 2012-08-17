#消费记录表
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
alter table tbl_char_money add column cm_uRmbMoney	bigint unsigned	not null 	default 0;


drop table if exists tbl_skill_directions;
drop table if exists tbl_directions_cool_time;
##学会的配方
create table tbl_skill_directions
(
	cs_uId 						bigint unsigned 		not null,
	sd_sName					varchar(96)					not null, ##学会的配方
	sd_sSkillName				varchar(96)					not null,  ##所属技能
	sd_sDirectionType		varchar(96)					not null,  ##配方类型
	unique key(cs_uId,sd_sSkillName,sd_sDirectionType,sd_sName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

create table tbl_directions_cool_time
(
	cs_uId 						bigint unsigned 		not null,
	sd_sName					varchar(96)					not null, ##学会的配方
	sd_sSkillName				varchar(96)					not null,  ##所属技能
	sd_sDirectionType		varchar(96)					not null,  ##配方类型
	sd_dtStartTime			datetime not null, #冷却开始时间
	unique key(cs_uId,sd_sSkillName,sd_sDirectionType,sd_sName),
	foreign key(cs_uId) references tbl_char_static(cs_uId) on delete cascade on update cascade
)engine=innodb;

insert into tbl_database_upgrade_record values("lihongling_1_upgrade_0.4.36.0.sql");
