alter table tbl_char_pk drop foreign key tbl_char_pk_ibfk_1;
alter table tbl_char_pk drop primary key;
alter table tbl_char_pk add primary key(cs_uId,cp_uMin,cp_uMax);
alter table tbl_char_pk add foreign	key(cs_uId)	references tbl_char_static(cs_uId) on update cascade on delete cascade;