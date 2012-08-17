<?php
function GetBackEquip($var, $gm)
{
	global $tbl_sys_msg,$page_main,$tbl_role_equip_info;
	$js_str=<<<_js_str
	<script type="text/javascript" src="/js/page.js"></script>
	<script type="text/javascript">
	function getback(){
		document.form1.target = "_block";
		document.form1.action_name.value="166";
		document.form1.submit();
	}
	</script>
_js_str;

	$server_id = $var["serverid"];
	$item_id = $var["item_id"];
	$event_id = $var["event_id"];
	$str = " order by le_uId desc limit 1";
	if($event_id != null || $event_id != ""){
		$str = " and le_uId <= $event_id".$str;
	}

	$allowe = HasAuth(166,$gm);
	
	$qs = "select lcs_uId, le.le_dtDateTime, let.let_uEventType, le.le_uId from tbl_log_item_del lid, tbl_log_player_giver lpg, tbl_log_event le, tbl_log_event_type let where le.le_uId = lid.le_uId and let.le_uId = lid.le_uId and lid.le_uId = lpg.le_uId and lis_uId = $item_id";
	$rs = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($rs)){
		global $tbl_event_type;
		$desc = ShowVirWealthLog_Desc($row[2],$row[3],$server_id);
		echo "<table class='list_table'>\n";
		printf("<tr><td style='width:120px;'>%s</td><td style='width:250px;'>%s</td><td>%s</td></tr>\n",$row[1],$tbl_event_type[$row[2]],$desc);
		echo "</table>\n";
		$role_id = $row[0];
		$role_name = GetRoleName($role_id, $server_id);
		if($role_name == "-"){
			$allowe = false;
		}
	}else{
		//$allowe = false;
	}
	
	printf($js_str,$tbl_sys_msg["reason"])."\n";
	form($var, $gm);
	$style = <<<_style
<style>
.equip {
	width: 230px;
	border: 1px solid #ccc;
	padding:10px;
	float:left;
}
.equip div{
	float:left;
}
.equip .head, .equip .binding, .equip .durability, .equip .armor, .equip .armor .armor_item, .equip .advance, .equip .advance_item, .equip .intensify, .equip .intensify .intensify_item, .equip .equip_info .armor_item, .equip .JingLing {
	width: 230px;
}
.equip .head .name {
	float:left;
}
.equip .head .level, .equip .equip_info .weapon_speed, .equip .intensify .intensify_total, .equip .advance .advance_times, .equip .addation, .equip .JingLingSkill {
	float:right;
	text-align:right;
	clear: right;
}
.equip .binding, .equip .durability {
	text-align:right;
}
.equip .armor .armor_item .armor_name, .equip .advance .advance_item .advance_name, .equip .intensify .intensify_name, .equip .equip_info .armor_item .armor_name {
	width: 80px;
}
.equip .armor .armor_item .armor_value,.equip .advance .advance_item .advance_value,.equip .intensify .intensify_value,.equip .equip_info .armor_item .armor_value {
	width: 30px;
}
.equip .armor, .equip .advance, .equip .intensify, .equip .JingLing {
	margin-top: 5px;
}
</style>
_style;
	echo $style;
	echo "<div class='equip'>\n";
	
	$qs = "select * from tbl_log_item_static where lis_uId = $item_id";
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$item_type = $row[1];
		$item_name = Local2UTF8($row[2]);
	}else{
		return false;
	}
	
	$attr = array();
	$attr["Static"] = array();
	$attr["ShieldAttr"] = array();
	$attr["Advance"] = array();
	$attr["Armor1"] = array();
	$attr["Armor2"] = array();
	$attr["Armor3"] = array();
	$attr["Armor4"] = array();
	$attr["Intensify"] = array();
	$attr["InTensifyAddAttr"] = array();
	$attr["Durability"] = array();
	$attr["Binding"] = array();
	$attr["SuperAddation"] = array();
	
	switch($item_type){
		case 5: //weapon
			$qs = "select * from tbl_log_item_weapon where lis_uId = $item_id".$str;
			$result = MySQLQuery($qs, "Log", $server_id);
			if($row = mysql_fetch_row($result)){
				$attr["Static"] = $row;
				$level = $row[2];
				$intensify = $row[7];
				$inten = $row[8];
				$DPS = $row[4];
				$speed = $row[5];
				$rate = $row[6];
				$min_attack = intval($DPS*$speed*(1-($rate*0.01)));
				$max_attack = intval($DPS*$speed*(1+($rate*0.01)));
				$equip_info = "<div class='weapon_attack'>".sprintf($tbl_role_equip_info[1][0][0],$min_attack,$max_attack)."<br/>".sprintf($tbl_role_equip_info[1][0][2],$DPS)."</div>\n<div class='weapon_speed'>".sprintf($tbl_role_equip_info[1][0][1],$speed)."</div>\n";
			}
			break;
		case 6: //armor
		case 9: //Jewelry
			$qs = "select * from tbl_log_item_armor where lis_uId = $item_id".$str;
			$result = MySQLQuery($qs, "Log", $server_id);
			if($row = mysql_fetch_row($result)){
				$attr["Static"] = $row;
				$level = $row[2];
				$intensify = $row[8];
				$inten = $row[9];
				
				$equip_info = "<div class='armor_item'><div class='armor_name'>".$tbl_role_equip_info[1][1][0]."</div><div class='armor_value'>+".$row[4]."</div></div>\n";
				$equip_info.= "<div class='armor_item'><div class='armor_name'>".$tbl_role_equip_info[1][1][1]."</div><div class='armor_value'>+".$row[5]."</div></div>\n";
				$equip_info.= "<div class='armor_item'><div class='armor_name'>".$tbl_role_equip_info[1][1][2]."</div><div class='armor_value'>+".$row[6]."</div></div>\n";
			}
			break;
		case 7: //shield
			$qs = "select * from tbl_log_item_shield where lis_uId = $item_id".$str;
			$result = MySQLQuery($qs, "Log", $server_id);
			if($row = mysql_fetch_row($result)){
				$attr["Static"] = $row;
				$level = $row[2];
				$intensify = $row[4];
				$inten = $row[5];
			}
			$qs = "select * from tbl_log_item_shield_Attr where lis_uId = $item_id".$str;
			$result = MySQLQuery($qs, "Log", $server_id);
			if($row = mysql_fetch_row($result)){
				$attr["ShieldAttr"] = $row;
			}
			break;
		case 8: //ring 
			$qs = "select * from tbl_log_item_ring where lis_uId = $item_id".$str;
			$result = MySQLQuery($qs, "Log", $server_id);
			if($row = mysql_fetch_row($result)){
				$attr["Static"] = $row;
				$level = $row[2];
				$intensify = $row[6];
				$inten = $row[7];
				$DPS = $row[4];
				$equip_info = "<div class='ring_attack'>".sprintf($tbl_role_equip_info[1][2][0],$DPS)."</div>\n";
			}
			break;
	}
	
	$addation = 0;
	$qs = "select * from tbl_log_item_equip_superaddation where lis_uId = $item_id";
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["SuperAddation"] = $row;
		$addation = $row[2];
	}
	
	$addation_rate = 0;
	$addation_time = "";
	$addation_desc = "";
	switch($addation){
		case 1:
			$addation_rate = 10;
			break;
		case 2:
			$addation_rate = 21;
			break;
		case 3:
			$addation_rate = 33;
			break;
		case 4:
			$addation_rate = 46;
			break;
		case 5:
			$addation_rate = 60;
			break;
		case 6:
			$addation_rate = 75;
			break;
		case 7:
			$addation_rate = 91;
			break;
		case 8:
			$addation_rate = 108;
			break;
	}
	if($addation > 0){
		$addation_time = "&nbsp;+".$addation;
		$addatiom_desc = "<div class='addation'>(".$tbl_role_equip_info[6].$addation_rate."%)</div>\n";
	}
	
	$inten_times = 0;
	$qs = "select * from tbl_log_item_equip_intensify where lis_uId = $item_id".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Intensify"] = $row;
		$inten_times = $row[11];
		$inten_totle = $row[15];
		$suit_info = Lan_Equip_GetSuitInfo(Local2UTF8($row[13]),$row[17]);
		$suit_name = $suit_info["name"];
		if($row[7])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[7]))."</div>\n<div class='intensify_value'>+".$row[8]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[8]/100).")</div>\n")."</div>\n";
		if($row[9])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[9]))."</div>\n<div class='intensify_value'>+".$row[10]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[10]/100).")</div>\n")."</div>\n";
	}
	
	if($suit_name<>"") $suit_name .= "<br/>";
	
	echo "<div class='head'>\n<div class='name'>".$suit_name.Lan_Item_GetDisplayName($item_name,$item_tupe,false).$addation_time."</div>\n<div class='level'>".$level.$tbl_role_equip_info[7]."</div>\n".$addatiom_desc."</div>\n";
	
	$qs = "select * from tbl_log_item_binding where lis_uId = $item_id".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		if($row[2]>0){
			global $tbl_item_is_binding;
			echo "<div class='binding'>".$tbl_item_is_binding[$row[2]]."</div>\n";
		}
		$row[2]=2;
		$attr["Binding"] = $row;
	}
	
	$qs = "select * from tbl_log_item_equip_durability where lis_uId = $item_id".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Durability"] = $row;
		echo "<div class='durability'>".sprintf($tbl_role_equip_info[0],$row[3],$row[2])."</div>\n";
	}
	
	echo "<div class='equip_info'>\n$equip_info</div>\n";
	
	echo "<div class='armor'>\n";
	$qs = "select * from tbl_log_item_equip_armor where lis_uId = $item_id and liea_uIndex = 1".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Armor".$row[4]] = $row;
		echo "<div class='armor_item'>\n";
		$armor = Lan_Equip_GetArmorPiece(Local2UTF8($row[2]));
		echo "<div class='armor_name'>".$armor[1]."</div>\n";
		echo "<div class='armor_value'>+".($armor[2]*$row[3])."</div>\n";
		if($armor[0]>1){
			echo "<div class='aromr_time'>(".$row[3]."/".$armor[0].")</div>\n";
		}
		echo "</div>\n";
	}
	$qs = "select * from tbl_log_item_equip_armor where lis_uId = $item_id and liea_uIndex = 2".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Armor".$row[4]] = $row;
		echo "<div class='armor_item'>\n";
		$armor = Lan_Equip_GetArmorPiece(Local2UTF8($row[2]));
		echo "<div class='armor_name'>".$armor[1]."</div>\n";
		echo "<div class='armor_value'>+".($armor[2]*$row[3])."</div>\n";
		if($armor[0]>1){
			echo "<div class='aromr_time'>(".$row[3]."/".$armor[0].")</div>\n";
		}
		echo "</div>\n";
	}
	$qs = "select * from tbl_log_item_equip_armor where lis_uId = $item_id and liea_uIndex = 3".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Armor".$row[4]] = $row;
		echo "<div class='armor_item'>\n";
		$armor = Lan_Equip_GetArmorPiece(Local2UTF8($row[2]));
		echo "<div class='armor_name'>".$armor[1]."</div>\n";
		echo "<div class='armor_value'>+".($armor[2]*$row[3])."</div>\n";
		if($armor[0]>1){
			echo "<div class='aromr_time'>(".$row[3]."/".$armor[0].")</div>\n";
		}
		echo "</div>\n";
	}
	$qs = "select * from tbl_log_item_equip_armor where lis_uId = $item_id and liea_uIndex = 4".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Armor".$row[4]] = $row;
		echo "<div class='armor_item'>\n";
		$armor = Lan_Equip_GetArmorPiece(Local2UTF8($row[2]));
		echo "<div class='armor_name'>".$armor[1]."</div>\n";
		echo "<div class='armor_value'>+".($armor[2]*$row[3])."</div>\n";
		if($armor[0]>1){
			echo "<div class='aromr_time'>(".$row[3]."/".$armor[0].")</div>\n";
		}
		echo "</div>\n";
	}
	echo "</div>\n";
	
	$qs = "select * from tbl_log_item_equip_advance where lis_uId = $item_id".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["Advance"] = $row;
		echo "<div class='advance'>\n";
		$advance_name = Local2UTF8($row[16]);
		$advance_cur_tiems = $row[2];
		$advance_tot_tiems = $row[3];
		if($advance_tot_tiems>0)
			echo "<div class='advance_item'>\n<div class='advance_name'>".sprintf($tbl_role_equip_info[2],$advance_name)."</div>\n<div class='advance_value'>".$advance_cur_tiems."/".$advance_tot_tiems."</div>\n<div class='advance_times'>".sprintf($tbl_role_equip_info[5],$row[11])."</div>\n</div>\n";
		if($row[12]!=0)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[17]))."</div>\n<div class='advance_value'>+".$row[12]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[12]/100).")</div>\n")."</div>\n";
		if($row[13]!=0)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[18]))."</div>\n<div class='advance_value'>+".$row[13]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[13]/100).")</div>\n")."</div>\n";
		if($row[14]!=0)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[19]))."</div>\n<div class='advance_value'>+".$row[14]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[14]/100).")</div>\n")."</div>\n";
		if($row[15]!=0)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[20]))."</div>\n<div class='advance_value'>+".$row[15]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[15]/100).")</div>\n")."</div>\n";
		$JingLingType = Local2UTF8($row[9]);
		$JingLingSkill = $row[8];
		echo "</div>\n";
	}
	
	global $tbl_item_inten_soul;
	
	$qs = "select * from tbl_log_item_equip_intensifyAddAttr where lis_uId = $item_id".$str;
	$result = MySQLQuery($qs, "Log", $server_id);
	if($row = mysql_fetch_row($result)){
		$attr["InTensifyAddAttr"] = $row;
		if($row[2])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[2]))."</div>\n<div class='intensify_value'>+".$row[3]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[3]/100).")</div>\n")."</div>\n";
		if($row[4])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[4]))."</div>\n<div class='intensify_value'>+".$row[5]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[5]/100).")</div>\n")."</div>\n";
		if($row[6])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[6]))."</div>\n<div class='intensify_value'>+".$row[7]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[7]/100).")</div>\n")."</div>\n";
		if($row[8])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[8]))."</div>\n<div class='intensify_value'>+".$row[9]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[9]/100).")</div>\n")."</div>\n";
		if($row[10])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[10]))."</div>\n<div class='intensify_value'>+".$row[11]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[11]/100).")</div>\n")."</div>\n";
		if($row[12])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[12]))."</div>\n<div class='intensify_value'>+".$row[13]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[13]/100).")</div>\n")."</div>\n";
	}
	
	echo "<div class='intensify'>\n";
	echo "<div class='intensify_item'>\n";
	if($intensify>0)
		echo "<div class='intensify_name'>".sprintf($tbl_role_equip_info[3],$tbl_item_inten_soul[Local2base64($inten)])."</div>\n<div class='intensify_value'>$inten_times/$intensify</div>\n";
	if($inten_totle){
		echo "<div class='intensify_total'>".sprintf($tbl_role_equip_info[4],$inten_totle)."</div>\n";
	}
	echo "</div>\n";
	echo $inten_info;
	echo "<div class='intensify_item'>\n";
	if($suit_info["name"]!=null){
		echo $suit_info["name"]."\n";
	}
	echo "<div style='margin-left: 10px'>\n";
	if($suit_info["part"]!=null){
		foreach($suit_info["part"] as $part){
			echo $suit_info["name"].$part."<br/>\n";
		}
	}
	if($suit_info["desc"]!=null){
		foreach($suit_info["desc"] as $i => $desc){
			printf($tbl_role_equip_info[8],$i);
			echo $desc."<br/>\n";
		}
	}
	echo "</div>\n";
	if($suit_info["talent"]!=null){
		echo "<div>".$tbl_role_equip_info[9].$suit_info["talent"]."</div>\n";
	}
	echo "</div>\n";
	echo "</div>\n";
	global $tbl_JingLing_type;
	echo "<div class='JingLing'><div>".$tbl_JingLing_type[UTF82Base64($JingLingType)]."</div>";
	if($JingLingSkill>0){
		echo "<div class='JingLingSkill'>".Lan_Equip_GetSkillName($JingLingType,$JingLingSkill)."</div>";
	}
	echo "</div>\n";
	echo "</div>\n";
	echo "</div>\n";
	if($allowe){
		$attrstr = $item_name.",".$item_type.",".$item_id.";";
		$attrstr.= attrtostr($attr["Static"]).";";
		$attrstr.= attrtostr($attr["ShieldAttr"]).";";
		$attrstr.= attrtostr($attr["Advance"]).";";
		$attrstr.= attrtostr($attr["Armor1"]).";";
		$attrstr.= attrtostr($attr["Armor2"]).";";
		$attrstr.= attrtostr($attr["Armor3"]).";";
		$attrstr.= attrtostr($attr["Armor4"]).";";
		$attrstr.= attrtostr($attr["Intensify"]).";";
		$attrstr.= attrtostr($attr["InTensifyAddAttr"]).";";
		$attrstr.= attrtostr($attr["Durability"]).";";
		$attrstr.= ",,2,;";
		$attrstr.= attrtostr($attr["SuperAddation"]).";";
		echo "<input type='hidden' name='attr' value='$attrstr'/>\n";
		echo "<table style='float: left; margin-left: 20px;'>\n";
		echo "<tr><th>".$page_main["role_name"]."</th><td><input type='text' name='role_name' value='$role_name'".(($role_name=="")?"":" readonly=\"readonly\"")." /></td></tr>\n";
		echo "<tr><th>".$tbl_sys_msg["reason"]."</th><td><textarea name='reason' style='width:300px;'></textarea></td></tr>\n";
		echo "<tr><th>&nbsp;</th><td><input type='button' onclick='getback()' value='".$tbl_sys_msg["submit"]."' /></td></tr>\n";
		echo "</table>\n";
	}
	echo "</form>\n";
	return true;
}

function attrtostr($arr){
	$str = "";
	foreach($arr as $k => $s){
		if($k>1)
			$str.=Local2UTF8($s);
		$str.=",";
	}
	return $str;
}


function SetParamInfo(&$param, $info, $pre_name){
	if($info==null or count($info)<=1){
		$param[$pre_name."_0"] = "null";
		return;
	}
	foreach ($info as $key=>$value){
		$param[$pre_name."_".$key] = UTF82Local($value);
	}
}

function GetBackEquipFun($var, $gm){
	$attr = $var["attr"];
	$item_type = intval($var["item_type"]);
	$server_id = intval($var["serverid"]);
	$role_name = $var["role_name"];
	$reason = $var["reason"];
	
	$attr_array = explode(";",$attr);
	$temp = explode(",",$attr_array[0]);
	$item_name = $temp[0];
	$item_type = $temp[1];
	$item_id = $temp[2];
	
	$param = array();
	$param["role_name"] = $role_name;
	$param["item_name"] = UTF82Local($item_name);
	$param["item_type"] = $item_type;
	SetParamInfo($param, explode(",",$attr_array[1]),  "St");
	SetParamInfo($param, explode(",",$attr_array[2]),  "SA");
	SetParamInfo($param, explode(",",$attr_array[3]),  "Ad");
	SetParamInfo($param, explode(",",$attr_array[4]),  "Ar1");
	SetParamInfo($param, explode(",",$attr_array[5]),  "Ar2");
	SetParamInfo($param, explode(",",$attr_array[6]),  "Ar3");
	SetParamInfo($param, explode(",",$attr_array[7]),  "Ar4");
	SetParamInfo($param, explode(",",$attr_array[8]),  "In");
	SetParamInfo($param, explode(",",$attr_array[9]),  "IAA");
	SetParamInfo($param, explode(",",$attr_array[10]), "Du");
	SetParamInfo($param, explode(",",$attr_array[11]), "Bd");
	SetParamInfo($param, explode(",",$attr_array[12]), "Su");

	global $tbl_game_server;
	$gas_host = $tbl_game_server[$server_id][0]; 
	$gas_port = $tbl_game_server[$server_id][1]; 

	$ret = PostData($gas_host, $gas_port, "GetBackEquip", $param);
	
	$action = array("role_name" => $role_name, "type" => $item_type, "item" => $item_name, "item_id" => $item_id, "reason" => $reason);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $server_id)){	
		PrintNotice(LogInfo($action,true));
		BackAndRefresh($tbl_sys_msg["success"]);
		return true;
	}
	return false;
}

?>
