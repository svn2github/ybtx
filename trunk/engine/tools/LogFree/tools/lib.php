<?php

function InsertLogInfo($error_code, $error_title, $error_stack, $error_type, $error_extra)
{
	$qs = "insert ignore into tbl_log_info set error_code = '$error_code', error_title = '$error_title'";
	if ($error_stack != "")
	{
		$error_stack = mysql_escape_string($error_stack);
		$qs = $qs . ", error_stack = '$error_stack'";
	}

	if ($error_type != "")
		$error_type = mysql_escape_string($error_type);
	else
		$error_type = "³ÌÐò";
	$qs = $qs . ", error_type = '$error_type'";

	if ($error_stack && $error_stack != "")
	{
		$qs = $qs . ", error_extra = '$error_extra'";
	}

	mysql_query($qs);
}

function InsertGuidInfo($guid, $name)
{
	$guid = mysql_escape_string($guid);
	$name = mysql_escape_string($name);
	$qs = "insert ignore into tbl_guid_info set guid='$guid', name='$name'";
	mysql_query($qs);
}

function InsertClientLogCommon($error_code, $happened_time, $times, $urs, $role_name, $serverip, $server_guid, $client_guid, $conn_guid, $error_msg, $version, $network_flag)
{
	$error_code = mysql_escape_string($error_code);
	$happened_time = mysql_escape_string($happened_time);
	$client_guid = mysql_escape_string($client_guid);
	$error_msg = mysql_escape_string($error_msg);
	$version = intval($version);
	if ($version == 116885)
		return;

	$qs = "insert into tbl_client_log_common set error_code = '$error_code', happened_time = '$happened_time', times = '$times', client_guid='$client_guid', version='$version', network_flag='$network_flag'";
	if ($urs != "")
	{
		$urs = mysql_escape_string($urs);
		$qs = $qs . ", urs='$urs'";
	}
	if ($role_name != "")
	{
		$role_name = mysql_escape_string($role_name);
		$qs = $qs . ", role_name = '$role_name'";
	}
	if ($serverip != "")
	{
		$serverip = mysql_escape_string($serverip);
		$qs = $qs . ", serverip = '$serverip'";
	}
	if ($server_guid != "")
	{
		$server_guid = mysql_escape_string($server_guid);
		$qs = $qs . ", server_guid = '$server_guid'";
	}
	if ($conn_guid != "")
	{
		$conn_guid = mysql_escape_string($conn_guid);
		$qs = $qs . ", conn_guid = '$conn_guid'";
	}
	if ($error_msg != "")
	{
		$error_msg = mysql_escape_string($error_msg);
		$qs = $qs . ", log_msg='$error_msg'";
	}
	mysql_query($qs);
}

function InsertServerLogCommon($error_code, $happened_time, $times, $urs, $role_name, $remote_ip, $server_guid, $conn_guid, $error_msg, $version, $network_flag)
{
	$error_code = mysql_escape_string($error_code);
	$happened_time = mysql_escape_string($happened_time);
	$server_guid = mysql_escape_string($server_guid);
	$error_msg = mysql_escape_string($error_msg);
	$version = intval($version);

	$qs = "insert ignore into tbl_server_log_common set error_code = '$error_code', happened_time = '$happened_time', times = '$times', server_guid='$server_guid', version='$version', network_flag='$network_flag'";
	if ($urs != "")
	{
		$urs = mysql_escape_string($urs);
		$qs = $qs . ", urs='$urs'";
	}
	if ($role_name != "")
	{
		$role_name = mysql_escape_string($role_name);
		$qs = $qs . ", role_name='$role_name'";
	}
	if ($conn_guid != "")
	{
		$conn_guid = mysql_escape_string($conn_guid);
		$qs = $qs . ", conn_guid='$conn_guid'";
	}
	if ($remote_ip != "")
	{
		$remote_ip = mysql_escape_string($remote_ip);
		$qs = $qs . ", remote_ip='$remote_ip'";
	}
	if ($error_msg != "")
	{
		$error_msg = mysql_escape_string($error_msg);
		$qs = $qs . ", log_msg='$error_msg'";
	}
	mysql_query($qs);
}

function InsertHarewareInfo($guid, $os, $hd, $cpu, $video, $dx, $mem, $d3d_desc, $d3d_mem)
{
	$guid = mysql_escape_string($guid);
	$os = mysql_escape_string($os);
	$hd = mysql_escape_string($hd);
	$cpu = mysql_escape_string($cpu);
	$video = mysql_escape_string($video);
	$dx = mysql_escape_string($dx);
	$mem = mysql_escape_string($mem);
	$d3d_desc = mysql_escape_string($d3d_desc);
	$d3d_mem = mysql_escape_string($d3d_mem);
	
	$qs = "insert into tbl_hardware_info set guid = '$guid', os = '$os', hd = '$hd', cpu = '$cpu', video = '$video', dx = '$dx', mem = '$mem', D3D_Desc='$d3d_desc', D3D_Mem='$d3d_mem'";
	mysql_query($qs);
}

function InsertClientAllGuid($version, $client_guid, $happened_time){
	$version = intval($version);
	$client_guid = mysql_escape_string($client_guid);
	$happened_time = mysql_escape_string($happened_time);

	$qs = "insert into tbl_client_guid_info set version = '$version', client_guid = '$client_guid', happened_time = '$happened_time'";
	mysql_query($qs);
}
?>
