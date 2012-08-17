#!/usr/bin/php
<?php

require_once("../../conf/LogFree/config.php");
require_once("../../lib/LogFree/Lib.php");
function Deal($var)
{
	global $db_host, $db_user, $db_pass, $db_name;	
	$Conn = mysql_connect($db_host, $db_user, $db_pass);
	mysql_query("SET NAMES 'GBK'");
	mysql_select_db($db_name, $Conn);

	$error_code = mysql_escape_string($var["code"]);
	$error_owner = mysql_escape_string($var["owner"]);
	$error_status = mysql_escape_string($var["status"]);
	$error_desc = mysql_escape_string($var["log_desc"]);
	$peer = mysql_escape_string($var["peer"]);

	$qs = "update tbl_log_info set error_owner = '$error_owner', error_status = '$error_status', owner_desc = '$error_desc' where error_code = '$error_code'";
	mysql_query($qs);
	$relocate_str = sprintf("Location:/cgi-bin/LogFree/detail_%s.php?code=%s", $peer, $error_code);
	header($relocate_str);

/*
$str_js=<<<__str_js
<html>
<title>Msg</title>
<head>
<script type="text/javascript">
<!--//
function OnClose(){
        if (window.opener)
                window.opener.location.reload();
        window.close();

}
//-->
</script>
<body>
<H2>%s</H2>
<a href='#' onclick='OnClose()'>关闭</a>
</body>
</html>
__str_js;
        printf($str_js, "已经成功操作，请返回");	
*/
}

Deal($_GET);

?>
