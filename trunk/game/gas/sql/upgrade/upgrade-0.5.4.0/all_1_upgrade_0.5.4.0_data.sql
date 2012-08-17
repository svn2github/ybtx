set autocommit=0;

delete from tbl_tong_item_create where tic_sItemName = "国战红药";
delete from tbl_item_static where is_sName = "国战红药";

insert into tbl_database_upgrade_record values("all_1_upgrade_0.5.4.0_data.sql");
commit;
