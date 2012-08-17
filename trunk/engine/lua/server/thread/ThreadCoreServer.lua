etc_com_require "DevConfig"
engine_require "common/Script/ScriptLib"
engine_require "common/Script/ExportClass"

ThreadCoreServer={}
p=ThreadCoreServer

local function Run( FileName )
	EnableCppMemLeakCheck(DevConfig.CheckMemLeak == 1)
	EnableExportClass(DevConfig.ExportClass == 1)

	ThreadCoreServer=nil
	Run=nil

	--创建Server主对象
	g_ThreadApp = CScriptThread_Inst()
	
	local lServer=g_ThreadApp
		
	local uCounter=0;
	
	local r=require
	local nr=function( modname )
		uCounter = uCounter + 1
		return r(modname)
	end
	
	_G.require=nr
	
	print("Child VM begin loading script files.")
	
	local uTime=GetProcessTime()	
	Require(FileName)	
	uTime = GetProcessTime() - uTime
	
	_G.require=r;	
	
	print( "Child vm loaded " .. tostring(uCounter) .. " script files in " .. tostring(uTime) .. " milliseconds." )
	
	StartUp()
	
	lServer:Run()
	
	CleanUp()  
	
	g_ThreadApp=nil
	
	collectgarbage("collect")
	collectgarbage("collect")
	
	if DevConfig.CheckMemLeak == 1 then
		CheckCppObjMemLeak()
		TraverseMemCheckTable()
	end
end

ThreadCoreServer=
{
	Run = Run
}

