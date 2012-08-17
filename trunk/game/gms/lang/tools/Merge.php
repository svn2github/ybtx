<?php
function each_obj($str_file, $obj, $filename, $lang){
	require("../chinese_simple/".$filename);
	require("../".$lang."/temp_".$filename);
	if(eval("return is_array(\$".$obj.");")){
		foreach(eval("return \$".$obj.";") as $key => $val){
			each_obj($str_file, $obj."[\"".$key."\"]",$filename,$lang);
		}
	}elseif(eval("return is_string(\$".$obj.");")){
		fwrite($str_file, md5(eval("return \$".$obj.";"))."\t");
		if(eval("return ((\$temp_".$obj." == \"\")?true:false);")){
			fwrite($str_file, eval("return \$".$obj.";")."\n");
		}else{
			fwrite($str_file, eval("return \$temp_".$obj.";")."\n");
		}
	}
}

function load_file($str_file, $lang, $filename){
	$temp_str = file_get_contents("../".$lang."/".$filename);
	$temp_file = fopen("../".$lang."/temp_".$filename, "w");
	$temp_str = str_replace("\n\$","\n\$temp_",$temp_str);
	fwrite($temp_file,$temp_str);
	fclose($temp_file);
	
	$str = file_get_contents("../chinese_simple/".$filename);
	while($index = strpos($str,"\n\$",$index+1)){
		$stop = strpos($str,"=",$index);
		$obj = substr($str, $index+2, $stop-$index-2);
		each_obj($str_file, $obj, $filename, $lang);
	}
}

function Main(){
	$lang = "english";
	$src_file = fopen("../GMS_LANG_SRC_".$lang."_".date("Ymdhis").".txt","w");
	load_file($src_file, $lang, "sys_config.php");
	load_file($src_file, $lang, "page_localized.php");
	fclose($src_file);
}

Main();
?>
