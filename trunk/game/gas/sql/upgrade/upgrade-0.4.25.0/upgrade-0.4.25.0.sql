
create table tbl_toolsmall_money_pay
( 
  tmp_uDetailID bigint unsigned not null, #detailid
  tmp_PayType   tinyint unsigned  not null,
  tmp_uSubjectID tinyint unsigned not null,
  tmp_nMoney  bigint unsigned not null,
  unique key(tmp_uSubjectID,tmp_PayType,tmp_uDetailID)
)engine=innodb;

drop table if exists tbl_toolsmall_buy_detail;
drop table if exists tbl_toolsmall_item_buyed;
drop table if exists tbl_toolsmall_item_geter;
drop table if exists tbl_toolsmall_money_pay;
create table tbl_toolsmall_buy_detail
(
	tbd_uDetailID					bigint unsigned not null 	auto_increment,	#detail id
	cs_uId								bigint unsigned not null,			#char_id
	cs_uLevel							tinyint unsigned	not	null,		#char_level
	tbd_uRatingID					int unsigned not null,			#rating_id
	tbd_IBCode						varchar(96)			not null,			#item_name
	tbd_uPackageFlag			tinyint unsigned	not	null,		#package_flag
	tbd_uBuyCount					int	unsigned not null,			#count	
	tbl_uIP								int	unsigned not null,			#user_ip
	tbd_uSubjectID				tinyint unsigned not null,	#subject id
	tbd_AuditFlag					int	unsigned not null,			#audit_flag
	tbd_uPrice						int	unsigned not null,			#price
	tbd_uDiscountPrice		int	unsigned not null,			#discount_price
	tbd_dtBuyTime					datetime		not null,				#pay_time
	primary key(tbd_uDetailID),
	key(cs_uId),
	key(cs_uLevel),
	key(tbd_uRatingID),
	key(tbd_uSubjectID)
)engine=innodb;

##购买物品记录
create table tbl_toolsmall_item_buyed
(	
	tbd_uDetailID		bigint unsigned not null 	,	#detail_id
	is_uId					bigint unsigned		not null,#item_id
	unique key(is_uId),
	key(tbd_uDetailID),
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade,
	foreign	key(tbd_uDetailID) references tbl_toolsmall_buy_detail(tbd_uDetailID) on	update cascade on delete cascade
)engine=innodb;

#被赠送者
create table tbl_toolsmall_item_geter
(
	tbd_uDetailID	bigint unsigned not null,	#order id
	cs_uGeterID		bigint unsigned not null,	#buyer id
	unique key(tbd_uDetailID),
	foreign	key(tbd_uDetailID)	references tbl_toolsmall_buy_detail(tbd_uDetailID) on	update cascade on delete cascade
)engine=innodb;

