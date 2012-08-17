create table tbl_grid_in_room	
(
	gir_uGridID		bigint unsigned		not	null auto_increment,#position sequence id
	cs_uId					bigint 	unsigned	not null,		#pos owner
	gir_uRoomIndex 	tinyint	unsigned 	not null,	#index of baggage which item is in
	gir_uPos				tinyint	unsigned 	not null,		#position
	primary key(gir_uGridID),
	key(cs_uId,	gir_uRoomIndex, gir_uPos),
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_item_in_grid	#item index that of main baggage
(
	is_uId		bigint 	unsigned	not	null,		#item identifier
	gir_uGridID		bigint unsigned		not	null,
	unique key(is_uId),
	foreign	key(gir_uGridID)	references tbl_grid_in_room(gir_uGridID) on	update cascade on delete cascade,
	foreign	key(is_uId)	references tbl_item_static(is_uId) on	update cascade on delete cascade
)engine=innodb;

create table tbl_grid_info	
(
	gir_uGridID		bigint unsigned		not	null,
	gi_uCount	int		unsigned 	not null,		#item count
	is_uType	tinyint unsigned		not	null,		#item type
	is_sName	varchar(96)			not	null,		#item name
	unique key(gir_uGridID),
	foreign	key(gir_uGridID)	references tbl_grid_in_room(gir_uGridID) on	update cascade on delete cascade
)engine=innodb;

alter table tbl_mail_item drop foreign key tbl_mail_item_ibfk_2;
alter table tbl_mail_item drop foreign key tbl_mail_item_ibfk_1;
alter table tbl_mail_item drop key is_uId;
alter table tbl_mail_item drop key ms_uID;
alter table tbl_mail_item add key (ms_uID,mi_uSlotIndex);
alter table tbl_mail_item add unique key (is_uId);
alter table tbl_mail_item add foreign	key(is_uId)	references tbl_item_static(is_uId) on update cascade on delete cascade;
alter table tbl_mail_item add foreign	key (ms_uID)	references tbl_mail(ms_uID) on delete cascade on update cascade;



