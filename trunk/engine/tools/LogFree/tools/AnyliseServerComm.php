<?php
function ReadServerCommFile($filename, $network_flag)
{
	$fp = fopen($filename, 'r');
	if (!$fp){
		echo "$filename 文件不存在";
		return;
	}
	
	$server_guid = "";
	$server_name = "";
	$version = "";

	$head_info = array();
	$head_num = 0;
	while ($head_num < 3){
		$line = fgets($fp);
		$line = substr($line, 0, strlen($line) - 1);
		$pos = strpos($line, ":");
		$key = substr($line, 0, $pos);
		$val = substr($line, $pos + 1);
		$head_info[$key] = $val;
		$head_num ++;
	}

	$server_name = $head_info["Process Name"];
	$server_guid = $head_info["Process GUID"];
	$version = $head_info["Version"];

	InsertGuidInfo($server_guid, $server_name);	

	while(!feof($fp)){
		$content["UserName"] = "";
		$content["RoleName"] = "";
		$content["Client Addr"] = "";
		$content["Conn Guid"] = "";
		$content["msg"] = "";
		$line = fgets($fp);
		if (strlen($line) < 5)
			continue;
		$line = substr($line, 0, strlen($line) - 1);
		$content = array();
		$offset = 0;
		$index = 0;
		while ($pos = strpos($line, "\t", $offset)){
			if ($index < 3)
			{
				$content[$index] = substr($line, $offset, $pos-$offset);
			}
			else {
				$str = substr($line, $offset - 1);
				$p1 = 0;
				while ($p1 < strlen($str))
				{	$begin = strpos($str, "[", $p1); 
					if (!$begin){
						$offset = $begin;
						break;
					}
					$end = strpos($str, "]", $begin);
					if (!$end){
						$offset = $begin;
						break;
					}
					$pairs = substr($str, $begin+1 , $end-$begin-1);
					$code_pos = strpos($pairs, ":");
					$content[substr($pairs, 0, $code_pos)] = substr($pairs, $code_pos+1);
					$p1 = $end + 1;
				}
			}
			$offset = $pos + 1;
			$index ++;
		}
		$content["msg"] = substr($line, $offset);
		InsertServerLogCommon($content[0], $content[1], intval($content[2]), $content["UserName"], $content["RoleName"], $content["Client Addr"], $server_guid, $content["Conn Guid"], $content["msg"], $version, $network_flag);	
	}
}


function process_all_server_common($path, $network_flag){
	foreach(glob("$path/*.comm.log") as $filename)
	{
		ReadServerCommFile($filename, $network_flag);
	}
}

function ReadClientGuidFile($filename){
	$fp = fopen($filename, 'r');
	if (!$fp){
		echo "$filename 文件不存在";
		return;
	}
	$line = fgets($fp);
	$version = intval($line);
	
	while(!feof($fp)){
		$line = fgets($fp);
		$line = substr($line, 0, strlen($line) -1);
		$pos = strpos($line, "\t", 0);
		$guid = substr($line, 0, $pos);
		$time = substr($line, $pos+1);
		InsertClientAllGuid($version, $guid, $time);
	}			
}

function process_all_client_guid($path){
	foreach(glob("$path/ClientGuid_*.log") as $filename)
	{
		ReadClientGuidFile($filename);
	}
}

