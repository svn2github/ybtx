#记录角色速度变化log
create table tbl_log_char_speed
(
	lcs_sName	varchar(96)	not	null,
	lcs_uSpeed float not null,
	lcs_dtAppearTime datetime not null,
	lcs_sChangeReason varchar(96) not null,
	
	key(lcs_sName),
	key(lcs_uSpeed)
)engine=innodb;