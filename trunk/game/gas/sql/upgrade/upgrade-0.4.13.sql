

alter table tbl_mail add column m_uLogMailType smallint unsigned 	not null;

##分解熟练度
create table tbl_item_break_exp	#item left time
(
	cs_uId		bigint 		unsigned	not	null,	#char identifier
	ibp_uExp  bigint 		unsigned	not	null,	#break exp
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine=innodb;


##*粮草领取*##
create table tbl_tong_draw_forage
(
	tdf_uId   bigint unsigned     	not null,										#char id
	tdf_uTotal    bigint unsigned    	not null  default 50,			#forage total num
	primary	key(tdf_uId),
  foreign	key(tdf_uId) references tbl_char_static(cs_uId) on update cascade on delete cascade
)engine=innodb;
#洗天赋功能
delete from tbl_shortcut where sc_Arg1 in (select distinct fs_sName from tbl_fight_skill where fs_uKind = 2 and tbl_shortcut.cs_uid = tbl_fight_skill.cs_uId);
delete from tbl_skill_Series;
delete from tbl_fight_skill where fs_uKind = 2;
delete from tbl_skill_node;
delete from tbl_skill_layer;
delete from tbl_char_appellation where ca_sAppellation in("见习勇武剑士","勇武剑士","技艺大剑士","英勇大剑士","战魂大剑士","狂鹫大剑士","神勇大剑士","见习护卫剑士","护卫剑士","铁卫大剑士","无畏大剑士","铁血卫士","铁盾卫士","铁剑卫士","见习狂暴剑士","狂暴剑士","杀戮大剑士","残酷大剑士","复仇大剑士","无情大剑士","狙击大剑士");
delete from tbl_char_appellation where ca_sAppellation in("初阶冰霜魔剑士","冰霜魔剑士","冰原魔剑士","元素魔剑士","冰王魔剑士","冰灵魔剑士","冷酷魔剑士","初阶火焰魔剑士","火焰魔剑士","怒焰魔剑士","炎心魔剑士","焚世魔剑士","火灵魔剑士","天火魔剑士","初阶闪电魔剑士","闪电魔剑士","无情魔剑士","剑灵魔剑士","武神魔剑士","神灵魔剑士","雷神魔剑士");
delete from tbl_char_appellation where ca_sAppellation in("见习火法师","火焰法师","炽焰大法师","火灵大法师","红袍魔导师","怒焰魔导师","炎龙魔导师","见习冥想法师","冥想法师","秘文大法师","冥思大法师","法能魔导师","辉煌魔导师","博学魔导师","见习冰法师","冰霜法师","暴风魔导师","冰火魔导师","风暴大魔导师","元素之王","传奇大魔导师");
delete from tbl_char_appellation where ca_sAppellation in("初阶契约邪魔","契约邪魔","混沌邪魔","灵魂邪魔","深渊使者","暗日使者","毁灭使者","初阶黑暗邪魔","黑暗邪魔","夜之邪魔","暗之邪魔","黑暗使者","地狱使者","邪能使者","初阶操控邪魔","操控邪魔","憎恨邪魔","厄运邪魔","毒心者","掌控者","诅咒者");
delete from tbl_char_appellation where ca_sAppellation in("初阶驱邪牧师","驱邪牧师","光明牧师","纯净牧师","惩戒贤者","庇佑贤者","圣典贤者","初阶虔诚牧师","虔诚牧师","守护者","奉献者","天使守护者","天使祷告者","天使奉献者","初阶恢复牧师","恢复牧师","神启牧师","怜悯牧师","法典神官","神谕神官","审判神官");
delete from tbl_char_appellation where ca_sAppellation in("初阶嗜血战士","嗜血战士","血腥酋长","嗜杀酋长","战争主宰","嗜血霸主","暴君","初阶无畏战士","无畏战士","杀戮酋长","无畏酋长","碎颅者","断脊者","碾骨者","初阶疯狂战士","疯狂战士","残酷酋长","凶暴酋长","野蛮主宰","疯狂者","屠戮者");
delete from tbl_char_appellationList where ca_uAppellation >= 90 and  ca_uAppellation <= 232;
delete from tbl_char_servant_outline;
delete from tbl_char_servant_name;
delete from tbl_char_magic_obj;
delete from tbl_char_magicstate;
delete from tbl_aure_magic;
update tbl_war_zone_station set wz_uId = 1 where wz_uId = 3;
update tbl_tong_challenge set tc_nPassiveWzId = 1 where tc_nPassiveWzId = 3;
update tbl_tong_challenge_battle set tcb_nPassiveWzId = 1 where tcb_nPassiveWzId = 3;
delete from tbl_war_zone where wz_uId = 3;
delete from tbl_scene where sc_uType = 6;
