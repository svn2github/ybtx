
drop table if exists tbl_item_pos_count;
drop table if exists tbl_item_count;
create table tbl_item_count	
(
	cs_uId		bigint 	unsigned	not null,		#item owner
	isr_uRoomIndex tinyint	unsigned 	not null,	#index of baggage which item is in
	isr_uPos	tinyint	unsigned 	not null,		#position
	ic_uCount	int		unsigned 	not null,		#item count
	is_uType	tinyint unsigned		not	null,		#item type
	is_sName	varchar(96)			not	null,		#item name
	unique key(cs_uId,	isr_uRoomIndex, isr_uPos),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;