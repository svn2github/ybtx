#!/usr/bin/php
<?php
header("Content-type: application/octet-stream");
header("Content-Disposition: attachment; filename=".$_REQUEST["display"].".csv");

$filename = "../../".$_REQUEST["file"].".csv";
$file = fopen($filename,"r");
echo fread($file,filesize($filename));
fclose($file);
unlink($filename);
?>