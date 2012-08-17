#!/usr/bin/php
<!DOCTYPE YBTX-BuyCoupons>
<?php
require_once("../../conf/gm_tools/Config.php");
global $tbl_gm_db;
$dbinfo = $tbl_gm_db[0];
$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
mysql_select_db($dbinfo[3],$Conn);

$rs = mysql_query("DELETE FROM `tbl_joint_sales_log` WHERE `jsi_uId` = ".$_REQUEST["id"]." AND `jss_sSN` = '".$_REQUEST["code"]."'");
if(!$rs){
	echo "<root>\n<error code=\"-100\" msg=\"".mysql_error()."\" />\n</root>";
	return;
}

if(mysql_affected_rows()==0){
	echo "<root>\n<error code=\"-1\" msg=\"".$_REQUEST["id"].",".$_REQUEST["code"]."\" />\n</root>";
	return;
}else{
	$rs = mysql_query("insert into `tbl_joint_sales_sn`(`jsi_uId`,`jss_sSN`) values(".$_REQUEST["id"].",'".$_REQUEST["code"]."')");
	if(!$rs){
		echo "<root>\n<error code=\"-100\" msg=\"".mysql_error()."\" />\n</root>";
		return;
	}
	if(mysql_affected_rows()==0){
		echo "<root>\n<error code=\"-2\" />\n</root>";
		return;
	}
}
?>
<root><succese /></root>