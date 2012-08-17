<?php
function PrintNotice($msg)
{
	$str = "";
	$str = $str.sprintf("<div class='notice'>%s</div>\n",$msg);
  echo $str;
}

function PostDataToUrl($url, $data) {
	$url = parse_url($url);
	if (!$url) return "couldn't parse url";
	if (!isset($url['port'])) { $url['port'] = ""; }
	if (!isset($url['query'])) { $url['query'] = ""; }
	$encoded = "";
	while (list($k,$v) = each($data)) {
		$encoded .= ($encoded ? "&" : "");
		$encoded .= $k."=".$v;
	}
	$fp = fsockopen($url['host'], $url['port'] ? $url['port'] : 80);
	if (!$fp) return "Failed to open socket to url[host]";
	$str_post = sprintf("POST %s%s%s HTTP/1.0\n", $url['path'], $url['query'] ? "?" : "", $url['query']);
	fputs($fp, $str_post);
	fputs($fp, "Host: ".$url[host]."\n");
	fputs($fp, "Content-type: application/x-www-form-urlencoded\n");
	fputs($fp, "Content-length: " . strlen($encoded) . "\n");
	fputs($fp, "Connection: close\n\n");
	fputs($fp, "$encoded\n");
	$line = fgets($fp,1024);
	//if (!eregi("^HTTP/1\.. 200", $line)) return;
	$results = ""; $inheader = 1;
	while(!feof($fp)) {
		$line = fgets($fp,1024);
		if ($inheader && ($line == "\n" || $line == "\r\n")) {
			$inheader = 0;
		}
		elseif (!$inheader) {
			$results .= $line;
		}
	}
	fclose($fp);
	return $results;
}

function PostData($host, $port, $action, $var)
{
	global $tbl_error_msg;
	$flag = false;
	$hasr = false;
	$params = "";
	$max_len = 540000;//3500;
	foreach($var as $key=>$value){
		if ($key != "role_names"){
			if ($flag)
				$params.="&";
			$params.=$key."=";
			$params.=urlencode($value);
			$flag = true;
		}else{
			$hasr = true;
		}
	}
	$length=strlen(urlencode($params));
	if($length > $max_len){
		PrintNotice($tbl_error_msg["MsgTooLang"]);
		return false;
	}
	$role_name = array();
	if($hasr){
		if ($flag)
			$params.="&";
		$value = $var["role_names"];
		array_push($role_name, $value);
		while(strlen(urlencode($role_name[count($role_name)-1]))>($max_len-$length)){
			$t1 = urldecode(substr(urlencode($role_name[count($role_name)-1]),0,($max_len-$length)));
			$index = strrpos($t1,",");
			$t2 = substr($role_name[count($role_name)-1],0,$index);
			$t3 = substr($role_name[count($role_name)-1],$index+1);
			$role_name[count($role_name)-1] = $params."role_names=".urlencode($t2);
			array_push($role_name, $t3);
		}
		$role_name[count($role_name)-1] = $params."role_names=".urlencode($role_name[count($role_name)-1]);
	}else{
		array_push($role_name, $params);
	}
	
	$content = "";
	foreach($role_name as $params){
		$length = strlen($params);
		$header = "POST /".$action." HTTP/1.1\r\n";
		$header .= "Content-Type:application/x-www-form-urlencoded\r\n";
		$header .= "Content-Length: ".$length."\r\n";
		$header .= "Connection: connect\r\n\r\n";
		$header .= $params."\r\n";
		$fp = @fsockopen($host, $port, $errno,$errstr, 10) or printf("<div class='notice'><error>".$tbl_error_msg["PostDataFail"]."</error></div>",Local2UTF8($errstr)."--->".$errno);
		if($fp!=null){
			fputs($fp, $header);
			$inheader = 1;
			
			$read_size = 0;
			$content_size = 0;
			$total_size = 0;
			while (!feof($fp)){
				$line = fgets($fp, 512);
				$total_size += strlen($line);
				if ($inheader && ($line == "\n" || $line == "\r\n")) $inheader = 0;
		
				if (!$inheader){
					$content.=$line;
					$read_size += strlen($line);
					if ($read_size >= $content_size) break;
				}else{
					if (substr($line,0,14) == 'Content-length'){
						$content_size = substr($line, 15);
						$content_size = intval($content_size);
					}
				}
			}
			fclose($fp);
		}
	}
	return $content;
}
function SetCommonVar($var)
{
	$str = sprintf("<input type='hidden' name='UID' value='%s' />", $var["UID"]);
	if (array_key_exists('serverid', $var))
		$str = $str.sprintf("<input type='hidden' name='serverid' value='%s' />", $var['serverid']);
	return $str;
}

function LogGMCommand($gm, $action, $reason, $server_id)
{
	$action = mysql_escape_string($action);
	$gm = mysql_escape_string($gm);
	$reason = mysql_escape_string(ParamEncode($reason));
	
	$qs="insert into `tbl_gm_command_log`(`gcl_sGmName`, `gcl_fAction`, `gcl_sReason`, `gcl_uServerId`) values ('$gm',$action,'$reason','$server_id')";
	$result = MySQLQuery($qs, "GMS");
	if (!$result){
		return false;
	}
	return true;
}

function SaveReqAction($gm, $action_id, $role_name, $serv_id, $arg1, $arg2, $reason)
{
	$gm = mysql_escape_string($gm);
	$role_name = mysql_escape_string($role_name);
	$arg1 = mysql_escape_string($arg1);
	$arg2 = mysql_escape_string($arg2);
	$reason = mysql_escape_string(ParamEncode($reason));

	$qs="insert into tbl_gm_compensate_action set gm = '$gm', action_id = '$action_id', role_name='$role_name', arg1='$arg1', arg2='$arg2', reason='$reason', server_id='$serv_id'";

	$result = MySQLQuery($qs, "GMS");
	if (!$result){
		return false;
	}
	return true;
}

function CreateSession($gm)
{
	$sesskey="";
	return $sesskey;
}

function BackAndClose(){
	global $tbl_sys_msg;
	echo "<table class='list_table'><tr><td><a href='#' onclick='javascript:window.close();'>".$tbl_sys_msg["close"]."</a></td></tr></table>";
}

function BackAndRefresh()
{
$str_js=<<<__str_js
<script type="text/javascript">
<!--//
function OnClose(){
	if (window.opener)
		//window.opener.location.reload();
	window.close();

}
//-->
</script>
__str_js;
	printf($str_js);
	global $tbl_sys_msg;
	echo "<table class='list_table'><tr><td><a href='#' onclick='OnClose()'>".$tbl_sys_msg["close"]."</a></td></tr></table>";
}

function ReturnAndBack(){
	global $tbl_sys_msg;
	echo "<table class='list_table'><tr><td><a href='javascript:history.back(1)'>[".$tbl_sys_msg["return"]."]</a></td></tr></table>";
}

function Reload(){
	global $tbl_sys_msg;
	echo "<table class='list_table'><tr><td><a href='javascript:location.reload()'>[".$tbl_sys_msg["refurbish"]."]</a></td></tr></table>\n";
}

function GetItemInfo($info, &$item_type, &$item_name)
{
	$pos = strpos($info, "_");
	$item_type = intval(substr($info,0, $pos));
	$item_name = substr($info, $pos+1);	
}

function DownloadInfo($title, $content, $filename)
{
	printf(header("content-disposition", "attachment; filename=\"$filename\""));
	for($i = 0; $i < count($title); ++$i){
		printf("%s\t", $title[$i]);
	printf("\n");
 	}
	for ($i = 0; $i < count($content); ++$i){
		for ($j = 0; $j < count($content[$i]); ++$j)
			printf("%s\t", $content[$i][$j]);
		printf("\n");
	}
}

function Local2UTF8($var){
	global $sys_config;
	return iconv($sys_config["locale"], "utf-8", $var);
}

function UTF82Local($var){
	global $sys_config;
	return iconv("utf-8", $sys_config["locale"], $var);
}

function Local2Base64($var){
	$temp = base64_encode($var);
	$temp = str_replace("/","-",$temp);
	$temp = str_replace("=","",$temp);
	return $temp;
}

function UTF82Base64($var){
	return Local2Base64(UTF82Local($var));
}

function Base642Local($var){
	$temp = str_replace("-","/",$var);
	$temp = base64_decode($temp);
	return $temp;
}

function Base642UTF8($var){
	return Local2UTF8(Base642Local($var));
}

function IsDownload($var){
	if (!isset($var["download"]))
		return false;
	if ($var["download"] == "yes")
		return true;
	return false;
}

function OutDataToFile($tbl_content, $tbl_title){
	for ($i = 0; $i < count($tbl_content); ++$i){
		for ($j = 0; $j < count($tbl_content[$i]); ++$j){
			echo $tbl_content[$i][$j] . ",";
		}
		echo "\n";
	}
}

function CreateTable($tbl_content, $tbl_title){
	$str = "<table border=1>\n";
	$str .= "<tr>";
	for ($i = 0; $i < count($tbl_title); ++$i){
		$str .= "<th>".$tbl_title[$i]."</th>";
	}
	$str .= "</tr>\n";
	
	for ($i = 0; $i < count($tbl_content); ++$i){
		$str .= "<tr>";
		for ($k = 0; $k < count($tbl_content[$i]); ++$k){
			$str .= "<td>" . $tbl_content[$i][$k] . "</td>";
		}
		$str .= "</tr>\n";
	}
	$str .= "</table>";
	return $str;
}

function WriteGrowth($before, $end){
	if ($before == 0)
		return 100;	
	return ($end - $before) * 100 / $before;
}

function join_server_id($serverid)
{
	$str = "";
	$flag = false;	
	foreach ($serverid as $key => $value){
		if ($flag){
			$str .= ",";
		}
		$str .= $value;
		$flag = true;
	}
	return $str;
}

function SendWarningFun($serverid,$role_name,$warning){
	$serverid =intval($serverid);
	$warning=mysql_escape_string($warning);
	
	$param = array();
	$param["warning"] = $warning;
	$param["role_name"] = $role_name;
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "SendWarning", $param);
	return $ret;
}

function SendMailFun($serverid, $role_name, $content, $items, $money_num ){
	$serverid 	= intval($serverid);
	$role_names	= mysql_escape_string($role_name);
	$content		= mysql_escape_string($content);
	$item_info			= mysql_escape_string($items);
	$money	= intval($money_num);
	
	global $tbl_game_server;
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1];
	
	$items = explode(";",$item_info);
	$count = count($items)-1;
	$item_temp = "";
	for($i = 0; $i < $count; $i++){
		$item_temp .= $items[$i].";";
		if($i%4 == 3 || ($i+1)==$count){
			$param = array();
			$param["role_names"] = $role_names;
			$param["content"] = $content;
			$param["item_info"] = UTF82Local($item_temp);
			$param["money"] = $money;
			
			$ret = PostData($gas_host, $gas_port, "SendMail", $param);
			
			$item_temp = "";
			$money = 0;
		}
	}
	return $ret;
}

function page_default_func($row){
	return $row;
}

function page($var,$query,$action_id,$database,$serverid,&$result,&$str,&$desc,$func="page_default_func",$title="file",$th=array()){
	global $sys_config,$tbl_page;
	if($var["page_field"]!=""){
		$filter = " ".$var["page_field"]."='".$var["page_value"]."'";
		if($index = stripos($query," where ")){
			$index += 6;
			$query = substr($query,0,$index).$filter." and".substr($query,$index);
		}else{
			$length = strlen($query);
			$index1 = (($index1 = stripos($query," order by ")))?$index1:$length;
			$index2 = (($index2 = stripos($query," group by ")))?$index2:$length;
			$index3 = min($index1,$index2);
			$query = substr($query,0,$index3)." where".$filter.substr($query,$index3);
		}
	}
	if($var["download"]=="true"){
		$tname = gmmktime();
		$filename = "../../".$tname.".csv";
		$file = fopen($filename,"w");
		
		$result = MySQLQuery($query, $database, $serverid);
		
		$count = count($th);
		if($count==0){
			while($row = mysql_fetch_field($result))
				fwrite($file, "\"".mysql_field_name($result,$count++)."\",");
			fwrite($file, "\n");
		}else
			fwrite($file, UTF82Local("\"".implode("\",\"",$th)."\"\n"));
		
		while($row = mysql_fetch_row($result)){
			$row = $func($row);
			for($i = 0; $i < $count; $i++)
				fwrite($file,UTF82Local("\"".$row[$i]."\","));
			fwrite($file, "\n");
		}
		fclose($file);
		
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$tname&display=$title';\n";
		echo "</script>\n";
	}else{
		$page_size = ($var["page_size"]=="")?$sys_config["page_size"]:$var["page_size"];
		$page = ($var["page"]=="")?1:$var["page"];
		$prev = ($page-1) * $page_size;
		$limit = "limit ".$prev.",".$page_size;
		
		$query = "SELECT SQL_CALC_FOUND_ROWS ".substr($query, 7)." ".$limit;
		$result = MySQLQuery($query, $database, $serverid);
		$count = reset(mysql_fetch_row(MySQLQuery("SELECT FOUND_ROWS()",$database,$serverid)));
		
		$desc = "";
		
		$page_num = ceil($count/$page_size);
		$fun = "<select onchange='topage(this)'>\n";
		for($i=1;$i<=min($page_num,$sys_config["show_page"]);$i++)
			$fun.="<option".(($i==$page)?" selected='selected'":"").">".$i."</option>\n";
		$fun.= "</select>\n";
		$fun.= "<input type='hidden' name='url' value='".get_http_args()."' disabled=\"disabled\"/>\n";
		$str = sprintf("<table class='list_table'><tr><td style='text-align:center;'>".$tbl_page[0].$tbl_page[1].$tbl_page[2]."</tr></td></table>",$fun,$page_num,$count,$page_size);
		$str.= "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>".$tbl_page[3]."</a></td></tr></table>\n";
	}
}

function get_http_args($arg=null){
	if($arg==null){
		$arg = $_REQUEST;
	}
	$uri = $_SERVER['PHP_SELF'];
	$request = "";
	foreach($arg as $key => $value){
		if($request == ""){
			$request .= "?";
		}else{
			$request .= "&";
		}
		if(is_array($value)){
			foreach($value as $v){
				if($request == ""){
					$request .= "?";
				}else{
					$request .= "&";
				}
				$request .= $key."[]=".$v;
			}
		}else{
			$request .= $key."=".$value;
		}
	}
	$uri.=$request;
	return $uri;
}

function page_filter($field, $th_id, $div_id, $query, $database, $serverid, $fun = null){
	global $tbl_sys_msg;
	$length = strlen($query);
	$index1 = (($index1 = stripos($query," order by ")))?$index1:$length;
	$index2 = (($index2 = stripos($query," group by ")))?$index2:$length;
	$index3 = min($index1,$index2);
	$form_markup = stripos($query," from ");
	$query = "select ".$field.substr($query,$form_markup,$index3-$form_markup)." group by ".$field;
	$rs1 = MySQLQuery($query, $database, $serverid);
	$str = "<div class='filter' style='display:none;'>";
	$str.= "<a href='#' onclick=\"toNoFilter(this)\">".$tbl_sys_msg["all"]."</a><hr/>";
	while($rw1 = mysql_fetch_row($rs1)){
		$str.= "<a href='#' onclick=\"toFilter(this,'".$field."','".$rw1[0]."')\">";
		$str.= ($fun == null)?$rw1[0]:$fun($rw1[0]);
		$str.= "</a><br/>";
	}
	$str.= "</div>";
	
	$str.= '<script type="text/javascript">'."\n";
	$str.= '$("#div_'.$div_id.' .'.$th_id.'").css("cursor","pointer");'."\n";
	$str.= '$("#div_'.$div_id.' .'.$th_id.'").hover('."\n";
	$str.= '	function(){'."\n";
	$str.= '		$(this).find("div.filter").slideDown()'."\n";
	$str.= '	},'."\n";
	$str.= '	function(){'."\n";
	$str.= '		$(this).find("div.filter").hide()'."\n";
	$str.= '	}'."\n";
	$str.= ');'."\n";
	$str.= '</script>'."\n";
	return $str;
}

function ServerList(){
	$rand = rand();
	$ret = "";
	global $tbl_sys_msg;
	global $tbl_serv_partition,$tbl_game_server;
	$ret.= "<input type='checkbox' id='server".$rand."_all' class='checkbox_all' /><label for='server".$rand."_all'>".$tbl_sys_msg["all_server"]."</label><br/>\n";
	$groups = $tbl_serv_partition[$_REQUEST["gameId"]];
	$i = 0;
	if(is_array($groups)){
		foreach($groups as $group_name => $group){
			$ret.= "<div class='server_partition'><input type='checkbox' id='server".$rand."_".$i."_all' class='checkbox_all server".$rand."_checkbox' /><label for='server".$rand."_".$i."_all'>".$group_name."</label></div>\n";
			if(is_array($group)){
				foreach($group as $serverid){
					$ret.= "<div class='server_item'><input type='checkbox' id='server_".$rand."_".$serverid."' name='server_id[]' class='server".$rand."_checkbox server".$rand."_".$i."_checkbox' value='".$serverid."' /><label for='server_".$rand."_".$serverid."'>".$tbl_game_server[$serverid][2]."</label></div>\n";
				}
			}
			$i++;
		}
	}
	return $ret;
}

function eachServer($var, $gm, $fun){
	global $tbl_game_server;
	$server_array = $var["server_id"];
	if($server_array != null && count($server_array)>0) {
		for($i=0;$i<count($server_array);$i++){
			echo "<h2>".$tbl_game_server[intval($server_array[$i])][2].":</h2>\n";
			$serverid = $server_array[$i];
			$ret = $fun($var,$gm,$serverid);
		}
	}elseif($var["serverid"]!=null && $var["serverid"]!="") {
			echo "<h2>".$tbl_game_server[intval($var["serverid"])][2].":</h2>\n";
			$serverid = $var["serverid"];
			$ret = $fun($var,$gm,$serverid);
	}
	return true;
}

function eachServerFrame($var, $gm){
	global $tbl_game_server;
	$server_array = $var["server_id"];
	if($server_array != null && count($server_array)>0) {

		$init_script = "";
		$init_divs = "";
		for($i=0;$i<count($server_array);$i++){
			echo "<h2>".$tbl_game_server[$server_array[$i]][2].":</h2>\n";
			echo "<div class='panel' id='div_".$server_array[$i]."'>Loading...</div>\n";
			$init_script.= '	$.ajax({'."\n";
			$init_script.= '		type:"get",'."\n";
			$init_script.= '		url:"'.get_http_args($var).'",'."\n";
			$init_script.= '		data:"serverindex='.$server_array[$i].'&rand='.rand(0,100).'",'."\n";
			$init_script.= '		success:function(msg){'."\n";
			$init_script.= '			$("#div_'.$server_array[$i].'").html(msg);'."\n";
			$init_script.= '		}'."\n";
			$init_script.= '	});'."\n";
		}
		echo "<script>\n";
		echo "$(function(){\n";
		echo $init_script;
		echo "});\n";
		echo "</script>\n";
	}elseif($var["serverid"]!=null && $var["serverid"]!=""){
		echo "<h2>".$tbl_game_server[$var["serverid"]][2].":</h2>\n";
		echo "<div class='panel' id='div_".$var["serverid"]."'>Loading...</div>\n";
		echo "<script>\n";
		echo "$(function(){\n";
		echo '	$.ajax({'."\n";
		echo '		type:"get",'."\n";
		echo '		url:"'.get_http_args($var).'",'."\n";
		echo '		data:"serverindex='.$var["serverid"].'&rank='.rand(0,100).'",'."\n";
		echo '		success:function(msg){'."\n";
		echo '			$("#div_'.$var["serverid"].'").html(msg);'."\n";
		echo '		}'."\n";
		echo '	});'."\n";
		echo "});\n";
		echo "</script>\n";
	}
	return true;
}

function form($var, $gm){
	echo "<form method='post' name='form1' action='/cgi-bin/gm_tools/FrameWork.php'>\n";
	echo "<input type='hidden' name='UID' value='".$var["UID"]."'/>\n";
	echo "<input type='hidden' name='gameId' value='".$var["gameId"]."'/>\n";
	echo "<input type='hidden' name='serverid' value='".$var["serverid"]."'/>\n";
	echo "<input type='hidden' name='action_name'/>\n";
}

function checkbox($user, $role){
	if($user || $role){
		global $tbl_sys_msg;
		echo "<table class='form_table'>\n";
		echo "	<tr>\n";
		echo "		<th>".$tbl_sys_msg["selected"]."</th>\n";
		echo "		<td style='padding:0px;'><textarea style='width:100%;margin:0px;border:0px;' name='selected_str'></textarea></td>\n";
		echo "	</tr>\n";
		echo "</table>\n";
	}
}

function RoleFunction($role_name,$serverid){
	return "<input type='checkbox' flag='role' value='$role_name' onclick='Checkbox(this)' /><a href='#' onclick=\"role_detail('$role_name','$serverid')\">$role_name</a>";
}

function ParamEncode($params){
	if(is_array($params)){
		$temp = array();
		foreach($params as $key => $value){
			array_push($temp, $key."=".$value);
		}
		return implode("&", $temp);
	}elseif(is_string($params)){
		return $params;
	}
	return "";
}

function ParamDecode($params){
	$ret = array();
	$temp = explode("&",$params);
	foreach($temp as $param){
		$p = explode("=",$param);
		$ret[$p[0]] = $p[1];
	}
	return $ret;
}

function LogInfo($date,$succ = null,$ret = ""){
	global $tbl_sys_msg,$page_main;
	$str = "";
	if(gettype($succ) == "boolean"){
		if($succ == true){
			global $tbl_sys_msg;
			$str = $tbl_sys_msg["success"]."<hr/>";
		}else{
			global $tbl_error_msg;
			$str = $tbl_error_msg["ActionFail"]."<hr/>";
		}
	}
	if(is_string($date)){
		$date = ParamDecode($date);
	}
	foreach($date as $key => $value){
		$field = $page_main[$key];
		if($field == ""){
			$field = $tbl_sys_msg[$key];
		}
		if($field == ""){
			$field = $key;
		}
		$str.=$field.": ".$value."<br/>";
	}
	while(($i1 = strpos($str,"#["))){
		$i2 = strpos($str,"[",$i1+2);
		$i3 = strpos($str,"]]#",$i2);
		$type = substr($str,$i1+2,$i2-$i1-2);
		$item = explode(",",substr($str,$i2+1,$i3-$i2-1));
		global $tbl_gm_log_user;
		$fun = "&nbsp;";
		foreach($tbl_gm_log_user[$type] as $jsn => $act){
			$fun .= "<a href='#' onclick='".$jsn."(this)'>".$act."</a>&nbsp;";
		}
		foreach($item as $key => $i){
			$item[$key] = "<span><a href=\"#\" onclick=\"log_info('".$type."',this)\">".$i."</a><div class='log_info'>".$fun."</div>";
		}
		$str = substr($str,0,$i1).implode(",",$item).substr($str,$i3+3);
	}
	if($ret != ""){
		$str .= "<hr/>".$ret;
	}
	return $str;
}

function GetGMCommandLogAction($action){
	global $page_action_name;
	$bigaction = intval($action);
	if($bigaction == 54 || $bigaction == 55){
		$subaction = "-".$page_action_name[$action*10000-$bigaction*10000];
	}
	return $page_action_name[floatval($action)].$subaction;
}

function download($table,$display){
	if($_REQUEST["download"]=="true"){
		$filename = "../../".gmmktime().".csv";
		$file = fopen($filename,"w");
		foreach($table as $row)
			fwrite($file,UTF82Local("\"".implode("\",\"",$row)."\"\n"));
		fclose($file);
		
		echo "<script>\n";
		echo "window.location.href='../../cgi-bin/gm_tools/download.php?file=$filename&display=$display';\n";
		echo "</script>\n";
	}else{
		global $tbl_sys_msg;
		echo "<table class='list_table'><tr><td><a href='".get_http_args()."&download=true' target='_blank'>[".$tbl_sys_msg["download"]."]</a></td></tr></table>";
	}
}

function LastLogTime(){
	global $tbl_sys_msg;
	$js = <<<_js_str
<script>
function log_time_open(){
	$.ajax({
		url:	"/cgi-bin/gm_tools/FrameWork.php?rand='.rand(0,100).'",
		data:	"UID=%s&gameId=%s&action_name=241",
		success:function(msg){
			$("body").append("<div style='position: absolute; right:10px; top:10px; text-align:right; background:#FFFFFF' id = 'log_time' onclick='log_time_close(this);'></div>");
			$("#log_time").html(msg);
		}
	});
}

function log_time_close(obj){
	$(obj).remove();
}
</script>
_js_str;
	printf($js,$_REQUEST["UID"],$_REQUEST["gameId"]);
	echo "<table class='list_table'><tr><td><input type='button' onclick='log_time_open();' value='".$tbl_sys_msg["LastLogTime"]."' /></td></tr></table>\n";
}

function ShowGMToolsNoticeList($gm){
	global $tbl_sys_msg;
	$qs = "select gmn.gmn_uId, gmn.gmn_sTitle, gmn.gmn_dDate from tbl_gm_tools_notice gmn, tbl_gm_tools_notice_unread gmnr where gmn.gmn_uId = gmnr.gmn_uId and gmnr.gms_sName = '$gm'";
	$rs = MySQLQuery($qs, "GMS");
	if(mysql_num_rows($rs)>0){
		echo "<table class='GMToolsNotice'>\n";
		while($row = mysql_fetch_row($rs)){
			printf("<tr><td style='width:150px;'><a href=\"ShowNotice.php?UID=%s&id=%d\">%s</a></td><td style='width:60px;'>%s</td></tr>\n",$_REQUEST["UID"],$row[0],$row[1],$row[2]);
		}
		echo "<tr class='bottom'><td colspan='2'><a onclick='javascript:$(this).parents(\".GMToolsNotice\").fadeOut();'>".$tbl_sys_msg["close"]."</a></td></tr>";
		echo "</table>\n";
	}
}

function Chart_Pie($labels,$values){
	$identity = rand()*1000;
	echo "<table class='list_table'><tr><td><div id='chart_".$identity."' class='chart' style='width:300px; height:200px;'></div></td></tr></table>";
	echo "<script>\n";
	echo "var data_".$identity." = [];\n";
	foreach($labels as $i => $label){
		echo "data_".$identity."[$i] = { label: \"$label\", data: $values[$i] }\n";
	}
	echo "$.plot($(\"#chart_".$identity."\"), data_".$identity.",{series: {pie: {show: true}}});\n";
	echo "</script>\n";
}

function Chart_Line($label1,$label2,$values,$args = ""){
	$identity = rand();
	echo "<table class='list_table'><tr><td><div id='chart_".$identity."' class='chart' style='width:100%; height:200px;'></div></td></tr></table>";
	echo "<script>\n";
	echo "var data_".$identity." = [\n";
	foreach($label1 as $i => $label){
		echo "\t{\n";
		echo "\t\tlabel: \"".$label."\",\n";
		echo "\t\tdata: [";
		foreach($label2 as $j => $point){
			echo "[".$point.",".$values[$i][$j]."],";
		}
		echo "],\n";
		echo "\t\tlines: { show: true },\n";
		echo "\t\tpoints: { show: true },\n";
		echo "\t},\n";
	}
	echo "];\n";
	echo "$.plot($(\"#chart_".$identity."\"), data_".$identity.",{ $args });\n";
	echo "</script>\n";
}

function Lan_Load($param, $action){
	global $sys_config,$tbl_game_server,$tbl_gm_db;
	if($sys_config["search_server"]==null){
		$qs = "SELECT `gv_value` FROM `tbl_gm_variable` WHERE `gv_name` = 'SearchServer'";
		$rs = MySQLQuery($qs, "GMS");
		if($row = mysql_fetch_row($rs)){
			$sys_config["search_server"] = $row[0];
		}
	}
	$server_id = $sys_config["search_server"];
	$param["lang"] = $sys_config["lang"];
	$server = $tbl_game_server[$server_id];
	$gas_host = $server[0];
	$gas_port = $server[1];
	$ret = PostData($gas_host, $gas_port, $action, $param);
	$xml_item = new DOMDocument();
	$xml_item->loadXML($ret);
	return $xml_item;
}

function Lan_Item_GetDisplayName($name, $type = "", $b = true){
	global $Lan_Item_DisplayName;
	if($name=="") return "-";
	if($Lan_Item_DisplayName[$name]==""){
		$ret = array("<i>$name</i>","<span class='base64'>".UTF82Base64($name)."</span>");
		$param = array();
		$param["name"] = UTF82Local($name);
		if($type != "") $param["type"] = $type;
		$xml_item = Lan_Load($param, "Lan_Item_GetDisplayName");
		if($xml_item->getElementsByTagName("Item")->length>0){
			$ret[0] = $xml_item->getElementsByTagName("Item")->item(0)->nodeValue;
		}
		$Lan_Item_DisplayName[$name] = $ret;
	}
	$r = $Lan_Item_DisplayName[$name][0];
	if($b) $r .= $Lan_Item_DisplayName[$name][1];
	return $r;
}

function Lan_Item_SearchByDisplayName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = $name;
	$xml_item = Lan_Load($param, "Lan_Item_GetByDisplayName");
	foreach($xml_item->getElementsByTagName("Type") as $type_item){
		$temp = array();
		foreach($type_item->getElementsByTagName("Item") as $item_item){
			$item = array();
			$item["RealName"] = $item_item->getElementsByTagName("RealName")->item(0)->nodeValue;
			$item["DisplayName"] = $item_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
			array_push($temp, $item);
		}
		$typeid = $type_item->getAttribute("id");
		$ret[$typeid] = $temp;
	}
	return $ret;
}

function Lan_Item_SearchByRealName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Item_GetByRealName");
	foreach($xml_item->getElementsByTagName("Type") as $type_item){
		$temp = array();
		foreach($type_item->getElementsByTagName("Item") as $item_item){
			$item = array();
			$item["RealName"] = $item_item->getElementsByTagName("RealName")->item(0)->nodeValue;
			$item["DisplayName"] = $item_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
			array_push($temp, $item);
		}
		$typeid = $type_item->getAttribute("id");
		$ret[$typeid] = $temp;
	}
	return $ret;
}

function Lan_Equip_GetPropertyName($name){
	if($name=="") return "-";
	$ret = "<i>$name</i>";
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Equip_GetPropertyName");
	if($xml_item->getElementsByTagName("Property")->length>0){
		$ret = $xml_item->getElementsByTagName("Property")->item(0)->nodeValue;
	}
	return $ret;
}

function Lan_Equip_GetSkillName($type, $index){
	if($type=="" or $index=="") return "";
	$ret = "";
	$param = array();
	$param["type"] = UTF82Local($type);
	$param["index"] = $index;
	$xml_item = Lan_Load($param, "Lan_Equip_GetSkillName");
	if($xml_item->getElementsByTagName("Skill")->length>0){
		$ret = $xml_item->getElementsByTagName("Skill")->item(0)->nodeValue;
	}
	return $ret;
}

function Lan_Quest_GetDisplayName($name, $b = true){
	if($name=="") return "-";
	$ret = "<i>$name</i>";
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Quest_GetDisplayName");
	if($xml_item->getElementsByTagName("Quest")->length>0){
		$ret = $xml_item->getElementsByTagName("Quest")->item(0)->nodeValue;
	}
	if($b){
		$ret .= "<span class='base64'>".UTF82Base64($name)."</span>";
	}
	return $ret;
}

function Lan_Scene_GetDisplayName($name, $b = true){
	if($name=="") return "-";
	$ret = "<i>$name</i>";
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Scene_GetDisplayName");
	if($xml_item->getElementsByTagName("Scene")->length>0){
		$ret = $xml_item->getElementsByTagName("Scene")->item(0)->nodeValue;
	}
	if($b){
		$ret .= "<span class='base64'>".UTF82Base64($name)."</span>";
	}
	return $ret;
}

function Lan_Scene_SearchByDisplayName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = $name;
	$xml_item = Lan_Load($param, "Lan_Scene_GetByDisplayName");
	foreach($xml_item->getElementsByTagName("Scene") as $scene_item){
		$item = array();
		$item["RealName"] = $scene_item->getElementsByTagName("RealName")->item(0)->nodeValue;
		$item["DisplayName"] = $scene_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
		$item["SceneType"] = $scene_item->getElementsByTagName("SceneType")->item(0)->nodeValue;
		$item["InitPosX"] = $scene_item->getElementsByTagName("InitPosX")->item(0)->nodeValue;
		$item["InitPosY"] = $scene_item->getElementsByTagName("InitPosY")->item(0)->nodeValue;
		array_push($ret, $item);
	}
	return $ret;
}

function Lan_Quest_SearchByDisplayName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = $name;
	$xml_item = Lan_Load($param, "Lan_Quest_GetByDisplayName");
	foreach($xml_item->getElementsByTagName("Quest") as $scene_item){
		$item = array();
		$item["RealName"] = $scene_item->getElementsByTagName("RealName")->item(0)->nodeValue;
		$item["DisplayName"] = $scene_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
		array_push($ret, $item);
	}
	return $ret;
}

function Lan_Quest_SearchByRealName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Quest_GetByRealName");
	foreach($xml_item->getElementsByTagName("Quest") as $scene_item){
		$item = array();
		$item["RealName"] = $scene_item->getElementsByTagName("RealName")->item(0)->nodeValue;
		$item["DisplayName"] = $scene_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
		array_push($ret, $item);
	}
	return $ret;
}

function Lan_Equip_GetSuitInfo($name,$talent = 0){
	$ret = array();
	if($name=="") return $ret;
	$param = array();
	$param["name"] = UTF82Local($name);
	$param["talent"] = $talent;
	$xml_item = Lan_Load($param, "Lan_Equip_GetSuitInfo");
	if($xml_item->getElementsByTagName("Name")->length>0){
		$ret["name"] = $xml_item->getElementsByTagName("Name")->item(0)->nodeValue;
		$parts = $xml_item->getElementsByTagName("Part");
		$ret["part"] = array();
		foreach($parts as $i => $part){
			$ret["part"][$i] = $part->nodeValue;
		}
		$descs = $xml_item->getElementsByTagName("Desc");
		$ret["desc"] = array();
		foreach($descs as $i => $desc){
			$i = $desc->getAttribute("number");
			$ret["desc"][$i] = $desc->nodeValue;
		}
		$ret["talent"] = $xml_item->getElementsByTagName("Talent")->item(0)->nodeValue;
	}
	return $ret;
}

function Lan_Equip_GetArmorPiece($name){
	$ret = array();
	if($name=="") return $ret;
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Equip_GetArmorPiece");
	if($xml_item->getElementsByTagName("Property")->length>0){
		$ret[0] = $xml_item->getElementsByTagName("AddTimes")->item(0)->nodeValue;
		$ret[1] = $xml_item->getElementsByTagName("Property")->item(0)->nodeValue;
		$ret[2] = $xml_item->getElementsByTagName("Value")->item(0)->nodeValue;
	}
	return $ret;
}

function Lan_Skill_GetDisplayName($name, $b = true){
	$ret = "<i>$name</i>";
	if($name=="") return $ret;
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Skill_GetDisplayName");
	if($xml_item->getElementsByTagName("Skill")->length>0){
		$ret = $xml_item->getElementsByTagName("Skill")->item(0)->nodeValue;
	}
	if($b){
		$ret .= "<span class='base64'>".UTF82Base64($name)."</span>";
	}
	return $ret;
}

function Lan_Appellation($id){
	$ret = "";
	if($id==0 || $id=="") return $ret;
	$param = array();
	$param["id"] = $id;
	$xml_item = Lan_Load($param, "Lan_Appellation");
	if($xml_item->getElementsByTagName("Appellation")->length>0){
		$ret = $xml_item->getElementsByTagName("Appellation")->item(0)->nodeValue;
	}
	return $ret;
}

function Lan_NPC_GetDisplayName($name, $b = true){
	$ret = "<i>$name</i>";
	if($name=="") return $ret;
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_NPC_GetDisplayName");
	if($xml_item->getElementsByTagName("NPC")->length>0){
		$ret = $xml_item->getElementsByTagName("NPC")->item(0)->nodeValue;
	}
	if($b){
		$ret .= "<span class='base64'>".UTF82Base64($name)."</span>";
	}
	return $ret;
}

function Lan_NPC_SearchByDisplayName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = $name;
	$xml_item = Lan_Load($param, "Lan_NPC_GetByDisplayName");
	foreach($xml_item->getElementsByTagName("NPC") as $scene_item){
		$item = array();
		$item["RealName"] = $scene_item->getElementsByTagName("RealName")->item(0)->nodeValue;
		$item["DisplayName"] = $scene_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
		array_push($ret, $item);
	}
	return $ret;
}

function Lan_NPC_SearchByRealName($name){
	if($name=="") return array();
	$ret = array();
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_NPC_GetByRealName");
	foreach($xml_item->getElementsByTagName("NPC") as $scene_item){
		$item = array();
		$item["RealName"] = $scene_item->getElementsByTagName("RealName")->item(0)->nodeValue;
		$item["DisplayName"] = $scene_item->getElementsByTagName("DisplayName")->item(0)->nodeValue;
		array_push($ret, $item);
	}
	return $ret;
}

function Lan_Tong_GetTechName($name, $level = ""){
	$ret = "<i>$name</i>";
	if($name=="") return $ret;
	$name.= $level;
	$param = array();
	$param["name"] = UTF82Local($name);
	$xml_item = Lan_Load($param, "Lan_Tong_GetTechName");
	if($xml_item->getElementsByTagName("Tech")->length>0){
		$ret = $xml_item->getElementsByTagName("Tech")->item(0)->nodeValue;
	}
	return $ret;
}

function UpdateJointSalesList(){
	global $tbl_serv_partition;
	foreach($tbl_serv_partition[$_REQUEST["gameId"]] as $group){
		foreach($group as $server){
			PostData($server[0], $server[1], "UpdateJointSalesList", array());
		}
	}
}

function ArgRoleName(){
	if($_REQUEST["role_item"]=="id"){
		return GetRoleName($_REQUEST["role_str"],$_REQUEST["serverid"]);
	}else{
		return $_REQUEST["role_str"];
	}
}

function ArgRoleId(){
	if($_REQUEST["role_item"]=="id"){
		return $_REQUEST["role_str"];
	}else{
		return GetRoleId($_REQUEST["role_str"],$_REQUEST["serverid"]);
	}
}

function MySQLQuery($query, $database, $server = 0){
	global $databases,$tbl_error_msg;
	if($databases == null){
		$databases = array("Game" => array(), "Log" => array(), "GameLog" => array());
	}
	$conn = null;
	if($database == "GMS"){
		$conn = $databases["GMS"];
		if($conn == null){
			global $tbl_gm_db;
			$conn = mysql_connect($tbl_gm_db[0][2], $tbl_gm_db[0][0], $tbl_gm_db[0][1], true);
			mysql_select_db($tbl_gm_db[0][3], $conn);
			$databases["GMS"] = $conn;
		}
	}else{
		$conn = $databases[$database][$server];
		if(!$conn){
			if($database == "Log"){
				global $tbl_log_db;
				$dbinfo = $tbl_log_db[$server];
			}else{
				global $tbl_serv_db;
				$dbinfo = $tbl_serv_db[$server];
			}
			$conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1], true);
			if(!$conn){
				global $tbl_game_server,$tbl_mysql_connect_type;
				if(!$rs) PrintNotice(sprintf($tbl_error_msg["MySQLError"],$tbl_game_server[$server][2],$tbl_mysql_connect_type[$database],$tbl_error_msg["MySQLConnect"],$query));
				return null;
			}
			if($database == "GameLog"){
				mysql_select_db($dbinfo[4],$conn);
			}else{
				mysql_select_db($dbinfo[3],$conn);
			}
			$databases[$database][$server] = $conn;
		}
		$conn = $databases[$database][$server];
	}
	mysql_ping($conn);
	$rs = mysql_query($query, $conn);
	if(!$rs){
		global $tbl_game_server,$tbl_mysql_connect_type;
		PrintNotice(sprintf($tbl_error_msg["MySQLError"],$tbl_game_server[$server][2],$tbl_mysql_connect_type[$database],mysql_error($conn),$query));
	}
	return $rs;
}
?>
