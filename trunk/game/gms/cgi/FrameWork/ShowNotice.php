#!/usr/bin/php
<?php
header("Content-type: text/html; charset=utf-8");
require_once("../../lib/gm_tools/Auth.php");
global $action_page,$action_form;
require_once("../../conf/gm_tools/Config.php");
global $sys_config,$tbl_gm_db;
require_once("../../lang/gm_tools/".$sys_config["lang"]."/sys_config.php");
require_once("../../lang/gm_tools/".$sys_config["lang"]."/page_localized.php");
global $page_title,$page_action_name,$page_main;
$UID = $_REQUEST["UID"];
$id = $_REQUEST["id"];
$flag = $_REQUEST["flag"];
?>
<html>
	<head>
		<title><?=$page_title["ShowNotice"] ?></title>
		<link href="/css/action.css" rel="stylesheet" type="text/css" />
		<script type="text/javascript" src="/js/jquery-1.3.1.min.js"></script>
		<script type="text/javascript" src="/js/WebCalendar.js"></script>
		<script type="text/javascript">
			$(function(){
				$("table.form_content:visible").css("display","none").find("input,textarea,select").attr("disabled","disabled");
				$("table.action_"+$("#content").val()).css("display","").find("input,textarea,select").removeAttr("disabled");
				$("#content").change(function(){
					$("table.form_content:visible").css("display","none").find("input,textarea,select").attr("disabled","disabled");
					$("table.action_"+$(this).val().replace(".","_")).css("display","").find("input,textarea,select").removeAttr("disabled");
				});
				$("textarea[name='role_name'], textarea[name='tong_name']").change(function(){Enter2Comma($(this));});
				$("form").submit(onsubmit);
			});
			
			function Enter2Comma(obj){
				var str = $(obj).val();
				str = str.replace(/\n/g,",");
				$(obj).val(str);
			}
			
			function onsubmit(){
				var action = document.form1.action_name.value;
				if (!action || action == "" || !Number(action)){
					return false;
				}
				document.form1.action_name.value = Math.floor(action);
			}
			
			function search(action, obj){
				var str = $(obj).parents("td").find(".str").val();
				var name =$(obj).parents("td").find(".str").attr("name");
				var div = $(obj).parents("td").find(".pop");
				$(div).html("Loading.");
				$.ajax({
					type: "post",
					url: "/cgi-bin/gm_tools/FrameWork.php",
					data: "action_name="+action+"&UID="+$("input[name='UID']").val()+"&gameId="+$("input[name='gameId']").val()+"&str_match="+str,
					success:function(msg){
						var start = msg.indexOf("<error>");
						if(start==-1){
							start = msg.indexOf("<select")+7;
							var end = msg.indexOf("</select>",start)+9;
							$(div).html("<select onchange='search_change(\""+name+"\",this)'"+msg.substring(start,end));
						}else{
							var end = msg.indexOf("</error>",start);
							$(div).html(msg.substring(start+7,end));
						}
					}
				});
			}
			
			function search_change(name, obj){
				$(obj).parents("td").find("input[name='"+name+"']").val($(obj).val());
			}
		</script>
		<style>
			.info_table {
				border-collapse:collapse;
				border-spacing: 0px;
				width: 705px;
				padding:0px;
				margin:0px;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 0px 0px 1px;
			}
			
			.info_table th {
				width: 250px;
				height:30px;
				padding: 0px;
				font-weight:normal;
				text-align: right;
				vertical-align: middle;
				background: #cde4f6;
				color: #333333;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
			}
			
			.info_table td {
				height:30px;
				padding: 5px;
				vertical-align: middle;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
			}
			ul, ol {
				margin: 0px;
				padding: 0px;
				padding-left: 22px;
			}
		</style>
	</head>
	<body>
		<h1><?=$page_action_name["ShowNotice"] ?></h1>
<?
	ReturnAndBack();
	if($flag=="delete"){
		$qs = "delete from tbl_gm_tools_notice_unread where gmn_uId = $id and gms_sName = (select gm_name from tbl_session where UID = '$UID')";
		$rs = MySQLQuery($qs, "GMS");
		PrintNotice($tbl_sys_msg["success"]);
	}else{
		$qs = "select gmn_sTitle, gmn_sContent, gmn.gms_sName, gmn_dDate from tbl_gm_tools_notice gmn, tbl_gm_tools_notice_unread gmnr, tbl_session gms where gmn.gmn_uId = gmnr.gmn_uId and gmnr.gms_sName = gms.gm_name and gms.UID = '$UID' and gmn.gmn_uId = $id";
		$rs = MySQLQuery($qs, "GMS");
		if($row = mysql_fetch_row($rs)){
?>
		<form method="post" name="form1">
			<input type="hidden" name="UID" value="<?=$UID ?>" />
			<input type="hidden" name="id" value="<?=$id ?>" />
			<input type="hidden" name="flag" value="delete" />
			<table class="info_table">
				<tr>
					<th><?=$page_main["title"] ?></th>
					<td><?=$row[0] ?></td>
				</tr>
				<tr>
					<th><?=$page_main["content"] ?></th>
					<td><?=$row[1] ?></td>
				</tr>
				<tr>
					<th><?=$page_main["user_name"] ?></th>
					<td><?=$row[2] ?></td>
				</tr>
				<tr>
					<th><?=$tbl_sys_msg["time"] ?></th>
					<td><?=$row[3] ?></td>
				</tr>
			</table>
			<table class="form_table">
				<tr>
					<td style="text-align:center"><input type="submit" value="<?=$tbl_sys_msg["read"] ?>" /></td>
				</tr>
			</table>
		</form>
<?
		}
	}
?>
	</body>
</html>