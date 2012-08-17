alter table tbl_user_logout add column us_uGameID	int	unsigned not null default 8 after us_sName;
alter table tbl_user_logout drop foreign key tbl_user_logout_ibfk_1;
alter table tbl_user_logout drop primary key;
alter table tbl_user_logout add unique key(us_sName,us_uGameID);

alter table tbl_user_static drop key `us_sName`;
alter table tbl_user_static add column us_uGameID	int	unsigned	not	null default 8 after us_sName;
alter table tbl_user_static add unique key(us_sName,us_uGameID);
alter table tbl_user_static drop key `us_sName_2`;
alter table tbl_user_static add key(us_sName,us_uGameID,us_sPassword);

alter table tbl_user_wait_queue add column us_uGameID	int	unsigned not null default 8 after us_sName;
alter table tbl_user_wait_queue drop key `us_sName`;
alter table tbl_user_wait_queue add unique key (us_sName,us_uGameID);

alter table tbl_erating_audit add column us_uGameID int unsigned not null default 8 after ea_ID;
alter table tbl_erating_audit_log add column us_uGameID int unsigned not null default 8 after ea_ID;

alter table tbl_freeze_account drop primary key;
alter table tbl_freeze_account add column us_uGameID	int	unsigned not null default 8 after username;
alter table tbl_freeze_account add unique key(username,us_uGameID);

alter table tbl_gm_validate drop primary key;
alter table tbl_gm_validate add column us_uGameID	int	unsigned not null default 8 after username;
alter table tbl_gm_validate add unique key(username,us_uGameID);