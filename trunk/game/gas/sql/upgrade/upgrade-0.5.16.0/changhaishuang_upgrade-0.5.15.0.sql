
alter table tbl_char_status drop column cs_uPKValue;
alter table tbl_char_status drop column cs_uCriminalTime;

alter table tbl_duel_count add primary key(cs_uId,dc_uChallengerId);