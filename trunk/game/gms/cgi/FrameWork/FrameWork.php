#!/usr/bin/php
<?php
if (isset($_REQUEST["download"]) && ($_REQUEST["download"] == 'yes'))
{
	header("Content-type: application/octet-stream");
	header("Content-Disposition: attachment; filename=download.csv");
}
else
	header("Content-type: text/html; charset=utf-8");

require_once("../../conf/gm_tools/Config.php");
require_once("../../lib/gm_tools/Auth.php");
require_once("../../lib/gm_tools/Lib.php");
require_once("../../lib/gm_tools/Management.php");
require_once("../../lib/gm_tools/NpcManage.php");
require_once("../../lib/gm_tools/UserRoleList.php");
require_once("../../lib/gm_tools/RoleDetail.php");
require_once("../../lib/gm_tools/UserSocialInfo.php");
require_once("../../lib/gm_tools/UserLevelInfo.php");
require_once("../../lib/gm_tools/ReqAction.php");
require_once("../../lib/gm_tools/TaskInfo.php");
require_once("../../lib/gm_tools/GetAllCompensate.php");
require_once("../../lib/gm_tools/OccuptationTotal.php");
require_once("../../lib/gm_tools/GetRoles.php");
require_once("../../lib/gm_tools/GetRoleFriends.php");
require_once("../../lib/gm_tools/DealCompensate.php");
require_once("../../lib/gm_tools/GetAllComplaints.php");
require_once("../../lib/gm_tools/GetAllGMLog.php");
require_once("../../lib/gm_tools/CorpsTotal.php");
require_once("../../lib/gm_tools/CorpsMember.php");
require_once("../../lib/gm_tools/CorpsDetail.php");
require_once("../../lib/gm_tools/AnyliseLog.php");
require_once("../../lib/gm_tools/GetGMMgr.php");
require_once("../../lib/gm_tools/BulletinMgr.php");
require_once("../../lib/gm_tools/GetMoneyCorrencyLimit.php");
require_once("../../lib/gm_tools/GetJifenUseLimit.php");
require_once("../../lib/gm_tools/GetItemUseLimit.php");
require_once("../../lib/gm_tools/GetCmpstByGM.php");
require_once("../../lib/gm_tools/VirWealthMonitor.php");
require_once("../../lib/gm_tools/GameWorldManagement.php");
require_once("../../lib/gm_tools/ServerMgr.php");
require_once("../../lib/gm_tools/TalkMgr.php");
require_once("../../lib/gm_tools/UserAction.php");
require_once("../../lib/gm_tools/GetBackEquip.php");
require_once("../../lib/gm_tools/ShowMoneyLogDetail.php");
require_once("../../lib/gm_tools/PeriodicTask.php");
require_once("../../lib/gm_tools/MoneyLog.php");
require_once("../../lib/gm_tools/DesignerData.php");



$ReqHandler=array(
	"1"  => "OutFunc",
	"2"  => "LeftUserInfo",
	"3"  => "LeftRoleInfo",
	"4"  => "LeftItemFlow",
	"5"  => "LeftRoleMgr",
	"6"  => "LeftBulletinMgr",
	"7"  => "LeftRoleCompansate",
	"8"  => "LeftUserCompansate",
	"9"  => "LeftDealCompensate",
	"10" => "LeftNpcMgr",
	"11" => "LeftActiveMgr",
	"12" => "LeftViewComplaint",
	"13" => "LeftGameWorldInfo",
	"14" => "LeftGMLog",
	"15" => "LeftAnyliseLog",
	"16" => "LeftDataMonitor",
	"21" => "GetRolesByBindingMoney",
	"22" => "GetRolesByLevel",
	"23" => "GetRolesByChannel",
	"24" => "GetRolesBySceneName",
	"25" => "Test",
	"26" => "ShutUp",
	"27" => "CancelShutUp",
	"28" => "KickOff",
	"29" => "MoveRole",
	"30" => "SendWarning",
	"31" => "FreezeUrs",
	"32" => "UnFreezeUrs",
	"33" => "ReqAddExp",
	"34" => "ReqAddMoney",
	"35" => "ReqAddTicket",
	"36" => "ReqAddQuest",
	"37" => "ReqFinishQuest",
	"38" => "ReqAddEquip",
	"39" => "ReqAddItem",
	"40" => "ReqDelQuest",
	"41" => "ReqAddSoul",
	"42" => "ReqAddGMMgr",
	"43" => "ReqMoneyCurrencyLimit",
	"44" => "ReqJifenUseLimit",
	"45" => "ReqItemUseLimit",
	"46" => "NpcInfo",
	"50" => "GetRoleList",
	"51" => "GetLoginInfo",
	"52" => "GetUserByIP",
	"53" => "GetAllCompensate",
	"54" => "PassCompensate",
	"55" => "CancelCompensate",
	"56" => "DelItemInBag",
	"57" => "DelItemOnBody",
	"58" => "GetAllComplaints",
	"59" => "PassComplaint",
	"60" => "CancelComplaint",
	"61" => "ClearState",
	"62" => "GetGMActionLog",
	"63" => "GetTongInfo",
	"64" => "GetTongMember",
	"65" => "GetTongDetail",
	"66" => "ShowItemLog",
	"67" => "ShowVirWealthLog",
	"70" => "ShowVirWealthLog_Item",
	"71" => "ShowItemInOutLog",
	"72" => "ShowItemInOutDetail",
	"73" => "ShowTongMemberLog",
	"74" => "ShowTongItemLog",
	"75" => "ShowNpcOutLog",
	"76" => "ShowSceneOut",
	"77" => "ShowSceneOutDetail",
	"78" => "DelItemInDepot",
	"79" => "DelItemInTongDepot",
	"81" => "ListBulletin",
	"82" => "PostBulletin",
	"83" => "ViewBulletin",
	"84" => "DelBulletin",
	"85" => "GetGoodsHistory",
	"88" => "GetGMMgrInfo",
	"89" => "DelGMMmr",
	"90" => "GetMoneyCorrencyLimit",
	"91" => "GetJifenUseLimit",
	"92" => "GetItemUseLimit",
	"93" => "ReqMoneyCurrencyLimitCanel",
	"94" => "ReqJifenUseLimitCanel",
	"95" => "ReqItemUseLimitCanel",
	"96" => "ChangeGMLevel",
	"97" => "GetRoleAllDetail",
	"99" => "GetTaskInfo",
	"100"=> "GetFinishedTask",
	"101"=> "GetOnGoingTask",
	"102"=> "GetRoleItemDetail",
	"103"=> "GetOccuptationTotal",
	"104"=> "GetRolesByMoney",
	"105"=> "NpcNameMatch",
	"120"=> "ItemNameMatch",
	"121"=> "QuestNameMatch",
	"122"=> "MatchScene",
	"123"=> "GetCmpstByGM",
	"124"=> "GMCancelCmpst",
	"125"=> "PassBulletin",
	"140"=> "DownloadRoleList",
	"141"=> "GetRoleEquipByID",
	"147"=> "ReqSendMail",
	"148"=> "LeftGameWorldMgr",
	"149"=> "ReqQuestFinishLimit",
	"151"=> "ReqObjDropLimit",
	"150"=> "ReqQuestFinishLimitCanel",
	"152"=> "ReqObjDropLimitCanel",
	"153"=> "ServerMgr",
	"154"=> "HotUpdate",
	"155"=> "HotUpdateResult",
	"156"=> "HotUpdateLog",
	"157"=> "LeftTalk",
	"158"=> "TalkWithRole",
	"159"=> "AllMsg",
	"160"=> "LoadTalkMsg",
	"161"=> "ChannelWordMgr",
	"162"=> "ChannelWordIns",
	"163"=> "ChannelWordDel",
	"164"=> "ChannelChat",
	"165"=> "ChannelChatAll",
	"166"=> "GetBackEquipFun",
	"167"=> "GetPeriodAction",
	"168"=> "GetActionRelatedInfo",
	"169"=> "TongRename",
	"170"=> "RemoveTongMember",
	"171"=> "ChangeTongMember",
	"172"=> "ChangeTongLevel",
	"175"=> "ReqAddItemToTong",
	"177"=> "ShowNPCDropLog",
	"178"=> "GetTongDepot",
	"179"=> "RoleRename",
	"182"=>	"ShowVirWealthLog_Level",
	"184"=> "RoleToUser",
	"185"=> "LeftUserMgr",
	"186"=> "DelChar",
	"187"=>	"GetBackRole",
	"191"=> "ClearItemInBag",
	"192"=> "ClearItemInDepot",
	"193"=>	"RoleSearch",
	"194"=> "ShowJifenLog",
	"195"=> "ShowJifenLogDetail",
	"196"=> "ReqAddBindingMoney",
	"197"=> "ReqAddBindingSoul",
	"198"=> "GetServerDate",
	"199"=> "GetFriends",
	"200"=> "GetRolesByOnline",
	"201"=> "GetRoleItems",
	"202"=> "GetQuestFinishLimit",
	"203"=> "GetObjDropLimit",
	"204"=>	"GetRoleRealTimeData",
	"205"=> "GetBackEquip",
	"206"=> "ShowVirWealthLog_Money",
	"207"=> "AuthManagement",
	"208"=> "AuthMgrFun",
	"209"=> "ChannelChatAllReload",
	"210"=> "ChannelChatReload",
	"211"=> "MoneyLog",
	"221"=>	"MoneyLog_2_6",
	"222"=>	"MoneyLog_2_7",
	"229"=>	"MoneyLog_3_7",
	"233"=>	"ServerInfo",
	"234"=> "ShowItemFlowLog",
	"235"=> "ShowVirWealthLog_Experience",
	"236"=> "ShowVirWealthLog_Guogong",
	"237"=> "ShowVirWealthLog_Jifen",
	"238"=> "SearchTaskInfo",
	"239"=>	"OnlineIP",
	"240"=>	"OnlineIPSearch",
	"241"=>	"LastLogDate",
	"242"=> "RoleLostEquip",
	"243"=>	"ShowVirWealthLog_Soul",
	"244"=>	"GetItemCfg",
	"245"=>	"GetTaskCfg",
	"246"=>	"GetNPCCfg",
	"247"=> "ShowVirWealthLog_Market",
	"248"=> "ReqAddMoneyToTong",
	"249"=>	"ReqAddResourceToTong",
	"250"=>	"ReqAddHonorToTong",
	"251"=> "GMToolsNoticeInsert",
	"252"=>	"RushRoleConfig",
	"253"=>	"ShowEquipUpgrade",
	"254"=> "RushRoleState",
	"255"=>	"GetRoleSceneLog",
	"256"=>	"ShowVirWealthLog_MarketBuyOrder",
	"257"=> "ShowPlayerLatency",
	"258"=> "GetTongItemLog",
	"259"=>	"ServerConfig",
	"260"=> "RushRoleServerConfig",
	"261"=> "GetTongLog",
	"262"=> "GetTongLog_Member",
	"263"=> "GetTongLog_Technology",
	"264"=>	"GetTongLog_Build",
	"265"=>	"GetTongLog_Produce",
	"266"=> "SetServerType",
	"267"=>	"JointSales_ItemEdit",
	"268"=>	"JointSales_InsertSN",
	"269"=>	"JointSales_ItemList",
	"270"=>	"JointSales_ItemLog",
	"271"=>	"JointSales_ChangeState",
	"272"=> "ShowServerType",
	"273"=> "SetRecruitMethod",
	"274"=> "SetRecruitInfo",
	"275"=> "GetRecruitSetting",
	"276"=>	"LoginIP",
	"277"=>	"LoginIPSearch",
	"278"=>	"GetTongLog_StationMove",
	"279"=>	"GetTongLog_Challenge",
	"280"=>	"ChannelChatAllLog",
	"281"=>	"GetRoleTeamLog",
	"282"=>	"SetMaxOnLineUserNum",
	"283"=>	"ReqAddToTong",
	"284"=>	"ServerPartition",
	"285"=>	"RushRoleLog",
	"286"=>	"ShowVirWealthLog_CommerceSkill",
	"287"=>	"GetTongLog_Boss",
	"288"=>	"UserTrust",
	"289"=>	"CancelUserTrust",
	"290"=>	"ShowVirWealthLog_ModulusChange",
	"291"=>	"SetSearchServer",
);


function HandleRequest($var, $gm)
{
	global $ReqHandler;
	return $ReqHandler[floor($var["action_name"])]($var, $gm);
}

function FrameWork($var){
	global $tbl_error_msg,$sys_config;
	
	$gm = "gm";
	if ($sys_config["NeedAuth"]){
		if (!Authenticate($var, $gm)){
			PrintNotice("<p>You have none login.</p>");
			return;
		}
		if (!CheckPrevilege($var)){
			PrintNotice($tbl_error_msg["NoPriv"]);
			return;
		}
	}
	if (!HandleRequest($var, $gm)){
		PrintNotice($tbl_error_msg["ActionFail"]);
		return;
	}
	return;
}
$b = (isset($_SERVER['HTTP_X_REQUESTED_WITH']) && (strtolower($_SERVER['HTTP_X_REQUESTED_WITH']) == 'xmlhttprequest'));
if(!$b){
?>
<html>
	<head>
		<script src='/js/jquery-1.3.1.min.js'></script>
		<script src='/js/jquery.flot.min.js'></script>
		<script src='/js/jquery.flot.pie.min.js'></script>
		<style>
			* {
				font-size: 12px;
			}
			h1, h2 {
				width:703px;
				*width:705px;
				height:20px;
				padding:0px;
				padding-top:5px;
				margin-bottom:0px;
				font-size: 12px;
				border: #6a90b4 solid 1px;
				border-bottom: 0px;
				background: url(/css/head_back.gif) repeat-x;
				text-align:center;
				color:#333333;
			}
			form {
				padding: 0px;
				margin: 0px;
			}
			table {
				border-collapse:collapse;
				border-spacing: 0px;
			}
			.inner_table td {
				height: auto !important;
				border: 0px !important;
			}
			.form_table {
				width: 705px;
				padding:0px;
				margin:0px;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 0px 0px 1px;
			}
			
			.form_table th {
				width: 250px;
				height:30px;
				padding: 0px;
				font-weight:normal;
				text-align: right;
				vertical-align: middle;
				color:#333333;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
			}
			
			.form_table td {
				height:30px;
				padding: 0px;
				vertical-align: middle;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
			}
			
			.info_table {
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
				padding: 0px;
				padding-left: 5px;
				vertical-align: middle;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
			}
			
			.list_table {
				width: 705px;
				padding:0px;
				margin:0px;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 0px 0px 1px;
			}
			
			.list_table th {
				background: #cde4f6;
				color: #333333;
				padding:5px 3px;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
			}
			
			.list_table td {
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 0px;
				padding:0px 3px;
				vertical-align: middle;
				height:30px;
			}
			
			.list_table tr:hover td {
				background: #ebe9e9;
			}
			
			.list_table tr.even td {
			}
			
			.list_table tr th.order {
				cursor: pointer;
				background: url(/css/OrderDefault.gif) right no-repeat #cde4f6;
			}
			.list_table tr th.order:hover, .list_table tr th.asc, .list_table tr th.desc:hover {
				cursor: pointer;
				background: url(/css/OrderAsc.gif) right no-repeat #cde4f6;
			}
			.list_table tr th.asc:hover, .list_table tr th.desc {
				cursor: pointer;
				background: url(/css/OrderDesc.gif) right no-repeat #cde4f6;
			}
			.list_table tr th div.filter {
				position:absolute;
				width: inherit;
				padding: 3px;
				margin-left: -4px;
				text-align: left;
				font-weight: normal;
				background: #cde4f6;
				border:#aeaeb0 solid 1px;
				border-top:0px;
			}
			.list_table tr th div.filter a {
				color: #333333;
			}
			
			.log_info {
				background: #cde4f6;
				display: none;
				position: absolute;
				padding: 5px;
				border:#aeaeb0 solid 1px;
			}
			
			.notice {
				width:693px;
				padding:5px;
				border:1px solid #aeaeb0;
			}
			
			input[type=text] {
				width: 200px;
			}
			input[type=checkbox] {
				width: 10px;
				height: 10px;
			}
			textarea {
				width: 400px;
				height: 70px;
			}
			
			img {
				max-width:703px;
				border-color:#aeaeb0;
				border-style:solid;
				border-width:0px 1px 1px 1px;
			}
			
			.chart {
				float: right;
				margin: 10px 0px;
			}
			.chart td {
				border: 0px;
				background: none;
			}
			
			.base64 {
				display: block;
				color: #ccc;
			}
		</style>
	</head>
	<body>
<?
	if($_REQUEST["serverindex"]==null || $_REQUEST["serverindex"]==""){
		if($_REQUEST["role_item"]=="id"){
			$_REQUEST["role_name"] = GetRoleName($_REQUEST["role_str"],$_REQUEST["serverid"]);
			$_REQUEST["role_id"] = $_REQUEST["role_str"];
		}elseif($_REQUEST["role_item"]=="name"){
			$_REQUEST["role_id"] = GetRoleId($_REQUEST["role_str"],$_REQUEST["serverid"]);
			$_REQUEST["role_name"] = $_REQUEST["role_str"];
		}
		echo "		<h1>".$page_action_name[floor($_REQUEST["action_name"])]."</h1>\n";
		ReturnAndBack();
	}
}
FrameWork($_REQUEST);
if(!$b){
?>
	</body>
</html>
<?
}
?>
