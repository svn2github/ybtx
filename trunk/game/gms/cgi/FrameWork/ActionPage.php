#!/usr/bin/php
<?php
header("Content-type: text/html; charset=utf-8");
require_once("../../lib/gm_tools/Auth.php");
global $action_page,$action_form;
require_once("../../conf/gm_tools/Config.php");
global $sys_config;
require_once("../../lang/gm_tools/".$sys_config["lang"]."/sys_config.php");
require_once("../../lang/gm_tools/".$sys_config["lang"]."/page_localized.php");
global $page_title,$page_action_name,$page_main;
$url = $_SERVER["REQUEST_URI"];
$pos1 = strpos($url,"?name=")+6;
$pos2 = strpos($url,"?UID=");
$pos3 = strpos($url,"&gameId=")+8;
$page = substr($url,$pos1,$pos2-$pos1);
$UID = substr($url,$pos2+5,32);
$_REQUEST["UID"]=$UID;
$gameId = substr($url,$pos3);
Authenticate($_REQUEST,$gm);
?>
<html>
	<head>
		<title><?=$page_title[$page] ?></title>
		<link href="/css/action.css" rel="stylesheet" type="text/css" />
		<script type="text/javascript" src="/js/jquery-1.3.1.min.js"></script>
		<script type="text/javascript" src="/js/WebCalendar.js"></script>
		<script type="text/javascript">
			Calendar.language = {
				"year"   : [["<?=$tbl_sys_msg["year"] ?>"]],
				"months" : [["<?=implode("\",\"",$tbl_sys_msg["months"])?>"]],
				"weeks"  : [["<?=implode("\",\"",$tbl_sys_msg["weeks"])?>"]],
				"hour"  : [["<?=$tbl_sys_msg["hour"] ?>"]],
				"minute"  : [["<?=$tbl_sys_msg["minute"] ?>"]],
				"second"  : [["<?=$tbl_sys_msg["second"] ?>"]],
				"clear"  : [["<?=$tbl_sys_msg["clear"] ?>"]],
				"today"  : [["<?=$tbl_sys_msg["today"] ?>"]],
				"pickTxt"  : [["<?=$tbl_sys_msg["ok"] ?>"]],
				"close"  : [["<?=$tbl_sys_msg["close"] ?>"]]
			}
			
			$(function(){
				$("table.form_content:visible").css("display","none").find("input,textarea,select").attr("disabled","disabled");
				$("table.action_"+$("#content").val()).css("display","").find("input,textarea,select").removeAttr("disabled");
				$("#content").change(function(){
					$("table.form_content:visible").css("display","none").find("input,textarea,select").attr("disabled","disabled");
					$("table.action_"+$(this).val().replace(".","_")).css("display","").find("input,textarea,select").removeAttr("disabled");
				});
				$("textarea[name='role_name'], textarea[name='tong_name']").change(function(){Enter2Comma($(this));});
				$("form").submit(onsubmit);
				$(".checkbox_all").each(function(){
					var checkbox_all_id = $(this).attr("id");
					var action_id = checkbox_all_id.substring(0,checkbox_all_id.lastIndexOf("_"));
					$(this).change(function(){
						var state = $(this).attr("checked");
						$("."+action_id+"_checkbox").attr("checked",state);
					});
					$("."+action_id+"_checkbox").change(function(){
						var state = $(this).attr("checked");
						if(state){
							if($("."+action_id+"_checkbox:checked").length == $("."+action_id+"_checkbox").length){
								$("#"+action_id+"_all").attr("checked",true);
							}
						}else{
							$("#"+action_id+"_all").attr("checked",false);
						}
					});
				});
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
				document.form1.action_name.value = action;
			}
			
			function search(action, obj, flag){
				var str = $(obj).parents("td").find(".str").val();
				var div = $(obj).parents("td").find(".pop");
				var name =$(obj).parents("td").find(".str").attr("name");
				$(div).html("Loading.");
				$.ajax({
					type: "post",
					url: "/cgi-bin/gm_tools/FrameWork.php",
					data: "action_name="+action+"&UID="+$("input[name='UID']").val()+"&gameId="+$("input[name='gameId']").val()+"&str_match="+str,
					success:function(msg){
						var start = msg.indexOf("<error>");
						if(start==-1){
							if(flag == "item"){
								start = msg.indexOf("<select")+7;
								var end = msg.indexOf("</select>",start)+9;
								$(div).html("<select onchange='search_change(\""+name+"\",this)'"+msg.substring(start,end));
							}else if(flag == "scene"){
								start = msg.indexOf("<select")+7;
								var end = msg.indexOf("</select>",start)+9;
								$(div).html("<select onchange='search_scene_change(\""+name+"\",this)'"+msg.substring(start,end));
							}else{
								start = msg.indexOf("<select");
								var end = msg.indexOf("</select>",start)+9;
								$(div).html(msg.substring(start,end));
							}
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
			
			function search_scene_change(name, obj){
				$(obj).parents("table").find("input[name='x_pos']").val($(obj).find(":selected").attr("InitPosX"));
				$(obj).parents("table").find("input[name='y_pos']").val($(obj).find(":selected").attr("InitPosY"));
			}
		</script>
		<style>
		h1, h2 {
			width:703px;
			*width:705px;
		}
		.GMToolsNotice{
			position: absolute;
			background: #f5f5f5;
			border: 1px solid #ccc;
			color: #888;
			border-collapse:collapse;
			border-spacing: 0px;
		}
		.GMToolsNotice td{
			padding: 2px 5px;
		}
		.GMToolsNotice a{
			color: #000;
		}
		.GMToolsNotice tr:last-child{
			border-top: 1px solid #cccccc;
			text-align: right;
		}
		.GMToolsNotice tr:last-child a{
			cursor: pointer;
		}
		.server_item {
			float:left;
			width:90px;
		}
		.server_partition {
			width: 100%;
			clear: both;
			font-weight: bold;
			border-top: 1px dashed #ccc;
		}
		form {
			padding: 0px;
			margin: 0px;
		}
		</style>
	</head>
	<body>
<?
		ShowGMToolsNoticeList($gm);
?>
		<h1><?=$page_title[$page] ?></h1>
		<form method="post" name="form1" action="/cgi-bin/gm_tools/FrameWork.php">
			<input type="hidden" name="UID" value="<?=$UID ?>" />
			<input type="hidden" name="gameId" value="<?=$gameId ?>" />
			<table class="form_table">
				<tr>
					<th><?=$page_main["action"] ?></th>
					<td>
						<select id="content" name="action_name">
							<option selected><?=$page_main["select_action"] ?></option>
<?
foreach($action_page[$page] as $action){
	if(HasAuth($action, $gm)){
?>
							<option value="<?=$action ?>"><?=$page_action_name[$action] ?></option>
<?
	}
}
?>
						</select>
					</td>
				</tr>
			</table>
<?
foreach($action_page[$page] as $action){
	if($action_form[$action] == null){
	}else{
?>
			<table class="form_table form_content action_<?=str_replace(".","_",$action) ?>">
<?
		foreach($action_form[$action] as $item){
?>
				<tr>
					<th><?=$item[0] ?></th>
					<td>
						<?=$item[1]."\n" ?>
					</td>
				</tr>
<?
		}
?>
			</table>
<?
	}
}
?>
			<table class="form_table">
				<tr>
					<td style="text-align:center"><input type="submit" value="<?=$tbl_sys_msg["submit"] ?>" /></td>
				</tr>
			</table>
		</form>
	</body>
</html>