<?php
function RetGetTaskInfo($tbl_content,$role_name, $role_id)
{
	$tbl_count=count($tbl_content);
	$str = "";
	global $page_main,$tbl_sys_msg;
  echo "<table class='list_table'>\n";
  echo sprintf("<tr><th>%s</th><th>%s</th><th>%s</th><th>%s</th></tr>\n", $page_main["task"], $tbl_sys_msg["status"], $page_main["begin_time"], $page_main["end_time"]);
	for($i=0;$i<$tbl_count;$i++){
		printf("<tr".(($i%2==0)?"":" class='even'")."><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>", Lan_Quest_GetDisplayName(Local2UTF8($tbl_content[$i][0])),$tbl_content[$i][1],$tbl_content[$i][2],("1970-01-01 08:00:00"==$tbl_content[$i][3])?"-":$tbl_content[$i][3]);
	}

	echo "</table>\n";
	ReturnAndBack();
}
function get_task_info($role_name,$state,$rank,$var)
{
	$serverid=$var["serverid"];
	$str = "";
	if($var["begin_time"]!=null && $var["begin_time"]!=""){
		$str.= " and ".$rank." >= ".strtotime($var["begin_time"]);
	}
	if($var["end_time"]!=null && $var["end_time"]!=""){
		$str.= " and ".$rank." <= ".strtotime($var["end_time"]);
	}
	global $tbl_list_task,$tbl_sys_msg;
	$role_name = mysql_escape_string($role_name);
	if($row=mysql_fetch_row(MySQLQuery("select cs_uId from tbl_char where c_sName = '".$role_name."'","Game",$serverid))){
		$role_id=$row[0];
	}else{
		PrintNotice($tbl_sys_msg["nobody"]);
		return false;
	}
	$query_string = "select q_sName,q_state,FROM_UNIXTIME(q_tAcceptTime),FROM_UNIXTIME(q_tFinishTime) from tbl_quest where cs_uId=".$role_id;
	if($state!=null){
		$query_string .= " and q_state=".$state;
	}
	$result = MySQLQuery($query_string.$str." order by ".$rank,"Game",$serverid);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$row[1]=$tbl_list_task[intval($row[1])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	
	if (!IsDownload($var))	
	{
		RetGetTaskInfo($tbl_content,$role_name, $role_id);
		return;
	}
}
function GetTaskInfo($var, $gm)
{
	$role_name = ArgRoleName();
	get_task_info($role_name,null,$var["rank"],$var);
	return true;
}
function GetOnGoingTask($var, $gm)
{
	$role_name = ArgRoleName();
	get_task_info($role_name,1,"q_tAcceptTime",$var);
	return true;
}
function GetFinishedTask($var, $gm)
{
	$role_name = ArgRoleName();
	get_task_info($role_name,3,"q_tFinishTime",$var);
	return true;
}
function SearchTaskInfo($var, $gm){
	global $tbl_list_task;
	$serverid=$var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$query_string = "select q_sName,q_state,FROM_UNIXTIME(q_tAcceptTime),FROM_UNIXTIME(q_tFinishTime) from tbl_quest where cs_uId=".$role_id." and q_sName = '".UTF82Local($var["quest_name"])."'";
	$result = MySQLQuery($query_string." order by q_tAcceptTime","Game",$serverid);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$row[1]=$tbl_list_task[intval($row[1])];
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	
	if (!IsDownload($var))	
	{
		RetGetTaskInfo($tbl_content,$role_name, $role_id);
		return true;
	}
	return true;
}
?>
