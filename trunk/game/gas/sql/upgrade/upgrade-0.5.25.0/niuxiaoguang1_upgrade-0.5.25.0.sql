alter table `tbl_gms_allowed_ip` rename `tbl_web_server_conf`;
alter table `tbl_web_server_conf` add column `wsc_sName` varchar(32) not null;
alter table `tbl_web_server_conf` change `gai_IP` `wsc_sValue` varchar(128) not null after `wsc_sName`;
