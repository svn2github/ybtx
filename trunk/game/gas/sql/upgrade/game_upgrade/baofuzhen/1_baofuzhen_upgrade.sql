alter table tbl_char_servant add column cso_uServantID tinyint unsigned	not null default 0 after cso_uLeftTime;
alter table tbl_char_magicstate add column cso_uServantID tinyint unsigned	not null default 0 after cms_uServantName;
alter table tbl_char_magic_obj add column cso_uServantID tinyint unsigned	not null default 0 after cmo_uServantName;