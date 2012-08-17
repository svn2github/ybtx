engine_require "server/thread/DbChannelMgr"
engine_require "server/thread/DataBaseInit"

local clock = os.clock
local apcall = apcall
function CoreCallDbTrans(process_data)
	local data = process_data["data"]
	local filename = process_data["filename"]
	local funcname = process_data["funcname"]
	local func = 	process_data["func"]
	local callback = process_data["callback"]
	local errfunc = process_data["errfunc"]
	local channel = process_data["channel"]
	local entry_func = process_data["entryfunc"]
	local clean_func = process_data["cleanfunc"]
	local tickIndex = process_data["tickIndex"]
	
	local function CallTransaction()
	
		local function InnerCallDb()
			if callback then
				local function HandleCallBack(...)
					--增加下面几句只是为了在测试的时候检查db执行的返回值
					--以及查到是哪个db操作造成的
					local arg = {...}
					local argv = select("#", ...)
					
					local function CheckCallBackParam(tbl)
						for k, v in pairs(tbl) do
							if type(v) == "function" or type(v) == "thread" then
								error(filename .. " " .. funcname .. "返回给callback的参数不是基本的数据类型")		
							end
							if type(v) == "table" then
								CheckCallBackParam(v)
    						end
    						
    						if type(v) == "userdata" then
    							if getmetatable(v) == CDbResultSet then
									tbl[k] = v:GetValueTable()
								else
								    if getmetatable(v) ~= CValueTable then							     
    									error(filename .. " " .. funcname .. "返回给callback的参数不是基本的CDbResultSet或者CValueTable")
    								end
								end
    						end
						end
					end
					
					CheckCallBackParam(arg)
			
					OnHandleDbCallback(nil, true, callback, errfunc, unpack(arg, 1, n))							
				end
				HandleCallBack(entry_func(func, data, funcname, channel))
			else	
				assert(false, "不再有空的 callback")
			end
		end
		
		local RetryNum = 0
		
		local ret, msg
		
		local error_handler = function(err_msg)
		
			if not string.find(err_msg, "Deadlock") then
				
				local msg = string.format("FuncName: %s", funcname) 
				local LogCode = 0
		
				if next(channel) then
					local channel_msg = "  Channel ID:"
				
					for k, v in pairs(channel) do
						channel_msg = channel_msg .. " " .. v
					end
							
					LogCode = LogErr(err_msg, msg .. channel_msg)
				else
					LogCode = LogErr(err_msg, msg)
				end

				--如果LogCode为0，表明未使用容错，这时候需要db
				if LogCode == 0 then
					db()
				end
			
			end			
			
			return err_msg
		end
		
		
		while true do
			local BeginTime = clock()
			ret, msg = xpcall( InnerCallDb, error_handler )
			
			apcall(clean_func,ret)	--每次调用完后 ,都调用清理函数
			if ret == false then
				CancelTran()

				--如果碰到死锁或者timeout，我们重试3次，如果还是失败，那么我们在退出
				if string.find(msg, "Deadlock") then
					RetryNum = RetryNum + 1
				
					if RetryNum > 3 then
						local RetryError = msg .. " 重试3次失败" 
						LogErr(RetryError)
						break
					end					
				else 
					--不需要再次处理，直接退出，
					break
				end
			else 
				--如果成功，我们正常返回
				g_DbChannelMgr:EndTran()
			
				local EndTime = clock()
				local OffTime = EndTime - BeginTime
				if OffTime > 10 then
					local type = string.format("Db Trans 执行时间过长")

					local msg = string.format("FuncName : %s , 总耗时 %d 秒 ", funcname, OffTime)
					if next(channel) then
						local channel_msg = " Channel ID:"
						
						for k, v in pairs(channel) do
							channel_msg = channel_msg .. " " .. v
						end
						
						msg = msg .. channel_msg
					end
	
					LogErr(type, msg)
				end

				OnHandleVMCallback(nil, funcname, tickIndex, unpack(channel))
				
				return
			end	
		
		end
		
		if ret == false then
			--如果出错，我们就让主线程调用errfunc
			if errfunc then
				OnHandleDbCallback(nil, false, callback, errfunc)
			end
			
			OnHandleVMCallback(nil, funcname, tickIndex, unpack(channel))		
		end
	end

	g_DbChannelMgr:CreateQueryCoroutine(CallTransaction, funcname, unpack(channel))
end

