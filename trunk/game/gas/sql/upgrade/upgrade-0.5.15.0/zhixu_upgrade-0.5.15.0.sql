alter table tbl_scene_team add t_uBelongID bigint unsigned not null default 0 after t_uId;
alter table tbl_scene_team drop foreign key tbl_scene_team_ibfk_1;