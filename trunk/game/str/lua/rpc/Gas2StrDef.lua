str_require "rpc/Gas2StrInc"
gac_gas_require "framework/rpc/RemoteInfo"
str_require "rpc/LoginRpc"
str_require "rpc/SceneRpc"
str_require "rpc/ShopRpc"

function Gas2Gac:OnError(num, fun)
--[[
		-1 ：表示id错误
		-2 ：表示验证码错误
		-3 ：表示数据格式错误
		-4 ：表示没有找到处理函数
		-5 ：表示返回的错误
		>0 ：表示用户自己返回的参数
--]]
	if num == -4 then
		--print(fun)
		return true
	end
	error(fun .. "rpc call error " .. tostring(num) )
	return false
end

--压测的时候,才会把这些消息通知客户端
function StressRpcToServer(func, ...)
	Gac2Gas[func](Gac2Gas,...)
end
