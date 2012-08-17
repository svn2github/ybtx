set autocommit=0;

delete from tbl_item_static where is_uType=1 and is_sName="一次性驻地传送";
update ignore tbl_tong a set t_uInitiatorId = (select cs_uId from tbl_member_tong b where a.t_uId = b.t_uId and b.mt_sPosition = 5);

insert into tbl_database_upgrade_record values ("all_upgrade_0.5.0.0_data.sql");
commit;
