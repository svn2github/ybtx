delete from tbl_item_static where is_sName = "大型运输车";
delete from tbl_item_static where is_sName = "小型运输车";
delete from tbl_item_static where is_sName = "捕获运输车";
delete from tbl_item_static where is_sName = "艾米佣兵团刷怪本报名人";
delete from tbl_item_static where is_sName = "神圣佣兵团刷怪本报名人";
delete from tbl_item_static where is_sName = "修斯佣兵团刷怪本报名人";

delete from tbl_item_static where is_sName = "佣兵探险装备包裹（战系）";

insert into tbl_tong_proffer select cs_uId,afp_uPoint from tbl_area_fb_point where afp_uPointType = 5;
update tbl_area_fb_point set afp_uPoint = 0 where afp_uPointType = 5;

#洗天赋功能
delete from tbl_shortcut where sc_Arg1 in (select distinct fs_sName from tbl_fight_skill where fs_uKind = 2 and tbl_shortcut.cs_uid = tbl_fight_skill.cs_uId);
delete from tbl_skill_Series;
delete from tbl_fight_skill where fs_uKind = 2;
delete from tbl_skill_node;
delete from tbl_skill_layer;
delete from tbl_char_servant_outline;
delete from tbl_char_servant_name;
delete from tbl_char_magic_obj;
delete from tbl_char_magicstate;
delete from tbl_aure_magic;

#清除技能脏数据
delete from tbl_fight_skill where fs_sName in ("暴气状态","瞬冰弹","瞬火丸","血腥打击","火焰之径","蓄力箭");
update tbl_fight_skill set fs_uKind = 5 where fs_sName in ("重砍","法杖术","火鸦术","勇击");
delete from tbl_shortcut where sc_Arg1 in("重砍","法杖术","火鸦术","勇击","血腥打击","火焰之径","蓄力箭","瞬冰弹","瞬火丸","暴气状态");

insert ignore into tbl_area_fb_point select cs_uId,7,cs_uPKValue from tbl_char_status;

update tbl_conf_server set sc_sVarValue = '32' where sc_sVarName = 'AGIP_Version';
