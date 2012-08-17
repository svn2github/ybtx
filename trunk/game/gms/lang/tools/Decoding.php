<?php
function each_obj(&$file, $src, $obj){
	if(is_array($obj)){
		foreach($obj as $item){
			each_obj($file, $src, $item);
		}
	}elseif(is_string($obj)){
		if($obj<>"" && $src[md5($obj)]<>""){
			$new = str_replace("\$","\\\$",$src[md5($obj)]);
			$obj = str_replace("\$","\\\$",$obj);
			if(strrpos($new,"\n") == strlen($new)-1){
				$new = substr($new, 0, strlen($new)-1);
			}
			$index = strpos($file,"\"".$obj."\"");
			if(!$index) $index = strpos($file,"'".$obj."'");
			if(!$index) $index = strpos($file,$obj);
			else $index+=1;
			$file = substr_replace($file, $new,$index, strlen($obj));
		}
	}
}

function load_file($lang, $src, $filename){
	$w_file = fopen("../".$lang."/".$filename, "w");
	
	$file = file_get_contents("../chinese_simple/".$filename);
	$new_file = $file;
	require("../chinese_simple/".$filename);
	$index = 0;
	while($index = strpos($file,"\n\$",$index+1)){
		$stop = strpos($file,"=",$index);
		$obj = substr($file, $index, $stop-$index);
		eval("each_obj(\$new_file, \$src, ".$obj.");");
	}
	fwrite($w_file, $new_file);
	fclose($w_file);
}

function Main(){
	$lang = "english";
	$filename = "GMS_LANG_SRC_english_20110816075748.txt";
	
	$src_file = fopen($filename, "r");
	$src = array();
	while(! feof($src_file)){
		$str = fgets($src_file);
		$src[substr($str,0,32)] = substr($str,33);
	}
	
	load_file($lang, $src, "sys_config.php");
	load_file($lang, $src, "page_localized.php");
}

Main();
?>