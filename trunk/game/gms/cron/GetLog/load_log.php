<?php

function LoadId($filename, $ServID)
{
	$fp = fopen($filename, 'r');
	if (!$fp){
		echo "$filename not exists";
		return false;
	}
	fgets($fp);
	while (!feof($fp)){
		$line = fgets($fp);
		$line = substr($line, 0, strlen($line)-1);
		$pieces = explode("\t", $line);
		mysql_query("insert into tbl_log_event values('".implode("','",$pieces)."',".$ServID.")");
	}
	fclose($fp);
	unlink($filename);
}


function LoadLog($filename, $ServID)
{
	$fp = fopen($filename, 'r');
	if (!$fp){
		echo "$filename not exists";
		return;
	}
	while (!feof($fp)){
		$line = fgets($fp);
		$line = substr($line, 0, strlen($line)-1);
		$pieces = explode("\t", $line);
		mysql_query("insert into ".$pieces[0]." values('".implode("','",array_splice($pieces,1))."',".$ServID.")");
	}
	fclose($fp);
	unlink($filename);
}

function MainFunc($path, $ServID)
{
	foreach(glob("$path/tbl_log_event*.txt") as $filename)
        {
                LoadId($filename, $ServID);
        }
	$files = glob("$path/*.log");
	while(count($files)>0){
		foreach(glob("$path/*.log") as $filename){
			foreach(glob("$path/tbl_log_event*.txt") as $filename){
				LoadId($filename, $ServID);
			}
			LoadLog($filename, $ServID);
		}
	$files = glob("$path/*.log");
	}
}


if (count($argv) < 3){
        $usage=<<<_usage_str
    usage: php load_log.php path_to_fold server_id
\n
_usage_str;
        echo $usage;

}
else{
	if($lock = fopen($path."/lock.lock","w")){
		flock($lock, LOCK_EX);
        	$path = $argv[1];
		$ServID = $argv[2];
		mysql_connect("127.0.0.1", "root", "1");
		mysql_select_db("ybtx_game_log");
		set_time_limit(0);
        	MainFunc($path, $ServID);
		mysql_close();
		flock($lock, LOCK_UN);
		fclose($lock);
	}
}

?>
