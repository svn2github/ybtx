#!/usr/bin/php
<!DOCTYPE YBTX-BuyCoupons>
<?php
require_once("../../conf/gm_tools/Config.php");
global $tbl_gm_db;
$dbinfo = $tbl_gm_db[0];
$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
mysql_select_db($dbinfo[3],$Conn);

mysql_query("LOCK TABLES `tbl_joint_sales_sn` WRITE");
$rs = mysql_query("SELECT `jss_sSN` FROM `tbl_joint_sales_sn` where `jsi_uId` = ".$_REQUEST["id"]." LIMIT 1");
if(!$rs){
	echo "<root>\n<error code=\"-100\" msg=\"".mysql_error()."\" />\n</root>";
	return;
}
if($row = mysql_fetch_row($rs)){
	$sn = $row[0];
	$r = mysql_query("DELETE FROM `tbl_joint_sales_sn` WHERE `jsi_uId` = ".$_REQUEST["id"]." and `jss_sSN` = '".$sn."'");
	if(!$r){
		echo "<root>\n<error code=\"-100\" msg=\"".mysql_error()."\" />\n</root>";
		return;
	}
	if(mysql_affected_rows()==0){
		echo "<root>\n<error code=\"-1\" msg=\"ID: ".$_REQUEST["id"].", SN: ".$sn."\" />\n</root>";
		return;
	}
	echo "<root>\n<code>".str_replace("\r","",str_replace("\n","",$sn))."</code>\n</root>";
}else{
	echo "<root>\n<error code=\"-1\" msg=\"ID: ".$_REQUEST["id"]."\"/>\n</root>";
	return;
}
mysql_query("UNLOCK TABLE");

mysql_query("insert into `tbl_joint_sales_log`(`jsi_uId`,`jss_sSN`,`jsl_uServerId`,`jsl_uUser`,`jsl_uChar`,`jsl_dtDateTime`) values(".$_REQUEST["id"].",'".$sn."',".$_REQUEST["server"].",'".$_REQUEST["user"]."','".$_REQUEST["role"]."',now())");
?>