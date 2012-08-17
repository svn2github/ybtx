gas_require "multiserver/ReviseLoginData"

local StartDate = {year = 2010, month = 1, day =1}
local DelayDataId = (os.time() - os.time(StartDate)) * 10000

local DelaySaveData = {}  --延迟保存到数据库的数据

local LastServerData = {} --切服前的数据, 用来修正创建角色信息的数据


function GetPlayerLastServerData(charId)
	local v = LastServerData[charId]
	if v then
		return v.Data, v.ServerId, v.Data["DelayDataId"]
	end
end

function ClearPlayerLastServerData(charId)
	LastServerData[charId] = nil
end

function ReceiveData.LastServerData(serverId, data)
	local charId = data["char_id"]
--	assert(LastServerData[charId] == nil ) -- 一个多余的断言
	
	
	LastServerData[charId] = {}
	LastServerData[charId].ServerId = serverId
	LastServerData[charId].Data = data
	
end

function SaveDelayDataToDB(data, username, AccountID, charId)
	local function callback()
--		print"保存 延迟存盘数据成功"	
		if charId then
 			DelaySaveData[charId] = nil
 		end
	end
	
	local function err_callback()
		LogErr("调用 LogoutDelaySaveData出错",  "要切至的服务器为: " .. tostring(data["ChangeToServer"]) )
	end
	if g_SceneMgr:IsMainScene(data["scene_id"]) then
		PCallDbTrans("CharacterMediatorDB", "LogoutDelaySaveData", callback, err_callback, data, username, AccountID)
	else
		PCallDbTrans("CharacterMediatorDB", "LogoutDelaySaveData", callback, err_callback, data, username, AccountID, data["scene_id"])
	end
end


function GetDelayDataId()
	DelayDataId = DelayDataId + 1
	return DelayDataId
end

function SavePlayerLogoutData(data, username, AccountID, dataId)	
	local charId = data["char_id"]
	if DelaySaveData[charId] then
		UnRegisterTick(DelaySaveData[charId].Tick)
		DelaySaveData[charId] = nil
	end
	
	local function OnSaveTick(tick)
		UnRegisterTick(tick)
		assert(DelaySaveData[charId])
		SaveDelayDataToDB(DelaySaveData[charId].Data, username, AccountID, charId)
	end


	DelaySaveData[charId] = {}
	DelaySaveData[charId].Data = GetDelaySaveData(data)
	DelaySaveData[charId].Username = username
	DelaySaveData[charId].AccountID = AccountID
	DelaySaveData[charId].Tick = RegisterTick("DelaySaveDataTick", OnSaveTick, (SecretKeyLifeTime + 20) * 1000)
--	print"注册延迟存盘tick"
--	print "发送数据去目标服务器"
	assert(dataId)
	DelaySaveData[charId].Data["DelayDataId"] = dataId
	SendDataToServer(GetServerConn(data["ChangeToServer"]), "LastServerData", DelaySaveData[charId].Data)
end

--关服存盘
function SaveAllDelayLogoutData()
	for charId, v in pairs(DelaySaveData) do
		UnRegisterTick(v.Tick)
		SaveDelayDataToDB(v.Data, v.Username, v.AccountID, charId)
	end
end


function Db2Gas:ForceSaveDelayData(data)
--	print"强制存盘"
	local charId = data[1]
	local v = DelaySaveData[charId]
	if v then
--		print ("强制存盘		成功 " .. v.Data["DelayDataId"])
		UnRegisterTick(v.Tick)
		SaveDelayDataToDB(v.Data, v.Username, v.AccountID, charId)
	else
--		print"强制存盘  	无数据"
	end
end

function Gas2GasDef:CancelDelaySaveTick(Conn, charId, delayDataId)
--	print "取消延迟存盘"
	local v = DelaySaveData[charId]
	if v and v.Data["DelayDataId"] == delayDataId then
--		print ("取消延迟存盘   成功 " .. delayDataId)
		UnRegisterTick(v.Tick)
		DelaySaveData[charId] = nil
	else
--		print "取消延迟存盘   无数据"
	end
end