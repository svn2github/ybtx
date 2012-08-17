engine_require "common/Script/ScriptLib"
engine_require "common/Script/ExportClass"
etc_com_require "DevConfig"

CoreClient={}
p=CoreClient

local function Run( FileName, uCyc )
	EnableCppMemLeakCheck(DevConfig.CheckMemLeak == 1)
	EnableExportClass(DevConfig.ExportClass == 1)
	
	CoreClient=nil
	Run=nil
	
	--创建Client主对象
	local lClient=CBaseAppClient_Inst()
	
	--把该对象暴露给游戏逻辑
	g_App=lClient;
	g_ScriptApp=CBaseScriptApp_Inst()
	
	--载入游戏逻辑的主程序的文件
	local uCounter=0;
	
	local r=require
	local nr=function( modname )
		uCounter = uCounter + 1
		return r(modname)
	end
	
	_G.require=nr
	
	--我们在载入文件之前先把全局空间里面的对象添加到leak filter里面去
	--这样我们进行内存泄露检查遍历全局空间的时候这些对象就不会遍历
	for k, v in pairs(_G) do
		AddCheckLeakFilterObj(v)
	end
	
	print("Main VM begin loading script files.")
	
	local uTime=GetProcessTime()	
	Require(FileName)	
	uTime = GetProcessTime() - uTime
	
	_G.require=r;	
	
	print( "Main VM loaded " .. tostring(uCounter) .. " script files in " .. tostring(uTime) .. " milliseconds." )
	
	StartUp()
	
	g_App:GetMainWnd():Show(true)
	
	lClient:Run()
	
	CleanUp()
	
	g_App=nil
	
	collectgarbage("collect")
	collectgarbage("collect")
	
	if DevConfig.CheckMemLeak == 1 then
		CheckCppObjMemLeak()
		TraverseMemCheckTable()
	end
end


CoreClient=
{
	Run = Run
}

