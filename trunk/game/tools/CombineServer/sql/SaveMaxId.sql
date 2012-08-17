select @baseindex:=bi_uId from medi_database.tbl_base_Id where bi_sBaseName=database();


set autocommit=0;

update medi_database.tbl_id_add_count set 
#army_corps.sql
tac_uId=tac_uId+(select case when max(ac_uId) is null then 0 else max(ac_uId) end from tbl_army_corps),

#association.sql
#fc_uId=fc_uId+(select case when max(fc_uId) is null then 0 else max(fc_uId) end from tbl_friends_class),
om_uID=om_uID+(select case when max(om_uID) is null then 0 else max(om_uID) end from tbl_offline_msg),
fg_uId=fg_uId+(select case when max(fg_uId) is null then 0 else max(fg_uId) end from tbl_friends_group),
sm_uId=sm_uId+(select case when max(sm_uId) is null then 0 else max(sm_uId) end from tbl_system_message),

#battle.sql
bs_uId=bs_uId+(select case when max(bs_uId) is null then 0 else max(bs_uId) end from tbl_battle_shape), 
ba_uId=ba_uId+(select case when max(ba_uId) is null then 0 else max(ba_uId) end from tbl_battle_array),

#erating.sql
tid_uDetailID=tid_uDetailID+(select case when max(tid_uDetailID) is null then 0 else max(tid_uDetailID) end from tbl_erating_detail_id),


#gm_command.sql
gc_uID=gc_uID+(select case when max(gc_uID) is null then 0 else max(gc_uID) end from tbl_gm_command),
gcl_uID=gcl_uID+(select case when max(gcl_uID) is null then 0 else max(gcl_uID) end from tbl_gm_command_log),
ac_uID=ac_uID+(select case when max(ac_uID) is null then 0 else max(ac_uID) end from tbl_action_closed),

#item.sql
ii_uId=ii_uId+(select case when max(ii_uId) is null then 0 else max(ii_uId) end from tbl_item_id),
is_uId=is_uId+(select case when max(is_uId) is null then 0 else max(is_uId) end from tbl_item_static),
ibp_uId=ibp_uId+(select case when max(ibp_uId) is null then 0 else max(ibp_uId) end from tbl_item_boxitem_pickup),      
sh_uId=sh_uId+(select case when max(sh_uId) is null then 0 else max(sh_uId) end from tbl_stone_hole),
cds_uId=cds_uId+(select case when max(cds_uId) is null then 0 else max(cds_uId) end from tbl_collectivity_depot),

#liveskill.sql
cs_uSID=cs_uSID+(select case when max(cs_uSID) is null then 0 else max(cs_uSID) end from tbl_commerce_skill),

#log.sql
le_uId=le_uId+(select case when max(le_uId) is null then 0 else max(le_uId) end from tbl_log_event),

#mail.sql
ms_uID=ms_uID+(select case when max(ms_uID) is null then 0 else max(ms_uID) end from tbl_mail),

#market.sql
mos_uId=mos_uId+(select case when max(mos_uId) is null then 0 else max(mos_uId) end from tbl_market_order_static),
npsi_nId=npsi_nId+(select case when max(npsi_nId) is null then 0 else max(npsi_nId) end from tbl_npcshop_player_sell_item),
cmo_uId=cmo_uId+(select case when max(cmo_uId) is null then 0 else max(cmo_uId) end from tbl_contract_manufacture_order_static),

#pet.sql
ps_uId=ps_uId+(select case when max(ps_uId) is null then 0 else max(ps_uId) end from tbl_pet_static),

#scene.sql
#tbl_scene和附后只需要保留主场景，且所有sc_uId都用1服的id
#sc_uId=sc_uId+(select case when max(sc_uId) is null then 0 else max(sc_uId) end from tbl_scene),

#scene_fb.sql
tar_uId=tar_uId+(select case when max(ar_uId) is null then 0 else max(ar_uId) end from tbl_action_room),
tat_uId=tat_uId+(select case when max(at_uId) is null then 0 else max(at_uId) end from tbl_action_team),

#team.sql
t_uId=t_uId+(select case when max(t_uId) is null then 0 else max(t_uId) end from tbl_team_static),

#tong.sql
tong_uId=tong_uId+(select case when max(t_uId) is null then 0 else max(t_uId) end from tbl_tong),
lt_uId=lt_uId+(select case when max(lt_uId) is null then 0 else max(lt_uId) end from tbl_log_tong),
rt_uId=rt_uId+(select case when max(rt_uId) is null then 0 else max(rt_uId) end from tbl_request_tong),
bt_uId=bt_uId+(select case when max(bt_uId) is null then 0 else max(bt_uId) end from tbl_building_tong),
tic_uId=tic_uId+(select case when max(tic_uId) is null then 0 else max(tic_uId) end from tbl_tong_item_create),
tts_uId=tts_uId+(select case when max(tts_uId) is null then 0 else max(tts_uId) end from tbl_tong_tech_static),

#toolsmall.sql
tbd_uDetailID=tbd_uDetailID+(select case when max(tbd_uDetailID) is null then 0 else max(tbd_uDetailID) end from tbl_toolsmall_buy_detail),

#user_log.sql
ua_uErrorCode=ua_uErrorCode+(select case when max(ua_uErrorCode) is null then 0 else max(ua_uErrorCode) end from tbl_user_advice),


#war_zone.sql
wz_uId=wz_uId+(select case when max(wz_uId) is null then 0 else max(wz_uId) end from tbl_war_zone)

where ian_sBaseName in (select bi_sBaseName from medi_database.tbl_base_Id where bi_uId>@baseindex);

commit;
