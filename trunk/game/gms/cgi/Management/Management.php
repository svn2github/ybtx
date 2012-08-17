<?php
function ShutUp($var, $gm){
	global $tbl_sys_msg;
	ReturnAndBack();
	if($var["role_name"]==""){
		PrintNotice($tbl_sys_msg["nobody"]);
		return true;
	}
	return eachServer($var, $gm, ShutUp_f);
}
function ShutUp_f($var, $gm, $serverid){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);
	$action = "";
	$param = array();
	$channel_ids =$var["channel_id"];
	$start_time =$var["start_time"];
	$end_time =$var["end_time"];
	if (!$start_time)
		$start_time="2009-10-01";
	if (!$end_time)
		$end_time="2019-10-01";
	$param["start_time"] = $start_time;
	$param["end_time"] = $end_time;
	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "channel" => implode(",",$channel_ids), "reason" => $reason);
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret = "";
	
	if($channel_ids != null && count($channel_ids)>0) {
		if($channel_ids[0]==0){
			$param["channel_id"] = 0;
			$ret.=PostData($gas_host, $gas_port, "ShutUp", $param);
			if($var["end_time"] != null && $var["end_time"]!=""){
				foreach(explode(",",$var["role_name"]) as $role_name){
					$qs = "insert into tbl_forbid_talk(`role_name`,`channal_id`,`end_date`,`server_id`,`reason`,`gm`) values('".$role_name."',0,'".$var["end_time"]."',".$serverid.",'".$reason."','".$gm_name."')";
					MySQLQuery($qs, "GMS");
				}
			}
		}else{
			for($i=0;$i<count($channel_ids);$i++){
				$channel_id = $channel_ids[$i];
				$param["channel_id"] = $channel_id;
				$ret.=PostData($gas_host, $gas_port, "ShutUp", $param);
				if($var["end_time"] != null && $var["end_time"]!=""){
					foreach(explode(",",$var["role_name"]) as $role_name){
						$qs = "insert into tbl_forbid_talk(`role_name`,`channal_id`,`end_date`,`server_id`,`reason`,`gm`) values('".$role_name."',".$channel_id.",'".$var["end_time"]."',".$serverid.",'".$reason."','".$gm_name."')";
						MySQLQuery($qs, "GMS");
					}
				}
			}
		}
	}
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function CancelShutUp($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, CancelShutUp_f);
}
function CancelShutUp_f($var, $gm, $serverid){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);
	
	$channel_ids =$var["channel_id"];

	$action = "";
	$param = array();
	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "channel" => implode(",",$channel_ids), "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret = "";
	if($channel_ids != null && count($channel_ids)>0) {
		for($i=0;$i<count($channel_ids);$i++){
			$channel_id = $channel_ids[$i];
			$param["channel_id"] = $channel_id;
			$ret.=PostData($gas_host, $gas_port, "CancelShutUp", $param);
		}
	}
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function KickOff($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, KickOff_f);
}
function KickOff_f($var, $gm, $serverid){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$action = "";
	$param = array();
	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "KickOff", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function MatchScene($var, $gm){
	global $page_main;
	$ret = Lan_Scene_SearchByDisplayName($var["str_match"]);
	echo "<select name=\"scene_name\">\n";
	echo "<option>".$page_main["select_action"]."</option>\n";
	foreach($ret as $item){
		if($item["SceneType"]==1 || $item["SceneType"]==7)
			printf("<option value=\"%s\" InitPosY=\"%d\" InitPosX=\"%d\">%s</option>\n",Base642UTF8($item["RealName"]), intval($item["InitPosX"]), intval($item["InitPosY"]), $item["DisplayName"]."(".Base642UTF8($item["RealName"]).")");
	}
	printf("</select>");
	return true;
	
}
function MoveRole($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, MoveRole_f);
}
function MoveRole_f($var, $gm, $serverid){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$action = "";
	$param = array();

	$scene_name = mysql_escape_string($var["scene_name"]);
	if($scene_name==""){
		global $page_main;
		PrintNotice($page_main["scene"]);
		return false;
	}
	$param["scene_name"] = UTF82Local($scene_name);

	$x_pos = intval($var["x_pos"]);
	$y_pos = intval($var["y_pos"]);
	$param["x_pos"] = $x_pos;
	$param["y_pos"] = $y_pos;
	
	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "scene" => $scene_name, "x" => $x_pos, "y" => $y_pos, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "MoveRole", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}
	return false;
}
function SendWarning($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, SendWarning_f);
}
function SendWarning_f($var, $gm, $serverid){
	$warning=mysql_escape_string($var["warning"]);
	$ret = SendWarningFun($serverid,$var["role_name"],$warning);
	
	$reason=$var["reason"];
	$gm_name = mysql_escape_string($gm);
	$reason = mysql_escape_string($reason);
	$action = array("role_name" => $var["role_name"], "content" => $warning, "reason" => $reason);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function FreezeUrs($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, FreezeUrs_f);
}
function FreezeUrs_f($var, $gm, $serverid){
	$urs=mysql_escape_string($var["user_name"]);
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 

	$query_string = "select c.c_sName 
		from tbl_user_static as us,tbl_char_static as cs ,tbl_char as c
		where us.us_uId = cs.us_uId and 
		cs.cs_uId = c.cs_uId and 
		us.us_sName = '".$urs."'";
	$result = MySQLQuery($query_string, "Game", $serverid);
	while($row=mysql_fetch_row($result)){
		$param2 = array();
		$param2["role_name"] = $row[0];
		$ret=PostData($gas_host, $gas_port, "KickOff", $param2);
	}
	
	$gm_name = mysql_escape_string($gm);
	$reason = $var["reason"];
	$reason = mysql_escape_string($reason);

	$param1 = array();
	$param1["urs"] = $urs;
	$action = array("user_name" => $urs, "reason" => $reason);
	
	$ret=PostData($gas_host, $gas_port, "FreezeUrs", $param1);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return false;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function UnFreezeUrs($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, UnFreezeUrs_f);
}
function UnFreezeUrs_f($var, $gm, $serverid){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);
	
	$param = array();
	
	$urs=mysql_escape_string($var["user_name"]);

	$param["urs"] = $urs;
	$action = array("user_name" => $urs, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "UnFreezeUrs", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function DelItemInBag($var, $gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$action = "";
	$param = array();
	
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$param["item_name"] = UTF82Local($item_name);
	$param["item_type"] = $item_type;
	
	$item_num=intval($var["num"]);
	$param["item_num"] = $item_num;

	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "type" => $item_type, "item" => $item_name, "number" => $item_num, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "DelItemInBag", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}
function DelItemOnBody($var, $gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$action = "";
	$param = array();
	
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$param["item_name"] = UTF82Local($item_name);
	$param["item_type"] = $item_type;

	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "item" => $item_name, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "DelItemOnBody", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}
function ClearState($var, $gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$action = "";
	$param = array();
	
	$state_name=mysql_escape_string($var["state_name"]);
	$param["state_name"] = UTF82Local($state_name);

	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "status" => $state_name, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "ClearState", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function DelGMMmr($var, $gm){
	$gm_name = mysql_escape_string($gm);

	$serverid =intval($var["serverid"]);
	$param = array();
	$user_name = $var["id"];
	$game_id = intval($var["game_id"]);
	$param["user_name"] = $user_name;
	$param["game_id"] = $game_id;
	$action = array("user_name" => $user_name, "GameID" => $game_id, "reason" => $reason);
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "DelGMMmr", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}

function ChangeGMLevel($var, $gm){
	$gm_name = mysql_escape_string($gm);
	$serverid =intval($var["serverid"]);
	$param = array();
	$user_name = $var["user_name"];
	$gm_level =intval($var["gm_level"]);
	$game_id = intval($var["game_id"]);
	$param["user_name"] = $user_name;
	$param["gm_level"] = $gm_level;
	$param["game_id"] = $game_id;
	$action = array("user_name" => $user_name, "level" => $gm_level, "GameId" => $game_id, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "ChangeGMLevel", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}

	return false;
}
function DelItemInDepot($var, $gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$param = array();
	
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$param["item_name"] = UTF82Local($item_name);
	$param["item_type"] = $item_type;
	
	$item_num=intval($var["num"]);
	$param["item_num"] = $item_num;

	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "item" => $item_name, "number" => $item_num, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "DelItemInDepot", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}
function DelItemInTongDepot($var, $gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$page = $var["uPage"];
	$pos  = $var["pos"];
	$item_num=intval($var["num"]);
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$action = "";
	
	global $tbl_tong_page,$DelItemInTongDepot;
	$param = array();
	$param["page"] = $page;
	$param["pos"] = $pos;
	$param["tong_name"] = $var["tong_name"];
	$param["msg"] = sprintf($DelItemInTongDepot,$tbl_tong_page[$page],$item_num,$item_name);
	$action = array("tong" => $var["tong_name"], "item" => $item_name, "number" => $item_num, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "DelItemInTongDepot", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}
function ItemNameMatch($var, $gm){
	global $bid2type,$page_main;
	$ret = Lan_Item_SearchByDisplayName($var["str_match"]);
	echo "<select name=\"item_name_\">\n";
	echo "<option>".$page_main["select_action"]."</option>\n";
	foreach($ret as $type => $items){
		echo "<optgroup label=\"".$bid2type[$type]."\">";
		foreach($items as $item){
			$item_realname = base642UTF8($item["RealName"]);
			$item_displayname = $item["DisplayName"];
			printf("<option value=\"%s_%s\">%s(%s)</option>\n", $type,$item_realname,$item_displayname,$item_realname);
		}
	}
	echo "</select>";
	printf($str);
	return true;
}
function QuestNameMatch($var, $gm){
	global $page_main;
	$ret = Lan_Quest_SearchByDisplayName($var["str_match"]);
	echo "<select name=\"quest_name\">\n";
	echo "<option>".$page_main["select_action"]."</option>\n";
	foreach($ret as $item){
			printf("<option value=\"%s\">%s</option>\n",Base642UTF8($item["RealName"]), $item["DisplayName"]."(".Base642UTF8($item["RealName"]).")");
	}
	echo "</select>";
	return true;
}
function ClearItemInBag($var,$gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$action = "";
	$param = array();
	
	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "type" => $item_type, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "ClearItemInBag", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}

function ClearItemInDepot($var,$gm){
	$gm_name = mysql_escape_string($gm);

	$reason=$var["reason"];
	$reason = mysql_escape_string($reason);

	$serverid =intval($var["serverid"]);
	$action = "";
	$param = array();
	
	$param["role_name"] = $var["role_name"];
	$action = array("role_name" => $var["role_name"], "type" => $item_type, "reason" => $reason);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "ClearItemInDepot", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}

function UserTrust($var, $gm){
	$role_name = $var["role_name"];
	$password = md5($var["password"]);
	$time = intval($var["time"]);
	$reason = $var["reason"];
	$serverid = $var["serverid"];
	
	$param = array();
	$param["role_name"] = $role_name;
	$param["password"] = $password;
	$param["time"] = $time;
	
	$action = array("role_name" => $role_name, "time" => $time, "reason" => $reason);
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "UserTrust", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}

function CancelUserTrust($var, $gm){
	$role_name = $var["role_name"];
	$reason = $var["reason"];
	$serverid = $var["serverid"];
	
	$param = array();
	$param["role_name"] = $role_name;
	
	$action = array("role_name" => $role_name, "reason" => $reason);
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	
	$ret=PostData($gas_host, $gas_port, "CancelUserTrust", $param);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serverid)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}

	return false;
}
?>
