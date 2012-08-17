alter table tbl_tong_line_time drop primary key;
alter table tbl_tong_line_time add primary key(t_uId);
alter table tbl_tong_line_time add foreign key(t_uId) references tbl_tong(t_uId) on	update cascade on	delete cascade;
alter table tbl_tong_line_time add column tlt_uWsId tinyint unsigned not null after tlt_uLineId;
alter table tbl_tong_line_time modify column tlt_uLineId  tinyint unsigned not null;
alter table tbl_tong_line_time modify column tlt_uTime  smallint unsigned not null default 0;


alter table tbl_tong_station_time drop primary key;
alter table tbl_tong_station_time add primary key(t_uId);
alter table tbl_tong_station_time add foreign key(t_uId) references tbl_tong(t_uId) on	update cascade on	delete cascade;
alter table tbl_tong_station_time add column tst_uWsId tinyint unsigned not null after tst_uStationId;
alter table tbl_tong_station_time modify column tst_uStationId  smallint unsigned not null;
alter table tbl_tong_station_time modify column tst_uTime  smallint unsigned not null default 0;