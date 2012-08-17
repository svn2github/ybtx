#更新装备进阶魂根
update tbl_item_equip_advance, tbl_temp_id_soul set iea_sAdvancedSoulRoot =  soul where is_uId = id ;

#更新装备进阶属性
update tbl_item_equip_advance, temp_equip_advanceattr 
set iea_sAdvancedAttr1 = tea_sAdvanceAttr1, 
iea_sAdvancedAttr2 = tea_sAdvanceAttr2,
iea_sAdvancedAttr3 = tea_sAdvanceAttr3,
iea_sAdvancedAttr4 = tea_sAdvanceAttr4 where iea_sAdvancedSoulRoot = tea_sAdvanceSoul;

#更新装备强化魂根
update tbl_item_weapon, temp_equip_inten_id_soul  set iw_sIntenSoul = soul where is_uId = id ;  
update tbl_item_armor, 	temp_equip_inten_id_soul  set ia_sIntenSoul = soul where is_uId = id ;  
update tbl_item_ring, 	temp_equip_inten_id_soul  set ir_sIntenSoul = soul where is_uId = id ;  
update tbl_item_shield, temp_equip_inten_id_soul  set is_sIntenSoul = soul where is_uId = id ;   

#更新拍卖行中装备的相关信息（可进阶上限、可强化上限、进阶魂根、强化魂根）
insert into tbl_market_equip_info select mos_uId, iw_uIntensifyQuality, iea_uTotalAdvancedTimes, iw_sIntenSoul, iea_sAdvancedSoulRoot from temp_sell_order o, tbl_item_equip_advance a, tbl_item_weapon w 
where o.is_uId = a.is_uId and o.is_uId = w.is_uId and o.mso_sItemType = 5;

update tbl_market_equip_info, temp_soul set mei_sIntenSoul = godwarAttack where mei_sIntenSoul  = "战神";
update tbl_market_equip_info, temp_soul set mei_sIntenSoul = magicAttack where mei_sIntenSoul  = "法魔";
##############防具

insert into tbl_market_equip_info select mos_uId, ia_uIntensifyQuality, iea_uTotalAdvancedTimes, ia_sIntenSoul, iea_sAdvancedSoulRoot from temp_sell_order o, tbl_item_equip_advance a, tbl_item_armor aa 
where o.is_uId = a.is_uId and o.is_uId = aa.is_uId and o.mso_sItemType = 6 ;

update tbl_market_equip_info, temp_soul set mei_sIntenSoul = godwarDefence where mei_sIntenSoul  = "战神";
update tbl_market_equip_info, temp_soul set mei_sIntenSoul = magicDefence where mei_sIntenSoul  = "法魔";

##############盾牌
insert into tbl_market_equip_info select mos_uId, is_uIntensifyQuality, iea_uTotalAdvancedTimes, is_sIntenSoul, iea_sAdvancedSoulRoot from temp_sell_order o, tbl_item_equip_advance a, tbl_item_shield s 
where o.is_uId = a.is_uId and o.is_uId = s.is_uId and o.mso_sItemType = 7;

update tbl_market_equip_info, temp_soul set mei_sIntenSoul = godwarAttack where mei_sIntenSoul  = "战神";
update tbl_market_equip_info, temp_soul set mei_sIntenSoul = magicAttack where mei_sIntenSoul  = "法魔";

##############戒指
insert into tbl_market_equip_info select mos_uId, ir_uIntensifyQuality, iea_uTotalAdvancedTimes, ir_sIntenSoul, iea_sAdvancedSoulRoot from temp_sell_order o, tbl_item_equip_advance a, tbl_item_ring r where o.is_uId = a.is_uId and o.is_uId = r.is_uId and o.mso_sItemType = 8;

update tbl_market_equip_info, temp_soul set mei_sIntenSoul = godwarAttack where mei_sIntenSoul  = "战神";
update tbl_market_equip_info, temp_soul set mei_sIntenSoul = magicAttack where mei_sIntenSoul  = "法魔";

##############饰品
insert into tbl_market_equip_info select mos_uId, ia_uIntensifyQuality, iea_uTotalAdvancedTimes, ia_sIntenSoul, iea_sAdvancedSoulRoot from temp_sell_order o, tbl_item_equip_advance a, tbl_item_armor aa where o.is_uId = a.is_uId and o.is_uId = aa.is_uId and o.mso_sItemType = 9;

update tbl_market_equip_info, temp_soul set mei_sIntenSoul = godwarDefence where mei_sIntenSoul  = "战神";
update tbl_market_equip_info, temp_soul set mei_sIntenSoul = magicDefence where mei_sIntenSoul  = "法魔";

##################更新拍卖行中装备进阶魂跟
update tbl_market_equip_info, temp_advance_soul set mei_sAdvanceSoul = showSoul where mei_sAdvanceSoul  = soul;
