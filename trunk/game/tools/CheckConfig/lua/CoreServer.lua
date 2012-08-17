require "common/Script/ScriptLib"

CoreServer={}
p=CoreServer

local function Run( FileName, uCyc )
	CoreServer=nil
	Run=nil

	--创建Server主对象
	local lServer=CBaseAppServer_Inst()
	
	--把该对象暴露给游戏逻辑
	g_App=lServer;
	g_ScriptApp=CBaseScriptApp_Inst()
	
	--载入游戏逻辑的主程序的文件
	
	local uTime=GetProcessTime()
	require(FileName)
	uTime = GetProcessTime() - uTime
	print( "Load script used " .. tostring(uTime) .. " milliseconds." )
	
	StartUp()
	
	lServer:Run()
	
	CleanUp()  
	
	g_App=nil
	
	
	print("全局对象数目：" .. getNumOfObjTable() .. "    传递给lua对象数目：" .. getNumOfCppTable())
	--db()

end

CoreServer=
{
	Run = Run
}

