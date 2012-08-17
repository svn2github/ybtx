目录说明

bin		整个项目所有可执行文件的存放位置
engine		存放且只存放引擎代码
game		存放且只存放游戏逻辑的代码
implib		存放本项目用到的第三方库的头文件、库文件、安装文件。较大的第三方库，例如Direct3d,fmod等不存放在这里
etc		存放运行时静态配置文件，文件均不得在运行时修改
var		存放运行时可修改的文件，例如log文件，用户配置文件等。静态配置文件不要存放在这个目录。
tools 存放自动化工具


MakefileTemplate.include 文件里面放置了Linux环境下面的编译参数

Debian下：
  需要 cp MakefileTemplate.include Makefile.include
  33机器上需要修改
	GCC:=gcc $(COMMON_COMPILE_FLAG)  =>  GCC:=gcc-4.1 $(COMMON_COMPILE_FLAG)
	CC:=g++ $(COMMON_COMPILE_FLAG)  =>  CC:=g++-4.1 $(COMMON_COMPILE_FLAG)
	LIB_MYSQL:=-L/usr/local/lib/mysql -lmysqlclient -lz  =>  LIB_MYSQL:=-lmysqlclient -lz 
