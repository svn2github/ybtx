<?php
function RoleToUser($var, $gm){
	global $page_main;
	if($var["serverindex"]==null || $var["serverindex"]==""){
		echo "<script src='/js/checkbox.js' type='text/javascript'></script>\n";
		form($var, $gm);
		checkbox(true,false);
		echo "</form>\n";
		ReturnAndBack();
		$ret = eachServerFrame($var, $gm);
		ReturnAndBack();
		return $ret;
	}
	$serverid = $var["serverindex"];
	$role_names = $var["role_name"];
	$role_names = implode("','",explode(",",$role_names));
	$qs = "select distinct us.us_uId, us.us_sName from tbl_char_static cs, tbl_char c, tbl_user_static us where cs.cs_uId = c.cs_uId and cs.us_uId = us.us_uId and c.c_sName in ('".$role_names."');";
	$result = MySQLQuery($qs, "Game", $serverid);
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$page_main["user_id"]."</th><th>".$page_main["user_name"]."</th></tr>\n";
	while($row = mysql_fetch_row($result)){
		printf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td><input type='checkbox' flag='user' onclick='Checkbox(this);' value='%s'/>%s</td></tr>\n",$row[0],$row[1],$row[1]);
	}
	echo "</table>\n";
	return true;
}
?>
