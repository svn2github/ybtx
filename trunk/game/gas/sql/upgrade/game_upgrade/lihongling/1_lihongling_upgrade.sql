alter table tbl_mail add column m_nStrType  tinyint unsigned not null default 0 after m_uLogMailType;
alter table tbl_item_mail_letter add column iml_nStrType tinyint unsigned not null default 0 after ms_uID;
drop table tbl_liveskill_log;