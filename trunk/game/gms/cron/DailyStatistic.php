<?php
require("../../conf/gm_tools/Config.php");
function DailyStatistic($serverid)
{
	global $tbl_serv_db, $tbl_gm_db;

	$dbinfo=$tbl_serv_db[intval($serverid)];

	$Conn1 = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
	mysql_select_db($dbinfo[3], $Conn1);
	

	$dbinfo=$tbl_gm_db[0];
	$Conn2 = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1], true);
	//mysql_query("SET NAMES 'GBK'", $Conn2);
	mysql_select_db($dbinfo[3], $Conn2);

	//处理钱
	$qs = "SELECT us.us_uId, us.us_sName, cs.cs_uId, c.c_sName, date(le.le_dtDateTime), sum(lm.lm_uMoney) AS sum_money, $serverid AS serverid FROM tbl_log_money lm, tbl_log_event le, tbl_log_player_taker lpt, tbl_user_static us, tbl_char_static cs, tbl_char c WHERE lpt.lcs_uId = cs.cs_uId AND c.cs_uId = cs.cs_uId AND cs.us_uId = us.us_uId AND lpt.le_uId = le.le_uId AND le.le_uId = lm.le_uId AND date(le.le_dtDateTime) = date(DATE_ADD(NOW(), INTERVAL -1 day)) group by cs.cs_uId";
	$result1 = mysql_query($qs, $Conn1);
	while ($row = mysql_fetch_row($result1)){
		$qs = sprintf("insert into tbl_money_statistic values ('%s', '%s', '%s','%s','%s','%s','%s')",
			$row[0], $row[1], $row[3], $row[3], $row[4], $row[5], $row[6]);
		mysql_query($qs, $Conn2);
	}
	

	//经验
	$qs = "SELECT us.us_uId, us.us_sName, cs.cs_uId, c.c_sName, date(le.le_dtDateTime), sum(lm.le_uExp) AS sum_exp, $serverid AS serverid FROM tbl_log_experience lm, tbl_log_event le, tbl_log_player_taker lpt, tbl_user_static us, tbl_char_static cs, tbl_char c WHERE lpt.lcs_uId = cs.cs_uId AND c.cs_uId = cs.cs_uId AND cs.us_uId = us.us_uId AND lpt.le_uId = le.le_uId AND le.le_uId = lm.le_uId AND date(le.le_dtDateTime) = date(DATE_ADD(NOW(), INTERVAL -1 day)) group by cs.cs_uId";
	$result = mysql_query($qs, $Conn1);
	while ($row = mysql_fetch_row($result)){
		$qs = sprintf("insert into tbl_exp values ('%s', '%s', '%s','%s','%s','%s','%s')",
			$row[0], $row[1], $row[3], $row[3], $row[4], $row[5], $row[6]);
		mysql_query($qs, $Conn2);
	}

	//魂
	$qs = "SELECT us.us_uId, us.us_sName, cs.cs_uId, c.c_sName, date(le.le_dtDateTime), sum(lm.ls_uSoulValue) AS sum_money, $serverid AS serverid FROM tbl_log_soul lm, tbl_log_event le, tbl_log_player_taker lpt, tbl_user_static us, tbl_char_static cs, tbl_char c WHERE lpt.lcs_uId = cs.cs_uId AND c.cs_uId = cs.cs_uId AND cs.us_uId = us.us_uId AND lpt.le_uId = le.le_uId AND le.le_uId = lm.le_uId AND date(le.le_dtDateTime) = date(DATE_ADD(NOW(), INTERVAL -1 day)) group by cs.cs_uId";
	$result = mysql_query($qs, $Conn1);
	while ($row = mysql_fetch_row($result)){
		$qs = sprintf("insert into tbl_soul_statistic values ('%s', '%s', '%s','%s','%s','%s','%s')",
			$row[0], $row[1], $row[3], $row[3], $row[4], $row[5], $row[6]);
		mysql_query($qs, $Conn2);
	}

	//物品
	$qs = "SELECT gm_sVarValue FROM tbl_gm_monitor WHERE gm_sVarName = 'MonitItem'";
	$result = mysql_query($qs);
	$flag = false;
	$item_str = "(";
	while ($row = mysql_fetch_row($result)){
		if ($flag){
			$item_str = $item_str.",";
		}
		$item_str = $item_str."'".$row[0]."'";
		$flag = true;
	}
	$item_str = $item_str.")";
	$qs = "SELECT us.us_uId, us.us_sName, cs.cs_uId, c.c_sName, date(le.le_dtDateTime), lis.lis_sName, count(li.lis_uId) AS item_count, $serverid AS serverid FROM tbl_log_item li, tbl_log_item_static lis, tbl_log_event le, tbl_log_player_taker lpt, tbl_user_static us, tbl_char_static cs, tbl_char c WHERE lpt.lcs_uId = cs.cs_uId AND c.cs_uId = cs.cs_uId AND cs.us_uId = us.us_uId AND lpt.le_uId = le.le_uId AND le.le_uId = li.le_uId AND li.lis_uId = lis.lis_uId AND lis.lis_sName in $item_str AND date(le.le_dtDateTime) = date(DATE_ADD(NOW(), INTERVAL -1 day)) group by cs.cs_uId";
	$result = mysql_query($qs, $Conn1);
	while ($row = mysql_fetch_row($result)){
		$qs = sprintf("insert into tbl_item_statistic values ('%s', '%s', '%s','%s','%s', '%s')",
			$row[0], $row[1], $row[3], $row[3], $row[4], $row[5], $row[6], $row[7]);
		mysql_query($qs, $Conn2);
	}

	//登陆
	$qs = "SELECT lus.lus_uId, lus.lus_sName, date(le.le_dtDateTime), count(le.le_uId), $serverid AS serverid FROM tbl_log_event le, tbl_log_login ll, tbl_log_user_static lus, tbl_log_user lu WHERE date(le.le_dtDateTime) = date(DATE_ADD(NOW(), INTERVAL -1 day)) AND le.le_uId = lu.le_uId AND ll.le_uId = le.le_uId AND lus.lus_uId = lu.lus_uId group by lus.lus_uId";
	$result = mysql_query($qs, $Conn1);
	while ($row = mysql_fetch_row($result)){
		$qs = sprintf("insert into tbl_login_statistic values ('%s', '%s', '%s','%s','%s')",
			$row[0], $row[1], $row[2], $row[3], $row[4]);
		mysql_query($qs, $Conn2);
	}
	mysql_close($Conn1);
	mysql_close($Conn2);
}

function player_info_statistic($serverid) {
	global $tbl_serv_db, $tbl_gm_db;

	$dbinfo=$tbl_serv_db[intval($serverid)];

	$Conn1 = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
	mysql_select_db($dbinfo[3], $Conn1);
	

	$dbinfo=$tbl_gm_db[0];
	$Conn2 = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1], true);
	//mysql_query("SET NAMES 'GBK'", $Conn2);
	mysql_select_db($dbinfo[3], $Conn2);
	
	//tbl_statistic_gender
	//	(query_date, male, female, serverid)
	$qs = "select cs_uSex, count(*) from tbl_char_static group by cs_uSex";
	$result = mysql_query($qs, $Conn1);
	$male = 0; $female = 0;
	while($row = mysql_fetch_row($result)){
		if ($row[0] == 1) $male = $row[1];
		if ($row[0] == 2) $female = $row[1];
	}
	$qs = "insert into tbl_statistic_gender set query_date = date(now()), male = '$male', female = '$female', serverid = '$serverid'";
	mysql_query($qs, $Conn2);

	//tbl_statistic_class
	//	(query_date,dj,mj,qs,fs,xm,ms,aq,jl,sr,serverid)
	$qs = "select cs_uClass, count(*) from tbl_char_static group by cs_uClass";
	$tbl_class_info = array(0,0,0,0,0,0,0,0,0,0);
	$result = mysql_query($qs, $Conn1);
	while($row = mysql_fetch_row($result)){
		$tbl_class_info[$row[0]] = $row[1];
	}
	$qs = sprintf("insert into tbl_statistic_class set query_date = date(now()), dj=%d,mj=%d,qs=%d,fs=%d,xm=%d,ms=%d,aq=%d,jl=%d,sr=%d, serverid = '$serverid'", $tbl_class_info[1], $tbl_class_info[2], $tbl_class_info[3], $tbl_class_info[4], $tbl_class_info[5], $tbl_class_info[6], $tbl_class_info[7], $tbl_class_info[8], $tbl_class_info[9]);
	mysql_query($qs, $Conn2);

	//tbl_statistic_level
	//	(query_date, level, num, serverid)
	$qs = "select cb_uLevel, count(*) from tbl_char_basic group by cb_uLevel";
	$result = mysql_query($qs, $Conn1);
	while($row=mysql_fetch_row($result)){
		$qs = sprintf("insert into tbl_statistic_level set query_date = date(now()), level=%d, num=%d, serverid='$serverid'",
			$row[0], $row[1]);
		mysql_query($qs, $Conn2);
	}
	
	//tbl_statistic_camp
	//	(query_date, aimi, shensheng, xiusi, serverid)
	$tbl_camp_info = array(0,0,0,0);
	$qs = "select cs_uCamp, count(*) from tbl_char_static group by cs_uCamp";
	$result = mysql_query($qs, $Conn1);
	while ($row = mysql_fetch_row($result)){
		$tbl_camp_info[$row[0]] = $row[1];
	}
	$qs = sprintf("insert into tbl_statistic_camp set query_date = date(now()), aimi = %d, shensheng=%d, xiusi=%d, serverid = $serverid", $tbl_camp_info[1], $tbl_camp_info[2], $tbl_camp_info[3]);
	mysql_query($qs, $Conn2);
	
	mysql_close($Conn1);
	mysql_close($Conn2);
}

function login_info_statistic($serverid){
	global $tbl_serv_db, $tbl_gm_db;

	$dbinfo=$tbl_serv_db[intval($serverid)];

	$Conn1 = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1]);
	mysql_select_db($dbinfo[3], $Conn1);
	

	$dbinfo=$tbl_gm_db[0];
	$Conn2 = mysql_connect($dbinfo[2], $dbinfo[0], $dbinfo[1], true);
	//mysql_query("SET NAMES 'GBK'", $Conn2);
	mysql_select_db($dbinfo[3], $Conn2);

	$qs = "create view view_online_time_info as select lu.lus_uid as userid, lli.ll_sIP as ip, le1.le_dtDateTime as begin_time,  le2.le_dtDateTime as end_time from tbl_log_user as lu, tbl_log_event le1, tbl_log_event le2, tbl_log_login lli, tbl_log_logout llo where lu.le_uid = lli.le_uId and lli.ll_sSN = llo.ll_sSN and lli.le_uId = le1.le_uId and le2.le_uId = llo.le_uId  and date(le1.le_dtDateTime) = date(date_add(now(), interval -1 day));";
	mysql_query($qs, $Conn1);
	$qs = "select userid, us.us_sName, sum(timestampdiff(SECOND,begin_time,end_time)) as onlinetime, ip from tbl_user_static us, view_online_time_info voti where us.us_uid = userid group by userid";
	$result = mysql_query($qs, $Conn1);
	while($row=mysql_fetch_row($result)){
		$qs = sprintf("insert into user_daily_online_time set userid=%d, username='%s', onlinetime=%d, query_date=now(), ip='%s', serverid='$serverid'", $row[0], $row[1], $row[2], $row[3]);
		mysql_query($qs, $Conn2);
	}
	
	$qs = "drop view view_online_time_info";
	mysql_query($qs, $Conn1);

	mysql_close($Conn1);
	mysql_close($Conn2);
}

function MoneyLog(){
	echo "========Money Log Begin=========\n";
	global $tbl_log_db, $tbl_gm_db;
	
	$log_conn = mysql_connect($tbl_log_db[0][2], $tbl_log_db[0][0], $tbl_log_db[0][1]);
	mysql_select_db($tbl_log_db[0][3], $log_conn);
	$qs = "select date(le.le_dtDateTime), let.let_uEventType, lm.lm_uMoneyType, ll.ll_uLevel, le.le_uServerId, sum(lm_uMoney), count(*), lm.lcs_uId ";
	$qs.= "from ";
	$qs.= "tbl_log_money lm, tbl_log_event le, tbl_log_event_type let ";
	$qs.= "left join tbl_log_level ll on  where lm.le_uId = le.le_uId and lm.le_uServerId = le.le_uServerId and let.le_uId = le.le_uId and let.le_uServerId = le.le_uServerId and ll.le_uId = le.le_uId and ll.le_uServerId = le.le_uServerId and lm.lm_uMoney > 0 and le.le_dtDateTime >= date(date_add(now(), interval -1 day)) and le.le_dtDateTime < date(now()) group by let.let_uEventType, lm.lm_uMoneyType, ll.ll_uLevel, le.le_uServerId, lm.lcs_uId";
	$result = mysql_query($qs, $log_conn);
	$rs = mysql_fetch_row(mysql_query("select found_rows(), date(date_add(now(), interval -1 day))"));
	echo "Date: ".$rs[1]."\n";
	$rows = $rs[0];
	if(!$result) echo mysql_error()."\n";
	$gm_conn = mysql_connect($tbl_gm_db[0][2], $tbl_gm_db[0][0], $tbl_gm_db[0][1]);
	mysql_select_db($tbl_gm_db[0][3], $gm_conn);
	while($row = mysql_fetch_row($result)){
		$is = "insert into tbl_daily_money values('$row[0]','$row[1]','$row[2]','$row[3]','$row[4]','$row[5]','$row[6]','$row[7]')";
		mysql_query($is, $gm_conn);
	}
	
	$log_conn = mysql_connect($tbl_log_db[0][2], $tbl_log_db[0][0], $tbl_log_db[0][1]);
	mysql_select_db($tbl_log_db[0][3], $log_conn);
	$qs = "select date(le.le_dtDateTime), let.let_uEventType, lm.lm_uMoneyType, ll.ll_uLevel, le.le_uServerId, sum(lm_uMoney), count(*), lm.lcs_uId from tbl_log_money lm, tbl_log_event le, tbl_log_event_type let, tbl_log_level ll where lm.le_uId = le.le_uId and lm.le_uServerId = le.le_uServerId and let.le_uId = le.le_uId and let.le_uServerId = le.le_uServerId and ll.le_uId = le.le_uId and ll.le_uServerId = le.le_uServerId and lm.lm_uMoney < 0 and le.le_dtDateTime >= date(date_add(now(), interval -1 day)) and le.le_dtDateTime < date(now()) group by let.let_uEventType, lm.lm_uMoneyType, ll.ll_uLevel, le.le_uServerId, lm.lcs_uId";
	$result = mysql_query($qs, $log_conn);
	$rs = mysql_fetch_row(mysql_query("select found_rows()"));
	echo "Rows: ".($rows + $rs[0])."\n";
	if(!$result) echo mysql_error()."\n";
	$gm_conn = mysql_connect($tbl_gm_db[0][2], $tbl_gm_db[0][0], $tbl_gm_db[0][1]);
	mysql_select_db($tbl_gm_db[0][3], $gm_conn);
	while($row = mysql_fetch_row($result)){
		$is = "insert into tbl_daily_money values('$row[0]','$row[1]','$row[2]','$row[3]','$row[4]','$row[5]','$row[6]','$row[7]')";
		mysql_query($is, $gm_conn);
	}
	
	mysql_close($gm_conn);
	mysql_close($log_conn);
	echo "========Money Log End=========\n";
}

if (count($argv) != 2){
	return;
}

MoneyLog();
DailyStatistic($argv[1]);
player_info_statistic($argv[1]);
login_info_statistic($argv[1]);
?>
