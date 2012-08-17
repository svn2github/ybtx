<?php
function GetGMMgrInfo($var, $gm)
{
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type="text/javascript">
<!--//
function DelGMMmr(obj,id,game_id)
{
	document.form1.action =  "/cgi-bin/gm_tools/FrameWork.php";
	document.form1.action_name.value = '89';
	document.form1.target = "block";
	document.form1.id.value = id;
	document.form1.game_id.value = game_id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
function ChangeGMLevel(obj,id,game_id)
{	
	document.form1.action =  "/lib/gm_tools/ChangeGMLevel.php";
	document.form1.target = "block";
	document.form1.id.value = id;
	document.form1.game_id.value = game_id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
	
}
</script>
_js_src;
		echo $js_src."\n";
		form($var, $gm);
		echo "<input type='hidden' name='id'/>\n";
		echo "<input type='hidden' name='game_id'/>\n";
		$ret = eachServerFrame($var, $gm);
		echo "</form>\n";
		ReturnAndBack();
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}
	global $page_main,$tbl_sys_msg;

	$query_string = "select username,gv_uLevel, us_uGameID from tbl_gm_validate";
	$result = MySQLQuery($query_string, "Game", $serverid);
	
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n", $page_main["user_name"], $tbl_sys_msg["level"], $tbl_sys_msg["act"]);

	while($row=mysql_fetch_row($result)){
		$func_str = "";
		if(hasAuth(89,$gm)) $func_str.= sprintf("<input type=button value=\"%s\" onclick=\"DelGMMmr(this,'%s',%d)\"/>",$tbl_sys_msg["delete"],$row[0],$row[2]);
		if(hasAuth(96,$gm)) $func_str.= sprintf("<input type=button value=\"%s\" onclick=\"ChangeGMLevel(this,'%s',%d)\"/>",$tbl_sys_msg["edit"],$row[0],$row[2]);
		printf("<tr".(($i%2==0)?"":" class='even'")."><td>%s</td><td>%d</td><td>%s</td></tr>\n",$row[0],$row[1], $func_str);
	}

	echo "</table>\n";
	return true;
}
?>
