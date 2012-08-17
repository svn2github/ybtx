#!/usr/bin/php
<?php
header("content-type: text/html; charset=utf-8");
?>
<html>
<head>
<script type="text/javascript" src="/js/pager.js"></script>
</head>
<body>
<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");
$total = 0;
$page = 0;
$size=20;
$view_size = 5;
function ViewLog()
{
	global $db_host, $db_user, $db_pass, $db_name;	
	global $total, $page, $size, $view_size;
	global $server_count_info, $server_from_info, $server_select_info, $server_where_info, $server_order_info;
	$page = intval($_GET["page"]);
	$code = $_GET["code"];

	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);
	$code = mysql_escape_string($code);
	
	$qs = "$server_count_info $server_from_info $server_where_info and li.error_code = '$code'";
	$result = mysql_query($qs);
	$row = mysql_fetch_row($result);
	$total = intval($row[0]);

	$b = $page * $size - $size;
	$e = $size;

	$qs = "$server_select_info $server_from_info $server_where_info and li.error_code = '$code' order by happened_time desc limit $b, $e";
	$result = mysql_query($qs);
	
	$content = array();
	while($row=mysql_fetch_row($result))
	{
		$content[count($content)] = $row;
	}
	printf(GetUserLogHTML($content));
}
ViewLog();
?>
<div id="pager"></div>
<script>
<!--
<?php
	printf("var url='list_user_log_by_code.php?code=%s&';\n", $_GET["code"]);
	printf("CreatePage($page, $total, $size, \"pager\", $view_size, url);\n");	
?>
-->
</script>
</body>
</html>
