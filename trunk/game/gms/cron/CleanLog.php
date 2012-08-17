#!/usr/bin/php
<?php
require_once("/home/webfold/conf/gm_tools/Config.php");

function MainFun($day_step){
	echo "======Log Database Clear Begin======\n";
	echo "Date: ".date("Y-m-d h:i:s")."\n\n";

	global $tbl_log_db,$tbl_log_tables;
	$dbinfo=$tbl_log_db[0];
	$Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
	mysql_select_db($dbinfo[3], $Conn);
	$qs = "select le_uServerId, min(le_uId), min(le_dtDateTime) from tbl_log_event where le_dtDateTime > date(date_sub(now(), interval $day_step day)) and le_dtDateTime <= date(date_sub(now(), interval $day_step day)) group by le_uServerId";
	$rs = mysql_query($qs);
	if(!$rs) echo mysql_error();
	$where = array();
	echo "This will remove all logs created before follow lines each servers.\n";
	while($row = mysql_fetch_row($rs)){
		echo "Server ID: ".$row[0]."\tDate: ".$row[2]."\n";
		array_push($where,"(le_userverId = ".$row[0]." and le_uId < ".$row[1].")");
	}
	echo "\n";
	if(count($where)==0) return;
	$where = implode(" or ", $where);
	$file = fopen("CleanLog.txt","r");
	while(! feof($file)){
		$table = fgets($file);
		if($table=="") continue;
		$qs = "delete from $table where $where";
		$rs = mysql_query($qs);
		if(!$rs){
			echo "Faile! Table '$table'";
			echo "==>".mysql_error()."\n";
		}else{
			printf("Success! Remove %d rows\t from table %s",mysql_affected_rows(),$table);
		}
	}
}

MainFun($argv[1]);
?>
