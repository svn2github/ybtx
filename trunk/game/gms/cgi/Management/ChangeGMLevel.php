<?php
header("Content-type: text/html; charset=utf-8");
?>
<html>
	<head></head>
		<body>
			<h3>Level: </h3>
			<form action = "/cgi-bin/gm_tools/FrameWork.php" name = "from0" id = "form0" method = "get">
				<input name='gm_level' type='input'/>
				<?php
	require("Lib.php");
	echo "<input type = 'submit' size = 18 name = 'submit' value = 'submit'/>";
	printf(SetCommonVar($_REQUEST));
	printf("<p><input type='hidden' name='user_name' value='%s'/></p>", $_REQUEST["id"]);
	printf("<p><input type='hidden' name='game_id' value='%s'/></p>", $_REQUEST["game_id"]);
				?>
				<p><input type="hidden" name="action_name" value = "96"/></p>
			</form>
		</body>
	</html>
		
