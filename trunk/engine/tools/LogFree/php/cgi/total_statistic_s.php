#!/usr/bin/php
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />

<title>服务器外网数据统计报表</title>

<link rel="stylesheet" href="/css/piechart.css" type="text/css" media="all" />
<script type="text/javascript" src="/js/mootools-beta-1.2b2.js"></script> 

<!--[if IE]>
        <script type="text/javascript" src="/js/moocanvas.js"></script>
<![endif]-->
        
<script type="text/javascript" src="/js/piechart.js"></script>      

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
<p style="padding-bottom: 12px;">服务器端外网数据统计报表</p>

<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");

global $db_host, $db_user, $db_pass, $db_name;	
$Conn = mysql_connect($db_host, $db_user, $db_pass);
mysql_query("SET NAMES 'GBK'");
mysql_select_db($db_name, $Conn);
?>

<table class="pieChart">
    <tr><th>版本号  </th> <th>log总数</th></tr>
<?php
	$qs = "select version, count(*) from tbl_server_log_common where network_flag=2 group by version order by version desc";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		echo "<tr><td>".$row[0]."</td> <td>".$row[1]."</td></tr>\n";
	}
?>
<!--
    <tr><td>JavaScript</td> <td>100  </td></tr>
    <tr><td>CSS       </td> <td>200  </td></tr>
    <tr><td>HTML      </td> <td>180  </td></tr>
    <tr><td>PHP       </td> <td>50   </td></tr>
    <tr><td>MySQL     </td> <td>320  </td></tr>
    <tr><td>Apache    </td> <td>90   </td></tr>
    <tr><td>Linux     </td> <td>30   </td></tr>
-->
</table>

<table class="pieChart">
    <tr><th>版本号</th> <th>log总类</th></tr>
<?php
	$qs = "select version, count(distinct error_code) from tbl_server_log_common where network_flag=2 group by version order by version desc";
	$result = mysql_query($qs);
	while($row = mysql_fetch_row($result)){
		echo "<tr><td>".$row[0]."</td> <td>".$row[1]."</td></tr>\n";
	}
?>
<!--
    <tr><td>Safari </td> <td>180  </td></tr>
    <tr><td>Firefox</td> <td>210  </td></tr>
    <tr><td>IE     </td> <td>30   </td></tr>
    <tr><td>Opera  </td> <td>120  </td></tr>
-->
</table>

</body>
</html>
