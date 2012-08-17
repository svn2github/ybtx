#!/usr/bin/php
<?php
require_once('../../conf/gm_tools/Config.php');

function Load($server_id){
	global $tbl_serv_db,$tbl_gm_db;
	echo $server_id."\n";
	$dbinfo = $tbl_serv_db[$server_id];
	$Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
        mysql_select_db($dbinfo[3], $Conn);
	
	$camp = array(0,0,0);
	$qs = "select cs.cs_uCamp, count(*) c from tbl_char c, tbl_char_static cs where c.cs_uId = cs.cs_uId group by cs.cs_uCamp";
	$rs = mysql_query($qs);
        while($row = mysql_fetch_row($rs)){
		$camp[intval($row[0])-1] = $row[1];
	}
	
	$dbinfo = $tbl_gm_db[0];
        $Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
        mysql_select_db($dbinfo[3], $Conn);
	
	$qs = "insert into tbl_rush_role_log values(now(),".$server_id.",".implode(",",$camp).")";
	mysql_query($qs);
}

function MainFunc(){
	echo date("Y-M-d H:i:s")."=====================\n";
	global $tbl_gm_db;
	$dbinfo = $tbl_gm_db[0];
	$Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
	mysql_select_db($dbinfo[3], $Conn);
	
	$qs = "select rrs_uServerId from tbl_rush_role_servers";
	$rs = mysql_query($qs);
	while($row = mysql_fetch_row($rs)){
		Load($row[0]);
	}
	
	mysql_close($Conn);
}

MainFunc();
?>
