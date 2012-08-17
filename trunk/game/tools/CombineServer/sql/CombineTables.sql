set autocommit=0;

#第一阶段，合并没有外键的表
#army_corps.sql
insert into ybtx.tbl_army_corps select * from tbl_army_corps;

#association.sql
insert into ybtx.tbl_friends_group select * from tbl_friends_group;
insert into ybtx.tbl_gm_msg select * from tbl_gm_msg;

#erating.sql
insert into ybtx.tbl_erating_detail_id select * from tbl_erating_detail_id;
insert into ybtx.tbl_IBPay_detail select * from tbl_IBPay_detail;
insert into ybtx.tbl_IBPay_log select * from tbl_IBPay_log;
insert into ybtx.tbl_money_exchange select * from tbl_money_exchange;
insert into ybtx.tbl_money_exchange_log select * from tbl_money_exchange_log;
insert into ybtx.tbl_user_draw_balance select * from tbl_user_draw_balance;
insert into ybtx.tbl_user_draw_balance_log select * from tbl_user_draw_balance_log;

#item.sql
insert into ybtx.tbl_item_id select * from tbl_item_id;
insert into ybtx.tbl_item_static select * from tbl_item_static;
insert ignore into ybtx.tbl_item_type_order select * from tbl_item_type_order;
insert ignore into ybtx.tbl_item_smalltype_order select * from tbl_item_smalltype_order;
insert into ybtx.tbl_collectivity_depot select * from tbl_collectivity_depot;

#gm_command.sql
insert into ybtx.tbl_gm_command select * from tbl_gm_command;
insert into ybtx.tbl_gm_command_log select * from tbl_gm_command_log;
insert ignore into ybtx.tbl_freeze_account select * from tbl_freeze_account;
insert ignore into ybtx.tbl_gm_validate select * from tbl_gm_validate;
insert ignore into ybtx.tbl_money_corrency_limit select * from tbl_money_corrency_limit;
insert ignore into ybtx.tbl_jifen_use_limit select * from tbl_jifen_use_limit;
insert ignore into ybtx.tbl_item_use_limit select * from tbl_item_use_limit;
insert into ybtx.tbl_action_closed select * from tbl_action_closed;
insert ignore into ybtx.tbl_quest_finish_limit select * from tbl_quest_finish_limit;
insert ignore into ybtx.tbl_obj_drop_limit select * from tbl_obj_drop_limit;
insert ignore into ybtx.tbl_gms_allowed_ip select * from tbl_gms_allowed_ip;

#log.sql
insert into ybtx.tbl_log_event select * from tbl_log_event;
insert ignore into ybtx.tbl_serial_item select * from tbl_serial_item;
insert ignore into ybtx.tbl_serial_money select * from tbl_serial_money;
insert ignore into ybtx.tbl_serial_depot_money select * from tbl_serial_depot_money;
insert ignore into ybtx.tbl_serial_item_del select * from tbl_serial_item_del;
insert ignore into ybtx.tbl_serial_experience select * from tbl_serial_experience;
insert ignore into ybtx.tbl_serial_soul select * from tbl_serial_soul;
insert ignore into ybtx.tbl_serial_jifen select * from tbl_serial_jifen;
insert into ybtx.tbl_log_channel_chat select * from tbl_log_channel_chat;

#market.sql
insert into ybtx.tbl_market_order_static select * from tbl_market_order_static;
insert into ybtx.tbl_contract_manufacture_order_static select * from tbl_contract_manufacture_order_static;
insert into ybtx.tbl_contract_manufacture_cfg_info select * from tbl_contract_manufacture_cfg_info;

#multiserver.sql
insert into ybtx.tbl_msg_unready_mark select * from tbl_msg_unready_mark;


#pet.sql
insert into ybtx.tbl_pet_static select * from tbl_pet_static;

#scene.sql
insert ignore into ybtx.tbl_scene select * from tbl_scene;

#scene_fb.sql
insert ignore into ybtx.tbl_fbvar_award select * from tbl_fbvar_award;

#server.sql
#insert ignore into ybtx.tbl_conf_server select * from tbl_conf_server; 

#team.sql
insert into ybtx.tbl_team_static select * from tbl_team_static;

#tong.sql
insert ignore into ybtx.tbl_tong_draw_forage_time select * from tbl_tong_draw_forage_time;
insert into ybtx.tbl_tong_rob_resource select * from tbl_tong_rob_resource;
insert into ybtx.tbl_tong_rob_resource_temp select * from tbl_tong_rob_resource_temp;
insert ignore into ybtx.tbl_tong_rob_res_log select * from tbl_tong_rob_res_log;

#toolsmall.sql
insert into ybtx.tbl_toolsmall_buy_detail select * from tbl_toolsmall_buy_detail;
insert ignore into ybtx.tbl_toolsmall_tabtype_memory select * from tbl_toolsmall_tabtype_memory;

#user.sql
insert ignore into ybtx.tbl_user_static select * from tbl_user_static;

#第二阶段  ，合并有外键的表

insert into ybtx.tbl_char_static select * from tbl_char_static;
insert into ybtx.tbl_tong select * from tbl_tong;

#area.sql
insert into ybtx.tbl_area select * from tbl_area;
insert into ybtx.tbl_player_area select * from tbl_player_area;

#army_corps.sql
insert into ybtx.tbl_army_corps_member select * from tbl_army_corps_member;
insert into ybtx.tbl_army_corps_admin select * from tbl_army_corps_admin;
insert into ybtx.tbl_leave_army_corps select * from tbl_leave_army_corps;

#association.sql
insert into ybtx.tbl_friends_class select * from tbl_friends_class;
insert into ybtx.tbl_player_friends select * from tbl_player_friends;
insert into ybtx.tbl_request_addfriend select * from tbl_request_addfriend;
insert into ybtx.tbl_offline_msg select * from tbl_offline_msg;
insert into ybtx.tbl_group_manager select * from tbl_group_manager;
insert into ybtx.tbl_request_add_group select * from tbl_request_add_group;
insert into ybtx.tbl_friends_info select * from tbl_friends_info;
insert into ybtx.tbl_system_message select * from tbl_system_message;

#battle.sql
insert into ybtx.tbl_battle_shape select * from tbl_battle_shape;
insert into ybtx.tbl_battle_array select * from tbl_battle_array;
insert into ybtx.tbl_battle_skill_icon select * from tbl_battle_skill_icon;

#char.sql
insert into ybtx.tbl_char select * from tbl_char;
insert into ybtx.tbl_char_onlinetime select * from tbl_char_onlinetime;
insert into ybtx.tbl_char_basic select * from tbl_char_basic;
insert into ybtx.tbl_char_position select * from tbl_char_position;
insert into ybtx.tbl_char_fb_position select * from tbl_char_fb_position;
insert into ybtx.tbl_char_change_position select * from tbl_char_change_position;
insert into ybtx.tbl_char_experience select * from tbl_char_experience;
insert into ybtx.tbl_char_integral select * from tbl_char_integral;
insert into ybtx.tbl_char_non_combat select * from tbl_char_non_combat;
insert into ybtx.tbl_char_status select * from tbl_char_status;
insert into ybtx.tbl_char_levelup_time select * from tbl_char_levelup_time;
insert into ybtx.tbl_char_last_levelup select * from tbl_char_last_levelup;
insert into ybtx.tbl_char_soul select * from tbl_char_soul;
insert into ybtx.tbl_char_fight_info select * from tbl_char_fight_info;
insert into ybtx.tbl_char_appellation select * from tbl_char_appellation;
insert into ybtx.tbl_char_appellationList select * from tbl_char_appellationList;
insert into ybtx.tbl_char_money select * from tbl_char_money;
insert into ybtx.tbl_duel_count select * from tbl_duel_count;
insert into ybtx.tbl_char_pk select * from tbl_char_pk;
insert into ybtx.tbl_char_pk_flag select * from tbl_char_pk_flag;
insert into ybtx.tbl_char_fighting_evaluation select * from tbl_char_fighting_evaluation;
insert into ybtx.tbl_char_off_line_exp select * from tbl_char_off_line_exp;
insert into ybtx.tbl_char_off_line_exchage_limit_value select * from tbl_char_off_line_exchage_limit_value;
insert into ybtx.tbl_char_afflatus_value select * from tbl_char_afflatus_value;
insert into ybtx.tbl_char_auction_money select * from tbl_char_auction_money;

#direct.sql成就表
insert into ybtx.tbl_char_direct select * from tbl_char_direct;
insert into ybtx.tbl_char_direct_state select * from tbl_char_direct_state;
insert into ybtx.tbl_char_direct_award select * from tbl_char_direct_award;

#erating.sql


#gm_command.sql
insert into ybtx.tbl_forbid_talking select * from tbl_forbid_talking;
insert into ybtx.tbl_forbid_login select * from tbl_forbid_login;

#item.sql
insert into ybtx.tbl_item_bag_in_use select * from tbl_item_bag_in_use;
insert into ybtx.tbl_item_amount_limit select * from tbl_item_amount_limit;
insert into ybtx.tbl_item_store_room select * from tbl_item_store_room;
#insert into ybtx.tbl_item_store_room_lock select * from tbl_item_store_room_lock; 
insert into ybtx.tbl_item_is_binding select * from tbl_item_is_binding;
insert into ybtx.tbl_item_life select * from tbl_item_life;
insert into ybtx.tbl_item_break_exp select * from tbl_item_break_exp;
insert into ybtx.tbl_item_boxitem_pickup select * from tbl_item_boxitem_pickup;
insert into ybtx.tbl_stone_hole select * from tbl_stone_hole;
insert into ybtx.tbl_stone_in_use select * from tbl_stone_in_use;
insert into ybtx.tbl_stone_frame_use select * from tbl_stone_frame_use;
insert into ybtx.tbl_item_collectivity_depot select * from tbl_item_collectivity_depot;
insert into ybtx.tbl_depot_money select * from tbl_depot_money;
insert into ybtx.tbl_item_equip select * from tbl_item_equip;
insert into ybtx.tbl_item_weapon select * from tbl_item_weapon;
insert into ybtx.tbl_item_armor select * from tbl_item_armor;
insert into ybtx.tbl_item_ring select * from tbl_item_ring;
insert into ybtx.tbl_item_shield select * from tbl_item_shield;
insert into ybtx.tbl_item_shield_Attr select * from tbl_item_shield_Attr;
insert into ybtx.tbl_item_equip_advance select * from tbl_item_equip_advance;
insert into ybtx.tbl_char_equip_advancedActiveSkill select * from tbl_char_equip_advancedActiveSkill;
insert into ybtx.tbl_item_equip_intensify select * from tbl_item_equip_intensify;
insert into ybtx.tbl_item_equip_intensifyAddAttr select * from tbl_item_equip_intensifyAddAttr;
insert into ybtx.tbl_item_equip_armor select * from tbl_item_equip_armor;
insert into ybtx.tbl_item_equip_durability select * from tbl_item_equip_durability;
insert into ybtx.tbl_item_maker select * from tbl_item_maker;
insert into ybtx.tbl_item_pearl select * from tbl_item_pearl;
insert into ybtx.tbl_item_pearl_limit select * from tbl_item_pearl_limit;
insert into ybtx.tbl_item_battlebook select * from tbl_item_battlebook;
insert into ybtx.tbl_item_oremap select * from tbl_item_oremap;
insert into ybtx.tbl_item_cool_down select * from tbl_item_cool_down;
insert into ybtx.tbl_item_pickore_dura select * from tbl_item_pickore_dura;
insert into ybtx.tbl_item_soul_bottle select * from tbl_item_soul_bottle;
insert into ybtx.tbl_item_exp_bottle select * from tbl_item_exp_bottle;
insert into ybtx.tbl_contract_manufacture_order select * from tbl_contract_manufacture_order;
insert into ybtx.tbl_contract_manufacture_order_item select * from tbl_contract_manufacture_order_item;


#liveskill.sql
insert into ybtx.tbl_commerce_skill select * from tbl_commerce_skill;
insert into ybtx.tbl_skill_directions select * from tbl_skill_directions;
insert into ybtx.tbl_directions_cool_time select * from tbl_directions_cool_time;
insert into ybtx.tbl_specialize_smithing select * from tbl_specialize_smithing;
insert into ybtx.tbl_expert_smithing select * from tbl_expert_smithing;
insert into ybtx.tbl_liveskill_log select * from tbl_liveskill_log;
insert into ybtx.tbl_flower select * from tbl_flower;

#mail.sql
insert into ybtx.tbl_mail select * from tbl_mail;
insert into ybtx.tbl_mail_item select * from tbl_mail_item;
insert ignore into ybtx.tbl_item_mail_letter select * from tbl_item_mail_letter;

#market.sql
insert into ybtx.tbl_item_market select * from tbl_item_market;
insert into ybtx.tbl_market_buy_order select * from tbl_market_buy_order;
insert into ybtx.tbl_market_sell_order select * from tbl_market_sell_order;
insert into ybtx.tbl_market_sell_price select * from tbl_market_sell_price;
insert into ybtx.tbl_market_buy_price select * from tbl_market_buy_price;
insert into ybtx.tbl_npcshop_buy_tip select * from tbl_npcshop_buy_tip;
insert into ybtx.tbl_market_equip_info select * from tbl_market_equip_info;

#multiserver.sql
insert into ybtx.tbl_delay_save_info select * from tbl_delay_save_info;

#novice_direct.sql
insert into ybtx.tbl_novice_direct select * from tbl_novice_direct;

#pet.sql
insert into ybtx.tbl_pet_egg select * from tbl_pet_egg;
insert into ybtx.tbl_char_pet select * from tbl_char_pet;
insert into ybtx.tbl_pet_skill select * from tbl_pet_skill;
insert into ybtx.tbl_pet_prepare select * from tbl_pet_prepare;

#player_pannel.sql
insert into ybtx.tbl_attribute_panel select * from tbl_attribute_panel;

#quest.sql
insert into ybtx.tbl_quest select * from tbl_quest;
insert into ybtx.tbl_char_quest_finish_num select * from tbl_char_quest_finish_num;
insert into ybtx.tbl_quest_var select * from tbl_quest_var;
insert into ybtx.tbl_quest_tempitem select * from tbl_quest_tempitem;
insert into ybtx.tbl_quest_limit select * from tbl_quest_limit;
insert into ybtx.tbl_mercenary_level select * from tbl_mercenary_level;
insert into ybtx.tbl_mercenary_level_award select * from tbl_mercenary_level_award;
insert into ybtx.tbl_finish_npc_talk select * from tbl_finish_npc_talk;
insert into ybtx.tbl_quest_loop select * from tbl_quest_loop;

#relation.sql
insert into ybtx.tbl_revert_validate select * from tbl_revert_validate;
insert into ybtx.tbl_blackname select * from tbl_blackname;
insert into ybtx.tbl_marriage select * from tbl_marriage;

#scene.sql
insert into ybtx.tbl_scene_arg select * from tbl_scene_arg;

#scene_fb.sql
insert into ybtx.tbl_char_dataoshapoint select * from tbl_char_dataoshapoint;
insert into ybtx.tbl_char_dataoshaaccum select * from tbl_char_dataoshaaccum;
insert into ybtx.tbl_char_jifensaipoint select * from tbl_char_jifensaipoint;
insert into ybtx.tbl_char_jifensaiaccum select * from tbl_char_jifensaiaccum;
insert into ybtx.tbl_area_fb_point select * from tbl_area_fb_point;
insert into ybtx.tbl_activity_count select * from tbl_activity_count;
insert into ybtx.tbl_activity_history_count select * from tbl_activity_history_count;
insert into ybtx.tbl_activity_store_data select * from tbl_activity_store_data;
insert into ybtx.tbl_extra_reward_count select * from tbl_extra_reward_count;
insert into ybtx.tbl_merc_educate_card select * from tbl_merc_educate_card;
insert into ybtx.tbl_char_entered_scene select * from tbl_char_entered_scene;
insert into ybtx.tbl_action_room select * from tbl_action_room;
insert into ybtx.tbl_action_team select * from tbl_action_team;
insert into ybtx.tbl_action_team_member select * from tbl_action_team_member;
insert into ybtx.tbl_action_match_list select * from tbl_action_match_list;

#setting.sql
insert into ybtx.tbl_game_setting select * from tbl_game_setting;
insert into ybtx.tbl_keymap select * from tbl_keymap;
insert into ybtx.tbl_keymap_mouse select * from tbl_keymap_mouse;
insert ignore into ybtx.tbl_ui_setting select * from tbl_ui_setting;

#skill.sql
insert into ybtx.tbl_fight_skill select * from tbl_fight_skill;
insert into ybtx.tbl_skill_layer select * from tbl_skill_layer;
insert into ybtx.tbl_skill_Series select * from tbl_skill_Series;
insert into ybtx.tbl_skill_node select * from tbl_skill_node;
insert into ybtx.tbl_cool_down select * from tbl_cool_down;
insert into ybtx.tbl_aure_magic select * from tbl_aure_magic;
insert into ybtx.tbl_char_magicstate select * from tbl_char_magicstate;
insert into ybtx.tbl_char_magic_obj select * from tbl_char_magic_obj;
insert into ybtx.tbl_char_servant_name select * from tbl_char_servant_name;
insert into ybtx.tbl_char_servant select * from tbl_char_servant;

#team.sql
insert into ybtx.tbl_team select * from tbl_team;
insert into ybtx.tbl_team_list select * from tbl_team_list; 
insert into ybtx.tbl_member_team select * from tbl_member_team;
insert into ybtx.tbl_mark_team select * from tbl_mark_team;
insert into ybtx.tbl_scene_team select * from tbl_scene_team;

#tong.sql
insert into ybtx.tbl_member_tong select * from tbl_member_tong;
insert into ybtx.tbl_log_tong select * from tbl_log_tong;
insert into ybtx.tbl_request_tong select * from tbl_request_tong;
insert into ybtx.tbl_leave_tong select * from tbl_leave_tong;
insert into ybtx.tbl_tong_purview select * from tbl_tong_purview;
insert into ybtx.tbl_building_tong select * from tbl_building_tong;
insert into ybtx.tbl_building_life select * from tbl_building_life;
insert into ybtx.tbl_building_item select * from tbl_building_item;
insert into ybtx.tbl_tong_market select * from tbl_tong_market;
insert into ybtx.tbl_tong_item_create select * from tbl_tong_item_create;
insert into ybtx.tbl_tong_rob_res select * from tbl_tong_rob_res;
insert into ybtx.tbl_tong_rob_res_time select * from tbl_tong_rob_res_time;
insert into ybtx.tbl_tong_challenge select * from tbl_tong_challenge;
insert into ybtx.tbl_tong_challenge_battle select * from tbl_tong_challenge_battle;
insert into ybtx.tbl_tong_tech_static select * from tbl_tong_tech_static;
insert into ybtx.tbl_tong_tech_order select * from tbl_tong_tech_order;
insert into ybtx.tbl_truck_tong select * from tbl_truck_tong;
insert into ybtx.tbl_tong_exploit select * from tbl_tong_exploit;
insert into ybtx.tbl_war_tong select * from tbl_war_tong;
insert into ybtx.tbl_tong_header_award_time select * from tbl_tong_header_award_time;
insert into ybtx.tbl_tong_proffer select * from tbl_tong_proffer;
insert into ybtx.tbl_tong_contribute_money select * from tbl_tong_contribute_money;
insert into ybtx.tbl_tong_type_change select * from tbl_tong_type_change;
insert into ybtx.tbl_tong_draw_forage select * from tbl_tong_draw_forage;
insert into ybtx.tbl_tong_draw_forage_temp select * from tbl_tong_draw_forage_temp;
insert into ybtx.tbl_tong_get_proffer_time select * from tbl_tong_get_proffer_time;
insert into ybtx.tbl_tong_boss select * from tbl_tong_boss;
insert into ybtx.tbl_tong_char_boss select * from tbl_tong_char_boss;
insert into ybtx.tbl_tong_char_other select * from tbl_tong_char_other;
insert into ybtx.tbl_tong_other select * from tbl_tong_other;
insert into ybtx.tbl_tong_jfs select * from tbl_tong_jfs;
insert into ybtx.tbl_tong_station_time select * from tbl_tong_station_time;
insert into ybtx.tbl_tong_line_time select * from tbl_tong_line_time; 

#tong_rob.sql

#toolbar.sql
insert into ybtx.tbl_msgpanel select * from tbl_msgpanel;
insert into ybtx.tbl_msgchannel select * from tbl_msgchannel;
insert into ybtx.tbl_shortcut select * from tbl_shortcut;

#toolsmall.sql
insert into ybtx.tbl_toolsmall_item_buyed select * from tbl_toolsmall_item_buyed;
insert into ybtx.tbl_toolsmall_item_geter select * from tbl_toolsmall_item_geter;
insert ignore into ybtx.tbl_user_charge_detail select * from tbl_user_charge_detail;

#user.sql
insert ignore into ybtx.tbl_user_basic select * from tbl_user_basic;
insert ignore into ybtx.tbl_auth_logout select * from tbl_auth_logout;
insert ignore into ybtx.tbl_user_last_login_logout_time select * from tbl_user_last_login_logout_time;

#user_log.sql
insert into ybtx.tbl_user_advice select * from tbl_user_advice;

#war_zone.sql
insert into ybtx.tbl_station_transfer_time select * from tbl_station_transfer_time;

commit;
