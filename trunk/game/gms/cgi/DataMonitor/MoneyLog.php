<?php
$production_all = "2,4,32,42,78,79,84,91,105,108,168,146,159";
$consumption_all = "1,6,27,86,88,89,96,99,153,156,160,167,173,179,181,182,183,184";
$production_binding = "2,4,78,42,84,146";
$consumption_binding = "1,89,88,96,99,153,173,179,181,182,183,184";
$production_circulation = "32,42,79,84,108,159,91,105,168";
$consumption_circulation = "1,6,27,89,88,96,99,86,153,179,181,182,183,184,156,167,160";
$circulation = "3,114,165,81,85,152";

function number_to_string($number,$f=0){
	$ceil = floor($number);
	$float = floor($number * pow(10,$f) - $ceil * pow(10,$f));
	$ceil = preg_replace('/(?<=[0-9])(?=(?:[0-9]{3})+(?![0-9]))/',',',$ceil);
	if($f==0){
		$str = $ceil;
	}else{
		for($i=$f-strlen($float);$i>0;$i--)
			$temp .= "0";
		$str = $ceil.".".$temp.$float;
	}
	return $str;
}
function money_to_string($m){
	global $tbl_sys_msg;
	$t = floor($m/100);
	
	$g = floor($t/100);
	$s = $t - ($g*100);
	$c = $m - ($t*100);
	
	$str = "";
	if($g>0) $str.= number_to_string($g).$tbl_sys_msg["gold"];
	if($s>0) $str.= $s.$tbl_sys_msg["silver"];
	if($c>0) $str.= $c.$tbl_sys_msg["copper"];
	
	return $str;
}

function MoneyLog_2_6($var, $gm){
	global $tbl_serv_db,$production_all,$tbl_event_type,$tbl_money_binding,$tbl_sys_msg,$page_main;
	echo "<table class=\"list_table\" style=\"text-align:right;\">\n";
	echo "<tr style=\"text-align:center;\"><th>".$tbl_sys_msg["time"]."</th><th>".$page_main["event_type"]."</th><th>".$page_main["money_type"]."</th><th>".$page_main["number"]."</th></tr>\n";
	
	$day = $var["day"];
	$date = date("Y-m-d",time()-($day*24*60*60));
	
	$serverid = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$result = MySQLQuery("select le.le_dtDateTime, let.let_uEventType, lm.lm_uMoneyType, lm.lm_uMoney from tbl_log_money lm, tbl_log_event le force index(le_dtDateTime), tbl_log_event_type let where lm.le_uId = le.le_uId and let.le_uId = le.le_uId and let.let_uEventType in ($production_all) and lm.lm_uMoney > 0 and lm.lm_uMoneyType < 3 and le.le_dtDateTime >= '$date 00:00:00' and lm.lcs_uId = '$role_id'","Log",$serverid);
	while($row = mysql_fetch_row($result)){
		printf("<tr><td style='text-align:left;'>%s</td><td style='text-align:left;'>%s</td><td style='text-align:left;'>%s</td><td>%s</td></tr>\n",$row[0],($tbl_event_type[$row[1]]=="")?$row[1]:$tbl_event_type[$row[1]],$tbl_money_binding[$row[2]],number_to_string($row[3]));
	}
	echo "</table>\n";
	return true;
}

function MoneyLog_2_7($var, $gm){
	global $tbl_serv_db,$consumption_all,$tbl_event_type,$tbl_money_binding,$tbl_sys_msg,$page_main;
	echo "<table class=\"list_table\" style=\"text-align:right;\">\n";
	echo "<tr style=\"text-align:center;\"><th>".$tbl_sys_msg["time"]."</th><th>".$page_main["event_type"]."</th><th>".$page_main["money_type"]."</th><th>".$page_main["number"]."</th></tr>\n";
	
	$day = $var["day"];
	$date = date("Y-m-d",time()-($day*24*60*60));
	
	$serverid = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$result = MySQLQuery("select le.le_dtDateTime, let.let_uEventType, lm.lm_uMoneyType, lm.lm_uMoney from tbl_log_money lm, tbl_log_event le force index(le_dtDateTime), tbl_log_event_type let where lm.le_uId = le.le_uId and let.le_uId = le.le_uId and let.let_uEventType in ($consumption_all) and lm.lm_uMoney < 0 and lm.lm_uMoneyType < 3 and le.le_dtDateTime >= '$date 00:00:00' and le.le_dtDateTime < '".date("Y-m-d")." 00:00:00' and lm.lcs_uId = '$role_id'","Log",$serverid);
	while($row = mysql_fetch_row($result)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[0],$tbl_event_type[$row[1]],$tbl_money_binding[$row[2]],number_to_string($row[3]));
	}
	echo "</table>\n";
	return true;
}

function MoneyLog_3_7($var, $gm){
	global $circulation,$tbl_event_type,$tbl_serv_db,$tbl_sys_msg,$page_main;
	
	echo "<table class=\"list_table\" style=\"text-align:right;\">\n";
	echo "<tr style=\"text-align:center;\"><th>".$tbl_sys_msg["time"]."</th><th>".$page_main["event_type"]."</th><th>".$page_main["number"]."</th><th>".$tbl_sys_msg["desc"]."</th></tr>\n";
	
	$day = $var["day"];
	$date = date("Y-m-d",time()-($day*24*60*60));
	
	$server_id = $var["serverid"];
	
	$tbl = array();
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$qs = "select lm.le_uId, le.le_dtDateTime, let.let_uEventType, lm.lm_uMoney from tbl_log_money lm, tbl_log_event le force index(le_dtDateTime), tbl_log_event_type let where lm.le_uId = le.le_uId and le.le_uId = let.le_uId and let.let_uEventType in ($circulation) and le.le_dtDateTime > '$date 00:00:00' and lm.lcs_uId = $role_id";
	$result = MySQLQuery($qs,"Log",$server_id);
	while($row = mysql_fetch_row($result)){
		$le_uId = $row[0];
		$event_type = $row[2];
		
		$str = "";
		
		switch($event_type){
		case 114:	//订单被成功购买邮件
			break;
		case 165:	//代工成功
			break;
		case 81:	//玩家邮件提取钱
			break;
		case 85:	//玩家交易
			break;
		case 152:	//拍卖平均分钱
			break;
		}
		
		printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",$row[1],$tbl_event_type[$row[2]],number_to_string($row[3]),$str);
	}
	
	echo "</table>\n";
	return true;
}
?>
