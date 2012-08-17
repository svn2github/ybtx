--[[
	压力测试单元测试框架	
--]]

-- Import
local type = type
local print = print
local ipairs = ipairs
local pairs = pairs
local string = string
local table = table
local pcall = pcall
local xpcall = xpcall
local traceback = debug.traceback
local error = error
local setmetatable = setmetatable
local rawset = rawset
local assert = assert
local getfenv = getfenv
local setfenv = setfenv
local tostring = tostring

local function is_nil(x)
	return type(x) == "nil"
end

local function is_boolean(x)
	return type(x) == "boolean"
end

local function is_number(x)
	return type(x) == "number"
end

local function is_string(x)
	return type(x) == "string"
end

local function is_table(x)
	return type(x) == "table"
end

local function is_function(x)
	return type(x) == "function"
end

local function is_thread(x)
	return type(x) == "thread"
end

local function is_userdata(x)
  	return type(x) == "userdata"
end

--[[
	Test Case
	对于Test Case来说，如果我们不提供自己的SetUp和TearDown函数，那么就是用系统默认的
--]]

tc_mt = {
	__newindex = function(tc, key, value)
		local _value = rawget(tc, key)
    	assert(_value == nil)
    	
    	rawset(tc, key, value)
	    if is_string(key) and is_function(value) then
			local name = string.lower(key)
	      	if string.find(name, "^test") or string.find(name, "test$") then
	        	table.insert(tc._test_units, key)
	      	elseif name == "setup" then
	        	tc._setup_func = value
	      	elseif name == "teardown" then
	        	tc._teardown_func = value
	      	end
	    end
	end
}


function TestCase(name)
	local tc = {
    	_name = name;
    	_setup_func = nil;
	    _test_units = { };
    	_teardown_func = nil;
  	}
  	setmetatable(tc, tc_mt)
  	return tc
end

DefaultSetUp = function () end
DefaultTearDown = function () end

local CallNum = 0
local SucNum = 0

function OutputTestResult()
	--print(CallNum .. " Tests Called " .. SucNum .. " Successed!")
	io.write("任意键结束")
	io.read()
end

function Call(func, ...)
	local arg = {...}
	CallNum = CallNum + 1
	local ok, funRetOne, funRetTwo = xpcall(function() return func(unpack(arg)) end, db)
	--print(ok, funRetOne, funRetTwo)
	if ok then
		SucNum = SucNum + 1
	end
	return ok
end

function SetUpTestCase(tc, ...)
	local arg = {...}
	--print(tc._name .. ":SetUp")
	if tc._setup_func then
		return Call(tc._setup_func, tc, ...)
	else
		return Call(DefaultSetUp, ...)
	end
	
	return true
end

function TearDownTestCase(tc, ...)
	local arg = {...}
	--print(tc._name .. ":TearDown")
	if tc._teardown_func then
		return Call(tc._teardown_func, tc, ...)
	else
		return Call(DefaultTearDown, ...)		
	end
end

TestRunEnabled = true

function RunTestCase(tc, ...)	
	local arg = {...}
	if not next(tc._test_units) then
		return false
	end

	for k, v in ipairs(tc._test_units) do
		if not TestRunEnabled then
			return false
		end
		
		--print(tc._name .. ":" .. v)
		
		local ret = Call(tc[v], tc, ...)	
		if not ret then
			return false
		end
	end

	return true

end







