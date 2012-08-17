

delete from tbl_conf_server where sc_sVarName = "MaxOnLineUserNum";
insert into tbl_conf_server 
(sc_sVarName, sc_sVarValue)
values
("AGIP_AllServerMaxUserNum","3000"),
("AGIP_SingleServerMaxUserNum","1000");