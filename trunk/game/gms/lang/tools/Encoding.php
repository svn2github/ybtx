<?php
function each_obj($src_file, $obj){
	if(is_array($obj)){
		foreach($obj as $item){
			each_obj($src_file, $item);
		}
	}elseif(is_string($obj)){
		fwrite($src_file, md5($obj)."\t".$obj."\n");
	}
}

function load_file($src_file, $filename){
	$file = file_get_contents($filename);
	require($filename);
	$index = 0;
	while($index = strpos($file,"\n\$",$index+1)){
		$stop = strpos($file,"=",$index);
		$obj = substr($file, $index, $stop-$index);
		eval("each_obj(\$src_file, ".$obj.");");
	}
}

function Main(){
	$src_file = fopen("../GMS_LANG_SRC_".date("Ymdhis").".txt","w");
	load_file($src_file, "../chinese_simple/sys_config.php");
	load_file($src_file, "../chinese_simple/page_localized.php");
	fclose($src_file);
}

Main();
?>