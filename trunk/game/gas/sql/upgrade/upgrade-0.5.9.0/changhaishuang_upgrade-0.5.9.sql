
alter table tbl_system_message drop column sm_sContent;

delete from tbl_char_appellation;
alter table tbl_char_appellation drop column ca_sAppellation;
alter table tbl_char_appellation add column ca_uAppellationId1 smallint unsigned not null default 0;
alter table tbl_char_appellation add column ca_uAppellationId2 smallint unsigned not null default 0;