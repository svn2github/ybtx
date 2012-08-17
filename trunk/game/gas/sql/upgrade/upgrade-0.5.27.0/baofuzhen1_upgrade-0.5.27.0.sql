
alter table tbl_tong add column t_uDevelopDegree bigint		unsigned	not null default 0 after t_uInitiatorId;
alter table tbl_tong add column t_uPermitUpLevel tinyint	unsigned	not null default 1 after t_uDevelopDegree;