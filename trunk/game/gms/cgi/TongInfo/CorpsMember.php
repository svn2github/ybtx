<?php
function RetTongMember($tbl_content,$var,$page,$serverid)
{
	global $tbl_tong_pos,$tbl_tong_list_member, $tbl_sys_msg;
	$str="";
	$str=$str.sprintf("<table class='list_table'>\n");
	$str=$str.sprintf("<tr><th>%s</th><th style='width:50px;'>%s</th><th style='width:30px;'>%s</th><th style='width:80px;'>%s</th><th style='width:30px;'>%s</th><th style='width:130px;'>%s</th><th style='width:130px;'>%s</th></tr>\n", $tbl_tong_list_member[0][0], $tbl_tong_list_member[0][1], $tbl_tong_list_member[0][2], $tbl_tong_list_member[0][3], $tbl_tong_list_member[0][4], $tbl_tong_list_member[0][5], $tbl_sys_msg["act"]);

	$tong_member = count($tbl_content);
	for($i=0;$i<$tong_member;$i++){
		$control = sprintf("<input type='button' value='".$tbl_tong_list_member[1][1]."' onclick=\"dochange(this,'%s','%s','%s')\" />",$tbl_content[$i][0],$tbl_content[$i][1],$tbl_content[$i][3]);
		if(intval($tbl_content[$i][3])!=5)
			$control=sprintf("<input type='button' value='".$tbl_tong_list_member[1][0]."' onclick=\"doremove('%s','%s')\" />",$tbl_content[$i][0],$tbl_content[$i][1]).$control;
		$str=$str.sprintf("<tr".(($i%2==0)?"":" class='even'")."><td>%s</td><td>%d</td><td align='right'>%d</td><td>%s</td><td align='right'>%s</td><td align='center'>%s</td><td align='center'>%s</td></tr>\n", RoleFunction($tbl_content[$i][0],$serverid), $tbl_content[$i][1], $tbl_content[$i][2], getTongMemberType($tbl_content[$i][3]), $tbl_content[$i][4],$tbl_content[$i][5],$control);
	}
	$str=$str.sprintf("</table>");
	$str=$str.$page;
	echo $str;
}

function getTongMemberType($type){
	global $tbl_tong_pos;
	$str ="<select>";
	for($i=count($tbl_tong_pos)-1;$i>0;$i--){
		$temp = "";
		if($i==$type) $temp = " selected='selectd'";
		$str.="<option value='$i'".$temp.">".$tbl_tong_pos[$i]."</option>";
	}
	$str.="</select>";
	return $str;
}

function GetTongMember($var, $gm)
{
	$id = intval($var["id"]);
	global $tbl_sys_msg;
	$serverid=intval($var["serverid"]);
	$qs = "";
	if($var["role_name"]!=null && $var["role_name"]!="") $qs = " and c.c_sName like '%".$var["role_name"]."%'";
	$query = "select c.c_sName, c.cs_uId, cb.cb_uLevel, mt.mt_sPosition, tp.tp_uProffer, mt.mt_dtJoinTime from tbl_tong t, tbl_member_tong mt, tbl_char c left join tbl_tong_proffer tp on tp.cs_uId = c.cs_uId, tbl_char_basic cb where c.cs_uId = mt.cs_uId and mt.t_uId = t.t_uId and cb.cs_uId=c.cs_uId and t.t_uid='$id'".$qs." order by mt.mt_sPosition desc";
	page($var, $query, 64, "Game", $serverid, $result, $str, $desc);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}

	if($var["page"]==null || $var["page"]==""){
		$js_src=<<<_js_src
<script type="text/javascript" src="/js/page.js"></script>
<script type="text/javascript" src="/js/checkbox.js"></script>
<script type="text/javascript">
<!--//
function OnClick(role_name){
	document.form1.action_name.value = '97';
	document.form1.role_name.value = role_name;
	document.form1.submit();
}
function doremove(role_name,role_id){
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.target = "block";
	document.form1.reason.value = reason;
	document.form1.action_name.value = '170';
	document.form1.role_name.value = role_name;
	document.form1.role_id.value = role_id;
	document.form1.submit();
}
function dochange(obj,role_name,role_id,now_type){
	var new_type = $(obj).parents("tr").find("select").val();
	if(new_type==now_type) return;
	document.form1.position.value = new_type;
	document.form1.o_position.value = now_type;
	var reason = prompt("%s","");
	if(reason==null) return;
	document.form1.target = "block";
	document.form1.reason.value = reason;
	document.form1.action_name.value = '171';
	document.form1.role_name.value = role_name;
	document.form1.role_id.value = role_id;
	document.form1.submit();
}
function OnSearch(){
	var query = $("#role_name_str").val();
	var url_str = $(".panel").find("input[name='url']").val();
	var panel = $(".panel");
	$(panel).css("height",$(panel).height());
	$(panel).html("Loading...");
	$.ajax({
		type:"get",
		url:url_str,
		data:"page=1&role_name="+query+"&rand=%d",
		success:function(msg){
			$(panel).html(msg);
			$(panel).css("height","auto");
		}
	});
}
</script>
_js_src;
		echo sprintf($js_src."\n",$tbl_sys_msg["reason"],$tbl_sys_msg["reason"],rand(0,100));
		form($var,$gm);
		checkbox(false,true);
		echo "<input type='hidden' name='role_name'/>\n";
		echo "<input type='hidden' name='role_id'/>\n";
		echo "<input type='hidden' name='position'/>\n";
		echo "<input type='hidden' name='o_position'/>\n";
		echo "<input type='hidden' name='reason'/>\n";
		echo "<input type='hidden' name='id' value='".$var["id"]."'/>\n";
		ReturnAndBack();
		echo "<h2>".$tbl_sys_msg["search"]."</h2>\n";
		echo "<table class='list_table'><tr><td><input type='text' id='role_name_str'/><input type='button' onclick='OnSearch()' value='".$tbl_sys_msg["search"]."'/></td></tr></table>\n";
		echo "<div class='panel'>\n";
	}
	echo $desc;
	RetTongMember($tbl_content, $var, $str,$serverid);
	if($var["page"]==null || $var["page"]==""){
		echo "</div>\n";
		ReturnAndBack();
		echo "</form>\n";
	}
	return true;
}

function RemoveTongMember($var, $gm){
	global $tbl_game_server,$RemoveTongMember;
	
	$serverid = $var["serverid"];
	$role_name = $var["role_name"];
	$role_id = $var["role_id"];
	$tong_id = $var["id"];
	$reason = $var["reason"];
	
	$row = mysql_fetch_row(MySQLQuery("select t_sName from tbl_tong where t_uId = $tong_id","Game",$serverid));
	$tong_name = $row[0];
	
	$param = array();
	$param["tong_id"] = $tong_id;
	$param["role_id"] = $role_id;
	$param["msg"] = sprintf($RemoveTongMember,$role_name);
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "RemoveTongMember", $param);
	
	$action = array("tong" => $tong_name, "role_name" => $role_name, "reason" => $reason);
	
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, $var["action_name"], $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
	return false;
}

function ChangeTongMember($var, $gm){
	$serverid = $var["serverid"];
	$role_name = $var["role_name"];
	$role_id = $var["role_id"];
	$tong_id = $var["id"];
	$position = $var["position"];
	$o_position = $var["o_position"];
	$reason = $var["reason"];

	$row = mysql_fetch_row(MySQLQuery("select t_sName from tbl_tong where t_uId = $tong_id","Game",$serverid));
	$tong_name = $row[0];
	
	if($position==5){
		$row = mysql_fetch_row(MySQLQuery("select mt.cs_uId,c_sName from tbl_member_tong mt,tbl_char c where mt.cs_uId = c.cs_uId and t_uId = $tong_id and mt_sPosition = 5","Game",$serverid));
		$master_id = $row[0];
		$master_name = $row[1];
		ChangeTongMemberFun($tong_id,$master_id,4,$tong_name,$master_name,$serverid,$reason,$gm,$o_position);
	}
	if($o_position==5){
		$row = mysql_fetch_row(MySQLQuery("select mt.cs_uId,c_sName from tbl_member_tong mt,tbl_char c where mt.cs_uId = c.cs_uId and t_uId = $tong_id and c.cs_uId != $role_id order by mt_sPosition desc","Game",$serverid));
		$master_id = $row[0];
		$master_name = $row[1];
		ChangeTongMemberFun($tong_id,$master_id,5,$tong_name,$master_name,$serverid,$reason,$gm,$o_position);
	}
	ChangeTongMemberFun($tong_id,$role_id,$position,$tong_name,$role_name,$serverid,$reason,$gm,$o_position);
	return true;
}

function ChangeTongMemberFun($tong_id,$role_id,$position,$tong_name,$role_name,$serverid,$reason,$gm,$o_position){
	global $tbl_game_server,$tbl_tong_pos,$ChangeTongMember;
	$param = array();
	$param["tong_id"] = $tong_id;
	$param["role_id"] = $role_id;
	$param["position"]= $position;
	$param["msg"] = sprintf($ChangeTongMember[0],$tbl_tong_pos[$o_position],$role_name,$tbl_tong_pos[$position]);
	
	$gas_host = $tbl_game_server[$serverid][0]; 
	$gas_port = $tbl_game_server[$serverid][1]; 
	$ret=PostData($gas_host, $gas_port, "ChangeTongMember", $param);
	
	$action = array("tong" => $tong_name, "role_name" => $role_name, "pos" => $position, "reason" => $reason);
	if (intval($ret) != 200){
		PrintNotice(LogInfo($action,false,$ret));
		BackAndRefresh();
		return true;
	}
	if (LogGMCommand($gm, 171, $action, $serverid)){
		PrintNotice(LogInfo($action,true));
		BackAndRefresh();
		return true;
	}
}
?>
