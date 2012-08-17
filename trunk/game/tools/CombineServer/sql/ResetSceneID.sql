set autocommit=0;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='轻歌林地';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '轻歌林地' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '轻歌林地' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '轻歌林地' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='海帕斯高原';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '海帕斯高原' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '海帕斯高原' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '海帕斯高原' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='艾米帝国树屋酒吧';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '艾米帝国树屋酒吧' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '艾米帝国树屋酒吧' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '艾米帝国树屋酒吧' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='神圣教廷树屋酒吧';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '神圣教廷树屋酒吧' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '神圣教廷树屋酒吧' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '神圣教廷树屋酒吧' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='修斯帝国树屋酒吧';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '修斯帝国树屋酒吧' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '修斯帝国树屋酒吧' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '修斯帝国树屋酒吧' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='尼瑞克平原';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '尼瑞克平原' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '尼瑞克平原' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '尼瑞克平原' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='雁南郡';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '雁南郡' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '雁南郡' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '雁南郡' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='界林山区';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '界林山区' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '界林山区' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '界林山区' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='前线山区';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '前线山区' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '前线山区' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '前线山区' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='史坎布雷';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '史坎布雷' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '史坎布雷' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '史坎布雷' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='三乡小镇';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '三乡小镇' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '三乡小镇' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '三乡小镇' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='银杏村';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '银杏村' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '银杏村' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '银杏村' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='通云关';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '通云关' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '通云关' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '通云关' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='双子镇';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '双子镇' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '双子镇' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '双子镇' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='天梁都';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '天梁都' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '天梁都' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '天梁都' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='乌鲁城';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '乌鲁城' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '乌鲁城' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '乌鲁城' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='河阳大草原';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '河阳大草原' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '河阳大草原' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '河阳大草原' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='狮子河谷地';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '狮子河谷地' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '狮子河谷地' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '狮子河谷地' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='圣雪山';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '圣雪山' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '圣雪山' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '圣雪山' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='西林岛';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '西林岛' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '西林岛' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '西林岛' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='测试1';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '测试1' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '测试1' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '测试1' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='测试地图';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '测试地图' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '测试地图' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '测试地图' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='刷门';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '刷门' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '刷门' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '刷门' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='狩猎入口';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '狩猎入口' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '狩猎入口' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '狩猎入口' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='地表测试地图';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '地表测试地图' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '地表测试地图' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '地表测试地图' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='艾米帝国职业圣殿';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '艾米帝国职业圣殿' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '艾米帝国职业圣殿' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '艾米帝国职业圣殿' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='神圣教廷职业圣殿';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '神圣教廷职业圣殿' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '神圣教廷职业圣殿' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '神圣教廷职业圣殿' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='修斯帝国职业圣殿';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '修斯帝国职业圣殿' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '修斯帝国职业圣殿' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '修斯帝国职业圣殿' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='测试用帮会挑战地图';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '测试用帮会挑战地图' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '测试用帮会挑战地图' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '测试用帮会挑战地图' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='前线运输';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '前线运输' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '前线运输' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '前线运输' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='测试用新花语平原';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '测试用新花语平原' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '测试用新花语平原' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '测试用新花语平原' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='新轻歌林地';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '新轻歌林地' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '新轻歌林地' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '新轻歌林地' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='花语平原1';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '花语平原1' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '花语平原1' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '花语平原1' and sc_uId != @minscid;

select @minscid:=case when min(sc_uId) is null then 0 else min(sc_uId) end from tbl_scene where sc_sSceneName='新雁南郡';
update tbl_scene as sc,tbl_char_position as cp
set cp.sc_uId = @minscid
where sc.sc_sSceneName = '新雁南郡' and sc.sc_uId = cp.sc_uId;
update tbl_scene as sc,tbl_war_zone as wz
set wz.sc_uId = @minscid
where sc.sc_sSceneName = '新雁南郡' and sc.sc_uId = wz.sc_uId;
delete from tbl_scene where sc_sSceneName = '新雁南郡' and sc_uId != @minscid;


#所有的主场景的ID都用1区的ID
update tbl_scene as s,ybtx.tbl_scene as tmp 
set s.sc_uId=tmp.sc_uId 
where s.sc_sSceneName=tmp.sc_sSceneName 
and (s.sc_sSceneName != '轻歌林地' 
or s.sc_sSceneName != '海帕斯高原' 
or s.sc_sSceneName != '艾米帝国树屋酒吧' 
or s.sc_sSceneName != '神圣教廷树屋酒吧' 
or s.sc_sSceneName != '修斯帝国树屋酒吧' 
or s.sc_sSceneName != '尼瑞克平原' 
or s.sc_sSceneName != '雁南郡' 
or s.sc_sSceneName != '界林山区' 
or s.sc_sSceneName != '前线山区' 
or s.sc_sSceneName != '史坎布雷' 
or s.sc_sSceneName != '三乡小镇'
or s.sc_sSceneName != '银杏村'
or s.sc_sSceneName != '通云关'
or s.sc_sSceneName != '双子镇'
or s.sc_sSceneName != '天梁都'
or s.sc_sSceneName != '乌鲁城'
or s.sc_sSceneName != '河阳大草原'
or s.sc_sSceneName != '狮子河谷地'
or s.sc_sSceneName != '圣雪山'
or s.sc_sSceneName != '西林岛'
or s.sc_sSceneName != '测试1'
or s.sc_sSceneName != '测试地图'
or s.sc_sSceneName != '刷门'
or s.sc_sSceneName != '狩猎入口'
or s.sc_sSceneName != '地表测试地图'
or s.sc_sSceneName != '艾米帝国职业圣殿'
or s.sc_sSceneName != '神圣教廷职业圣殿'
or s.sc_sSceneName != '修斯帝国职业圣殿'
or s.sc_sSceneName != '测试用帮会挑战地图'
or s.sc_sSceneName != '前线运输'
or s.sc_sSceneName != '测试用新花语平原'
or s.sc_sSceneName != '新轻歌林地'
or s.sc_sSceneName != '花语平原1'
or s.sc_sSceneName != '新雁南郡');

commit;
