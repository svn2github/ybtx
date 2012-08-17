<?
$channel_id = array(
	"系统"     => 1,	
	"世界"     => 2,	
	"阵营"     => 3,	
	"地图"     => 4,	
	"附近"     => 5,	
	"队伍"     => 6,	
	"佣兵小队" => 7,	
	"佣兵团"   => 8,	
	"传声"     => 10, 
);

function ResMsg($tbl_content, $page, $b, $c, $o){
	global $tbl_talk;
	echo "<table class='list_table'>\n";
	$str = "";
	for($i=0; $i< count($tbl_content); $i++){
		$s = "";
		$s.= "<tr".((($i+$c)%2)?"":" class='even'")."><td style=\"border-bottom:0px; border-right:0px;\"><b>";
		$s.= ($b)?"<a href=\"#\" onclick=\"show_by_role(this,'".$tbl_content[$i][0]."')\">".$tbl_content[$i][0]."</a>":(($tbl_content[$i][3]==0)?$tbl_talk["me"]:$tbl_content[$i][0]);
		$s.= "</b></td><td style=\"text-align:right;border-bottom:0px;\"><i>".$tbl_content[$i][1]."</i></td></tr>\n";
		$s.= "<tr".((($i+$c)%2)?"":" class='even'")."><td colspan='2'>".$tbl_content[$i][2]."</td></tr>\n";
		$str = ($o)? $str.$s:$s.$str;
	}
	echo $str;
	echo $page;
	if(!$b) echo "<||".((($i+$c)%2)?"1":"0")."||>";
}
function AllMsg($var, $gm){
	if($var["serverindex"]==null || $var["serverindex"]==""){
		$js_str = <<<_js_str
<script type='text/javascript' src='/js/page.js'></script>
<script>
	function show_by_role(obj,role_name){
		document.form1.role_name.value = role_name;
		document.form1.action_name.value = '158';
		document.form1.server.value = $(obj).parents(".panel").attr("id").substring(4);
		document.form1.submit();
	}
	setInterval("window.location.reload()",30000);
</script>
_js_str;
		global $tbl_talk;
		echo "<h1>".$tbl_talk["AllMsg"]."</h1>";
		ReturnAndBack();
		echo $js_str."\n";
		form($var, $gm);
		echo "<input type='hidden' name='role_name' />\n";
		echo "<input type='hidden' name='server' />\n";
		$ret = eachServerFrame($var, $gm);
		echo "</form>\n";
		ReturnAndBack();
		return $ret;
	}
	$serverid = $var["serverindex"];
	$query = "select c_sName, gm_dtCreateTime, gm_sContent, gm_uSenderId from tbl_gm_msg gm, tbl_char c where gm_uRecieverId = 0 and gm.gm_uSenderId = c.cs_uId order by gm_dtCreateTime desc";
	page($var, $query, 159, "Game", $serverid, $result, $page, $desc);
	$tbl_content = array();
	while($row=mysql_fetch_row($result)){
		$count = count($tbl_content);
		$tbl_content[$count] = $row;
	}
	
	echo $desc;
	ResMsg($tbl_content, $page, true, 0, true);
	
	return true;
}

function LoadTalkMsg($var, $gm){
	$role_name = $var["role_name"];
	$serverid = $var["serverid"];
	$date = $var["date"];
	$type = $var["type"];
	$limit = $var["limit"];
	$c = $var["c"];
	
	$where = "";
	if($type=="next"){
		$where = " and gm_dtCreateTime > '".$date."' group by gm_dtCreateTime,gm_sContent order by gm_dtCreateTime";
	}else if($type=="prev"){
		if($limit == "") $limit = 10;
		$where = " and gm_dtCreateTime <= '".$date."' group by gm_dtCreateTime,gm_sContent order by gm_dtCreateTime desc limit ".(intval($limit)+1);
	}
	 
	$query="select c_sName, gm_dtCreateTime, gm_sContent, gm_uSenderId from tbl_gm_msg gm, tbl_char c where (gm.gm_uSenderId = c.cs_uId or gm.gm_uRecieverId = c.cs_uId) and c.c_sName = '".$role_name."'".$where;
	$result = MySQLQuery($query, "Game", $serverid);
	$tbl_content = array();
	$new_date = $date;
	$num = (mysql_affected_rows()-1);
	$i = 0;
	while($row=mysql_fetch_row($result)){
		$new_date = $row[1];
		if($type=="next")
			$tbl_content[$i] = $row;
		else if($type=="prev" && $num!=1){
			if($limit!=$num || $i!=$num)
				$tbl_content[$i] = $row;
		}
		$i++;
	}
	echo "<|".$new_date."|>";
	if($type=="next"){
		MySQLQuery("replace tbl_talk_log values('".$role_name."','".$new_date."',".$serverid.")", "GMS");
	}
	
	ResMsg($tbl_content, $page, false, $c, ($type=="next")?true:false);
	
	
	return true;
}

function TalkWithRole($var, $gm){
	$role_name = ArgRoleName();
	$serverid = $var["serverid"];
	
	global $tbl_talk;
	$result = MySQLQuery("select last_update from tbl_talk_log where role_name='".$role_name."' and serverid=".$serverid, "GMS");
	$last_update = mysql_fetch_row($result);
	$last_update = $last_update[0];
	if($last_update=="") $last_update=0;
	
	echo '<style>'."\n";
	echo '#win table {width: 100% !important;}'."\n";
	echo '</style>'."\n";
	echo '<script>'."\n";
	echo '$(function(){more_new();setInterval("more_new()",30000);});'."\n";
	echo 'function more_new(){'."\n";
	echo '	$.ajax({'."\n";
	echo '		type:"get",'."\n";
	echo '		url:"/cgi-bin/gm_tools/FrameWork.php?rand="+new Date().getTime(),'."\n";
	echo '		data:encodeURI("UID='.$var["UID"].'&gameId='.$var["gameId"].'&action_name=160&role_name='.$role_name.'&serverid='.$serverid.'&date="+$("#new_date").val()+"&type=next&c="+$("#ce").val()),'."\n";
	echo '		success:function(msg){'."\n";
	echo '			var bb = msg.indexOf("<|");'."\n";
	echo '			var be = msg.indexOf("|>");'."\n";
	echo '			var cb = msg.indexOf("<||");'."\n";
	echo '			var ce = msg.indexOf("||>");'."\n";
	echo '			$("#new_date").val(msg.substring(bb+2,be));'."\n";
	echo '			$("#win").append(msg.substring(be+2,cb));'."\n";
	echo '			$("#ce").val(msg.substring(cb+3,ce));'."\n";
	echo '		}'."\n";
	echo '	});'."\n";
	echo '}'."\n";
	echo 'function more_older(){'."\n";
	echo '	$.ajax({'."\n";
	echo '		type:"get",'."\n";
	echo '		url:"/cgi-bin/gm_tools/FrameWork.php?rand="+new Date().getTime(),'."\n";
	echo '		data:encodeURI("UID='.$var["UID"].'&gameId='.$var["gameId"].'&action_name=160&role_name='.$role_name.'&serverid='.$serverid.'&date="+$("#old_date").val()+"&type=prev&c="+$("#cb").val()),'."\n";
	echo '		success:function(msg){'."\n";
	echo '			var bb = msg.indexOf("<|");'."\n";
	echo '			var be = msg.indexOf("|>");'."\n";
	echo '			var cb = msg.indexOf("<||");'."\n";
	echo '			var ce = msg.indexOf("||>");'."\n";
	echo '			$("#old_date").val(msg.substring(bb+2,be));'."\n";
	echo '			$("#win").html(msg.substring(be+2,cb)+$("#win").html());'."\n";
	echo '			$("#cb").val(msg.substring(cb+3,ce));'."\n";
	echo '		}'."\n";
	echo '	});'."\n";
	echo '}'."\n";
	echo 'function send(){'."\n";
	echo '	$.ajax({'."\n";
	echo '		type:"post",'."\n";
	echo '		url:"/cgi-bin/gm_tools/FrameWork.php?rand="+new Date().getTime(),'."\n";
	echo '		data:encodeURI("UID='.$var["UID"].'&gameId='.$var["gameId"].'&action_name=30&role_name='.$role_name.'&serverid='.$serverid.'&warning="+$("textarea").val()+"&reason='.$tbl_talk["reason"].'"),'."\n";
	echo '		success:function(msg){'."\n";
	echo '			more_new();'."\n";
	echo '		}'."\n";
	echo '	});'."\n";
	echo '}'."\n";
	echo '</script>'."\n";
	echo '<input type="hidden" id="old_date" value="'.$last_update.'" />'."\n";
	echo '<input type="hidden" id="new_date" value="'.$last_update.'" />'."\n";
	echo '<input type="hidden" id="cb" value="0" />'."\n";
	echo '<input type="hidden" id="ce" value="1" />'."\n";
	echo '<div id="win" style="border:1px solid #ccc; width:703px; height:300px; overflow: scroll;"></div>'."\n";
	echo '<table style="width:705; border:1px solid #ccc; border-top:0px;">'."\n";
	echo '	<tr>'."\n";
	echo '		<td><textarea style="width:100%; height:100%;"></textarea></td>'."\n";
	echo '		<td style="width:150px;">'."\n";
	echo '			<button style="width:150px;" onclick="send()">'.$tbl_talk["send"].'</button>'."\n";
	echo '			<button style="width:150px;" onclick="more_new()">'.$tbl_talk["reflesh"].'</button>'."\n";
	echo '			<button style="width:150px;" onclick="more_older()">'.$tbl_talk["older"].'</button>'."\n";
	echo '		</td>'."\n";
	echo '	</tr>'."\n";
	echo '</table>'."\n";
	return true;
}

function ChannelWordMgr($var, $gm){
	global $tbl_channel_word;
	$js_str = <<<_js_str
<script type="text/javascript">
function new_word(){
	document.form1.target = "_block";
	document.form1.action_name.value = "162";
	document.form1.submit();
	document.form1.word.value = "";
}
function del(word){
	document.form1.target = "_block";
	document.form1.action_name.value = "163";
	document.form1.word.value = word;
	document.form1.submit();
	document.form1.word.value = "";
}
</script>
_js_str;
	echo $js_str."\n";
	echo "<h1>".$tbl_channel_word[0]."</h1>\n";
	ReturnAndBack();
	form($var, $gm);
	echo "<input type='text' name='word' />\n";
	echo "<input type='button' value='".$tbl_channel_word[2]."' onclick='new_word()' />\n";
	echo "</form>\n";
	echo "<table class='list_table' >\n";
	echo "<tr><th>".$tbl_channel_word[1][0]."</th><th>".$tbl_channel_word[1][1]."</th></tr>\n";
	
	$qs = "select * from tbl_channel_word";
	$result = MySQLQuery($qs, "GMS");
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td>%s</td><td><input type='button' value='".$tbl_channel_word[1][1]."' onclick=\"del('%s')\" /></td></tr>\n",$row[0],$row[0]);
	}
	echo "</table>\n";
	ReturnAndBack();
	return true;
}

function ChannelWordIns($var, $gm){
	global $tbl_channel_word;
	
	echo "<h1>".$tbl_channel_word[3]."</h1>\n";
	
	$word = $var["word"];
	$result = MySQLQuery("insert into tbl_channel_word set word = '$word'", "GMS");
	if(mysql_affected_rows()>0){
		PrintNotice(sprintf($tbl_channel_word[4],$word));
		if(ChannelChatLoad()){
			global $tbl_sys_msg;
			PrintNotice($tbl_sys_msg["check_chat"]);
		}
	}
	
	BackAndRefresh();
	return true;
}

function ChannelWordDel($var, $gm){
	global $tbl_channel_word;
	
	echo "<h1>".$tbl_channel_word[5]."</h1>\n";
	
	$word = $var["word"];
	$result = MySQLQuery("delete from tbl_channel_word where word = '$word'", "GMS");
	if(mysql_affected_rows()>0){
		PrintNotice(sprintf($tbl_channel_word[6],$word));
		if(ChannelChatLoad()){
			global $tbl_sys_msg;
			PrintNotice($tbl_sys_msg["check_chat"]);
		}
	}
	
	BackAndRefresh();
	return true;
}

function ChannelChatReload($var, $gm){
	ChannelChatLoad();
	global $tbl_game_server;
	$qs = "select Name,ChatTime,ServerId,Msg from tbl_channel_chat where ServerId in (".$var["serv_id"].") and ChatTime > '".$var["last_dt"]."' order by ChatTime desc";
	$result = MySQLQuery($qs, "GMS");
	while($row = mysql_fetch_row($result)){
		echo sprintf("<tr".(($i%2==0)?"":" class='even'")."><td><input type='checkbox' flag='role' value='%s' onclick='Checkbox(this)'/>%s</td><td class='dt'>%s</td><td>%s</td></tr>\n",$row[0],$row[0],$row[1],$tbl_game_server[intval($row[2])][2]);
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td colspan='3'>%s</td></tr>\n",$row[3]);
	}
	return true;
}

function ChannelChat($var, $gm){
	global $tbl_game_server,$tbl_sys_msg,$page_main;
	
	$server_array = $var["server_id"];
	$servers = "";
	if(count($server_array)>0){
		$servers = $server_array[0];
		for($i=1;$i<count($server_array);$i++){
			$servers.= ",".$server_array[$i];
		}
	}else{
		return false;
	}
	
	if($var["page"]==null || $var["page"]==""){
		echo "<script type='text/javascript' src='/js/page.js'></script>\n";
		echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
		form($var,$gm);
		checkbox(false,true);
		ReturnAndBack();
		Reload();
		echo "<div class='panel'>\n";
	}
	
	if($var["page"]==null || $var["page"]=="" || $var["page"]=="1"){
		if(ChannelChatLoad()){
			global $tbl_sys_msg;
			PrintNotice($tbl_sys_msg["check_chat"]);
		}
	}
	
	$qs = "select Name,ChatTime,ServerId,Msg,cc_sUser from tbl_channel_chat where ServerId in (".$servers.") order by ChatTime desc";
	page($var,$qs,164,"GMS",0,$result,$page,$desc);
	echo $desc;
	echo "<table class='list_table'>\n";
	echo "<tr><th>".$page_main["user_name"]."</th><th>".$page_main["role_name"]."</th><th>".$tbl_sys_msg["time"]."</th><th>".$tbl_sys_msg["server"]."</th></tr>\n";
	while($row=mysql_fetch_row($result)){
		echo sprintf("<tr".(($i%2==0)?"":" class='even'")."><td><input type='checkbox' flag='user' value='%s' onclick='Checkbox(this)'/>%s</td><td><input type='checkbox' flag='role' value='%s' onclick='Checkbox(this)'/>%s</td><td class='dt'>%s</td><td>%s</td></tr>\n",$row[4],$row[4],$row[0],$row[0],$row[1],$tbl_game_server[intval($row[2])][2]);
		echo sprintf("<tr".(($i++%2==0)?"":" class='even'")."><td colspan='5'>%s</td></tr>\n",$row[3]);
	}
	echo "</table>\n";
	echo $page;
	
	if($var["page"]==null || $var["page"]==""){
		echo "</div>\n";
		Reload();
		ReturnAndBack();
		echo "</form>\n";
	}
	return true;
}

function ChannelChatLoad(){
	require_once("../../cron/gm_tools/ChannelChatCheck.php");
	return true;
}

function ChannelString($str){
	return preg_replace('/\#c[0-9,A-F,a-f]{6}\#l\[/','[',preg_replace('/\]\#i\[\*[\s\S]*\]\#l\#W/',']',preg_replace('/\)\d+\]/',']',preg_replace('/\#c[0-9,A-F,a-f]{6}\[\(/','[',$str))));
}

function ChannelChatAllReload($var, $gm){
	echo "_start_";
	$serv_id = $var["serv_id"];
	$last_dt = $var["last_dt"];
	$nld = $last_dt;
	global $tbl_serv_db,$tbl_game_server,$tbl_channel;
	foreach(explode(",",$serv_id) as $key){
		$qs = "select lcc.cs_uId,lcc.lcc_dtChatTime,lcc.lcc_sMsg,lcc.lcc_sChannelName,lcc.cs_uId from tbl_log_channel_chat lcc where lcc.lcc_dtChatTime > '$last_dt'";
		$result = MySQLQuery($qs, "GameLog", $key);
		
		while($row = mysql_fetch_row($result)){
			$qs = "select c_sName from tbl_char where cs_uId = $row[0]";
			$rs = MySQLQuery($qs, "Game", $key);
			if($r = mysql_fetch_row($rs)) $row[0] = $r[0];
			if($row[1]>$nld) $nld = $row[1];
				$msg = ChannelString($row[2]);
			$temp = $row[3];
			switch($row[3]){
			case "阵营":
				$row[3] = 3;
				break;
			case "世界":
				$row[3] = 2;
				break;
			case "传声":
				$row[3] = 10;
				break;
			}
			printf("<tr><td style='width:85px'>%s</td><td>%s</td><td style='width:115px;'>%s</td><td style='width:50px;'>%s</td><td style='width:25px;'>%s</td></tr>\n",$row[0],$msg,$row[1],$tbl_game_server[$key][2],$tbl_channel[$row[3]]);
		}
	}
	echo ">|+|+|<".$nld;
	return true;
}

function ChannelChatAllLog($var, $gm){
	global $tbl_serv_db,$tbl_sys_msg,$page_main,$tbl_channel,$channel_id;
	echo "<script type='text/javascript' src='/js/page.js'></script>\n";
	echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
	$serverid = $var["serverid"];
	
	$role_id = ArgRoleId();
	if(!$role_id) return true;
	
	$str = "";
	if($var["begin_time"] != null && $var["begin_time"] != ""){
		$str .= " and lcc_dtChatTIme >= '".$var["begin_time"]."'";
	}
	if($var["end_time"] != null && $var["end_time"] != ""){
		$str .= " and lcc_dtChatTIme <= '".$var["end_time"]."'";
	}
	$qs = "select lcc_sMsg, lcc_dtChatTime, lcc_sChannelName from tbl_log_channel_chat where cs_uId = ".$role_id.$str;
	$rs = MySQLQuery($qs, "GameLog", $serverid);
	echo "<table class='list_table'>\n";
	printf("<tr><th>%s</th><th>%s</th><th>%s</th></tr>\n", $tbl_sys_msg["time"], $page_main["content"], $page_main["channel"]);
	while($row = mysql_fetch_row($rs)){
		printf("<tr><td>%s</td><td>%s</td><td>%s</td></tr>\n", $row[1],ChannelString($row[0]),$tbl_channel[$channel_id[$row[2]]]);
	}
	echo "</table>\n";
	return true;
}

function ChannelChatAll($var, $gm){
	global $tbl_serv_db,$tbl_game_server,$tbl_sys_msg;
	echo "<script type='text/javascript' src='/js/page.js'></script>\n";
	echo "<script type='text/javascript' src='/js/checkbox.js'></script>\n";
	$jsstr = <<<_jsstr
<script type="text/javascript">
function re_load(){
	$(".panel").each(function(){
		var ins = $(this).find("table");
		if($(ins).find("tbody").html()!=null)
			ins = $(ins).find("tbody");
		var last_dt = $("#ldt").val();
		var url = "/cgi-bin/gm_tools/FrameWork.php?UID=%s&action_name=209&serv_id="+$(this).attr("id").substring(4)+"&last_dt="+last_dt;
		$.ajax({
			url: url,
			data: "serv_id="+$(this).attr("id").substring(4)+"&last_dt="+last_dt,
			success: function(msg) {
				var s = msg.indexOf("_start_");
				var m = msg.indexOf(">|+|+|<");
				var ndt = msg.substring(m+7);
				var str = msg.substring(s+7,m);
				$(ins).append(str);
				$("#ldt").val(ndt);
			}
		});
	});
}

setInterval("re_load()",10000);
</script>
_jsstr;
	printf($jsstr,$var["UID"]);
	form($var,$gm);
	ReturnAndBack();
	
	$serv_id = implode(",",$var["server_id"]);
	
	echo "<div class='panel' id = 'div_$serv_id'>\n";
	echo "<input type='hidden' id='ldt' value='".date("Y-m-d H:i:s")."' />\n";
	echo "<table class='list_table'>\n";
	echo "</table>\n";
	echo "</div>\n";
	Reload();
	ReturnAndBack();
	echo "</form>\n";
	return true;
}
?>
