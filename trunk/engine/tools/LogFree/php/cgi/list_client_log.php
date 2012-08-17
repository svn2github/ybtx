#!/usr/bin/php
<html>
<title>程序客户端log列表</title>
<head>
<script type="text/javascript" src="/js/pager.js"></script>
<script type="text/javascript" src="/js/WriteStatus.js"></script>
<link href="/css/logfree.css" type="text/css" rel="stylesheet"></link>
</head>
<body align=center>
<h3>欢迎进入LogFree系统</h3>
<p>
      <ul>
         <li class="item other item1 "><a href='list_serv_log.php?page=1&order=1'>服务器端程序log</a></li>
         <li class="item current"><a href='list_client_log.php?page=1&order=1'>客户端程序log</a></li>
         <li class="item other item3 "><a href='list_art_log.php?page=1&order=1'>美术资源log</a></li>
         <li class="item other item4 "><a href='list_designer_log_c.php?order=1&page=1'>客户端配置表log</a></li>
         <li class="item other item5 "><a href='list_designer_log_s.php?order=1&page=1'>服务器配置表log</a></li>
	 <li class="item other item6 "><a href='list_compatible_log.php?order=1&page=1'>兼容性错误log</a></li>
	 <li class="item other item7 "><a href='list_user_log.php?order=1&page=1' target='_blank'>用户提交log</a></li>
	 <li class="item other item4 "><a href='list_statistic.php'>查看统计</a></li>
      </ul>
</p>
<br />
<p><b>所有客户端的log如下</b></p>
<table border=1>
<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");
$total = 0;
$page = 0;
$size=20;
$view_size = 5;
$order = 1;
$who = "";
$nf="";
$version="";

function ViewLog()
{
	global $db_host, $db_user, $db_pass, $db_name;	
	global $total, $page, $size, $view_size, $order, $who, $nf, $version;
	global $client_count_info, $client_from_info, $client_where_info, $client_select_info, $client_order_info;
	$page = $_GET["page"];
	$order= $_GET["order"];

	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);
	
	$who = $_GET["who"];
	$query_by_user = "";
	if ($who == "") 
		$query_by_user = "";
	else
		$query_by_user = " and error_owner = '$who'";

	$version = $_GET["version"];
	$query_by_version = "";
	if ($version != "")
		$query_by_version = " and version = '$version' ";

	$query_by_nf = "";
	$nf = intval($_GET["nf"]);
	if ($nf == 1)
		$query_by_nf = " and clc.network_flag = 1 ";
	else if ($nf == 2)
		$query_by_nf = " and clc.network_flag = 2 ";

	$qs = "select lat_dtDateTime from tbl_last_action_time";
	$rows = mysql_query($qs);
	$last_update_time = "";
	if ($row = mysql_fetch_row($rows))
		$last_update_time = $row[0];
	printf("<p>同步数据的最近时间是: $last_update_time </p>");
	
	$qs = "select count(distinct clc.error_code) from tbl_client_log_common clc, tbl_log_info li, tbl_guid_info gi where li.error_code = clc.error_code and gi.guid = clc.client_guid and li.error_type='程序' $query_by_user $query_by_nf $query_by_version"; 
	$result = mysql_query($qs);
	$row = mysql_fetch_row($result);
	$total = $row[0];

	$b = $page * $size - $size;
	$e = $size;
	$order_str = " order by first desc ";
	if ($order == 1)
	{
		$order_str = " order by last desc ";
	}
		
	
	$qs = "select li.error_code, li.error_title, max(clc.happened_time) as last, min(clc.happened_time) as first, count(clc.happened_time), count(distinct clc.client_guid), li.error_owner, li.error_status, max(clc.version) $client_from_info $client_where_info and li.error_type='程序' $query_by_user $query_by_nf $query_by_version group by li.error_code $order_str, clc_uId limit $b, $e";
	$result = mysql_query($qs);
	
	$str = sprintf("<tr bgcolor='#9acd32'>\n")
		.sprintf("<th width=400>log标题(点击查看堆栈)</th><th width=100>错误码</th><th width=125><a href='list_client_log.php?order=1&page=1&who=$who&nf=$nf&version=$version'>最后发生时间</a></th><th width=125><a href='list_client_log.php?order=2&page=1&who=$who&nf=$nf&version=$version'>首次发生时间</a></th><th width=75>次数(点击查看列表)</th><th width=75>客户端数</th><th width='100'>"
		.GetNetFlagTitle($nf, "list_client_log.php?order=$order&page=1&who=$who&version=$version")
		."</th><th width=75>")
		.GetOwner($who, "list_client_log.php?order=$order&page=1&nf=$nf&version=$version")
		.sprintf("</th><th width=75>处理状态</th><th width=75>")
		.GetVersion("c", $version, "list_client_log.php?order=$order&page=1&nf=$nf&who=$who")
		.sprintf("</th>\n")
		.sprintf("</tr>\n");

	$str_code = "<a href='list_by_code_c.php?page=1&code=%s' target='_blank'>%s</a>";
	$str_title = "<a href='detail_c.php?code=%s' target='_blank'>%s</a>";

	$i=1;
	while($row=mysql_fetch_row($result))
	{
		$code = $row[0];
		$title= $row[1];
		$last = $row[2];
		$first= $row[3];
		$times= $row[4];
		$clients = $row[5];
		$color = "#ffffff";
		$owner = $row[6];
		$status = $row[7];
		$ver = $row[8];

		$qs2 = "select sum(distinct network_flag) from tbl_client_log_common where error_code = '$code'";
		$result2 = mysql_query($qs2);
		$row2 = mysql_fetch_row($result2);
		$cnf = intval($row2[0]);
                $network_flag = GetNetFlag($cnf);

		if ($i % 2)
			$color = "#d8d8d8";
		$str = $str.sprintf("<tr bgcolor='%s'>\n", $color)
			.sprintf("<td align='left'>&nbsp;").sprintf($str_title, urlencode($code), str_replace('%', '%%', $title)).sprintf("</td>\n")
			.sprintf("<td align='center'>%s</td>", $code)
			.sprintf("<td align='center'>%s</td>", $last)
			.sprintf("<td align='center'>%s</td>", $first)
			.sprintf("<td align='center'>").sprintf($str_code, $code, $times).sprintf("</td>")
			.sprintf("<td align='center'>$clients</td>")
                        .sprintf("<td align='center'>%s</td>", $network_flag)
			.sprintf("<td align='center'>%s</td>", WriteOwner($owner))
			.sprintf("<td align='center'><script>WriteStatus(%d)</script></td>", $status)
			.sprintf("<td align='center'>%s</td>", $ver)
			.sprintf("</tr>");
		$i++;
	}

	printf($str);
	
}
ViewLog();
?>

</table>
<div id="pager"></div>
<script>
<!--
<?php
	printf("var url='list_client_log.php?order=%d&who=%s&nf=%d&version=%s&';\n", $order, $who, $nf, $version);
	printf("CreatePage($page, $total, $size, \"pager\", $view_size, url);\n");	
?>
-->
</script>
</body>
</html>
