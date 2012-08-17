<?php
function NpcInfo($var, $gm){
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$ret = eachServerFrame($var, $gm);
		return $ret;
	}
	global $tbl_npc_info,$tbl_camp,$bid2type,$tbl_jifen,$tbl_sys_msg,$page_main;
	$gm_name = mysql_escape_string($gm);

	$serverid =intval($var["serverindex"]);
	$action = "";
	
	$npc_name = $var["npc_name"];
	if($npc_name==""){
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	$npc_name = str_replace(" ","+",$npc_name);
	$param = array();
	$param["npc_name"] = Base642Local($npc_name);

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "GetNpcInfo", $param);
	
	$xml = new DOMDocument();
	$xml->loadXML(Local2UTF8($ret));

	$root = $xml->getElementsByTagName("root")->item(0);
	
	$npc = $root->getElementsByTagName("npc")->item(0);
	echo "<h2>".$tbl_npc_info[0]."</h2>\n";
	echo "<table class='form_table'>\n";
	$npc_name = $npc->getElementsByTagName("npc-name")->item(0)->nodeValue;
	echo "<tr><th>".$page_main["npc_name"]."</th><td>".Lan_Npc_GetDisplayName($npc_name,false)."</td></tr>\n";
	echo "<tr><th>".$page_main["real_name"]."</th><td>".$npc_name."</td></tr>\n";
	echo "<tr><th>Base 64</th><td>".UTF82Base64($npc_name)."</td></tr>\n";
	echo "<tr><th>".$tbl_sys_msg["camp"]."</th><td>".$tbl_camp[intval($npc->getElementsByTagName("npc-camp")->item(0)->nodeValue)]."</td></tr>\n";
	echo "</table>\n";
	
	$questset = $npc->getElementsByTagName("npc-quests")->item(0);
	if($questset){
		echo "<h2>".$tbl_npc_info[1]."</h2>\n";
		$npc_pos = $questset->getElementsByTagName("npc-quest-pos")->item(0);
		echo "<table class='list_table'>\n";
		echo "<tr><th>".$page_main["scene"]."</th><th>".$page_main["pos"]."</th></tr>\n";
		echo "<tr><td>".Lan_Scene_GetDisplayName($npc_pos->getAttribute("scene"))."</td><td>".$npc_pos->getAttribute("x").",".$npc_pos->getAttribute("y")."</td></tr>\n";
		echo "</table>\n";
		echo "<table class='list_table'>\n";
		echo "<tr><th>".$page_main["task"]."</th></tr>\n";
		$npc_quests = $questset->getElementsByTagName("npc-quest");
		foreach($npc_quests as $quest){
			echo "<tr><td>".Lan_Quest_GetDisplayName($quest->nodeValue)."</td></tr>\n";
		}
		echo "</table>\n";
	}
	
	$shop = $npc->getElementsByTagName("npc-shop")->item(0);
	if($shop){
		echo "<h2>".$tbl_npc_info[4]."</h2>\n";
		echo "<table class='form_table'>\n";
		$shop_pay = $shop->getElementsByTagName("npc-shop-pay")->item(0);
		if($shop_pay){
			echo "<tr><th>".$page_main["category"]."</th><td>".$tbl_jifen[intval($shop_pay->nodeValue)]."</td></tr>\n";
		}
		echo "</table>\n";
		echo "<table class='list_table'>\n";
		echo "<tr><th>".$page_main["type"]."</th><th>".$page_main["item"]."</th></tr>\n";
		$shop_items = $shop->getElementsByTagName("npc-shop-items")->item(0)->getElementsByTagName("npc-shop-item");
		foreach($shop_items as $shop_item){
			echo "<tr><td>".$bid2type[intval($shop_item->getAttribute("type"))]."</td><td>".Lan_Item_GetDisplayName($shop_item->nodeValue)."</td></tr>\n";
		}
	}
	
	$drop = $npc->getElementsByTagName("npc-drop")->item(0);
	if($drop){
		echo "<h2>".$tbl_npc_info[2]."</h2>\n";
		echo "<table class='form_table'>\n";
		$drop_scene = $drop->getElementsByTagName("npc-drop-scene")->item(0)->nodeValue;
		if($drop_scene){
			echo "<tr><th>".$page_main["scene"]."</th><td>".Lan_Scene_GetDisplayName($drop_scene)."</td></tr>\n";
		}
		$drop_level = $drop->getElementsByTagName("npc-drop-level")->item(0)->nodeValue;
		if($drop_level){
			echo "<tr><th>".$page_main["level"]."</th><td>".$drop_level."</td></tr>\n";
		}
		echo "</table>\n";
		$drop_teams = $drop->getElementsByTagName("npc-drop-team");
		foreach($drop_teams as $drop_team){
			$group_rate = $drop_team->getAttribute("rate");
			$drop_team_num = $drop_team->getElementsByTagName("npc-drop-team-num")->item(0);
			$group_number = $drop_team_num->getAttribute("min")."-".$drop_team_num->getAttribute("max");
			echo "<table class='list_table'>\n";
			echo "<tr><th style='width:50%'>".$page_main["item"]."</th><th style='width:25%;'>".$tbl_npc_info[3]."</th><th>".$page_main["number"]."</th></tr>\n";
			$drop_items = $drop_team->getElementsByTagName("npc-drop-team-item");
			foreach($drop_items as $drop_item){
				echo "<tr><td>".Lan_Item_GetDisplayName($drop_item->nodeValue)."</td><td>".$group_rate."*".$drop_item->getAttribute("rate")."</td><td>".$group_number."</td></tr>\n";
			}
			echo "</table>\n";
		}
	}
	return true;
}

function NpcNameMatch($var, $gm)
{
	global $page_main;
	$ret = Lan_NPC_SearchByDisplayName($var["str_match"]);
	echo "<select name=\"npc_name\">\n";
	echo "<option>".$page_main["select_action"]."</option>\n";
	foreach($ret as $item){
		printf("<option value=\"%s\">%s</option>\n",$item["RealName"], $item["DisplayName"]."(".Base642UTF8($item["RealName"]).")");
	}
	printf("</select>");
	return true;
}
?>
