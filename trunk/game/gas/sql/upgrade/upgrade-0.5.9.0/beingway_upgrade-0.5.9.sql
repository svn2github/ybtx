#以id为索引的channel取消指定hash_index
drop table tbl_channel_id;
create table tbl_channel_id
(
	ci_nId bigint not null,
	primary key(ci_nId)
)engine=innodb;