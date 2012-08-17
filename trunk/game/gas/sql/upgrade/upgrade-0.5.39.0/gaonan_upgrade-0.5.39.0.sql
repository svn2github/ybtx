create table tbl_action_xianxie		#鲜血试炼场得分记录
(
	cs_uId	bigint unsigned		not	null,		#角色id
	ax_uWin tinyint not null default 0,		#胜利
	ax_uScore tinyint not null default 0,		#队伍分
	ax_dtTime datetime not null default 0,
	foreign	key(cs_uId)	references tbl_char_static(cs_uId) on	update cascade on delete cascade
)engine = innodb;