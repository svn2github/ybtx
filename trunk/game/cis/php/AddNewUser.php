#!/usr/bin/php
<?php
require_once("../../conf/cis/Config.php");
require_once("../../lib/cis/Lib.php");
function AddNewUser($var)
{
	global $tbl_db_info;

	$Conn = mysql_connect($tbl_db_info[0], $tbl_db_info[1], $tbl_db_info[2]);
	mysql_select_db($tbl_db_info[3], $Conn);
	
	$ret = "";
	$username = $var["username"];
	$userid   = $var["userid"];
	$vip_type = $var["vip_type"];
	$serv_id  = $var["serv_id"];
	
	$username_esc = mysql_escape_string($username);
	$userid_esc   = intval($userid);
	$vip_type_esc = intval($vip_type);
	$qs = "replace into tbl_user_static set us_uId = '$userid_esc', us_sName='$username_esc', us_uVipType='$vip_type_esc'";
	$result = mysql_query($qs);
	if (!$result) {
		$ret = "400";
		return;
	}

	LogAction("AddNewUser($username_esc, $userid_esc, $vip_type_esc)", $serv_id);
	$ret = "200";
	header("Content-type: text/html; charset=utf-8");
	header("Content-Length:".strlen($ret));
	print $ret;
}

AddNewUser($_GET);
?>
