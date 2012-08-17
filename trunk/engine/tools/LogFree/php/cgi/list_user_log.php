#!/usr/bin/php
<?php
header("content-type: text/html; charset=utf-8");
?>
<html>
<title>user type</title>
<head>
<script type="text/javascript" src="/js/pager.js"></script>
<script type="text/javascript" src="/js/WriteStatus.js"></script>
<link href="/css/logfree.css" type="text/css" rel="stylesheet"></link>
</head>
<body align=center>
<br />
<table border=1 align='center' width='100%'><tr>
<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");
function ViewLog()
{
	global $db_host, $db_user, $db_pass, $db_name;	
	global $server_count_info, $server_from_info, $server_select_info, $server_where_info, $server_order_info;

	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);


	$qs = "select li.error_code, li.error_title, count(*) $server_from_info $server_where_info and li.error_type like '%UserAdvice%' $query_by_nf group by li.error_code order by li.error_title ";
	$result = mysql_query($qs);
	
	$str_code = "<a href='list_user_log_by_code.php?page=1&code=%s' target='list_user_log_by_code'>%s</a>";
	while($row=mysql_fetch_row($result))
	{
		$code = $row[0];
		$title= $row[1];
		$num  = $row[2];

		$qs2 = "select sum(distinct network_flag) from tbl_server_log_common where error_code = '$code'";
		$result2 = mysql_query($qs2);
		$row2 = mysql_fetch_row($result2);
		$cnf = intval($row2[0]);
                $network_flag = GetNetFlag($cnf);
?>
<th bgcolor='#9acd32'>
<?php
		echo "".sprintf($str_code, $code, $title)."<br />\n";
?>
</th>
<?php
	}

	printf($str);
	
}
ViewLog();
?>
</tr></table>
<!--<iframe name='list_user_log_by_code' width='100%' height='50%' frameborder=0 scrolling=0></iframe>-->
<iframe name="list_user_log_by_code" id="iframe" allowtransparency="true" align="default" marginwidth="0" marginheight="0" frameborder="0" scrolling="no" height="600" width='100%' onload="this.height=0;var fdh=(this.Document?this.Document.body.scrollHeight:this.contentDocument.body.offsetHeight);this.height=(fdh>600?fdh:600)"></iframe>
</body>
</html>

