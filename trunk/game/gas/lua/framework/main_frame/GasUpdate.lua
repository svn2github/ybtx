local g_ChildVMNum = g_ChildVMNum
local CallSpecialVMDbTrans = CallSpecialVMDbTrans

function Db2Gas:NotifyGasUpdate(data)
	--热更新记录数据库后，通知gas去数据库中取数据更新
	HotUpdateDb()
	HotUpdateGasGac()	
end

function HotUpdateDb()
	for i = 0, g_ChildVMNum do
		CallSpecialVMDbTrans(i, "UpdateDB", "GetHotUpdateDb", nil, nil, {})
	end 
end

function HotUpdateGasGac()
	local function callback(tbl_gas, tbl_gac, gasId, gacId)

		gas_id = gasId
		gac_id = gacId
		for k,v in pairs(tbl_gac) do
			tbl_gac_update[k] = tbl_gac[k]
		end
		local tbl_gas_required_file = package["loaded"]

		for k,v in pairs(tbl_gas) do
			if tbl_gas_required_file[k] ~= nil then
				for _,code in pairs(v) do
					LoadCode(k, code)
				end
				tbl_gas[k] = nil
			end
		end
		
		for k,v in pairs(tbl_gas) do
			if tbl_update_code[k] ~= nil then
				for _,code in pairs(v) do
					table.insert(tbl_update_code[k], code)
				end
			else
				tbl_update_code[k] = tbl_gas[k]
			end
		end
		Gas2Gac:NotifyGacReply(g_AllPlayerSender)
	end
	local data = {["gas_id"] = gas_id, ["gac_id"] = gac_id}
	CallDbTrans("UpdateDB", "GetHotUpdateGasGac", callback, data)
end

function Gac2Gas:SendIdToGas(conn, gacId)
	local tbl_id = {}
	for k in pairs(tbl_gac_update) do
		table.insert(tbl_id, k)
	end
	table.sort(tbl_id)
	for _,id in pairs(tbl_id) do
		if id > gacId then
			local filename = tbl_gac_update[id][1]
			local code     = tbl_gac_update[id][2]
			Gas2Gac:SendMsgToGac(conn, gac_id, filename, code)
		end
	end
end

function Gac2Gas:SendHotUpdateMsg(conn, type, filename, code, result)
	local data = {}
	data["type"] = type
	data["filename"] = filename
	data["code"] = code
	data["result"] = result
	CallDbTrans("GMToolsDbActionDB", "AddHotUpdateResult", nil, data)
end