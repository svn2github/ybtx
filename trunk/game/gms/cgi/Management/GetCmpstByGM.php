<?php
function GMCancelCmpst($var, $gm)
{
	global $tbl_sys_msg;

	$gm_name = mysql_escape_string($gm);
	$id=intval($var["id"]);
	$qs = "update tbl_gm_compensate_action set action_state=2 where gca_uId = '$id' and gm = '$gm_name'";

	$result = MySQLQuery($qs, "GMS");
	
	$qs = "select action_id, reason, server_id from tbl_gm_compensate_action where gca_uId = '$id'";
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
	if (LogGMCommand($gm_name, $var["action_name"].".".$action_str, $row[1], $row[2])){	
		PrintNotice($tbl_sys_msg["success"]);
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}


function GetCmpstByGM($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_list_compensate;
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type='text/javascript' src='/js/checkbox.js'></script>
<script type='text/javascript' src='/js/page.js'></script>
<script type='text/javascript'>
function cancel(id)
{
	document.form1.target = "_block"
	document.form1.action_name.value='124';
	document.form1.id.value=id;
	document.form1.submit();
}
</script>
_js_src;
		echo $js_src."\n";
		echo "<script type='text/javascript' src='/js/log_info.js'></script>\n";
		form($var, $gm);
		echo "<input type='hidden' name='flag' />\n";
		echo "<input type='hidden' name='role_name' />\n";
		echo "<input type='hidden' name='str' />\n";
		echo "<input type='hidden' name='urs' />\n";
		echo "<input type='hidden' name='id' />";
		checkbox(false, true);
		ReturnAndBack();
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		echo "</form>\n";
		return $ret;
	}

	$serv_id = intval($var["serverindex"]);
	$gm_name = mysql_escape_string($gm);
	$filter = "";
	if($var["flag"]!="" || $var["role_name"]!=""){
		$filter = " and";
		if($var["flag"]!=""){
			$filter .= " reason like '%#[".$var["flag"]."[%'";
		}
		if($var["flag"]!="" && $var["role_name"]!=""){
			$filter .= " and";
		}
		if($var["role_name"]!=""){
			$k = $var["role_name"];
			$filter .= " (reason like '%[".$k.",%' or reason like '%,".$k."]%' or reason like '%,".$k.",%' or reason like '%[".$k."]%')";
		}
	}
	$query = "select gca_uId, gm, action_id, reason, action_state from tbl_gm_compensate_action where server_id = '$serv_id' and gm = '$gm_name'".$filter." order by gca_uId desc";
	page($var, $query, 123, "GMS", 0, $result, $page, $desc);
	
	echo "<table class='list_table'>\n";
	echo "<tr><th style='width:60px;'>".$page_main["user_name"]."</th><th style='width:160px;'>".$page_main["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th><th style='width:60px;'>".$tbl_sys_msg["act"]."</th><th style='width:80px;'>".$tbl_sys_msg["status"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td class='server' title='".$serv_id."'>%s</td></tr>",$row[1],GetGMCommandLogAction($row[2]),LogInfo($row[3]),(hasAuth(124,$gm) && $row[4]==1)?"<input type=button value='".$tbl_sys_msg["cancel"]."' onclick='cancel(".$row[0].")'/>":"",$tbl_list_compensate[$row[4]]);
	}
	echo "</table>\n";
	echo $page;
	return true;
}

?>
