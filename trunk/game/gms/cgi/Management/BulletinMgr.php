<?php
function ViewBulletin($var, $gm)
{
	global $page_main;

	$id = intval($var["id"]);

	$gm_name = mysql_escape_string($gm);

	$qs = "select title, begin_time, end_time, step, content from tbl_gm_anousment where ga_id = '$id'";
	$result = MySQLQuery($qs, "GMS");
	$row=mysql_fetch_row($result);

	$begin_time=$row[1];
	$end_time=$row[2];
	$step=$row[3];
	$title=$row[0];
	$content=$row[4];

	global $tbl_list_bulletin;
	echo "<table class='info_table'>\n";
	printf("<tr><th>%s</th><td>%s</td></tr>\n",$page_main["begin_time"],	$begin_time);
	printf("<tr><th>%s</th><td>%s</td></tr>\n",$page_main["end_time"],		$end_time);
	printf("<tr><th>%s</th><td>%s</td></tr>\n",$page_main["step"],				$step);
	printf("<tr><th>%s</th><td>%s</td></tr>\n",$page_main["title"],				$title);
	printf("<tr><th>%s</th><td>%s</td></tr>\n",$page_main["content"],			$content);
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function PostBulletin($var, $gm){
	return eachServer($var, $gm, PostBulletinFun);
	ReturnAndBack();
}
function PostBulletinFun($var, $gm, $serv_id){
	$begin_time = mysql_escape_string($var["begin_time"]);
	$end_time = mysql_escape_string($var["end_time"]);
	$step = intval($var["step"]);
	$title = mysql_escape_string($var["title"]);
	$content = mysql_escape_string($var["content"]);

	$gm_name = mysql_escape_string($gm);
	
	$action = array("title" => $title, "content" => $content, "begin_time" => $begin_time, "end_time" => $end_time, "step" => $step);

	$qs = "insert into tbl_gm_anousment set begin_time = '$begin_time', end_time = '$end_time', step = '$step', title = '$title', content = '$content', serverid = '$serv_id', state='0'";

	$result = MySQLQuery($qs,"GMS");
	if (!$result){
		PrintNotice(LogInfo($action,false));
		return false;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serv_id)){	
		PrintNotice(LogInfo($action,true));
		return true;
	}
	return false;
	
}

function DelBulletin($var, $gm)
{
	$id = intval($var["id"]);

	$gm_name = mysql_escape_string($gm);

	$qs = "update tbl_gm_anousment set state = 2 where ga_id = '$id'";
	$result = MySQLQuery($qs, "GMS");

	$qs = "select title, begin_time, end_time, step, content, serverid from tbl_gm_anousment where ga_id = '$id'";
	$result = MySQLQuery($qs,"GMS");
	$row=mysql_fetch_row($result);

	$begin_time=$row[1];
	$end_time=$row[2];
	$step=$row[3];
	$title=$row[0];
	$content=$row[4];
	$serv_id=$row[5];
	$action = array("title" => $title, "content" => $content, "begin_time" => $begin_time, "end_time" => $end_time, "step" => $step);
	if (LogGMCommand($gm_name, $var["action_name"], $action, $serv_id)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}

function ListBulletin($var, $gm)
{
	global $page_main,$tbl_sys_msg,$tbl_list_bulletin_state,$tbl_game_server;
	if($var["page"]==null || $var["page"]==""){
		$strjs = <<<_strjs
	<script type="text/javascript" src="/js/page.js"></script>
	<script type="text/javascript">
	function View(id){
		document.form1.action_name.value='83';
		document.form1.id.value=id;
		document.form1.submit();
	}
	function Delete(id){
		document.form1.target = "_blank";
		document.form1.action_name.value='84';
		document.form1.id.value=id;
		document.form1.submit();
	}
	</script>
_strjs;
		echo $strjs."\n";
		form($var,$gm);
		echo "<input type='hidden' name='id'/>\n";
		echo "</form>\n";
	}
	
	$serverid = $var["server_id"];

	$serv_id = intval($var["serverindex"]);
	$query = "select ga_id, begin_time, end_time, step, title, state, serverid from tbl_gm_anousment where serverid in (".implode(",",$serverid).") order by ga_id desc";
	page($var,$query,81,"GMS",0,$result,$page,$desc);
	
	echo "<table class='list_table'>\n";
	echo "<tr><th style='width:120px;'>".$page_main["begin_time"]."</th><th style='width:120px;'>".$page_main["end_time"]."</th><th style='width:60px;'>".$page_main["step"]."</th><th>".$page_main["title"]."</th><th style='width:40px;'>".$tbl_sys_msg["status"]."</th><th>".$page_main["select_server"]."</th><th style='width:150px;'>".$tbl_sys_msg["act"]."</th></tr>\n";

	while($row=mysql_fetch_row($result)){
		printf("<tr><td>%s</td><td>%s</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s%s</td></tr>\n",$row[1],$row[2],$row[3],$row[4],$tbl_list_bulletin_state[$row[5]],$tbl_game_server[$row[6]][2],(hasAuth(83,$gm))?"<input type=button value='".$tbl_sys_msg["detail"]."' onclick='View(".$row[0].")'/>":"",(hasAuth(84,$gm))?"<input type=button value='".$tbl_sys_msg["delete"]."' onclick='Delete(".$row[0].")'/>":"");
	}

	echo "</table>\n";
	echo $page;
	if($var["page"]==null || $var["page"]==""){
		echo "</div>\n";
		ReturnAndBack();
	}
	return true;
}

function GMToolsNoticeInsert($var, $gm){
	global $page_action_name,$tbl_sys_msg,$page_main;
	$title = $var["title"];
	$content = $var["content"];
	$rs = MySQLQuery("insert into tbl_gm_tools_notice(gmn_sTitle, gmn_sContent, gms_sName, gmn_dDate) values('".$title."','".$content."','".$gm."',curdate())","GMS");
	$gmn_uId = mysql_insert_id();
	$rs = MySQLQuery("insert into tbl_gm_tools_notice_unread select $gmn_uId, gm_name from tbl_session","GMS");
	
	$action = array("title" => $title, "content" => $content);
	LogGMCommand($gm, $var["action_name"], $action, 0);
	PrintNotice($tbl_sys_msg["success"]."<hr>".LogInfo($action));
	return true;
}
?>
