alter table tbl_IBPay_detail drop column tid_uState;

alter table tbl_money_exchange drop column tme_uState;

insert into tbl_database_upgrade_record values("lihongling_3_upgrade-0.5.7.sql");
