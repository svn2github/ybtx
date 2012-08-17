#物品一段时间可以获得的最大数量
create table tbl_item_amount_limit	
(
	cs_uId		bigint 	unsigned	not null,		#item owner
	is_sName	varchar(96)		not	null,		#item name	
	key(is_sName),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;