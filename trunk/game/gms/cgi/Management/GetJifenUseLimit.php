<?php
function RetJifenUseLimit($tbl_content, $var)
{
	$tong_count = count($tbl_content);

	global $page_main,$tbl_sys_msg;
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$page_main["jifen"]."</th><th>".$tbl_sys_msg["act"]."</th></tr>\n";
	global $tbl_jifen;
	for($i=0;$i<$tong_count;$i++){
		$func_str=sprintf("<input type=button value='%s' onclick='DelJifenLimit(\"%d\",this)'/>",$tbl_sys_msg["cancel"],$tbl_content[$i][0]);
		echo sprintf("<tr".(($i%2==0)?"":" class='even'")."><td align='center'>%s</td><td align='center'>%s</td></tr>\n", $tbl_jifen[$tbl_content[$i][0]], $func_str);
	}
	echo "</table>\n";
}
function GetJifenUseLimit($var, $gm)
{
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type="text/javascript">
function DelJifenLimit(jifen_type,obj){
	document.form1.target = "block";
	document.form1.action_name.value = '94';
	document.form1.jifen_type.value = jifen_type;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
</script>
_js_src;
		echo $js_src."\n";
		form($var, $gm);
		echo "<input type='hidden' name='jifen_type'/>";
		echo "</form>\n";
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}

	$query_string = "select jul_uType  from tbl_jifen_use_limit";
	$result = MySQLQuery($query_string, "Game", $serverid);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	RetJifenUseLimit($tbl_content,$var);
	return true;
}
?>
