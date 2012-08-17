#添加佣兵小队创始人记录，用于小队指引任务加计数
alter table tbl_tong add t_uInitiatorId	bigint unsigned	not null default 0;

insert into tbl_database_upgrade_record values ("all_upgrade_0.5.0.0.sql");
