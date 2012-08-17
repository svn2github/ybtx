#!/usr/bin/php
<?php
require_once("../../conf/cis/Config.php");
require_once("../../lib/cis/Lib.php");
function CheckName($var)
{
	global $tbl_db_info;
	
	$Conn = mysql_connect($tbl_db_info[0], $tbl_db_info[1], $tbl_db_info[2]);
	mysql_select_db($tbl_db_info[3], $Conn);
	
	$ret = "";
	$uid   = intval($var["userid"]);
	$serv_id = intval($var["serv_id"]);
	$rolename = mysql_escape_string($var["rolename"]);
	$qs = "select distinct us_uId from tbl_char c, tbl_char_static cs where cs.cs_uId = c.cs_uId and (c_sName = '$rolename' or c_sNameFormer = '$rolename')";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		if ($row[0] != $uid){
			$ret = "400";
			return;
		}
	}

	$ret = "200";
	header("Content-type: text/html; charset=utf-8");
	header("Content-Length:".strlen($ret));
	print $ret;
}

CheckName($_GET);
?>

