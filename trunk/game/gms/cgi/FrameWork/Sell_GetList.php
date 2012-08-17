#!/usr/bin/php
<!DOCTYPE YBTX-BuyCoupons>
<?php
require_once("../../conf/gm_tools/Config.php");
global $tbl_gm_db;

$dbinfo = $tbl_gm_db[0];
$Conn = mysql_connect($dbinfo[2],$dbinfo[0],$dbinfo[1]);
mysql_select_db($dbinfo[3],$Conn);

$qs = "select jsi_uId,jsi_sItemName,jsi_sTooltips,jsi_uSmallIcon,jsi_sURL,jsi_uPrice from tbl_joint_sales_items where jsi_uState = '1'";
echo "<root>\n";
$rs = mysql_query($qs) or print("<error code=\"-100\" msg=\"".mysql_error()."\" />");
while($row = @mysql_fetch_row($rs)){
	echo "<Item>\n";
	echo "<Id>".$row[0]."</Id>\n";
	echo "<ItemName>".$row[1]."</ItemName>\n";
	echo "<Tooltips>".$row[2]."</Tooltips>\n";
	echo "<SmallIcon>".$row[3]."</SmallIcon>\n";
	echo "<URL>".$row[4]."</URL>\n";
	echo "<Price>".$row[5]."</Price>\n";
	echo "</Item>\n";
}
echo "</root>";
?>