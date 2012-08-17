<?php
function GetRoleId($role_name, $server_id){
	global $tbl_sys_msg;
	$qs = "(select cs_uId from tbl_char where c_sName = '".$role_name."') union (select cs_uId from tbl_char_deleted where cd_sName = '".$role_name."')";
	$result = MySQLQuery($qs, "Game", $server_id);
	if($row=mysql_fetch_row($result)){
		$role_id = intval($row[0]);
		return $role_id;
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
}

function GetRoleName($role_id, $server_id){
	if($role_id < 10000){
		global $tbl_event_type;
		return "<b>".$tbl_event_type[$role_id]."</b>";
	}else{
		$qs = "(select c_sName from tbl_char where cs_uId = $role_id) union (select cd_sName from tbl_char_deleted where cs_uId = $role_id)";
		$result = MySQLQuery($qs, "Game", $server_id);
		if($row=mysql_fetch_row($result)){
			return $row[0];
		}else{
			return "-";
		}
	}
}

function GetUserName($user_id, $server_id){
	$qs = "select us_sName from tbl_user_static where us_uId = ".$user_id;
	$result = MySQLQuery($qs, "Game", $server_id);
	if($row=mysql_fetch_row($result)){
		return $row[0];
	}else{
		return "-";
	}
}

function ShowItemLog($var, $gm){
//当前物品, 数目
//所有流通过的物品列表, 数目
	return false;
}

function ShowItemFlowLog($var, $gm){
	$serverid = $var["serverid"];
	$itemid = $var["item_id"];
	global $tbl_event_type,$tbl_sys_msg,$page_main;
	$qs = "(select le.le_dtDateTime, let.let_uEventType, lpg.lcs_uId, le.le_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_item_giver lig, tbl_log_event le, tbl_log_event_type let, tbl_log_player_giver lpg ";
	$qs.= "where ";
	$qs.= "lig.le_uId = le.le_uId and ";
	$qs.= "le.le_uId = let.le_uId and ";
	$qs.= "le.le_uId = lpg.le_uId and ";
	$qs.= "lig.lis_uId = $itemid)";
	$qs.= "union";
	$qs.= "(select le.le_dtDateTime, let.let_uEventType, lpg.lcs_uId, le.le_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_item_del lid, tbl_log_event le, tbl_log_event_type let, tbl_log_player_giver lpg ";
	$qs.= "where ";
	$qs.= "lid.le_uId = le.le_uId and ";
	$qs.= "le.le_uId = let.le_uId and ";
	$qs.= "le.le_uId = lpg.le_uId and ";
	$qs.= "let.let_uEventType <> 2 and ";
	$qs.= "lid.lis_uId = $itemid";
	$qs.= ")union(";
	$qs.= "select le.le_dtDateTime, let.let_uEventType, lpt.lcs_uId, le.le_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_item_taker lit, tbl_log_event le, tbl_log_event_type let, tbl_log_player_taker lpt ";
	$qs.= "where ";
	$qs.= "lit.le_uId = le.le_uId and ";
	$qs.= "le.le_uId = let.le_uId and ";
	$qs.= "le.le_uId = lpt.le_uId and ";
	$qs.= "lit.lis_uId = $itemid)";
	$qs.= "union";
	$qs.= "(select le.le_dtDateTime, 85, lpt.lpt_uFromCharId, le.le_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_event le, tbl_log_player_trade lpt ";
	$qs.= "where ";
	$qs.= "lpt.le_uId = le.le_uId and ";
	$qs.= "lpt.lis_uId = $itemid)";
	$qs.= "union";
	$qs.= "(select le.le_dtDateTime, let.let_uEventType, lpg.lcs_uId, le.le_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_item li, tbl_log_event le, tbl_log_event_type let, tbl_log_player_giver lpg ";
	$qs.= "where ";
	$qs.= "li.le_uId = le.le_uId and ";
	$qs.= "le.le_uId = let.le_uId and ";
	$qs.= "le.le_uId = lpg.le_uId and ";
	$qs.= "li.lis_uId = $itemid)";
	$qs.= " order by le_uId";
	$rs = MySQLQuery($qs, "Log", $serverid);
	
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_sys_msg["time"]."</th><th>".$page_main["event_type"]."</th><th>".$page_main["role_name"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_event_type[$row[1]],GetRoleName($row[2],$serverid),ShowVirWealthLog_Desc($row[1],$row[3],$serverid));
	}
	echo "</table>\n";
	return true;
}

function ShowVirWealthLog_Desc($event_type,$event_id,$server_id){
	global $tbl_log_desc;
	$des = "";
	switch($event_type){
	case 1:
		$qs = "select lng_sNpcName from tbl_log_npc_giver where le_uId = $event_type group by lng_sNpcName";
		$rs = MySQLQuery($qs, "Log", $server_id);
		while($row = mysql_fetch_row($rs)){
			$des.= sprintf($tbl_log_desc[1][0]."<br/>",Lan_NPC_GetDisplayName(Local2UTF8($row[0]),false));
		}
		$qs = "select count(*), lis.lis_sName from tbl_log_item_taker lit, tbl_log_item_static lis where lit.lis_uId = lis.lis_uId and lit.le_uId = $event_id group by lis.lis_sName";
		$rs = MySQLQuery($qs, "Log", $server_id);
		while($row = mysql_fetch_row($rs)){
			$des.= sprintf($tbl_log_desc[1][1]."<br/>",$row[0],Lan_Item_GetDisplayName(Local2UTF8($row[1]),"",false));
		}
		$qs = "select lm_uMoneyType, sum(lm_uMoney) from tbl_log_money where le_uId = $event_id and lm_uMoneyType < 3 group by lm_uMoneyType";
		$rs = MySQLQuery($qs, "Log", $server_id);
		while($row = mysql_fetch_row($rs)){
			global $tbl_money_binding;
			$des.= sprintf($tbl_log_desc[1][2]."<br/>",$tbl_money_binding[$row[0]],money_to_string(0-$row[1]));
		}
		$qs = "select lj_uType, sum(lj_uPoint) from tbl_log_jifenpoint where le_uId = $event_id group by lj_uType";
		$rs = MySQLQuery($qs, "Log", $server_id);
		while($row = mysql_fetch_row($rs)){
			global $tbl_jifen;
			$des.= sprintf($tbl_log_desc[1][2]."<br/>",$tbl_jifen[$row[0]],number_to_string(0-$row[1]));
		}
		break;
	case 2:
		$qs = "select lnt_sNpcName from tbl_log_npc_taker where le_uId = $event_id group by lnt_sNpcName";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
      global $tbl_money_binding;
      $des.= sprintf($tbl_log_desc[2][0]."<br/>",Lan_NPC_GetDisplayName(Local2UTF8($row[0]),false));
		}
		$qs = "select count(*), lis.lis_sName from tbl_log_item_giver lig, tbl_log_item_static lis where lig.lis_uId = lis.lis_uId and lig.le_uId = $event_id group by lis.lis_sName";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
      global $tbl_money_binding;
      $des.= sprintf($tbl_log_desc[2][1]."<br/>",$row[0],Lan_Item_GetDisplayName(Local2UTF8($row[1]),"",false));
		}
		$qs = "select lm_uMoneyType, sum(lm_uMoney) from tbl_log_money where le_uId = $event_id and lm_uMoneyType < 3 group by lm_uMoneyType";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
      global $tbl_money_binding;
      $des.= sprintf($tbl_log_desc[2][2]."<br/>",$tbl_money_binding[$row[0]],money_to_string($row[1]));
		}
		$qs = "select lj_uType, sum(lj_uPoint) from tbl_log_jifenpoint where le_uId = $event_id group by lj_uType";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
      global $tbl_jifen;
      $des.= sprintf($tbl_log_desc[2][2]."<br/>",$tbl_jifen[$row[0]],number_to_string($row[1]));
		}
		break;
	case 3:
		$qs = "select lpg.lcs_uId, lpt.lcs_uId from tbl_log_player_taker lpt, tbl_log_player_giver lpg where lpg.le_uId = $event_id and lpt.le_uId = $event_id";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
			$des.= sprintf($tbl_log_desc[3][0],GetRoleName($row[0],$server_id));
			$des.= sprintf($tbl_log_desc[3][1],GetRoleName($row[1],$server_id));
			$des.= $tbl_log_desc[3][2];
		}
		break;	
	case 16:
		$qs = "select lmk.lm_uCount,lis.lis_sName,lmk.lm_uPrice,lmk.mos_uId from tbl_log_market lmk,tbl_log_item_giver lig,tbl_log_item_static lis where lmk.le_uId = $event_id and lig.le_uId = $event_id and lig.lis_uId = lis.lis_uId";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
			$mos = $row[3];
      $des.= sprintf($tbl_log_desc[16][0],$row[0],Lan_Item_GetDisplayName(Local2UTF8($row[1]),"",false),money_to_string($row[2]));
      $query = "select le.le_dtDateTime, lpt.lcs_uId from tbl_log_market_item lmi, tbl_log_event le, tbl_log_event_type let, tbl_log_player_taker lpt where le.le_uId = lmi.le_uId and le.le_uId = let.le_uId and le.le_uId = lpt.le_uId and let.let_uEventType = 18 and lmi.mos_uId = $mos";
      $rs = MySQLQuery($query, "Log", $server_id);
      if($r = mysql_fetch_row($rs)){
      	$des.= sprintf("<br/>".$tbl_log_desc[16][1],$r[0],GetRoleName($r[1],$server_id));
      }
		}
		break;
	case 18:
		$qs = "select lpg.lcs_uId, lmk.lm_uCount, lis.lis_sName, lmk.lm_uPrice from tbl_log_market lmk, tbl_log_market_item lmi, tbl_log_player_giver lpg, tbl_log_item_taker lit, tbl_log_item_static lis where lmi.le_uId = $event_id and lit.le_uId = $event_id and lit.lis_uId = lis.lis_uId and lmk.mos_uId = lmi.mos_uId and lpg.le_uId = lmk.le_uId";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
      $des.= sprintf($tbl_log_desc[18][0],GetRoleName($row[0],$server_id),$row[1],Lan_Item_GetDisplayName(Local2UTF8($row[2]),"",false),money_to_string($row[3]));
		}
		break;
	case 42:
		$qs = "select lqf_sQuestName from tbl_log_quest_finish where le_uId = $event_id";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
			$des.= sprintf($tbl_log_desc[42],Lan_Quest_GetDisplayName(Local2UTF8($row[0]),false));
		}
		break;
	case 85:
		$qs = "select lpt.lpt_uFromCharId,lpt.lpt_uToCharId, lpt.lpt_uMoney, lis.lis_sName, count(*), lis.lis_uType from tbl_log_player_trade lpt left join tbl_log_item_static lis on lpt.lis_uId = lis.lis_uId where lpt.le_uId = $event_id group by lpt_uFromCharId, lis.lis_sName";
		$rs = MySQLQuery($qs, "Log", $server_id);
		$role_id1 = 0;
		$role_id2 = 0;
		$money1 = "";
		$money2 = "";
		$goods1 = array();
		$goods2 = array();
		while($row = mysql_fetch_row($rs)){
			if($role_id1 == 0) $role_id1 = $row[0];
			if($role_id2 == 0) $role_id2 = $row[1];
			if($role_id1 == $row[0]){
				if($money1 == "" && $row[2] != 0) $money1 = sprintf($tbl_log_desc[85][3],money_to_string($row[2]));
				if($row[3] != NULL){
					array_push($goods1,sprintf($tbl_log_desc[85][2],$row[4],Lan_Item_GetDisplayName(Local2UTF8($row[3]),$row[5],false)));
				}
			}else{
				if($money2 == "" && $row[2] != 0) $money2 = sprintf($tbl_log_desc[85][3],money_to_string($row[2]));
				if($row[3] != NULL){
					array_push($goods2,sprintf($tbl_log_desc[85][2],$row[4],Lan_Item_GetDisplayName(Local2UTF8($row[3]),$row[5],false)));
				}
			}
		}
		if($money1 != "") array_push($goods1,$money1);
		if($money2 != "") array_push($goods2,$money2);
		$role_name1 = GetRoleName($role_id1, $server_id);
		$role_name2 = GetRoleName($role_id2, $server_id);
		$des.= sprintf($tbl_log_desc[85][0],$role_name1,$role_name2).sprintf($tbl_log_desc[85][1],implode($tbl_log_desc[85][4],$goods1),implode($tbl_log_desc[85][4],$goods2));
	case 153:
		$qs = "select lng_sNpcName, count(*), lis.lis_sName, lm_uMoneyType, sum(lm_uMoney) from tbl_log_npc_giver lng, tbl_log_money lm, tbl_log_item_taker lit, tbl_log_item_static lis where lng.le_uId = lm.le_uId and lm.le_uId = lit.le_uId and lit.lis_uId = lis.lis_uId and lm.le_uId = $event_id and lm_uMoneyType < 3 group by lm.le_uId";
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($row = mysql_fetch_row($rs)){
        global $tbl_money_binding;
        $des.= sprintf($tbl_log_desc[153],Lan_NPC_GetDisplayName(Local2UTF8($row[0]),false),$row[1],Lan_Item_GetDisplayName(Local2UTF8($row[2]),"",false),$tbl_money_binding[$row[3]],money_to_string(abs($row[4])));
		}
		break;
	case 10002:
		$des = $tbl_log_desc[10002];
		break;
	default:
		$des.= "";
		break;
	}
	return $des;
}

function ShowVirWealthLog($var, $gm){
	global $tbl_log_db, $tbl_sys_msg, $tbl_log_vir_wealth, $tbl_event_type, $tbl_money_binding, $tbl_jifen, $tbl_serv_db;
	
	$server_id = $var["serverid"];
	
	echo "<h1>".$tbl_log_vir_wealth[0]."</h1>\n";
	ReturnAndBack();
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	echo sprintf("<p>".$tbl_log_vir_wealth[1]."</p>\n",$role_name,$role_id);
	
	$data_limit = getDateLimit($var,$server_id);
	
	echo "<h2>".$tbl_log_vir_wealth[2]."</h2>\n";
	$qs = "select ll.ll_uLevel as level, min(le.le_dtDateTime) as dtTime from tbl_log_level ll, tbl_log_player lp, tbl_log_event le force index(le_dtDateTime) where le.le_uId = lp.le_uId and lp.le_uId = ll.le_uId and lp.lcs_uId = '$role_id' $data_limit group by ll.ll_uLevel order by ll.ll_uLevel desc";
	$result = MySQLQuery($qs, "Log", $server_id);
	echo "<table class='list_table'>\n";
	echo sprintf("<tr><th>%s</th><th>%s</th></tr>\n", $tbl_log_vir_wealth[3][0], $tbl_log_vir_wealth[3][1]);
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td></tr>\n", $row[0], $row[1]);
	}
	echo "</table>\n";
	
	echo "<h2>".$tbl_log_vir_wealth[4]."</h2>\n";
	echo "<table class='list_table'>\n";
	echo sprintf("<tr><th>%s</th><th>%s</th></tr>\n", $tbl_log_vir_wealth[5][0], $tbl_log_vir_wealth[5][1]);
	$qs = "select let.let_uEventType, sum(lee.le_uExp) s from tbl_log_experience lee,tbl_log_event_type let,tbl_log_event le force index(le_dtDateTime) where le.le_uId = lee.le_uId and lee.le_uId = let.le_uId and lee.lcs_uId = '$role_id' $data_limit group by let.let_uEventType order by s desc";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%d</td></tr>\n", $tbl_event_type[intval($row[0])], $row[1]);
	}
	echo "</table>\n";

	echo "<h2>".$tbl_log_vir_wealth[6]."</h2>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_log_vir_wealth[7][0]."</th><th>".$tbl_log_vir_wealth[7][1]."</th><th>".$tbl_log_vir_wealth[7][2]."</th><th>".$tbl_log_vir_wealth[7][3]."</th><th>".$tbl_log_vir_wealth[7][4]."</th></tr>\n";
	$qs = "select let.let_uEventType, lm.lm_uMoneyType, count(*), sum(lm.lm_uMoney) s, avg(lm.lm_uMoney) from tbl_log_money lm, tbl_log_event_type let, tbl_log_event le force index(le_dtDateTime) where lm.le_uId = le.le_uId and lm.le_uId = let.le_uId and lm.lcs_uId = '$role_id' $data_limit group by let.let_uEventType, lm.lm_uMoneyType order by s desc";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td></tr>\n", $tbl_event_type[intval($row[0])], $tbl_money_binding[intval($row[1])], $row[2], $row[3], $row[4]);
	}
	echo "</table>\n";
	
	echo "<h2>".$tbl_log_vir_wealth[8]."</h2>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_log_vir_wealth[9][0]."</th><th>".$tbl_log_vir_wealth[9][1]."</th><th>".$tbl_log_vir_wealth[9][2]."</th><th>".$tbl_log_vir_wealth[9][3]."</th></tr>\n";
	$qs = "select let.let_uEventType, count(*), sum(ls.ls_uSoulValue) s, avg(ls.ls_uSoulValue) from tbl_log_soul ls, tbl_log_event_type let, tbl_log_event le force index(le_dtDateTime) where ls.le_uId = le.le_uId and ls.le_uId = let.le_uId and ls.lcs_uId = '$role_id' $data_limit group by let.let_uEventType order by s desc";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%d</td><td>%d</td><td>%d</td></tr>\n",$tbl_event_type[intval($row[0])], $row[1], $row[2], $row[3]);
	}
	echo "</table>\n";
	
	echo "<h2>".$tbl_log_vir_wealth[10]."</h2>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_log_vir_wealth[11][0]."</th><th>".$tbl_log_vir_wealth[11][1]."</th><th>".$tbl_log_vir_wealth[11][2]."</th><th>".$tbl_log_vir_wealth[11][3]."</th><th>".$tbl_log_vir_wealth[11][4]."</th></tr>\n";
	$qs = "select lj.lj_uType, let.let_uEventType, count(lj.le_uId), sum(lj.lj_uPoint) s, avg(lj.lj_uPoint) from tbl_log_jifenpoint lj, tbl_log_event_type let, tbl_log_event le force index(le_dtDateTime) where lj.le_uId = le.le_uId and lj.le_uId = let.le_uId and lj.lcs_uId = '$role_id' $data_limit group by lj.lj_uType,let.let_uEventType order by s desc";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%d</td><td>%d</td><td>%d</td></tr>\n", $tbl_jifen[intval($row[0])], $tbl_event_type[intval($row[1])], $row[2], $row[3], $row[4]);
	}
	echo "</table>\n";
	
	echo "<h2>".$tbl_log_vir_wealth[12]."</h2>\n";
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$tbl_log_vir_wealth[13][0]."/".$tbl_log_vir_wealth[13][1]."</th><th>".$tbl_log_vir_wealth[13][2]."</th><th>".$tbl_log_vir_wealth[13][3]."</th></tr>\n";
	$qs = "(select '0',let.let_uEventType, count(lpt.le_uId) c from tbl_log_player_taker lpt,tbl_log_event_type let,tbl_log_item_taker lit,tbl_log_event le force index(le_dtDateTime) where lpt.le_uId = lit.le_uId and lpt.le_uId = let.le_uId and lpt.le_uId = le.le_uId and lpt.lcs_uId='$role_id' $data_limit group by let.let_uEventType) ";
	$qs.= "union ";
	$qs.= "(select '1',let.let_uEventType, count(lpg.le_uId) c from tbl_log_player_giver lpg,tbl_log_event_type let,tbl_log_item_giver lig,tbl_log_event le force index(le_dtDateTime) where lpg.le_uId = lig.le_uId and lpg.le_uId = let.le_uId and lpg.le_uId = le.le_uId and lpg.lcs_uId='$role_id' $data_limit group by let.let_uEventType) ";
	$qs.= "union ";
	$qs.= "(select '1',let.let_uEventType, count(lpg.le_uId) c from tbl_log_player_giver lpg,tbl_log_event_type let,tbl_log_item_del   lid,tbl_log_event le force index(le_dtDateTime) where lpg.le_uId = lid.le_uId and lpg.le_uId = let.le_uId and lpg.le_uId = le.le_uId and lpg.lcs_uId='$role_id' $data_limit group by let.let_uEventType) ";
	$qs.= "order by c desc";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%d</td></tr>\n", $tbl_log_vir_wealth[13][intval($row[0])], $tbl_event_type[intval($row[1])], $row[2]);
	}
	echo "</table>\n";
	
	ReturnAndBack();
	return true;
}

function ShowVirWealthLog_Item($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_log_item_flow,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$tbl_sys_msg,$page_main;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$event_type = $var["event_type"];
	$str = "";
	if($event_type <> 0){
		$str = " and let.let_uEventType = $event_type";
	}
	
	$item_name = $var["item_name"];
	if($item_name <> ""){
		GetItemInfo(mysql_escape_string($item_name), $item_type, $item_name);
		$str .= " and lis.lis_sName = '".UTF82Local($item_name)."'";
	}
	
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["event_type"],$page_main["item"],$page_main["item_id"],$page_main["number"],$tbl_sys_msg["desc"],$tbl_sys_msg["detail"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th style='width:30px;'>".$page_main["event_type"]."</th><th>".$page_main["item"]."</th><th style='width:30px;'>".$page_main["item_id"]."</th><th>".$page_main["number"]."</th><th>".$tbl_sys_msg["desc"]."</th><th>".$tbl_sys_msg["detail"]."</th></tr>\n";
	}
	
	$date_limit = getDateLimit($var, $server_id);
	
	$qs = "(select le.le_dtDateTime, 1, lis.lis_sName, count(*), let.let_uEventType, le.le_uId, lig.lis_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_player_giver lpg, tbl_log_event le, tbl_log_event_type let, tbl_log_item_giver lig ";
	$qs.= "left join tbl_log_item_static lis on lig.lis_uId = lis.lis_uId ";
	$qs.= "where ";
	$qs.= "lig.le_uId = lpg.le_uId and ";
	$qs.= "lig.le_uId =  le.le_uId and ";
	$qs.= "lig.le_uId = let.le_uId and ";
	$qs.= "lpg.lcs_uId = $role_id$str $date_limit ";
	$qs.= "group by le.le_uId,lis.lis_sName) ";
	$qs.= "union ";
	$qs.= "(select le.le_dtDateTime, 0, lis.lis_sName, count(*), let.let_uEventType, le.le_uId, lit.lis_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_player_taker lpt, tbl_log_event le, tbl_log_event_type let, tbl_log_item_taker lit ";
	$qs.= "left join tbl_log_item_static lis on lit.lis_uId = lis.lis_uId ";
	$qs.= "where ";
	$qs.= "lit.le_uId = lpt.le_uId and ";
	$qs.= "lit.le_uId =  le.le_uId and ";
	$qs.= "lit.le_uId = let.le_uId and ";
	$qs.= "lpt.lcs_uId = $role_id$str $date_limit ";
	$qs.= "group by le.le_uId,lis.lis_sName) ";
	$qs.= "union ";
	$qs.= "(select le.le_dtDateTime, 1, lis.lis_sName, count(*), let.let_uEventType, le.le_uId, lid.lis_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_player_giver lpg, tbl_log_event le, tbl_log_event_type let, tbl_log_item_del lid ";
	$qs.= "left join tbl_log_item_static lis on lid.lis_uId = lis.lis_uId ";
	$qs.= "where ";
	$qs.= "lid.le_uId = lpg.le_uId and ";
	$qs.= "lid.le_uId =  le.le_uId and ";
	$qs.= "lid.le_uId = let.le_uId and ";
	$qs.= "let.let_uEventType <> 2 and ";
	$qs.= "lpg.lcs_uId = $role_id$str $date_limit ";
	$qs.= "group by le.le_uId,lis.lis_sName) ";
	if($event_type == 0 || $event_type == 85){
		$qs.= "union ";
		$qs.= "(select le.le_dtDateTime, if(lpt.lpt_uToCharId = $role_id,0,1), lis.lis_sName, count(*), 85, le.le_uId, lpt.lis_uId ";
		$qs.= "from ";
		$qs.= "tbl_log_event le, tbl_log_player_trade lpt ";
		$qs.= "left join tbl_log_item_static lis on lpt.lis_uId = lis.lis_uId ";
		$qs.= "where ";
		$qs.= "(lpt.lpt_uFromCharId = $role_id or lpt.lpt_uToCharId = $role_id) and ";
		$qs.= "lpt.le_uId = le.le_uId and lpt.lis_uId <> 0 $date_limit ";
		$qs.= "group by le.le_uId, lis.lis_sName) ";
	}
	$qs.= "union ";
	$qs.= "(select le.le_dtDateTime, 1, lis.lis_sName, count(*), let.let_uEventType, le.le_uId, lig.lis_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_player_giver lpg, tbl_log_event le, tbl_log_event_type let, tbl_log_item lig ";
	$qs.= "left join tbl_log_item_static lis on lig.lis_uId = lis.lis_uId ";
	$qs.= "where ";
	$qs.= "lig.le_uId = lpg.le_uId and ";
	$qs.= "lig.le_uId =  le.le_uId and ";
	$qs.= "lig.le_uId = let.le_uId and ";
	$qs.= "lpg.lcs_uId = $role_id$str $date_limit ";
	$qs.= "group by le.le_uId,lis.lis_sName) ";
	$qs.= "union ";
	$qs.= "(select le.le_dtDateTime, 0, lis.lis_sName, count(*), let.let_uEventType, le.le_uId, lit.lis_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_player_taker lpt, tbl_log_event le, tbl_log_event_type let, tbl_log_item lit ";
	$qs.= "left join tbl_log_item_static lis on lit.lis_uId = lis.lis_uId ";
	$qs.= "where ";
	$qs.= "lit.le_uId = lpt.le_uId and ";
	$qs.= "lit.le_uId =  le.le_uId and ";
	$qs.= "lit.le_uId = let.le_uId and ";
	$qs.= "lpt.lcs_uId = $role_id$str $date_limit ";
	$qs.= "group by le.le_uId,lis.lis_sName) ";
	if($event_type == 0){
		$qs.= "union ";
		$qs.= "(select le.le_dtDateTime, ltd.ltd_uType-1, lis.lis_sName, count(*), 10001, le.le_uId, ltd.lis_uId ";
		$qs.= "from ";
		$qs.= "tbl_log_event le, tbl_log_tong_depot ltd ";
		$qs.= "left join tbl_log_item_static lis on ltd.lis_uId = lis.lis_uId ";
		$qs.= "where ";
		$qs.= "ltd.le_uId = le.le_uId and ";
		$qs.= "ltd.ltd_uExecutorId = $role_id $date_limit ";
		$qs.= "group by le.le_dtDateTime,lis.lis_sName) ";
	}
	$qs.= "order by le_uId";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[4],$row[5],$server_id);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],$tbl_log_item_flow[$row[1]],Lan_Item_GetDisplayName(Local2UTF8($row[2])),$row[6],$row[3],$tbl_event_type[intval($row[4])],$des))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_log_item_flow[$row[1]],Lan_Item_GetDisplayName(Local2UTF8($row[2])),$row[6],$row[3],$tbl_event_type[intval($row[4])],$des);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_Money($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$page_main,$tbl_sys_msg;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$event_type = $var["event_type"];
	$str = "";
	if($event_type <> 0){
		$str = " and let.let_uEventType = $event_type";
	}
	
	$money_type 	= "";
	if($var["money_type"]!=0){
		$money_type	= " and lm.lm_uMoneyType = ".$var["money_type"];
	}
	
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["type"],$page_main["money_type"],$page_main["number"],$tbl_sys_msg["status"],$page_main["event_type"],$tbl_sys_msg["desc"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th style='width:30px;'>".$page_main["type"]."</th><th>".$page_main["money_type"]."</th><th style='width:30px;'>".$page_main["number"]."</th><th>".$tbl_sys_msg["status"]."</th><th>".$page_main["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	}
	
	$qs = "(select le.le_uId,le.le_dtDateTime,if(let.let_uEventType = 85,10002,let.let_uEventType),lm.lm_uMoneyType,sum(lm.lm_uMoney), if(lm.lm_uMoney>0,max(lm.lm_sCode1),min(lm.lm_sCode1)) from tbl_log_money lm, tbl_log_event le, tbl_log_event_type let where lm.lcs_uId = $role_id and lm.le_uId = le.le_uId and le.le_uId = let.le_uId and let.let_uEventType not in (178,188,189,190,191,192) and lm.lm_uMoneyType < 3".$str.$money_type.getDateLimit($var,$server_id)." group by le.le_uId) ";
	if(($event_type == 0 || $event_type == 85) && ($money_type == 0 || $money_type == 1)){
		$qs.= "union ";
		$qs.= "(select le.le_uId,le.le_dtDateTime,85,1,0,0 from tbl_log_player_trade lpt, tbl_log_event le where lpt.le_uId = le.le_uId and (lpt.lpt_uFromCharId = $role_id or lpt.lpt_uToCharId = $role_id)".getDateLimit($var,$server_id)." group by le.le_uId)";
	}
	$qs.= "order by le_uId";
	$result = MySQLQuery($qs, "Log", $server_id);
	global $tbl_log_item_flow,$tbl_money_binding;
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[2],$row[0],$server_id);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[1],($row[4]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$tbl_money_binding[$row[3]],money_to_string(abs($row[4])),money_to_string($row[5]),$tbl_event_type[intval($row[2])],$des))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[1],($row[4]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$tbl_money_binding[$row[3]],money_to_string(abs($row[4])),money_to_string($row[5]),$tbl_event_type[intval($row[2])],$des);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_Experience($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$page_main,$tbl_sys_msg;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$event_type = $var["event_type"];
	$str = "";
	if($event_type <> 0){
		$str = " and let.let_uEventType = $event_type";
	}
	
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["number"],$tbl_sys_msg["status"],$page_main["event_type"],$tbl_sys_msg["desc"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["number"]."</th><th>".$tbl_sys_msg["status"]."</th><th>".$page_main["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	}
	
	$qs = "select le.le_dtDateTime, let.let_uEventType, lex.le_uExp, lex.le_sCode1, le.le_uId from tbl_log_experience lex, tbl_log_event le, tbl_log_event_type let where lex.le_uId = le.le_uId and le.le_uId = let.le_uId and lex.lcs_uId = $role_id$str ".getDateLimit($var,$server_id)." order by le_dtDateTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[1],$row[4],$server_id);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],$row[2],$row[3],$tbl_event_type[intval($row[1])],$des))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$row[2],$row[3],$tbl_event_type[intval($row[1])],$des);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_Guogong($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$page_main,$tbl_sys_msg;
		if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$event_type = $var["event_type"];
	$str = "";
	if($event_type <> 0){
		$str = " and let.let_uEventType = $event_type";
	}
		
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["type"],$page_main["number"],$tbl_sys_msg["status"],$page_main["event_type"],$tbl_sys_msg["desc"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["type"]."</th><th>".$page_main["number"]."</th><th>".$tbl_sys_msg["status"]."</th><th>".$page_main["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	}
	
	$qs = "select le.le_dtDateTime, let.let_uEventType, lj.lj_uPoint, lj.lj_sCode1, le.le_uId from tbl_log_jifenpoint lj, tbl_log_event le, tbl_log_event_type let where lj.le_uId = le.le_uId and le.le_uId = let.le_uId and lj.lj_uType = 10 and lj.lcs_uId = $role_id$str ".getDateLimit($var,$server_id)." order by le_dtDateTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	global $tbl_log_item_flow;
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[1],$row[4],$server_id);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],($row[2]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$row[2],$row[3],$tbl_event_type[intval($row[1])],$des))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],($row[2]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$row[2],$row[3],$tbl_event_type[intval($row[1])],$des);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_Jifen($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$page_main,$tbl_sys_msg;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$event_type = $var["event_type"];
	$str = "";
	if($event_type <> 0){
		$str .= " and let.let_uEventType = $event_type";
	}
	
	$jifen_type = $var["jifen_type"];
	if($jifen_type <> 0){
		$str .= " and lj.lj_uType = $jifen_type";
	}
		
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["type"],$page_main["jifen"],$tbl_sys_msg["number"],$tbl_sys_msg["status"],$tbl_sys_msg["event_type"],$tbl_sys_msg["desc"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["type"]."</th><th>".$page_main["jifen"]."</th><th>".$tbl_sys_msg["number"]."</th><th>".$tbl_sys_msg["status"]."</th><th>".$tbl_sys_msg["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	}
	
	$qs = "select le.le_dtDateTime, let.let_uEventType, lj.lj_uPoint, lj.lj_sCode1, le.le_uId, lj.lj_uType from tbl_log_jifenpoint lj, tbl_log_event le, tbl_log_event_type let where lj.le_uId = le.le_uId and le.le_uId = let.le_uId and lj.lj_uType <> 10 and lj.lcs_uId = $role_id$str ".getDateLimit($var,$server_id)." order by le_dtDateTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	global $tbl_log_item_flow,$tbl_jifen;
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[1],$row[4],$server_id);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],($row[2]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$tbl_jifen[$row[5]],$row[2],$row[3],$tbl_event_type[intval($row[1])],$des))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],($row[2]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$tbl_jifen[$row[5]],$row[2],$row[3],$tbl_event_type[intval($row[1])],$des);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_Soul($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$tbl_sys_msg,$page_main;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
		
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["type"],$page_main["number"],$tbl_sys_msg["status"],$page_main["event_type"],$tbl_sys_msg["desc"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["type"]."</th><th>".$page_main["number"]."</th><th>".$tbl_sys_msg["status"]."</th><th>".$page_main["event_type"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	}
	
	$qs = "select le.le_dtDateTime, ls.ls_uSoulValue, ls.ls_sCode1, let.let_uEventType, le.le_uId from tbl_log_soul ls, tbl_log_event le, tbl_log_event_type let where ls.le_uId = le.le_uId and le.le_uId = let.le_uId and ls.ls_uSoulValue <> 0 and ls.lcs_uId = $role_id ".getDateLimit($var,$server_id)." order by le_dtDateTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	global $tbl_log_item_flow,$tbl_jifen;
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[3],$row[4],$server_id);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],($row[1]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$row[1],$row[2],$tbl_event_type[intval($row[3])],$des))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],($row[1]>0)?$tbl_log_item_flow[0]:$tbl_log_item_flow[1],$row[1],$row[2],$tbl_event_type[intval($row[3])],$des);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_Market($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_log_desc,$tbl_sys_msg,$page_main;
	
	LastLogTime();
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
		
	echo "<table class='list_table'>\n";
	echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["event_type"]."</th><th>".$page_main["item"]."</th><th>".$page_main["item_id"]."</th><th>".$page_main["money"]."</th><th>".$page_main["number"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	
	$qs = "(select le.le_dtDateTime, let.let_uEventType, lis.lis_sName, lis.lis_uId, lm.lm_uPrice, lm.lm_uCount, le.le_uId ";
	$qs.= "from tbl_log_player_giver lpg, tbl_log_market lm, tbl_log_event le, tbl_log_event_type let, tbl_log_item_giver lig, tbl_log_item_static lis ";
	$qs.= "where le.le_uId = lpg.le_uId and le.le_uId = lm.le_uId and le.le_uId = let.le_uId and le.le_uId = lig.le_uId and lig.lis_uId = lis.lis_uId and lpg.lcs_uId = $role_id ".getDateLimit($var,$server_id)." ";
	$qs.= "group by le.le_uId) ";
	$qs.= "union ";
	$qs.= "(select le.le_dtDateTime, let.let_uEventType, lis.lis_sName, lis.lis_uId, lm.lm_uPrice, lm.lm_uCount, le.le_uId ";
	$qs.= "from tbl_log_player_taker lpt, tbl_log_market_item lmi, tbl_log_market lm, tbl_log_event le, tbl_log_event_type let, tbl_log_item_taker lit, tbl_log_item_static lis ";
	$qs.= "where le.le_uId = lpt.le_uId and le.le_uId = lmi.le_uId and le.le_uId = let.le_uId and le.le_uId = lit.le_uId and lit.lis_uId = lis.lis_uId and lmi.mos_uId = lm.mos_uId and lpt.lcs_uId = $role_id ".getDateLimit($var,$server_id)." ";
	$qs.= "group by le.le_uId) ";
	$qs.= "order by le_uId";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[1],$row[6],$server_id);
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_event_type[$row[1]],Lan_Item_GetDisplayName(Local2UTF8($row[2])),$row[3],money_to_string($row[4]),$row[5],$des);
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function ShowVirWealthLog_MarketBuyOrder($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$tbl_serv_db,$tbl_market_buy_order,$tbl_sys_msg,$page_main;
	
	LastLogTime();
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
		
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"],$page_main["item"],$page_main["number"],$page_main["UnitPrice"]);
	
	$qs = "select le_dtDateTime, lmbo_sItemName, lmbo_uCount, lmbo_uPrice, lmos_uId from tbl_log_market_buy_order_static lmbo, tbl_log_player_giver lpg, tbl_log_event le where le.le_uId = lpg.le_uId and lpg.le_uId = lmbo.le_uId and lpg.lcs_uId = $role_id ".getDateLimit($var,$server_id)." order by le_dtDateTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],Lan_Item_GetDisplayName(Local2UTF8($row[1])),$row[2],money_to_string($row[3]));
		$qs = "select le_dtDateTime, lc.lc_sName, count(*) from tbl_log_char lc, tbl_log_market_item lmi, tbl_log_player_giver lpg, tbl_log_item_giver lig, tbl_log_event le where lpg.lcs_uId = lc.lcs_uId and le.le_uId = lmi.le_uId and le.le_uId = lpg.le_uId and le.le_uId = lig.le_uId and lmi.mos_uId = ".$row[4]." group by le.le_uId order by le.le_uId";
		$rs = MySQLQuery($qs, "Log", $server_id);
		$count = 0;
		while($r = mysql_fetch_row($rs)){
			$count += $r[2];
			printf("<tr style='color:#888;'><td>%s</td><td colspan='3'>%s</td></tr>\n",$r[0],sprintf($tbl_market_buy_order[0],$r[1],$r[2]));
		}
		if($count>0){
			printf("<tr style='color:#888;'><td></td><td colspan='3'>".$tbl_market_buy_order[1]."</td></tr>\n",$count);
		}
		$qs = "select le_dtDateTime, let_uEventType from tbl_log_market_item lmi, tbl_log_player_taker lpt, tbl_log_event le, tbl_log_event_type let where le.le_uId = lmi.le_uId and le.le_uId = lpt.le_uId and le.le_uId = let.le_uId and lmi.mos_uId = ".$row[4];
		$rs = MySQLQuery($qs, "Log", $server_id);
		if($r = mysql_fetch_row($rs)){
			printf("<tr style='color:#888;'><td>%s</td><td colspan='3'>".$tbl_market_buy_order[2]."</td></tr>\n",$r[0],$tbl_market_buy_order[3][$r[1]]);
		}
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function ShowVirWealthLog_CommerceSkill($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_event_type,$page_main,$tbl_commerce_skill;
	
	LastLogTime();
	$server_id = $var["serverid"];
	$type = $var["type"];
	
	$str = "";
	if($type[0] != "0"){
		foreach($type as $key => $item){
			$type[$key] = base642UTF8($item);
		}
		$str = " and lcs.lcs_sSkillName in ('".implode("','",$type)."') ";
		
	}
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
		
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"],$tbl_sys_msg["skill"],$page_main["type"],$page_main["level"],$page_main["exp"]);
	
	$qs = "select le_dtDateTime, lcs.lcs_sSkillName, let.let_uEventType, lcs.lcs_uSkillLevel, lcs.lcs_uExperience, le.le_uId from tbl_log_commerce_skill lcs, tbl_log_event le, tbl_log_event_type let where le.le_uId = lcs.le_uId and le.le_uId = let.le_uId and lcs.lcs_uId = $role_id ".$str.getDateLimit($var,$server_id)." group by le.le_uId order by le.le_uId";
	$result = MySQLQuery($qs, "Log", $server_id);
	$event_type = 0;
	$skill_name = "";
	$skill_level = 0;
	while($row=mysql_fetch_row($result)){
		$style = "";
		if($skill_name = $row[1] && $event_type == $row[2] && $skill_level == $row[3]){
			$style = " style=\"color: #ccc;\"";
		}else{
			$skill_name = $row[1];
			$event_type = $row[2];
			$skill_level = $row[3];
		}
		printf("<tr%s><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$style,$row[0],$tbl_commerce_skill[Local2base64($row[1])],$tbl_event_type[$row[2]],$row[3],$row[4]);
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function RoleLostEquip($var, $gm){
	global $tbl_log_db,$tbl_event_type,$tbl_serv_db,$tbl_sys_msg,$page_main;
	
	$js_str = <<<_js_str
<script>
function getback(item_id){
	document.form1.action_name.value = '205';
	document.form1.item_id.value = item_id;
	document.form1.submit();
}
</script>
_js_str;
	echo $js_str."\n";
	form($var,$gm);
	echo "<input type='hidden' name='item_id' />\n";
	echo "</form>\n";

	LastLogTime();
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$str = "";
	
	$auth = HasAuth(205,$gm);
		
	echo "<table class='list_table'>\n";
	echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["event_type"]."</th><th>".$page_main["item"]."</th><th style='width:30px;'>".$page_main["item_id"]."</th>";
	if($auth) echo "<th style='width:30px;'>".$tbl_sys_msg["detail"]."</th>";
	echo "</tr>\n";
	
	$qs = "select le.le_dtDateTime, let.let_uEventType, lis.lis_sName, lid.lis_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_player_giver lpg, tbl_log_event le, tbl_log_event_type let, tbl_log_item_del lid, tbl_log_item_static lis ";
	$qs.= "where ";
	$qs.= "lid.le_uId  = lpg.le_uId and ";
	$qs.= "lid.le_uId  =  le.le_uId and ";
	$qs.= "lid.le_uId  = let.le_uId and ";
	$qs.= "lid.lis_uId = lis.lis_uId and ";
	$qs.= "lis.lis_uType >= 5 and lis.lis_uType <= 9 and ";
	$qs.= "lpg.lcs_uId = $role_id$str ".getDateLimit($var,$server_id)." ";
	$qs.= "order by le.le_uId";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		$des = ShowVirWealthLog_Desc($row[4],$row[5],$server_id);
		printf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%d</td>",$row[0],$tbl_event_type[intval($row[1])],Lan_Item_GetDisplayName(Local2UTF8($row[2])),$row[3]);
		if($auth) printf("<td><button onclick='getback(%d)'>".$tbl_sys_msg["detail"]."</button></td>",$row[3]);
		echo "</tr>\n";
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function ShowItemInOutLog($var, $gm){
	return true;
}

function ShowItemInOutDetail($var, $gm){
	return true;
}

 function ShowTongMemberLog($var, $gm){
//查看佣兵团创建，加人，离人，踢人，解散日志
//帮会细节处提供查询连接
	$serverid = intval($var["serverid"]);
	$tong_id = intval($var["id"]);

	$qs = "select lt_dtCreateTime, lt_sContent from tbl_log_tong lt where lt.t_uId = '$tong_id' and lt.lt_Type = 1";
	$result = MySQLQuery($qs, "Game", $serverid);

	ReturnAndBack();
	while($row=mysql_fetch_row($result)){
		echo sprintf("<p>[%s] %s</p>\n",$row[0], $row[1]);
	}
	ReturnAndBack();
	return true;
}

function ShowTongItemLog($var, $gm){
//帮会物品流动信息
	$serverid = intval($var["serverid"]);
	$tong_id = intval($var["id"]);

	$qs = "select lt_dtCreateTime, lt_sContent from tbl_log_tong lt where lt.t_uId = '$tong_id' and lt.lt_Type = 2";
	$result = MySQLQuery($qs, "Game", $serverid);

	ReturnAndBack();
	while($row=mysql_fetch_row($result)){
		echo sprintf("<p>[%s] %s</p>\n",$row[0],$row[1]);
	}
	ReturnAndBack();
	printf($str);
	return true;
}

function ShowNpcOutLog($var, $gm){
	return true;
}

function ShowSceneOut($var, $gm){
	return true;
}

function ShowSceneOutDetail($var, $gm){
	return true;
}

function ShowNPCDropLog($var, $gm){
	return true;
}

function ShowVirWealthLog_Level($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_serv_db,$page_main;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["level"],$tbl_sys_msg["status"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		echo "<tr><th style='width:120px;'>".$tbl_sys_msg["time"]."</th><th>".$page_main["level"]."</th><th>".$tbl_sys_msg["status"]."</th></tr>\n";
	}
	
	$qs = "select min(le.le_dtDateTime), ll.ll_uLevel from tbl_log_experience lex, tbl_log_level ll, tbl_log_player_taker lpt, tbl_log_event le where lex.le_uId = le.le_uId and ll.le_uId = le.le_uId and lpt.le_uId = le.le_uId and lpt.lcs_uId = $role_id ".getDateLimit($var,$server_id)." group by ll.ll_uLevel order by le_dtDateTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	$time = 0;
	$status_str = "% 3d ".$tbl_sys_msg["day"]." %02d ".$tbl_sys_msg["hour"]." %02d ".$tbl_sys_msg["minute"]." %02d ".$tbl_sys_msg["second"];
	while($row=mysql_fetch_row($result)){
		$new_time = strtotime($row[0]);
		if($time==0){
			$step = 0;
		}else{
			$step = $new_time - $time;
		}
		$time = $new_time;
		$d = intval( $step / 86400 );
		$h = intval( ( $step - ( $d * 86400 ) ) / 3600 );
		$m = intval( ( $step - ( $d * 86400 ) - ( $h * 3600 ) ) / 60 );
		$s = $step - ( $d * 86400 ) - ( $h * 3600 ) - ( $m * 60 );
		$status = sprintf($status_str,$d,$h,$m,$s);
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],$row[1],$status))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td><pre>%s</pre></td></tr>\n",$row[0],$row[1],$status);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function ShowVirWealthLog_ModulusChange($var, $gm){
	global $tbl_log_db,$tbl_sys_msg,$tbl_serv_db,$page_main,$tbl_modulus_type;
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
	}else{
		LastLogTime();
	}
	$server_id = $var["serverid"];
	$type = intval($var["type"]);
	
	$str = "";
	
	if($var["begin_time"] != null && $var["begin_time"] != ""){
		$str .= " and lcesm_dtChangeTime >= '".$var["begin_time"]."'";
	}
	if($var["end_time"] != null && $var["end_time"] != ""){
		$str .= " and lcesm_dtChangeTime <= '".$var["end_time"]."'";
	}
	if($type != 0){
		$str .= " and lcesm_uModulusType = ".$type;
	}
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	if($var["download"]=="true"){
		fwrite($file, "\"".UTF82Local(implode("\",\"",array($tbl_sys_msg["time"],$page_main["type"],$page_main["number"]))."\"\n"));
	}else{
		echo "<table class='list_table'>\n";
		printf("<tr><th style='width:120px;'>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"],$page_main["type"],$page_main["number"]);
	}
	
	$qs = "select lcesm_dtChangeTime, lcesm_uModulusType, lcesm_uModulusValue from tbl_log_char_exp_soul_modulus where lcs_uId = $role_id".$str." order by lcesm_dtChangeTime";
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		if($var["download"]=="true"){
			fwrite($file, UTF82Local("\"".implode("\",\"",array($row[0],$tbl_modulus_type[$row[1]],$row[2]))."\"\n"));
		}else{
			echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td><pre>%s</pre></td></tr>\n",$row[0],$tbl_modulus_type[$row[1]],$row[2]);
		}
	}
	if($var["download"]=="true"){
		global $page_action_name;

		fclose($file);
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=".$page_action_name[$var["action_name"]]."';\n";
		echo "</script>\n";
	}else{
		echo "</table>\n";
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_sys_msg["download"]."</a></td></tr></table>\n";
		ReturnAndBack();
	}
	return true;
}

function getDateLimit($var, $server=""){
	if($server=="") $server = $_REQUEST["serverid"];
	$str = "";
	if($var["begin_time"]!=null && $var["begin_time"]!=""){
		$qs = "select le_uId from tbl_log_event where le_dtDateTime >= '".$var["begin_time"]."' limit 1";
		$rs = MySQLQuery($qs, "Log", $server);
		if($row = mysql_fetch_row($rs)){
			$str.= " and le.le_uId >= ".$row[0];
			if($var["serverid"]=="808003" && $row[0] < 148855607){
				$str.= " and le.le_uId < 148855607";
			}
		}
	}
	if($var["end_time"]!=null && $var["end_time"]!=""){
		$qs = "select le_uId from tbl_log_event where le_dtDateTime <= '".$var["end_time"]."' order by le_dtDateTime desc limit 1";
		$rs = MySQLQuery($qs, "Log", $server);
		if($row = mysql_fetch_row($rs)){
			$str.= " and le.le_uId <= ".$row[0];
		}
	}
	return $str;
}

function ShowJifenLog($var, $gm){
	return true;
}

function ShowEquipUpgrade($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_log_db,$tbl_equip_upgrade_type;
	
	$server_id = $var["serverid"];
	$item_id = $var["item_id"];
	$types = $var["type"];
	
	if(!$types) return false;
	
	LastLogTime();
	
	$js_str = <<<_js_str
<script>
function detail(event_id){
	document.form1.action_name.value = '205';
	document.form1.event_id.value = event_id;
	document.form1.submit();
}
</script>
_js_str;
	echo $js_str."\n";
	form($var,$gm);
	echo "<input type='hidden' name='item_id' value='$item_id' />\n";
	echo "<input type='hidden' name='event_id' />\n";
	echo "</form>\n";
	
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th style='width:100px;'>%s</th><th style='width:100px;'>%s</td><th>%s</th><th style='width:60px;'>%s</th></tr>\n",$tbl_sys_msg["time"],$page_main["event_type"],$page_main["role_name"],$tbl_sys_msg["desc"],$tbl_sys_msg["detail"]);
	
	$date_limit = getDateLimit($var,$server_id);
	$tables = array();
	foreach($types as $type){
		switch($type){
			case "armor":
				array_push($tables,"(select 'armor', le.le_uId, concat_ws(',',liea_uIndex,liea_uAddTimes,liea_sAttr) from tbl_log_item_equip_armor liea, tbl_log_event le where lis_uId = $item_id and liea.le_uId = le.le_uId $date_limit)");
				break;
			case "advance":
				array_push($tables,"(select 'advance', le.le_uId, concat_ws(',',liea_sAdvancedAttr1,liea_uAdvancedAttrValue1,liea_sAdvancedAttr2,liea_uAdvancedAttrValue2,liea_sAdvancedAttr3,liea_uAdvancedAttrValue3,liea_sAdvancedAttr4,liea_uAdvancedAttrValue4) from tbl_log_item_equip_advance liea, tbl_log_event le where lis_uId = $item_id and liea.le_uId = le.le_uId $date_limit)");
				break;
			case "intensify":
				array_push($tables,"(select 'intensify', le.le_uId, concat_ws(',',liei_uAddAttr1,liei_uAddAttrValue1,liei_uAddAttr2,liei_uAddAttrValue2) from tbl_log_item_equip_intensify liei, tbl_log_event le where lis_uId = $item_id and liei.le_uId = le.le_uId $date_limit)");
				break;
			case "intensifyAddAttr":
				array_push($tables,"(select 'intensifyAddAttr', le.le_uId, concat_ws(',', liei_sAddAttr4, liei_uAddAttr4Value, liei_sAddAttr5, liei_uAddAttr5Value, liei_sAddAttr6, liei_uAddAttr6Value, liei_sAddAttr7, liei_uAddAttr7Value, liei_sAddAttr8, liei_uAddAttr8Value, liei_sAddAttr9, liei_uAddAttr9Value) from tbl_log_item_equip_intensifyAddAttr liea, tbl_log_event le where lis_uId = $item_id and liea.le_uId = le.le_uId $date_limit)");
				break;
			case "superaddation":
				array_push($tables,"(select 'superaddation', le.le_uId, lies_uCurSuperaddPhase from tbl_log_item_equip_superaddation liea, tbl_log_event le where lis_uId = $item_id and liea.le_uId = le.le_uId $date_limit)");
		}
	}
	
	$qs = "select le.le_dtDateTime, lc.lc_sName, tbl.* ";
	$qs.= "from tbl_log_event le, tbl_log_player lp, tbl_log_char lc, (";
	$qs.= implode("union",$tables);
	$qs.= ") as tbl ";
	$qs.= "where tbl.le_uId = le.le_uId and le.le_uId = lp.le_uId and lp.lcs_uId = lc.lcs_uId ";
	$qs.= "order by le.le_uId";
	
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		$type = $row[2];
		switch($type){
			case "armor":
				$desc = vsprintf($tbl_equip_upgrade_type["armor"][1],explode(",",Local2UTF8($row[4])));
				break;
			case "advance":
				$val = explode(",",Local2UTF8($row[4]));
				$desc = "<table class='inner_table'>\n";
				if($val[1]!=0)
					$desc.= "<tr><td>".$val[0]."</td><td>".$val[1]."</td><td></td></tr>\n";
				if($val[3]!=0)
					$desc.= "<tr><td>".$val[2]."</td><td>".$val[3]."</td><td></td></tr>\n";
				if($val[5]!=0)
					$desc.= "<tr><td>".$val[4]."</td><td>".$val[5]."</td><td></td></tr>\n";
				if($val[7]!=0)
					$desc.= "<tr><td>".$val[6]."</td><td>".$val[7]."</td><td></td></tr>\n";
				$desc.= "</table>\n";
				break;
			case "intensify":
				$val = explode(",",Local2UTF8($row[4]));
				$desc = "<table class='inner_table'>\n";
				if($val[1]!=0)
					$desc.= "<tr><td>".$val[0]."</td><td>".$val[1]."</td><td></td></tr>\n";
				if($val[3]!=0)
					$desc.= "<tr><td>".$val[2]."</td><td>".$val[3]."</td><td></td></tr>\n";
				$desc.= "</table>\n";
				break;
			case "intensifyAddAttr":
				$val = explode(",",Local2UTF8($row[4]));
				$desc = "<table class='inner_table'>\n";
				if($val[1]!=0)
					$desc.= "<tr><td>".$val[0]."</td><td>".$val[1]."</td><td></td></tr>\n";
				if($val[3]!=0)
					$desc.= "<tr><td>".$val[2]."</td><td>".$val[3]."</td><td></td></tr>\n";
				if($val[5]!=0)
					$desc.= "<tr><td>".$val[4]."</td><td>".$val[5]."</td><td></td></tr>\n";
				if($val[7]!=0)
					$desc.= "<tr><td>".$val[6]."</td><td>".$val[7]."</td><td></td></tr>\n";
				if($val[9]!=0)
					$desc.= "<tr><td>".$val[8]."</td><td>".$val[9]."</td><td></td></tr>\n";
				if($val[11]!=0)
					$desc.= "<tr><td>".$val[10]."</td><td>".$val[11]."</td><td></td></tr>\n";
				$desc.= "</table>\n";
				break;
			case "superaddation":
				$desc = sprintf($tbl_equip_upgrade_type["superaddation"][1],$row[4]);
				break;
		}
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td align='center'><button onclick='detail(%d);'>%s</button></td></tr>\n",$row[0],$tbl_equip_upgrade_type[$row[2]][0],$row[1],$desc,$row[3],$tbl_sys_msg["detail"]);
	}
	
	echo "</table>\n";

	return true;
}

function GetRoleSceneLog($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_log_db,$tbl_serv_db;
	
	$server_id = $var["serverid"];
	
	LastLogTime();
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"],$page_main["scene"]);
	
	$qs = "select le.le_dtDateTime, ls.ls_sSceneName ";
	$qs.= "from tbl_log_event le, tbl_log_player lp, tbl_log_scene ls ";
	$qs.= "where ls.le_uId = le.le_uId and le.le_uId = lp.le_uId and ";
	$qs.= "lp.lcs_uId = '$role_id' ".getDateLimit($var,$server_id)." ";
	$qs.= "order by le.le_uId";
	
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		printf("<tr><td>%s</td><td>%s</td></tr>\n",$row[0],Lan_Scene_GetDisplayName(Local2UTF8($row[1])));
	}
	
	echo "</table>\n";

	return true;
}

function GetRoleTeamLog($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_log_db,$tbl_serv_db,$tbl_team_log;
	
	$server_id = $var["serverid"];
	
	LastLogTime();
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"],$page_main["type"],$page_main["role_name"],$tbl_sys_msg["status"]);
	
	$qs = "select le.le_dtDateTime, let.let_uTeamId ";
	$qs.= "from tbl_log_player lp, tbl_log_event le, tbl_log_enter_team let ";
	$qs.= "where le.le_uId = lp.le_uId and le.le_uId = let.le_uId and ";
	$qs.= "lp.lcs_uId = '$role_id' ".getDateLimit($var,$server_id)." ";
	$qs.= "order by le.le_uId";
	
	$result = MySQLQuery($qs, "Log", $server_id);
	while($row=mysql_fetch_row($result)){
		$team_member = array();
		printf("<tr><td>%s</td><td colspan='3'></td></tr>\n",$row[0]);
		$qs = "(select le.le_dtDateTime, 0, lp.lcs_uId from tbl_log_enter_team let, tbl_log_event le, tbl_log_player lp where let.le_uId = le.le_uId and lp.le_uId = le.le_uId and let.let_uTeamId = ".$row[1].")";
		$qs.= "union";
		$qs.= "(select le.le_dtDateTime, 1, lp.lcs_uId from tbl_log_leave_team llt, tbl_log_event le, tbl_log_player lp where llt.le_uId = le.le_uId and lp.le_uId = le.le_uId and llt.let_uTeamId = ".$row[1].")";
		$qs.= "order by le_dtDateTime";
		$rs = MySQLQuery($qs, "Log", $server_id);
		while($r = mysql_fetch_row($rs)){
			$role_name = GetRoleName($r[2],$server_id);
			if($r[1]=="0"){
				$type = $tbl_sys_msg["enter"];
				array_push($team_member, $role_name);
			}else{
				$type = $tbl_sys_msg["leave"];
				$index = array_search($role_name, $team_member);
				unset($team_member[$index]);
			}
			printf("<tr style='color: #888;'><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$r[0], $type, $role_name, implode(", ", $team_member));
		}
	}
	echo "</table>\n";

	return true;
}

function GetTongItemLog($var, $gm){
	global $tbl_sys_msg,$page_main,$tbl_tong_depot_log,$tbl_tong_pos;
	
	$server_id = $var["serverid"];
	$tong_name = $var["tong_name"];
	
	LastLogTime();
	
	$qs = "select t_uId from tbl_tong where t_sName = '$tong_name'";
	$rs = MySQLQuery($qs, "Game", $server_id);
	if($row = mysql_fetch_row($rs)){
		$tong_id = $row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
		
	echo "<table class='list_table'>\n";
	printf("<tr><th style='width:120px;'>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["time"],$page_main["type"],$page_main["item"], $page_main["number"], $page_main["role_name"]."(".$page_main["position"].")");
	
	$qs = "select le.le_dtDateTime, ltd.ltd_uType, lis.lis_sName, ltd.ltd_uExecutorId, ltd.ltd_uExecutorPosition, count(*) ";
	$qs.= "from tbl_log_tong_depot ltd, tbl_log_event le, tbl_log_item_static lis ";
	$qs.= "where ltd.le_uId = le.le_uId and ltd.lis_uId = lis.lis_uId and ";
	$qs.= "lts_uId = $tong_id".getDateLimit($var,$server_id)." ";
	$qs.= "group by le.le_dtDateTime, lis.lis_sName, ltd.ltd_uExecutorId";
	
	$result = MySQLQuery($qs, "Log", $server_id);
	
	while($row=mysql_fetch_row($result)){
		if($row[3]==0){
			$role_name = $tbl_sys_msg["admin"];
		}else{
			$role_name = GetRoleName($row[3], $server_id);
		}
		if($row[4]==0){
			$position = $tbl_sys_msg["admin"];
		}else{
			$position = $tbl_tong_pos[$row[4]];
		}
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_tong_depot_log[$row[1]],Lan_Item_GetDisplayName(Local2UTF8($row[2])),$row[5],$role_name."(".$position.")");
	}
	
	echo "</table>\n";

	return true;
}

function LastLogDate($var, $gm){
	global $tbl_game_server,$tbl_sys_msg,$tbl_serv_partition;
	
	foreach($tbl_serv_partition[$var["gameId"]] as $group_name => $group){
		echo "<b>".$group_name."</b><br/>";
		foreach($group as $serv_id){
			$server = $tbl_game_server[$serv_id];
			echo "<b>".$server[2]."</b>: ";
			$qs = "select le_dtDateTime from tbl_log_event where le_uId = (select max(le_uId) from tbl_log_service_online_num)";
			$rs = MySQLQuery($qs, "Log", $serv_id);
			if($row = mysql_fetch_row($rs)){
				echo $row[0];
			}
			echo "<br/>\n";
		}
	}
	return true;
}
?>
