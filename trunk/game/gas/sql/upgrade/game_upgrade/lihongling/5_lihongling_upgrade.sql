alter table tbl_user_everyday_soul_and_exp drop foreign key tbl_user_everyday_soul_and_exp_ibfk_1;
alter table tbl_user_everyday_soul_and_exp add unique key(us_uId);
alter table tbl_user_everyday_soul_and_exp add foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade; 
