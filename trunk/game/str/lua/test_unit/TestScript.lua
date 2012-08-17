--将创建的test case放入一个table管理
g_TestCaseFun = {}

local TestCaseTbl = {
	--"test_unit/TestLogin",--测试登录,只是测试登录
	--"test_unit/TestLogoutAndLogin",--循环(登录,退出,登录)
	--"test_unit/TestLoginWait",--测试排队登录
	--"test_unit/TestChangeScene",--测试切场景
	--"test_unit/TestLoginRepeat",--测试顶号
	--"test_unit/TestLoginFlow",--目前没有应该.循环(登录游戏,切场景,下线,上线)
	"test_unit/TestMatchGameFb",--测试进比赛本
	--"test_unit/TestConsignment",--测试拍卖行
	--"test_unit/TestPurchasing",--测试求购行
}

for i=1, #(TestCaseTbl) do
	g_TestCaseFun[i] = str_require(TestCaseTbl[i])
end
