create table tbl_tong_issue_record
(
	t_uId		 			bigint 	unsigned      not null,		 #tong id
	tir_uNum	tinyint    not null,			
	primary	key(t_uId),
	foreign	key(t_uId)	 references tbl_tong(t_uId) 		on	update cascade on	delete cascade
)engine=innodb;