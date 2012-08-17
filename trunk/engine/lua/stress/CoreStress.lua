engine_require "common/Script/ScriptLib"
engine_require "common/Script/ExportClass"
etc_com_require "DevConfig"

CoreStress={}
p=CoreStress


local function Run( FileName, uCyc )
	EnableExportClass(DevConfig.ExportClass == 1)

	CoreStress=nil
	Run=nil
	
	--创建Client主对象
	local lStress=CBaseAppStress_Inst()
	
	--把该对象暴露给游戏逻辑
	g_App=lStress;
	
	--载入游戏逻辑的主程序的文件
	local uTime=GetProcessTime()
	
	Require(FileName)
	
	uTime = GetProcessTime() - uTime
	
	print( "script load time:" ..tostring(uTime) );
	
	StartUp()
	
	lStress:Run()
	
	CleanUp()
	
	g_App=nil
end


CoreStress=
{
	Run = Run
}

