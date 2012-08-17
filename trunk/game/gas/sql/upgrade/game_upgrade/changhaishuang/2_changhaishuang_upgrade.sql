alter table tbl_player_sell_item_id add unique key(psii_nItemId);

alter table tbl_area drop column a_uQuestCount;
alter table tbl_area modify column  a_sAreaName	varchar(64) not	null;
alter table tbl_player_area modify column pa_sAreaName	varchar(64) not	null;

alter table tbl_quest modify column q_uExpMultiple	tinyint unsigned not null;
alter table tbl_char_quest_finish_num modify column cqfn_uFinishNum	smallint unsigned 	not null default 0;
alter table tbl_merc_educate_card modify column mec_uCardId	tinyint	unsigned not null;


alter table tbl_quest modify column q_tAcceptTime	int	unsigned	not null;
alter table tbl_quest modify column q_tFinishTime	int unsigned	not null default 0;