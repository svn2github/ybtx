#!/usr/bin/php
<?php
require_once("../../conf/cis/Config.php");
require_once("../../lib/cis/Lib.php");
function ChangeRoleName($var)
{
	global $tbl_db_info;
	
	$Conn = mysql_connect($tbl_db_info[0], $tbl_db_info[1], $tbl_db_info[2]);
	mysql_select_db($tbl_db_info[3], $Conn);
	
	$newname = $var["newname"];
	$roleid   = $var["roleid"];
	$serv_id  = $var["serv_id"];
	
	$ret = "";	
	$newname_esc = mysql_escape_string($newname);
	$roleid   = intval($roleid);
	$qs = "update tbl_char set c_sNameFormer = c_sName, c_sName = '$newname' where cs_uId = '$roleid'";
	$result = mysql_query($qs);
	if (!$result) {
		$ret = "400";
		return;
	}
	LogAction("ChangeRoleName($roleid, $newname_esc)", $serv_id);
	$ret = "200";
	header("Content-type: text/html; charset=utf-8");
	header("Content-Length:".strlen($ret));
	print $ret;
}

ChangeRoleName($_GET);
?>
