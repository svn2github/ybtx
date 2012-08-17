<?php
function AddExp($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0];
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddExp", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0033", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddMoney($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddMoney", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0034", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddBindingMoney($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddBindingMoney", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0196", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddTicket($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddTicket", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0035", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddQuest($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["quest_name"] = UTF82Local($arg1);
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddQuest", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0036", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function FinishQuest($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["quest_name"] = UTF82Local($arg1);
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "FinishQuest", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0037", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddEquip($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["item_info"] = UTF82Local($arg2);
	$param["role_name"] = $role_name;
	
	GetItemInfo($arg2, $item_type, $item_name);
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddEquip", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0038", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddItem($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["item_info"] = UTF82Local($arg2);
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	GetItemInfo($arg2, $item_type, $item_name);
	
	$ret=PostData($gas_host, $gas_port, "AddItem", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0039", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function DelQuest($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["quest_name"] = UTF82Local($arg1);
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "DelQuest", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0040", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddSoul($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddSoul", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0041", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddBindingSoul($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["num"] = $arg1;
	$param["role_name"] = $role_name;
	
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddBindingSoul", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0197", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddGMMgr($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["user_name"] = $arg1;
	$info = explode(",",$arg2);
	$param["gm_level"] = $info[0];
	$param["game_id"] = $info[1];

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "ResAddGMMgr", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0042", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function MoneyCurrencyLimit($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["money_func"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "MoneyCurrencyLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0043", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function MoneyCurrencyLimitCanel($var, $gm, $role_name, $arg1, $arg2, $reason){
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["money_func"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "DelCurrencyLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0093", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function PassCompensate($var, $gm)
{
	global $tbl_list_compensate;
	$ids = $var["ids"];
	if($ids==""){
		$ids = array();
		$id=intval($var["id"]);
		$serv_id = intval($var["serverid"]);
		array_push($ids,$id."_".$serv_id);
	}
	foreach($ids as $i){
		$ia = explode("_",$i);
		$id = $ia[0];
		$serv_id = $ia[1];
		
		$qs = "select action_id, role_name, action_state, arg1, arg2, reason from tbl_gm_compensate_action where gca_uId = '$id' and action_state = 1 and server_id = '$serv_id'";
		$result = MySQLQuery($qs,"GMS");
		$row = mysql_fetch_row($result);
		if (!$row) {
			PrintNotice($tbl_list_compensate[0]);
		}
	
		if ($row[2] != 1 ){
			PrintNotice($tbl_list_compensate[2]);
		}
	
		global $ReqHandler;
		$action = substr($ReqHandler[$row[0]],3);
		$var["serverid"] = $serv_id;
		if ($action($var, $gm, $row[1], $row[3], $row[4], $row[5])){
			$qs = "update tbl_gm_compensate_action set action_state=3 where gca_uId = '$id'";
			$result = MySQLQuery($qs, "GMS");
		}
	}
	BackAndRefresh($tbl_sys_msg["success"]);
	return true;
}

function CancelCompensate($var, $gm)
{
	global $tbl_sys_msg,$tbl_list_compensate;

	$gm_name = mysql_escape_string($gm);
	$ids = $var["ids"];
	if($ids==""){
		$ids = array();
		$id=intval($var["id"]);
		$serv_id = intval($var["serverid"]);
		array_push($ids,$id."_".$serv_id);
	}
	foreach($ids as $i){
		$ia = explode("_",$i);
		$id = $ia[0];
		$serv_id = $ia[1];
		$qs = "update tbl_gm_compensate_action set action_state=2 where gca_uId = '$id'";
	
		$result = MySQLQuery($qs, "GMS");
		
		$qs = "select action_id, reason from tbl_gm_compensate_action where gca_uId = '$id'";
		$result = MySQLQuery($qs, "GMS");
		$row = mysql_fetch_row($result);
		
		$action_id = intval($row[0]);
		if($action_id < 10){
			$action_str = "000".$action_id;
		}elseif($action_id < 100){
			$action_str = "00".$action_id;
		}elseif($action_id < 1000){
			$action_str = "0".$action_id;
		}
		if (LogGMCommand($gm_name, $var["action_name"].".".$action_str, $row[1], $serv_id)){	
			PrintNotice($tbl_list_compensate[2]);
		}
	}
	BackAndRefresh($tbl_sys_msg["success"]);
	return true;
}

function JifenUseLimit($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["jifen_type"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "ResAddJifenLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0044", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function JifenUseLimitCanel($var, $gm, $role_name, $arg1, $arg2, $reason){
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["jifen_type"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "DelJifenUseLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0094", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function ItemUseLimit($var, $gm, $role_name, $arg1, $arg2, $reason)
{
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["item_name"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "ResAddItemLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0045", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function ItemUseLimitCanel($var, $gm, $role_name, $arg1, $arg2, $reason){
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["item_name"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "DelItemUseLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0059", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function QuestFinishLimit($var, $gm, $role_name, $arg1, $arg2, $reason){
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["quest_name"] = UTF82Local($arg1);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "AddQuestFinishLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0149", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function QuestFinishLimitCanel($var, $gm, $role_name, $arg1, $arg2, $reason){
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["quest_name"] = UTF82Local($arg1);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$ret=PostData($gas_host, $gas_port, "DelQuestFinishLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0150", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function ObjDropLimitCanel($var, $gm, $role_name, $arg1, $arg2, $reason){
	global $tbl_gm_db, $tbl_sys_msg;
	$gm_name = mysql_escape_string($gm);
	
	$serverid = intval($var["serverid"]);

	$param = array();
	$param["obj_name"] = UTF82Local($arg1);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "DelObjDropLimit", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0152", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function SendMail($var, $gm, $role_name, $arg1, $arg2, $reason){
	$gm_name = mysql_escape_string($gm);
	$serverid = intval($var["serverid"]);
	$tbl_arg1 = explode("|",$arg1);
	$ret = SendMailFun($serverid, $role_name, $tbl_arg1[0], $arg2, $tbl_arg1[1]);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0147", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddItemToTong($var, $gm, $role_name, $arg1, $arg2, $reason){
	$gm_name = mysql_escape_string($gm);
	$serverid = intval($var["serverid"]);
	
	$tbl = explode("_",$arg1);
	
	global $tbl_tong_page;
	
	$temp = strpos($tbl[1],":");
	if($temp){
		$base64name = UTF82Base64(substr($tbl[1],0,$temp)).substr($tbl[1],$temp);
	}else{
		$base64name = UTF82Base64($tbl[1]);
	}
	global $AddItemToTong;
	$param = array();
	$param["tong_name"] = $role_name;
	$param["item_type"] = $tbl[0];
	$param["item_name"] = $base64name;
	$param["num"] = $tbl[2];
	$param["page"] = $arg2;
	$param["log"] = sprintf($AddItemToTong,$tbl[2],$tbl[1],$tbl_tong_page[$arg2]);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	
	
	$ret=PostData($gas_host, $gas_port, "AddItemToTong", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0175", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddMoneyToTong($var, $gm, $role_name, $arg1, $arg2, $reason){
	$gm_name = mysql_escape_string($gm);
	$serverid = intval($var["serverid"]);
	
	$param = array();
	$param["tong_name"] = $role_name;
	$param["add_money"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "AddMoneyToTong", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0248", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddResourceToTong($var, $gm, $role_name, $arg1, $arg2, $reason){
	$gm_name = mysql_escape_string($gm);
	$serverid = intval($var["serverid"]);
	
	$param = array();
	$param["tong_name"] = $role_name;
	$param["add_resource"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "AddResourceToTong", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0249", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

function AddHonorToTong($var, $gm, $role_name, $arg1, $arg2, $reason){
	$gm_name = mysql_escape_string($gm);
	$serverid = intval($var["serverid"]);
	
	$param = array();
	$param["tong_name"] = $role_name;
	$param["add_honor"] = $arg1;

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "AddHonorToTong", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($reason,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"].".0250", $reason, $serverid)){	
		PrintNotice(LogInfo($reason,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

?>
