#交易所求购订单表
create table tbl_log_market_buy_order_static
(			
	le_uId		bigint unsigned	not	null,
	lmos_uId				bigint unsigned		not null,		# order identifier
	lmbo_sItemName		varchar(96)			not null,		# acquired item name
	lmbo_uCount			bigint unsigned		not null,		# acquired item number
	lmbo_uPrice			bigint unsigned		not null,		# item price
	
	key(le_uId),
	key(lmos_uId),
	key(lmbo_sItemName)
)engine=innodb;	