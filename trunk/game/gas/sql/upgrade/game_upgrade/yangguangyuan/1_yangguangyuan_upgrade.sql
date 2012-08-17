create table tbl_user_trust
(	
	us_uId					int	unsigned	not	null,
	ut_sPassword		varchar(32)		not 	null,			#托管密码
	ut_dtTrustTime	datetime not null,						#托管起始时间
	ut_uTrustLength	int	unsigned not null,				#托管持续时间
	foreign key(us_uId) references tbl_user_static(us_uId) on update cascade on delete cascade
)engine = innodb;
