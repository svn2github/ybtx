##清理军资运输时间数据库表
delete from tbl_tong_draw_forage_time;

#删除"恶魔之力"天赋
delete from tbl_fight_skill where fs_sName = "恶魔之力";
