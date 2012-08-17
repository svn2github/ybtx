<?php
function RetGetRoleList($tbl_content, $serv_id, $type)
{
	global $tbl_class,$tbl_camp,$tbl_sys_msg,$tbl_game_server,$tbl_user_role_list_function;
	$str="";
	

	$role_count=count($tbl_content);
	global $tbl_class;
	global $tbl_camp;
	for($i=0;$i<$role_count;$i++)
	{	
		$delete = sprintf(($type==1)?"<input type='button' value='$tbl_user_role_list_function[0]' onclick=\"role_delete('%s','%s',this)\" />":"<input type='button' value='$tbl_user_role_list_function[1]' onclick=\"role_back('%s','%s',this)\" />",$tbl_content[$i][0],$tbl_content[$i][6]);
		$online_str = $tbl_sys_msg["offline"];
		if ($tbl_content[$i][4] > 0)
			$online_str = $tbl_sys_msg["online"];
		$str_href = sprintf("<input type=\"checkbox\" onclick=\"Checkbox(this)\" flag=\"role\" value='%s' /><a href='#' onclick='OnClick(\"%s\",this)'/>%s</a>", $tbl_content[$i][0], $tbl_content[$i][0], $tbl_content[$i][0]);
		$str=$str.sprintf("<tr".(($i%2)?" class='even'":"")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td class='server' title='%d'>%s</td><td>%s</td><td>%s<input type='button' onclick=\"role_rename('%s',this)\" value='$tbl_user_role_list_function[2]'/></td></tr>", $str_href,$online_str,$tbl_class[$tbl_content[$i][1]], $tbl_camp[$tbl_content[$i][2]], $tbl_content[$i][3],$tbl_content[$i][5],$serv_id,$tbl_game_server[$serv_id][2],$tbl_user_type[$type],$delete,$tbl_content[$i][0]);
	}
	echo $str;

}
function get_role_list($urs,$serv_id)
{
	$urs = mysql_escape_string($urs);
	$query_string = "select c.c_sName,cs.cs_uClass,cs.cs_uCamp,cb.cb_uLevel,ifnull(co_uOnServerId,0),cs.cs_dtCreateDate,cs.cs_uId 
		from tbl_user_static as us,tbl_char as c,tbl_char_basic as cb,tbl_char_static as cs left join tbl_char_online as co on co.cs_uId = cs.cs_uId
		where us.us_uId = cs.us_uId and 
		cb.cs_uId = cs.cs_uId and 
		c.cs_uId = cb.cs_uId and 
		us.us_sName = '".$urs."'";
	$result = MySQLQuery($query_string,"Game",$serv_id);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	
	$query_string = "select c.cd_sName,cs.cs_uClass,cs.cs_uCamp,cb.cb_uLevel,ifnull(co_uOnServerId,0),cs.cs_dtCreateDate,cs.cs_uId 
		from tbl_user_static as us,tbl_char_deleted as c,tbl_char_basic as cb,tbl_char_static as cs left join tbl_char_online as co on co.cs_uId = cs.cs_uId 
		where us.us_uId = cs.us_uId and 
		cb.cs_uId = cs.cs_uId and 
		c.cs_uId = cb.cs_uId and 
		us.us_sName = '".$urs."'";
	$result = MySQLQuery($query_string,"Game",$serv_id);
	$tbl_content2=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content2);
		$tbl_content2[$tbl_size]=$row;
	}
	if(count($tbl_content)>0){
		RetGetRoleList($tbl_content, $serv_id, 1);
	}
	if(count($tbl_content2)>0){
		RetGetRoleList($tbl_content2, $serv_id, 2);
	}
}
function GetRoleList($var, $gm)
{
	global $tbl_sys_msg,$tbl_user_role_list_function,$page_main;
	$js_src=<<<_js_src
<script type="text/javascript" src="/js/checkbox.js" ></script>
<script type="text/javascript">
function OnClick(role_name,obj){
	document.form1.target = "_self";
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.action_name.value = '97';
	document.form1.role_name.value = role_name;
	document.form1.submit();
}
function role_rename(role_name,obj){
	document.form1.target = "_blank";
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.action_name.value = '179';
	document.form1.role_name.value = role_name;
	var new_name = prompt("%s","");
	if(new_name==null) return;
	document.form1.new_name.value = new_name;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
function role_delete(role_name,role_id,obj){
	document.form1.target = "_blank";
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.action_name.value = '186';
	document.form1.role_name.value = role_name;
	document.form1.role_id.value   = role_id;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
function role_back(role_name,role_id,obj){
	document.form1.target = "_blank";
	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");
	document.form1.action_name.value = '187';
	document.form1.role_name.value = role_name;
	document.form1.role_id.value   = role_id;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
</script>
_js_src;
	printf ($js_src,$tbl_user_role_list_function[3],$tbl_sys_msg["reason"],$tbl_sys_msg["reason"],$tbl_sys_msg["reason"])."\n";
	form($var,$gm);
	echo "<input type='hidden' name='role_name' />\n";
	echo "<input type='hidden' name='role_id' />\n";
	echo "<input type='hidden' name='new_name' />\n";
	echo "<input type='hidden' name='reason' />\n";
	
	checkbox(false,true);
	ReturnAndBack();
	echo "<table class='list_table'>\n";
	echo "<tr><th style=\"text-align:left;\"><input type=\"checkbox\" onclick=\"checkboxAll(this,'role')\" />".$page_main["role_name"]."</th>\n<th>".$tbl_sys_msg["status"]."</th>\n<th>".$tbl_sys_msg["class"]."</th>\n<th>".$tbl_sys_msg["camp"]."</th>\n<th>".$page_main["level"]."</th>\n<th>".$tbl_sys_msg["CreateTime"]."</th><th>".$tbl_sys_msg["server"]."</th><th>".$page_main["type"]."</th><th>".$tbl_sys_msg["act"]."</th></tr>\n";
	
	$urs = $var["urs"];
	global $tbl_serv_partition;
	foreach($tbl_serv_partition[$var["gameId"]] as $group){
		foreach($group as $i){
			get_role_list($urs, $i);
		}
	}
	echo "</table>\n";
	ReturnAndBack();
	echo "</form>\n";
	return true;
}

function GetLoginInfo($var, $gm) {
	global $tbl_user_login,$tbl_game_server,$tbl_sys_msg,$tbl_serv_partition;
	echo "<h1>".$tbl_user_login[0]."</h1>\n";
	ReturnAndBack();
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_user_login[1][0]."</th><th>".$tbl_user_login[1][1]."</th><th>".$tbl_sys_msg["server"]."</th></tr>\n";
	
	$urs = $var["urs"];
	foreach($tbl_serv_partition[$var["gameId"]] as $group){
		foreach($group as $serv){
			$qs = "select us_uId from tbl_user_static where us_sName = '$urs'";
			$rs = MySQLQuery($qs,"Game",$serv);
			if($row = mysql_fetch_row($rs)){
				$us_uId = $row[0];
				break;
			}
		}
	}
	
	$rs = array();
	foreach($tbl_serv_partition[$var["gameId"]] as $group){
		foreach($group as $serv){
			$query = "select le.le_dtDateTime, lli.ll_sIP, $serv from tbl_log_login lli, tbl_log_user lu, tbl_log_event le where lli.le_uId = lu.le_uId and lli.le_uId = le.le_uId and lu.lus_uId = $us_uId ".getDateLimit($var,$serv);
			$result = MySQLQuery($query, "Log", $serv);
			while($row = mysql_fetch_row($result)){
				$rs[strtotime($row[0])] = $row;
			}
		}
	}
	
	ksort($rs);
	foreach($rs as $row){
		printf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td class='server' title='%d'>%s</td></tr>\n",$row[0],$row[1],$row[2],$tbl_game_server[$row[2]][2]);
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function GetUserByIP($var, $gm){
	global $tbl_user_login,$tbl_serv_db,$tbl_game_server,$tbl_sys_msg,$tbl_serv_partition;
	echo "<script src='/js/checkbox.js'></script>\n";
	echo '<script type="text/javascript">'."\n";
	echo 'function click(obj) {'."\n";
	echo '	document.form1.action_name.value = "50";'."\n";
	echo '	document.form1.serverid.value = $(obj).parents("tr").find(".server").attr("title");'."\n";
	echo '	document.form1.urs.value = $(obj).html();'."\n";
	echo '	document.form1.submit();'."\n";
	echo '}'."\n";
	echo '</script>'."\n";
	echo "<h1>".$tbl_user_login[2]."</h1>\n";
	ReturnAndBack();
	form($var,$gm);
	checkbox(treu,false);
	echo "<input type='hidden' name='urs' />\n";
	echo "</form>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_user_login[3][0]."</th><th style=\"text-align:left;\"><input type=\"checkbox\" onclick=\"checkboxAll(this,'user')\" />".$tbl_user_login[3][1]."</th><th>".$tbl_sys_msg["server"]."</th></tr>\n";
		
	$ip = $var["ip"];
	$rs = array();
	foreach($tbl_serv_partition[$var["gameId"]] as $group){
		foreach($group as $serv){
		
			$query = "select le.le_dtDateTime, lu.lus_uId, $serv from tbl_log_login lli, tbl_log_user lu, tbl_log_event le where lli.le_uId = lu.le_uId and lli.le_uId = le.le_uId and lli.ll_sIP = '$ip' ".getDateLimit($var,$serv);
			$result = MySQLQuery($query, "Log", $serv);
			while($row = mysql_fetch_row($result)){
				$qs = "select us_sName from tbl_user_static where us_uId = ".$row[1];
				$re = MySQLQuery($qs, "Game", $serv);
				$row[3] = "-";
				if($r = mysql_fetch_row($re)){
					$row[3] = $r[0];
				}
				$rs[strtotime($row[0])] = $row;
			}
		}
	}
	
	ksort($rs);
	foreach($rs as $row){
		printf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td><input type='checkbox' flag='user' onclick='Checkbox(this)' value='%s' /><a href='#' onclick='click(this)'>%s</a></td><td class='server' title='%d'>%s</td></tr>\n",$row[0],$row[3],$row[3],$row[2],$tbl_game_server[$row[2]][2]);
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function RoleSearch($var, $gm){
	global $tbl_role_search,$tbl_class,$tbl_camp,$tbl_sys_msg,$page_main;
	if($var["serverindex"]==""){
		echo "<script type=\"text/javascript\" src=\"/js/checkbox.js\" ></script>\n";
		echo "<script type=\"text/javascript\" src=\"/js/page.js\" ></script>\n";
		$js_str = <<<_js_str
<script type="text/javascript">
function OnClick(role_name,obj){
	document.form1.target = "_self";
	document.form1.serverid.value = $(obj).parents(".panel").attr("id").substring(4);
	document.form1.action_name.value = '97';
	document.form1.role_name.value = role_name;
	document.form1.submit();
}
</script>
_js_str;
		echo $js_str;
		form($var,$gm);
		echo "<input type='hidden' name='role_name' />\n";
		echo "<h1>".$tbl_role_search[0]."</h1>\n";
		checkbox(false,true);
		ReturnAndBack();
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		echo "</form>\n";
		return $ret;
	}
	$role_name = ArgRoleName();
	$serv_id = $var["serverindex"];
	$query = "select c.c_sName, us.us_sName, cb.cb_uLevel, ifnull(co_uOnServerId,0), cs.cs_uClass, cs.cs_uCamp, cs.cs_dtCreateDate, co.co_dtLastLoginTime from tbl_char c, tbl_char_static cs, tbl_user_static us, tbl_char_basic cb, tbl_char_onlinetime co left join tbl_char_online con on con.cs_uId = co.cs_uId where c.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId and cs.cs_uId = cb.cs_uId and cs.cs_uId = co.cs_uId and c.c_sName like '%$role_name%'";
	page($var, $query, 193, "Game", $serv_id, $result, $page, $desc);
	echo $desc;
	echo "<table class='list_table'>\n";
	echo "<tr><th style=\"text-align:left;\"><input type=\"checkbox\" onclick=\"checkboxAll(this,'role')\" />".$page_main["role_name"]."</th>\n<th style=\"text-align:left;\"><input type=\"checkbox\" onclick=\"checkboxAll(this,'user')\" />".$page_main["user_name"]."</th>\n<th>".$page_main["level"]."</th>\n<th>".$tbl_sys_msg["online"]."/".$tbl_sys_msg["offline"]."</th>\n<th>".$tbl_sys_msg["class"]."</th>\n<th>".$page_main["camp"]."</th><th>".$tbl_sys_msg["CreateTime"]."</th><th>".$tbl_sys_msg["LastLoginTime"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		printf("<tr><td><input type=\"checkbox\" onclick=\"Checkbox(this)\" flag=\"role\" value='%s' /><a href='#' onclick='OnClick(\"%s\",this)'>%s</a></td><td><input type=\"checkbox\" onclick=\"Checkbox(this)\" flag=\"user\" value='%s' />%s</td><td>%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$row[0],$row[0],$row[1],$row[1],$row[2],($row[3]!=0)?$tbl_sys_msg["online"]:$tbl_sys_msg["offline"],$tbl_class[$row[4]],$tbl_camp[$row[5]],$row[6],$row[7]);
	}
	echo $page;
	echo "</table>\n";
	return true;
}

?>
