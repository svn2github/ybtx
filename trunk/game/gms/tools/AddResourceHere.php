#!/usr/bin/php
<?php
header("Content-type:text/plain; charset=utf-8");
require_once("../../lib/gm_tools/Auth.php");
require_once("../../lib/gm_tools/Lib.php");
require_once("../../conf/gm_tools/Config.php");
require_once("../../cgi-bin/gm_tools/InterFace.php");
require_once("../../lang/gm_tools/".$sys_config["lang"]."/page_localized.php");
function Update($var){
	global $sys_config,$page_title,$action_page;
	$key = $sys_config["game_key"];
	$gameid =intval($var["gameId"]);
	$url = $var["url"];
	$gms = $var["gms"];
	$sign = md5("$gameid$key");
 
	$res = "1,外部功能,,,0,0,0;\n";
	$i = 2;
	$count = count($action_page)+1;
	foreach($action_page as $key => $v){
		$res.= $i.",".$page_title[$key].",".$key.",http://$gms/cgi-bin/gm_tools/ActionPage.php?name=".$key.",1,1,".$i++;
		if($i<=$count) $res.= ";\n";
	}

	$param = array();
	$param["gameId"] = $gameid;
	$param["resources"] = $res;
	$param["sign"] = $sign;
 
	$ret=PostDataToUrl($url, $param);
	$xml_item = new DOMDocument();
	$xml_item->loadXML($ret);
	echo $xml_item->getElementsByTagName("Response")->item(0)->getElementsByTagName("result")->item(0)->nodeValue;
}
Update($_POST);
?>
