<?php
require_once("../../conf/gm_tools/Config.php");
require_once("../../lib/gm_tools/Lib.php");

function P($msg){
//	echo $msg;
}

function Main(){
	$now = Date("Y-m-d H:i:s");
	P($now."==============================\n");
	global $tbl_gm_db,$tbl_serv_db,$tbl_game_server;
	$str = "(";
	
	$qs = "select * from tbl_channel_word";
	$result = MySQLQuery($qs, "GMS");
	$words = array();
	while($row = mysql_fetch_row($result)){
		array_push($words, "lcc.lcc_sMsg like '%".$row[0]."%'");
	}
	$str.= implode(" or ", $words);
	$str.= ")";
	P("Number of Check Word: ".count($words)."\n");
	
	$qs = "select gv_value from tbl_gm_variable where gv_name = 'ChannelChatStemp'";
	$result = MySQLQuery($qs, "GMS");
	if($row = mysql_fetch_row($result)){
		P("Last updated at: ".$row[0]."\n");
		$str.= " and lcc.lcc_dtChatTime > '".$row[0]."' and lcc.lcc_dtChatTime <= '".$now."'";
	}
	
	$qs = "update tbl_gm_variable set gv_value = '".$now."' where gv_name = 'ChannelChatStemp'";
	MySQLQuery($qs, "GMS");
	
	foreach($tbl_serv_db as $key => $dbinfo){
		P("Server: ".$key."\t");
		
		$insert = array();
		
		$qs = "select lcc.cs_uId,lcc.lcc_dtChatTime,$key as serv_id,lcc.lcc_sMsg,lcc.lcc_sChannelName from tbl_log_channel_chat lcc where ".$str;
		$result = MySQLQuery($qs, "GameLog", $key);
		while($row=mysql_fetch_row($result)){
			$qs = "select c.c_sName, us.us_sName from tbl_char c, tbl_char_static cs, tbl_user_static us where c.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId and cs.cs_uId = ".$row[0];
			$rs = MySQLQuery($qs, "Game", $key);
			if($r = mysql_fetch_row($rs)){
				$row[0] = $r[0];
				$user_name = $r[1];
			}else{
				$row[0] = "[".$row[0]."]";
			}
			
			array_push($insert, "insert into tbl_channel_chat values('".$row[0]."','".$row[3]."','".$row[1]."','".Local2UTF8($row[4])."',".$row[2].",'".$user_name."')");
		}
		P("Number of Chat: ".count($insert)."\n");
		foreach($insert as $query){
			MySQLQuery($query, "GMS");
		}
	}
}

Main();
?>
