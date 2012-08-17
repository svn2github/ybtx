<?php
require_once("/home/webfold/conf/gm_tools/Config.php");
require_once("/home/webfold/lib/gm_tools/Lib.php");

function SendAnnounce(){
	$qs = "select title, begin_time, end_time, step, content, serverid, ga_id from tbl_gm_anousment where end_time > now() and begin_time < now() and state <> 2";
	$result = mysql_query($qs);
	while ($row=mysql_fetch_row($result))
	{
		$begin_time	=$row[1];
		$end_time		=$row[2];
		$step				=$row[3];
		$title			=$row[0];
		$content		=$row[4];
		$serv_id		=$row[5];
		$id					=$row[6];
		$date				=getdate();
		if($date["minutes"]%$step==0){
			$param = array();
			global $tbl_game_server;
			$gas_host = $tbl_game_server[$serv_id][0]; 
			$gas_port = $tbl_game_server[$serv_id][1]; 
			$param["content"] = $content;
			$ret=PostData($gas_host, $gas_port, "SendAnousment", $param);
			
			mysql_query("update tbl_gm_anousment set state = 3 where ga_id = '$id'");
			echo "Send Announce: \tServer:".$serv_id."\tTitle:".$title."\n";
		}
	}
}

function CancelForbidTalk(){
	$qs = "select * from tbl_forbid_talk where end_date < now()";
	$result = mysql_query($qs);
	while ($row=mysql_fetch_row($result))
	{
		$id					=$row[0];
		$role_name	=$row[1];
		$channel_id	=$row[2];
		$serv_id		=$row[4];
		
		$param = array();
		global $tbl_game_server;
		$gas_host = $tbl_game_server[$serv_id][0]; 
		$gas_port = $tbl_game_server[$serv_id][1]; 
		$param["role_name"] 	= $role_name;
		$param["channel_id"] 	= $channel_id;
		$ret.=PostData($gas_host, $gas_port, "CancelShutUp", $param);
		
		mysql_query("delete from tbl_forbid_talk where id = $id") or print(mysql_error());
		echo "Cancel Forbid Talk: \tServer:".$serv_id."\tRole Name:".$role_name."\tChannel:".$channel_id."\n";
	}
}

function MainFunc(){
	echo date("Y-M-d H:i:s")."=================\n";
	global $tbl_gm_db;
	$dbinfo=$tbl_gm_db[0];
	$Conn = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
	mysql_select_db($dbinfo[3], $Conn);
	
	SendAnnounce();
	CancelForbidTalk();
	
	mysql_close($Conn);
}
MainFunc();
?>
