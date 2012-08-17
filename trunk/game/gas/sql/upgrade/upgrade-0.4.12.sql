create table tbl_hot_update_result(
	hu_uId		bigint unsigned		not null,
	hu_serIid	bigint unsigned		not null,
	hu_state	tinyint unsigned	not null,
	hu_sMsg		text
)engine = InnoDb;