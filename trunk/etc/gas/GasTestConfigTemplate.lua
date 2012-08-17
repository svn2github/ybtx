GasTestConfig =
{
	Need 		 			= 0,	--是否进行gas测试,1 表示运行测试用例。 0 表示服务器正常启动
	QuitWhenDone	= 0,	--跑完测试用力是否立即退出
	ConnCount 		= 0,	--需要多少client来连接才运行测试用例。 0 表示直接运行
	
	TestSuites=
	{
		InitGasGenEquipAttribTest,
		InitGasReloadTest,
		InitGasEventTest,
		InitGasAccountMgrTest,
		InitGasContextMgrTest,
		InitGasPropMgrTest,
		InitGasLogMgrTest,
		InitGasTextFilterMgrTest,
		InitGasSceneControl
	}
}