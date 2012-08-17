set autocommit=0;

delete from tbl_sort_char_kill_boss_num;
delete from tbl_sort_char_kill_boss_num_by_camp;
delete from tbl_sort_char_resource;
delete from tbl_sort_char_resource_by_camp;
delete from tbl_sort_create_time;
delete from tbl_sort_dataosha_all_jointimes;
delete from tbl_sort_dataosha_all_jointimes_by_camp;
delete from tbl_sort_dataosha_all_wintimes;
delete from tbl_sort_dataosha_all_wintimes_by_camp;
delete from tbl_sort_dataosha_point;
delete from tbl_sort_dataosha_point_by_camp;
delete from tbl_sort_deadtimes;
delete from tbl_sort_deadtimes_by_camp;
delete from tbl_sort_fight_evaluation;
delete from tbl_sort_fight_evaluation_by_camp;
delete from tbl_sort_jifensai_join_times;
delete from tbl_sort_jifensai_join_times_by_camp;
delete from tbl_sort_jifensai_wintimes;
delete from tbl_sort_jifensai_wintimes_by_camp;
delete from tbl_sort_kill_npc_count;
delete from tbl_sort_kill_npc_count_by_camp;
delete from tbl_sort_kill_player_count;
delete from tbl_sort_kill_player_count_by_camp;
delete from tbl_sort_level;
delete from tbl_sort_level_by_camp;
delete from tbl_sort_money;
delete from tbl_sort_money_by_camp;
delete from tbl_sort_player_onlinetime;
delete from tbl_sort_player_onlinetime_by_camp;
delete from tbl_sort_resource_kill_num;
delete from tbl_sort_resource_kill_num_by_camp;
delete from tbl_sort_tong_exploit;
delete from tbl_sort_tong_exploit_by_camp;
delete from tbl_sort_tong_kill_boss_num;
delete from tbl_sort_tong_kill_boss_num_by_camp;
delete from tbl_sort_tong_kill_num;
delete from tbl_sort_tong_kill_num_by_camp;
delete from tbl_sort_tong_level;
delete from tbl_sort_tong_level_by_camp;
delete from tbl_sort_tong_money;
delete from tbl_sort_tong_money_by_camp;
delete from tbl_sort_tong_resource;
delete from tbl_sort_tong_resource_by_camp;
delete from tbl_sort_tong_resource_occupy_times;
delete from tbl_sort_tong_resource_occupy_times_by_camp;

#delete from tbl_cpu_status;
delete from tbl_long_string_value1;
delete from tbl_arg_common1;
delete from tbl_msg_common1;
delete from tbl_long_string_value2;
delete from tbl_arg_common2;
delete from tbl_msg_common2;
delete from tbl_long_string_value3;
delete from tbl_arg_common3;
delete from tbl_msg_common3;
delete from tbl_long_string_value4;
delete from tbl_arg_common4;
delete from tbl_msg_common4;
delete from tbl_long_string_value5;
delete from tbl_arg_common5;
delete from tbl_msg_common5;
delete from tbl_long_string_value6;
delete from tbl_arg_common6;
delete from tbl_msg_common6;
delete from tbl_long_string_value7;
delete from tbl_arg_common7;
delete from tbl_msg_common7;
delete from tbl_long_string_value8;
delete from tbl_arg_common8;
delete from tbl_msg_common8;
delete from tbl_long_string_value9;
delete from tbl_arg_common9;
delete from tbl_msg_common9;
delete from tbl_long_string_value10;
delete from tbl_arg_common10;
delete from tbl_msg_common10;
delete from tbl_long_string_value11;
delete from tbl_arg_common11;
delete from tbl_msg_common11;
delete from tbl_long_string_value12;
delete from tbl_arg_common12;
delete from tbl_msg_common12;
delete from tbl_long_string_value13;
delete from tbl_arg_common13;
delete from tbl_msg_common13;
delete from tbl_long_string_value14;
delete from tbl_arg_common14;
delete from tbl_msg_common14;
delete from tbl_long_string_value15;
delete from tbl_arg_common15;
delete from tbl_msg_common15;

delete from tbl_hot_update_gas;
delete from tbl_hot_update_gac;
delete from tbl_hot_update_db;
delete from tbl_hot_update_result;

delete from tbl_IBPay_detail_memory;
delete from tbl_user_draw_balance_memory;

delete from tbl_tong_rob_state;
delete from tbl_tong_rob_scene;

delete from tbl_char_server_lastleve;
delete from tbl_var_server;
delete from tbl_war_zone_sentry;
delete from tbl_war_zone_station;
delete from tbl_user_online;
delete from tbl_server;
delete from tbl_user_wait_queue;
delete from tbl_channel_id;
delete from tbl_channel_name;
delete from ONLINE_STAT;
delete from tbl_server_version;
delete from tbl_user_logout;
delete from tbl_npcshop_player_sell_item;
delete from tbl_player_sell_item_id;
delete from tbl_market_cfg_info;
delete from tbl_char_deleted;

#tbl_erating_detail_id会每10分钟清一次，要是取到的数据刚刚清空，合并后自增id就是0了，所以在合并之前先插入一个，这样就不会出现这个表为空的情况，暂时先放在这里
insert into tbl_erating_detail_id values();

#场景表只留下主场景
delete from tbl_scene 
where sc_sSceneName != '轻歌林地' 
and sc_sSceneName != '海帕斯高原' 
and sc_sSceneName != '艾米帝国树屋酒吧' 
and sc_sSceneName != '神圣教廷树屋酒吧' 
and sc_sSceneName != '修斯帝国树屋酒吧' 
and sc_sSceneName != '尼瑞克平原' 
and sc_sSceneName != '雁南郡' 
and sc_sSceneName != '界林山区' 
and sc_sSceneName != '前线山区' 
and sc_sSceneName != '史坎布雷' 
and sc_sSceneName != '三乡小镇'
and sc_sSceneName != '银杏村'
and sc_sSceneName != '通云关'
and sc_sSceneName != '双子镇'
and sc_sSceneName != '天梁都'
and sc_sSceneName != '乌鲁城'
and sc_sSceneName != '河阳大草原'
and sc_sSceneName != '狮子河谷地'
and sc_sSceneName != '圣雪山'
and sc_sSceneName != '西林岛'
and sc_sSceneName != '测试1'
and sc_sSceneName != '测试地图'
and sc_sSceneName != '刷门'
and sc_sSceneName != '狩猎入口'
and sc_sSceneName != '地表测试地图'
and sc_sSceneName != '艾米帝国职业圣殿'
and sc_sSceneName != '神圣教廷职业圣殿'
and sc_sSceneName != '修斯帝国职业圣殿'
and sc_sSceneName != '测试用帮会挑战地图'
and sc_sSceneName != '前线运输'
and sc_sSceneName != '测试用新花语平原'
and sc_sSceneName != '新轻歌林地'
and sc_sSceneName != '花语平原1'
and sc_sSceneName != '新雁南郡';

commit;
