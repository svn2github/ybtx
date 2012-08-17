insert into tbl_gms_allowed_ip select sc_sVarValue from tbl_conf_server where sc_sVarName = 'gmsallowedip';
delete from tbl_conf_server where sc_sVarName = 'gmsallowedip';