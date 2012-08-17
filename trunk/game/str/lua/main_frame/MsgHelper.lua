--[[
	因为我们的压力测试的流程如下
	
	1，测试端给服务器发送相应命令
	2，等待
	3，接收到服务器返回相应命令
	4，处理，如果是正确的命令，那么继续第一步，否则抛出错误

	所以我们得需要一套等待机制来处理这样的情况
--]]

--参数为预计会等待到的正确消息，如果为空，那么任何消息都可以触发
function WaitForMsg(...)

	local arg = {...}

	assert(coroutine.running())
	
	local msg = coroutine.yield()

	assert(type(msg) == "string")
	
	if nil == arg[1] then
		return msg
	end

	for k, v in ipairs(arg) do
		if msg == v then
			return msg
		end
	end

	--我们收到了一个不能正确处理的消息,抛出异常
	local want_msg = ""
	for k, v in ipairs(arg) do
		want_msg = want_msg .. v .. " "
	end
	
	--error(msg .. " is the invalid msg, the msg we want is " .. want_msg)
	print("收到消息"..msg .. ", 我们关心的消息是:" .. want_msg)
	WaitForMsg(...)
end

--发送消息
function PostMsg(co, msg)
	assert(type(msg) == "string")
	coroutine.resume(co, msg)
end

function PostConnMsg(conn, msg)
	PostMsg(conn.m_Co, msg)
end
