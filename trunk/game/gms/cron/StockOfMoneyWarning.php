#!/usr/bin/php
<?php
require_once("/home/webfold/conf/gm_tools/Config.php");
function MainFun($serv_id,$money_max,$binding_max){
	global $tbl_serv_db,$tbl_game_server,$tbl_gm_db;
	$dbinfo = $tbl_serv_db[$serv_id];
	$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
	mysql_select_db($dbinfo[3],$Conn);
	$qs = "select sum(cm_uMoney) s1, sum(cm.cm_uBindingMoney) s2, count(*) c from tbl_char_onlinetime co, tbl_char_money cm where co.cs_uId = cm.cs_uId and co.co_dtLastLoginTime > addDate(now(), interval -3 day) having (s1/c) > $money_max or (s2/c) > $binding_max";
	$result = mysql_query($qs);
	if(!$result) echo mysql_error()."\n";

	$dbinfo = $tbl_gm_db[0];
	$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
        mysql_select_db($dbinfo[3],$Conn);

	while($row = mysql_fetch_row($result)){
		mysql_query("insert into tbl_server_money_warning values('".date("Y-m-d h:i:s")."','$serv_id','$row[2]','$row[0]','$row[1]')");
		echo date("Y-m-d h:i:s")."\t".$tbl_game_server[$serv_id][2]."\t".$row[2]."\t".$row[0]."\t".$row[0]/$row[2]."\t".$row[1]."\t".$row[1]/$row[2]."\n";
	}
	mysql_close($Conn);
}

if(count($argv)==3){
	global $tbl_serv_db;
	foreach($tbl_serv_db as $serv => $dbinfo){
		MainFun($serv,$argv[1],$argv[2]);
	}
}else if(count($argv)==4){
	MainFun($argv[1],$argv[2],$argv[3]);
}else{
	echo "Argv error";
}
?>
