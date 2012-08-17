<?php
function GetFriends($var, $gm){
	global $tbl_role_all_detail;
	echo "<h1>$tbl_role_all_detail[16]</h1>\n";
	ReturnAndBack();
	Reload();
	$role_id = ArgRoleId();
	echo "<table class='list_table'>\n";
	echo sprintf("<tr><td>$tbl_role_all_detail[18]</td></tr>\n", ArgRoleName()."(".$role_id.")");
	echo "</table>\n";
	
	global $tbl_role_friends,$tbl_camp,$tbl_class,$tbl_sex,$tbl_sys_msg;
	$serverid = $var["serverid"];
	
	
	if(!$role_id) return true;
	
	echo "<h1>".$tbl_role_friends[0]."</h1>\n";
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</th></tr>\n",$tbl_role_friends[1][0],$tbl_role_friends[1][1],$tbl_role_friends[1][2],$tbl_role_friends[1][3],$tbl_role_friends[1][4],$tbl_role_friends[1][5]);
	$rs = MySQLQuery("select c.c_sName, cs.cs_uCamp, cs.cs_uClass, cb.cb_uLevel, cs.cs_uSex, fc.fc_sName from tbl_player_friends pf, tbl_friends_class fc, tbl_char c, tbl_char_basic cb, tbl_char_static cs where pf.fc_uId = fc.fc_uId and pf.cs_uId = fc.cs_uId and pf.pf_uFriendId = c.cs_uId and fc.fc_uId<>2 and pf.cs_uId='$role_id' and cb.cs_uId = c.cs_uId and cs.cs_uId = c.cs_uId", "Game", $serverid);
	while($row = mysql_fetch_row($rs)){
		if($row[5]==UTF82Local($tbl_role_friends[1][6])){
			$row[5] = $tbl_role_friends[1][6];
		}
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[5],$row[0],$tbl_camp[$row[1]],$tbl_class[$row[2]],$row[3],$tbl_sex[$row[4]]);
	}
	echo "</table>\n";
	
	echo "<h1>".$tbl_role_friends[2]."</h1>\n";
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</th></tr>\n",$tbl_role_friends[3][0],$tbl_role_friends[3][1],$tbl_role_friends[3][2],$tbl_role_friends[3][3],$tbl_role_friends[3][4]);
	$rs = MySQLQuery("select c.c_sName, cs.cs_uCamp, cs.cs_uClass, cb.cb_uLevel, cs.cs_uSex from tbl_player_friends pf, tbl_char c, tbl_char_basic cb, tbl_char_static cs where pf.pf_uFriendId = c.cs_uId and pf.cs_uId='$role_id' and pf.fc_uId=2 and cb.cs_uId = c.cs_uId and cs.cs_uId = c.cs_uId", "Game", $serverid);
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_camp[$row[1]],$tbl_class[$row[2]],$row[3],$tbl_sex[$row[4]]);
	}
	echo "</table>\n";
	
	echo "<h1>".$tbl_role_friends[4]."</h1>\n";
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_role_friends[5][0],$tbl_role_friends[5][1],$tbl_role_friends[5][2]);
	$rs = MySQLQuery("select fg.fg_sName,gm1.gm_uIdentity,count(*) from tbl_group_manager gm1,tbl_group_manager gm2,tbl_friends_group fg where gm1.cs_uId='$role_id' and gm1.fg_uId = gm2.fg_uId and gm1.fg_uId = fg.fg_uId group by gm2.fg_uId", "Game", $serverid);
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%d</td></tr>\n",$row[0],$tbl_role_friends[5][3][intval($row[1])],$row[2]);
	}
	echo "</table>\n";
	
	echo "<h1>".$tbl_role_friends[6]."</h1>\n";
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_role_friends[7][0],$tbl_role_friends[7][1],$tbl_role_friends[7][2]);
	$rs = MySQLQuery("select t.t_sName, mt1.mt_sPosition, count(*) from tbl_member_tong mt1, tbl_tong t, tbl_member_tong mt2 where mt1.t_uId = t.t_uId and t.t_uId = mt2.t_uId and mt1.cs_uId = '$role_id' group by t.t_uId", "Game", $serverid);
	while($row = mysql_fetch_row($rs)){
		global $tbl_tong_pos;
		printf("<tr><td>%s</td><td>%s</td><td>%d</td></tr>\n",$row[0],$tbl_tong_pos[intval($row[1])],$row[2]);
	}
	echo "</table>\n";
	return true;
}
?>
