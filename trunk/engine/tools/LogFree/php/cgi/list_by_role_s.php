#!/usr/bin/php
<html>
<title>服务器端角色log列表</title>
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
	global $server_count_info, $server_from_info, $server_select_info, $server_where_info, $server_order_info;
	$page = intval($_GET["page"]);
	$role = $_GET["role"];

	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);
	$role = mysql_escape_string($role);
	
	$qs = "$server_count_info $server_from_info $server_where_info and slc.role_name = '$role'";
	$result = mysql_query($qs);
	$row = mysql_fetch_row($result);
	$total = intval($row[0]);

	$b = $page * $size - $size;
	$e = $size;

	printf("<p>查询条件是角色名: <b>$role</b><p>");
	
	$qs = "$server_select_info $server_from_info $server_where_info and slc.role_name = '$role' order by happened_time desc limit $b, $e";
	$result = mysql_query($qs);
	
	$content = array();
	while($row=mysql_fetch_row($result))
	{
		$content[count($content)] = $row;
	}
	printf(GetServerLogHTML($content));
}
ViewLog();
?>
<div id="pager"></div>
<script>
<!--
<?php
	printf("var url='list_by_role_s.php?role=%s&';\n", $_GET["role"]);
	printf("CreatePage($page, $total, $size, \"pager\", $view_size, url);\n");	
?>
-->
</script>
<p><a href='javascript:history.back(1)'><font face=arial size=2>[返回]</font></a></p>
</body>
</html>
