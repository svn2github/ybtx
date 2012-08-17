<?php
function MainFunc($db_name){
	$db_host = '10.10.43.142';
	$db_user = 'root';
	$db_pass = '1';
	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);
	$qs = "select count(distinct client_guid) from tbl_client_log_common";
	$result = mysql_query($qs);
	$row = mysql_fetch_row($result);
	$cc = $row[0];
	$power1 = intval($cc * 0.04);
	
	$qs = "create view view_client_error_type as select error_code, client_guid, count(*) as num from tbl_client_log_common group by error_code, client_guid";
	mysql_query($qs);
	
	$qs = "create view view_client_error_num as select count(distinct error_code) as ct, client_guid from view_client_error_type group by client_guid";
	mysql_query($qs);

	$qs = "select sum(ct) as ct, count(distinct client_guid) as cc from view_client_error_num";
	$result = mysql_query($qs);
	$row = mysql_fetch_row($result);
	$ct = $row[0];
	$cc = $row[1];

	$power2 = intval($ct/$cc*2);

	$qs = "create view view_error_client as select error_code, count(client_guid) as count_client, sum(num) as s from view_client_error_type group by error_code";
	mysql_query($qs);	
	
	$qs = "create view view_special_error as select error_code from view_error_client where count_client < '$power1'";
	mysql_query($qs);

	$qs = "create view view_guid_info as select distinct client_guid from view_client_error_type v1, view_special_error v2 where v1.error_code = v2.error_code";
	mysql_query($qs);
	
	$qs = "create view view_special_guid as select v1.client_guid, count(distinct error_code) as count_error from view_guid_info v1, view_client_error_type v2 where v1.client_guid = v2.client_guid group by v2.client_guid";
	mysql_query($qs);
	
	$qs = "update tbl_guid_info, view_special_guid set tbl_guid_info.is_special_client = 1 where tbl_guid_info.guid = view_special_guid.client_guid and count_error > '$power2'";
	mysql_query($qs);

	$qs = "drop view view_client_error_type, view_error_client, view_special_error, view_special_guid, view_guid_info, view_client_error_num";
	mysql_query($qs);
}

MainFunc("LogFree");
?>
