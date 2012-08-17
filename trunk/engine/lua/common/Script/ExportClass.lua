--[[
	dynamic export class information
	
	ClassName, FileName, BaseClassName1, BaseClassName2, ...
	
	
	local tbl = {
		{"ClassName", "framework/main_frame/ClassName", "BaseName1", "BaseName2"},
		......
	}
--]]

--[[
	对于lua的export class，有两类
	第一种是c++注册的class，这类class本来就在lua里面有对应的数据了，
	而且因为class new出来的对象可能是table也可能是userdata，所以我们得通过
	替换class的new以及class的metatable来进行export设置
	
	第二种是lua使用class函数注册的class，这类我们使用一个dummy class来表示，
	因为他的定义是在我们要require的定义文件里面的，等我们require该文件之后，
	我们会将dummy替换成实际的class
--]]

local eSCT_Isolated = EScriptClassType.eSCT_Isolated
local eSCT_Static = EScriptClassType.eSCT_Static
local eSCT_Dynamic = EScriptClassType.eSCT_Dynamic
local eSCT_Share = EScriptClassType.eSCT_Share

local str_len = string.len

local ExportClassTbl = {}

local function IsCppClass(Class)
	if rawget(Class, "__info") then
		return true
	end
	
	return false
end


local function _ExportClass(Class)
	local Info = rawget(Class, "__ExportInfo")
	
	local ClassName = rawget(Class, "__ClassName")
	
	if not Info then
	    if ClassName then
	        return _G[ClassName]
	    else
	        return Class
	    end
	end
			
	--如果有export info消息，我们需要export class了
	rawset(Class, "__ExportInfo", nil)
	
	local bCppClass = IsCppClass(Class)
	if bCppClass then
		local ClassNew = rawget(Class, "__orign_new")
		rawset(Class, "new", ClassNew)
		rawset(Class, "__orign_new", nil)

		local ClassIndex = rawget(Class, "__orign_index")
		rawset(Class, "__index", ClassIndex)
		rawset(Class, "__orign_index", nil)
		
		local ClassCall = rawget(Class, "__orign_call")
		rawset(Class, "__call", ClassCall)
		rawset(Class, "__orign_call", nil)
	
		local ClassMt = rawget(Class, "__orign_mt")
		setmetatable(Class, ClassMt)
		rawset(Class, "__orign_mt", nil)			

		RequireFile(Info[1], Info[2])
		
		ClassMt.__newindex = function(tbl, key, value)
			error("不允许往只读表里面添加数据")
		end

		return Class
	else
		--如果是lua类，那么我们首先将dummy class清除出全局空间
		
		_G[ClassName] = nil

		--载入文件
		RequireFile(Info[1], Info[2])			
		
		--得到实际的lua类
		local NewClass = _G[ClassName]
		
		NewClass["__ClassName"] = ClassName
		
		assert(NewClass)
		
		--载入完成之后我们的类就是只读的了
		local NewClassMt = getmetatable(NewClass)
		
		NewClassMt.__newindex = function(tbl, key, value)
			error("不允许往只读表里面添加数据")
		end
	
		
		return NewClass
	end
end

local ExportFileName = {}

local function _DynamicExportClass(Class)
	local NewClass = _ExportClass(Class)
	
	local BaseClassList = rawget(NewClass, "__base_list")
	if BaseClassList then
		local n = #BaseClassList
		for i = 1, n do
			local BaseClass = BaseClassList[i]
			BaseClassList[i] = _ExportClass(BaseClass)
		end
	end
	
	return NewClass
end

local function _RegExportClass(ClassName, FileName)
	assert(type(ClassName) == "string")
	assert(type(FileName) == "string")
	
	if ExportFileName[FileName] then
		error(FileName .. "已经被注册了，一个文件只允许注册一个类")
	end
	
	ExportFileName[FileName] = true	
		
	if ExportClassTbl[ClassName] then
		error(ClassName .. "已经设置成导出类，不允许再次设置")
	end
	
	ExportClassTbl[ClassName] = true
end

local function _CheckCppClass(ClassName)
	--如果这个类是c++类，那么程序启动的时候已经存在与全局表里面了
	if ClassObj then
		if not rawget(ClassObj, "__info") then
			error(ClassName .. "存在与全局空间并且不是注册的C++类")		
		end
		
		local ClassType = rawget(ClassObj, "__ClassType")
		if ClassType == eSCT_Share then
			error(ClassName .. "是share类，不允许设置为导出类")
		end
	end
end

local function _SetCppExportClass(CppClass)
	CppClass.__orign_new = CppClass.new
	
	CppClass.__orign_index = CppClass.__index
	CppClass.__orign_call = CppClass.__call
	
	CppClass.__index = function(tbl, key)
		_DynamicExportClass(CppClass)
		
		local CtorFunc = CppClass.Ctor
		if CtorFunc then
			CtorFunc(tbl)
		end
				
		local IndexFunc = rawget(CppClass, "__index")
		return IndexFunc(tbl, key)	
	end
	
	CppClass.__call = function(tbl, ...)
		_DynamicExportClass(CppClass)
		
		local CtorFunc = CppClass.Ctor
		if CtorFunc then
			CtorFunc(tbl)
		end
		
		local CallFunc = rawget(CppClass, "__call")
		return CallFunc(tbl, ...)	
	end

	
	--因为设置成了export class，所以我们的new函数也要重写了
	function CppClass:new(...)
		local n = select("#", ...)
		--如果new的时候有参数，那么我们就要直接载入文件用来调用Ctor
		if n > 0 then
			_DynamicExportClass(CppClass)
			local NewInstance = CppClass:new(...)
			return NewInstance
		else
			local NewInstance = CppClass.ctor( CppClass )

			--[[
			local function ExportFunc(tbl, CppClass)
				_DynamicExportClass(CppClass)
					
				ChangeMetaTable(tbl, CppClass)
					
				local CtorFunc = CppClass.Ctor
				if CtorFunc then
					CtorFunc(tbl)
				end
			end

			local DummyClass = {
				__index = function(tbl, key)
					ExportFunc(tbl, CppClass)
					
					local IndexFunc = rawget(CppClass, "__index")
					return IndexFunc(tbl, key)
				end,
				
				__call = function(tbl, ...)
					ExportFunc(tbl, CppClass)
					
					local CallFunc = rawget(CppClass, "__call")
					return CallFunc(tbl, ...)
				end,
				
				__newindex = function(tbl, key, value)
					CppClass.__newindex(tbl, key, value)
				end,
			}		
			
			ChangeMetaTable(NewInstance, DummyClass)
			--]]
			
			return NewInstance
		end
		
	end
		
	--下面将export class的一些meta方法改写，这样当第一次调用的时候会进行载入
	
	local ClassMt = getmetatable(CppClass)
	assert(ClassMt)
	
	CppClass.__orign_mt = ClassMt
	
	local NewMt = {
		__index = function(Class, key)
			_DynamicExportClass(Class)
			
			--这时候class的mt已经改变了，我们需要使用新的mt的index来运行
			local ClassMt = getmetatable(Class)
			local IndexFunc = rawget(ClassMt, "__index")
						
			return IndexFunc(Class, key)
		end,
		
		__newindex = function(Class, key, value)
			rawset(Class, key, value)
		end,
	}
	
	setmetatable(CppClass, NewMt)
	
end

local function _SetLuaExportClass(LuaClass)
	local ClassMt = 
	{
		__index = function(Class, key)
			local NewClass = _DynamicExportClass(Class)
			
			local NewClassMt = getmetatable(NewClass)
			local IndexFunc = rawget(NewClassMt, "__index")
			
			return IndexFunc(NewClass, key)
		end,
	
		--对于lua的class来说，因为在没导入之前他是一个dummy class，所以我们禁止__newindex
		__newindex = function(Class, key, value)
			error("不允许往只读表里面添加数据")
		end,
	}	
		
	function LuaClass:new(...)
		local n = select("#", ...)
		if n > 0 then
			local Class = _DynamicExportClass(LuaClass)	
			local Inst = Class:new(...)
			return Inst
		else
			local function ExportFunc(tbl, LuaClass)
				local Class = _DynamicExportClass(LuaClass)
					
				local init_obj = rawget(Class, "init_obj")
				
				if init_obj then
					--将相关c++创建对象信息设置到obj上面
					init_obj(Class, tbl)
				else
					setmetatable(tbl, Class)
				end
									
				--如果有Ctor，第一次index的时候必须保证Ctor调用
				local CtorFunc = Class.Ctor
				if CtorFunc then
					CtorFunc(tbl)
				end
				
				return Class, init_obj
			end
			
			local DummyClass = 
			{
				__index = function(tbl, key)
					local Class, init_obj = ExportFunc(tbl, LuaClass)
					
					if init_obj then
						--因为如果有init_obj，那么就表明父类一定有一个cpp class
						--这时候我们会将一些信息装入对象上面，我们首先判断key
						--是不是新装入的字段的
						local value = rawget(tbl, key)
						if value then
							return value
						end
					end
					
					local IndexFunc = rawget(Class, "__index")
					return IndexFunc(tbl, key)
				end,
				
				__call = function(tbl, ...)
					local Class, init_obj = ExportFunc(tbl, LuaClass)
					
					if init_obj then
						local value = rawget(tbl, key)
						if value then
							return value
						end
					end
					
					local CallFunc = rawget(Class, "__call")
					return CallFunc(tbl, key)
				end,
			
				
				__newindex = function(tbl, key, value)
					rawset(tbl, key, value)
				end,
			
			}
			
			local Inst = {}
			setmetatable(Inst, DummyClass)
			return Inst
		end
	end

	setmetatable(LuaClass, ClassMt)
end

local function _CreateExportClass(AliasPath, ClassName, FileName)
	local ClassObj = _G[ClassName]
	
	_CheckCppClass(ClassObj)
	
	--如果不是c++注册类，那么我们可以创建一个lua类了
	if not ClassObj then
		ClassObj = {}
		ClassObj["__ClassName"] = ClassName
		
		--设置为export class info
		ClassObj["__ExportInfo"] = { AliasPath, FileName }
		
		_G[ClassName] = ClassObj
		
		_SetLuaExportClass(ClassObj)	
	else
		--设置为export class info
		ClassObj["__ExportInfo"] = { AliasPath, FileName }
	
		_SetCppExportClass(ClassObj)
	end


end

local ls_ExportClassEnabled = false

function EnableExportClass(Enabled)
	ls_ExportClassEnabled = Enabled
end

function ExportClassEnabled()
	return ls_ExportClassEnabled
end

local RequireFuncTbl = {}
local ExportInfoTbl = {}

function LoadAllExportClass()
	if ls_ExportClassEnabled then
		ExportInfoTbl = {}
		return
	end

	for k, v in pairs(ExportInfoTbl) do
		_DynamicExportClass(v)
	end
end

AddCheckLeakFilterObj(LoadAllExportClass)

function SetExportClass(AliasPath, ClassTbl)
	assert(type(AliasPath) == "string")

	local FuncName = AliasPath .. "_require"
	local RequireFunc = _G[FuncName]
	assert(RequireFunc)
	
	if not RequireFuncTbl[FuncName] then
		_G[FuncName] = function(FileName)
			if ExportFileName[FileName] then
				error(FileName .. "设置成了导出类，不允许直接require其定义文件")
			end
			
			return RequireFile(AliasPath, FileName)
		end
		
		RequireFuncTbl[FuncName] = true		
	end
	

	local n = #ClassTbl
	for i = 1, n do
		local ClassInfo = ClassTbl[i]
		local ClassName = ClassInfo[1]
		local FileName = ClassInfo[2]
				
		_RegExportClass(ClassName, FileName)
		
		_CreateExportClass(AliasPath, ClassName, FileName)		
		
		table.insert(ExportInfoTbl, _G[ClassName])
	end		


end

AddCheckLeakFilterObj(SetExportClass)
