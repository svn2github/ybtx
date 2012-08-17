<?php
function GetGMActionLog($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_game_server,$page_action_name;
	if($var["page"]=="" and $var["page_field"]=="" and $var["flag"]=="" and $var["role_name"]==""){
		echo "<script type='text/javascript' src='/js/page.js'></script>\n";
		echo "<script type='text/javascript' src='/js/log_info.js'></script>\n";
		echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
		form($var,$gm);
		checkbox(true,true);
		echo "<input type='hidden' name='flag' />\n";
		echo "<input type='hidden' name='role_name' />\n";
		echo "<input type='hidden' name='str' />\n";
		echo "<input type='hidden' name='urs' />\n";
		ReturnAndBack();
		echo "<div class='panel' id='div_0'>\n";
	}
	
	$filter = "";
	if($var["flag"]!="" || $var["role_name"]!=""){
		$filter = " where";
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
	$query="select gcl_sGmName, gcl_fAction, gcl_sReason, gcl_uServerId, gcl_tUptime from tbl_gm_command_log".$filter." order by gcl_tUptime desc";
	page($var, $query, 62, "GMS", 0, $result, $page, $desc);
	
	echo "<table class='list_table'>";
	echo "	<tr>";
	echo "		<th style='width:60px;' class='f0'>".$page_main["user_name"].page_filter("gcl_sGmName", "f0", 0, $query, "GMS", 0)."</th>";
	echo "		<th style='width:160px;' class='f1'>".$page_main["event_type"].page_filter("gcl_fAction", "f1", 0, $query, "GMS", 0, "GetGMCommandLogAction")."</th>";
	echo "		<th class='f2'>".$tbl_sys_msg["desc"]."<div class='filter' style='display:none;'><a href='#' onclick=\"toNoFilter(this)\">".$tbl_sys_msg["all"]."</a></div>";
	echo "<script type=\"text/javascript\">\n";
	echo "	$(\"#div_0 .f2\").css(\"cursor\",\"pointer\");\n";
	echo "	$(\"#div_0 .f2\").hover(\n";
	echo "	function(){\n";
	echo "		$(this).find(\"div.filter\").slideDown();\n";
	echo "	},\n";
	echo "	function(){\n";
	echo "		$(this).find(\"div.filter\").hide();\n";
	echo "	}\n";
	echo ");\n";
	echo "</script>\n";
	echo "		</th>";
	echo "		<th style='width:70px;'>".$tbl_sys_msg["server"]."</th>";
	echo "		<th style='width:130px;'>".$tbl_sys_msg["time"]."</th>";
	echo "	</tr>\n";
	
	while($row=mysql_fetch_row($result)){
		printf("<tr><td align='center'>%s</td><td>%s</td><td>%s</td><td align='center' class='server' title='%d'>%s</td><td align='center'>%s</td></tr>\n",$row[0],GetGMCommandLogAction($row[1]),LogInfo($row[2]),$row[3],$tbl_game_server[$row[3]][2],$row[4]);
	}
	echo "</table>\n";
	echo $page;
	if($var["page"]=="" and $var["page_field"]=="" and $var["flag"]=="" and $var["role_name"]==""){
		echo "</div>\n";
		ReturnAndBack();
		echo "</form>\n";
	}
	return true;
}

?>
