#!/usr/bin/php
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />

<title>统计报表</title>

<link rel="stylesheet" href="/css/layout.css" type="text/css" media="all" />

<!--[if IE]>
        <script type="text/javascript" src="/js/excanvas.min.js"></script>
<![endif]-->
        
<script type="text/javascript" src="/js/jquery.js"></script>      
<script type="text/javascript" src="/js/jquery.flot.js"></script>      

</head>
<body>

<h1></h1>
<p style="padding-bottom: 12px;">客户端外网数据log统计报表</p>

<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");

global $db_host, $db_user, $db_pass, $db_name;	
$Conn = mysql_connect($db_host, $db_user, $db_pass);
mysql_query("SET NAMES 'GBK'");
mysql_select_db($db_name, $Conn);
?>
<div id="placeholder" style="width:900px;height:300px"></div>
<script id='source' language="javascript" type="text/javascript">
$(function(){
	var d1=[
<?php
	$qs = "select version, count(*) from tbl_client_log_common where network_flag=2 group by version order by version ";
	$result = mysql_query($qs);
	$x_axis = array();
	$flag1 = false;
	$index = 0;
	while($row = mysql_fetch_row($result)){
		if ($flag1){
			echo ",";
		}
		echo "[$index,".Log10($row[1])."]";
		$x_axis[$index] = array($index, $row[0], $row[1]);
		$flag1 = true;
		$index++;
	}
?>
	];
	var d2 = [
<?php
	$qs = "select version, count(distinct error_code) from tbl_client_log_common where network_flag=2 group by version order by version ";
	$result = mysql_query($qs);
	$flag2 = false;
	$index = 0;
	while($row = mysql_fetch_row($result)){
		if ($flag2)
			echo ",";
		echo "[$index,".Log10($row[1])."]";
		$flag2 = true;
		$x_axis[$index][3] = $row[1];
		$index++;
	}
?>
	];
	var d3 = [
<?php
	$qs = "select version, count(distinct client_guid) from tbl_client_log_common where network_flag=2 group by version order by version ";
	$result = mysql_query($qs);
	$flag3 = false;
	$index = 0;
	while($row = mysql_fetch_row($result)){
		if ($flag3)
			echo ",";
		echo "[$index,".Log10($row[1])."]";
		$x_axis[$index][4] = $row[1];
		$flag3 = true;
		$index++;
	}
?>
	];

	$.plot($("#placeholder"),[
	{label: "log总数", data: d1},
	{label: "log总类", data: d2},
	{label: "客户端数目", data: d3}],
		{
		series: {
			lines: {show: true},
			//points: {show: true}
		},
		xaxis:{
		},
		yaxis:{
			ticks: [0,[1,10],[2,100],[3,1000],[4,10000],[5,100000],[6,1000000]]
		},
		grid: {
			backgroundColor: {colors: ["#fff", "#eee"]}
		}
	});
});
			
</script>
<table border='1'>
<tr><th>索引</th><th>版本号</th><th>log总数</th><th>log总类</th><th>客户端数目</th></tr>
<?php
for($i = 0; $i < count($x_axis); ++$i)
{
	echo "<tr><td>";
	echo $x_axis[$i][0];
	echo "</td><td>";
	echo $x_axis[$i][1];
	echo "</td><td>";
	echo $x_axis[$i][2];
	echo "</td><td>";
	echo $x_axis[$i][3];
	echo "</td><td>";
	echo $x_axis[$i][4];
	echo "</td></tr>\n";
}
?>
</table>

</body>
</html>
