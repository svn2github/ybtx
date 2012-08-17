engine_require "common/Misc/TypeCheck"
gas_require "framework/main_frame/GlobalChannel"
gas_require "entrance/logout/GasLogoutInc"
gas_require "framework/main_frame/GetTableFromExecuteSql"

--[[
主要函数说明：
1.用户退出游戏时写入相关信息到数据库: SaveLogout(nUserID)
2.向GBS发送退出记录: BeginSendQuit2GBS()
3.以固定频率保存当前时间，并更新用户最新累计在线时间：SaveAndUpTime2DB(nUserID)
4.停止更新用户最新累计在线时间：StopUpdateOnlineTime(UpOLTimeTick)
5.服务器重启时检查有无上次没发给GBS的退出记录（可能由于当机），
  若有则拷贝数据到待发送表发送: CopyUnSentRecord()
--]]

local LogoutServerDB = "LogoutServerDB"


function SaveLogout(nUserID)
	if not nUserID then
		LogErr("玩家在退出时,存储数据库信息出错","玩家的ID为空")
	end
	--local logout_db = (g_DBTransDef["LogoutServerDB"])
	local data = { ["nUserID"] = nUserID }
	
	CallDbTrans(LogoutServerDB, "SaveLogoutInfo", nil, data, nUserID)
	--print("------------------>>> In SaveLogout()")
end


function DbCallBack:RegisterLogoutTick(userName,game_id)
	local function OnTick()
		local data = {}
		data["userName"] = userName
		data["game_id"] = game_id
		CallDbTrans("LoginQueueDB", "DelLogoutUserInfo", nil, data, userName)
	end
	RegisterOnceTick(g_App, "UserLogoutTick", OnTick, GasConfig.LogoutTime)
end

function DbCallBack:EProtocolLogOut(data, username)
	local function remote_callback(flag, msg)
		if not flag then
			LogErr("ERating Logout Fail", "user: " .. username .. ", msg: " .. msg)
		end
	end
	if g_EProtocol and g_EProtocol.m_bConnected == true then
		g_EProtocol:UserLogout(data, remote_callback)
	end
end

--------------------------------------------------------------------------------------
--向GBS发送退出记录
--------------------------------------------------------------------------------------

--[[
返回值：0-不需再试发送记录
        1-需再试发送记录
        2-需过一段时间再试发送记录
--]]


--发送退出记录给GBS
--数据库coroutine func
function SendQuitRecord()
	local function Callback(result)
	
		local num = result:GetRowNum()
		for i = 1, num do
			local data = {result(i,1),result(i,2),result(i,3),result(i,4),result(i,5),result(i,6),result(i,7)}
			local function remote_callback(flag, msg)
				if not flag then
					return
				end
			end
			
			if next(data) and g_EProtocol.m_bConnected == true then
				g_EProtocol:UserLogout(data, remote_callback)
			end
		end
	end
	CallDbTrans(LogoutServerDB, "SendQuitRecord", Callback, {["CurServerId"] = g_CurServerId}, GlobalChannel.server_var)
end

function CSaveAndUpTime:new(nUserID)
	local sut	= CSaveAndUpTime:__new()
	sut.m_nUserID = nUserID
	sut.m_bStopUpOLTime = false
	return sut
end

--间隔时间一到，就创建数据库coroutine保存当前时间，并且更新用户最新累计在线时间
function CSaveAndUpTime:OnTick()
	--print("------->>>CSaveAndUpTime:OnTick()")
	local data = {['user']=self.m_nUserID, ['time'] = self.m_bStopUpOLTime }
	--local logout_db = (g_DBTransDef["LogoutServerDB"])
	CallDbTrans(LogoutServerDB, "SaveAndUpTime", nil, data, self.m_nUserID)
end

function SaveAndUpTime2DB(nUserID)
	--print("------------>>>In SaveAndUpTime2DB()")
	local sut = CSaveAndUpTime:new(nUserID)
	g_App:RegisterTick(sut,5*60*1000)
	sut:SetTickName("CSaveAndUpTime")
	return sut
end

function StopUpdateOnlineTime(UpOLTimeTick)
	--print("------------>>>In StopUpdateOnlineTime()")
	UnRegisterTick(UpOLTimeTick)
end

--------------------------------------------------------------------------------------
--服务器重启时检查有无上次没发给GBS的退出记录（可能由于当机）
--------------------------------------------------------------------------------------
-- 服务器重启时将表tbl_user_online中上次没正常发送的退出记录复制到表tbl_auth_logout中以发送到GBS
function CopyUnSentRecord()
	--print("------------>>>In CopyUnSentRecord()")
	--取得时间

	--local logout_db = (g_DBTransDef['LogoutServerDB'])
	CallDbTrans(LogoutServerDB, "CopyUnSentRecord", nil, {["CurServerId"] = g_CurServerId}, GlobalChannel.server_var)
end
