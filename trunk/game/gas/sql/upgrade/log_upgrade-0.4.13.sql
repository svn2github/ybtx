

#已被删除的角色列表。
create table tbl_log_char_deleted		#deleted roles
(
	le_uId			bigint unsigned not null,
	lcs_uId			bigint	unsigned	not	null,   	#role identifier
	lcd_sName		char(32) collate utf8_unicode_ci not null,	#role name that be used while be avtived
	
	primary	key(lcs_uId),
	key(le_uId),
	key(lcd_sName)
)engine=myisam;

alter table tbl_log_equip_puton add column lep_uEquipPart	tinyint unsigned  not null after le_uId;
