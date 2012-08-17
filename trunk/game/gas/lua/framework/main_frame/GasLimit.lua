local function shut_down_tick_func()
	os.exit(0)
end

local function delay_shut_down_tick_func()
	local rate = 95	
	if math.random(1, 100) > rate then
		RegisterTick("shut_down_tick_func", shut_down_tick_func, math.random(1,9) * 60 * 1000)
	end
end

function limit_server_tick_func()
	if not IsWindows() then
		return
	end
	
	if IsWindows() then
		g_delay_shut_down_tick = RegisterTick("g_delay_shut_down_tick", delay_shut_down_tick_func, 10 * 60 * 1000)
	end 
	
	if g_limit_server_tick then
		UnRegisterTick(g_limit_server_tick)
	end
end
