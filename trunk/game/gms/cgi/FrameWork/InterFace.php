<?php
require_once("../../conf/gm_tools/Config.php");
require("../../lib/gm_tools/Lib.php");
global $sys_config;
require_once("../../lang/gm_tools/".$sys_config["lang"]."/page_localized.php");
require_once("../../lang/gm_tools/".$sys_config["lang"]."/sys_config.php");
global $page_main,$tbl_event_type,$tbl_camp,$tbl_jifen,$tbl_money_binding,$tbl_hot_update_type,$tbl_tong_page,$tbl_equip_upgrade_type,$tbl_tong_msg,$tbl_server_type,$tbl_recruit,$tbl_commerce_skill,$tbl_tong_boss,$tbl_modulus_type;
/*
$action_page = array(
	"RoleAction"				=>	array(184,103,200,176,167,62,76,180,182,"182.1",183,194,206,112,113,114,116,117,118,149,202,151,203,43,90,44,91,45,92,207,198)
);
*/
$action_page = array(
	"UserInfo"					=> array(50,51),
	"RoleInfo"					=> array(193,97,199,201,99,101,100,238,141),
	"RoleAction"				=> array(28,26,27,29,30,61,23,158,288,289),
	"Bulletin"					=> array(81,82,251),
	"RoleCompensateReq"	=> array(33,34,196,35,36,37,38,39,40,147,41,197,42),
	"TongMgr"						=> array(63,"63.1",175,283,261,262,263,264,265,258,278,279,287),
	"DealRoleCompensate"=> array(53),
	"GMGetCmpstBy"			=> array(123),
	"PlayerComplaint"		=> array(58),
	"GameWorld"					=> array(88,24,104,21,22),
	"LogInfo"						=> array(70,206,235,236,237,243,281,182,255,247,256,286,290,234,242,253),
	"MoneyLog"					=> array(221,222,229),
	"Other"							=> array(184,200,167,62,76,194,149,202,151,203,43,90,44,91,45,92,207,198),
	"ServerManagement"	=> array(154,156,260,"260.1",252,254,285,257,259,"259.1","259.2",284,266,272,282,291),
	"Talk"							=> array(159,165,164,161,280),
	"IPMgr"							=> array(52,239,240,276,277),
	"EquipBack"					=> array(205),
	"CfgSearch"					=> array(244,245,246,46),
	"Recruit"						=> array(275,273,"273.1",274),
	"InGameShop"				=> array(269),
);


$sub_action = array(
	"*"	=> array(105,120,121,122,241),
	53	=> array(54,55),
	201	=> array(56,57,78,79,141,191,192),
	63	=> array(64,65,73,74,169,170,171,172,175,178),
	71	=> array(72),
	76	=> array(77),
	81	=> array(83,84,125),
	88	=> array(89,96),
	90	=> array(93),
	91	=> array(94),
	92	=> array(95),
	123	=> array(124),
	202	=> array(150),
	203	=> array(152),
	156	=> array(155),
	158	=> array(160),
	161	=> array(162,163),
	205	=> array(166),
	50	=> array(179,186,187),
	194	=> array(195),
	97	=> array(204),
	207	=> array(208),
	165	=> array(209),
	164	=> array(210),
	260 => array("260.2"),
	269 => array(267,268,270,271),
	283	=> array(248,249,250),
	284	=> array("284.1","284.2","284.3"),
);

function ServerSelect(){
	$gameId = $_REQUEST["gameId"];
	$str = "<select name=serverid>\n";
	global $tbl_serv_partition,$tbl_game_server;
	$groups = $tbl_serv_partition[$gameId];
	if(is_array($groups)){
		foreach($groups as $group_name => $group){
			if(is_array($group)){
				$str.= sprintf("<optgroup label='%s'>\n", $group_name);
				foreach($group as $serverid){
					$str.= sprintf("<option value='%d'>%s</option>\n", $serverid, $tbl_game_server[$serverid][2]);
				}
				$str.= "</optgroup>\n";
			}
		}
	}
	$str.="</select>";
	return $str;
}

function ChannelList($action_id){
	$str = "";
	global $tbl_channel;
	$str.= "<input type=\"checkbox\" id=\"Channel".$action_id."_all\" class=\"checkbox_all\" name=\"channel_id[]\" onclick=\"checkboxAllSelect(this)\" value=\"0\" />".$tbl_channel[0]."<br/>\n";
	foreach($tbl_channel as $i => $c){
		if($i != 0) $str.= "<input type=\"checkbox\" class=\"Channel".$action_id."_checkbox\" name=\"channel_id[]\" onclick=\"checkboxAllSelect(this)\" value=\"".$i."\" />".$c."<br/>\n";
	}
	return $str;
}

function RoleList(){
	$str = "<textarea name='role_name' style=\"width:400px; height:100px;\">";
	$roles_array = $_REQUEST["role_names"];
	$roles_str = $_REQUEST["role_str"];
	if($roles_array != null && count($roles_array)>0) {
		for($i=0;$i<count($roles_array);$i++){
			$roles_str.=$roles_array[$i].",";
		}
	}
	$str.= str_replace("\n",",",$roles_str);
	$str.= "</textarea>";
	return $str;
}

function MoneyLimit(){
	global $page_of_compensate;
	$str = "<select name=\"money_func\">\n";
	$str.= "<option value='NpcShop".				"+Sell".					"'>".$page_of_compensate[0] ."</option>\n"; 
	$str.= "<option value='NpcShop".				"+Buy".						"'>".$page_of_compensate[1] ."</option>\n";
	$str.= "<option value='NpcShop".				"+BuyBack".				"'>".$page_of_compensate[2] ."</option>\n";
	$str.= "<option value='CSM".						"+AddSellOrder".	"'>".$page_of_compensate[3] ."</option>\n";
	$str.= "<option value='CSM".						"+BuyOrder".			"'>".$page_of_compensate[4] ."</option>\n";
	$str.= "<option value='Quest". 					"+MoneyAward".		"'>".$page_of_compensate[5] ."</option>\n";
	$str.= "<option value='Quest". 					"+Drink".					"'>".$page_of_compensate[6] ."</option>\n";
	$str.= "<option value='GMcmd". 					"+AddMoney".			"'>".$page_of_compensate[7]."</option>\n";
	$str.= "<option value='GmTools". 				"+AddMoney".			"'>".$page_of_compensate[8]."</option>\n";
	$str.= "<option value='GmTools". 				"+AddTicket".			"'>".$page_of_compensate[9]."</option>\n";
	$str.= "<option value='Depot". 					"+SaveMoney".			"'>".$page_of_compensate[10]."</option>\n";
	$str.= "<option value='Depot". 					"+GetMoney".			"'>".$page_of_compensate[11]."</option>\n";
	$str.= "<option value='Mail". 					"+GetMoney".			"'>".$page_of_compensate[12]."</option>\n";
	$str.= "<option value='Mail". 					"+SendMail".			"'>".$page_of_compensate[13]."</option>\n";
	$str.= "<option value='Tong". 					"+RegistTong".		"'>".$page_of_compensate[14]."</option>\n";
	$str.= "<option value='Tong". 					"+Contribut".			"'>".$page_of_compensate[15]."</option>\n";
	$str.= "<option value='fuben". 					"+Daduobao".			"'>".$page_of_compensate[16]."</option>\n";
	$str.= "<option value='PlayerTran". 		"+PlayerTran".		"'>".$page_of_compensate[17]."</option>\n";
	$str.= "<option value='Trap". 					"+TrapAddMoney".	"'>".$page_of_compensate[18]."</option>\n";
	$str.= "<option value='LearnSkill".			"+LearnSkill".		"'>".$page_of_compensate[19]."</option>\n";
	$str.= "<option value='EquipDurability"."+RenewEquip".		"'>".$page_of_compensate[20]."</option>\n";
	$str.= "<option value='ArmyCorps".		"+RegistArmyCorps".	"'>".$page_of_compensate[21]."</option>\n";
	$str.= "<option value='LiveSkill".		"+LearnSkill".			"'>".$page_of_compensate[22]."</option>\n";
	$str.= "<option value='Afflatus".			"+ExChgMoney".			"'>".$page_of_compensate[23]."</option>\n";
	$str.= "<option value='Stone".				"+SynthesisExpend".	"'>".$page_of_compensate[24]."</option>\n";
	$str.= "<option value='Chat".				"+WorldChat".					"'>".$page_of_compensate[25]."</option>\n";
	$str.= "<option value='ContractManufacture". 	"+AddContractOrder".	"'>".$page_of_compensate[26]."</option>\n";
	$str.= "<option value='RobResourceAward". 		"+HeaderAward".				"'>".$page_of_compensate[27]."</option>\n";

	$str.= "</select>";
	return $str;
}

function JifenLimit(){
	//global $sys_config;
	//include_once("../../lang/gm_tools/".$sys_config["lang"]."/sys_config.php");
	global $tbl_jifen;
	$str = "<select name=\"jifen_type\">\n";
	for ($i=1; $i < count($tbl_jifen); ++$i){
		$str.= sprintf("<option value='%d'>%s</option>\n", $i, $tbl_jifen[$i]);
	}
	$str.= "</select>";
	return $str;
}

function EventType(){
	global $tbl_event_type;
	$str = "<select name=\"event_type\">\n";
	foreach($tbl_event_type as $id => $name){
		$str.= sprintf("<option value='%d'>%s</option>\n", $id, $name);
	}
	$str.= "</select>\n";
	return $str;
}

$server_list 	= array($page_main["select_server"],	ServerList());
$server_item	= array($page_main["select_server"],	ServerSelect());
$reason 			= array($tbl_sys_msg["reason"],				"<textarea name='reason' rows='10' cols='30' ></textarea>");
$begin_time		= array($page_main["begin_time"],			"<input type='text' onclick=\"SelectDate(this,'yyyy-MM-dd hh:mm')\" name='begin_time' />");
$end_time			= array($page_main["end_time"],				"<input type='text' onclick=\"SelectDate(this,'yyyy-MM-dd hh:mm')\" name='end_time' />");
$item_search	= array($page_main["item"],						"<input type='text' class='str' name='item_name' /><input type='button' onclick='search(120,this,\"item\")' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>");
$role_item		= array($page_main["role_name"],			"<input type='radio' name='role_item' value='name' checked='checked' />".$page_main["name"]."<input type='radio' name='role_item' value='id' />ID<br /><input type='text' name='role_str' />");
$user_item		= array($page_main["user_name"],			"<input type='text' name='urs'/>");
$tong_item		= array($page_main["tong"],						"<input type='text' name='tong_name' />");

$action_form = array(
	"21" 	=>	array($server_list,
                  array($page_main["range"],					$page_main["min"]."<input type='text' name='min_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='min_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='min_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]."<br/>".$page_main["max"]."<input type='text' name='max_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='max_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='max_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"])),
	"22" 	=>	array($server_list,
                  array($page_main["range"],					$page_main["min"]."<input name='MinNum' type='input' />".$page_main["max"]."<input name='MaxNum' type='input' />")),
	"23" 	=>	array($server_list,
                  array($page_main["channel"],				ChannelList(23))),
	"24" 	=>	array($server_list,
                  array($page_main["scene"],					"<input class='str' type='text' /><input type='button' onclick='search(122,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div><input type='hidden' name='scene_name_' />")),
	"26"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["channel"],				ChannelList(26)),
                  $end_time,
                  $reason),
	"27"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["channel"],				ChannelList(27)),
                  $reason),
	"28"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  $reason),
	"29"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["scene"],					"<input class=\"str\" type=\"input\" /><input type=\"button\" onclick='search(122,this,\"scene\")' value='".$tbl_sys_msg["search"]."'/><div class='pop'></div>"),
                  array($page_main["x"],							"<input name='x_pos' type='input' />"),
                  array($page_main["y"],							"<input name='y_pos' type='input' />"),
                  $reason),
	"30"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["content"],				"<textarea name='warning' rows='10' cols='30'></textarea>"),
                  $reason),
	"33" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["exp"],						"<input type='text' name='num' />"),
                  $reason),
	"34" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["money"],					"<input type='text' name='money_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='money_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='money_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]),
                  $reason),
	"35" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["ticket"],					"<input type='text' name='num' />"),
                  $reason),
	"36" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["task"],						"<input type='text' class='str' /><input type='button' onclick='search(121,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $reason),
	"37" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["task"],						"<input type='text' class='str' /><input type='button' onclick='search(121,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $reason),
	"38" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  $item_search,
                  array($page_main["number"],					"<input type='text' name='num' />"),
                  $reason),
	"39" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  $item_search,
                  array($page_main["number"],					"<input type='text' name='num' />"),
                  $reason),
	"40" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["task"],						"<input type='text' class='str' /><input type='button' onclick='search(121,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $reason),
	"41" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["soul"],						"<input type='text' name='num' />"),
                  $reason),
	"42" 	=>	array($server_list,
                  array($page_main["user_name"],			"<input type='text' name='user_name' />"),
                  array($page_main["level"],					"<input type='text' name='gm_level' />"),
                  array("GameID",											"<input type='text' name='game_id' />"),
                  $reason),
	"43" 	=>	array($server_list,
                  array($page_main["function"],				MoneyLimit()),
                  $reason),
	"44" 	=>	array($server_list,
                  array($page_main["jifen"],					JifenLimit()),
                  $reason),
	"45" 	=>	array($server_list,
                  $item_search,
                  $reason),
	"46"	=>	array(array($page_main["npc_name"],				"<input type='text' class='str'/><input type='button' onclick='search(105,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>")),
	"50"	=>	array($user_item),
	"51"	=>	array($user_item,
                  $begin_time,
                  $end_time),
	"52"	=>	array(array($page_main["ip"],							"<input type='text' name='ip'/>"),
                  $begin_time,
                  $end_time),
	"53"	=>	array($server_list),
	"58"	=>	array($server_list),
	"61" 	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["state"],					"<input type='text' name='state_name'/>")),
	"62"	=>	array(),
	"63" 	=>	array($server_list),
	"63.1"=>	array($server_list,
                  array($page_main["tong"],						"<input type='text' name='str' />")),
	"70" 	=>	array($server_item,
                  $role_item,
                  array($page_main["event_type"],			"<select name='event_type'><option value='0'>".$tbl_event_type[0]."</option><option value='1'>".$tbl_event_type[1]."</option><option value='2'>".$tbl_event_type[2]."</option><option value='42'>".$tbl_event_type[42]."</option><option value='153'>".$tbl_event_type[153]."</option></select>"),
                  $item_search,
                  $begin_time,
                  $end_time),
	"71" 	=>	array($server_list,
                  $item_search,
                  $begin_time,
                  $end_time),
	"75" 	=>	array($server_list,
                  array($page_main["npc_name"],				"<input type='text' class='str' /><input type='button' onclick='search(105,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $item_search,
                  $begin_time,
                  $end_time),
	"76" 	=>	array($server_list,
                  array($page_main["scene"],					"<input type='hidden' name='scene_real_name'/><input type='text' class='str' /><input type='button' onclick='search(122,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $item_search,
                  $begin_time,
                  $end_time),
	"81"	=>	array($server_list),
	"82"	=>	array($server_list,
                  $begin_time,
                  $end_time,
                  array($page_main["step"],						"<select name='step'><option value='1'>1</option><option value='2'>2</option><option value='5'>5</option><option value='10'>10</option><option value='15'>15</option><option value='30'>30</option><option value='60'>60</option></select>".$page_main["step_ps"]),
                  array($page_main["title"],					"<input type='text' name='title'/>"),
                  array($page_main["content"],				"<textarea name='content'></textarea>")),
	"88" 	=>	array($server_list),
	"90" 	=>	array($server_list),
	"91" 	=>	array($server_list),
	"92" 	=>	array($server_list),
	"97"	=> 	array($server_item,
                  $role_item),
	"99"	=> 	array($server_item,
                  $role_item,
                  array($tbl_sys_msg["rank"],					"<input type='radio' name='rank' value='q_tAcceptTime' id='rank_99_1' checked='checked'/><label for='rank_99_1'>".$page_main["begin_time"]."</label><input type='radio' name='rank' value='q_tFinishTime' id='rank_99_2'/><label for='rank_99_2'>".$page_main["end_time"]."</label>"),
                  $begin_time,
                  $end_time),
	"100"	=> 	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"101"	=> 	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"104"	=>	array($server_list,
                  array($page_main["range"],					$page_main["min"]."<input type='text' name='min_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='min_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='min_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]."<br/>".$page_main["max"]."<input type='text' name='max_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='max_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='max_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"])),
	"112"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"113"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"114"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"116"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"117"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"118"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"123"	=>	array($server_list),
	"141"	=>	array($server_item,
                  array($page_main["item_id"],				"<input type='text' name='item_id' />")),
	"147"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["content"],				"<textarea name='content'></textarea>"),
                  $item_search,
                  array($page_main["number"],					"<input type='text' name='num' /><input type='button' onclick='additem(this)' value='".$tbl_sys_msg["insert"]."'/><script>function additem(obj){if($(obj).parents(\"table\").find(\"[name='item_name']\").val()){var name = $(obj).parents(\"table\").find(\"[name='item_name']\").val();var num  = $(obj).parents(\"table\").find(\"[name='num']\").val();document.form1.item.value += name + \"_\" + num + \";\";}else{alert(\"".$page_main["item"]."\");}}</script>"),
                  array($page_main["list"],						"<textarea name='item'></textarea><br/>".$page_main["list_ps"]),
                  array($page_main["money"],					"<input type='text' name='money_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='money_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='money_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]),
                  $reason),
	"149"	=>	array($server_list,
                  array($page_main["task"],						"<input type='text' class='str' /><input type='button' onclick='search(121,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $reason),
	"151"	=>	array($server_list,
                  $item_search,
                  $reason),
	"154"	=>	array($server_list,
                  array($page_main["category"],				"<input type='radio' name='type' value=1 checked='checked'/>".$tbl_hot_update_type[1]."<input type='radio' name='type' value=2/>".$tbl_hot_update_type[2]."<input type='radio' name='type' value=3/>".$tbl_hot_update_type[3]),
                  array($page_main["file"],						"<input type='text' name='file' style='width:400px;' /></textarea>"),
                  array($page_main["code"],						"<textarea name='code' style=\"height:250px;\"></textarea>")),
	"156"	=>	array($server_list),
	"158"	=>	array($server_item,
                  $role_item),
	"159"	=>	array($server_list),
	"161"	=>	array($server_list),
	"164"	=>	array($server_list),
	"165"	=>	array($server_list),
	"167"	=>	array($server_list,
                  $role_item,
                  $begin_time,
                  $end_time),
	"173"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"175"	=> 	array($server_item,
                  array($page_main["tong"],						"<textarea name='tong_name' rows='10' cols='30'></textarea>"),
                  $item_search,
                  array($page_main["type"],						"<select name='uPage'><option value='1'>".$tbl_tong_page[1]."</option><option value='2'>".$tbl_tong_page[2]."</option><option value='3'>".$tbl_tong_page[3]."</option><option value='4'>".$tbl_tong_page[4]."</option><option value='5'>".$tbl_tong_page[5]."</option></select>"),
                  array($page_main["number"],					"<input type='text' name='num' />"),
                  $reason),
	"176"	=>	array($server_list),
	"177"	=>	array($server_list,
                  array($page_main["npc_name"],				"<input type='text' class='str' /><input type='button' onclick='search(105,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>"),
                  $begin_time,
                  $end_time),
	"180"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"182"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"184"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList())),
	"193"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList())),
	"194"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"196"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["money"],					"<input type='text' name='money_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='money_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='money_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]),
                  $reason),
	"197"	=>	array($server_list,
                  array($page_main["role_name"],			RoleList()),
                  array($page_main["soul"],						"<input type='text' name='num' />"),
                  $reason),
	"198"	=>	array($server_list),
	"199"	=> 	array($server_item,
                  $role_item),
	"200"	=>	array($server_list),
	"201"	=> 	array($server_item,
                  $role_item),
	"202"	=>	array($server_list),
	"203"	=>	array($server_list),
	"205"	=> 	array($server_item,
                  array($page_main["item_id"],				"<input type='text' name='item_id' />")),
	"206"	=>	array($server_item,
                  $role_item,
                  array($page_main["event_type"],			EventType()),
                  array($page_main["money_type"],			"<select name='money_type'><option value='0'>".$tbl_money_binding[0]."</option><option value='1'>".$tbl_money_binding[1]."</option><option value='2'>".$tbl_money_binding[2]."</option></select>"),
                  $begin_time,
                  $end_time),
	"207"	=>	array(array($page_main["gm_user"],				"<input type='text' name='user'>")),
	"221"	=>	array(array($page_main["day_step"][0],		"<select name='day'><option value='1'>1".$page_main["day_step"][1]."</option><option value='2'>2".$page_main["day_step"][1]."</option><option value='3'>3".$page_main["day_step"][1]."</option><option value='7'>1".$page_main["day_step"][2]."</option><option value='14'>2".$page_main["day_step"][2]."</option><option value='15'>0.5".$page_main["day_step"][3]."</option><option value='30'>1".$page_main["day_step"][3]."</option><option value='60'>2".$page_main["day_step"][3]."</option><option value='90'>3".$page_main["day_step"][3]."</option><option value='180'>6".$page_main["day_step"][3]."</option></select>"),
                  $server_item,
                  $role_item),
	"222"	=>	array(array($page_main["day_step"][0],		"<select name='day'><option value='1'>1".$page_main["day_step"][1]."</option><option value='2'>2".$page_main["day_step"][1]."</option><option value='3'>3".$page_main["day_step"][1]."</option><option value='7'>1".$page_main["day_step"][2]."</option><option value='14'>2".$page_main["day_step"][2]."</option><option value='15'>0.5".$page_main["day_step"][3]."</option><option value='30'>1".$page_main["day_step"][3]."</option><option value='60'>2".$page_main["day_step"][3]."</option><option value='90'>3".$page_main["day_step"][3]."</option><option value='180'>6".$page_main["day_step"][3]."</option></select>"),
                  $server_item,
                  $role_item),
	"229"	=>	array(array($page_main["day_step"][0],		"<select name='day'><option value='1'>1".$page_main["day_step"][1]."</option><option value='2'>2".$page_main["day_step"][1]."</option><option value='3'>3".$page_main["day_step"][1]."</option><option value='7'>1".$page_main["day_step"][2]."</option><option value='14'>2".$page_main["day_step"][2]."</option><option value='15'>0.5".$page_main["day_step"][3]."</option><option value='30'>1".$page_main["day_step"][3]."</option><option value='60'>2".$page_main["day_step"][3]."</option><option value='90'>3".$page_main["day_step"][3]."</option><option value='180'>6".$page_main["day_step"][3]."</option></select>"),
                  $server_item,
                  $role_item),
	"234" =>	array($server_item,
                  array($page_main["item_id"],				"<input type='text' name='item_id'/>"),
                  $begin_time,
                  $end_time),
	"235" =>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"236" =>	array($server_item,
                  $role_item,
                  array($page_main["event_type"],			"<select name='event_type'><option value='0'>".$tbl_event_type[0]."</option><option value='1'>".$tbl_event_type[1]."</option><option value='2'>".$tbl_event_type[2]."</option><option value='42'>".$tbl_event_type[42]."</option><option value='153'>".$tbl_event_type[153]."</option></select>"),
                  $begin_time,
                  $end_time),
	"237" =>	array($server_item,
                  $role_item,
                  array($page_main["event_type"],			"<select name='event_type'><option value='0'>".$tbl_event_type[0]."</option><option value='1'>".$tbl_event_type[1]."</option><option value='2'>".$tbl_event_type[2]."</option><option value='42'>".$tbl_event_type[42]."</option><option value='153'>".$tbl_event_type[153]."</option></select>"),
                  array($page_main["jifen"],					"<select name='jifen_type'><option value='0'>".$tbl_event_type[0]."</option><option value='1'>".$tbl_jifen[1]."</option><option value='2'>".$tbl_jifen[2]."</option><option value='3'>".$tbl_jifen[3]."</option><option value='4'>".$tbl_jifen[4]."</option><option value='5'>".$tbl_jifen[5]."</option><option value='6'>".$tbl_jifen[6]."</option><option value='7'>".$tbl_jifen[7]."</option><option value='8'>".$tbl_jifen[8]."</option><option value='9'>".$tbl_jifen[9]."</option></select>"),
                  $begin_time,
                  $end_time),
	"238"	=> 	array($server_item,
                  $role_item,
                  array($page_main["task"],						"<input type='text' class='str' /><input type='button' onclick='search(121,this)' value=\"".$tbl_sys_msg["search"]."\"/><div class=\"pop\"></div>")),
	"239"	=>	array($server_list),
	"240"	=>	array($server_list,
                  array($page_main["ip"],							"<input type='text' name='ip' />")),
	"242"	=> 	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"243"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"244"	=>	array(array($tbl_sys_msg["EnterType"],		"<input type='radio' name='type' value='display_name' id='224_1' /><label for='224_1'>".$page_main["display_name"]."</label><input type='radio' name='type' value='real_name' id='224_2' /><label for='224_2'>".$page_main["real_name"]."</label><input type='radio' name='type' value='base64' id='224_3' /><label for='224_3'>Base64</label>"),
                  array($page_main["item"],						"<input type='text' name='name' />")),
	"245"	=>	array(array($tbl_sys_msg["EnterType"],		"<input type='radio' name='type' value='display_name' id='224_1' /><label for='224_1'>".$page_main["display_name"]."</label><input type='radio' name='type' value='real_name' id='224_2' /><label for='224_2'>".$page_main["real_name"]."</label><input type='radio' name='type' value='base64' id='224_3' /><label for='224_3'>Base64</label>"),
                  array($page_main["task"],						"<input type='text' name='name' />")),
	"246"	=>	array(array($tbl_sys_msg["EnterType"],		"<input type='radio' name='type' value='display_name' id='224_1' /><label for='224_1'>".$page_main["display_name"]."</label><input type='radio' name='type' value='real_name' id='224_2' /><label for='224_2'>".$page_main["real_name"]."</label><input type='radio' name='type' value='base64' id='224_3' /><label for='224_3'>Base64</label>"),
                  array($page_main["npc_name"],				"<input type='text' name='name' />")),
	"247"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"248"	=>	array($server_item,
                  array($page_main["tong"],						"<textarea name='tong_name' rows='10' cols='30'></textarea>"),
                  array($page_main["money"],					"<input type='text' name='money_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='money_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='money_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]),
                  $reason),
	"249"	=>	array($server_item,
                  array($page_main["tong"],						"<textarea name='tong_name' rows='10' cols='30'></textarea>"),
                  array($page_main["number"],					"<input type='text' name='num_resource' />"),
                  $reason),
	"250"	=>	array($server_item,
                  array($page_main["tong"],						"<textarea name='tong_name' rows='10' cols='30'></textarea>"),
                  array($page_main["number"],					"<input type='text' name='num_honor' />"),
                  $reason),
	"251"	=>	array(array($page_main["title"],					"<input type='text' name='title' />"),
                  array($page_main["content"],				"<textarea name='content'></textarea>")),
	"253"	=>	array($server_item,
                  array($page_main["item_id"],				"<input type='text' name='item_id' />"),
                  array($page_main["type"],						"<input type='checkbox' id='253_all' class='checkbox_all' /><label for='253_all'>".$tbl_sys_msg["all"]."</label><br/><input type='checkbox' id='253_armor' class='253_checkbox' name='type[]' value='armor' /><label for='253_armor'>".$tbl_equip_upgrade_type["armor"][0]."</label><br/><input type='checkbox' id='253_advance' class='253_checkbox' name='type[]' value='advance' /><label for='253_advance'>".$tbl_equip_upgrade_type["advance"][0]."</label><br/><input type='checkbox' id='253_intensify' class='253_checkbox' name='type[]' value='intensify' /><label for='253_intensify'>".$tbl_equip_upgrade_type["intensify"][0]."</label><br/><input type='checkbox' id='253_intensifyAddAttr' class='253_checkbox' name='type[]' value='intensifyAddAttr' /><label for='253_intensifyAddAttr'>".$tbl_equip_upgrade_type["intensifyAddAttr"][0]."</label><br/><input type='checkbox' id='253_superaddation' class='253_checkbox' name='type[]' value='superaddation' /><label for='253_superaddation'>".$tbl_equip_upgrade_type["superaddation"][0]."</label>"),
                  $begin_time,
                  $end_time),
	"255"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"256"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"257"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"258"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"259"	=>	array($server_item),
	"259.1"=>	array($server_item,
									array("",														"<input type='hidden' name='flag' value='delete' />")),
	"260.1"=>	array($server_list,
                  array($page_main["end_time"],				"<input type='text' onclick=\"SelectDate(this,'yyyy-MM-dd')\" name='date' />")),
	"261"	=>	array($server_item,
                  $tong_item,
                  array($page_main["type"],						"<input type='checkbox' name='type[]' id='A261T1' value='honor' /><label for='A261T1'>".$tbl_tong_msg["honor"]."</label><input type='checkbox' name='type[]' id='A261T2' value='level' /><label for='A261T2'>".$page_main["level"]."</label><input type='checkbox' name='type[]' id='A261T3' value='money' /><label for='A261T3'>".$tbl_tong_msg["money"]."</label><input type='checkbox' name='type[]' id='A261T4' value='resource' /><label for='A261T4'>".$tbl_tong_msg["resource"]."</label>"),
                  $begin_time,
                  $end_time),
	"262"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"263"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"264"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"265"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"266"	=>	array($server_list,
                  array($page_main["type"],						"<select name='type'>\n<option value='0'>".$tbl_server_type[0]."</option>\n<option value='1'>".$tbl_server_type[1]."</option>\n<option value='2'>".$tbl_server_type[2]."</option>\n<option value='3'>".$tbl_server_type[3]."</option>\n</select>")),
	"273"	=>	array($server_list,
                  array($tbl_sys_msg["on"]."/".$tbl_sys_msg["off"],	"<input type='radio' name='method' value='1' id='method_273_1' /><label for='method_273_1'>".$tbl_sys_msg["on"]."</label><input type='radio' name='method' value='0' id='method_273_2' checked='checked' /><label for='method_273_2'>".$tbl_sys_msg["off"]."</label>")),
	"273.1"=>	array($server_list,
                  array($tbl_camp[1],                  "<input type='radio' name='camp1' value='0' id='method_273_1_0' checked='checked' /><label for='method_273_1_0'>".$tbl_sys_msg["off"]."</label><input type='radio' name='camp1' value='1' id='method_273_1_1' /><label for='method_273_1_1'>".$tbl_recruit[0]."</label><input type='radio' name='camp1' value='2' id='method_273_1_2' /><label for='method_273_1_2'>".$tbl_recruit[1]."</label>"),
                  array($tbl_camp[2],                  "<input type='radio' name='camp2' value='0' id='method_273_1_0' checked='checked' /><label for='method_273_1_0'>".$tbl_sys_msg["off"]."</label><input type='radio' name='camp2' value='1' id='method_273_1_1' /><label for='method_273_1_1'>".$tbl_recruit[0]."</label><input type='radio' name='camp2' value='2' id='method_273_1_2' /><label for='method_273_1_2'>".$tbl_recruit[1]."</label>"),
                  array($tbl_camp[3],                  "<input type='radio' name='camp3' value='0' id='method_273_1_0' checked='checked' /><label for='method_273_1_0'>".$tbl_sys_msg["off"]."</label><input type='radio' name='camp3' value='1' id='method_273_1_1' /><label for='method_273_1_1'>".$tbl_recruit[0]."</label><input type='radio' name='camp3' value='2' id='method_273_1_2' /><label for='method_273_1_2'>".$tbl_recruit[1]."</label>")),
	"274"	=>	array($server_list,
                  array($tbl_recruit[0],							"<input type='text' name='minNum' value='100' />"),
                  array($tbl_recruit[1],							"<input type='text' name='maxNum' value='200' />"),
                  array($tbl_sys_msg["time"],					"<input type='text' name='time' value='720' />")),
	"275"	=> 	array($server_list),
	"276"	=>	array($server_list,
                  $begin_time,
                  $end_time),
	"277"	=>	array($server_list,
                  array($page_main["ip"],							"<input type='text' name='ip'/>"),
                  $begin_time,
                  $end_time),
	"278"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"279"	=>	array($server_item,
                  $tong_item,
                  $begin_time,
                  $end_time),
	"280"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"281"	=>	array($server_item,
                  $role_item,
                  $begin_time,
                  $end_time),
	"282"	=>	array($server_list,
                  array($page_main["type"],						"<input type='radio' name='type' value='0' id='282type0' checked='checked' /><label for='282type0'>".$tbl_sys_msg["SingleServer"]."</label><input type='radio' name='type' value='1' id='282type1' /><label for='282type1'>".$tbl_sys_msg["AllServer"]."</label>"),
                  array($page_main["number"],					"<input type='text' name='newMaxOnLineUserNum' />"),
                  array($page_main["user_name"],			"<input type='text' name='user_name' />"),
                  array("Game id",                  	"<input type='text' name='game_id' />")),
	"283"	=>	array($server_item,
                  array($page_main["tong"],						"<textarea name='tong_name' rows='10' cols='30'></textarea>"),
                  array($tbl_tong_msg["money"],				"<input type='text' name='money_1' style='text-align:right; width:30px;' />".$tbl_sys_msg["gold"]."<input type='text' name='money_2' style='text-align:right; width:20px;' />".$tbl_sys_msg["silver"]."<input type='text' name='money_3' style='text-align:right; width:20px;' />".$tbl_sys_msg["copper"]),
                  array($tbl_tong_msg["resource"],		"<input type='text' name='num_resource' />"),
                  array($tbl_tong_msg["honor"],				"<input type='text' name='num_honor' />"),
                  $reason),
	"286"	=>	array($server_item,
                  $role_item,
                  array($page_main["type"],						"<input type='checkbox' name='type[]' value='0' id='286type_all' class='checkbox_all' checked='checked' /><label for='286type_all'>".$tbl_sys_msg["all"]."</label><br/><input type='checkbox' name='type[]' value='u6i73A' id='286type_u6i73A' class='286type_checkbox' checked='checked' /><label for='286type_u6i73A'>".$tbl_commerce_skill["u6i73A"]."</label><input type='checkbox' name='type[]' value='ssm-8w' id='286type_ssm-8w' class='286type_checkbox' checked='checked' /><label for='286type_ssm-8w'>".$tbl_commerce_skill["ssm-8w"]."</label><br/><input type='checkbox' name='type[]' value='xevivw' id='286type_xevivw' class='286type_checkbox' checked='checked' /><label for='286type_xevivw'>".$tbl_commerce_skill["xevivw"]."</label><input type='checkbox' name='type[]' value='1sbSqQ' id='286type_1sbSqQ' class='286type_checkbox' checked='checked' /><label for='286type_1sbSqQ'>".$tbl_commerce_skill["1sbSqQ"]."</label><input type='checkbox' name='type[]' value='1v281w' id='286type_1v281w' class='286type_checkbox' checked='checked' /><label for='286type_1v281w'>".$tbl_commerce_skill["1v281w"]."</label><input type='checkbox' name='type[]' value='ts3U7A' id='286type_ts3U7A' class='286type_checkbox' checked='checked' /><label for='286type_ts3U7A'>".$tbl_commerce_skill["ts3U7A"]."</label><input type='checkbox' name='type[]' value='uaTS1Q' id='286type_uaTS1Q' class='286type_checkbox' checked='checked' /><label for='286type_uaTS1Q'>".$tbl_commerce_skill["uaTS1Q"]."</label>"),
                  $begin_time,
                  $end_time),
	"287"	=>	array($server_item,
                  array($page_main["npc_name"],				"<input type='radio' name='boss_name' value='4501' id='287npc_Qk9TU9X5tuFCT1NT' checked='checked' /><label for='287npc_Qk9TU9X5tuFCT1NT'>".$tbl_tong_boss["Qk9TU9X5tuFCT1NT"]."</label><br/><input type='radio' name='boss_name' value='5628' id='287npc_Qk9TU9X5tuFCT1NTMQ' /><label for='287npc_Qk9TU9X5tuFCT1NTMQ'>".$tbl_tong_boss["Qk9TU9X5tuFCT1NTMQ"]."</label><br/><input type='radio' name='boss_name' value='7315' id='287npc_Qk9TU9X5tuFCT1NTMg' /><label for='287npc_Qk9TU9X5tuFCT1NTMg'>".$tbl_tong_boss["Qk9TU9X5tuFCT1NTMg"]."</label><br/>"),
                  $begin_time,
                  $end_time),
	"288"	=>	array($server_item,
                  $role_item,
                  array($page_main["password"],				"<input type='password' name='password' />"),
                  array($tbl_sys_msg["time"],					"<input type='text' name='time' value='60' />".$page_main["step_ps"]),
                  $reason),
	"289"	=>	array($server_item,
                  $role_item,
                  $reason),
	"290"	=>	array($server_item,
                  $role_item,
                  array($page_main["type"],						"<input type='radio' name='type' value='0' id='290type_0' checked='checked' /><label for='290type_0'>".$tbl_sys_msg["all"]."</label><input type='radio' name='type' value='2' id='290type_2' /><label for='290type_2'>".$tbl_modulus_type[2]."</label><input type='radio' name='type' value='4' id='290type_4' /><label for='290type_4'>".$tbl_modulus_type[4]."</label>"),
                  $begin_time,
                  $end_time),
);
  
function ActionForm($id){
	global $action_form;
	$temp = $action_form[$id];
	//$str = "<table class=\"list_table\" id=\"action_".$id."\">\n";
	$str = "";
	foreach($temp as $item){
		$str.= "<tr><th>$item[0]</th><td>$item[1]</td></tr>";
	}
	//$str.= "</table>\n";
	return $str;
} 
?>
