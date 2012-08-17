set autocommit=0;
#洗天赋功能
delete from tbl_shortcut where sc_Arg1 in (select distinct fs_sName from tbl_fight_skill where fs_uKind = 2 and tbl_shortcut.cs_uid = tbl_fight_skill.cs_uId);
delete from tbl_skill_Series;
delete from tbl_fight_skill where fs_uKind = 2;
delete from tbl_skill_node;
delete from tbl_skill_layer;
delete from tbl_char_servant;
delete from tbl_char_servant_name;
delete from tbl_char_magic_obj;
delete from tbl_char_magicstate;
delete from tbl_aure_magic;

#清除技能脏数据
delete from tbl_fight_skill where fs_sName in ("冰爆","恶魔牺牲","魔息侵扰");


insert into tbl_database_upgrade_record values("changyang_1_upgrade_0.4.36.0_data.sql");
commit;
