drop table if exists tbl_hot_update_result;

create table tbl_hot_update_result(
	hur_id int unsigned not null auto_increment,
	hur_type tinyint unsigned not null,
	hur_filename varchar(96) not null,
	hur_code varchar(36) not null,
	hur_result varchar(32) not null,
	primary key(hur_id),
	unique key(hur_type, hur_filename, hur_code)
)engine = InnoDb;

