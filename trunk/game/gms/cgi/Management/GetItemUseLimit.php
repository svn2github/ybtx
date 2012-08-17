<?php
function RetGetItemUseLimit($tbl_content, $var){
	$tong_count = count($tbl_content);
	global $page_main,$tbl_sys_msg;

	echo "<table class='list_table'>\n";
	echo "<tr><th>".$page_main["item"]."</th><th>".$tbl_sys_msg["act"]."</th></tr>\n";
	for($i=0;$i<$tong_count;$i++){
		$func_str=sprintf("<input type=button value='%s' onclick='DelGetItemLimit(\"%s\",this)'/>",$tbl_sys_msg["cancel"],TempLocal2UTF8($tbl_content[$i][0]));
		echo sprintf("<tr".(($i%2==0)?"":" class='even'")."><td align='center'>%s</td><td align='center'>%s</td></tr>\n",TempLocal2UTF8($tbl_content[$i][0]),$func_str);
	}
	echo "</table>\n";
}
function GetItemUseLimit($var, $gm){
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type="text/javascript">
function DelGetItemLimit(item_name,obj)
{
	document.form1.target = "block";
	document.form1.action_name.value = '95';
	document.form1.item_name.value = item_name;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
</script>
_js_src;
		echo $js_src."\n";
		form($var, $gm);
		echo "<input type='hidden' name='item_name'/>";
		echo "</form>\n";
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}

	$query_string = "select iul_sItemName from tbl_item_use_limit";
	$result = MySQLQuery($query_string, "Game", $serverid);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	RetGetItemUseLimit($tbl_content,$var);
	return true;
}

function GetQuestFinishLimit($var, $gm){
	global $page_main,$tbl_sys_msg;
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type="text/javascript">
function DelQuestFinishLimit(quest_name,obj)
{
	document.form1.target = "block";
	document.form1.action_name.value = '150';
	document.form1.quest_name.value = quest_name;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
</script>
_js_src;
		echo $js_src."\n";
		ReturnAndBack();
		form($var, $gm);
		echo "<input type='hidden' name='quest_name'/>";
		echo "</form>\n";
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}

	$query_string = "select qfl_sQuestName from tbl_quest_finish_limit";
	$result = MySQLQuery($query_string, "Game", $serverid);

	echo "<table class='list_table'>\n";
	echo "<tr><th>".$page_main["task"]."</th><th>".$tbl_sys_msg["act"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		$func_str=sprintf("<input type=button value='%s' onclick='DelQuestFinishLimit(\"%s\",this)'/>",$tbl_sys_msg["cancel"],Local2UTF8($row[0]));
		echo sprintf("<tr".(($i%2==0)?"":" class='even'")."><td align='center'>%s</td><td align='center'>%s</td></tr>\n",Local2UTF8($row[0]),$func_str);
	}
	echo "</table>\n";
	return true;
}

function GetObjDropLimit($var, $gm){
	global $page_main,$tbl_sys_msg;
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type="text/javascript">
function DelObjDropLimit(item_name,obj)
{
	document.form1.target = "block";
	document.form1.action_name.value = '152';
	document.form1.item_name.value = item_name;
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.submit();
}
</script>
_js_src;
		echo $js_src."\n";
		form($var, $gm);
		echo "<input type='hidden' name='item_name'/>";
		echo "</form>\n";
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}else{
		$serverid = intval($var["serverindex"]);
	}

	$query_string = "select odl_sObjName from tbl_obj_drop_limit";
	$result = MySQLQuery($query_string, "Game", $serverid);

	echo "<table class='list_table'>\n";
	echo "<tr><th>".$page_main["item"]."</th><th>".$tbl_sys_msg["act"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		$func_str=sprintf("<input type=button value='%s' onclick='DelObjDropLimit(\"%s\",this)'/>",$tbl_sys_msg["cancel"],Local2UTF8($row[0]));
		echo sprintf("<tr".(($i%2==0)?"":" class='even'")."><td align='center'>%s</td><td align='center'>%s</td></tr>\n",Local2UTF8($row[0]),$func_str);
	}
	echo "</table>\n";
	return true;
}
?>
