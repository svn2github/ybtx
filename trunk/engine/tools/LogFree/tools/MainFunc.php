<?php
require_once("lib.php");
require_once("AnyliseClientComm.php");
require_once("AnyliseInfo.php");
require_once("AnyliseServerComm.php");

/*
*/
function MainFunc($path, $db_host, $db_user, $db_pass, $db_name)
{
	if (!$path || $path == "")
	{
		echo "error";
		exit(0);
	}
	

	$db_host = "10.10.43.142";
	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");

	$network_flag = 0;
	if ($db_name == "LogFree") 
		$network_flag = 1;
	if ($db_name == "LogFree_WAN") 
		$network_flag = 2;

	mysql_select_db("LogFree", $Conn);

	$qs = "delete from tbl_last_action_time";
	mysql_query($qs);
	$qs = "insert into tbl_last_action_time set lat_dtDateTime = now()";
	mysql_query($qs);
	process_all_info("$path/gac");
	process_all_info("$path/gas");

	process_all_server_common("$path/gas", $network_flag);
	process_all_client_guid("$path/gas");
	process_all_client_common("$path/gac", $network_flag);

	$qs = "update tbl_client_log_common set happened_time = now() where happened_time > now()";
	mysql_query($qs);
	mysql_close();
}

if (count($argv) < 6){
	$usage=<<<_usage_str
    usage: php MainFunc.php path_name db_host db_user db_pass db_name

    需要path_name的目录结构
	path_name
	|-----gas
	|------|----gas******.comm.log
	|------|----gas******.info.log
	|-----gac
	|------|----gac******.comm.log
	|------|----gac******.info.log

\n
_usage_str;
	echo $usage;

}
else{
	$path = $argv[1];
	$db_host = $argv[2];
	$db_user = $argv[3];
	$db_pass = $argv[4];
	$db_name = $argv[5];
	MainFunc($path, $db_host, $db_user, $db_pass, $db_name);
}

?>
