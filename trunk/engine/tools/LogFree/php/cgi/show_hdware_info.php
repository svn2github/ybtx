#!/usr/bin/php
<html>
<title>客户端硬件信息</title>
<head>
<script type="text/javascript"/>
<!--//
//-->
</script>
<style>
.ta
{ 
	width:800; 
	height:500;
	word-wrap: normal;
	overflow-x:auto; 
	overflow-y:auto;
}
</style>
</head>
<body>
<?php
require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");
function ViewLog()
{
/*
create table tbl_log_info(
	error_code int unsigned not null,
	error_title varchar(256) not null,
	error_type varchar(32) not null,
	error_stack text not null,
	error_content text not null,
	primary key(error_code),
	key(error_title)
)engine = InnoDB default charset = latin1;
*/
	global $db_host, $db_user, $db_pass, $db_name;	
	$guid = $_GET["guid"];

	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);
	$guid = mysql_escape_string($guid);
	
	$qs = "select gi.name, hi.os, hi.hd, hi.cpu, hi.video, hi.dx, hi.mem, hi.D3D_Desc, hi.D3D_Mem from tbl_hardware_info hi, tbl_guid_info gi where gi.guid = hi.guid and hi.guid = '$guid'";
	$result = mysql_query($qs);

	$str = "";
	$content = array();
	if($row=mysql_fetch_row($result))
	{
		$str = $str.sprintf("<ol>\n")
			.sprintf("<li><p><b>客户端硬件详细信息如下：</b></p>\n")
			.sprintf("<p>IP: %s</p>\n", $row[0])
			.sprintf("<p>OS: %s</p>\n", $row[1])
			.sprintf("<p>HD: %s</p>\n", $row[2])
			.sprintf("<p>CPU:%s</p>\n", $row[3])
			.sprintf("<p>VIDIO:%s</p>\n", str_replace('%', '%%',$row[4]))
			.sprintf("<p>DX: %s</p>\n", $row[5])
			.sprintf("<p>MEM:%s</p>\n", $row[6])
			.sprintf("<p>D3D Desc:%s</p>\n", $row[7])
			.sprintf("<p>D3D Mem:%s</p>\n", $row[8])
			.sprintf("</li>\n")
			.sprintf("</ol>\n");
	}
	printf($str);
}
ViewLog();
?>
</body>
</html>

