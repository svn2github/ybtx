#!/usr/bin/php
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />

<title>Error统计</title>

<link rel="stylesheet" href="/css/piechart.css" type="text/css" media="all" />
<script type="text/javascript" src="/js/mootools-beta-1.2b2.js"></script> 

<!--[if IE]>
        <script type="text/javascript" src="/js/moocanvas.js"></script>
<![endif]-->
        
<script type="text/javascript" src="/js/piechart.js"></script> 
<script type="text/javascript" src="/js/WriteStatus.js"></script> 
<style type="text/css">

body    {
        font-family: Arial, Helvetica, sans-serif;
        font-size: 12px;
        line-height: 16px;
        margin: 20px 40px;
}

h1 {
        font-size: 24px;
        margin: 5px 0 0 0;
        padding: 0 0 12px 0;
}

a {
        color: #690;
        text-decoration: none;
        }

a:hover {
        color: #e60;
        text-decoration: none;
        }

p {
        margin: 0 0 9px 0;
        padding: 0;
}

</style>

</head>
<body>

<h1></h1>
<p style="padding-bottom: 12px;">统计报表</p>

<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");

global $db_host, $db_user, $db_pass, $db_name;	
$Conn = mysql_connect($db_host, $db_user, $db_pass);
mysql_query("SET NAMES 'GBK'");
mysql_select_db($db_name, $Conn);
?>

<table class="pieChart">
    <tr><th>log类型  </th> <th>log总数</th></tr>
<?php
	$qs = "select error_type, count(*) from tbl_log_info group by error_type";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		echo "<tr><td>".$row[0]."</td> <td>".$row[1]."</td></tr>\n";
	}
?>
</table>

<table class="pieChart">
    <tr><th>log状态</th> <th>log总类</th></tr>
<?php
	$qs = "select error_status, count(*) from tbl_log_info group by error_status";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		echo "<tr><td>".GetStatus($row[0])."</td> <td>".$row[1]."</td></tr>\n";
	}
?>
</table>

<table class="pieChart">
    <tr><th>log负责人</th> <th>log总类</th></tr>
<?php
	$qs = "select error_owner, count(*) from tbl_log_info group by error_owner";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		echo "<tr><td>".$row[0]."</td> <td>".$row[1]."</td></tr>\n";
	}
?>
</table>

</body>
</html>

