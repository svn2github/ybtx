engine_require "common/Script/TimeHelper"

local function printGlobalReferenceTable()
	for k,v in pairs( __global_reference_table ) do
		--print( k,v )
		if( type(v) == "table" ) then
			for k1, v1 in pairs( v ) do
				--print( "", k1, v1, "\n" )
			end
		end
	end	
end

function getNumOfGlobalReferenceTable()
	local i = 0
	for k,v in pairs( __global_reference_table ) do
		i = i + 1
	end	
	
	return i
end

function getNumOfObjTable()
	local i = 0
	for k,v in pairs( __ObjTable ) do
		i = i + 1
	end	
	return i
end


function printTable(tbl)
	for k,v in pairs( tbl) do
		print(k )
	end
end


function getNumOfCppTable()
	local i = 0
	for k,v in pairs( __CppTable ) do
		i = i + 1
	end	
	return i
end

function getNumOfPtrIndexTable()
	local i = 0
	for k,v in pairs( __PtrIndexTable ) do
		i = i + 1
	end	
	
	return i
end

function print2DTable( tbl )
	for k,v in pairs( tbl )do
		--print( k,v )
		if( type(v) == "table" ) then
			for k1, v1 in pairs( v ) do
				--print( "", k1, v1, "\n" )
			end			
		end

	end
end


function printObjTable()
	print2DTable( __ObjTable )
end

function printPtrTable()
	print2DTable( __PtrIndexTable )
end


function printNormalTable( tbl )
	for k,v in pairs( tbl ) do
		--print( k,v )
	end
end

local function TravelTable(tbl) 
	for k, v in paris(tbl) do
		if type(v) == "table" then
			TravelTable(v)
		else
				
		end
	end
end

pause = debug.debug

function PrintUnRefTable()
	print("ref table未删除对象\n")
   	for k, v in pairs (__global_reference_table) do
		local refData = v.__ref_data
		print(tostring(refData), "  ref num: ", v.__ref_num)
		for key, value in pairs(refData) do
			if type(key) == "string" then
				print("  ", tostring(key), " = ", tostring(value))
			end
		end
		print("\n")
	end
	
end

function WriteToVarFile( sFileName, sMessage )

	local sActualFileName = CBaseScriptApp_Inst():GetRootFilePath( "/var/" .. sFileName )
	  
	local file = CLuaIO_Open( sActualFileName, "a" )
	  
	if file then
		file:WriteString( sMessage )
		file:Close()
	end

end

MemCheckTable = {}
setmetatable(MemCheckTable, {__mode = "v"})

function RegMemCheckTbl(tbl_name, tbl)
	if MemCheckTable.tbl_name then
		error("MemCheckTable表中 "..tbl_name.." 已经注册过了")
		return
	end
		
	MemCheckTable[tbl_name] = tbl
end

function TraverseMemCheckTable(MaxLevel)
	for k, v in pairs(MemCheckTable) do
		if not next(v) then
			MemCheckTable[k] = nil
		end
	end

	if not next(MemCheckTable) then
		return
	end

	local tid = GetCurrentThreadID()	
	local filename = "LuaMemTbl_" .. tid .. ".log"
	TraverseTable(MemCheckTable, filename, "a", MaxLevel)
end


function CheckCppObjMemLeak()
	collectgarbage("collect")
	collectgarbage("collect")

	local tid = GetCurrentThreadID()	
	local filename = "LuaCppObj_" .. tid .. ".log"
	
	if not next(CppObjMemLeakTbl) then
		return
	end
	
	local log = CLog_CreateLog(filename, "w")
	
	for k, v in pairs(CppObjMemLeakTbl) do
		log:Write(v)
	end
	
	log:Release()
	
end

local CheckFilterObj = {}

setmetatable(CheckFilterObj, {__mode = "kv"})
CheckFilterObj[CheckFilterObj] = CheckFilterObj

function AddCheckLeakFilterObj(obj)
	--如果是全局_G，那么不能加入，因为我们就是要从_G开始遍历
	if obj == _G then
		return
	end
	
	CheckFilterObj[obj] = obj
end

function CheckAllObjLeak(filename, MaxLevel)
	TraverseTable(_G, filename, "w", MaxLevel, CheckFilterObj)
end

local _class = class

function class(...)
	local obj =  _class(...)
	AddCheckLeakFilterObj(obj)
	return obj
end