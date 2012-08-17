#!/usr/bin/php
<html>
<title>欢迎进入 <?php require_once("../../conf/LogFree/config.php"); echo "$log_free_name"; ?> LogFree</title>
<head>
<link href="/css/logfree.css" type="text/css" rel="stylesheet"></link>
</head>
<body>
<h3>欢迎进入 <?php require_once("../../conf/LogFree/config.php"); echo "$log_free_name"; ?> LogFree</h3>
</head>
<body align=center>
<div align=center>
      <ul>
         <li class="item other item1"><a href='list_serv_log.php?page=1&order=1'>服务器端程序log</a></li>
         <li class="item other item2"><a href='list_client_log.php?page=1&order=1'>客户端程序log</a></li>
         <li class="item other item3"><a href='list_art_log.php?page=1&order=1'>美术资源log</a></li>
         <li class="item other item4"><a href='list_designer_log_c.php?order=1&page=1'>客户端配置表log</a></li>
         <li class="item other item5"><a href='list_designer_log_s.php?order=1&page=1'>服务器配置表log</a></li>
         <li class="item other item6"><a href='list_compatible_log.php?order=1&page=1'>兼容性错误log</a></li>
	 <li class="item other item7 "><a href='list_user_log.php?order=1&page=1' target='_blank'>用户提交log</a></li>
	 <li class="item other item4 "><a href='list_statistic.php'>查看统计</a></li>
      </ul>
</div>
</body>
</html>
