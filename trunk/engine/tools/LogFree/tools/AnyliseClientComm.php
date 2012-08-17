<?php
function ReadClientCommFile($filename, $network_flag)
{
	$fp = fopen($filename, 'r');
	if (!$fp){
		echo "$filename 文件不存在";
		return;
	}
	$line = "";	
	$head_info = array();
	$head_info["D3D Desc"] = "";
	$head_info["D3D Mem"] = "";
	while(!feof($fp))
	{
		$line = fgets($fp);
		$line = substr($line, 0, strlen($line) - 1);
		$pos = strpos($line, ":");
		if ($pos > 20)
			break;
		$key = substr($line, 0, $pos);
		$val = substr($line, $pos + 1);
		$head_info[$key] = $val;
	}
	
	InsertGuidInfo($head_info["Process GUID"], $head_info["Process Name"]);	
	if (array_key_exists('OS', $head_info)){
		InsertHarewareInfo(
			$head_info["Process GUID"], $head_info["OS"], 
			$head_info["HD"], $head_info["CPU"], 
			$head_info["VIDEO"], $head_info["DX"], 
			$head_info["MEM"], $head_info["D3D Desc"],
			$head_info["D3D Mem"]);
	}

	while(!feof($fp)){
		$code = "";
		$happened_time = "";
		$msg = "";
		$times = 1;
		if (strlen($line) < 5)
			continue;
		$line = substr($line, 0, strlen($line) - 1);
		$pos = strpos($line, ":");
		
		if ($pos > 20){
			$pos1 = strpos($line, "\t");
			$code = substr($line, 0, $pos1);
			$pos2 = strpos($line, "\t", $pos1 + 1);
			$happened_time = substr($line, $pos1+1, $pos2 - $pos1 -1);
			if ($pos3 = strpos($line, "\t", $pos2 + 1)){
				$times = intval(substr($line, $pos2+1, $pos3-$pos2));
				$msg = substr($line, $pos3);
			}
			else{
				$times = intval(substr($line, $pos2));
			}
		}
		else {
			$key = substr($line, 0, $pos);
			$val = substr($line, $pos + 1);
			$head_info[$key] = $val;
			if ($key == "Conn Guid")
			{
				InsertGuidInfo($val, $client_name);	
				$head_info["RoleName"] = "";
				$head_info["UserName"] = "";
			}
			$line = fgets($fp);
			continue;
		}
		InsertClientLogCommon($code, $happened_time, $times, $head_info["UserName"],
			$head_info["RoleName"], $head_info["Server Addr"], $head_info["Server GUID"],
			$head_info["Process GUID"], $head_info["Conn Guid"], $msg, $head_info["Version"], $network_flag);
		$line = fgets($fp);
	}
}


function process_all_client_common($path, $network_flag){
	foreach(glob("$path/*comm.log") as $filename)
	{
		ReadClientCommFile($filename, $network_flag);
	}
}

?>
