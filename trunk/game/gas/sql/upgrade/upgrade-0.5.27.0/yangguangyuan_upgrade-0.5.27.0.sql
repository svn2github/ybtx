create table tbl_client_msg
(
	cs_uId					bigint	unsigned	not	null,
	cm_sTypeName		varchar(32)				not	null,
	cm_uId					int unsigned	not	null default 0,
	primary key(cs_uId, cm_sTypeName)
)engine=innodb;
