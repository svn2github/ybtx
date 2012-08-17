alter table tbl_IBPay_detail add column tid_uIBCodeType	tinyint not null default 0 after tid_uState;

insert into tbl_database_upgrade_record values("lihongling_upgrade-0.5.5.0.sql");
