<?php
function ReqAddExp($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddExp_f);
}
function ReqAddExp_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = intval($var["num"]);	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "number" => $var["num"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddMoney($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddMoney_f);
}
function ReqAddMoney_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = intval($var["money_1"])*10000+intval($var["money_2"])*100+intval($var["money_3"]);	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "money" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddBindingMoney($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddBindingMoney_f);
}
function ReqAddBindingMoney_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = intval($var["money_1"])*10000+intval($var["money_2"])*100+intval($var["money_3"]);	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "money" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddTicket($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddTicket_f);
}
function ReqAddTicket_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = intval($var["num"]);	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "number" => $var["num"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddQuest($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddQuest_f);
}
function ReqAddQuest_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = $var["quest_name"];	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "task" => $var["quest_name"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqFinishQuest($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqFinishQuest_f);
}
function ReqFinishQuest_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = $var["quest_name"];	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "task" => $var["quest_name"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddEquip($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddEquip_f);
}
function ReqAddEquip_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$arg1 = intval($var["num"]);	
	$arg2 = "ItemType:$item_type ItemName:$item_name";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "type" => $item_type, "item" => $item_name, "number" => $var["num"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddItem($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddItem_f);
}
function ReqAddItem_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$arg1 = intval($var["num"]);	
	$arg2 = "ItemType:$item_type ItemName:$item_name";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "type" => $item_type, "item" => $item_name, "number" => $var["num"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqDelQuest($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqDelQuest_f);
}
function ReqDelQuest_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = $var["quest_name"];	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "task" => $var["quest_name"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddSoul($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddSoul_f);
}
function ReqAddSoul_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = intval($var["num"]);	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "number" => $var["num"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddBindingSoul($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddBindingSoul_f);
}
function ReqAddBindingSoul_f($var, $gm, $serv_id){
	$role_name = $var["role_name"];
	$arg1 = intval($var["num"]);	
	$arg2 = "";
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "number" => $var["num"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddGMMgr($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqAddGMMgr_f);
}
function ReqAddGMMgr_f($var, $gm, $serv_id){
	$arg1 = $var["user_name"];	
	$arg2 = $var["gm_level"].",".$var["game_id"];	
	$reason = $var["reason"];
	$opertion = array("user_name" => $arg1, "level" => $var["gm_level"], "GameId" => $var["game_id"], "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqMoneyCurrencyLimit($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqMoneyCurrencyLimit_f);
}
function ReqMoneyCurrencyLimit_f($var, $gm, $serv_id){
	$arg1 = $var["money_func"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("action" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], "", $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqMoneyCurrencyLimitCanel($var, $gm){
	BackAndRefresh();
	return eachServer($var, $gm, ReqMoneyCurrencyLimitCanel_f);
}
function ReqMoneyCurrencyLimitCanel_f($var, $gm, $serv_id){
	$arg1 = $var["money_func"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("action" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], "", $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqItemUseLimit($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqItemUseLimit_f);
}
function ReqItemUseLimit_f($var, $gm, $serv_id){
	GetItemInfo(mysql_escape_string($var["item_name"]), $item_type, $item_name);
	$arg1 = $item_name;
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("item" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqItemUseLimitCanel($var, $gm){
	BackAndRefresh();
	return eachServer($var, $gm, ReqItemUseLimitCanel_f);
}
function ReqItemUseLimitCanel_f($var, $gm, $serv_id){
	$arg1 = $var["item_name"];
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("item" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqJifenUseLimit($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqJifenUseLimit_f);
}
function ReqJifenUseLimit_f($var, $gm, $serv_id){
	$arg1 = $var["jifen_type"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("jifen" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqJifenUseLimitCanel($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqJifenUseLimitCanel_f);
}
function ReqJifenUseLimitCanel_f($var, $gm, $serv_id){
	$arg1 = $var["jifen_type"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("jifen" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqQuestFinishLimit($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqQuestFinishLimit_f);
}
function ReqQuestFinishLimit_f($var, $gm, $serv_id){
	$arg1 = $var["quest_name"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("task" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqQuestFinishLimitCanel($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqQuestFinishLimitCanel_f);
}
function ReqQuestFinishLimitCanel_f($var, $gm, $serv_id){
	$arg1 = $var["quest_name"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("task" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqObjDropLimit($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqObjDropLimit_f);
}
function ReqObjDropLimit_f($var, $gm, $serv_id){
	$item_array = explode("_",$var["item_name"]);
	$arg1 = $item_array[1];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("item" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqObjDropLimitCanel($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, ReqObjDropLimitCanel_f);
}
function ReqObjDropLimitCanel_f($var, $gm, $serv_id){
	$arg1 = $var["item_name"];	
	$arg2 = "";	
	$reason = $var["reason"];
	$opertion = array("item" => $arg1, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}



function ReqSendMail($var, $gm){
	ReturnAndBack();
	return eachServer($var, $gm, SendMail_f);
}
function SendMail_f($var, $gm, $serv_id){
	$role_name 	= $var["role_name"];
	$content		= $var["content"];
	$items			= $var["item"];
	$money_num	= intval($var["money_1"])*10000+intval($var["money_2"])*100+intval($var["money_3"]);	
	
	$arg1 = $content."|".$money_num;
	$arg2 = $items;
	$reason = $var["reason"];
	$opertion = array("role_name" => $role_name, "content" => $content, "item" => $items, "money" => $money_num, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $role_name, $serv_id, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serv_id);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	return true;
}
function ReqAddItemToTong($var,$gm){
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	$num = $var["num"];
	$item_info = $var["item_name"];
	$page = $var["uPage"];
	GetItemInfo($item_info, $item_type, $item_name);
	$reason = $var["reason"];
	
	$arg1 = $item_info."_".$num;
	$arg2 = $page;
	$reason = $var["reason"];
	global $bid2type,$tbl_tong_page;
	$opertion = array("tong" => $tong_name, "item" => $item_name, "type" => $bid2type[$item_type], "pos" => $tbl_tong_page[$page], "number" => $num, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $tong_name, $serverid, $arg1, $arg2, $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serverid);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	BackAndClose();
	return true;
}

function ReqAddToTong($var, $gm){
	global $page_action_name;
	if(($var["money_1"]!=null && $var["money_1"]!="") || ($var["money_2"]!=null && $var["money_2"]!="") || ($var["money_3"]!=null && $var["money_3"]!="")){
		echo "<h2>".$page_action_name["248"]."</h2>\n";
		$var["action_name"] = 248;
		ReqAddMoneyToTong($var,$gm);
	}
	if($var["num_resource"]!=null && $var["num_resource"]!=""){
		echo "<h2>".$page_action_name["249"]."</h2>\n";
		$var["action_name"] = 249;
		ReqAddResourceToTong($var,$gm);
	}
	if($var["num_honor"]!=null && $var["num_honor"]!=""){
		echo "<h2>".$page_action_name["250"]."</h2>\n";
		$var["action_name"] = 250;
		ReqAddHonorToTong($var,$gm);
	}
	return true;
}
function ReqAddMoneyToTong($var,$gm){
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	$num = intval($var["money_1"])*10000+intval($var["money_2"])*100+intval($var["money_3"]);
	$reason = $var["reason"];
	
	$arg1 = $num;
	global $bid2type,$tbl_tong_page;
	$opertion = array("tong" => $tong_name, "number" => $num, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $tong_name, $serverid, $arg1, "", $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serverid);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	BackAndClose();
	return true;
}

function ReqAddResourceToTong($var,$gm){
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	$num = $var["num_resource"];
	$reason = $var["reason"];
	
	$arg1 = $num;
	global $bid2type,$tbl_tong_page;
	$opertion = array("tong" => $tong_name, "number" => $num, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $tong_name, $serverid, $arg1, "", $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serverid);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	BackAndClose();
	return true;
}

function ReqAddHonorToTong($var,$gm){
	$tong_name = $var["tong_name"];
	$serverid = $var["serverid"];
	$num = $var["num_honor"];
	$reason = $var["reason"];
	
	$arg1 = $num;
	global $bid2type,$tbl_tong_page;
	$opertion = array("tong" => $tong_name, "number" => $num, "reason" => $reason);
	if(SaveReqAction($gm, $var["action_name"], $tong_name, $serverid, $arg1, "", $opertion)){
		LogGMCommand($gm, $var["action_name"], $opertion, $serverid);
		PrintNotice(LogInfo($opertion,true));
	}else
		PrintNotice(LogInfo($opertion,false));
	BackAndClose();
	return true;
}
?>
