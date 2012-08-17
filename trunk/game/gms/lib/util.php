<?php
function OutputXML($tblTitle, $tblContent)
{
	printf("<Response>\n");
	
	printf("<title>\n");
	for ($i=0; $i<count($tblTitle); $i++){
		printf("<name>%s</name>\n", $tblTitle[$i]);
	}
	printf("</title>\n");

	for ($j=0; $j<count($tblContent); $j++){
		printf("<result>\n");
		for ($k=0; $k<count($tblContent[$j]); $k++){
			printf("<info>%s</info>\n", $tblContent[$j][$k]);
		}
		printf("</result>\n");
	}

	printf("</Response>\n");
}
function OutputDBResult($num)
{
	printf("<Response>\n");
	printf("<title>\n<name>result</name>\n</title>\n");
	printf("<result>\n<info>%d</info>\n</result>\n", ($num>0));
	printf("</Response>\n");
}
function OutputError($errinfo)
{
	printf("<Response>\n");
	printf("<title>\n<name>result</name>\n</title>\n");
	printf("<result>\n<info>%s</info>\n</result>\n", $errinfo);
	printf("</Response>\n");
}
function RetXML($row)
{
	$str = "";
	$str=$str.sprintf("<?xml version='1.0' encoding='utf-8'?>\n");
	$str=$str.sprintf("<Response>");
	$str=$str.sprintf("<result>%d</result>", $row);
	$str=$str.sprintf("</Response>");
	header("Content-Length: ".strlen($str));
	header("Content-Type: text/xml");
	printf($str);
}
?>
