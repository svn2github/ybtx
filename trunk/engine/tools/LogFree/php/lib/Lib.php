<?php
/*
客户端		
	error_code(超链接)	到相同error_code的客户端log
	error_title（超链接 到 详细）	显示：包含 error_stack, error_content
	client_ip（超链接）	根据client_guid查询  本客户端log
	server_ip（超链接）	根据server_guid查询  服务器log
	conn_guid（超链接)	根据conn_guid查询    本客户端对应服务器指定连接
	urs（超链接)	时间排序，指定urs的log
	role_name（超链接)	时间排序，指定role_name的log
	happened_time	
*/
function GetClientLogHTML($content)
{
/******************查询结果*******************
0: code
1: title
2: name
3: time
4: urs
5: role_name
6: server_ip
7: server_guid
8: client_guid
9: conn_guid
*********************************************/
	$str_code = "<a href='list_by_code_c.php?page=1&code=%s' target='_self'>%s</a>";
	$str_title = "<a href='detail_c.php?code=%s' target='_blank'>%s</a>";
	$str_client = "<a href='list_by_guid_c.php?page=1&guid=%s' target='_self'>%s</a>";
	$str_hardware_info = "<a href='show_hdware_info.php?guid=%s' target='_blank'>查看</a>";
	$str_server_all = "<a href='list_by_guid_s.php?page=1&guid=%s' target='_self'>查看对以服务器进程的log</a>";
	$str_server_conn = "<a href='list_by_conn_guid_s.php?page=1&guid=%s' target='_self'>查看log</a>";
	$str_user = "<a href='list_by_user_c.php?page=1&user=%s' target='_self'>%s</a>";
	$str_role = "<a href='list_by_role_c.php?page=1&role=%s' target='_self'>%s</a>";

	$str = sprintf("<table border=1>")
		.sprintf("<tr bgcolor='#9acd32'><th width='75'>%s</th><th width='200'>%s</th><th width='100'>%s</th><th width='100'>%s</th><th width='75'>%s</th><th width='50'>%s</th><th width='75'>%s</th><th width='100'>%s</th><th width='75'>%s</th><th width='150'>%s</th><th width=50>%s</th></tr>",
		"编码", "标题", "客户端", "服务器", "连接", "帐户", "角色", "发生时间", "硬件信息", "信息", "版本");
	
	for ($i = 0; $i < count($content); ++$i)
	{
		$color = "#ffffff";
		if ($i % 2)
			$color = "#d8d8d8";

		$row = $content[$i];
		$code = $row[0];
		$title = $row[1];
		$name = $row[2];
		$time = $row[3];
		$urs = $row[4];
		$role = $row[5];
		$ip = $row[6];
		$serv_guid = $row[7];
		$clnt_guid = $row[8];
		$conn_guid = $row[9];
		$error_msg = $row[10];
		$version = $row[11];
		$str = $str.sprintf("<tr bgcolor='%s'>\n", $color)
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_code, urlencode($code), $code).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_title, urlencode($code), str_replace('%', '%%', $title)).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_client, urlencode($clnt_guid), $name).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_server_all, urlencode($serv_guid)).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_server_conn, urlencode($conn_guid)).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_user, $urs, $urs).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_role, $role, $role).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", $time)
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_hardware_info, urlencode($clnt_guid)).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", str_replace('%', '%%', $error_msg))
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", $version)
			.sprintf("</tr>\n");
	
	}

	$str = $str.sprintf("</table>");
	return $str;
}

/*****************************************
服务器		
	error_code（超链接)	到相同error_code的客户端log
	error_title（超链接 到 详细）	显示：包含 error_stack, error_content
	server_name（超链接)	根据server_guid查询这个进程的所有log
	client_ip（超链接)	根据conn_guid查询    对应客户端
	urs（超链接)	时间排序，指定urs的log
	role_name（超链接)	时间排序，指定role_name的log
	happened_time	

*****************************************/
function GetServerLogHTML($content)
{
/******************查询结果*******************
0: code
1: title
2: name
3: time
4: urs
5: role_name
6: client_ip
7: server_guid
8: conn_guid
*********************************************/
	$str_code = "<a href='list_by_code_s.php?page=1&code=%s' target='_self'>%s</a>";
	$str_title = "<a href='detail_s.php?code=%s' target='_blank'>%s</a>";
	$str_server = "<a href='list_by_guid_s.php?page=1&guid=%s' target='_self'>%s</a>";
	$str_client_conn = "<a href='list_by_conn_guid_c.php?page=1&guid=%s' target='_self'>%s</a>";
	$str_user = "<a href='list_by_user_s.php?page=1&user=%s' target='_self'>%s</a>";
	$str_role = "<a href='list_by_role_s.php?page=1&role=%s' target='_self'>%s</a>";

	$str = "";
	$str = sprintf("<table border=1>")
		.sprintf("<tr bgcolor='#9acd32'><th width='75'>%s</th><th width='150'>%s</th><th width='150'>%s</th><th width='150'>%s</th><th width='75'>%s</th><th width='70'>%s</th><th width='120'>%s</th><th width='150'>%s</th><th width=50>%s</th></tr>",
		"编码", "标题", "服务器名", "客户端", "帐户", "角色", "发生时间", "错误信息信息", "版本");
	
	for ($i = 0; $i < count($content); ++$i)
	{
		$color = "#ffffff";
		if ($i % 2)
			$color = "#d8d8d8";

		$row = $content[$i];

		$code = $row[0];
		$title = $row[1];
		$name = $row[2];
		$time = $row[3];
		$urs = $row[4];
		$role = $row[5];
		$ip = $row[6];
		$serv_guid = $row[7];
		$conn_guid = $row[8];
		$error_msg = $row[9];
		$version = $row[10];
		$str = $str.sprintf("<tr bgcolor='%s'>\n", $color)
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_code, urlencode($code), $code).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_title, urlencode($code), str_replace('%', '%%', $title)).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_server, urlencode($serv_guid), $name).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_client_conn, urlencode($conn_guid), $ip).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_user, $urs, $urs).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;").sprintf($str_role, $role, $role).sprintf("</td>\n")
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", $time)
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", str_replace('%','%%',$error_msg))
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", $version)
			.sprintf("</tr>\n");
	
	}
	$str = $str.sprintf("</table>");
	return $str;
}

function GetUserLogHTML($content)
{
/******************查询结果*******************
0: code
1: title
2: name
3: time
4: urs
5: role_name
6: client_ip
7: server_guid
8: conn_guid
*********************************************/
	$str_code = "<a href='list_by_code_s.php?page=1&code=%s' target='_self'>%s</a>";
	$str_title = "<a href='detail_s.php?code=%s' target='_blank'>%s</a>";
	$str_server = "<a href='list_by_guid_s.php?page=1&guid=%s' target='_self'>%s</a>";
	$str_client_conn = "<a href='list_by_conn_guid_c.php?page=1&guid=%s' target='_self'>%s</a>";
	$str_user = "<a href='list_by_user_s.php?page=1&user=%s' target='_self'>%s</a>";
	$str_role = "<a href='list_by_role_s.php?page=1&role=%s' target='_self'>%s</a>";

	$str = "";
	$str = sprintf("<table border=1 align='center'>")
		.sprintf("<tr bgcolor='#d8d8d8'><th width='350'>%s</th><th width='100'>%s</th><th width='150'>%s</th><th width='75'>%s</th><th width='75'>%s</th><th width='170'>%s</th><th width='50'>%s</th><th width=50>%s</th></tr>",
		"content", "server", "client", "account", "role", "time", "type", "version");
	
	for ($i = 0; $i < count($content); ++$i)
	{
		$color = "#ffffff";
		if ($i % 2)
			$color = "#d8d8d8";

		$row = $content[$i];

		$code = $row[0];
		$title = $row[1];
		$name = $row[2];
		$time = $row[3];
		$urs = $row[4];
		$role = $row[5];
		$ip = $row[6];
		$serv_guid = $row[7];
		$conn_guid = $row[8];
		$error_msg = $row[9];
		$version = $row[10];
		$str = $str.sprintf("<tr bgcolor='%s'>\n", $color)
			.sprintf("\t<td align='left'>&nbsp;%s</td>\n", str_replace('%','%%',$error_msg))
			.sprintf("\t<td align='center'>&nbsp;$name</td>\n")
			.sprintf("\t<td align='center'>&nbsp;$ip</td>\n")
			.sprintf("\t<td align='center'>&nbsp;$urs</td>\n")
			.sprintf("\t<td align='center'>&nbsp;$role</td>\n")
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", $time)
			.sprintf("\t<td align='center'>&nbsp;$title</td>\n")
			.sprintf("\t<td align='center'>&nbsp;%s</td>\n", $version)
			.sprintf("</tr>\n");
	
	}
	$str = $str.sprintf("</table>");
	return $str;
}
function GetNetFlag($cnf)
{
	if($cnf == 1)
		return "<font color='#0A14D3'><b>内网</b></font>";
	else if($cnf == 2)
		return "<font color='#066016'><b>外网</b></font>";
	else if ($cnf == 3)
		return "<b>内外网</b>";
	else 
		return "<font color='#FF0000'><b>未定义</b></font>";
}

function GetOwner($who, $url)
{
	$qs = "select distinct error_owner from tbl_log_info order by error_owner";
	$result = mysql_query($qs);
	$str = sprintf("<select id='who' name='who' OnChange='ReflushUrl(\"$url\")'>");
	$str .= sprintf("<option value=''>查看全部</option>");
	while ($row = mysql_fetch_row($result)){
		if ($row[0] == $who)
		{
			$str .= sprintf("<option value='%s' selected>%s</option>", $row[0], $row[0]);	
		}
		else
			$str .= sprintf("<option value='%s' >%s</option>", $row[0], $row[0]);	
	}
	$str .= "</select>";
	return $str;
}

function GetVersion($flag, $version, $url)
{
	$qs = "";
	if ($flag == "s")
		$qs = "select distinct version from tbl_server_log_common order by version desc";
	else if ($flag == "c")
		$qs = "select distinct version from tbl_client_log_common order by version desc";

	$result = mysql_query($qs);
	$str = sprintf("<select id='version' name='version' OnChange='ReflushUrlVersion(\"$url\")'>");
	$str .= sprintf("<option value=''>查看全部</option>");
	while ($row = mysql_fetch_row($result)){
		if ($row[0] == $version)
		{
			$str .= sprintf("<option value='%s' selected>%s</option>", $row[0], $row[0]);	
		}
		else
			$str .= sprintf("<option value='%s' >%s</option>", $row[0], $row[0]);	
	}
	$str .= "</select>";
	return $str;
}

function GetNetFlagTitle($nf, $url)
{
	$tbl_place = array ("全部","内网", "外网");
	$str = "<select id='nf' name='nf' OnChange='ReflushUrlNF(\"$url\")'>\n";
	for ($i = 0; $i < count($tbl_place); ++$i){
		if ($i == $nf)
			$str .= "<option value=$i selected>" . $tbl_place[$i] . "</option>";
		else
			$str .= "<option value=$i >" . $tbl_place[$i] . "</option>";

	}
	$str .= "</select>";
	return $str;
}

function WriteOwner($owner)
{
	if ($owner == "未指派")
		return "<font color='#FF0000'><b>未指派</b></font>";
	return $owner;
}

function GetStatus($status)
{
	if ($status == 0)
		return "未处理";
	if ($status == 1)
		return "其他bug导致";
	if ($status == 2)
		return "已处理完";
	if ($status == 3)
		return "观察中";
}
?>
