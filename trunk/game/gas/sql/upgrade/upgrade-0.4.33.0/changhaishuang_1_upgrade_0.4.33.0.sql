alter table tbl_npcshop_player_sell_item modify column npsi_sItemType tinyint unsigned not null;
alter table tbl_npcshop_player_sell_item add foreign key(cs_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade;
alter table tbl_player_sell_item_id add foreign key(psii_nItemId) references tbl_item_static(is_uId) on update cascade on delete cascade;