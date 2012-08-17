select @nowbase:=database();
set autocommit=0;

#army_corps
select @maxid:=case when max(ac_uId) is null then 0 else max(ac_uId) end from tbl_army_corps;
select @addnum:=@maxid+tac_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_army_corps set ac_uId=ac_uId+@addnum;


#association.sql
#select @maxid:=case when max(fc_uId) is null then 0 else max(fc_uId) end from tbl_friends_class;
#select @addnum:=@maxid+fc_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
#update tbl_friends_class set fc_uId=fc_uId+@addnum where fc_uId>2; #因为fc_uId 的 1，2默认为”我的好友“，”黑名单“，所以大于2的ID才进行进行偏移

select @maxid:=case when max(om_uID) is null then 0 else max(om_uID) end from tbl_offline_msg;
select @addnum:=@maxid+om_uID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_offline_msg set om_uID=om_uID+@addnum;

select @maxid:=case when max(fg_uId) is null then 0 else max(fg_uId) end from tbl_friends_group;
select @addnum:=@maxid+fg_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_friends_group set fg_uId=fg_uId+@addnum;

select @maxid:=case when max(sm_uId) is null then 0 else max(sm_uId) end from tbl_system_message;
select @addnum:=@maxid+sm_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_system_message set sm_uId=sm_uId+@addnum;


#battle.sql
select @maxid:=case when max(bs_uId) is null then 0 else max(bs_uId) end from tbl_battle_shape;
select @addnum:=@maxid+bs_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_battle_shape set bs_uId=bs_uId+@addnum;

select @maxid:=case when max(ba_uId) is null then 0 else max(ba_uId) end from tbl_battle_array;
select @addnum:=@maxid+ba_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_battle_array set ba_uId=ba_uId+@addnum;


#erating.sql
select @maxid:=case when max(tid_uDetailID) is null then 0 else max(tid_uDetailID) end from tbl_erating_detail_id;
select @addnum:=@maxid+tid_uDetailID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_erating_detail_id set tid_uDetailID=tid_uDetailID+@addnum;


#gm_command.sql
select @maxid:=case when max(gc_uID) is null then 0 else max(gc_uID) end from tbl_gm_command;
select @addnum:=@maxid+gc_uID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_gm_command set gc_uID=gc_uID+@addnum;

select @maxid:=case when max(gcl_uID) is null then 0 else max(gcl_uID) end from tbl_gm_command_log;
select @addnum:=@maxid+gcl_uID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_gm_command_log set gcl_uID=gcl_uID+@addnum;

select @maxid:=case when max(ac_uID) is null then 0 else max(ac_uID) end from tbl_action_closed;
select @addnum:=@maxid+ac_uID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_action_closed set ac_uID=ac_uID+@addnum;


#item.sql
select @maxid:=case when max(ii_uId) is null then 0 else max(ii_uId) end from tbl_item_id;
select @addnum:=@maxid+ii_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_item_id set ii_uId=ii_uId+@addnum;

select @maxid:=case when max(is_uId) is null then 0 else max(is_uId) end from tbl_item_static;
select @addnum:=@maxid+is_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_item_static set is_uId=is_uId+@addnum;

select @maxid:=case when max(ibp_uId) is null then 0 else max(ibp_uId) end from tbl_item_boxitem_pickup;
select @addnum:=@maxid+ibp_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_item_boxitem_pickup set ibp_uId=ibp_uId+@addnum;

select @maxid:=case when max(sh_uId) is null then 0 else max(sh_uId) end from tbl_stone_hole;
select @addnum:=@maxid+sh_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_stone_hole set sh_uId=sh_uId+@addnum;

select @maxid:=case when max(cds_uId) is null then 0 else max(cds_uId) end from tbl_collectivity_depot;
select @addnum:=@maxid+cds_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_collectivity_depot set cds_uId=cds_uId+@addnum;


#liveskill.sql
select @maxid:=case when max(cs_uSID) is null then 0 else max(cs_uSID) end from tbl_commerce_skill;
select @addnum:=@maxid+cs_uSID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_commerce_skill set cs_uSID=cs_uSID+@addnum;


#log.sql
select @maxid:=case when max(le_uId) is null then 0 else max(le_uId) end from tbl_log_event;
select @addnum:=@maxid+le_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_log_event set le_uId=le_uId+@addnum;


#mail.sql
select @maxid:=case when max(ms_uID) is null then 0 else max(ms_uID) end from tbl_mail;
select @addnum:=@maxid+ms_uID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_mail set ms_uID=ms_uID+@addnum;


#market.sql
select @maxid:=case when max(mos_uId) is null then 0 else max(mos_uId) end from tbl_market_order_static;
select @addnum:=@maxid+mos_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_market_order_static set mos_uId=mos_uId+@addnum;

select @maxid:=case when max(npsi_nId) is null then 0 else max(npsi_nId) end from tbl_npcshop_player_sell_item;
select @addnum:=@maxid+npsi_nId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_npcshop_player_sell_item set npsi_nId=npsi_nId+@addnum;

select @maxid:=case when max(cmo_uId) is null then 0 else max(cmo_uId) end from tbl_contract_manufacture_order_static;
select @addnum:=@maxid+cmo_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_contract_manufacture_order_static set cmo_uId=cmo_uId+@addnum;


#pet.sql
select @maxid:=case when max(ps_uId) is null then 0 else max(ps_uId) end from tbl_pet_static;
select @addnum:=@maxid+ps_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_pet_static set ps_uId=ps_uId+@addnum;


#scene.sql
#select @maxid:=case when max(sc_uId) is null then 0 else max(sc_uId) end from tbl_scene;
#select @addnum:=@maxid+sc_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
#update tbl_scene set sc_uId=sc_uId+@addnum;


#scene_fb.sql
select @maxid:=case when max(ar_uId) is null then 0 else max(ar_uId) end from tbl_action_room;
select @addnum:=@maxid+tar_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_action_room set ar_uId=ar_uId+@addnum;

select @maxid:=case when max(at_uId) is null then 0 else max(at_uId) end from tbl_action_team;
select @addnum:=@maxid+tat_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_action_team set at_uId=at_uId+@addnum;


#team.sql
select @maxid:=case when max(t_uId) is null then 0 else max(t_uId) end from tbl_team_static;
select @addnum:=@maxid+t_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_team_static set t_uId=t_uId+@addnum;


#tong.sql
select @maxid:=case when max(t_uId) is null then 0 else max(t_uId) end from tbl_tong;
select @addnum:=@maxid+tong_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_tong set t_uId=t_uId+@addnum;

select @maxid:=case when max(lt_uId) is null then 0 else max(lt_uId) end from tbl_log_tong;
select @addnum:=@maxid+lt_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_log_tong set lt_uId=lt_uId+@addnum;

select @maxid:=case when max(rt_uId) is null then 0 else max(rt_uId) end from tbl_request_tong;
select @addnum:=@maxid+rt_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_request_tong set rt_uId=rt_uId+@addnum;

select @maxid:=case when max(bt_uId) is null then 0 else max(bt_uId) end from tbl_building_tong;
select @addnum:=@maxid+bt_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_building_tong set bt_uId=bt_uId+@addnum;

select @maxid:=case when max(tic_uId) is null then 0 else max(tic_uId) end from tbl_tong_item_create;
select @addnum:=@maxid+tic_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_tong_item_create set tic_uId=tic_uId+@addnum;

select @maxid:=case when max(tts_uId) is null then 0 else max(tts_uId) end from tbl_tong_tech_static;
select @addnum:=@maxid+tts_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_tong_tech_static set tts_uId=tts_uId+@addnum;



#toolsmall.sql
select @maxid:=case when max(tbd_uDetailID) is null then 0 else max(tbd_uDetailID) end from tbl_toolsmall_buy_detail;
select @addnum:=@maxid+tbd_uDetailID from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_toolsmall_buy_detail set tbd_uDetailID=tbd_uDetailID+@addnum;


#user_log.sql
select @maxid:=case when max(ua_uErrorCode) is null then 0 else max(ua_uErrorCode) end from tbl_user_advice;
select @addnum:=@maxid+ua_uErrorCode from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_user_advice set ua_uErrorCode=ua_uErrorCode+@addnum;


#war_zone.sql
select @maxid:=case when max(wz_uId) is null then 0 else max(wz_uId) end from tbl_war_zone;
select @addnum:=@maxid+wz_uId from medi_database.tbl_id_add_count where ian_sBaseName=@nowbase;
update tbl_war_zone set wz_uId=wz_uId+@addnum;

commit;
