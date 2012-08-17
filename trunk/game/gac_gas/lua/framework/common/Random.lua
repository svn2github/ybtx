engine_require "common/Misc/TypeCheck"
local function  Random_Lua(min, max)
	if max == nil then
		return GetHDProcessTime() % min + 1
	else
		return GetHDProcessTime() % ( max - min + 1 ) + min
	end
end
--新的随机函数
--math.random = Random_Lua
