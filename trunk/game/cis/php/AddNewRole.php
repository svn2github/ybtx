#!/usr/bin/php
<?php
require_once("../../conf/cis/Config.php");
require_once("../../lib/cis/Lib.php");
function AddNewRole($var)
{
	global $tbl_db_info;
	
	$Conn = mysql_connect($tbl_db_info[0], $tbl_db_info[1], $tbl_db_info[2]);
	mysql_select_db($tbl_db_info[3], $Conn);
	
	$ret = "";
	$rid   = intval($var["roleid"]);
	$uid   = intval($var["userid"]);
	$sex   = intval($var["sex"]);
	$camp  = intval($var["camp"]);
	$class = intval($var["class"]);
	$serv_id = intval($var["serv_id"]);
	$rolename = mysql_escape_string($var["rolename"]);
	$qs = "select distinct us_uId from tbl_char c, tbl_char_static cs where cs.cs_uId = c.cs_uId and c_sName = '$rolename' or c_sNameFormer = '$rolename'";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		if ($row[0] != $uid){
			$ret = "400";	
			return;
		}
	}

	$qs = "insert into tbl_char_static set cs_uId = '$rid',
us_uId='$uid', cs_uSex='$sex', cs_uClass='$class', cs_uCamp='$camp', cs_dtCreateDate=now(), cs_uServerId='$serv_id'";
	mysql_query($qs);

	$qs = "insert into tbl_char set cs_uId = '$rid', c_sName='$rolename', c_sNameFormer=''";
	mysql_query($qs);

	LogAction("AddNewRole($rid, $username, $uid, $sex, $class, $camp)", $serv_id);
	$ret = "200";
	header("Content-type: text/html; charset=utf-8");
	header("Content-Length:".strlen($ret));
	print $ret;
}

AddNewRole($_GET);
?>
