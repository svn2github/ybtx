##清除精灵技能错误数据
delete from tbl_shortcut where sc_Arg1 in ("烈焰领域","灼手兵器","火魔附体","火龙弹","火之侵袭","风之牵引","风之瞬移","风浪","阵风洞","灵动之风","虚拟黑洞","石化","尖石地域","落石术","钢铁皮肤","水之恢复","海之盾","神圣之泉","水流术","天降甘露","冰环","冰之雕像","冰箭术","冰之诅咒","冰川");
update  tbl_item_equip_advance set iea_uChoosedSkillPieceIndex = 0;
update  tbl_char_equip_advancedActiveSkill set iea_uActiveSkillIndex = 0;
