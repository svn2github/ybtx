<?php
function GetAllCompensate($var, $gm)
{
	global $page_action_name,$page_main,$tbl_sys_msg,$tbl_game_server;
	if($var["serverindex"]==null || $var["serverindex"]=="" and $var["flag"]=="" and $var["role_name"]==""){
		$strjs = <<<_strjs
<script type="text/javascript" src="/js/page.js"></script>
<script type="text/javascript">
function pass(obj,id){
	document.form1.action_name.value='54';
	document.form1.id.value=id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.target='_block';
	document.form1.submit();
}
function cancel(obj,id)
{
	document.form1.action_name.value='55';
	document.form1.id.value=id;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.target='_block';
	document.form1.submit();
}
function pass_all(){
	document.form1.action_name.value='54';
	document.form1.target='_block';
	document.form1.submit();
}
function cancel_all(){
	document.form1.action_name.value='55';
	document.form1.target='_block';
	document.form1.submit();
}
</script>
_strjs;
		echo $strjs."\n";
		echo "<script type='text/javascript' src='/js/page.js'></script>\n";
		echo "<script type='text/javascript' src='/js/log_info.js'></script>\n";
		echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
		form($var, $gm);
		checkbox(true,true);
		echo "<input type='hidden' name='flag' />\n";
		echo "<input type='hidden' name='role_name' />\n";
		echo "<input type='hidden' name='str' />\n";
		echo "<input type='hidden' name='urs' />\n";
		echo "<input type='hidden' name='id'/>\n";
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		echo "</form>\n";
		return $ret;
	}else{
		$serv_id = intval($var["serverindex"]);
	}
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
	$query_string="select gca_uId, gm, action_id, reason from tbl_gm_compensate_action where action_state = 1 and server_id = '$serv_id'".$filter." order by gca_uId desc";
	page($var,$query_string,53,"GMS",0,$result,$page,$desc);
	echo "<table class='list_table'>\n";
	echo "<tr><th style='width:60px;'>".$page_main["user_name"]."</th><th style='width:120px;'>".$page_main["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th><th style='width:120px;'>".$tbl_sys_msg["act"]."</th><th>".$tbl_sys_msg["server"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		printf("<tr><td align='center'>%s</td><td><input type='checkbox' name='ids[]' value='%d_%d'/>%s</td><td>%s</td><td align='center'>%s%s</td><td>%s</td></tr>\n",$row[1],$row[0],$serv_id,GetGMCommandLogAction($row[2]),LogInfo($row[3]),(hasAuth(54,$gm))?"<input type=button value='".$tbl_sys_msg["pass"]."' onclick='pass(this,".$row[0].")'/>":"",(hasAuth(55,$gm))?"<input type=button value='".$tbl_sys_msg["cancel"]."' onclick='cancel(this,".$row[0].")'/>":"",$tbl_game_server[$serv_id][2]);
	}
	echo "</table>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><td>";
	if(HasAuth(54,$gm)) echo "<input type='button' onclick='pass_all()' value='".$page_action_name[54]."' />";
	if(HasAuth(55,$gm)) echo "<input type='button' onclick='cancel_all()' value='".$page_action_name[55]."' />";
	echo "</td></tr>\n";
	echo "</table>\n";
	echo $page;
	return true;
}

?>
