<?php
function HotUpdate($var, $gm){
	$type = intval($var["type"]);
	$code = $var["code"];
	$file = $var["file"];
	MySQLQuery("insert into tbl_hot_update(hu_type,hu_code,hu_file) values($type,'$code','$file')","GMS");
	$id = mysql_insert_id();
	$var["hu_id"] = $id;
	
	ReturnAndBack();
	$ret = eachServer($var, $gm, HotUpdate_f);
	return $ret;
}
function HotUpdate_f($var, $gm, $serverid){
	$hu_id = $var["hu_id"];
	$type = intval($var["type"]);
	$code = $var["code"];
	$file = $var["file"];
	
	MySQLQuery("insert into tbl_hot_update_result(hu_id,hur_serverid) values($hu_id,$serverid)", "GMS");
	
	$code_server = UTF82Local($code);
	
	$md5_code = str_replace("\\\"","\"", $code_server);
	$md5_code = str_replace("\\\\", "\\", $md5_code);
	$md5_code = str_replace("\\'", "'", $md5_code);
	
	$param = array();
	$param["type"] = $type;
	$param["filename"] = $file;
	$param["md5"] = md5($md5_code);
	$param["code"] = $code_server;
	
	
	global $tbl_game_server;
	
	$action = array("ID" => $hu_id, "type" => $type, "file" => $file);
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret = PostData($gas_host, $gas_port, "HotUpdate", $param);
	if($ret==200){
		MySQLQuery("replace into tbl_hot_update_result values($hu_id,$serverid,1,now())", "GMS");
		LogGMCommand($gm, $var["action_name"], $action, $serverid);
		PrintNotice(LogInfo($action,true));
	}else{
		PrintNotice(LogInfo($action,false,$ret));
	}
	return true;
}

function HotUpdateResult($var, $gm)
{
	global $tbl_hot_update_log,$tbl_hot_update_Result,$tbl_hot_update_state,$tbl_hot_update_type,$tbl_gm_db,$tbl_game_server;
	
	$id = $var["id"];
	
	echo "<h1>".$tbl_hot_update_Result[3]."</h1>\n";
	ReturnAndBack();
	echo "<h2>".$tbl_hot_update_Result[4]."</h2>\n";
	
	$query = "select hu_type, hu_code, hu_datetime, hu_file from tbl_hot_update where hu_id = ".$id;
	$result = MySQLQuery($query, "GMS");
	echo "<table class='list_table'>\n";
	
	$row=mysql_fetch_row($result);
	echo "<tr><td style='width:80px;'>".$tbl_hot_update_type[intval($row[0])]."</td><td style='width:120px;'>".$row[2]."</td><td rowspan='2'><pre>".$row[1]."</pre></td></tr>\n";
	echo "<tr><td colspan='2'>".$row[3]."</td></tr>\n";
	echo "</table>\n";
	echo "<h2>".$tbl_hot_update_Result[5]."</h2>\n";
	
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_hot_update_Result[0]."</th><th>".$tbl_hot_update_Result[1]."</th><th style='width:120px;'>".$tbl_hot_update_Result[2]."</th></tr>\n";
	
	$query = "select hur_serverid, hur_state, hur_datetime from tbl_hot_update_result where hu_id=".$id;
	$result = MySQLQuery($query, "GMS");
	while($row=mysql_fetch_row($result)){
		$row[2] = ($row[1]==0)?"-":$row[2];
		$row[1] = $tbl_hot_update_state[intval($row[1])];
		$row[0] = $tbl_game_server[intval($row[0])][2];
		printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$row[1],$row[2]);
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function HotUpdateLog($var, $gm){
	global $tbl_hot_update_log;
	$strjs = <<<_strjs
<script type="text/javascript" src="/js/page.js"></script>
<script type="text/javascript">
function OnDetail(id,obj){
	document.form1.id.value = id;
	document.form1.action_name.value = '155';
	document.form1.submit();
}
</script>
_strjs;
	echo $strjs."\n";
	echo "<h1>".$tbl_hot_update_log[4]."</h1>\n";
	ReturnAndBack();
	form($var, $gm);
	echo "<input type='hidden' name='id'/>\n";
	
	echo "<table class='list_table'>\n";
	echo "<tr><th colspan='2'>".$tbl_hot_update_log[0]."</th><th>".$tbl_hot_update_log[1]."</th></tr>\n";

	global $tbl_sys_msg,$tbl_hot_update_type;
	$query = "select hu_type, hu_code, hu_id, hu_datetime, hu_file from tbl_hot_update order by hu_id desc";
	page($var, $query, 156, "GMS", 0, $result, $page, $desc);
	while($row=mysql_fetch_row($result)){
		$row[0] = $tbl_hot_update_type[intval($row[0])];
		printf("<tr".(($j%2)?"":" class='even'")."><td style='width:80px'>%s</td><td style='width:120px;'>%s</td><td rowspan='3'><pre style='word-break:break-all;'>%s</pre></td></tr>\n",$row[0],$row[3],$row[1]);
		printf("<tr".(($j%2)?"":" class='even'")."><td colspan='2' style='word-break:break-all;'>%s</td></tr>\n",$row[4]);
		printf("<tr".(($j++%2)?"":" class='even'")."><td colspan='2'><input type='button' value='".$tbl_hot_update_log[4]."' onclick='OnDetail(%d,this)' /></td></tr>\n",$row[2]);
	}
	echo "</table>\n";
	echo $page;
	
	echo "</form>\n";
	ReturnAndBack();
	
	return true;
}

function GetServerDate($var, $gm){
	global $tbl_get_server_date;
	if($var["serverindex"]==null || $var["serverindex"]==""){
		ReturnAndBack();
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}
	
	$serverid = $var["serverindex"];
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$param = array();
	$ret = PostData($gas_host, $gas_port, "GetServerDate", $param);
	
	echo "<table class='form_table'>\n";
	printf ("<tr><th>%s</th><td>%s<td></tr>\n",$tbl_get_server_date,Local2UTF8($ret));
	echo "</table>\n";
	return true;
}

function OnlineIP($var,$gm){
	global $tbl_serv_db,$page_main;
	$servers = $var["server_id"];
	$result = array();
	
	$js = <<<_js_str
<script type="text/javascript">
function detail(obj){
	document.form1.action_name.value = '240';
	document.form1.ip.value = $(obj).html();
	document.form1.submit();
}
</script>
_js_str;

	echo $js;
	
	form($var,$gm);
	echo "<input type='hidden' name='ip' />\n";
	echo "<input type='hidden' name='servers' value='".implode(",",$servers)."' />\n";
	echo "</form>\n";

	echo "<table class='list_table'>\n";
	echo "<tr><th>IP</th><th>".$page_main["number"]."</th></tr>\n";
	
	foreach($servers as $serv){
		$qs = "select uo.uo_sIp, count(*) c from tbl_user_online uo group by uo.uo_sIp having c > 3 order by c desc";
		$rs = MySQLQuery($qs, "Game", $serv);
		while($row = mysql_fetch_row($rs)){
			if($result[$row[0]] == null)
				$result[$row[0]] = intval($row[1]);
			else
				$result[$row[0]] = $result[$row[0]] + intval($row[1]);
		}
	}
	arsort($result);
	foreach($result as $ip => $count){
		printf("<tr><td><a href='#' onclick='detail(this);'>%s</a></td><td>%d</td></tr>\n",$ip,$count);
	}
	
	echo "</table>\n";
	
	return true;
}

function OnlineIPSearch($var,$gm){
	global $tbl_serv_db,$tbl_game_server,$tbl_sys_msg,$page_main;
	if($var["servers"]!=null && $var["servers"]!=""){
		$servers = explode(",",$var["servers"]);
	}else{
		$servers = $var["server_id"];
	}
	$ip = $var["ip"];
	
	echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
	form($var, $gm);
	checkbox(true,true);
	echo "</form>";
	
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_sys_msg["server"]."</th><th>".$page_main["user_name"]."</th><th>".$page_main["role_name"]."</th><th>".$page_main["level"]."</th><th>".$page_main["scene"]."</th><th>".$tbl_sys_msg["time"]."</th></tr>\n";
	
	foreach($servers as $serv){
		$qs = "select us.us_sName, c.c_sName, cb.cb_uLevel, sc.sc_sSceneName, uo.uo_dtLoginTime from tbl_user_online uo, tbl_user_static us, tbl_char_static cs, tbl_char_online co, tbl_char c, tbl_char_basic cb, tbl_char_position cp, tbl_scene sc where uo.us_uId = us.us_uId and cs.us_uId = us.us_uId and cs.cs_uId = co.cs_uId and cs.cs_uId = c.cs_uId and cs.cs_uId = cb.cs_uId and cs.cs_uId = cp.cs_uId and cp.sc_uId = sc.sc_uId and uo.uo_sIp = '$ip'";
		$rs = MySQLQuery($qs, "Game", $serv);
		while($row = mysql_fetch_row($rs)){
			printf("<tr><td>%s</td><td><input type='checkbox' flag='user' value='%s' onclick='Checkbox(this)' />%s</td><td><input type='checkbox' flag='role' value='%s' onclick='Checkbox(this)' />%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$tbl_game_server[$serv][2],$row[0],$row[0],$row[1],$row[1],$row[2],Lan_Scene_GetDisplayName(Local2UTF8($row[3])),$row[4]);
		}
	}
	
	echo "</table>\n";
	
	return true;
}

function LoginIP($var, $gm){
	global $tbl_log_db,$page_main;
	$servers = $var["server_id"];
	$result = array();
	
	$js = <<<_js_str
<script type="text/javascript">
function detail(obj){
	document.form1.action_name.value = '277';
	document.form1.ip.value = $(obj).html();
	document.form1.submit();
}
</script>
_js_str;

	echo $js;
	
	form($var,$gm);
	echo "<input type='hidden' name='ip' />\n";
	echo "<input type='hidden' name='servers' value='".implode(",",$servers)."' />\n";
	echo "<input type='hidden' name='begin_time' value='".$var["begin_time"]."' />\n";
	echo "<input type='hidden' name='end_time' value='".$var["end_time"]."' />\n";
	echo "</form>\n";

	echo "<table class='list_table'>\n";
	echo "<tr><th>IP</th><th>".$page_main["number"]."</th></tr>\n";
	
	foreach($servers as $serv){
		$qs = "select ll.ll_sIP, count(distinct lu.lus_uId) c1 from (select ll.ll_sIP, count(*) c from tbl_log_login ll, tbl_log_event le where ll.le_uId = le.le_uId ".getDateLimit($var,$serv)." group by ll.ll_sIP having c > 3) tbl, tbl_log_login ll, tbl_log_user lu, tbl_log_event le where tbl.ll_sIP = ll.ll_sIP and ll.le_uId = le.le_uId and lu.le_uId = le.le_uId ".getDateLimit($var,$serv)." group by ll.ll_sIP having c1 > 3 order by c1 desc";
		$rs = MySQLQuery($qs, "Log", $serv);
		while($row = mysql_fetch_row($rs)){
			if($result[$row[0]] == null)
				$result[$row[0]] = intval($row[1]);
			else
				$result[$row[0]] = $result[$row[0]] + intval($row[1]);
		}
	}
	arsort($result);
	foreach($result as $ip => $count){
		printf("<tr><td><a href='#' onclick='detail(this);'>%s</a></td><td>%d</td></tr>\n",$ip,$count);
	}
	
	echo "</table>\n";
	
	return true;
}

function LoginIPSearch($var, $gm){
	global $tbl_log_db,$tbl_game_server,$tbl_sys_msg,$page_main;
	if($var["servers"]!=null && $var["servers"]!=""){
		$servers = explode(",",$var["servers"]);
	}else{
		$servers = $var["server_id"];
	}
	$ip = $var["ip"];
	
	echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
	form($var, $gm);
	checkbox(true,true);
	echo "</form>";
	
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_sys_msg["server"]."</th><th>".$page_main["user_name"]."</th><th>".$tbl_sys_msg["time"]."</th></tr>\n";
	
	foreach($servers as $serv){
		$qs = "select lu.lus_uId, le.le_dtDateTime from tbl_log_login ll, tbl_log_user lu, tbl_log_event le where ll.le_uId = le.le_uId and lu.le_uId = le.le_uId and ll.ll_sIP = '$ip' ".getDateLimit($var,$serv)." group by lu.lus_uId";
		$rs = MySQLQuery($qs, "Log", $serv);
		while($row = mysql_fetch_row($rs)){
			$user_name = GetUserName($row[0], $serv);
			printf("<tr><td>%s</td><td><input type='checkbox' flag='user' value='%s' onclick='Checkbox(this)' />%s</td><td>%s</td></tr>\n",$tbl_game_server[$serv][2],$user_name,$user_name,$row[1]);
		}
	}
	
	echo "</table>\n";
	
	return true;
}

function GetItemCfg($var, $gm){
	global $page_main,$bid2type;
	
	$name = $var["name"];
	
	echo "<table class='list_table'>\n";
	printf("<tr><th colspan='2'>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n", $page_main["type"],$page_main["real_name"],"Base 64",$page_main["display_name"]);
	
	$param = array();
	if ($var["type"] == "display_name"){
		$ret = Lan_Item_SearchByDisplayName($name);
	}else{
		if ($var["type"] == "base64"){
			$name = base642UTF8($name);
		}
		$ret = Lan_Item_SearchByRealName($name);
	}
	
	foreach($ret as $type => $items){
		$str = sprintf("<td rowspan='%1\$d'>%2\$d</td><td rowspan='%1\$d'>%3\$s</td>",count($items),$type,$bid2type[$type]);
		foreach($items as $item){
			printf("<tr>%s<td>%s</td><td>%s</td><td>%s</td></tr>\n",$str,Base642UTF8($item["RealName"]),$item["RealName"],$item["DisplayName"]);
			$str = "";
		}
	}
	
	echo "</table>\n";
	return true;
}

function GetTaskCfg($var, $gm){
	global $page_main;
	
	$name = $var["name"];
	
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n", $page_main["real_name"],"Base 64",$page_main["display_name"]);
	
	$param = array();
	if ($var["type"] == "display_name"){
		$ret = Lan_Quest_SearchByDisplayName($name);
	}else{
		if ($var["type"] == "base64"){
			$name = base642UTF8($name);
		}
		$ret = Lan_Quest_SearchByRealName($name);
	}
	
	foreach($ret as $item){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",Base642UTF8($item["RealName"]),$item["RealName"],$item["DisplayName"]);
	}
	
	echo "</table>\n";
	return true;
}

function GetNPCCfg($var, $gm){
	global $page_main,$tbl_sys_msg;
	
	$name = $var["name"];
	
	$js_str = <<<_js_str
<script>
function ondetail(name){
	document.form1.action_name.value = '46';
	document.form1.npc_name.value = name;
	document.form1.submit();
}
</script>
_js_str;
	echo $js_str."\n";
	form($var, $gm);
	echo "<input type='hidden' name='npc_name' />\n";
	echo "</form>\n";
	
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n", $page_main["real_name"],"Base 64",$page_main["display_name"],$tbl_sys_msg["detail"]);
	
	$param = array();
	if ($var["type"] == "display_name"){
		$ret = Lan_NPC_SearchByDisplayName($name);
	}else{
		if ($var["type"] == "base64"){
			$name = base642UTF8($name);
		}
		$ret = Lan_NPC_SearchByRealName($name);
	}
	
	foreach($ret as $item){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td><button onclick='ondetail(\"%2\$s\")'>%s</button></td></tr>\n",Base642UTF8($item["RealName"]),$item["RealName"],$item["DisplayName"],$tbl_sys_msg["detail"]);
	}
	
	echo "</table>\n";
	return true;
}

function RushRoleConfig($var, $gm){
	if($var["server_id"] == null){
		global $page_main, $tbl_sys_msg, $tbl_gm_db, $tbl_game_server;
		form($var, $gm);
		echo "<table class='form_table'>";
		echo "<tr><th>".$page_main["select_server"]."</th><td>";
		
		$jsstr = <<<_jsstr
<script>
$(function(){
	var obj = $("#server_all");
	var checkbox_all_id = $(obj).attr("id");
	var action_id = checkbox_all_id.substring(0,checkbox_all_id.indexOf("_"));
	$(obj).change(function(){
		var state = $(obj).attr("checked");
		$("."+action_id+"_checkbox").attr("checked",state);
	});
	$("."+action_id+"_checkbox").change(function(){
		var state = $(obj).attr("checked");
		if(state){
			if($("."+action_id+"_checkbox:checked").length == $("."+action_id+"_checkbox").length){
				$("#"+action_id+"_all").attr("checked",true);
			}
		}else{
			$("#"+action_id+"_all").attr("checked",false);
		}
	});
	document.form1.action_name.value = "252";
});
</script>
_jsstr;
		echo $jsstr."\n";
		$qs = "select rrs_uServerId from tbl_rush_role_servers group by rrs_uServerId";
		$rs = MySQLQuery($qs, "GMS");
		echo "<input type='checkbox' id='server_all' onclick='checkall(this)'/><label for='all_server'>".$tbl_sys_msg["all_server"]."</label><br/>";
		while($row = mysql_fetch_row($rs)){
			$i = $row[0];
			echo "<input type='checkbox' class='server_checkbox' id='server_".$i."' name='server_id[]' value='".$i."' onclick='chooseServer(this)' /><label for='server_".$i."'>".$tbl_game_server[$i][2]."</label><br/>";
		}
		
		echo "</td></tr>\n";
		echo "<tr><th>".$tbl_sys_msg["on"]."/".$tbl_sys_msg["off"]."</th><td>"."<input type='radio' name='RushRegisterAccountFlag' value='1' id='252_1'/><label for='252_1'>".$tbl_sys_msg["on"]."</label><input type='radio' name='RushRegisterAccountFlag' value='0' id='252_2'/><label for='252_2'>".$tbl_sys_msg["off"]."</label>\n";
		echo "<tr><th>".$page_main["RoleNumPerServer"]."</th><td>"."<input type='text' name='MaxRushRoleNumPerServer' />\n";
		echo "<tr><th>".$page_main["RoleNumPerCamp"]."</th><td>"."<input type='text' name='MaxRushRoleNumPerCamp' />\n";
		echo "<tr><th>".$page_main["RoleNumPerAccount"]."</th><td>"."<input type='text' name='MaxRushRoleNumPerAccount' />\n";
		echo "</table>\n";
		echo "<table class='form_table'>\n";
		echo "<tr><td style=\"text-align:center\"><input type=\"submit\" value=\"".$tbl_sys_msg["submit"]."\" /></td></tr>\n";
		echo "</table>\n";
		echo "</form>\n";
		return true;
	}else{
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$ret = eachServerFrame($var, $gm);
		return $ret;
	}
	$serverid = $var["serverindex"];
	global $tbl_game_server,$tbl_serv_db,$tbl_sys_msg,$page_main;
	$gas_host = $tbl_game_server[$serverid][0];
	$gas_port = $tbl_game_server[$serverid][1];
	
	if($var["RushRegisterAccountFlag"]!=null && $var["RushRegisterAccountFlag"]!=""){
		$param = array();
		$param["name"] = "RushRegisterAccountFlag";
		$param["value"] = $var["RushRegisterAccountFlag"];
		$ret=PostData($gas_host, $gas_port, "SetConfServer", $param);
	}
	
	if($var["MaxRushRoleNumPerServer"]!=null && $var["MaxRushRoleNumPerServer"]!="" && is_numeric($var["MaxRushRoleNumPerServer"])){
		$param = array();
		$param["name"] = "MaxRushRoleNumPerServer";
		$param["value"] = $var["MaxRushRoleNumPerServer"];
		$ret=PostData($gas_host, $gas_port, "SetConfServer", $param);
	}
	
	if($var["MaxRushRoleNumPerCamp"]!=null && $var["MaxRushRoleNumPerCamp"]!="" && is_numeric($var["MaxRushRoleNumPerCamp"])){
		$param = array();
		$param["name"] = "MaxRushRoleNumPerCamp";
		$param["value"] = $var["MaxRushRoleNumPerCamp"];
		$ret=PostData($gas_host, $gas_port, "SetConfServer", $param);
	}
	
	if($var["MaxRushRoleNumPerAccount"]!=null && $var["MaxRushRoleNumPerAccount"]!="" && is_numeric($var["MaxRushRoleNumPerAccount"])){
		$param = array();
		$param["name"] = "MaxRushRoleNumPerAccount";
		$param["value"] = $var["MaxRushRoleNumPerAccount"];
		$ret=PostData($gas_host, $gas_port, "SetConfServer", $param);
	}
	
	echo "<table class='info_table'>\n";
	
	sleep(3);
	
	$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'RushRegisterAccountFlag'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		echo "<tr><th>".$tbl_sys_msg["on"]."/".$tbl_sys_msg["off"]."</th><td>".(($row[0]==0)?$tbl_sys_msg["off"]:$tbl_sys_msg["on"])."</td></tr>\n";
	}
	
	$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerServer'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		echo "<tr><th>".$page_main["RoleNumPerServer"]."</th><td>".$row[0]."</td></tr>\n";
	}
	
	$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerCamp'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		echo "<tr><th>".$page_main["RoleNumPerCamp"]."</th><td>".$row[0]."</td></tr>\n";
	}
	
	$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerAccount'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		echo "<tr><th>".$page_main["RoleNumPerAccount"]."</th><td>".$row[0]."</td></tr>\n";
	}
	
	echo "</table>\n";
	
	return true;
	}
}

function RushRoleState($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_camp;
	if($var["server_id"] == null){
		global $tbl_gm_db, $tbl_game_server;
		$jsstr = <<<_jsstr
<script>
$(function(){
	var obj = $("#server_all");
	var checkbox_all_id = $(obj).attr("id");
	var action_id = checkbox_all_id.substring(0,checkbox_all_id.indexOf("_"));
	$(obj).change(function(){
		var state = $(obj).attr("checked");
		$("."+action_id+"_checkbox").attr("checked",state);
	});
	$("."+action_id+"_checkbox").change(function(){
		var state = $(obj).attr("checked");
		if(state){
			if($("."+action_id+"_checkbox:checked").length == $("."+action_id+"_checkbox").length){
				$("#"+action_id+"_all").attr("checked",true);
			}
		}else{
			$("#"+action_id+"_all").attr("checked",false);
		}
	});
	document.form1.action_name.value = "254";
});
</script>
_jsstr;
		echo $jsstr."\n";
		form($var, $gm);
		echo "<table class='form_table'>";
		echo "<tr><th>".$page_main["select_server"]."</th><td>";
		$qs = "select rrs_uServerId from tbl_rush_role_servers group by rrs_uServerId";
		$rs = MySQLQuery($qs, "GMS");
		echo "<input type='checkbox' id='server_all' onclick='checkall(this)'/><label for='all_server'>".$tbl_sys_msg["all_server"]."</label><br/>";
		while($row = mysql_fetch_row($rs)){
			$i = $row[0];
			echo "<input type='checkbox' class='server_checkbox' id='server_".$i."' name='server_id[]' value='".$i."' onclick='chooseServer(this)' /><label for='server_".$i."'>".$tbl_game_server[$i][2]."</label><br/>";
		}
		
		echo "</td></tr>\n";
		echo "</table>\n";
		echo "<table class='form_table'>\n";
		echo "<tr><td style=\"text-align:center\"><input type=\"submit\" value=\"".$tbl_sys_msg["submit"]."\" /></td></tr>\n";
		echo "</table>\n";
		echo "</form>\n";
		return true;
	}else{
		if($var["serverindex"]==null || $var["serverindex"]==""){
			$ret = eachServerFrame($var, $gm);
			return $ret;
		}
		$serverid = $var["serverindex"];
		echo "<table class='info_table'>\n";
		
		$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'RushRegisterAccountFlag'";
		$rs = MySQLQuery($qs, "Game", $serverid);
		if($row = mysql_fetch_row($rs)){
			echo "<tr><th>".$tbl_sys_msg["on"]."/".$tbl_sys_msg["off"]."</th><td>".(($row[0]==0)?$tbl_sys_msg["off"]:$tbl_sys_msg["on"])."</td></tr>\n";
		}
		
		$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerServer'";
		$rs = MySQLQuery($qs, "Game", $serverid);
		if($row = mysql_fetch_row($rs)){
			echo "<tr><th>".$page_main["RoleNumPerServer"]."</th><td>".$row[0]."</td></tr>\n";
		}
		
		$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerCamp'";
		$rs = MySQLQuery($qs, "Game", $serverid);
		if($row = mysql_fetch_row($rs)){
			echo "<tr><th>".$page_main["RoleNumPerCamp"]."</th><td>".$row[0]."</td></tr>\n";
		}
		
		$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'MaxRushRoleNumPerAccount'";
		$rs = MySQLQuery($qs, "Game", $serverid);
		if($row = mysql_fetch_row($rs)){
			echo "<tr><th>".$page_main["RoleNumPerAccount"]."</th><td>".$row[0]."</td></tr>\n";
		}
		
		$total = 0;
		$qs = "select cs.cs_uCamp, count(*) from tbl_char c,tbl_char_static cs where c.cs_uId = cs.cs_uId group by cs.cs_uCamp";
		$rs = MySQLQuery($qs, "Game", $serverid);
		while($row = mysql_fetch_row($rs)){
			echo "<tr><th>".$tbl_camp[$row[0]]."</th><td>".$row[1]."</td></tr>\n";
			$total += $row[1];
		}
		echo "<tr><th>".$tbl_sys_msg["total"]."</th><td>$total</td></tr>\n";
		
		echo "</table>\n";
		
		return true;
	}
}

function RushRoleLog($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_game_server;
	if($var["server_id"] == null){
		$jsstr = <<<_jsstr
<script>
$(function(){
	var obj = $("#server_all");
	var checkbox_all_id = $(obj).attr("id");
	var action_id = checkbox_all_id.substring(0,checkbox_all_id.indexOf("_"));
	$(obj).change(function(){
		var state = $(obj).attr("checked");
		$("."+action_id+"_checkbox").attr("checked",state);
	});
	$("."+action_id+"_checkbox").change(function(){
		var state = $(obj).attr("checked");
		if(state){
			if($("."+action_id+"_checkbox:checked").length == $("."+action_id+"_checkbox").length){
				$("#"+action_id+"_all").attr("checked",true);
			}
		}else{
			$("#"+action_id+"_all").attr("checked",false);
		}
	});
	document.form1.action_name.value = "285";
});
</script>
_jsstr;
		echo $jsstr."\n";
		echo "<script type=\"text/javascript\" src=\"/js/WebCalendar.js\"></script>\n";
		form($var, $gm);
		echo "<table class='form_table'>";
		echo "<tr><th>".$page_main["select_server"]."</th><td>";
		$qs = "select rrs_uServerId from tbl_rush_role_servers group by rrs_uServerId";
		$rs = MySQLQuery($qs, "GMS");
		echo "<input type='checkbox' id='server_all' onclick='checkall(this)'/><label for='all_server'>".$tbl_sys_msg["all_server"]."</label><br/>";
		while($row = mysql_fetch_row($rs)){
			$i = $row[0];
			echo "<input type='checkbox' class='server_checkbox' id='server_".$i."' name='server_id[]' value='".$i."' onclick='chooseServer(this)' /><label for='server_".$i."'>".$tbl_game_server[$i][2]."</label><br/>";
		}
		
		echo "</td></tr>\n";
		echo "<tr><th>".$page_main["begin_time"]."</th><td><input type='text' onclick=\"SelectDate(this,'yyyy-MM-dd hh:mm')\" id='t1' name='begin_time' />YYYY-MM-DD HH-mm-ss</td></tr>\n";
		echo "<tr><th>".$page_main["end_time"]."</th><td><input type='text' onclick=\"SelectDate(this,'yyyy-MM-dd hh:mm')\" id='t2' name='end_time' /></td></tr>\n";
		echo "</table>\n";
		echo "<table class='form_table'>\n";
		echo "<tr><td style=\"text-align:center\"><input type=\"submit\" value=\"".$tbl_sys_msg["submit"]."\" /></td></tr>\n";
		echo "</table>\n";
		echo "</form>\n";
		return true;
	}else{
		if($var["serverindex"]==null || $var["serverindex"]==""){
			$ret = eachServer($var, $gm, "RushRoleLogFun");
			return $ret;
		}
	}
}

function RushRoleLogFun($var, $gm, $serverid){
	global $tbl_camp;
	$str = "";
	if($var["begin_time"] != null && $var["begin_time"] != ""){
		$str = " and rrl_dtDateTime >= '".$var["begin_time"]."'";
	}
	if($var["end_time"] != null && $var["end_time"] != ""){
		$str = " and rrl_dtDateTime <= '".$var["end_time"]."'";
	}
	$qs = "select rrl_dtDateTime, rrl_uCamp1, rrl_uCamp2, rrl_uCamp3 from tbl_rush_role_log where rrl_uServerId = ".$serverid.$str." order by rrl_dtDateTime";
	$rs = MySQLQuery($qs, "GMS");
	$label1 = array($tbl_camp[1],$tbl_camp[2],$tbl_camp[3]);
	$label2 = array();
	$values = array();
	$values[0] = array();
	$values[1] = array();
	$values[2] = array();
	while($row = mysql_fetch_row($rs)){
		array_push($label2,(strtotime($row[0])+intval(date("O"))*360)*1000);
		array_push($values[0],$row[1]);
		array_push($values[1],$row[2]);
		array_push($values[2],$row[3]);
	}
	Chart_Line($label1,$label2,$values,"xaxes: [ { mode: 'time' } ], legend: { position: 'nw' }");
	return true;
}

function ShowPlayerLatency($var, $gm){
	LastLogTime();
	eachServer($var, $gm, "ShowPlayerLatency_fun");
	return true;
}

function ShowPlayerLatency_fun($var, $gm, $server_id){
	global $tbl_sys_msg,$page_main;
	
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th>%s</th></tr>\n",$page_main["Latency"],$page_main["number"]);
	
	$qs = "select sum(if(pl_uLatency <    50, 1, 0)), ";
	$qs.=        "sum(if(pl_uLatency >=   50 and pl_uLatency <  100, 1, 0)), ";
	$qs.=        "sum(if(pl_uLatency >=  100 and pl_uLatency <  200, 1, 0)), ";
	$qs.=        "sum(if(pl_uLatency >=  200 and pl_uLatency <  500, 1, 0)), ";
	$qs.=        "sum(if(pl_uLatency >=  500 and pl_uLatency < 1000, 1, 0)), ";
	$qs.=        "sum(if(pl_uLatency >= 1000, 1, 0)), ";
	$qs.=        "max(pl_uLatency) ";
	$qs.= "from tbl_log_player_latency";
	
	$str = "";
	if($var["begin_time"]<>"" || $var["end_time"]<>""){
		$str = " where ";
		if($var["begin_time"]<>""){
			$str .= "pl_tTime >= '".$var["begin_time"]."'";
		}
		if($var["begin_time"]<>"" && $var["end_time"]<>""){
			$str .= " and ";
		}
		if($var["end_time"]<>""){
			$str .= "pl_tTime <= '".$var["end_time"]."'";
		}
	}
	
	$qs.= $str;
	
	$result = MySQLQuery($qs, "Log", $server_id);
	$labels = array();
	$values = array();
	if($row=mysql_fetch_row($result)){	
		$labels[0] = "x&lt;50";
		$labels[1] = "50&lt;=x&lt;100";
		$labels[2] = "100&lt;=x&lt;200";
		$labels[3] = "200&lt;=x&lt;500";
		$labels[4] = "500&lt;=x&lt;1000";
		$labels[5] = "x&gt;1000";
		
		$values[0] = $row[0];
		$values[1] = $row[1];
		$values[2] = $row[2];
		$values[3] = $row[3];
		$values[4] = $row[4];
		$values[5] = $row[5];
		
		printf("<tr><td>%s</td><td>%s</td></tr>\n","x&lt;50",$row[0]);
		printf("<tr><td>%s</td><td>%s</td></tr>\n","50&lt;=x&lt;100",$row[1]);
		printf("<tr><td>%s</td><td>%s</td></tr>\n","100&lt;=x&lt;200",$row[2]);
		printf("<tr><td>%s</td><td>%s</td></tr>\n","200&lt;=x&lt;500",$row[3]);
		printf("<tr><td>%s</td><td>%s</td></tr>\n","500&lt;=x&lt;1000",$row[4]);
		printf("<tr><td>%s</td><td>%s</td></tr>\n","x&gt;1000",$row[5]);
		printf("<tr><td>%s</td><td>%s</td></tr>\n",$tbl_sys_msg["Max_Latency"],$row[6]);
		
	}
	echo "</table>\n";
	Chart_Pie($labels,$values);
	return true;
}

function ServerConfig($var, $gm){
	global $tbl_serv_db, $tbl_serv_group, $tbl_log_db, $tbl_game_server, $tbl_sys_msg, $tbl_serv_partition;
	if($var["flag"]==null){
		$serv_id = $var["serverid"];
		$str = <<<_str
<style>
	th {width: auto !important;}
</style>
<script>
function clickbutton(){
	document.form1.action_name.value = "259";
	document.form1.flag.value = "save";
	document.form1.submit();
}
</script>
_str;
		echo $str."\n";
		form($var, $gm);
		$partitions = "";
		foreach($tbl_serv_partition[$var["gameId"]] as $group_name => $group){
			$partitions.= sprintf("<option%s>%s</option>",((in_array($serv_id,$group))?" selected='selected'":""),$group_name);
		}
		
		global $default_server;
		$tbl_game_server[$serv_id][0] = ($tbl_game_server[$serv_id][0] != "") ? $tbl_game_server[$serv_id][0] : $default_server[0][0];
		$tbl_game_server[$serv_id][1] = ($tbl_game_server[$serv_id][1] != "") ? $tbl_game_server[$serv_id][1] : $default_server[0][1];
		$tbl_game_server[$serv_id][2] = ($tbl_game_server[$serv_id][2] != "") ? $tbl_game_server[$serv_id][2] : $default_server[0][2];
		$tbl_serv_db[$serv_id][0] = ($tbl_serv_db[$serv_id][0] != "") ? $tbl_serv_db[$serv_id][0] : $default_server[1][0];
		$tbl_serv_db[$serv_id][1] = ($tbl_serv_db[$serv_id][1] != "") ? $tbl_serv_db[$serv_id][1] : $default_server[1][1];
		$tbl_serv_db[$serv_id][2] = ($tbl_serv_db[$serv_id][2] != "") ? $tbl_serv_db[$serv_id][2] : $default_server[1][2];
		$tbl_serv_db[$serv_id][3] = ($tbl_serv_db[$serv_id][3] != "") ? $tbl_serv_db[$serv_id][3] : $default_server[1][3];
		$tbl_serv_db[$serv_id][4] = ($tbl_serv_db[$serv_id][4] != "") ? $tbl_serv_db[$serv_id][4] : $default_server[1][4];
		$tbl_log_db[$serv_id][0] = ($tbl_log_db[$serv_id][0] != "") ? $tbl_log_db[$serv_id][0] : $default_server[2][0];
		$tbl_log_db[$serv_id][1] = ($tbl_log_db[$serv_id][1] != "") ? $tbl_log_db[$serv_id][1] : $default_server[2][1];
		$tbl_log_db[$serv_id][2] = ($tbl_log_db[$serv_id][2] != "") ? $tbl_log_db[$serv_id][2] : $default_server[2][2];
		$tbl_log_db[$serv_id][3] = ($tbl_log_db[$serv_id][3] != "") ? $tbl_log_db[$serv_id][3] : $default_server[2][3];
		
		echo "<input type='hidden' name='flag'>\n";
		echo "<table class='info_table'>\n";
		printf("<tr><th rowspan='5'>Server</th>	<th>ID</th>									<td><input type='text' value='%s' name='server_id' /></td></tr>\n",$serv_id);
		printf("<tr>														<th>Name</th>								<td><input type='text' value='%s' name='tgs2' /></td></tr>\n",$tbl_game_server[$serv_id][2]);
		printf("<tr>														<th>Partition</th>					<td><select name='partition'>%s</select></td></tr>\n", $partitions);
		printf("<tr>														<th>IP</th>									<td><input type='text' value='%s' name='tgs0' /></td></tr>\n",$tbl_game_server[$serv_id][0]);
		printf("<tr>														<th>Web Port</th>						<td><input type='text' value='%s' name='tgs1' /></td></tr>\n",$tbl_game_server[$serv_id][1]);
		printf("<tr><th rowspan='5'>GameDB</th>	<th>User</th>								<td><input type='text' value='%s' name='sdb0' /></td></tr>\n",$tbl_serv_db[$serv_id][0]);
		printf("<tr>														<th>Password</th>						<td><input type='text' value='%s' name='sdb1' /></td></tr>\n",$tbl_serv_db[$serv_id][1]);
		printf("<tr>														<th>IP</th>									<td><input type='text' value='%s' name='sdb2' /></td></tr>\n",$tbl_serv_db[$serv_id][2]);
		printf("<tr>														<th>Main Database Name</th>	<td><input type='text' value='%s' name='sdb3' /></td></tr>\n",$tbl_serv_db[$serv_id][3]);
		printf("<tr>														<th>Log Database Name</th>	<td><input type='text' value='%s' name='sdb4' /></td></tr>\n",$tbl_serv_db[$serv_id][4]);
		printf("<tr><th rowspan='4'>LogDB</th>	<th>User</th>								<td><input type='text' value='%s' name='ldb0' /></td></tr>\n",$tbl_log_db[$serv_id][0]);
		printf("<tr>														<th>Password</th>						<td><input type='text' value='%s' name='ldb1' /></td></tr>\n",$tbl_log_db[$serv_id][1]);
		printf("<tr>														<th>IP</th>									<td><input type='text' value='%s' name='ldb2' /></td></tr>\n",$tbl_log_db[$serv_id][2]);
		printf("<tr>														<th>Database Name</th>			<td><input type='text' value='%s' name='ldb3' /></td></tr>\n",$tbl_log_db[$serv_id][3]);
		echo "<tr><td colspan='3' style='text-align:center;'><button onclick='clickbutton()'>".$tbl_sys_msg["submit"]."</button></td></tr>\n";
		echo "</table>\n";
		echo "</form>\n";
		return true;
	}
	if($var["flag"]=="delete"){
		$old = $var["serverid"];
		$str = <<<_str
<script>
function clickbutton(){
	document.form1.action_name.value = "259.1";
	document.form1.flag.value = "deleted";
	document.form1.submit();
}
</script>
_str;
		echo $str."\n";
		form($var, $gm);
		echo "<input type='hidden' name='flag'>\n";
		echo "</form>\n";
		PrintNotice(sprintf($tbl_sys_msg["delete_sure"],"[".$tbl_game_server[$old][2]."]")."<br/><button onclick='clickbutton()'>".$tbl_sys_msg["submit"]."</button>");
		return true;
	}
	if($var["flag"]=="deleted"){
		$old = $var["serverid"];
		foreach($tbl_serv_partition[$var["gameId"]] as $group_name => $group){
			$tbl_serv_partition[$var["gameId"]][$group_name] = array_diff($group, array($old));
		}
		//$tbl_serv_group[$var["gameId"]] = array_diff($tbl_serv_group[$var["gameId"]], array($old));
		unset($tbl_game_server[$old]);
		unset($tbl_serv_db[$old]);
		unset($tbl_log_db[$old]);
	}
	if($var["flag"]=="save"){
		$old = $var["serverid"];
		$ser = $var["server_id"];
		if($old != $ser){
			if($old!=""){
				//$tbl_serv_group[$var["gameId"]] = array_diff($tbl_serv_group[$var["gameId"]], array($old));
				unset($tbl_game_server[$old]);
				unset($tbl_serv_db[$old]);
				unset($tbl_log_db[$old]);
			}
			if($tbl_serv_partition[$var["gameId"]] == null) $tbl_serv_partition[$var["gameId"]] = array();
		}
		foreach($tbl_serv_partition[$var["gameId"]] as $group_name => $group){
			$tbl_serv_partition[$var["gameId"]][$group_name] = array_diff($group, array($old));
		}
		if($tbl_serv_partition[$var["gameId"]][$var["partition"]] == null) $tbl_serv_partition[$var["gameId"]][$var["partition"]] = array();
		array_push($tbl_serv_partition[$var["gameId"]][$var["partition"]], $ser);
		$tbl_serv_partition[$var["gameId"]][$var["partition"]] = array_unique($tbl_serv_partition[$var["gameId"]][$var["partition"]]);
		sort($tbl_serv_partition[$var["gameId"]][$var["partition"]]);
		$tbl_game_server[$ser] = array($var["tgs0"],$var["tgs1"],$var["tgs2"]);
		$tbl_serv_db[$ser] = array($var["sdb0"],$var["sdb1"],$var["sdb2"],$var["sdb3"],$var["sdb4"]);
		$tbl_log_db[$ser] = array($var["ldb0"],$var["ldb1"],$var["ldb2"],$var["ldb3"]);
		ksort($tbl_game_server);
		ksort($tbl_serv_db);
		ksort($tbl_log_db);
	}
	
	$ret = changeServerListFile();
	
	if($ret){
		PrintNotice($tbl_sys_msg["success"]);
		return true;
	}else{
		return false;
	}
}

function changeServerListFile(){
	global $tbl_serv_db, $tbl_serv_group, $tbl_log_db, $tbl_game_server, $tbl_sys_msg, $tbl_serv_partition;
	$file = '<?php'."\n";
	$file.= '$tbl_serv_partition = array('."\n";
	foreach($tbl_serv_partition as $game_id => $groups){
		$file.= "\t\"".$game_id."\"\t=>\tarray(\n";
		foreach($groups as $group_name => $group){
			$file.= "\t\t\"".$group_name."\"\t=>\tarray(".implode(",",$group)."),\n";
		}
		$file.= "\t),\n";
	}
	$file.= ");\n";
	
	$file.= '$tbl_serv_db = array('."\n";
	foreach($tbl_serv_db as $serv_id => $array){
		$file.= "\t".$serv_id."\t=>\tarray(\"".$array[0]."\",\"".$array[1]."\",\"".$array[2]."\",\"".$array[3]."\",\"".$array[4]."\"),\n";
	}
	$file.= ");\n";
	
	$file.= '$tbl_log_db = array('."\n";
	foreach($tbl_log_db as $serv_id => $array){
		$file.= "\t".$serv_id."\t=>\tarray(\"".$array[0]."\",\"".$array[1]."\",\"".$array[2]."\",\"".$array[3]."\"),\n";
	}
	$file.= ");\n";
	
	$file.= '$tbl_game_server = array('."\n";
	foreach($tbl_game_server as $serv_id => $array){
		$file.= "\t".$serv_id."\t=>\tarray(\"".$array[0]."\",\"".$array[1]."\",\"".$array[2]."\"),\n";
	}
	$file.= ");\n";
	$file.= '?>';
	
	$f = fopen("../../conf/gm_tools/ServerList.php","w");
	$ret = fputs($f,$file);
	fclose($f);
	return $ret;
}

function RushRoleServerConfig($var, $gm){
	global $page_main,$tbl_game_server,$tbl_sys_msg;
	
	MySQLQuery("delete from tbl_rush_role_servers where rrs_dEndDate < curdate()", "GMS");
	
	if($var["server_id"] != null){
		$date = $var["date"];
		foreach($var["server_id"] as $serv_id){
			MySQLQuery("insert into tbl_rush_role_servers(`rrs_uServerId`,`rrs_dEndDate`) values($serv_id,'$date')", "GMS");
		}
	}
	if($var["rrsid"] != null){
		MySQLQuery("delete from tbl_rush_role_servers where rrs_uId = ".$var["rrsid"], "GMS");
	}
	
	$jsstr = <<<_jsstr
<script>
function del(id){
	document.form1.action_name.value = "260.2";
	document.form1.rrsid.value = id;
	document.form1.submit();
}
</script>
_jsstr;
	echo $jsstr."\n";
	
	form($var, $gm);
	echo "<input type='hidden' name='rrsid' />\n";
	echo "</form>\n";
	
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",$page_main["select_server"],$page_main["end_time"],$tbl_sys_msg["act"]);
	$qs = "select * from tbl_rush_role_servers";
	$rs = MySQLQuery($qs, "GMS");
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td align=\"center\"><a href=\"#\" onclick=\"del(%s);\" >%s</a></td></tr>\n",$tbl_game_server[$row[1]][2],$row[2],$row[0],$tbl_sys_msg["delete"]);
	}
	echo "</table>\n";
	return true;
}

function ShowServerType($var, $gm){
	global $tbl_serv_partition,$tbl_game_server,$page_main,$tbl_server_type;
	foreach($tbl_serv_partition[$var["gameId"]] as $group_name => $group){
		echo "<h2>".$group_name."</h2>";
		echo "<table class='list_table'>\n";
		printf("<tr><th>%s</th><th>%s</th></tr>\n",$page_main["select_server"],$page_main["type"]);
		foreach($group as $serv){
			$qs = "select sc_sVarValue from tbl_conf_server where sc_sVarName = 'ServerType'";
			$rs = MySQLQuery($qs, "Game", $serv);
			if($row = mysql_fetch_row($rs)){
				$type = $row[0];
			}else{
				$type = 0;
			}
			printf("<tr><td>%s</td><td>%s</td></tr>\n",$tbl_game_server[$serv][2],$tbl_server_type[$type]);
		}
		echo "</table>\n";
	}
	return true;
}
function SetServerType($var, $gm){
	global $tbl_game_server,$tbl_sys_msg,$tbl_error_msg;
	$servers = $var["server_id"];
	$type = $var["type"];
	foreach($servers as $server){
		echo "<h2>".$tbl_game_server[$server][2]."</h2>\n";
		$gas_host = $tbl_game_server[$server][0];
		$gas_port = $tbl_game_server[$server][1];
	
		$param = array();
		$param["name"] = "ServerType";
		$param["value"] = $type;
		$ret=PostData($gas_host, $gas_port, "SetConfServer", $param);
		
		if ($ret == 200){
			PrintNotice($tbl_sys_msg["success"]);
			$action = array("type" => $type);
			LogGMCommand($gm, $var["action_name"], $action, $server);
		}else{
			PrintNotice($tbl_error_msg["ActionFail"]);
		}
	}
	return true;
}

function JointSales_ItemList($var, $gm){
	global $page_main,$tbl_sys_msg,$tbl_joint_sales_status,$page_action_name;
	
	$js_str = <<<_js_str
<script>
function toEdit(id){
	document.form1.action_name.value = "267";
	document.form1.id.value = id;
	document.form1.flag.value = "edit";
	document.form1.submit();
}
function toDelete(id){
	document.form1.action_name.value = "267";
	document.form1.id.value = id;
	document.form1.flag.value = "delete";
	document.form1.submit();
}
function toStatus(id,statue){
	document.form1.action_name.value = "271";
	document.form1.id.value = id;
	document.form1.flag.value = statue;
	document.form1.submit();
}
function toSN(id){
	document.form1.action_name.value = "268";
	document.form1.id.value = id;
	document.form1.submit();
}
function toLog(id){
	document.form1.action_name.value = "270";
	document.form1.id.value = id;
	document.form1.submit();
}
</script>
_js_str;
	
	echo $js_str."\n";
	form($var,$gm);
	echo "<input type='hidden' name='id' />\n";
	echo "<input type='hidden' name='flag' />\n";
	echo "</form>\n";
	
	$qs = "select jsi.jsi_sItemName, jsi.jsi_uPrice, jsi.jsi_uState, jsi_uId, jsi_uSmallIcon, jsi_sTooltips from tbl_joint_sales_items jsi";
	$rs = MySQLQuery($qs, "GMS");
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$page_main["item"],"SmallIcon",$page_main["price"],$page_main["number"],$tbl_sys_msg["status"],$tbl_sys_msg["detail"],$page_action_name["268"],$page_action_name["270"],$tbl_sys_msg["act"]);
	while($row = mysql_fetch_row($rs)){
		$qs = "select count(*) from tbl_joint_sales_sn where jsi_uId = ".$row[3];
		$result = MySQLQuery($qs, "GMS");
		if($r = mysql_fetch_row($result)){
			$count = $r[0];
		}
		if($row[2]==0){
			$str = "<button onclick='toStatus(".$row[3].",1);'>".$tbl_joint_sales_status[1]."</button><button onclick='toEdit(".$row[3].");'>".$tbl_sys_msg["edit"]."</button><button onclick='toDelete(".$row[3].");'>".$tbl_sys_msg["delete"]."</button>";
		}else{
			$str = "<button onclick='toStatus(".$row[3].",0);'>".$tbl_joint_sales_status[0]."</button>";
		}
		printf("<tr><td>%s</td><td>%d</td><td>%s</td><td>%d</td><td>%s</td><td><button onclick='toDetal(%d);'>%s</button></td><td><button onclick='toSN(%d);'>%s</button></td><td><button onclick='toLog(%d);'>%s</button></td><td>%s</td></tr>\n",$row[0],$row[4],money_to_string($row[1]),$count,$tbl_joint_sales_status[$row[2]],$row[3],$tbl_sys_msg["detail"],$row[3],$page_action_name["268"],$row[3],$page_action_name["270"],$str);
		printf("<tr><td colspan='9'>%s</td></tr>\n",$row[5]);
	}
	echo "</table>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><td><button onclick='toEdit(0);'>".$tbl_sys_msg["insert"]."</button></td></tr>\n";
	echo "</table>\n";
	return true;
}

function JointSales_ItemEdit($var, $gm){
	global $tbl_sys_msg,$page_main;
	$flag = $var["flag"];
	if($flag == "edit"){
		if($var["id"] != 0){
			$qs = "select * from tbl_joint_sales_items where jsi_uId = ".$var["id"];
			$rs = MySQLQuery($qs, "GMS");
			$row = mysql_fetch_row($rs);
		}
		
		$js_str = <<<_js_str
<script>
function toSubmit(){
	document.form1.action_name.value = "267";
	document.form1.submit();
}
</script>
_js_str;
	
		echo $js_str."\n";
		form($var, $gm);
		echo "<input type='hidden' name='jsi_uId' value='".$row[0]."' />\n";
		echo "<table class='form_table'>\n";
		echo "<tr><th>".$page_main["item"]."</th><td><input type='text' name='jsi_sItemName' value='".$row[2]."' /></td></tr>\n";
		echo "<tr><th>".$tbl_sys_msg["desc"]."</th><td><textarea name='jsi_sTooltips'>".$row[3]."</textarea></td></tr>\n";
		echo "<tr><th>SmallIcon</th><td><input type='text' name='jsi_uSmallIcon' value='".$row[4]."' /></td></tr>\n";
		echo "<tr><th>URL</th><td><input type='text' name='jsi_sURL' value='".$row[5]."' /></td></tr>\n";
		$money = $row[6];
		echo "<tr><th>".$page_main["price"]."</th><td><input type='text' name='m1' style='width:60px; text-align:right;' value='".floor($money/10000)."' />".$tbl_sys_msg["gold"]."<input type='text' name='m2' style='width:40px; text-align:right;'value='".(($money/100)%100)."' />".$tbl_sys_msg["silver"]."<input type='text' name='m3' style='width:40px; text-align:right;'value='".($money%100)."' />".$tbl_sys_msg["copper"]."</td></tr>\n";
		echo "</table>\n";
		echo "<table class='form_table'>\n";
		echo "<tr><td style='text-align:center;'><button onclick='toSubmit();'>".$tbl_sys_msg["submit"]."</button></td></tr>\n";
		echo "</table>\n";
		echo "</form>\n";
		return true;
	}else if($flag == "delete"){
		$rs = MySQLQuery("delete from tbl_joint_sales_items where jsi_uId = ".$var["id"], "GMS");
		if(!$rs){
			return false;
		}
		PrintNotice($tbl_sys_msg["success"]);
		return true;
	}else{
		$rs = MySQLQuery("REPLACE INTO `tbl_joint_sales_items`(".(($var["jsi_uId"]=="")?"":"`jsi_uId`,")."`jsi_sItemName`,`jsi_sTooltips`,`jsi_uSmallIcon`,`jsi_sURL`,`jsi_uPrice`) values(".$var["jsi_uId"].(($var["jsi_uId"]=="")?"":",")."'".$var["jsi_sItemName"]."','".$var["jsi_sTooltips"]."',".$var["jsi_uSmallIcon"].",'".$var["jsi_sURL"]."',".(intval($var["m1"])*10000+intval($var["m2"])*100+intval($var["m3"])).")", "GMS");
		if(!$rs){
			return false;
		}
		PrintNotice($tbl_sys_msg["success"]);
		return true;
	}
}

function JointSales_ChangeState($var, $gm){
	global $tbl_sys_msg;
	$id = $var["id"];
	$status = $var["flag"];
	
	$qs = "update `tbl_joint_sales_items` set `jsi_uState` = '".$status."' where `jsi_uId` = ".$id;
	$rs = MySQLQuery($qs, "GMS");
	if(!$rs){
		return false;
	}
	PrintNotice($tbl_sys_msg["success"]);
	UpdateJointSalesList();
	return true;
}

function JointSales_InsertSN($var, $gm){
	global $tbl_sys_msg;
	if($var["SN"]==""){
		$js_str = <<<_js_str
<script>
function toSubmit(){
	document.form1.action_name.value = "268";
	document.form1.submit();
}
</script>
_js_str;
		echo $js_str."\n";
		
		form($var, $gm);
		echo "<input type='hidden' name='id' value='".$var["id"]."' />\n";
		echo "<table class='form_table'>\n";
		echo "<tr><th>SN</th><td><textarea name='SN' style='height: 400px;'></textarea></td></tr>\n";
		echo "</table>\n";
		echo "<table class='form_table'>\n";
		echo "<tr><td style='text-align:center;'><button onclick='toSubmit();'>".$tbl_sys_msg["submit"]."</button></td></tr>\n";
		echo "</table>\n";
		
		echo "</form>\n";
		return true;
	}else{
		$id = $var["id"];
		
		$SN = explode("\n",$var["SN"]);
		foreach($SN as $item){
			$item = str_replace("\r","",$item);
			$item = str_replace(" ","",$item);
			if($item == "") continue;
			MySQLQuery("insert into tbl_joint_sales_sn values(".$id.",'".$item."')", "GMS");
		}
		return true;
	}
}

function JointSales_ItemLog($var, $gm){
	$id = $var["id"];
	global $tbl_sys_msg,$page_main,$tbl_game_server;
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n","SN",$page_main["select_server"],$page_main["user_name"],$page_main["role_name"],$tbl_sys_msg["time"]);
	
	$qs = "select jss_sSN, jsl_uServerId, jsl_uUser, jsl_uChar, jsl_dtDateTime from tbl_joint_sales_log where jsi_uId = ".$id;
	$rs = MySQLQuery($qs, "GMS");
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_game_server[$row[1]][2],$row[2],$row[3],$row[4]);
	}
	
	echo "</table>\n";
	return true;
}

function SetRecruitMethod($var, $gm){
	global $tbl_game_server;
	$param = array();
	if($var["camp1"] == null || $var["camp1"] == ""){
		$param["method"] = $var["method"];
		$param["camp1"] = 0;
		$param["camp2"] = 0;
		$param["camp3"] = 0;
		$action = array("type" => $var["method"]);
	}else{
		$param["method"] = 2;
		$param["camp1"] = $var["camp1"];
		$param["camp2"] = $var["camp2"];
		$param["camp3"] = $var["camp3"];
		$action = array("camp" => $var["camp1"]."|".$var["camp2"]."|".$var["camp3"]);
	}
	$servers = $var["server_id"];
	foreach($servers as $server){
		$gas_host = $tbl_game_server[$server][0];
		$gas_port = $tbl_game_server[$server][1];
		
		$ret=PostData($gas_host, $gas_port, "SetRecruitMethod", $param);
		echo "<h2>".$tbl_game_server[$server][2]."</h2>\n";
		if($ret==200){
			PrintNotice(LogInfo($action,true));
			LogGMCommand($gm, $var["action_name"], $action, $server);
		}else{
			PrintNotice(LogInfo($action,false,$ret));
		}
	}
	return true;
}

function SetRecruitInfo($var, $gm){
	global $tbl_game_server;
	$param = array();
	$param["minNum"] = $var["minNum"];
	$param["maxNum"] = $var["maxNum"];
	$param["time"] = $var["time"];
	$action = array("minNum" => $var["minNum"], "maxNum" => $var["maxNum"], "time" => $var["time"]);
	$servers = $var["server_id"];
	foreach($servers as $server){
		$gas_host = $tbl_game_server[$server][0];
		$gas_port = $tbl_game_server[$server][1];
		
		$ret=PostData($gas_host, $gas_port, "SetRecruitInfo", $param);
		echo "<h2>".$tbl_game_server[$server][2]."</h2>\n";
		if($ret==200){
			PrintNotice(LogInfo($action,true));
			LogGMCommand($gm, $var["action_name"], $action, $server);
		}else{
			PrintNotice(LogInfo($action,false,$ret));
		}
	}
	return true;
}

function GetRecruitSetting($var, $gm){
	global $tbl_recruit,$tbl_sys_msg,$page_main,$tbl_camp,$tbl_game_server;
	$servers = $var["server_id"];
	foreach($servers as $server){
		echo "<h2>".$tbl_game_server[$server][2]."</h2>\n";
		$qs = "select ri_uMinNum, ri_uMaxNum, ri_uTime from tbl_recruit_info";
		$rs = MySQLQuery($qs, "Game", $server);
		if($row = mysql_fetch_row($rs)){
			echo "<table class='list_table'>\n";
			printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_recruit[0],$tbl_recruit[1],$tbl_sys_msg["time"]);
			printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$row[1],$row[2]);
			echo "</table>\n";
		}
		
		$camp = array();
		$qs = "select rc_uCamp, rc_uState from tbl_recruit_camp";
		$rs = MySQLQuery($qs, "Game", $server);
		while($row = mysql_fetch_row($rs)){
			$camp[$row[0]] = $row[1];
		}
		echo "<table class='list_table'>\n";
		printf("<tr><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["camp"],$page_main["type"]);
		foreach($camp as $c => $v){
			$type = "";
			if($v == 0){
				$type = $tbl_sys_msg["off"];
			}else{
				$type = $tbl_recruit[intval($v)-1];
			}
			printf("<tr><td>%s</td><td>%s</td></tr>\n",$tbl_camp[$c],$type);
		}
		echo "</table>\n";
	}
	return true;
}

function SetMaxOnLineUserNum($var, $gm){
	global $tbl_game_server;
	$servers = $var["server_id"];
	
	$param = array();
	$param["type"] = $var["type"];
	$param["newMaxOnLineUserNum"] = $var["newMaxOnLineUserNum"];
	$param["user_name"] = $var["user_name"];
	$param["game_id"] = $var["game_id"];
	
	$action = array("type" => $var["type"], "number" => $var["newMaxOnLineUserNum"], "user_name" => $var["user_name"], "game id" => $var["game_id"]);
	foreach($servers as $server){
		$gas_host = $tbl_game_server[$server][0];
		$gas_port = $tbl_game_server[$server][1];
		
		$ret=PostData($gas_host, $gas_port, "SetMaxOnLineUserNum", $param);
		echo "<h2>".$tbl_game_server[$server][2]."</h2>\n";
		if($ret==200){
			PrintNotice(LogInfo($action,true));
			LogGMCommand($gm, $var["action_name"], $action, $server);
		}else{
			PrintNotice(LogInfo($action,false,$ret));
		}
	}
	return true;
}

function ServerPartition($var, $gm){
	global $tbl_sys_msg,$tbl_serv_partition,$tbl_error_msg,$tbl_sys_msg;
	if($var["flag"] == "edit"){
		$name = explode("|",$var["name"]);
		if(is_array($tbl_serv_partition[$var["gameId"]][$name[1]])){
			printf($tbl_error_msg["PostDataFail"],"the partition with new name already exist.");
		}else if(is_array($tbl_serv_partition[$var["gameId"]][$name[0]])){
			$tbl_serv_partition[$var["gameId"]][$name[1]] = $tbl_serv_partition[$var["gameId"]][$name[0]];
			unset($tbl_serv_partition[$var["gameId"]][$name[0]]);
			changeServerListFile();
		}else{
			printf($tbl_error_msg["PostDataFail"],"the partition you selected don't exist.");
		}
	}else if($var["flag"] == "new"){
		if($tbl_serv_partition[$var["gameId"]][$var["name"]] == null){
			$tbl_serv_partition[$var["gameId"]][$var["name"]] = array();
			changeServerListFile();
		}else{
			printf($tbl_error_msg["PostDataFail"],"the partition with new name already exist.");
		}
	}else if($var["flag"] == "delete"){
		if(is_array($tbl_serv_partition[$var["gameId"]][$var["name"]]) && count($tbl_serv_partition[$var["gameId"]][$var["name"]])==0){
			unset($tbl_serv_partition[$var["gameId"]][$var["name"]]);
			changeServerListFile();
		}else{
			printf($tbl_error_msg["PostDataFail"],"the partition you selected don't exist or can't be deleted.");
		}
	}
	form($var, $gm);
	echo "<input type='hidden' name='flag' />\n";
	echo "<input type='hidden' name='name' />\n";
	echo "</form>\n";
	$js_str = <<<_js_str
<script>
function e(name){
	document.form1.action_name.value = "284.2";
	var new_name = window.prompt("$tbl_sys_msg[new_name]", name);
	document.form1.flag.value = "edit";
	document.form1.name.value = name + "|" + new_name;
	document.form1.submit();
}
function d(name){
	var b = window.confirm("$tbl_sys_msg[delete_sure]");
	if(b){
		document.form1.action_name.value = "284.3";
		document.form1.flag.value = "delete";
		document.form1.name.value = name;
		document.form1.submit();
	}
}
function n(){
	var name = window.prompt("$tbl_sys_msg[new_name]", "");
	if(name){
		document.form1.action_name.value = "284.1";
		document.form1.flag.value = "new";
		document.form1.name.value = name;
		document.form1.submit();
	}
}
</script>
_js_str;
	echo $js_str."\n";
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th></tr>\n", "", $tbl_sys_msg["act"]);
	foreach($tbl_serv_partition[$var["gameId"]] as $group_name => $group){
		printf("<tr><td>%1\$s</td><td><button onclick=\"e('%1\$s')\">%2\$s</button><button onclick=\"d('%1\$s')\">%3\$s</button></td></tr>\n",$group_name,$tbl_sys_msg["edit"],$tbl_sys_msg["delete"]);
	}
	echo "</table>";
	echo "<table class='list_table'><tr><td><button onclick='n()'>".$tbl_sys_msg["insert"]."</button</td></tr></table>\n";
	return true;
}

function SetSearchServer($var, $gm){
	global $tbl_game_server,$tbl_sys_msg,$page_main;
	if($var["server"]==null || $var["server"]==""){
		$js_str = <<<_js_str
<script>
function sbmt(){
	if(document.form1.server.value == 0){
		return false;
	}
	document.form1.action_name.value = '291';
}
</script>
_js_str;
		echo $js_str;
		
		$qs = "SELECT `gv_value` FROM `tbl_gm_variable` WHERE `gv_name` = 'SearchServer'";
		$rs = MySQLQuery($qs, "GMS");
		if($row = mysql_fetch_row($rs)){
			$server = $row[0];
		}
		
		form($var, $gm);
		echo "<table class='form_table'>\n";
		echo "<tr>\n\t<th>".$tbl_sys_msg["server"]."</th>\n\t<td>\n\t\t<select name='server'>\n";
		printf("\t\t\t<option value='%s'>%s</option>\n",0, $page_main["select_action"]);
		foreach($tbl_game_server as $key => $val){
			printf("\t\t\t<option value='%s'%s>%s</option>\n",$key, ($key == $server)?" selected='selected'":"", $val[2]);
		}
		echo "\t\t</select>\n\t\t<button onclick='return sbmt();'>".$tbl_sys_msg["submit"]."</button>\n\t</td>\n</tr>\n";
		echo "</table>\n";
		echo "</form>\n";
	}else{
		$qs = "REPLACE INTO `tbl_gm_variable` VALUES('SearchServer','".$var["server"]."')";
		$rs = MySQLQuery($qs, "GMS");
		if($rs){
			$action = array("server" => $var["server"]);
			LogGMCommand($gm, $var["action_name"], $action, 0);
			PrintNotice(LogInfo($action,true));
		}else{
			return false;
		}
	}
	return true;
}
?>
