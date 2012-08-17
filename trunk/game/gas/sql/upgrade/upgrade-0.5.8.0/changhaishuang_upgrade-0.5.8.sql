set autocommit=0;
delete from tbl_conf_server where sc_sVarName = "AGIP_AllServerMaxUserNum";
delete from tbl_conf_server where sc_sVarName = "AGIP_SingleServerMaxUserNum";
insert into tbl_conf_server 
(sc_sVarName, sc_sVarValue)
values
("AllServerMaxUserNum","3000"),
("SingleServerMaxUserNum","1000");

commit;

insert into tbl_database_upgrade_record values("changhaishuang_upgrade-0.5.8.sql");
