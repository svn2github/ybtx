#!/usr/bin/php
<html>
<title>客户端连接上的log</title>
<head>
<script type="text/javascript" src="/js/pager.js"></script>
</head>
<body>
<p><a href='javascript:history.back(1)'><font face=arial size=2>[返回]</font></a></p>
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
	global $client_count_info, $client_from_info, $client_where_info, $client_select_info, $client_order_info;
	$page = intval($_GET["page"]);
	$guid = $_GET["guid"];

	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);
	$guid = mysql_escape_string($guid);
	
	$qs = "$client_count_info $client_from_info $client_where_info and clc.conn_guid= '$guid'";
	$result = mysql_query($qs);
	$row = mysql_fetch_row($result);
	$total = intval($row[0]);

	$b = $page * $size - $size;
	$e = $size;

	printf("<p>查询条件是客户端guid: <b>$guid</b><p>");
	
	$qs = "$client_select_info $client_from_info $client_where_info and clc.conn_guid = '$guid' $client_order_info limit $b, $e";
	$result = mysql_query($qs);
	
	$content = array();
	while($row=mysql_fetch_row($result))
	{
		$content[count($content)] = $row;
	}
	printf(GetClientLogHTML($content));
}
ViewLog();
?>
<div id="pager"></div>
<script>
<!--
<?php
	printf("var url='list_by_conn_guid_c.php?guid=%s&';\n", $_GET["guid"]);
	printf("CreatePage($page, $total, $size, \"pager\", $view_size, url);\n");	
?>
-->
</script>
<p><a href='javascript:history.back(1)'><font face=arial size=2>[返回]</font></a></p>
</body>
</html>
