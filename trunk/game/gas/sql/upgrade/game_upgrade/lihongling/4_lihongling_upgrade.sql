create table tbl_user_activation_compenate
(	
	uac_uId bigint unsigned not null auto_increment,
	us_uId int unsigned not null,
	is_uType tinyint unsigned not null,#item type
	is_sName varchar(96) not null,#item name
	uac_ItemCount tinyint unsigned not null,
	primary key(uac_uId),
	foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade
)engine=innodb;

create table tbl_user_everyday_soul_and_exp
(us_uId int unsigned not null,
 esae_uSoul bigint unsigned not null,
 esae_uExp bigint unsigned not null,
 unique key (us_uId),
 foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade
)engine = innodb;

create table tbl_user_activation_code
(us_uId int unsigned not null,
 uac_ActCode varchar(60) not null,
 unique key (us_uId),
 foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade
)engine = innodb;