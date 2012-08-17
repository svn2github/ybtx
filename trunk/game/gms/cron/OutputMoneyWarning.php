#!/usr/bin/php
<?php
require_once("/home/webfold/conf/gm_tools/Config.php");

function MainFun($server_id,$time_step,$max_money,$max_times){
	global $tbl_log_db,$tbl_gm_db;
	
	$dbinfo = $tbl_log_db[0];
	$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
	mysql_select_db($dbinfo[3],$Conn);
	
	$qs = "select max(le_uId) from tbl_log_money where le_uServerId = $server_id";
	$result = mysql_query($qs);
	if(!$result) echo mysql_error()."\n";
	if($row = mysql_fetch_row($result)){
		$max_le = $row[0];
	}else
		return false;

	$qs = "select le_dtDateTime from tbl_log_event where le_uId = $max_le and le_uServerId = $server_id";
	$result = mysql_query($qs);
        if(!$result) echo mysql_error()."\n";
        if($row = mysql_fetch_row($result)){
                $max_dt = $row[0];
        }else
                return false;

	$qs = "select lm.lcs_uId, sum(lm.lm_uMoney) s, count(*) c from tbl_log_money lm, tbl_log_event le, tbl_log_event_type let where lm.le_uId = le.le_uId and lm.le_uServerId = le.le_uServerId and let.le_uId = le.le_uId and let.le_uServerId = le.le_uServerId and lm.lm_uMoney > 0 and let.let_uEventType in (2,78,42,84,159,126) and le.le_uServerId = $server_id and le.le_dtDateTime > DATE_SUB('$max_dt', INTERVAL $time_step MINUTE) group by lm.lcs_uId having s > $max_money and c > $max_times";
	$result = mysql_query($qs);
        if(!$result) echo mysql_error()."\n";

	$dbinfo = $tbl_gm_db[0];
	$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
        mysql_select_db($dbinfo[3],$Conn);

	while($row = mysql_fetch_row($result)){
		$ret = mysql_query("insert into tbl_output_money_warning values('$max_dt','$server_id','$row[0]',null,'$row[1]','$row[2]')");
		if(!$ret) echo mysql_error();
		printf("报警时间：%s\t角色ID：%s\t产出金额：%d\t产出次数：%d\n",$max_dt,$row[0],$row[1],$row[2]);
	}
	mysql_close($Conn);
	return true;
}

if(!MainFun($argv[1],$argv[2],$argv[3],$argv[4]))
	echo "ERROR";
?>
