

alter table tbl_log_player_post drop column lpp_uFromCharId;
alter table tbl_log_player_post drop column lpp_uToCharId;
alter table tbl_log_player_post drop column lis_uId;


#提升装备可强化上限
create table tbl_log_equip_intensify_level_up
(
	le_uId bigint unsigned not null,
	le_uServerId int unsigned not null,
	
	key(le_uId,le_uServerId),
	foreign key (le_uId,le_uServerId) references tbl_log_event(le_uId, le_uServerId ) on update cascade on delete cascade
)engine = innodb;


#离线经验兑换
create table tbl_log_exp_change
(
	le_uId bigint unsigned not null,
	le_uServerId int unsigned not null,
	
	key(le_uId,le_uServerId)
)engine = innodb;

insert into tbl_database_upgrade_record values ("log_upgrade-0.4.37.0.sql");
