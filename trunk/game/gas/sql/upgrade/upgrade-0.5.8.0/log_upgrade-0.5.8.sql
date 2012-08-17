
drop table tbl_log_gm_command;
drop table tbl_log_service_begin;
drop table tbl_log_service_end;
drop table tbl_log_service_crash;
drop table tbl_log_enter_world;
drop table tbl_log_leave_world;
drop table tbl_log_travel;
drop table tbl_log_change_scene;
drop table tbl_sample_scene_pop;
drop table tbl_log_equip_create;
drop table tbl_log_equip_destroy;
drop table tbl_log_activity_start;
drop table tbl_log_activity_stop;
drop table tbl_log_channel_used;
drop table tbl_log_activity_mark;
alter table tbl_log_item_static add key(lis_uType);
alter table tbl_log_money add key(lm_uMoneyType);

insert into tbl_database_upgrade_record values("log_upgrade-0.5.8.sql");
