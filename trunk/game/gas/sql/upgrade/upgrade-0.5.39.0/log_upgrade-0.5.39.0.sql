alter table tbl_log_xianxie add lx_uScore tinyint not null default 0 after lx_uWin;

create table tbl_log_create_scene
(
 le_uId			bigint unsigned 	not null,
 lcs_uSceneId	bigint unsigned 	not null,
 
 key(le_uId),
 key(lcs_uSceneId)
)engine=innodb;

CREATE TABLE `tbl_log_normal_delete`(
	`lnd_sName` varchar(96) not	null,
	primary	key(`lnd_sName`)
)engine = innodb;

insert into `tbl_log_normal_delete` values('tbl_log_event');
insert into `tbl_log_normal_delete` values('tbl_log_event_type');
insert into `tbl_log_normal_delete` values('tbl_log_char_deleted');
insert into `tbl_log_normal_delete` values('tbl_log_user');
insert into `tbl_log_normal_delete` values('tbl_log_player');
insert into `tbl_log_normal_delete` values('tbl_log_player_taker');
insert into `tbl_log_normal_delete` values('tbl_log_player_giver');
insert into `tbl_log_normal_delete` values('tbl_log_npc_taker');
insert into `tbl_log_normal_delete` values('tbl_log_npc_giver');
insert into `tbl_log_normal_delete` values('tbl_log_tong_taker');
insert into `tbl_log_normal_delete` values('tbl_log_tong_giver');
insert into `tbl_log_normal_delete` values('tbl_log_tong_honor');
insert into `tbl_log_normal_delete` values('tbl_log_tong_exploit');
insert into `tbl_log_normal_delete` values('tbl_log_tong_level');
insert into `tbl_log_normal_delete` values('tbl_log_tong_money');
insert into `tbl_log_normal_delete` values('tbl_log_tong_resource');
insert into `tbl_log_normal_delete` values('tbl_log_tong_develop_degree');
insert into `tbl_log_normal_delete` values('tbl_log_tong_member_event');
insert into `tbl_log_normal_delete` values('tbl_log_tong_technology_event');
insert into `tbl_log_normal_delete` values('tbl_log_tong_building_event');
insert into `tbl_log_normal_delete` values('tbl_log_tong_item_produce');
insert into `tbl_log_normal_delete` values('tbl_log_tong_change_type');
insert into `tbl_log_normal_delete` values('tbl_log_tong_depot');
insert into `tbl_log_normal_delete` values('tbl_log_tong_station_move');
insert into `tbl_log_normal_delete` values('tbl_log_tong_challenge_start');
insert into `tbl_log_normal_delete` values('tbl_log_army_corps_member_event');
insert into `tbl_log_normal_delete` values('tbl_log_service_online_num');
insert into `tbl_log_normal_delete` values('tbl_log_service_scene_online_num');
insert into `tbl_log_normal_delete` values('tbl_log_login');
insert into `tbl_log_normal_delete` values('tbl_log_logout');
insert into `tbl_log_normal_delete` values('tbl_log_char_login');
insert into `tbl_log_normal_delete` values('tbl_log_money');
insert into `tbl_log_normal_delete` values('tbl_log_depot_money');
insert into `tbl_log_normal_delete` values('tbl_log_item');
insert into `tbl_log_normal_delete` values('tbl_log_item_giver');
insert into `tbl_log_normal_delete` values('tbl_log_item_taker');
insert into `tbl_log_normal_delete` values('tbl_log_experience');
insert into `tbl_log_normal_delete` values('tbl_log_level');
insert into `tbl_log_normal_delete` values('tbl_log_reputation');	
insert into `tbl_log_normal_delete` values('tbl_log_skill');
insert into `tbl_log_normal_delete` values('tbl_log_market');
insert into `tbl_log_normal_delete` values('tbl_log_market_item');
insert into `tbl_log_normal_delete` values('tbl_log_market_buy_order_static');
insert into `tbl_log_normal_delete` values('tbl_log_contract_manufacture_order_static');
insert into `tbl_log_normal_delete` values('tbl_log_contract_manufacture_order');
insert into `tbl_log_normal_delete` values('tbl_log_player_trade');
insert into `tbl_log_normal_delete` values('tbl_log_quest_create');
insert into `tbl_log_normal_delete` values('tbl_log_quest_finish');
insert into `tbl_log_normal_delete` values('tbl_log_quest_giveup');
insert into `tbl_log_normal_delete` values('tbl_log_quest_failure');
insert into `tbl_log_normal_delete` values('tbl_log_enter_scene');
insert into `tbl_log_normal_delete` values('tbl_log_leave_scene');
insert into `tbl_log_normal_delete` values('tbl_log_xianxie');
insert into `tbl_log_normal_delete` values('tbl_log_scene');
insert into `tbl_log_normal_delete` values('tbl_log_soul');
insert into `tbl_log_normal_delete` values('tbl_log_equip_intensify');
insert into `tbl_log_normal_delete` values('tbl_log_equip_upgrade');
insert into `tbl_log_normal_delete` values('tbl_log_equip_intensifyback');
insert into `tbl_log_normal_delete` values('tbl_log_equip_puton');
insert into `tbl_log_normal_delete` values('tbl_log_equip_putoff');
insert into `tbl_log_normal_delete` values('tbl_log_equip_advancedActiveSkill');
insert into `tbl_log_normal_delete` values('tbl_log_player_dead');
insert into `tbl_log_normal_delete` values('tbl_log_player_kill');
insert into `tbl_log_normal_delete` values('tbl_log_npc_name');
insert into `tbl_log_normal_delete` values('tbl_log_char_fight_info');
insert into `tbl_log_normal_delete` values('tbl_log_char_pk');
insert into `tbl_log_normal_delete` values('tbl_log_team_activity');
insert into `tbl_log_normal_delete` values('tbl_log_talent');
insert into `tbl_log_normal_delete` values('tbl_log_reborn_info');
insert into `tbl_log_normal_delete` values('tbl_log_compose_info');
insert into `tbl_log_normal_delete` values('tbl_log_enter_team');
insert into `tbl_log_normal_delete` values('tbl_log_leave_team');
insert into `tbl_log_normal_delete` values('tbl_log_enter_activity');
insert into `tbl_log_normal_delete` values('tbl_log_leave_activity');
insert into `tbl_log_normal_delete` values('tbl_log_xiuxingta');
insert into `tbl_log_normal_delete` values('tbl_log_add_friend');
insert into `tbl_log_normal_delete` values('tbl_log_delete_friend');
insert into `tbl_log_normal_delete` values('tbl_log_jifenpoint');
insert into `tbl_log_normal_delete` values('tbl_log_dataosha_point');
insert into `tbl_log_normal_delete` values('tbl_log_player_logout_position');
insert into `tbl_log_normal_delete` values('tbl_log_commerce_skill');
insert into `tbl_log_normal_delete` values('tbl_log_specialize_smithing');
insert into `tbl_log_normal_delete` values('tbl_log_expert_smithing');
insert into `tbl_log_normal_delete` values('tbl_log_cultivate_flowers');
insert into `tbl_log_normal_delete` values('tbl_log_item_vendue');
insert into `tbl_log_normal_delete` values('tbl_log_item_vendue_end');
insert into `tbl_log_normal_delete` values('tbl_log_equip_intensify_level_up');
insert into `tbl_log_normal_delete` values('tbl_log_exp_change');
insert into `tbl_log_normal_delete` values('tbl_log_char_fighting_evaluation');
insert into `tbl_log_normal_delete` values('tbl_log_tong_resource_sig_up');
insert into `tbl_log_normal_delete` values('tbl_log_coupons_info');
insert into `tbl_log_normal_delete` values('tbl_log_char_bought_coupons');
insert into `tbl_log_normal_delete` values('tbl_log_tong_break');
insert into `tbl_log_normal_delete` values('tbl_log_item_del');