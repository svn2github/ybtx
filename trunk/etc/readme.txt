gac	game client			游戏客户端以及被客户端调用的程序
gmc	gm client			GM客户端
gas	game server			游戏服务器
rcp	reception server		接待服务器
cis	central infomation server	中央信息服务器
tools	any other things		维护和开发用的工具
common	存放最共用的代码

共用的代码必须放在专门的文件夹中，主要是方便打包工具提取文件
gac_gas
gac_rcp
gmc_gas



共用代码的目录名字的排列顺序
gac gmc gas cis rcp

例如gac_gas是正确的，但是gas_gac是错误的，因为不符合上述的排列顺序






规则：
1：全部以Template结尾的配置文件，在本地都要拷贝一份。表示每台机器的配置可能不一样

truck\etc\common目录：
	DirectoryTemplate.xml 配置策划文档路径和data路径
	
truck\etc\gac目录：
	GacConfig.xml  				 		 gac程序配置,由c++读取
	GacTestConfigTemplate.lua  gac的lua 测试用例配置
	GacConfigTemplate.lua  		 gac默认启动参数配置
	
truck\etc\gas目录：
	GasConfig.xml							 gas程序配置,由c++读取
	GasConfigTemplate.lua      gas启动端口、mysql端口，ip配置、跳过gbs认证配置
	GasTestConfigTemplate.lua  gas的lua 测试用例配置
	
truck\etc\rpc目录：
	RcpConfig.xml							 rcp程序配置,由c++读取
	
truck\etc\tools目录：
  TestSuiteTemplate.xml      engine test 测试用例配置
