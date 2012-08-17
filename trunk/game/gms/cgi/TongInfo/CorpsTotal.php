<?php
function RetTongInfo($tbl_content, $var, $page_str, $serverid)
{
	global $tbl_camp;
	$tong_count = count($tbl_content);
	global $tbl_list_tong, $tbl_sys_msg;
	$str=$str.sprintf("<table class='list_table'>\n");
	$str=$str.sprintf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th style='width:140px;'>%s</th><th style='width:80px;'>%s</th></tr>\n",$tbl_list_tong[1][0],$tbl_list_tong[1][1],$tbl_list_tong[1][2],$tbl_list_tong[1][3],$tbl_list_tong[1][4],$tbl_list_tong[1][5],$tbl_sys_msg["act"],$tbl_list_tong[1][6]);

	for($i=0;$i<$tong_count;$i++)
	{
		$func_str=          sprintf("<input type=button value='%s' onclick='OnDetail(this,%d)'/>"   ,$tbl_list_tong[2][0],$tbl_content[$i][6]);
		$func_str=$func_str.sprintf("<input type=button value='%s' onclick='OnMember(this,%d)'/>"   ,$tbl_list_tong[2][1],$tbl_content[$i][6]);
		$func_str=$func_str.sprintf("<input type=button value='%s' onclick='OnDepot(this,%d)'/>"    ,$tbl_list_tong[2][2],$tbl_content[$i][7]);
		$func_str=$func_str.sprintf("<input type=button value='%s' onclick='OnRename(this,%d)'/>"   ,$tbl_list_tong[2][3],$tbl_content[$i][6]);
		$func_str=$func_str.sprintf("<input type=button value='%s' onclick='OnLevel(this,%d)'/>"    ,$tbl_list_tong[2][4],$tbl_content[$i][6]);
		$func_str1=sprintf("<input type=button value='%s' onclick='OnMemberLog(this,%d)'/>"         ,$tbl_list_tong[3][0],$tbl_content[$i][6]);
		$func_str1=$func_str1.sprintf("<input type=button value='%s' onclick='OnItemLog(this,%d)'/>",$tbl_list_tong[3][1],$tbl_content[$i][6]);
		$level_str = "<select>";
		for($l=14;$l>0;$l--){
			$level_str.= sprintf("<option value='%d'".((intval($tbl_content[$i][5])==$l)?" selected='selected'":"").">%s</option>",$l,$l);
		}
		$level_str.= "</select>";
		$tbl_cap = $tbl_tong_owner[$tbl_content[$i][0]];
		$tong_cap = "";
		$str=$str=$str.sprintf("<tr".(($i%2==0)?"":" class='even'")."><td><input type='checkbox' name='tong[]' value='%s_%s'/>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",$serverid,$tbl_content[$i][0],$tbl_content[$i][0],$tbl_content[$i][1],RoleFunction($tbl_content[$i][2],$serverid),$tbl_camp[$tbl_content[$i][3]],$tbl_content[$i][4],$level_str, $func_str, $func_str1);
	}

	$str=$str.sprintf("</table>");
	$str=$str.$page_str;
	echo $str;
}
function GetTongInfo($var, $gm)
{
	if($var["serverindex"]==null || $var["serverindex"]==""){
		global $tbl_list_tong,$tbl_sys_msg,$tbl_tong_page,$tbl_sys_msg;
		$js_src=<<<_js_src
<script type='text/javascript' src='/js/checkbox.js'></script>
<script type='text/javascript' src='/js/page.js'></script>
<script type="text/javascript">
function OnDetail(obj,id){
	document.form1.target = '_self';
	document.form1.action_name.value = '65';
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function OnMember(obj,id){
	document.form1.target = '_self';
	document.form1.action_name.value = '64';
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function OnDepot(obj,id){
	document.form1.target = '_self';
	document.form1.action_name.value = '178';
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function OnRename(obj,id){
	var new_name = prompt("%s","");
	if(new_name==null) return;
	document.form1.new_name.value = new_name;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.target = "block";
	document.form1.action_name.value = "169";
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function OnLevel(obj,id){
	document.form1.target="block";
	var level = $(obj).parents("tr").find("select").val();
	document.form1.new_name.value = level;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.action_name.value = '172';
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function OnMemberLog(obj,id){
	document.form1.target = '_self';
	document.form1.action_name.value = '73';
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function OnItemLog(obj,id){
	document.form1.target = '_self';
	document.form1.action_name.value = '74';
	document.form1.id.value = id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
</script>
_js_src;
		printf($js_src."\n",$tbl_list_tong[4][0],$tbl_sys_msg["reason"],$tbl_sys_msg["reason"],$tbl_sys_msg["reason"]);
		echo "<h1>".$tbl_list_tong[0]."</h1>\n";
		form($var, $gm);
		echo "<input type='hidden' name='id' />";
		echo "<input type='hidden' name='new_name' />";
		echo "<input type='hidden' name='reason' />";
		echo "<input type='hidden' name='role_name' />";
		checkbox(false, true);
		$level_str = "<select name='uPage'>";
		for($l=1;$l<count($tbl_tong_page);$l++){
			$level_str.= sprintf("<option value='%d'>%s</option>",$l,$tbl_tong_page[$l]);
		}
		$level_str.= "</select>";
		ReturnAndBack();
		$var["str"] = UTF82Base64($var["str"]);
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		echo "</form>\n";
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}
	
	$qs = "";
	if($var["str"]!=null && $var["str"]!=""){
		$qs = " and t.t_sName like '%".Base642UTF8($var["str"])."%'";
	}
	
	global $tbl_list_tong;
	$query = "select t.t_sName, count(m.cs_uId), c.c_sName, t.t_uCamp,t.t_uHonor, t.t_uLevel, t.t_uId, t.t_uDepotID from tbl_tong t, tbl_member_tong m, tbl_char c, tbl_member_tong o where t.t_uId = o.t_uId and o.mt_sPosition=5 and o.cs_uId=c.cs_uId and t.t_uId=m.t_uId".$qs." group by t.t_uId order by t.t_uHonor desc";
	page($var,$query,63,"Game",$serverid,$result,$str,$desc,"_TongInfo",$tbl_list_tong[0],array($tbl_list_tong[1][0],$tbl_list_tong[1][1],$tbl_list_tong[1][2],$tbl_list_tong[1][3],$tbl_list_tong[1][4],$tbl_list_tong[1][5]));
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	echo $desc;
	RetTongInfo($tbl_content,$var,$str,$serverid);
	return true;
}

function _TongInfo($row){
	$tbl = $row;
	global $tbl_camp;
	$tbl[3] = $tbl_camp[$row[3]];
	$tbl[5] = $row[5];
	return $tbl;
}

function GetTongDepot($var, $gm){
	global $tbl_tong_depot,$bid2type,$tbl_tong_page,$tbl_sys_msg;
	$id = intval($var["id"]);
	$serverid=intval($var["serverid"]);
	
	if($var["page"]==null || $var["page"]==""){
		$js_str = <<<_js_str
<script type='text/javascript' src='/js/page.js'></script>
<script type='text/javascript'>
function del(i_type, i_name, i_num,i_page,i_pos){
	document.form1.target = "block";
	document.form1.item_name.value = i_type + "_" + i_name;
	document.form1.num.value = i_num;
	document.form1.uPage.value = i_page;
	document.form1.pos.value = i_pos;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.action_name.value = "79";
	document.form1.submit();
}
</script>
_js_str;
		echo sprintf($js_str,$tbl_sys_msg["reason"])."\n";
		echo "<h1>".$tbl_tong_depot[0]."</h1>\n";
		ReturnAndBack();
		form($var,$gm);
		
		$row = mysql_fetch_row(MySQLQuery("select t_sName from tbl_tong where t_uId = $id","Game",$serverid));
		
		echo "<input type='hidden' name='tong_name' value='".$row[0]."' />\n";
		echo "<input type='hidden' name='item_name' />\n";
		echo "<input type='hidden' name='num' />\n";
		echo "<input type='hidden' name='uPage' />\n";
		echo "<input type='hidden' name='pos' />\n";
		echo "<input type='hidden' name='reason' />\n";
		echo "</form>\n";
		echo "<div class='panel'>\n";
	}
	$query = "select is.is_sName,is.is_uType,icd.icd_uPageIndex,icd.icd_uPos,count(*) from tbl_item_collectivity_depot icd, tbl_item_static `is` where icd.is_uId = is.is_uId and icd.cds_uId = '$id' group by icd.icd_uPageIndex,icd.icd_uPos order by icd.icd_uPageIndex,icd.icd_uPos";
	page($var, $query, 178, "Game", $serverid, $result, $str, $desc);
	echo $desc;
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_tong_depot[1][0]."</th><th style='width:70px;'>".$tbl_tong_depot[1][1]."</th><th style='width:70px;'>".$tbl_tong_depot[1][2]."</th><th style='width:30px;'>".$tbl_tong_depot[1][3]."</th><th style='width:30px;'>".$tbl_tong_depot[1][4]."</th><th style='width:50px;'>".$tbl_sys_msg["act"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		$row[0] = Lan_Item_GetDisplayName(Local2UTF8($row[0]));
		echo sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>%d</td><td><input type='button' onclick=\"del('%d','%s','%d','%d','%d')\" value='".$tbl_sys_msg["delete"]."'/></td></tr>\n",$row[0],$bid2type[$row[1]],$tbl_tong_page[$row[2]],$row[3],$row[4],$row[1],$row[0],$row[4],$row[2],$row[3]);
	}
	echo "</table>\n";
	echo $str;
	if($var["page"]==null || $var["page"]==""){
		echo "</div>\n";
		ReturnAndBack();
	}
	return true;
}

function TongRename($var, $gm){
	global $tbl_game_server;
	$new_name = $var["new_name"];
	$id = $var["id"];
	$serverid = $var["serverid"];
	$reason = $var["reason"];
	
	$row = mysql_fetch_row(MySQLQuery("select t_sName from tbl_tong where t_uId = $id","Game",$serverid));
	$tong_name = $row[0];
	
	$param = array();
	$param["id"] = $id;
	$param["new_name"] = $new_name;
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "TongRename", $param);
	
	$action = array("tong" => $tong_name, "tong" => $new_name, "reason" => $reason);
	
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}

function ChangeTongLevel($var, $gm){
	global $tbl_game_server;
	$level = $var["new_name"];
	$id = $var["id"];
	$serverid = $var["serverid"];
	$reason = $var["reason"];
	
	$row = mysql_fetch_row(MySQLQuery("select t_sName from tbl_tong where t_uId = $id","Game",$serverid));
	$tong_name = $row[0];
	
	$param = array();
	$param["id"] = $id;
	$param["level"] = $level;
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "ChangeTongLevel", $param);
	
	$action = array("tong" => $tong_name, "level" => $level, "reason" => $reason);
	
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}

function GetTongLog($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg, $tbl_log_item_flow, $tbl_event_type;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	$type = $var["type"];
	
	LastLogTime();
	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs,"Game",$serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	
	$query = array();
	$date_limit = getDateLimit($var,$serverid);
	if(in_array("money", $type)){
		array_push($query, "(select le.le_dtDateTime, 'money', 0, ltm.ltm_uMoney, let.let_uEventType, le.le_uId from tbl_log_tong_money ltm, tbl_log_tong_taker ltt, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltm.le_uId and le.le_uId = ltt.le_uId and le.le_uId = let.le_uId and ltt.lts_uId = $tong_id $date_limit)");
		array_push($query, "(select le.le_dtDateTime, 'money', 1, ltm.ltm_uMoney, let.let_uEventType, le.le_uId from tbl_log_tong_money ltm, tbl_log_tong_giver ltg, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltm.le_uId and le.le_uId = ltg.le_uId and le.le_uId = let.le_uId and ltg.lts_uId = $tong_id $date_limit)");
	}
	if(in_array("honor", $type)){
		array_push($query, "(select le.le_dtDateTime, 'honor', 0, lth.lth_uHonor, let.let_uEventType, le.le_uId from tbl_log_tong_honor lth, tbl_log_tong_taker ltt, tbl_log_event le, tbl_log_event_type let where le.le_uId = lth.le_uId and le.le_uId = ltt.le_uId and le.le_uId = let.le_uId and ltt.lts_uId = $tong_id $date_limit)");
	}
	if(in_array("level", $type)){
		array_push($query, "(select le.le_dtDateTime, 'level', 0, ltl.ltl_uLevel, let.let_uEventType, le.le_uId from tbl_log_tong_level ltl, tbl_log_tong_taker ltt, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltl.le_uId and le.le_uId = ltt.le_uId and le.le_uId = let.le_uId and ltt.lts_uId = $tong_id $date_limit)");
	}
	if(in_array("resource", $type)){
		array_push($query, "(select le.le_dtDateTime, 'resource', 0, ltr.ltr_uResource, let.let_uEventType, le.le_uId from tbl_log_tong_resource ltr, tbl_log_tong_taker ltt, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltr.le_uId and le.le_uId = ltt.le_uId and le.le_uId = let.le_uId and ltt.lts_uId = $tong_id $date_limit)");
		array_push($query, "(select le.le_dtDateTime, 'resource', 1, ltr.ltr_uResource, let.let_uEventType, le.le_uId from tbl_log_tong_resource ltr, tbl_log_tong_giver ltg, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltr.le_uId and le.le_uId = ltg.le_uId and le.le_uId = let.le_uId and ltg.lts_uId = $tong_id $date_limit)");
	}
	if(in_array("develop", $type)){
		array_push($query, "(select le.le_dtDateTime, 'develop', 0, ltdd.ltdd_uDevelopDegree, let.let_uEventType, le.le_uId from tbl_log_tong_develop_degree ltdd, tbl_log_tong_taker ltt, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltdd.le_uId and le.le_uId = ltt.le_uId and le.le_uId = let.le_uId and ltt.lts_uId = $tong_id $date_limit)");
		array_push($query, "(select le.le_dtDateTime, 'develop', 1, ltdd.ltdd_uDevelopDegree, let.let_uEventType, le.le_uId from tbl_log_tong_develop_degree ltdd, tbl_log_tong_giver ltg, tbl_log_event le, tbl_log_event_type let where le.le_uId = ltdd.le_uId and le.le_uId = ltg.le_uId and le.le_uId = let.le_uId and ltg.lts_uId = $tong_id $date_limit)");
	}
	$qs = implode("union", $query);
	$qs.= "order by le_uId";
	
	$rs = MySQLQuery($qs,"Log",$serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["category"], $page_main["type"], $page_main["number"], $page_main["event_type"]);
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $tbl_tong_msg[$row[1]], $tbl_log_item_flow[$row[2]], $row[3], $tbl_event_type[$row[4]]);
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_Member($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg, $tbl_tong_pos;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();
	
	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs,"Game",$serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}

	$query = "select le.le_dtDateTime, ltme.ltme_uEventType, lc1.lc_sName, lc2.lc_sName, ltme.ltme_uJobType from tbl_log_tong_member_event ltme, tbl_log_event le, tbl_log_char lc1, tbl_log_char lc2 where ltme.le_uId = le.le_uId and ltme.ltme_uExecutorId = lc1.lcs_uId and ltme.ltme_uObjectId = lc2.lcs_uId and lts_uId = ".$tong_id." ".getDateLimit($var, $serverid);
	$rs = MySQLQuery($query,"Log",$serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["type"], $page_main["role_name"], $tbl_sys_msg["Executor"]."(".$page_main["position"].")");
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $tbl_tong_msg["member_event"][$row[1]], $row[3], $row[2]."(".$tbl_tong_pos[$row[4]].")");
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_Technology($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();

	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs,"Game",$serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}

	$query = "select le.le_dtDateTime, ltte.ltte_uEventType, ltte.ltte_sName, ltte.ltte_uLevel, lc.lc_sName from tbl_log_tong_technology_event ltte left join tbl_log_char lc on ltte.ltte_uExecutorId = lc.lcs_uId, tbl_log_event le where ltte.le_uId = le.le_uId and ltte_uEventType < 5 and lts_uId = ".$tong_id." ".getDateLimit($var,$serverid);
	$rs = MySQLQuery($query,"Log",$serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["type"], $page_main["item"], $page_main["level"], $tbl_sys_msg["Executor"]);
	while($row = mysql_fetch_row($rs)){
		if($row[4] == null)
			$row[4] = "-";
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $tbl_tong_msg["technology_event"][$row[1]], Lan_Tong_GetTechName(Local2UTF8($row[2]),$row[3]), $row[3], $row[4]);
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_Build($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();

	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	
	$query = "select le.le_dtDateTime, ltbe.ltbe_uEventType, ltbe.ltbe_sName, ltbe.ltbe_uLevel, lc.lc_sName, ltbe.ltbe_uExecutorTongId from tbl_log_tong_building_event ltbe left join tbl_log_char lc on ltbe.ltbe_uExecutorId = lc.lcs_uId, tbl_log_event le where ltbe.le_uId = le.le_uId and lts_uId = ".$tong_id." ".getDateLimit($var,$serverid);
	$rs = MySQLQuery($query, "Log", $serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["type"], $page_main["item"], $page_main["level"], $tbl_sys_msg["Executor"]."(".$page_main["tong"].")");
	while($row = mysql_fetch_row($rs)){
		if($row[5] != $tong_id && $row[5] != 0){
			$qs = "select t_sName from tbl_tong where t_uId = ".$row[5];
			$result = MySQLQuery($qs, "Game", $serverid);
			if($r = mysql_fetch_row($result)){
				$row[4].= "(".$r[0].")";
			}
		}
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $tbl_tong_msg["build_event"][$row[1]], Lan_Item_GetDisplayName(Local2UTF8($row[2]),10), $row[3], $row[4]);
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_Produce($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();
	
	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	
	$query = "select le.le_dtDateTime, ltip.ltip_uEventType, ltip.ltip_sName, lc.lc_sName from tbl_log_tong_item_produce ltip left join tbl_log_char lc on ltip.ltip_uExecutorId = lc.lcs_uId, tbl_log_event le where ltip.le_uId = le.le_uId and lts_uId = ".$tong_id." ".getDateLimit($var,$serverid);
	$rs = MySQLQuery($query, "Log", $serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["type"], $page_main["item"], $tbl_sys_msg["Executor"]);
	while($row = mysql_fetch_row($rs)){
		if($row[4] == null)
			$row[4] = "-";
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $tbl_tong_msg["produce_event"][$row[1]], Lan_Item_GetDisplayName(Local2UTF8($row[2]),10), $row[3]);
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_StationMove($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();
	
	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	
	$query = "select le.le_dtDateTime, ltsm.ltsm_uEventType, ltsm.ltsm_uOldWarzoneId, ltsm.ltsm_uOldIndex, ltsm.ltsm_uNewWarzoneId, ltsm.ltsm_uNewIndex, lc.lc_sName from tbl_log_tong_station_move ltsm left join tbl_log_char lc on ltsm.ltsm_uExecutorId = lc.lcs_uId, tbl_log_event le where ltsm.le_uId = le.le_uId and lts_uId = ".$tong_id." ".getDateLimit($var,$serverid);
	$rs = MySQLQuery($query, "Log", $serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["type"], $tbl_sys_msg["from"], $tbl_sys_msg["to"], $tbl_sys_msg["Executor"]);
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $tbl_tong_msg["station_event"][$row[1]], $tbl_tong_msg["war_zone"][$row[2]]." - ".$tbl_tong_msg["station_name"][$row[3]], $tbl_tong_msg["war_zone"][$row[4]]." - ".$tbl_tong_msg["station_name"][$row[5]], $row[6]);
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_Challenge($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg;
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();
	
	$qs = "select t_uId from tbl_tong where t_sName = '".$tong_name."'";
	$rs = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	
	$query = "select le.le_dtDateTime, ltce.ltce_dtEndTime, ltcs.ltcs_uChallengeTongId, ltcs.ltcs_uRecoveryTongId, ltcs.ltcs_uWarzoneId, ltcs.ltcs_uIndex, ltcs.ltcs_uExecutorId, ltce.ltce_uResult from tbl_log_event le, tbl_log_tong_challenge_start ltcs, tbl_log_tong_challenge_end ltce where ltcs.ltcs_uChallengeTongId = ltce.ltce_uChallengeTongId and ltcs.ltcs_uRecoveryTongId = ltce.ltce_uRecoveryTongId and ltcs.le_uId = le.le_uId and ltce.ltce_dtEndTime > le.le_dtDateTime and (ltcs.ltcs_uChallengeTongId = $tong_id or ltcs.ltcs_uRecoveryTongId = $tong_id) ".getDateLimit($var,$serverid)." group by le.le_uId";
	$rs = MySQLQuery($query, "Log", $serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$page_main["begin_time"], $page_main["end_time"], $tbl_sys_msg["from"], $tbl_sys_msg["to"], $page_main["pos"], $tbl_sys_msg["Executor"], $tbl_sys_msg["result"]);
	while($row = mysql_fetch_row($rs)){
		$qs = "select t_sName from tbl_tong where t_uId = ".$row[2];
		$result = MySQLQuery($qs, "Game", $serverid);
		if($r = mysql_fetch_row($result)){
			$from_tong = $r[0];
		}
		$qs = "select t_sName from tbl_tong where t_uId = ".$row[3];
		$result = MySQLQuery($qs, "Game", $serverid);
		if($r = mysql_fetch_row($result)){
			$to_tong = $r[0];
		}
		$qs = "select c_sName from tbl_char where cs_uId = ".$row[6];
		$result = MySQLQuery($qs, "Game", $serverid);
		if($r = mysql_fetch_row($result)){
			$char = $r[0];
		}
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[0], $row[1], $from_tong, $to_tong, $tbl_tong_msg["war_zone"][$row[4]]." - ".$tbl_tong_msg["station_name"][$row[5]], $char, $tbl_tong_msg["challenge_result"][$row[7]]);
	}
	echo "</table>\n";
	
	return true;
}

function GetTongLog_Boss($var, $gm){
	global $tbl_sys_msg, $page_main, $tbl_tong_msg;
	$boss_name = $var["boss_name"];
	$serverid = $var["serverid"];
	
	LastLogTime();
	
	$query = "select le.le_dtDateTime, lpt.lcs_uId from tbl_log_event le, tbl_log_jifenpoint lj, tbl_log_player_taker lpt, tbl_log_event_type let where le.le_uId = lpt.le_uId and le.le_uId = lj.le_uId and le.le_uId = let.le_uId and let.let_uEventType = 137 and lj.lj_uPoint = $boss_name ".getDateLimit($var,$serverid);
	$rs = MySQLQuery($query, "Log", $serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"], $page_main["role_name"]);
	while($row = mysql_fetch_row($rs)){
		$role_name = GetRoleName($row[1], $serverid);
		printf("<tr><td>%s</td><td>%s</td></tr>\n", $row[0], $role_name);
	}
	echo "</table>\n";
	
	return true;
}
?>
