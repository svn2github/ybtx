<?php
require("../../cgi-bin/gm_tools/InterFace.php");
function ResetSessionAction($uid, &$gm)
{
	global $sys_config, $tbl_sys_msg;
	$param = array();
	$param["UID"] = $uid;
	$param["gameId"] = $_REQUEST["gameId"];
	$key = $sys_config["game_key"];
	$param["sign"] = md5("$uid$key");
	$ret_xml = PostDataToUrl($sys_config["validate_url"], $param);
	$resp = new DOMDocument();
	$resp->loadXML("$ret_xml");
	$result = $resp->getElementsByTagName('result')->item(0)->nodeValue;
	if ($result != 1){
		if ($result == -100){
			printf($tbl_sys_msg["md5error"]);
			return false;
		}
		else if ($result == -201){
			printf($tbl_sys_msg["ip_error"]);
			return false;
		}
		else{
			printf($tbl_sys_msg["unknow_err"]);
			return false;
		}
	}
	$ret_game_id = $resp->getElementsByTagName('gameId')->item(0)->nodeValue;
	if ($ret_game_id != $_REQUEST["gameId"]){
		printf($tbl_sys_msg["no_same_id"]);
		return false;
	}

	$gm_name = $resp->getElementsByTagName('passportName')->item(0)->nodeValue;
	$gm = $gm_name;

	$qs = "select * from tbl_session where gm_name='$gm_name'";
	$result = MySQLQuery($qs, "GMS");
	if($row = mysql_fetch_row($result)){
		$qs = "update tbl_session set UID='$uid', sess_time=now() where gm_name = '$gm_name'";
	}else{
		$qs = "insert into tbl_session set UID='$uid', gm_name='$gm_name', sess_time=now()";
	}
	MySQLQuery($qs, "GMS");
	return true;
}

function Authenticate($var, &$gm)
{
	global $sys_config;
	$gm = "gm";
	if(!$sys_config["NeedAuth"]) return true;
	$uid = mysql_escape_string($var["UID"]);
	$qs = "select gm_name from tbl_session where UID = '$uid'";
	$result = MySQLQuery($qs, "GMS");

	if (($row = mysql_fetch_row($result)))
	{
		$gm = $row[0];
		return true;
	}
	return ResetSessionAction($uid, $gm);
}

function CheckPrevilege($var)
{
	global $sys_config;
	if(!$sys_config["NeedAuth"]) return true;
	$uid = mysql_escape_string($var["UID"]);
	$action_id = $var["action_name"];
	$qs = "select 1 from tbl_session_action sa, tbl_session s where sa.gm_name = s.gm_name and s.uid = '$uid' and sa.action_id = $action_id";
	$result = MySQLQuery($qs, "GMS");
	if (!$result){
		return false;
	}
	if ($row = mysql_fetch_row($result)) return true;
	return false;
}

function HasAuth($action_id, $gm){
	global $sys_config;
	if(!$sys_config["NeedAuth"]) return true;
	$ret = false;
	$qs = "select 1 from tbl_session_action where gm_name = '$gm' and action_id = $action_id";
	$result = MySQLQuery($qs, "GMS");
	if ($row = mysql_fetch_row($result)) $ret = true;
	return $ret;
}

function AuthManagement($var, $gm){
	$jsstr = <<<_js
<script type="text/javascript">
function submit_form(){
	document.form1.action_name.value = "208";
	document.form1.submit();
}
function select_all(obj){
	var o = $(obj).parents("tr");
	var b = $(obj).attr("checked");
	while($(o).next().children().get(0).nodeName=="TD"){
		o = $(o).next();
		$(o).find("input[type='checkbox']").attr("checked",b);
	}
}
</script>
_js;
	global $action_page,$sub_action,$page_main,$page_action_name,$page_title;
	$user = $var["user"];
	echo $jsstr;	
	form($var, $gm);
	echo "<input type=\"hidden\" name=\"user\" value=\"$user\" />\n";
	
	echo "<table class=\"list_table\">\n";
	echo "<tr><td>".$page_main["user_name"].": ".$user."</td></tr>";
	echo "</table>\n";
	echo "<table class=\"list_table\">\n";
	$auth_list = split(";\n",$authstr);
	$auth_table = array();
	foreach($auth_list as $auth){
		$auth_row = split(",",$auth);
		if($auth_row[5] == 1) {
			$auth_table[intval($auth_row[0])] = array($auth_row[1],array());
		}
		if($auth_row[5] == 2) {
			$auth_table[intval($auth_row[4])][1][intval($auth_row[0])] = $auth_row[1];
		}
	}
	
	$qs = "select action_id from tbl_session_action where gm_name = '$user'";
	$result = MySQLQuery($qs, "GMS");
	$right = array();
	while($row = mysql_fetch_row($result)){
		array_push($right,$row[0]);
	}
	
	echo "<tr><th style=\"width:30px;\"><input type=\"checkbox\" onchange=\"select_all(this);\" /></th><th style=\"text-align:left;\">".$page_title["*"]."</th></tr>\n";
	foreach($sub_action["*"] as $k){
		$str = "";
		if(in_array($k,$right)){
			$str = " checked = \"checked\"";
		}
		echo "<tr><td style=\"text-align:center;\"><input type=\"checkbox\" name=\"right[]\" value=\"$k\"$str /></td><td>".$k.":".$page_action_name[$k]."</td></tr>\n";
	}
	foreach($action_page as $key => $auth){
		echo "<tr><th style=\"width:30px;\"><input type=\"checkbox\" onchange=\"select_all(this);\" /></th><th style=\"text-align:left;\">".$page_title[$key]."</th></tr>\n";
		foreach($auth as $k){
			$str = "";
			if(in_array($k,$right)){
				$str = " checked = \"checked\"";
			}
			echo "<tr><td style=\"text-align:center;\"><input type=\"checkbox\" name=\"right[]\" value=\"$k\"$str /></td><td>".$k.":".$page_action_name[$k]."</td></tr>\n";
			if($sub_action[$k]!=null){
				foreach($sub_action[$k] as $sub){
					$substr = "";
					if(in_array($sub,$right)){
						$substr = " checked = \"checked\"";
					}
					echo "<tr><td style=\"text-align:center;\"><input type=\"checkbox\" name=\"right[]\" value=\"$sub\"$substr /></td><td style=\"color:#999;\">&nbsp;&nbsp;&nbsp;".$sub.":".$page_action_name[$sub]."</td></tr>\n";
				}
			}
		}
	}
	echo "</table>\n";
	global $tbl_sys_msg;
	echo "<table class=\"list_table\"><tr><td><input type=\"button\" onclick=\"submit_form()\" value=\"".$tbl_sys_msg["submit"]."\" /></td></tr></table>";
	echo "</form>";
	return true;
}

function AuthMgrFun($var, $gm){
	global $page_main;
	$user = $var["user"];
	echo "<table class=\"list_table\">\n";
	echo "<tr><td>".$page_main["user_name"].": ".$user."</td></tr>\n";
	echo "</table>";
	ReturnAndBack();
	
	MySQLQuery("delete from tbl_session_action where gm_name = '$user'", "GMS");
	
	$right = $var["right"];
	foreach($right as $r){
		MySQLQuery("insert into tbl_session_action values($r,'$user')", "GMS");
	}
	return true;
}
?>
