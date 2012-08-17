#!/usr/bin/php
<?php
require_once("../../conf/gm_tools/ServerList.php");

$tbl_serv_partition = array();
foreach($tbl_serv_group as $game_id => $array){
	$tbl_serv_partition[$game_id] = array();
	$tbl_serv_partition[$game_id]["default"] = $array;
}

$file = '<?php'."\n";
$file.= '$tbl_serv_partition = array('."\n";
foreach($tbl_serv_partition as $game_id => $groups){
	$file.= "\t\"".$game_id."\"\t=>\tarray(\n";
	foreach($groups as $group_name => $group){
		$file.= "\t\t\"".$group_name."\"\t=>\tarray(".implode(",",$group)."),\n";
	}
	$file.= "\t),\n";
}
$file.= ");\n";

$file.= '$tbl_serv_db = array('."\n";
foreach($tbl_serv_db as $serv_id => $array){
	$file.= "\t".$serv_id."\t=>\tarray(\"".$array[0]."\",\"".$array[1]."\",\"".$array[2]."\",\"".$array[3]."\",\"".$array[4]."\"),\n";
}
$file.= ");\n";

$file.= '$tbl_log_db = array('."\n";
foreach($tbl_log_db as $serv_id => $array){
	$file.= "\t".$serv_id."\t=>\tarray(\"".$array[0]."\",\"".$array[1]."\",\"".$array[2]."\",\"".$array[3]."\"),\n";
}
$file.= ");\n";

$file.= '$tbl_game_server = array('."\n";
foreach($tbl_game_server as $serv_id => $array){
	$file.= "\t".$serv_id."\t=>\tarray(\"".$array[0]."\",\"".$array[1]."\",\"".$array[2]."\"),\n";
}
$file.= ");\n";
$file.= '?>';

$f = fopen("../../conf/gm_tools/ServerList.php","w");
$ret = fputs($f,$file);
fclose($f);
?>
Finish.