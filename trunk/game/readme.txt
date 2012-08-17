gac	game client			游戏客户端以及被客户端调用的程序
gas	game server			游戏服务器
tools	any other things		维护和开发用的工具


共用的代码必须放在专门的文件夹中，主要是方便打包工具提取文件
gac_gas

例如gac_gas是正确的，但是gas_gac是错误的，因为不符合上述的排列顺序
