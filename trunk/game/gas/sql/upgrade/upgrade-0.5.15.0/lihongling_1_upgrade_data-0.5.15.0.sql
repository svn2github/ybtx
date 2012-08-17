##把科技等级大于3的改成3
update tbl_tong_tech_static set tts_uLevel = 3 where tts_uLevel > 3;