#!/usr/bin/php

<html>
<title>统计</title>
<head>
<script type="text/javascript" src="/js/pager.js"></script>
<script type="text/javascript" src="/js/WriteStatus.js"></script>
<link href="/css/logfree.css" type="text/css" rel="stylesheet"></link>
</head>
<body align=center>
<h3>欢迎进入LogFree系统</h3>
<p>
      <ul>
         <li class="item current"><a href='list_serv_log.php?page=1&order=1'>服务器端程序log</a></li>
         <li class="item other item2 "><a href='list_client_log.php?page=1&order=1'>客户端程序log</a></li>
         <li class="item other item3 "><a href='list_art_log.php?page=1&order=1'>美术资源log</a></li>
         <li class="item other item4 "><a href='list_designer_log_c.php?order=1&page=1'>客户端配置表log</a></li>
         <li class="item other item5 "><a href='list_designer_log_s.php?order=1&page=1'>服务器配置表log</a></li>
	 <li class="item other item6 "><a href='list_compatible_log.php?order=1&page=1'>兼容性错误log</a></li>
	 <li class="item other item7 "><a href='list_user_log.php?order=1&page=1' target='_blank'>用户提交log</a></li>
	 <li class="item other item4 "><a href='list_statistic.php'>查看统计</a></li>
      </ul>
</p>
<br />
<h1>开发中</h1>
<p><a target='_blank' href = "total_statistic_c.php">客户端log饼图</a></p>
<p><a target='_blank' href = "line_statistic_c.php">客户端log折线图</a></p>
<p><a target='_blank' href = "total_statistic_s.php">服务器log饼图</a></p>
<p><a target='_blank' href = "line_statistic_s.php">服务器log折线图</a></p>
<p><a target='_blank' href = "error_info_statistic.php">log信息统计</a></p>
</body>
</html>
