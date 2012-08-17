engine_require "common/Misc/TypeCheck"
gas_require "entrance/logout/GasLogout"
gas_require "entrance/login/LoginQueue"
gas_require "framework/main_frame/GetTableFromExecuteSql"
gac_gas_require "framework/common/StringSplit"

local LoginServerDB = "LoginServerDB"


g_UserForceLoginData = {}
g_UserActivationCode = {}
local PublicKeyXCoordinateTbl = {"A", "B", "C", "D", "E", "F", "G", "H"}
local PublicKeyYCoordinateTbl = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}
--[[
	登陆流程
	  客户端发过CheckUser指令
	  	服务器判断
--]]

function g_cmdFreezeAccount(arg1,arg2,arg3)
end
function g_cmdCancelFreezeAccount(arg1,arg2,arg3)
end

--判断Connection是否有效
local function CheckConnection(nContextID)
	if (not nContextID) or (g_ContextMgr:DelContext(nContextID) == nil) then
	--GAC和GAS已经断开
		return false
	else
		return true
	end
end

--创建帐号
local function RetAuthSucc(Connection, nContextID, uAgreedVer, Account, campInfoTbl)
	--返回验证通过
	if(CheckConnection(nContextID) == true) then
		Connection.m_nID = nil --把上下文ID删除	
		--将来要改成判断当前版本号，而不是用1
		local function Apex_UserLogin()
			if not Account then
				LogErr("RetAuthSucc时Account为nil, 登录Apex失败")
			else
				CApexProxy_GetInst():NoticeApexProxy_UserLogin(Account:GetID(), Account:GetName(), Connection)
			end
		end
		apcall(Apex_UserLogin)
		
		Connection.m_RecruitInfo = campInfoTbl
		Gas2Gac:ReturnRecruitInfo(Connection, campInfoTbl[1], campInfoTbl[2], campInfoTbl[3])
		
		if uAgreedVer == 1 then
			Gas2Gac:ReturnCheckUser( Connection,true,1,Connection.m_UserName)
		else
			Gas2Gac:ReturnCheckUser( Connection,true,6,Connection.m_UserName)
		end
		
		--CRecruitMgr.LoginCheckRecruit(Connection)
	else
		--if(GasConfig.SkipGBSAuth == 0) then
			--退出流程
			--写入相关退出信息到数据库
			SaveLogout(Account:GetID())
			--停止更新用户的最新累计在线时间
			StopUpdateOnlineTime(Connection.m_UpOnlineTimeTick)	
			g_AccountMgr:DeleteAccount( Connection.m_nGameID,Account:GetName() )
		--end
	end
end

--让被顶的号下线
local function Account_Logout(old_account, szName,game_id)
	local function account_delay_logout()
		--return 
		UnRegisterTick(old_account.shutdowntick)
		old_account:Exit()
	end
	
	if old_account.shutdowntick then
		--正在退出,不要急,慢慢等吧
		return
	end
	
	if old_account.m_Conn and old_account.m_Conn.ShutDown then
		if old_account.m_Conn.m_Player then
			MsgToConn(old_account.m_Conn, 3)
		end
		old_account.shutdowntick = RegisterTick("adlo", account_delay_logout, 3*1000)
		CCoSync.WaitFor(nil, true, old_account.m_Event)
		old_account.m_Event:Set(false)
	else
		old_account:Clear()
		g_AccountMgr:DeleteAccount(game_id,szName )
	end
end

--创建一个新的Account
local function Account_Create(Connection, szName, account, nContextID, uAgreedVer, CallBackFun, campInfoTbl)
	--DelayLogout(old_account.m_Conn, 0)
	--local res = g_AccountMgr:CreateAccount( szName, account )
	g_AccountMgr:CreateAccount(Connection.m_nGameID,szName, account )
	--print("重新创建用户", szName, res)
	Connection.m_Account = account
	if CallBackFun then
		local function Apex_UserLogin()
			if not account then
				LogErr("Account_Create时Account为nil, 登录Apex失败")
			else
				CApexProxy_GetInst():NoticeApexProxy_UserLogin(account:GetID(), account:GetName(), Connection)
			end
		end
		apcall(Apex_UserLogin)
		Gas2Gac:ReturnStartApex( Connection )
		CallBackFun(Connection)
	else
		RetAuthSucc(Connection, nContextID, uAgreedVer,account, campInfoTbl)
	end
end

local function CreateAccount(Connection, nContextID, uAgreedVer, szName, nUserID, uYuanBao, CallBackFun, campInfoTbl)
	Connection:SetUserName(szName)
	local game_id = Connection.m_nGameID
	local account = CAccountServer:new( szName, nUserID, uYuanBao, Connection )
	assert(account and szName)
	local old_account = g_AccountMgr:GetAccountInfo(game_id,szName )
	if( old_account ~= nil ) then
		local function CallBack()
			Account_Logout(old_account, szName,game_id)
			Account_Create(Connection, szName, account, nContextID, uAgreedVer, CallBackFun, campInfoTbl)
		end
		coroutine.resume(coroutine.create(CallBack))
	else
		--print("此用户第一次登录！！")
		Account_Create(Connection, szName, account, nContextID, uAgreedVer, CallBackFun, campInfoTbl)
	end
	--以固定频率保存当前时间，并更新用户最新累计在线时间
	Connection.m_UpOnlineTimeTick = SaveAndUpTime2DB(nUserID)
end

--认证通过
function AuthSuccess(str, Connection, szName,sPassword,iLoginKey, ip, nContextID, UserID,sMacAddress,activation_code)
	local data = {
					["name"] = szName,
					["userid"] = UserID,
					["password"] = sPassword,
					["ip"] = ip,
					["LoginKey"] = iLoginKey,
				}
				

	--保存数据到数据库	
	local SN = szName .. " " .. sPassword .. " " .. os.date("%Y-%m-%d %H:%M:%S")	
	--MD5
	local MD5Digester = CMd5Digester:new()
	MD5Digester:Begin()
	MD5Digester:Append(SN, string.len(SN))
	local SN_MD5 = CreateCBufAcr( CreateCBuf(33) )
	MD5Digester:GetMD5Base16Str(SN_MD5)
	local strSN = SN_MD5:ToString()
	assert(ip)
	
	data["Sn"] = strSN
	data["CurServerId"] = g_CurServerId
	local game_id = Connection.m_nGameID
	
	local function callback(ret, LoginKey)
	
		if ret then
			local account_info , uAgreedVer, campInfoTbl = ret[1], ret[2], ret[3]
			if not IsCppBound(Connection) then
				SaveLogout(account_info[2])
				return
			end
			if g_UserForceLoginData[game_id] and g_UserForceLoginData[game_id][szName] then
				g_UserForceLoginData[game_id][szName] = nil
			end
			CreateAccount(Connection, nContextID, uAgreedVer,account_info[1],account_info[2],account_info[3] ,nil, campInfoTbl)
		else
			if LoginKey then--有这个登录Key,说明是正常顶号,(如果返回false,并且没有这个key,就说明是其它的错误)
				Connection.m_TempData = {}
				Connection.m_TempData.m_Data = {}
				Connection.m_TempData.m_Data = data
				Connection.m_TempData.m_nContextID = nContextID
				
				--告诉客户有人在用他登录
				--同一个编号登录,就不提示,直接顶号
				local IsShowMsg = true
				if LoginKey == iLoginKey then
					IsShowMsg = false
				end
				
				Gas2Gac:RetReSetLogin(Connection, IsShowMsg)
				if not IsShowMsg then
					Gac2Gas:UserForceTipSigal(Connection,sMacAddress)
				end
			else
				if IsCppBound(Connection) then
					Connection.m_LogoutSetting = "Immediately"
					Connection:OldShutDown("GetAccountInfoFromDB低速操作有问题,登录出错")
				end
			end	
		end
	end
	data["sMacAddress"] = sMacAddress
	data["game_id"] = game_id
	data["activation_code"] = activation_code
	--local LoginServerSql = (g_DBTransDef["LoginServerDB"])
	CallDbTrans(LoginServerDB, "GetAccountInfoFromDB", callback, data, szName,game_id, UserID)
end

local function UnRegisterContextTick(user_LoginInfo)
	local nContextID = user_LoginInfo[7]
	if(g_ContextMgr.m_tblContext[nContextID] ~= nil) then
		g_ContextMgr.m_tblContext[nContextID].m_App:UnRegisterTick(g_ContextMgr.m_tblContext[nContextID])
	end
end

--@brief 检查已经在线的玩家账号数
--@param user_type:0---非vip账号,1--vip账号
--@return true--可以进入游戏,false--进入等待排队
local function CheckLoginGameUserNum(user_LoginInfo,user_type)
	local Connection = user_LoginInfo[2]
	local userName = user_LoginInfo[3]
	local data = { 
					["user_name"] = userName,
					["context_id"] = user_LoginInfo[7],
					
				}
	
	local function callback(isPassQueue)
		if not IsCppBound(Connection) then
			return
		end
		if g_LoginQueue.RepeatQueueUp(userName, user_LoginInfo,Connection.m_nGameID) then
			UnRegisterContextTick(user_LoginInfo)
			return
		end
		CheckUserOver(Connection)
		if isPassQueue then
			AuthSuccess(unpack(user_LoginInfo))
		else
			g_LoginQueue.QueueUp(userName, user_LoginInfo,Connection.m_nGameID)
			UnRegisterContextTick(user_LoginInfo)
		end
	end
	data["game_id"] = Connection.m_nGameID
	CallDbTrans("LoginQueueDB", "CheckUserLoginGame", callback,data)
end

--处理返回信息 
--无数据库操作
local function DealWithRetAuth(bFlag,uMsgID, tbl_ConnInfo, tbl_UserInfo,sMacAddress)
	tbl_ConnInfo = tbl_ConnInfo or {}
	tbl_UserInfo = tbl_UserInfo or {}
	local Connection, szName, sPassword, iLoginKey, ip, nContextID,game_id = unpack(tbl_ConnInfo)

	local user_id, user_type, adult_flag, user_class, user_role_count, user_point, promoter_id, user_flag,activation_code = unpack(tbl_UserInfo)
	if bFlag then --验证通过
		Connection.adult_flag = adult_flag
		local http = HttpClient:new()
		local host = CISConfig.CISHost
		local port = CISConfig.CISPort
		local url  = CISConfig.CISAddUser_url
		local params = { ['userid'] = user_id,
			['username'] = szName,
			['vip_type'] = user_type,
			['serv_id'] = tonumber(ERatingConfig["AGIP_Gateway_id"])*100+g_CurServerId
			}

		http:SendData(host, port, url, params)
		local user_LoginInfo = {str, Connection, szName, sPassword, iLoginKey, ip, nContextID, user_id,sMacAddress,activation_code or ""}
		Gas2Gac:RetLoginConnectStepInfo(Connection, 1154)
		--暂时先屏蔽登录排队,因为现在还没有vip。
		CheckLoginGameUserNum(user_LoginInfo,user_type)
	else
		Connection.m_nID = nil --把上下文ID删除
		if uMsgID == -1421 then
			--密保卡验证
			local coordinateXInfoTblLen = #PublicKeyXCoordinateTbl
			local coordinateYInfoTblLen = #PublicKeyYCoordinateTbl
			local randomNum1    =   math.random(1, coordinateXInfoTblLen)
	    local randomNum2    =   math.random(1, coordinateXInfoTblLen)
	    local randomNum3    =   math.random(1, coordinateXInfoTblLen)
	    
	    local randomNum4    =   math.random(1, coordinateYInfoTblLen)
	    local randomNum5    =   math.random(1, coordinateYInfoTblLen)
	    local randomNum6    =   math.random(1, coordinateYInfoTblLen)
	    
	    local randomPubKey1 =   PublicKeyXCoordinateTbl[randomNum1] .. PublicKeyYCoordinateTbl[randomNum4]
	    local randomPubKey2 =   PublicKeyXCoordinateTbl[randomNum2] .. PublicKeyYCoordinateTbl[randomNum5]
	    local randomPubKey3 =   PublicKeyXCoordinateTbl[randomNum3] .. PublicKeyYCoordinateTbl[randomNum6]
			g_UserActivationCode[game_id] = g_UserActivationCode[game_id] or {}
			g_UserActivationCode[game_id][szName] = randomPubKey1 .. randomPubKey2 .. randomPubKey3
			Gas2Gac:ReturnCheckUserByActivationCode( Connection,randomPubKey1,randomPubKey2,randomPubKey3)	
		else
			Gas2Gac:ReturnCheckUser( Connection,bFlag,uMsgID,0)	
		end	
	end
end

--coroutine数据库func
local function CheckLogin(ConnInfo,IsFreezed,IsAuthSucc,UserId,sMacAddress)
	if IsFreezed~=0 then
		DealWithRetAuth(false,1,ConnInfo)
		return
	end
	
	if GasConfig.SkipGBSAuth==1 or IsAuthSucc then 
		DealWithRetAuth(true,6, ConnInfo, {UserId},sMacAddress)
		return
	end
	
	DealWithRetAuth(false,5, ConnInfo)
end

local function InitConnectionData(Connection, UserName,GameID)
	local addr = CAddress:new()
	Connection:GetRemoteAddress(addr)
	local ip = addr:GetAddress()
	local port = addr:GetPort()
	local ip_info = {}

	SplitString(ip, "%.", ip_info)
	Connection.m_nIP = tonumber(string.format("0x%02x%02x%02x%02x", tonumber(ip_info[1]), tonumber(ip_info[2]), tonumber(ip_info[3]), tonumber(ip_info[4])))
	Connection.m_nPort = tonumber(port)

	local context = CContext:new(Connection, g_App)
	local id = g_ContextMgr:AddContext(context, 300000)
	assert(Connection.m_strIP == nil)
	Connection.m_strIP = ip
	Connection.m_nID = id
	Connection.m_UserName = UserName
	Connection.m_nGameID = GameID
end

local function CheckUser(Connection,UserName,Password, iLoginKey, MatrixPasswd, MatrixCoord,sMacAddress,game_id,DistStr,IDCode, IsTrustee)
	--添加上下文，以让GAS判断连接是否有效
	InitConnectionData(Connection, UserName,game_id)
	local nContextID = Connection.m_nID
	local ConnInfo = {Connection, UserName,Password,iLoginKey,Connection.m_strIP,nContextID,game_id}

	-------------------------------------------------------------------
	local function LocalAuthLogin()
		local ip = Connection.m_strIP
		local data = {
					["UserName"] = UserName,
					["Password"] = Password,
					["MatrixPasswd"] = MatrixPasswd,
					["MatrixCoord"] = MatrixCoord,
					["game_id"] = game_id
				}
		local function callback(result)
			CheckLogin(ConnInfo,result[1],result[2],result[3],sMacAddress)
		end
		--local LoginServerSql = (g_DBTransDef["LoginServerDB"])
		CallDbTrans(LoginServerDB, "SelectPlayerInfoFromLocalDB", callback, data, UserName,game_id)
	end
	
	local function LocalSavePasswd(user_info)
		local function callback(result)
			if result[1] ~= 0 then
				DealWithRetAuth(false,1, ConnInfo)
				return
			end
			
			DealWithRetAuth(true,6, ConnInfo, user_info,sMacAddress)
		end
		local data = {
				["UserName"] = UserName,
				["PassWord"] = Password,
				["game_id"] = game_id,
			}
		
		--local LoginServerSql = (g_DBTransDef["LoginServerDB"])
		CallDbTrans(LoginServerDB, "UpdatePasswd", callback, data, UserName,game_id)
	end
	
	local function RemoteAuthResult(result,uMsgID, user_info)
		if (result == UserLoginRetCode.LocalLogin) then
			LocalAuthLogin()
		elseif result == UserLoginRetCode.LoginFail then
			DealWithRetAuth(false,uMsgID, ConnInfo)
		elseif result == UserLoginRetCode.LoginSucc then
			LocalSavePasswd(user_info)
		end
	end
	
	if (UserName == 0) or (Password == nil) then
		DealWithRetAuth(false,2, ConnInfo)
		return
	end
	
	if GasConfig.SkipGBSAuth == 1 or IsTrustee then
		DealWithRetAuth(true,6, ConnInfo, {UserId},sMacAddress)
		return
	end
	if g_EProtocol.m_bConnected == false then
		--登陆时跳过验证(erating在连接着或者连接超时)
		LocalAuthLogin()
		return
	end
	local callback_func = RemoteAuthResult
	local local_auth_func = LocalAuthLogin
	g_EProtocol:UserLogin({UserName,Password,1,Connection.m_nPort,Connection.m_nIP,MatrixPasswd, MatrixCoord,game_id,DistStr,IDCode or ""}, 
						{callback_func, local_auth_func})
end

--帐号验证
function ServerCheckUser( Connection,UserName,Password,sLoginKey,sMacAddress,nGameID,DistStr,IDCode,MatrixPasswd, MatrixCoord)
	if not( g_ServerAttribute[g_CurServerId] and g_ServerAttribute[g_CurServerId].IsCanLogin ) then
		return
	end
	if Connection.m_strIP then
		--print("已经发过一次了,再发无效")--防止客户端连续发本消息
		return
	end
--	local function ontick(tick)
--		UnRegisterTick(tick)
--		CheckUser(Connection,UserName,Password,tonumber(sLoginKey),"","")
--	end
	local function callfunc()
		CheckUser(Connection,UserName,Password,tonumber(sLoginKey),MatrixPasswd, MatrixCoord,sMacAddress,nGameID,DistStr,IDCode)
		--RegisterTick("test", ontick, 15000)
	end
	
	local function callback(isUserTrust)
		if IsCppBound(Connection) then
			if not isUserTrust then
				CheckUserEntry(Connection, callfunc)
			else
				local ConnInfo = {Connection, UserName,Password,sLoginKey,Connection.m_strIP,Connection.m_nID,nGameID}
				DealWithRetAuth(false, 36, ConnInfo)
			end
		end
	end
	local data = {}
	data["UserName"] = UserName
	data["GameId"] = nGameID
	CallDbTrans("LoginServerDB", "CheckUserTrust", callback, data, UserName,nGameID)
end

function Gac2Gas:CheckTrusteeUser(Connection, CharName, Password, sLoginKey, sMacAddress, DistStr)
	local function callback(result, UserName, nGameID)
		if result == 1 then
			CheckUser(Connection,UserName,"",tonumber(sLoginKey),"","",sMacAddress,nGameID,DistStr,"", true)
		else
			local ConnInfo = {Connection, "","",sLoginKey,Connection.m_strIP,Connection.m_nID,nil}
			if result == -1 then  ---此角色未被托管
				DealWithRetAuth(false, 37, ConnInfo)
			elseif result == -2 then --托管密码错误
				DealWithRetAuth(false, 38, ConnInfo)
			end
		end
	end
	local data = {}
	data["CharName"] = CharName
	data["Password"] = Password
	CallDbTrans("LoginServerDB", "CheckTrusteeLogin", callback, data, CharName)
end


function Gac2Gas:CheckUser( Connection,UserName,Password,sLoginKey,sMacAddress,DistStr,MatrixCoord,MatrixPasswd)
	--yy专区通过这里登陆报错
	local GBSAuthType = tonumber(GasConfig.GBSAuthType)
	local game_id = tonumber(ERatingConfig.AGIP_GameID)
	local ConnInfo = {Connection, UserName,Password,sLoginKey,Connection.m_strIP,Connection.m_nID,game_id}
	if not(GBSAuthType == 0 or GBSAuthType == 1) then
		DealWithRetAuth(false,3, ConnInfo)
		LogErr("此服务器是联运专区，不能登陆其他账号,请检查客户端配置是否和服务端一致","UserName:" .. UserName .. "GBSAuthType:" .. GBSAuthType)
		return
	end
	if MatrixCoord ~= "" then
		if not (g_UserActivationCode[game_id] and g_UserActivationCode[game_id][UserName] == MatrixCoord) then
			--验证密保卡坐标是不是正确
			DealWithRetAuth(false,42, ConnInfo)
			return
		end
	end
	ServerCheckUser( Connection,UserName,Password,sLoginKey,sMacAddress,game_id,DistStr,"",MatrixPasswd, MatrixCoord)
end

--取消登录排队
function CancelLoginWaiting(Connection)
	g_LoginQueue.CanceLoginGame(Connection)
end

function Gac2Gas:CancelLoginWaiting(Connection)
	CancelLoginWaiting(Connection)
end

--====================多服的创建角色和切场景创建account================================
local UserNameList = {}
function Gas2GasDef:SetUserName(SerConn, UserName)
	--print("Gas2GasDef:SetUserName")
	for Key, Name in pairs(UserNameList) do
		if Name == UserName then
			UserNameList[Key] = nil
			break
		end
	end
	local sKey = UserName
	UserNameList[sKey] = UserName
	
	Gas2GasCall:RetUserKey(SerConn, sKey, UserName)
end

local function CreateAccountFun(Connection, CallBackFun)
	local nContextID, uAgreedVer,szName, nUserID, uYuanBao = unpack(Connection.LoginAccountInfo)
	Connection.LoginAccountInfo = nil
	CreateAccount(Connection, nContextID, uAgreedVer,szName, nUserID, uYuanBao, CallBackFun)
end

function Gac2Gas:CreateAccount(Connection, sKey)
	--这个名称是另一个服务器传过来的名称
	local UserName = UserNameList[sKey]
	--print(sKey.."======"..UserName)
	if UserName then
		UserNameList[sKey] = nil
		
		local function NewRoleFun()
			Gas2Gac:RetAccountSucc(Connection)
		end
		local game_id = Connection.m_nGameID
		local function callback(result)
			local nUserID,szName,uYuanBao,uAgreedVer = unpack(result)
			InitConnectionData(Connection, szName,game_id)
			Connection.LoginAccountInfo = {
					[1] = Connection.m_nID,
					[2] = uAgreedVer,
					[3] = szName,
					[4] = nUserID,
					[5] = uYuanBao,
					}
			CreateAccountFun(Connection, NewRoleFun)
		end
		
		local data = {}
		data["UserName"] = UserName
		data["game_id"] = game_id
		CallDbTrans("LoginServerDB", "GetAccountInfoFromName", callback, data, UserName,game_id)
	else
		--如果不对的话,就让他断开连接
		if Connection.ShutDown then
			Connection:ShutDown("UserNameList内没有找到UserName,断开连接")
		end
	end
end

function MultiServerCreateAccount(Connection, CallBackFun)
	CreateAccountFun(Connection, CallBackFun)
end
--==============================================================

local function OnUserForceLoginTick(szName, lastLoginTime,game_id)
	g_UserForceLoginData[game_id] = g_UserForceLoginData[game_id] or {}
	local tbl  = g_UserForceLoginData[game_id][szName]
	if not tbl then
		return
	end
	g_UserForceLoginData[game_id][szName] = nil
	
	if tbl.State ~= "Sure" or tbl.LastLoginTime ~=  lastLoginTime then
		return
	end
	local function callback(isSuccess)
		if isSuccess then
			LogErr("[用户非安全顶号] 在安全顶号持续长时间不能登录后, 为了不卡号进行了直接修改数据库的顶号. 这往往暗示用户登出存在bug, 请查看与登出有关的log即时解决.", "UserName:" .. szName)
		end
	end
	local data = {}
	data["UserName"] = szName
	data["LastLoginTime"] = lastLoginTime
	data["game_id"] = game_id
	CallDbTrans("LogoutServerDB", "ForceLogout", callback, data, szName,game_id)
end

function Gas2GasDef:CheckConnect(Conn, userName, lastLoginTime,game_id)
	Gas2GasAutoCall:RetCheckConnect(Conn, userName, lastLoginTime, g_CurServerId,game_id)
end

function Gas2GasDef:RetCheckConnect(Conn, userName, lastLoginTime, serverId,game_id)
	g_UserForceLoginData[game_id] = g_UserForceLoginData[game_id] or {}
	local tbl = g_UserForceLoginData[game_id][userName]
	if tbl and tbl.State == "WaitConfirm" 
		and tbl.LastLoginTime == lastLoginTime 
		and tbl.ConfirmServerId == serverId then
		tbl.State = "Sure"
	end
end

--强行顶号,上线
function Gac2Gas:UserForceTipSigal(Connection,sMacAddress)
	if not Connection.m_TempData then
		return
	end
	local data = Connection.m_TempData.m_Data
	local szName = data["name"]
	local UserID = data["userid"]
	local nContextID = Connection.m_TempData.m_nContextID
	local game_id = Connection.m_nGameID
	local function callback(ret, LoginKey, lastLoginTime, dbServerId)
		if ret then
			local account_info , uAgreedVer, campInfoTbl = ret[1], ret[2], ret[3]
			
			Connection.m_TempData.m_Data = nil
			Connection.m_TempData = nil
			
			if not IsCppBound(Connection) then
				SaveLogout(account_info[2])
				return
			end
			if g_UserForceLoginData[game_id] and g_UserForceLoginData[game_id][szName] then
				g_UserForceLoginData[game_id][szName] = nil
			end
			CreateAccount(Connection, nContextID, uAgreedVer,account_info[1],account_info[2],account_info[3], nil, campInfoTbl)
		else
				--调用其它的接口
				--添加一个发消息的Tick(检测玩家一直在线的话,就一直发让他下线的消息)
				
			for serverid in pairs(g_ServerList) do
				Gas2GasAutoCall:UserTipSigal(GetServerAutoConn(serverid), szName,Connection.m_nGameID)
			end

			if not g_UserForceLoginData[szName] then
				RegisterOnceTick(g_App, "UserForceLoginTick", OnUserForceLoginTick, 90 * 1000, szName, lastLoginTime,game_id)
				g_UserForceLoginData[game_id] = g_UserForceLoginData[game_id] or {}
				g_UserForceLoginData[game_id][szName] = {}
				g_UserForceLoginData[game_id][szName].State = "WaitConfirm"
				g_UserForceLoginData[game_id][szName].LastLoginTime = lastLoginTime
				g_UserForceLoginData[game_id][szName].ConfirmServerId = dbServerId
				Gas2GasAutoCall:CheckConnect(GetServerAutoConn(dbServerId), szName, lastLoginTime,game_id)
			end
		end
	end
	data["sMacAddress"] = sMacAddress
	data["game_id"] = game_id
	--local LoginServerSql = (g_DBTransDef["LoginServerDB"])
	CallDbTrans(LoginServerDB, "GetAccountInfoFromDB", callback, data, szName,game_id, UserID)
end

--======================顶账号时用=============================
function Gas2GasDef:UserTipSigal(SerConn, UserName,game_id)
--	print("Gas2GasDef:UserTipSigal")
	local old_account = g_AccountMgr:GetAccountInfo(game_id, UserName )
	if old_account then
		local function CallBack()
			Account_Logout(old_account, UserName,game_id)
		end
		coroutine.resume(coroutine.create(CallBack))
	end
end

function Gas2GasDef:UserAutoLoginGame(Conn, canLoginNum)
	g_LoginQueue.SetCanLoginNum(canLoginNum)
end

function DbCallBack:UserAutoLoginGame(canLoginNum, serverId)
	Gas2GasAutoCall:UserAutoLoginGame(GetServerAutoConn(serverId), canLoginNum)
end



function UserTrust(charName, password, trustLength, callback)
	local data = {}
	data["CharName"] = charName
	data["TrustLength"] = trustLength or 60 
	data["Password"] = password
	
	CallDbTrans("LoginServerDB", "UserTrust", callback,data)
end

function CancelUserTrust(charName, callback)
	local data = {}
	data["CharName"] = charName
	CallDbTrans("LoginServerDB", "CancelUserTrust", callback,data)
end
