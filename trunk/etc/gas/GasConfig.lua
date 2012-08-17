GasConfig =
{
	MysqlConnNum		= 2,					--mysql的连接数
	SkipGBSAuth 		= 1,                --设置是否要跳过GBS登录认证（1-跳过，0-不跳过）
	GBSAuthType		= 1,                --erating登陆验证方式（0-联运认证,1-混服,2-yy专区）
	EratingIBPaySend	= 1,							--erating消费消息发送方式（0--不发送，1--起服连上erating[serverid == 1]就注册tick开始发送，3--只有消费的时候注册tick发送）
	EratingIBPaySendNum = 20,				--erating消费每次最多发送多少条
	EratingExchangeSendNum = 10,		--erating金钱流通每次最多发送多少条
	SkipAdultCheck = 1,		--是否跳过防沉迷验证(1--跳过验证，2--不跳过)
	IgnoreRcp       = 1,
	GMValidate		= 0,		--是否进行gm指令权限验证，0不需要验证，1需要验证
	StopServerCountDown = 0,			--关服倒计时
	TestDropRate       = 0,       --设置是否测试掉落概率（1-测试，0-不测试）
	OpenCfgSwitch		= 1,					--设置是否开启配置表检测（1-开启，0-关闭）
	PreloadAllScene       = 0,       --设置场景载入方式（0-开发期:首次进入场景时载入，1-运营期:预载所有场景）
	DBCoroutine		= 1,	--设置Ctrl+C事退出程序还是进入GDB进行调试（1-退出， 0-调试）
	ApexProxyAddress	= "127.0.0.1",		--防外挂系统
	ApexProxyPort 		= 15306,
	GameWebServerIp		= "0.0.0.0",		--web server模块侦听IP
	GameWebServerPort	= 10077,		--web server模块侦听端口
	LogoutTime				= 300000,	--设置玩家登出5分钟之内再登录游戏跳过登录排队。
	CISCheck			= false,
	EServiceCheck = true,
	SceneObjLimit		= true,			--是否开启场景对象最大数目限制
	MaxDBResultSize 	= 3369600,		--db result结果集最大大小，超过这个值记录log
	SortTime = 7200000,  --每隔两小时刷一次排行榜
	StatisticDBTrans = 0,       --是否开服就开始统计transation调用的时间和次数（1-测试，0-不测试）
	StatisticDBCalls = 0,		--是否开服就开始统计sql语句时间和次数（1-测试，0-不测试）
	RequireDbBoxFile = 0,	--是否开服加载所有DB文件（1-加载，0-不加载）
	
	MaxSynLoginNum = 20,
	MaxSynChangeNum = 40,
	AllCpuLimit = 800,	--即当某机器总cpu使用率大于 AllCpuLimit 时候 角色不可切入该机器上的所有主场景,
	MaxThreadCpuLimit = 10000000, --即当进程中有某个线程cpu使用率大于MaxThreadCpuLimit 时候, 角色不可切入该进程中的所有主场景
	
	RecruitTime = 720,  --重金招募自动更新时间(分钟)
	AccountTransWatchTime = 5,	--帐号执行DbTrans的监控时间
	AccountTransCount = 100,	--帐号单位时间内允许执行DbTrans次数
	
	GasType = 0,  --服务器类型,  0 为正常服务器,  1为pvp联赛服务器,
	
	StressCheck = 0,--压测开关,(0表示不向客户端发压测的rpc消息;1表示发消息)如果压测的时候,改为1就行.
	BuChangCheck = 0,
}

CISConfig = {
	CISHost = "10.10.43.142",
	CISPort = "80",
	CISAddRole_url = "/cgi-bin/cis/AddNewRole.php",
	CISCheckName_url = "/cgi-bin/cis/CheckName.php",
	CISRoleRename_url = "/cgi-bin/cis/ChangeRoleName.php",
	CISDelRole_url = "/cgi-bin/cis/DelRole.php",
	CISAddUser_url = "/cgi-bin/cis/AddNewUser.php",
	CISReuseRole_url = "/cgi-bin/cis/ReuseRole.php",
}


EServiceConfig = {
	Host = "59.151.39.146",
	Port = "8080",
	InsertQuestion = "/eService/system/insertQuestionFromClient.do",
	QueryQuestionList = "/eService/system/listQueryGameQuestions.do",
}
