<?php
function RetComplaint($tblTitle,$tblContent,$var)
{	
	$str="";
	$str=$str.sprintf("<html><title>list</title><head>");
	$str=$str.sprintf("<script type=\"text/javascript\">");
	$strjs = <<<_strjs
	<!--//
function pass(id,role)
{
	document.form1.action_name.value='59';
	document.form1.id.value=id;
	document.form1.role_name.value=role;
	document.getElementById("mail").style.display = "block";
}

function cancel(id)
{
	document.form1.action_name.value='60';
	document.form1.id.value=id;
	document.form1.submit();
}	
	//-->
_strjs;
	$str=$str.$strjs;
	$str=$str.sprintf("</script>\n");
	$str=$str.sprintf("</head>");
	$str=$str.sprintf("<body>\n");
	$str=$str.sprintf("<form method='get' name='form1' action='/cgi-bin/gm_tools/FrameWork.php'>\n");
	$str=$str.sprintf("<table border='1' width='780px'><tr bgcolor='#9acd32'>");
	for ($i=0; $i<count($tblTitle); $i++){
		$str=$str.sprintf("<th>%s</th>\n", $tblTitle[$i]);
	}
	
	$str=$str.sprintf("<th>action</th></tr>\n");
	global $tbl_sys_msg;

	for ($j=0; $j<count($tblContent); $j++){
		$color = '#FFFFFF';
		if ($i % 2)
			$color='#d8d8d8';
		$str=$str.sprintf("<tr bgcolor='%s'>\n", $color);
		for ($k=1; $k<count($tblContent[$j]); $k++){
			$str=$str.sprintf("<td>&nbsp %s</td>\n", $tblContent[$j][$k]);
		}
		$str=$str.sprintf("<td><input type=button value='%s' onclick='pass(%d,%s)'/>&nbsp<input type=button value='%s' onclick='cancel(%d)'/></td></tr>\n", $tbl_sys_msg["pass"], $tblContent[$j][0], $tblContent[$j][2], $tbl_sys_msg["cancel"], $tblContent[$j][0]);
	}

	$str=$str.sprintf("</table>");
	$str=$str.SetCommonVar($var);
	$str=$str.sprintf("<div style='display:none;' id='mail'>");
	$str=$str.sprintf("<input type='text' name='role_name'/><br/>");
	$str=$str.sprintf("<input type='text' name='title' value='Please type mail's TITLE here.'/><br/>");
	$str=$str.sprintf("<textarea name='content'/>Please type mail's CONTENT here.</textarea><br/>");
	$str=$str.sprintf("<input type='hidden' name='action_name'/>");
	$str=$str.sprintf("<input type='hidden' name='id'/>");
	$str=$str.sprintf("<input type='submit' value='send'/>");
	$str=$str.sprintf("</div>");
	$str=$str.sprintf("</form></body></html>\n");




	printf($str);

}

function GetAllComplaints($var, $gm)
{
	global $tbl_list_complaint;
	if($var["serverindex"]==null || $var["serverindex"]==""){
		echo "<script src='/js/page.js'></script>\n";
		echo "<script src='/js/checkbox.js'></script>\n";
		form($var,$gm);
		checkbox(true,true);
		ReturnAndBack();
		echo "</form>\n";
		return eachServerFrame($var, $gm);
	}else{
		$serv_id = intval($var["serverindex"]);
	}
	
	global $page_main,$tbl_sys_msg;
	$query_string="select ua.ua_nMsgType, us.us_sName, c.c_sName, ua.ua_sErrorMsg, ua.ua_dtTime, ua.ua_uErrorCode from tbl_user_advice ua, tbl_char c, tbl_char_static cs, tbl_user_static us where ua.cs_uId = c.cs_uId and c.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId order by ua_dtTime desc";
	page($var,$query_string, 58, "Game", $serv_id, $result, $page, $desc);
	echo "<table class='list_table'>\n";
	echo "<tr><th class='type' style='width:60px;'>".$page_main["category"]."</th><th style='width:80px;'>".$page_main["user_name"]."</th><th style='width:80px;'>".$page_main["role_name"]."</th><th>".$page_main["content"]."</th><th style='width:40px;'>".$tbl_sys_msg["status"]."</th><th style='width:120px;'>".$tbl_sys_msg["time"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		printf("<tr><td>%s</td><td><input type='checkbox' flag='user' value='%s' onclick='Checkbox(this)' />%s</td><td><input type='checkbox' flag='role' value='%s' onclick='Checkbox(this)' />%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$tbl_list_complaint[$row[0]],$row[1],$row[1],$row[2],$row[2],$row[3],"-",$row[4]);
	}
	echo "</table>\n";
	echo $page;
	return true;
}

function PassComplaint($var, $gm)
{
	$id = intval($var["id"]);
	$role_name = $var["role_name"];
	$title = $var["title"];
	$content = $var["content"];
	
	$serv_id = intval($var["serverid"]);

	$qs = "update tbl_player_complaint set state = 1 where pc_uId = '$id'";
	$result = MySQLQuery($qs, "GMS");
	
	SendMail($serv_id, $role_name, $title, $content);
	
	PrintNotice("Pass!");
	return true;
}

function CancelComplaint($var, $gm)
{
	$id = intval($var["id"]);
	$serv_id = intval($var["serverid"]);

	$qs = "update tbl_player_complaint set state = 2 where pc_uId = '$id'";
	$result = MySQLQuery($qs, "GMS");
	PrintNotice("Cancel!");
	return true;
}
?>
