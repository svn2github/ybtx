<?php
function RetGetRoles($tbl_content,$var,$page)
{
	global $tbl_sys_msg,$page_main;
	echo "<input type='hidden' name='serverindex' value='".$var["serverindex"]."'/>";
	echo "<table class='list_table'>";
	echo "<tr>\n";
	$count = 8;
	echo "<th style='text-align:left;'><input type='checkbox' onclick=\"checkboxAll(this,'user')\" />".$page_main["user_name"]."</th>\n";
	echo "<th style='text-align:left;'><input type='checkbox' onclick=\"checkboxAll(this,'role')\" />".$page_main["role_name"]."</th>\n";
	echo "<th>".$tbl_sys_msg["status"]."</th>\n";
	echo "<th>".$page_main["level"]."</th>\n";
	echo "<th>".$tbl_sys_msg["class"]."</th>\n";
	echo "<th>".$page_main["camp"]."</th>\n";
	echo "<th>".$tbl_sys_msg["MoneyType1"]."</th>\n";
	echo "<th>".$tbl_sys_msg["MoneyType2"]."</th>\n";
	echo "</tr>\n";
	
	for($i=0;$i< count($tbl_content) ;$i++){
		echo "<tr".(($i%2)?"":" class='even'").">\n";
		echo "<td><input type='checkbox' value='".$tbl_content[$i][0]."' onclick=\"Checkbox(this)\" flag=\"user\"/><a href=\"#\" onclick=\"ViewUser(this,'".$tbl_content[$i][0]."')\">".$tbl_content[$i][0]."</a></td>\n";
		echo "<td><input type='checkbox' value='".$tbl_content[$i][1]."' onclick=\"Checkbox(this)\" flag=\"role\"/><a href=\"#\" onclick=\"ViewRole(this,'".$tbl_content[$i][1]."')\">".$tbl_content[$i][1]."</a></td>\n";
		echo "<td>".$tbl_content[$i][2]."</td>\n";
		echo "<td>".$tbl_content[$i][3]."</td>\n";
		echo "<td>".$tbl_content[$i][4]."</td>\n";
		echo "<td>".$tbl_content[$i][5]."</td>\n";
		echo "<td>".$tbl_content[$i][6]."</td>\n";
		echo "<td>".$tbl_content[$i][7]."</td>\n";
		echo "</tr>\n";
	}
	echo "</table>";
	echo $page;
}
function GetRoles($var, $gm, $fun){
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_src=<<<_js_src
<script type='text/javascript' src='/js/checkbox.js'></script>
<script type='text/javascript' src='/js/page.js'></script>
<script>
	function ViewUser(obj,urs)
	{
		alert($(obj).html());
		document.form1.action_name.value = '50';
		document.form1.serverid.value = $(obj).parents(".panel").find("input[name='serverindex']").val();
		document.form1.urs.value = urs;
		document.form1.submit();
	}
	function ViewRole(obj,role_name)
	{
		document.form1.action_name.value = '97';
		document.form1.serverid.value = $(obj).parents(".panel").find("input[name='serverindex']").val();
		document.form1.role_name.value = role_name;
		document.form1.submit();
	}
</script>
_js_src;
		echo $js_src."\n";
		form($var, $gm);
		echo "<input type='hidden' name='role_name' />";
		echo "<input type='hidden' name='urs' />";
		checkbox(true, true);
		ReturnAndBack();
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		echo "</form>\n";
		return $ret;
	}
	$serverid = $var["serverindex"];
	$channel_id=$var["channel_id"];
	
	$tbl_content = $fun($var, $gm);
	return true;
}

function GetRolesByLevel($var, $gm){
	return GetRoles($var, $gm, "level");
}

function level($var, $gm){
	global $tbl_class;
	global $tbl_camp;
	global $tbl_sys_msg;
	$MinNum = mysql_escape_string($var["MinNum"]);
	$MaxNum = mysql_escape_string($var["MaxNum"]);
	if (!$MinNum || $MinNum == "") $MinNum = 0;
	$query = "select us.us_sName,c.c_sName,ifnull(co_uOnServerId,0),cb.cb_uLevel,cs.cs_uClass,cs.cs_uCamp,cm.cm_uMoney,cm.cm_uBindingMoney,cb.cs_uId from tbl_char_basic as cb,tbl_char_money cm,tbl_char as c,tbl_user_static as us,tbl_char_static as cs left join tbl_char_online as co on cs.cs_uId = co.cs_uId ";
	$query.= "where ";
	$query.= "cb.cs_uId = c.cs_uId ";
	$query.= "and c.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId and cb.cs_uId = cm.cs_uId ";
	$query.= "and cb.cb_uLevel >= '".$MinNum."'";
	if($MaxNum && $MaxNum != "") $query.= " and cb.cb_uLevel <= '".$MaxNum."'";
	$query.= " order by cb.cb_uLevel desc, c.cs_uId";
	page($var, $query,22, "Game", $var["serverindex"], $result, $page, $desc);
	$tbl_content=array();
	
	while($row=mysql_fetch_row($result)){
		$row[2] = (intval($row[2])>0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"];
		$row[4] = $tbl_class[intval($row[4])];
		$row[5] = $tbl_camp[intval($row[5])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	echo $desc;
	RetGetRoles($tbl_content,$var,$page);
}

function GetRolesByBindingMoney($var, $gm){
	return GetRoles($var, $gm, "binding");
}

function binding($var, $gm){
	global $tbl_class;
	global $tbl_camp;
	global $tbl_sys_msg;
	$MinNum = intval($var["min_1"])*10000+intval($var["min_2"])*100+intval($var["min_3"]);
	$MaxNum = intval($var["max_1"])*10000+intval($var["max_2"])*100+intval($var["max_3"]);
	if (!$MinNum || $MinNum == "") $MinNum = 0;
	$query = "select us.us_sName,c.c_sName,ifnull(co_uOnServerId,0),cb.cb_uLevel,cs.cs_uClass,cs.cs_uCamp,cm.cm_uMoney,cm.cm_uBindingMoney, cb.cs_uId from tbl_char_basic as cb,tbl_char as c,tbl_user_static as us,tbl_char_static as cs, tbl_char_money cm left join tbl_char_online as co on co.cs_uId = cm.cs_uId
		where 
			cb.cs_uId = c.cs_uId
			and c.cs_uId = cs.cs_uId and cs.cs_uId = cm.cs_uId and cs.us_uId = us.us_uId
		and cm.cm_uBindingMoney >= '".$MinNum."'";
		if($MaxNum && $MaxNum != "") $query.= " and cm.cm_uBindingMoney <= '".$MaxNum."'";
		$query.= " order by cm.cm_uBindingMoney desc, c.cs_uId";
	page($var, $query,21, "Game", $var["serverindex"], $result, $page, $desc);
	$tbl_content=array();
	
	while($row=mysql_fetch_row($result)){
		$row[2] = (intval($row[2])>0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"];
		$row[4] = $tbl_class[intval($row[4])];
		$row[5] = $tbl_camp[intval($row[5])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	echo $desc;
	RetGetRoles($tbl_content,$var,$page);
}

function GetRolesByMoney($var, $gm){
	return GetRoles($var, $gm, "money");
}

function money($var, $gm){
	global $tbl_class;
	global $tbl_camp;
	global $tbl_sys_msg;
	$MinNum = intval($var["min_1"])*10000+intval($var["min_2"])*100+intval($var["min_3"]);
	$MaxNum = intval($var["max_1"])*10000+intval($var["max_2"])*100+intval($var["max_3"]);
	if (!$MinNum || $MinNum == "") $MinNum = 0;
	$query = "select us.us_sName,c.c_sName,ifnull(co_uOnServerId,0),cb.cb_uLevel,cs.cs_uClass,cs.cs_uCamp,cm.cm_uMoney,cm.cm_uBindingMoney, cb.cs_uId from tbl_char_basic as cb,tbl_char as c,tbl_user_static as us,tbl_char_static as cs, tbl_char_money cm left join tbl_char_online as co on cm.cs_uId = co.cs_uId
		where 
			cb.cs_uId = c.cs_uId
			and c.cs_uId = cs.cs_uId and cs.cs_uId = cm.cs_uId and cs.us_uId = us.us_uId
		and cm.cm_uMoney >= '".$MinNum."'";
		if($MaxNum && $MaxNum != "") $query.= " and cm.cm_uMoney <= '".$MaxNum."'";
		$query.= " order by cm.cm_uMoney desc, c.cs_uId";
	page($var, $query,21, "Game", $var["serverindex"], $result, $page, $desc);
	$tbl_content=array();
	
	while($row=mysql_fetch_row($result)){
		$row[2] = (intval($row[2])>0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"];
		$row[4] = $tbl_class[intval($row[4])];
		$row[5] = $tbl_camp[intval($row[5])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	echo $desc;
	RetGetRoles($tbl_content,$var,$page);
}

function GetRolesBySceneName($var, $gm){
	return GetRoles($var, $gm, "scene");
}
function scene($var, $gm){
	global $tbl_class;
	global $tbl_camp;
	global $tbl_sys_msg;
	$scene_name = UTF82Local(mysql_escape_string($var["scene_name_"]));
	$query = "select us.us_sName,c.c_sName,ifnull(co_uOnServerId,0),cb.cb_uLevel,cs.cs_uClass,cs.cs_uCamp,cm.cm_uMoney,cm.cm_uBindingMoney, cb.cs_uId from tbl_char_position as cp,tbl_char as c,tbl_scene as ts,tbl_char_basic as cb,tbl_char_static as cs, tbl_user_static us, tbl_user_online uo,tbl_char_money cm left join tbl_char_online as co on cm.cs_uId = co.cs_uId
		where 
			cb.cs_uId = cs.cs_uId and cs.cs_uId = c.cs_uId and  c.cs_uId = cm.cs_uId and 
			c.cs_uId = cp.cs_uId and cp.sc_uId = ts.sc_uId and cs.us_uId = us.us_uId and us.us_uId = uo.us_uId
		and ts.sc_sSceneName = '".$scene_name."'";
	page($var, $query,21, "Game", $var["serverindex"], $result, $page, $desc);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$row[2] = (intval($row[2])>0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"];
		$row[4] = $tbl_class[intval($row[4])];
		$row[5] = $tbl_camp[intval($row[5])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	echo $desc;
	RetGetRoles($tbl_content,$var,$page);
}

function GetRolesByChannel($var, $gm){
	return GetRoles($var, $gm, "channel");
}
function channel($var, $gm){
	global $tbl_class,$tbl_camp,$tbl_sys_msg,$page_main,$tbl_channel;
	$channel_id = implode(",",$var["channel_id"]);
	$query = "select us.us_sName,c.c_sName,ifnull(co_uOnServerId,0),cb.cb_uLevel,cs.cs_uClass,cs.cs_uCamp,cm.cm_uMoney,cm.cm_uBindingMoney, cb.cs_uId, group_concat(distinct ft_uChannelId order by ft_uChannelId) from tbl_forbid_talking as ft,tbl_char as c,tbl_user_static as us,tbl_char_static as cs,tbl_char_basic as cb ,tbl_char_money cm left join tbl_char_online as co on cm.cs_uId = co.cs_uId 
		where 
			ft.cs_uID = cb.cs_uId and cb.cs_uId = c.cs_uId and c.cs_uId = cm.cs_uId
			and c.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId
		and ft.ft_uChannelId in (".$channel_id.") group by cs.cs_uId";
	page($var, $query,21, "Game", $var["serverindex"], $result, $page, $desc);
	$tbl_content=array();
	echo "<table class='list_table'>\n";
	printf("<tr><th style='text-align:left;' rowspan='2'><input type='checkbox' onclick=\"checkboxAll(this,'user')\" />%s</th><th style='text-align:left;' rowspan=\"2\"><input type='checkbox' onclick=\"checkboxAll(this,'role')\" />%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$page_main["user_name"],$page_main["role_name"],$tbl_sys_msg["status"],$tbl_sys_msg["level"],$tbl_sys_msg["class"],$tbl_sys_msg["camp"],$tbl_sys_msg["MoneyType1"],$tbl_sys_msg["MoneyType2"]);
	printf("<tr><th>%s</th><th>%s</th><th colspan='2'>%s</th><th colspan='2'>%s</th></tr>\n",$page_main["channel"],"GM",$page_main["end_time"],$tbl_sys_msg["reason"]);
	
	while($row=mysql_fetch_row($result)){
		$row[2] = (intval($row[2])>0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"];
		$row[4] = $tbl_class[intval($row[4])];
		$row[5] = $tbl_camp[intval($row[5])];
		$channels = explode(",",$row[9]);
		printf("<tr><td rowspan='%9\$d'><input type='checkbox' value='%1\$s' onclick=\"Checkbox(this)\" flag=\"user\"/><a href=\"#\" onclick=\"ViewUser(this,'%1\$s')\">%1\$s</a></td><td rowspan='%9\$d'><input type='checkbox' value='%2\$s' onclick=\"Checkbox(this)\" flag=\"role\"/><a href=\"#\" onclick=\"ViewRole(this,'%2\$s')\">%2\$s</a></td><td>%3\$s</td><td>%4\$s</td><td>%5\$s</td><td>%6\$s</td><td>%7\$s</td><td>%8\$s</td></tr>\n",$row[0],$row[1],$row[2],$row[3],$row[4],$row[5],$row[6],$row[7],count($channels)+1);
		foreach($channels as $channel){
			$qs = "select gm,end_date,reason from tbl_forbid_talk where role_name = '".$row[1]."' and channal_id = ".$channel;
			$rs = MySQLQuery($qs, "GMS");
			$r = mysql_fetch_row($rs);
			printf("<tr><td>%1\$s</td><td>%2\$s</td><td colspan='2'>%3\$s</td><td colspan='2'>%4\$s</td></tr>\n",$tbl_channel[$channel],$r[0],$r[1],$r[2]);
		}
	}
	echo "</table>\n";
	echo $page;
	//RetGetRoles($tbl_content,$var,$page);
}

function GetRolesByOnline($var, $gm){
	return GetRoles($var, $gm, "online");
}

function online($var, $gm){
	global $tbl_class;
	global $tbl_camp;
	global $tbl_sys_msg;
	$query = "select us.us_sName,c.c_sName,ifnull(co_uOnServerId,0),cb.cb_uLevel,cs.cs_uClass,cs.cs_uCamp,cm.cm_uMoney,cm.cm_uBindingMoney, cb.cs_uId from tbl_char_basic as cb,tbl_char as c,tbl_user_static as us,tbl_char_static as cs, tbl_char_money cm left join tbl_char_online as co on co.cs_uId = cm.cs_uId
		where 
			cb.cs_uId = c.cs_uId
			and c.cs_uId = cs.cs_uId and cs.cs_uId = cm.cs_uId and cs.us_uId = us.us_uId
		";
		$query.= " order by cm.cm_uBindingMoney desc, c.cs_uId";
	page($var, $query,21, "Game", $var["serverindex"], $result, $page, $desc);
	$tbl_content=array();
	
	while($row=mysql_fetch_row($result)){
		$row[2] = (intval($row[2])>0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"];
		$row[4] = $tbl_class[intval($row[4])];
		$row[5] = $tbl_camp[intval($row[5])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	echo $desc;
	RetGetRoles($tbl_content,$var,$page);
}
?>
