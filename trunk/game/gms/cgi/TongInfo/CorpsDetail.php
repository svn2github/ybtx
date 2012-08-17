<?php
function RetTongDetail($tbl_content,$var)
{
	global $tbl_camp, $tbl_tong_detail,$tbl_tong_msg;
	echo "<table class='form_table'>";
	echo sprintf("<tr><th>%s</th><td><b>%s</b></td></tr>", $tbl_tong_detail[0], $tbl_content[0]);
	echo sprintf("<tr><th>%s</th><td>%s</td></tr>", $tbl_tong_detail[1], $tbl_content[1]);
	echo sprintf("<tr><th>%s</th><td>%s</td></tr>", $tbl_tong_detail[2], $tbl_camp[$tbl_content[2]]);
	echo sprintf("<tr><th>%s</th><td>%d</td></tr>", $tbl_tong_detail[3], $tbl_content[3]);
	echo sprintf("<tr><th>%s</th><td>%s</td></tr>", $tbl_tong_detail[4], $tbl_content[4]);
	echo sprintf("<tr><th>%s</th><td>%d</td></tr>", $tbl_tong_detail[5], $tbl_content[5]);
	echo sprintf("<tr><th>%s</th><td>%d</td></tr>", $tbl_tong_detail[6], $tbl_content[6]);
	echo sprintf("<tr><th>%s</th><td>%d</td></tr>", $tbl_tong_detail[7], $tbl_content[7]);
	echo sprintf("<tr><th>%s</th><td>%d</td></tr>", $tbl_tong_detail[8], $tbl_content[8]);
	echo sprintf("<tr><th>%s</th><td>%s</td></tr>", $tbl_tong_detail[9], $tbl_content[9]);
	echo sprintf("<tr><th>%s</th><td>%d</td></tr>", $tbl_tong_detail[10], $tbl_content[10]);
	echo sprintf("<tr><th>%s</th><td>%s</td></tr>", $tbl_tong_detail[11], $tbl_content[11]);
	echo sprintf("<tr><th>%s</th><td>%s</td></tr>", $tbl_tong_detail[12], $tbl_tong_msg["war_zone"][$tbl_content[12]]."-".$tbl_tong_msg["station_name"][$tbl_content[13]]);
	echo sprintf("</table>");   
	ReturnAndBack();            
}                             
function GetTongDetail($var, $gm)
{
	$id = intval($var["id"]);
	$serverid=intval($var["serverid"]);
	$query_string = "select t_sName, c.c_sName, t_uCamp, t.t_uId, t_dtCreateTime, t_uHonor, t_uLevel, t_uMoney, t_uResource, t_sPurpose, t_uDepotId, count(mt.cs_uId), ifnull(wz_uId, 0), ifnull(wz.wzs_uIndex, 0) from tbl_tong t left join tbl_war_zone_station wz on t.t_uId = wz.t_uId, tbl_member_tong mt, tbl_member_tong o, tbl_char c where o.t_uId=t.t_uId and o.mt_sPosition=5 and o.cs_uId=c.cs_uId and t.t_uId=mt.t_uId and t.t_uId='$id' group by t.t_uId";
	$result = MySQLQuery($query_string,"Game",$serverid);
	$tbl_content=array();
	while($row=mysql_fetch_row($result)){
		$tbl_size=count($tbl_content);
		$tbl_content[$tbl_size]=$row;
	}
	RetTongDetail($tbl_content[0],$var);
	return true;
}
?>
