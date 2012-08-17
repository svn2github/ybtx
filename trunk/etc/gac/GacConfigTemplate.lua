GacConfig = 
{
	DefaultLoginConfig = 
	{
		"127.0.0.1:52520",							--默认(IP:Port)Infos[1]
		"",									--服务器名; 
		"",									-- 参数 < yy:1（""-佣兵专区,1-佣兵和yy混服,2-yy专区）>
	},
	IgnoreLoginAnimation = 1,	--0表示在登陆界面点击“确定”按钮后，连接服务器成功要播动画，1表示不播动画
	SetCameraMouseMoveRange = 15,	--旋转镜头鼠标按下移动的缓冲距离，大于这个距离才开始旋转镜头
	LogMemEnabled		= 1,	--是否每隔一段时间记录内存使用情况（1-记录，0-不记录）
	LogMemTime			= 300000,	--记录内存使用情况时间间隔
}
