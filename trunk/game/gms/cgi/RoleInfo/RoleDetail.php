<?php
$tbl_level_exp = array(
	1=>954,
	2=>1257,
	3=>1726,
	4=>2386,
	5=>3261,
	6=>4381,
	7=>5774,
	8=>7467,
	9=>9491,
	10=>11877,
	11=>14656,
	12=>17860,
	13=>21524,
	14=>25681,
	15=>30366,
	16=>35615,
	17=>41464,
	18=>47953,
	19=>55118,
	20=>62999,
	21=>71637,
	22=>81072,
	23=>91347,
	24=>102504,
	25=>114588,
	26=>127641,
	27=>141711,
	28=>156842,
	29=>173083,
	30=>190481,
	31=>209086,
	32=>228946,
	33=>250112,
	34=>272636,
	35=>296571,
	36=>321969,
	37=>348884,
	38=>377371,
	39=>407487,
	40=>439287,
	41=>472829,
	42=>508172,
	43=>545375,
	44=>584497,
	45=>625600,
	46=>668746,
	47=>713996,
	48=>761416,
	49=>811069,
	50=>863020,
	51=>917336,
	52=>974084,
	53=>1033331,
	54=>1095146,
	55=>1159599,
	56=>1226760,
	57=>1296700,
	58=>1369492,
	59=>1445210,
	60=>2110149,
	61=>2813075,
	62=>3555142,
	63=>4337525,
	64=>5161415,
	65=>6028020,
	66=>6938564,
	67=>7894292,
	68=>8896462,
	69=>9946353,
	70=>12421178,
	71=>15018132,
	72=>17740488,
	73=>20591563,
	74=>23574719,
	75=>26693360,
	76=>29950931,
	77=>33350924,
	78=>36896872,
	79=>40592351,
	80=>44440981,
	81=>45550981,
	82=>45550982,
	83=>45550983,
	84=>45550984,
	85=>45550985,
	86=>45550986,
	87=>45550987,
	88=>45550988,
	89=>45550989,
	90=>45550990,
	91=>45550991,
	92=>45550992,
	93=>45550993,
	94=>45550994,
	95=>45550995,
	96=>45550996,
	97=>45550997,
	98=>45550998,
	99=>45550999,
	100=>45551000,
	101=>45551001,
	102=>45825645,
	103=>46756266,
	104=>47705382,
	105=>48673306,
	106=>49660353,
	107=>50666840,
	108=>51693089,
	109=>52739421,
	110=>53806162,
	111=>54893640,
	112=>56002186,
	113=>57132132,
	114=>58283815,
	115=>59457574,
	116=>60653748,
	117=>61872681,
	118=>63114720,
	119=>64380214,
	120=>65669513,
	121=>66982971,
	122=>68320946,
	123=>69683796,
	124=>71071882,
	125=>72485570,
	126=>73925226,
	127=>75391219,
	128=>76883922,
	129=>78403709,
	130=>79950958,
	131=>81526048,
	132=>83129361,
	133=>84761284,
	134=>86422202,
	135=>88112508,
	136=>89832592,
	137=>91582851,
	138=>93363683,
	139=>95175489,
	140=>97018670,
	141=>98893634,
	142=>100800789,
	143=>102740545,
	144=>104713316,
	145=>106719518,
	146=>108759570,
	147=>110833893,
	148=>112942911,
	149=>115087051,
	150=>117266742,
);

function GetRoleDetail($role_id,$serverid)
{
	$str="";
	
	$query_string = "select c_sName, us_sName, cs_uSex, cs_uClass, cb_uLevel, cs_uLevelExp, cm_uMoney, cm_uBindingMoney, cs_uCamp ";
	$query_string.= "from(";
	$query_string.= "(select cs_uId,c_sName from tbl_char c where c.cs_uId='$role_id') ";
	$query_string.= "union ";
	$query_string.= "(select cs_uId,cd_sName c_sName from tbl_char_deleted d where d.cs_uId='$role_id') ";
	$query_string.= ") c, tbl_char_basic cb ,tbl_char_static cs ,tbl_user_static us, tbl_char_money cm, tbl_char_experience ce ";
	$query_string.= "where c.cs_uId=cb.cs_uId and c.cs_uId = cs.cs_uId and cs.us_uId = us.us_uId and c.cs_uid = cm.cs_uId and c.cs_uId = ce.cs_uId";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $tbl_sex,$tbl_class,$tbl_camp,$tbl_level_exp,$page_main,$tbl_sys_msg;

	
	$str.= "<table class='list_table'>\n";
	$str.= sprintf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n",$page_main["role_name"],$page_main["user_name"],$tbl_sys_msg["sex"],$tbl_sys_msg["class"],$page_main["level"],$page_main["exp"],$tbl_sys_msg["MoneyType1"],$tbl_sys_msg["MoneyType2"],$page_main["camp"]);
	
	if($row=mysql_fetch_row($result)){
		$str.= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$row[1],$tbl_sex[intval($row[2])],$tbl_class[intval($row[3])],$row[4],$row[5]."/".$tbl_level_exp[$row[4]],$row[6],$row[7],$tbl_camp[intval($row[8])]);
	}
	$str=$str.sprintf("</table>\n");

	return $str;
}

function GetRoleLocation($role_id,$serverid)
{
	$query_string = "select sc_sSceneName, cp_uPosX, cp_uPosY 
		from tbl_char_position cp, tbl_scene s
		where s.sc_uId=cp.sc_uId and cp.cs_uId='".$role_id."'";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $page_main;
	
	$str.= "<table class='list_table'>\n";
	$str.= sprintf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n",$page_main["scene"],$page_main["x"],$page_main["y"]);
	
	while($row=mysql_fetch_row($result)){
		$str.= sprintf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n",Lan_Scene_GetDisplayName(Local2UTF8($row[0])),$row[1],$row[2]);
	}
	$str=$str.sprintf("</table>\n");

	return $str;
}

function GetRoleSort($role_id,$serverid){
	global $tbl_role_sort;
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th style='width:150px;'>".$tbl_role_sort[0][0]."</th><th style='width:50px;'>".$tbl_role_sort[0][1]."</th><th style='width:50px;'>".$tbl_role_sort[0][2]."</th><th>".$tbl_role_sort[0][3]."</th></tr>\n";
	
	$line = "<tr><td>%s</td><td align='right'>%s</td><td align='right'>%s</td><td>%s</td></tr>\n";
	
	if($row = mysql_fetch_row(MySQLQuery("select sl_uOrder, sl_uUpdown,sl_uLevel from tbl_sort_level where cs_uId = '$role_id'","Game",$serverid))){
		$str.=sprintf($line,$tbl_role_sort[1]["level"][0],$row[0],$row[1],sprintf($tbl_role_sort[1]["level"][1],$row[2]));
	}else{
		$str.=sprintf($line,$tbl_role_sort[1]["level"][0],"-","-","-");
	}
	
	if($row = mysql_fetch_row(MySQLQuery("select sm_uOrder, sm_uUpdown,sm_uMoney from tbl_sort_money where cs_uId = '$role_id'","Game",$serverid))){
		$str.=sprintf($line,$tbl_role_sort[1]["money"][0],$row[0],$row[1],sprintf($tbl_role_sort[1]["money"][1],substr($row[2],0,-4),substr($row[2],-4,-2),substr($row[2],-2)));
	}else{
		$str.=sprintf($line,$tbl_role_sort[1]["money"][0],"-","-","-");
	}
		
	if($row = mysql_fetch_row(MySQLQuery("select sd_uOrder, sd_uUpdown,sd_uDeadTimes from tbl_sort_deadtimes where cs_uId = '$role_id'","Game",$serverid))){
		$str.=sprintf($line,$tbl_role_sort[1]["dead"][0],$row[0],$row[1],sprintf($tbl_role_sort[1]["dead"][1],$row[2]));
	}else{
		$str.=sprintf($line,$tbl_role_sort[1]["dead"][0],"-","-","-");
	}
	
	if($row = mysql_fetch_row(MySQLQuery("select skpc_uOrder, skpc_uUpdown,skpc_uKillPlayerCount from tbl_sort_kill_player_count where cs_uId = '$role_id'","Game",$serverid))){
		$str.=sprintf($line,$tbl_role_sort[1]["kill_player"][0],$row[0],$row[1],sprintf($tbl_role_sort[1]["kill_player"][1],$row[2]));
	}else{
		$str.=sprintf($line,$tbl_role_sort[1]["kill_player"][0],"-","-","-");
	}
	
	if($row = mysql_fetch_row(MySQLQuery("select sknc_uOrder, sknc_uUpdown,sknc_uKillNpcCount from tbl_sort_kill_npc_count where cs_uId = '$role_id'","Game",$serverid))){
		$str.=sprintf($line,$tbl_role_sort[1]["kill_npc"][0],$row[0],$row[1],sprintf($tbl_role_sort[1]["kill_npc"][1],$row[2]));
	}else{
		$str.=sprintf($line,$tbl_role_sort[1]["kill_npc"][0],"-","-","-");
	}
	
	if($row = mysql_fetch_row(MySQLQuery("select spo_uOrder, spo_uUpdown,spo_uPlayerOnTimeTotal from tbl_sort_player_onlinetime where cs_uId = '$role_id'","Game",$serverid))){
		$str.=sprintf($line,$tbl_role_sort[1]["online"][0],$row[0],$row[1],sprintf($tbl_role_sort[1]["online"][1],$row[2]/(24*60*60),($row[2]/(60*60))%24,($row[2]/60)%(60),$row[2]%60));
	}else{
		$str.=sprintf($line,$tbl_role_sort[1]["online"][0],"-","-","-");
	}
	
	$str.= "</table>\n";
	return $str;
}

function GetRoleAppellation($role_id,$serverid){
	global $tbl_role_appellation,$sys_config;
	$str = "<table class='form_table'>\n";
	$str.= "<tr>\n";
	$str.= "<th>$tbl_role_appellation[0]</th>\n";
	if($row = mysql_fetch_row(MySQLQuery("select ca_uAppellationId1, ca_uAppellationId2 from tbl_char_appellation where cs_uId='$role_id'","Game",$serverid)))
		$str.= sprintf("<td>%s</td>\n",Lan_Appellation($row[0]).Lan_Appellation($row[1]));
	else
		$str.= "<td>-</td>\n";
	$str.= "</tr>\n";
	$str.= "</table>\n";
	
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th>".$tbl_role_appellation[1][0]."</th></tr>\n";
	$rs = MySQLQuery("select ca_uAppellation from tbl_char_appellationList where cs_uId='$role_id'","Game",$serverid);
	while($row=mysql_fetch_row($rs)){
		$str.= sprintf("<tr><td>%s</td></tr>\n",Lan_Appellation($row[0]));
	}
	$str.= "</table>\n";
	return $str;
}

function GetRoleItem($role_id,$b,$serverid)
{
	$query_string = "select s.is_sName, count(s.is_uId), gir.gir_uRoomIndex, gir.gir_uPos, s.is_uType, s.is_uId
		from tbl_item_static s, tbl_item_in_grid iig, tbl_grid_in_room gir 
		left join tbl_item_bag_in_use ibiu on (gir.cs_uId = ibiu.cs_uId and gir.gir_uRoomIndex = ibiu.ibiu_uRoomIndex)
		where s.is_uId = iig.is_uId and gir.gir_uGridID = iig.gir_uGridID 
		and gir.cs_uId = '".$role_id."'
		and ((ibiu.ibiu_uBagSlotIndex >= 1 and ibiu.ibiu_uBagSlotIndex <= 4)
				or
				(ibiu.ibiu_uBagSlotIndex is Null and gir.gir_uRoomIndex = 1))
		group by s.is_uType, s.is_sName, gir.cs_uId, gir_uRoomIndex, gir_uPos 
		order by gir.gir_uRoomIndex, gir.gir_uPos";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $tbl_role_item,$tbl_role_item_room,$tbl_role_item_room1,$tbl_sys_msg,$page_main;
	
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th style='width:100px;'>".$page_main["item_id"]."</th><th>".$page_main["item"]."</th><th style='width:40px;'>".$page_main["number"]."</th><th style='width:150px;'>".$page_main["pos"]."</th><th style='width:60px;'>".$tbl_sys_msg["detail"]."</th><th style='width:60px;'>".$tbl_sys_msg["delete"]."</th></tr>\n";
	
	while($row=mysql_fetch_row($result)){
		$row[0] = Local2UTF8($row[0]);
		$arg1 = $row[2];
		$arg2 = $row[3];
		if($arg1>49){
			$row[2] = sprintf($tbl_role_item_room,$arg1-49,$arg2);
		}else{
			$row[2] = sprintf($tbl_role_item_room1,$arg2);
		}
		$row[3] = ($b)?sprintf("<input type=button value=\"%s\" onclick=\"DelItemInBag('%d_%s',%d)\" />",$tbl_sys_msg["delete"],$row[4],$row[0],$row[1]):"-";
		$detail = ($row[4]>4 && $row[4]<10)?sprintf("<input type=button value='%s' onclick='OnDetail(%d)'/>",$tbl_sys_msg["detail"],$row[5]):"";
		$str.= sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td align='right'>%d</td><td>%s</td><td align='center'>%s</td><td align='center'>%s</td></tr>\n",$row[5],Lan_Item_GetDisplayName($row[0]),$row[1],$row[2],$detail,$row[3]);
	}
	$str=$str.sprintf("</table>\n");
	$str=$str."<table class='list_table'>\n";
	$str=$str."<tr><td><input type='button' onclick='ClearItemInBag()' value='".$tbl_sys_msg["delete"]."' /></td></tr>\n";
	$str=$str."</table>\n";
	return $str;
}

function GetRoleEquip($role_id,$b,$serverid)
{
	$query_string = "select e.ce_uEquipPart, s.is_sName, s.is_uId, s.is_uType 
		from tbl_item_equip e, tbl_item_static s
		where e.is_uId = s.is_uId and e.cs_uId='".$role_id."'";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $page_main,$tbl_part,$tbl_sys_msg;
	
	$count = count($tbl_role_equip);
	
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th style='width:100px;'>".$page_main["item_id"]."</th><th style='width:50px;'>".$page_main["pos"]."</th><th>".$page_main["item"]."</th><th style='width:60px;'>".$tbl_sys_msg["detail"]."</th><th style='width:60px;'>".$tbl_sys_msg["delete"]."</th></tr>\n";
	
	while($row=mysql_fetch_row($result)){
		$id = $row[2];
		$row[0] = $tbl_part[intval($row[0])];
		$row[1] = Local2UTF8($row[1]);
		$row[2] = sprintf("<input type=button value='%s' onclick='OnDetail(%d)'/>",$tbl_sys_msg["detail"],$row[2]);
		$row[3] = ($b)?sprintf("<input type=button value=\"%s\" onclick=\"DelItemOnBody('%d_%s')\" />",$tbl_sys_msg["delete"],$row[3],$row[1]):"-";
		$str.= sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td align='center'>%s</td><td align='center'>%s</td></tr>\n",$id,$row[0],Lan_Item_GetDisplayName($row[1]),$row[2],$row[3]);
	}
	$str=$str.sprintf("</table>\n");

	return $str;
}

function GetRoleStorage($role_id, $b,$serverid)
{
	$query_string = "select s.is_sName, count(s.is_uId), gir.gir_uRoomIndex, gir.gir_uPos, s.is_uType, s.is_uId
		from tbl_item_static s, tbl_item_in_grid iig, tbl_grid_in_room gir 
		left join tbl_item_bag_in_use ibiu on (gir.cs_uId = ibiu.cs_uId and gir.gir_uRoomIndex = ibiu.ibiu_uRoomIndex)
		where s.is_uId = iig.is_uId and gir.gir_uGridID = iig.gir_uGridID 
		and gir.cs_uId = '".$role_id."'
		and	(ibiu.ibiu_uBagSlotIndex is Null and gir.gir_uRoomIndex = 2)
		group by s.is_uType, s.is_sName, gir.cs_uId, gir_uRoomIndex, gir_uPos 
		order by gir.gir_uRoomIndex, gir.gir_uPos";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $tbl_role_item,$tbl_role_item_room,$tbl_role_item_room1,$tbl_sys_msg,$page_main;
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th style='width:100px;'>".$page_main["item_id"]."</th><th>".$page_main["item"]."</th><th style='width:40px;'>".$page_main["number"]."</th><th style='width:150px;'>".$page_main["pos"]."</th><th style='width:60px;'>".$tbl_sys_msg["detail"]."</th><th style='width:60px;'>".$tbl_sys_msg["delete"]."</th></tr>\n";
	
	while($row=mysql_fetch_row($result)){
		$row[0] = Local2UTF8($row[0]);
		$arg1 = $row[2];
		$arg2 = $row[3];
		if($arg1>49){
			$row[2] = sprintf($tbl_role_item_room,$arg1-49,$arg2);
		}else{
			$row[2] = sprintf($tbl_role_item_room1,$arg2);
		}
		$detail = ($row[4]>4 && $row[4]<10)?sprintf("<input type=button value='%s' onclick='OnDetail(%d)'/>",$tbl_sys_msg["detail"],$row[5]):"";
		$row[3] = ($b)?sprintf("<input type=button value=\"%s\" onclick=\"DelItemInDepot('%d_%s',%d)\" />",$tbl_sys_msg["delete"],$row[4],$row[0],$row[1]):"-";
		$str.= sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td align='right'>%d</td><td>%s</td><td align='center'>%s</td><td align='center'>%s</td></tr>\n",$row[5],Lan_Item_GetDisplayName($row[0]),$row[1],$row[2],$detail,$row[3]);
	}
	$str=$str.sprintf("</table>\n");
	$str=$str."<table class='list_table'>\n";
	$str=$str."<tr><td><input type='button' onclick='ClearItemInDepot()' value='".$tbl_sys_msg["delete"]."' /></td></tr>\n";
	$str=$str."</table>\n";
	return $str;
}

function GetRoleMailItem($role_id,$serverid)
{
	$query_string = "select m.m_sTitle,m.cs_sSender,is.is_sName,count(*),mi.is_uId, is.is_uType from tbl_mail m, tbl_mail_item mi, tbl_item_static `is` where m.ms_uId = mi.ms_uId and mi.is_uId = `is`.is_uId and m.cs_uReciever = '$role_id' group by m.ms_uId, mi.mi_uSlotIndex;";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $tbl_role_mail_item,$page_main,$tbl_sys_msg;
	
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th>".$tbl_role_mail_item[0]."</th><th style='width:80px;'>".$tbl_role_mail_item[1]."</th><th style='width:100px;'>".$page_main["item_id"]."</th><th style='width:150px;'>".$page_main["item"]."</th><th style='width:60px;'>".$page_main["number"]."</th><th style='width:60px;'>".$tbl_sys_msg["detail"]."</th></tr>\n";
	
	while($row=mysql_fetch_row($result)){
		$row[2] = Local2UTF8($row[2]);
		$detail = ($row[5]>4 && $row[5]<10)?sprintf("<input type=button value='%s' onclick='OnDetail(%d)'/>",$tbl_sys_msg["detail"],$row[4]):"";
		$str.= sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%d</td><td>%s</td></tr>\n",$row[0],$row[1],$row[4],Lan_Item_GetDisplayName($row[2]),$row[3],$detail);
	}
	$str=$str.sprintf("</table>\n");
	return $str;
}

function GetRoleMarketItem($role_id,$serverid)
{
	$query_string = "select mso_sItemName,mso_uCount,mso_uPrice,mso_tEndTime from tbl_market_sell_order where cs_uId='$role_id'";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $page_main;
	
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th>".$page_main["item"]."</th><th style='width:40px;'>".$page_main["number"]."</th><th style='width:50px;'>".$page_main["price"]."</th><th style='width:120px;'>".$page_main["end_time"]."</th></tr>\n";
	
	while($row=mysql_fetch_row($result)){
		$row[0] = Local2UTF8($row[0]);
		$str.= sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td>%d</td><td>%d</td><td>%s</td></tr>\n",Lan_Item_GetDisplayName($row[0]),$row[1],$row[2],$row[3]);
	}
	$str=$str.sprintf("</table>\n");
	return $str;
}

function GetRoleSkill($role_id,$serverid)
{
	$query_string = "select fs_sName,fs_uLevel 
		from tbl_fight_skill as fs
		where cs_uId = ".$role_id." group by fs_sName";
	$result = MySQLQuery($query_string, "Game",$serverid);
	
	global $tbl_sys_msg,$page_main;
	
	$str.= "<table class='list_table'>\n";
	$str.= sprintf("<tr><th>%s</th><th>%s</th></tr>\n",$tbl_sys_msg["skill"],$page_main["level"]);
	
	while($row=mysql_fetch_row($result)){
		$str.= sprintf("<tr><td>%s</td><td>%s</td></tr>\n",Lan_Skill_GetDisplayName(Local2UTF8($row[0])),$row[1]);
	}
	$str=$str.sprintf("</table>\n");

	return $str;
}

function GetRoleLogin($role_id,$serverid){
	$query_string = "select if(co_dtLastLogOutTime>co_dtLastLoginTime,0,1),co_dtLastLoginTime,co_dtLastLogOutTime from tbl_char_onlinetime where cs_uId='$role_id'";
	$result = MySQLQuery($query_string,"Game",$serverid);
	
	global $tbl_sys_msg;
	
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th>".$tbl_sys_msg["status"]."</th><th>".$tbl_sys_msg["LastLoginTime"]."</th><th>".$tbl_sys_msg["LastLogoutTime"]."</th></tr>\n";
	
	while($row=mysql_fetch_row($result)){
		if($row[0]!=0){
			$row[0] = $tbl_sys_msg["online"];
			$row[2] = $tbl_sys_msg["online"];
		}else{
			$row[0] = $tbl_sys_msg["offline"];
		}
		$str.= sprintf("<tr".(($k++%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$row[1],$row[2]);
	}
	$str=$str.sprintf("</table>\n");

	return $str;
}

function GetRoleStone($role_id,$serverid){
	global $tbl_role_stone;
	$qs = "select sh.sh_uDepart, sh.sh_uSlot, siu.siu_sName from tbl_stone_hole sh left join tbl_stone_in_use siu on sh.sh_uId = siu.sh_uId  where sh.cs_uId = $role_id";
	$rs = MySQLQuery($qs,"Game",$serverid);
	$str.= "<table class='list_table'>\n";
	$str.= "<tr><th>$tbl_role_stone[0]</th><th>$tbl_role_stone[1]</th><th>$tbl_role_stone[2]</th></tr>\n";
	while($row = mysql_fetch_row($rs)){
		$str.= sprintf("<tr><td>%s</td><td>%d</td><td>%s</td></tr>\n",$tbl_role_stone[3][$row[0]],$row[1],Lan_Item_GetDisplayName(Local2UTF8($row[2])));
	}
	$str.= "</table>\n";
	return $str;
}

function GetRoleEquipByID($var, $gm)
{
	$serverid =intval($var["serverid"]);
	$item_id = mysql_escape_string($var["item_id"]);
	
	global $tbl_role_equip_info;
	
	$qs = "select * from tbl_item_static where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		$item_type = $row[1];
		$item_name = Local2UTF8($row[2]);
	}else{
		return false;
	}
	
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
	
	switch($item_type){
		case 5: //weapon
			$qs = "select * from tbl_item_weapon where is_uId = $item_id";
			$result = MySQLQuery($qs, "Game", $serverid);
			if($row = mysql_fetch_row($result)){
				if($row[1]!="") $item_name = $row[1];
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
			$qs = "select * from tbl_item_armor where is_uId = $item_id";
			$result = MySQLQuery($qs, "Game", $serverid);
			if($row = mysql_fetch_row($result)){
				if($row[1]!="") $item_name = $row[1];
				$level = $row[2];
				$intensify = $row[8];
				$inten = $row[9];
				
				$equip_info = "<div class='armor_item'><div class='armor_name'>".$tbl_role_equip_info[1][1][0]."</div><div class='armor_value'>+".$row[4]."</div></div>\n";
				$equip_info.= "<div class='armor_item'><div class='armor_name'>".$tbl_role_equip_info[1][1][1]."</div><div class='armor_value'>+".$row[5]."</div></div>\n";
				$equip_info.= "<div class='armor_item'><div class='armor_name'>".$tbl_role_equip_info[1][1][2]."</div><div class='armor_value'>+".$row[6]."</div></div>\n";
			}
			break;
		case 7: //shield
			$qs = "select * from tbl_item_shield where is_uId = $item_id";
			$result = MySQLQuery($qs, "Game", $serverid);
			if($row = mysql_fetch_row($result)){
				if($row[1]!="") $item_name = $row[1];
				$level = $row[2];
				$intensify = $row[4];
				$inten = $row[5];
			}
			break;
		case 8: //ring 
			$qs = "select * from tbl_item_ring where is_uId = $item_id";
			$result = MySQLQuery($qs, "Game", $serverid);
			if($row = mysql_fetch_row($result)){
				if($row[1]!="") $item_name = $row[1];
				$level = $row[2];
				$intensify = $row[6];
				$inten = $row[7];
				$DPS = $row[4];
				$equip_info = "<div class='ring_attack'>".sprintf($tbl_role_equip_info[1][2][0],$DPS)."</div>\n";
			}
			break;
	}
	
	$addation = 0;
	$qs = "select * from tbl_item_equip_superaddation where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		$addation = $row[1];
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
	$qs = "select * from tbl_item_equip_intensify where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		$inten_times = $row[10];
		$inten_totle = $row[14];
		$suit_info = Lan_Equip_GetSuitInfo(Local2UTF8($row[12]),$row[16]);
		$suit_name = $suit_info["name"];
		if($row[6])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[6]))."</div>\n<div class='intensify_value'>+".$row[7]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[7]/100).")</div>\n")."</div>\n";
		if($row[8])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[8]))."</div>\n<div class='intensify_value'>+".$row[9]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[9]/100).")</div>\n")."</div>\n";
	}
	
	if($suit_name<>"") $suit_name .= "<br/>";
	
	echo "<div class='head'>\n<div class='name'>".$suit_name.Lan_Item_GetDisplayName($item_name,$item_type,false).$addation_time."</div>\n<div class='level'>".$level.$tbl_role_equip_info[7]."</div>\n".$addatiom_desc."</div>\n";
	
	$qs = "select * from tbl_item_is_binding where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		if($row[1]>0){
			global $tbl_item_is_binding;
			echo "<div class='binding'>".$tbl_item_is_binding[$row[1]]."</div>\n";
		}
	}
	
	$qs = "select * from tbl_item_equip_durability where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		echo "<div class='durability'>".sprintf($tbl_role_equip_info[0],$row[2],$row[1])."</div>\n";
	}
	
	echo "<div class='equip_info'>\n$equip_info</div>\n";
	
	$qs = "select * from tbl_item_equip_armor where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if(mysql_num_rows($result)>0){
		echo "<div class='armor'>\n";
		while($row = mysql_fetch_row($result)){
			echo "<div class='armor_item'>\n";
			$armor = Lan_Equip_GetArmorPiece(Local2UTF8($row[1]));
			echo "<div class='armor_name'>".$armor[1]."</div>\n";
			echo "<div class='armor_value'>+".($armor[2]*$row[2])."</div>\n";
			if($armor[0]>1){
				echo "<div class='aromr_time'>(".$row[2]."/".$armor[0].")</div>\n";
			}
		}
		echo "</div>\n";
	}
	
	$qs = "select * from tbl_item_equip_advance where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		echo "<div class='advance'>\n";
		$advance_name = Local2UTF8($row[15]);
		$advance_cur_tiems = $row[1];
		$advance_tot_tiems = $row[2];
		if($advance_tot_tiems>0)
			echo "<div class='advance_item'>\n<div class='advance_name'>".sprintf($tbl_role_equip_info[2],$advance_name)."</div>\n<div class='advance_value'>".$advance_cur_tiems."/".$advance_tot_tiems."</div>\n<div class='advance_times'>".sprintf($tbl_role_equip_info[5],$row[10])."</div>\n</div>\n";
		if($row[11]!=null)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[16]))."</div>\n<div class='advance_value'>+".$row[11]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[11]/100).")</div>\n")."</div>\n";
		if($row[12]!=null)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[17]))."</div>\n<div class='advance_value'>+".$row[12]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[12]/100).")</div>\n")."</div>\n";
		if($row[13]!=null)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[18]))."</div>\n<div class='advance_value'>+".$row[13]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[13]/100).")</div>\n")."</div>\n";
		if($row[14]!=null)
			echo "<div class='advance_item'>\n<div class='advance_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[19]))."</div>\n<div class='advance_value'>+".$row[14]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[14]/100).")</div>\n")."</div>\n";
		$JingLingType = Local2UTF8($row[8]);
		$JingLingSkill = $row[7];
		echo "</div>\n";
	}
	
	global $tbl_item_inten_soul;
	
	$qs = "select * from tbl_item_equip_intensifyAddAttr where is_uId = $item_id";
	$result = MySQLQuery($qs, "Game", $serverid);
	if($row = mysql_fetch_row($result)){
		if($row[1])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[1]))."</div>\n<div class='intensify_value'>+".$row[2]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[2]/100).")</div>\n")."</div>\n";
		if($row[3])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[3]))."</div>\n<div class='intensify_value'>+".$row[4]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[4]/100).")</div>\n")."</div>\n";
		if($row[5])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[5]))."</div>\n<div class='intensify_value'>+".$row[6]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[6]/100).")</div>\n")."</div>\n";
		if($row[7])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[7]))."</div>\n<div class='intensify_value'>+".$row[8]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[8]/100).")</div>\n")."</div>\n";
		if($row[9])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[9]))."</div>\n<div class='intensify_value'>+".$row[10]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[10]/100).")</div>\n")."</div>\n";
		if($row[11])
			$inten_info.= "<div class='intensify_item'>\n<div class='intensify_name'>".Lan_Equip_GetPropertyName(Local2UTF8($row[11]))."</div>\n<div class='intensify_value'>+".$row[12]."</div>\n".(($addation==0)?"":"<div class='addation'>(+".ceil($addation_rate*$row[12]/100).")</div>\n")."</div>\n";
	}
	
	echo "<div class='intensify'>\n";
	echo "<div class='intensify_item'>\n";
	if($intensify>0)
		echo "<div class='intensify_name'>".sprintf($tbl_role_equip_info[3],$tbl_item_inten_soul[Local2Base64($inten)])."</div>\n<div class='intensify_value'>$inten_times/$intensify</div>\n";
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
	
	echo "</div>";
	return true;
}

function GetRoleItems($var, $gm)
{
	global $tbl_role_all_detail;
	global $tbl_sys_msg;
	
	$strjs = <<<_strjs
<script type="text/javascript">
function OnDetail(id){
	document.form1.action_name.value='141';
	document.form1.item_id.value=id;
	document.form1.submit();
}
function DelItemInBag(item_name,max_num){
	document.form1.action_name.value='56';
	document.form1.item_name.value = item_name;
	var num = prompt("$tbl_role_all_detail[8]","0");
	if(num==null) return;
	if(num>max_num || num<0){alert("$tbl_role_all_detail[9]");return;}
	document.form1.num.value = num;
	var reason = prompt("$tbl_role_all_detail[7]","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
function DelItemOnBody(item_name){
	document.form1.action_name.value='57';
	document.form1.item_name.value = item_name;
	var reason = prompt("$tbl_role_all_detail[7]","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
function DelItemInDepot(item_name,max_num){
	document.form1.action_name.value='78';
	document.form1.item_name.value = item_name;
	var num = prompt("$tbl_role_all_detail[8]","0");
	if(num==null) return;
	if(num>max_num || num<0){alert("$tbl_role_all_detail[9]");return;}
	document.form1.num.value = num;
	var reason = prompt("$tbl_role_all_detail[7]","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
function ClearItemInBag(){
	document.form1.action_name.value = "191";
	var reason = prompt("$tbl_role_all_detail[7]","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
function ClearItemInDepot(){
	document.form1.action_name.value = "192";
	var reason = prompt("$tbl_role_all_detail[7]","");
	if(reason==null) return;
	document.form1.reason.value = reason;
	document.form1.submit();
}
</script>
_strjs;

	$serverid=$var["serverid"];
	
	echo $strjs."\n";
	echo "<h1>$tbl_role_all_detail[15]</h1>\n";
	ReturnAndBack();
	Reload();
	
	$role_name = ArgRoleName();
	if($role_id_row = mysql_fetch_row(MySQLQuery("select cs_uId,1 from tbl_char where c_sName='".$role_name."' union select cs_uId,2 from tbl_char_deleted where cd_sName='".$role_name."'","Game",$serverid))){
		$role_id = $role_id_row[0];
		$b = ($role_id_row[1]==1);
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return true;
	}
	
	echo sprintf("<table class='list_table'><tr><td>$tbl_role_all_detail[17]</td></tr></table>\n", $role_name."(".$role_id.")");
	
	form($var,$gm);
	echo "<input type='hidden' name='item_name'/>";
	echo "<input type='hidden' name='item_id'/>";
	echo "<input type='hidden' name='num'/>";
	echo "<input type='hidden' name='str'/>";
	echo "<input type='hidden' name='reason'/>";
	echo "<input type='hidden' name='role_name' value='".$role_name."'/>";
	echo "<input type='hidden' name='role_id' value='".$role_id."'/>";
	
	echo "<h2>$tbl_role_all_detail[3]</h2>\n". GetRoleItem($role_id,$b,$serverid);
	echo "<h2>$tbl_role_all_detail[4]</h2>\n". GetRoleEquip($role_id,$b,$serverid);
	echo "<h2>$tbl_role_all_detail[5]</h2>\n". GetRoleStorage($role_id,$b,$serverid);
	echo "<h2>$tbl_role_all_detail[13]</h2>\n". GetRoleMailItem($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[14]</h2>\n". GetRoleMarketItem($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[22]</h2>\n". GetRoleStone($role_id,$serverid);

	echo sprintf("</form>");
	return true;
}

function GetRoleAllDetail($var, $gm)
{
	global $tbl_role_all_detail,$tbl_sys_msg;
	
	$serverid=$var["serverid"];
	
	$js_str = <<<_js_str
<script type="text/javascript">
function toItem(){
	document.form1.action_name.value = "201";
	document.form1.submit();
}
function toFriend(){
	document.form1.action_name.value = "199";
	document.form1.submit();
}
function toRealTime(){
	document.form1.action_name.value = "204";
	document.form1.submit();
}
</script>
_js_str;
	
	echo $js_str."\n";
	echo "<h1>$tbl_role_all_detail[10]</h1>\n";
	ReturnAndBack();
	Reload();
	form($var,$gm);
	$role_id = ArgRoleId();
	echo "<input type='hidden' name='role_str' value='".$var["role_str"]."'/>";
	echo "<input type='hidden' name='role_item' value='".$var["role_item"]."'/>";
	echo "<table class='list_table'>\n";
	echo sprintf("<tr><td>$tbl_role_all_detail[0]</td></tr>\n", ArgRoleName()."(".$role_id.")");
	echo "<tr><td><a href='#' onclick='toItem()'>$tbl_role_all_detail[15]</a></td></tr>\n";
	echo "<tr><td><a href='#' onclick='toFriend()'>$tbl_role_all_detail[16]</a></td></tr>\n";
	echo "</table>\n";
	echo "</form>\n";
	
	echo "<h2>$tbl_role_all_detail[1]</h2>\n". GetRoleDetail($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[19]</h2>\n". GetRoleLogin($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[2]</h2>\n". GetRoleLocation($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[11]</h2>\n".GetRoleSort($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[12]</h2>\n".GetRoleAppellation($role_id,$serverid);
	echo "<h2>$tbl_role_all_detail[6]</h2>\n". GetRoleSkill($role_id,$serverid);
	return true;
}

function RoleRename($var, $gm){
	global $tbl_game_server;
	
	$serverid = $var["serverid"];
	$role_name = $var["role_name"];
	$new_name = $var["new_name"];
	$reason = $var["reason"];
	
	$param = array();
	$param["role_name"] = $role_name;
	$param["new_name"] = $new_name;
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "RoleRename", $param);
	
	$action = array("role_name" => $role_name, "role_name" => $new_name, "reason" => $reason);
	
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}

function DelChar($var, $gm){
	global $tbl_game_server;
	
	$serverid = $var["serverid"];
	$role_name = $var["role_name"];
	$role_id = $var["role_id"];
	$reason = $var["reason"];
	
	$param = array();
	$param["role_id"]   = $role_id;
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "DelChar", $param);
	
	$action = array("role_name" => $role_name, "reason" => $reason);
	
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}

function GetBackRole($var, $gm){
	global $tbl_game_server;
	
	$serverid = $var["serverid"];
	$role_name = $var["role_name"];
	$role_id = $var["role_id"];
	$reason = $var["reason"];
	
	$param = array();
	$param["role_id"]   = $role_id;
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "GetBackRole", $param);
	
	$action = array("role_name" => $role_name, "reason" => $reason);
	
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}
?>
