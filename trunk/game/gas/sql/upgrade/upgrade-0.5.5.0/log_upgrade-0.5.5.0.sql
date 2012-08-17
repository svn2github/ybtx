create table tbl_log_char_login
(
 le_uId					bigint unsigned			not null,
 le_uServerId int unsigned 			not null,

 key(le_uId,le_uServerId)
)engine=innodb;

insert into tbl_database_upgrade_record values("log_upgrade-0.5.5.0.sql");
