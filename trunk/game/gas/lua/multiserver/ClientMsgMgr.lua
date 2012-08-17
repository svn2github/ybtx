


function FlushDb2Gac(data)
	for typeName,  charTb in pairs(data) do
		for charId, tbl in pairs(charTb) do
			
			local msgId = tbl.MsgId
			local n = #tbl.MsgList
			for i, msg in ipairs(tbl.MsgList) do
				if IsString(msg[1]) then
				
					local funcName = "_Db2" .. msg[1]
					local arg = msg[2]
					if Gas2GacById[funcName] then
						Gas2GacById[funcName](Gas2GacById, charId, typeName, msgId, i==n, unpack(arg, 1, arg.n))
					else
						LogErr(msg[1] .. "  未注册为 能使用 Db2GacById 调用的rpc函数")
					end
					
				else -- DbMsg2ConnById
					local id = msg[1]
					assert(IsNumber(id))
					
					if g_MsgIDToFunTbl[id] then
						local funcName = "_Db2" .. g_MsgIDToFunTbl[id]
						local arg = msg[2]
	
						Gas2GacById[funcName](Gas2GacById, charId, typeName, msgId, i==n, id, unpack(arg, 1, arg.n))
					else
						LogErr("MsgCommon 表中未定义的消息", id)
					end
				end
			end
			
--			Gas2GacById:Db2GacEnd(charId, typeName, curId)   --同类型的消息到客户端可能和不同服务器的交错到达,需要一个end标识符
			
		end
	end
end