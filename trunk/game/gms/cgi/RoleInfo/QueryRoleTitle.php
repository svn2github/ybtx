<?php
function RetXML($role_id)
{	
        $str = "";	
	$str=$str.sprintf("<?xml version='1.0' encoding='utf-8'?>\n");	
	$str=$str.sprintf("<Response>");	
	$str=$str.sprintf("<result>1</result>");	
	$str=$str.sprintf("<title>无</title>");
 	$str=$str.sprintf("</Response>");

	$str=iconv("GB2312", "UTF-8", $str);



	printf($str);

}
function GetRoleTitle()
{	
	$role_id = $_REQUEST["Arg1"];
	RetXML($role_id);
}
function Real_GetRoleTitle()
{	
	$role_id = $_REQUEST["role_id"];
	RetXML($role_id);
}
if($_REQUEST["NoFromWebServer"])
	GetRoleTitle();
else	
	Real_GetRoleTitle();
?>
