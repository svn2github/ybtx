gas_require "e_protocol/EServiceInc"
gas_require "e_protocol/LuaInt2Byte"

lan_load "gm/Lan_UserAdvice_Common"

cfg_load "gm/UserAdvice_Common"

function CreateLuaTableFromJSonString(strJSon)
	if not strJSon then
		return
	end
	local temp = string.gsub(strJSon, "%[", "{")
	temp = string.gsub(temp, "(:%S*)null","%1nil")
	temp = string.gsub(temp, "%]","}")
	temp = string.gsub(temp, "(\"[%a%d]+\"):","[%1]=")
	
	local fun = loadstring("return " .. temp)
	assert(fun, strJSon)
	return fun()
end

local function GetMD5String(info)
	local MD5Digester = CMd5Digester:new()
	MD5Digester:Begin()
	MD5Digester:Append(info, string.len(info))
	local MD5_Buf = CreateCBufAcr( CreateCBuf(33) )
	MD5Digester:GetMD5Base16Str(MD5_Buf)
	return MD5_Buf:ToString()
end

function EService.SendData(url, params, callback)

	local http = HttpClient:new()
	local host = EServiceConfig.Host
	local port = EServiceConfig.Port
	
	if http.Used then
		return false
	end
	http.Used = true
	http.url = url
	http.params = params
	if not GasConfig.EServiceCheck then
		if callback then
			callback(nil)
			return true
		end
		return true
	end
	
	local function call()
		
		http:_Connect(host, port)
		local flag, msg = coroutine.yield()
		
		if callback then
			callback(http.m_ret_msg)
		end
	end
	http.m_co = coroutine.create(call)
	coroutine.resume(http.m_co)
end

function EService.InsertQuestionFromClient(conn, questionTypeId, typeOptionId, questionText)
	local typeOptionTbl = GetCfgTransformValue(true, "Lan_UserAdvice_Common", questionTypeId, "typeOption")
	if not (typeOptionTbl and typeOptionTbl[typeOptionId]) then
		return false
	end
	local player = conn.m_Player
	local playerAccount = conn.m_Account:GetID()
	local arg = {}
	arg["questionTypeId"] = questionTypeId
	arg["questionType"] = Lan_UserAdvice_Common(questionTypeId, "questionType")
	arg["typeOptionId"] = typeOptionId
	arg["typeOption"] = typeOptionTbl[typeOptionId]
	arg["playerId"] = player.m_uID
	arg["playerAccount"] = conn.m_UserName
	arg["gameRole"] = player.m_Properties:GetCharName()
	arg["gameId"] = tonumber(ERatingConfig["AGIP_GameID"])
	arg["gameServer"] = tonumber(ERatingConfig["AGIP_Gateway_id"])
	arg["questionText"] = questionText
	arg["MD5String"] = GetMD5String(arg["playerAccount"])
	arg["Cooke"] = player.m_uID
	arg["dynamicParams"] = nil
	
	local function callback(retMsg)
		if retMsg then
			local retTbl = CreateLuaTableFromJSonString(retMsg)
			if retTbl and retTbl["result"] == 1 then
				Gas2Gac:RetSendUserAdviceInfo(conn, 7042)
			else
				LogErr("EService ListQueryGameQuestions  ·µ»Ø´íÎó", "·µ»ØÄÚÈÝ: " .. retMsg)
				Gas2Gac:RetSendUserAdviceInfo(conn, 7044)
			end
		end
	end
	return EService.SendData(EServiceConfig.InsertQuestion, arg, callback)
end

function EService.ListQueryGameQuestions(conn)
	local player = conn.m_Player
	local arg = {}
	arg["gameId"] = ERatingConfig["AGIP_GameID"]
	arg["playerId"] = player.m_uID
	arg["gameRole"] = player.m_Properties:GetCharName()
	arg["gameServer"] = ERatingConfig["AGIP_Gateway_id"]
	arg["MD5String"] = GetMD5String(arg["playerId"])
	arg["Cooke"] = playerId
	
	local function callback(retMsg)
		if retMsg then
			local xml = CXmlConfig:new()
			if not xml:LoadConfigFormString("linekong",retMsg) then
				LogErr("EService ListQueryGameQuestions  ·µ»Ø´íÎó", "·µ»ØÄÚÈÝ: " .. retMsg)
				Gas2Gac:RetSendUserAdviceInfo(conn, 7051)
				return
			end
			local result = tonumber( xml:GetStr_1("result"))
			if result == 1 then
				Gas2Gac:RetQuestionsList(conn, retMsg)
			else
				LogErr("EService ListQueryGameQuestions  ·µ»Ø´íÎó", "·µ»ØÄÚÈÝ: " .. retMsg)
				Gas2Gac:RetSendUserAdviceInfo(conn, 7051)
			end
		end
	end
	return EService.SendData(EServiceConfig.QueryQuestionList, arg, callback)
end



