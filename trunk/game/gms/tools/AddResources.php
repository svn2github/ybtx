#!/usr/bin/php
<?php
header("Content-type: text/html; charset=utf-8");
?>
<html>
	<body>
	<form action="AddResourceHere.php" method="POST">
		<table>
			<tr><td>Game ID:</td><td><input type='text' style='width:400px;' name='gameId' value='8'/></td></tr>
			<tr><td>Form Action:</td><td><input type='text' style='width:400px;' name='url' value='http://tools.linekong.com:8080/etoolkitsweb/addResource' /></td></tr>
			<tr><td>GMS host:</td><td><input type='text' style='width:400px;' name='gms' value='ybtxsl.linekong.cn'/></td></tr>
			<tr><td colspan='2'><input type=submit value=submit /></td></tr>
		</table>
	</form>
	</body>
</html>