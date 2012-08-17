alter table tbl_char_jifensaipoint modify cs_uWinNum int unsigned not null default 0,
 modify cs_uLoseNum int unsigned not null default 0,
 modify cs_uRunNum int unsigned not null default 0,
 modify cs_uPoint int unsigned not null default 20000,
 modify cj_uInFbTimes int unsigned not null default 0,
 modify cj_uKillNum int unsigned not null default 0,
 modify cj_uDeadNum int unsigned not null default 0;


alter table tbl_char_jifensaiaccum modify cs_uUpWeekWinNum int unsigned not null default 0,
 modify cs_uUpWeekLoseNum int unsigned not null default 0,
 modify cs_uUpWeekRunNum int unsigned not null default 0,
 modify cs_uUpWeekPoint int unsigned not null default 0,
 modify cs_uCurrWeekWinNum int unsigned not null default 0,
 modify cs_uCurrWeekLoseNum int unsigned not null default 0,
 modify cs_uCurrWeekRunNum int unsigned not null default 0,
 modify cs_uCurrWeekPoint int unsigned not null default 0;
