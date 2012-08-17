<?php

function LogAction($action, $serv_id)
{
	$qs = "insert into tbl_log_action set action = '$action', serv_id = '$serv_id'";
	mysql_query($qs);
}

?>
