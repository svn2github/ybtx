#!/usr/bin/php
<?php
header("content-type:text/html; charset=utf-8");
require_once("../../conf/gm_tools/Config.php");
require_once("InterFace.php");
require_once("../../conf/gm_tools/Config.php");
require_once("../../lang/gm_tools/".$sys_config["lang"]."/page_localized.php");

$gm_index=<<<_gm_index_src
<html>
<title>
Welcome
</title>
<head>
</head>
<body>
<table border=1>
<tr>
<td width="160" align="left" valign="top" style="font-size:12px; padding:20px;">
<p>%s</p>
%s

</td>
<td width="800">
<iframe name="frame1" id="frame1" width="1000" height="700"/>
</td>
</tr>
</table>
</body>
</html>
_gm_index_src;

function gm_index($var)
{
	$welcome="";
	global $action_page,$page_title;
	$UID = $var["UID"];
	$action_src = "";
	foreach($action_page as $key => $v){
		$action_src = $action_src.sprintf("<p><a href='ActionPage.php?name=%s?UID=%s&gameId=".$var["gameId"]."' target='frame1'>%s</a></p>", $key,$UID,$page_title[$key]);
	}

	global $gm_index;
	printf($gm_index, $welcome, $action_src);
	return true;
}

gm_index($_REQUEST);
?>
