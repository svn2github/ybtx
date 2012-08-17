alter table tbl_tong_contribute_money add column tcm_uContriNumforExp bigint unsigned not null default 0 after tcm_uContriNum;
alter table tbl_tong_contribute_money add column tcm_uContriNumforSoul bigint unsigned not null default 0 after tcm_uContriNumforExp;
alter table tbl_tong_contribute_money add column tcm_uContriNumforResource bigint unsigned not null default 0 after tcm_uContriNumforSoul;
alter table tbl_tong_contribute_money add primary key(cs_uId);
alter table tbl_tong modify t_uResource float not null;