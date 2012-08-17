<?php
function RetMoneyCorrencyLimit($tbl_content, $var){
	$tong_count = count($tbl_content);
	global $page_main,$tbl_sys_msg;
	echo "<table class='list_table'>\n";
	echo sprintf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n", $page_main["function"], $page_main["module"], $tbl_sys_msg["act"]);
	for($i=0;$i<$tong_count;$i++)
	{
		$func_str=sprintf("<input type=button value='".$tbl_sys_msg["cancel"]."' onclick='DelMoneyCorrencyLimit(\"%s\",this)'/>", $tbl_content[$i][0] . "+" . $tbl_content[$i][1]);
		echo sprintf("<tr bgcolor='%s'><td align='center'>%s</td><td align='center'>%s</td><td align='center'>%s</td></tr>",$color, $tbl_content[$i][0],$tbl_content[$i][1], $func_str);
	}
}
function GetMoneyCorrencyLimit($var, $gm)
{
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type="text/javascript">
<!--//
function DelMoneyCorrencyLimit(money_func,obj)
{
	document.form1.target = "block";
	document.form1.action_name.value = '93';
	document.form1.money_func.value = money_func;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
</script>
_js_src;
		echo $js_src;
		form($var, $gm);
		echo "<input type='hidden' name='money_func'/>\n";
		echo "</form>\n";
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}

	$query_string = "select mcl_sFunName,mcl_sModule from tbl_money_corrency_limit";
	$result = MySQLQuery($query_string, "Game", $serverid);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}

	RetMoneyCorrencyLimit($tbl_content,$var);
	return true;
}
?>
