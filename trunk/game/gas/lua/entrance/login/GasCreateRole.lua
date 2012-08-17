cfg_load "scene/SceneMapDifCamp_Common"
gac_gas_require "scene_mgr/SceneCfg"

g_BornSkills = {}

g_BornSkills[ EClass.eCL_Warrior ] = {} 

g_BornSkills[ EClass.eCL_MagicWarrior ] = 
{
		{ 901, 1 },
		{ 1006, 6 },
		{ 1016, 6 },
		{ 1026, 6 },
		{ 1036, 6 },
		{ 1046, 6 },
		{ 1051, 1 },
		{ 1066, 6 },
		{ 1071, 1 },
		{ 1081, 1 },
		{ 1091, 1 },
		{ 1101, 1 },
		{ 1102, 1 },
		{ 1103, 1 },
		{ 1111, 1 },
		{ 1112, 1 },
		{ 1113, 1 },
		{ 1121, 1 },
		{ 1122, 1 },
		{ 1123, 1 },
}

g_BornSkills[ EClass.eCL_Paladin ] = 
{
		{ 903, 1 },
		{ 2001, 1 },
		{ 2016, 6 },
		{ 2026, 6 },
		{ 2036, 6 },
		{ 2046, 6 },
		{ 2056, 6 },
		{ 2061, 1 },
		{ 2076, 7 },
		{ 2086, 6 },
		{ 2091, 1 },
		{ 2101, 1 },
		{ 2116, 6 },
		{ 2121, 1 },
		{ 2131, 1 },
		{ 2141, 1 },
		{ 2151, 1 },
		{ 2163, 1 },
		{ 2173, 1 },
		{ 2183, 1 },
}

g_BornSkills[ EClass.eCL_NatureElf ] = 
{
		{ 905, 1 },
		{ 906, 1 },
		{ 3006, 6 },
		{ 3011, 1 },
		{ 3026, 6 },
		{ 3036, 6 },
		{ 3046, 6 },
		{ 3056, 6 },
		{ 3066, 6 },
		{ 3076, 6 },
		{ 3081, 1 },
		{ 3091, 1 },
		{ 3101, 1 },
}

g_BornSkills[ EClass.eCL_EvilElf ] = 
{
		{ 907, 1 },
		{ 4006, 6 },
		{ 4016, 6 },
		{ 4026, 6 },
		{ 4036, 6 },
		{ 4041, 1 },
		{ 4056, 6 },
		{ 4066, 6 },
		{ 4076, 6 },
		{ 4086, 6 },
		{ 4096, 6 },
		{ 4101, 1 },
		{ 4111, 1 },
		{ 4131, 1 },
		{ 4132, 1 },
		{ 4133, 1 },
		{ 4141, 1 },
		{ 4142, 1 },
		{ 4143, 1 },
}

g_BornSkills[ EClass.eCL_Priest ] = 
{
		{ 909, 1 },
		{ 5006, 6 },
		{ 5016, 6 },
		{ 5026, 6 },
		{ 5036, 6 },
		{ 5041, 1 },
		{ 5051, 1 },
		{ 5061, 1 },
		{ 5076, 6 },
		{ 5086, 6 },
		{ 5096, 6 },
		{ 5106, 6 },
		{ 5116, 6 },
		{ 5171, 1 },
		{ 5172, 1 },
		{ 5173, 1 },
		{ 5181, 1 },
		{ 5182, 1 },
		{ 5183, 1 },
}

g_BornSkills[ EClass.eCL_DwarfPaladin ] = 
{
		{ 911, 1 },
		{ 6006, 6 },
		{ 6011, 1 },
		{ 6021, 1 },
		{ 6036, 6 },
		{ 6046, 6 },
		{ 6066, 6 },
		{ 6076, 6 },
		{ 6086, 6 },
		{ 6096, 6 },
		{ 6116, 6 },
		{ 6131, 1 },
		{ 6141, 1 },
		{ 6171, 1 },
		{ 6172, 1 },
		{ 6173, 1 },
		{ 6181, 1 },
		{ 6182, 1 },
		{ 6183, 1 },
}

g_BornSkills[ EClass.eCL_ElfHunter ] = {}
g_BornSkills[ EClass.eCL_OrcWarrior ] = {}

local function CreateRole(Connection,ret,code, rushRegiAccountFlag)
	if ret then
	    if rushRegiAccountFlag == 1 then --如果是抢注角色活动中
		    Gas2Gac:RetCreateRoleSucAtRushActivity(Connection)
		else
		    Gas2Gac:ReturnCreateRole(Connection,0)
		end
	else
		Gas2Gac:ReturnCreateRole(Connection,code)
	end
end

local function CheckNewRoleSceneInfo(nCamp)

	for i, v in ipairs( SceneMapDifCamp_Common:GetKeys() ) do
		local info = SceneMapDifCamp_Common(v)
		if info("Camp") == nCamp then
			local sceneName = info("SceneMap") 
			if Scene_Common[sceneName] == nil 
				or Scene_Common[sceneName]["SceneType"] ~= 1 then
				return false
			else
				return true
			end
		end
	end
	return false
end


local UserConnList = {}
function Gas2GasDef:RetUserKey(SerConn, sKey, UserName)
--print("Gas2GasDef:RetUserKey")
	if UserConnList[UserName] then
		local CampType = UserConnList[UserName].CampType
		local Ip = UserConnList[UserName].Ip
		local Port = UserConnList[UserName].Port
		
		Gas2Gac:RetServerIpInfo(UserConnList[UserName].Conn, sKey, CampType, Ip, Port)
		UserConnList[UserName] = nil
	end
end

--阵营信息
local CampInfoList = {
			[1] = "艾米帝国树屋酒吧",
			[2] = "神圣教廷树屋酒吧",
			[3] = "修斯帝国树屋酒吧",
			}

function Gac2Gas:GetCampServerIpInfo(Connection, CampType)
--	if CampType >= 1 and CampType <= 3 then
--		local SceneId,ServerId = g_SceneMgr:GetSceneByName(CampInfoList[CampType])
--		if ServerId then
--			
--			if g_CurServerId == ServerId then
--				--print("因为场景是一个服务器,所以就不用断开连接了!!")
--				Gas2Gac:RetServerIpInfo(Connection, "", CampType, "", 0)
--			else
--				local Ip, Port = GetServerIporPort(ServerId)
--				if Ip then
--					
--					local UserName = Connection.m_UserName
--					UserConnList[UserName] = {}
--					UserConnList[UserName].Conn = Connection
--					UserConnList[UserName].CampType = CampType
--					UserConnList[UserName].Ip = Ip
--					UserConnList[UserName].Port = Port
--					Gas2GasCall:SetUserName(GetServerConn(ServerId), UserName)
--					
--					--print("因为场景不是同服务器,所以要断开连接!!")
--					--print("要去往服务器的IP:",Ip,"  Port:",Port)
--				end
--			end
--			
--		end
--	end
end

function Gac2Gas:CreateRole( Connection,strName,nHair,nHairColor,nFace,nScale,nSex,nClass,nCamp,xPos,yPos)
	--nUsID, strName, nHair,nHairColor,nFace, nSex,nClass,m_nInherence
	--检查名字是否正确	
	--应该还要包含其他的一些检测，包括性别，类型，门派等
	if CheckNewRoleSceneInfo(nCamp) == false then
		Gas2Gac:ReturnCreateRole(Connection,10004)--MsgBox_Client中的msgID，对应信息：非法字符
		return 
	end
	if not IsCppBound(Connection) then return end
	local strUserName = Connection.m_UserName
	local game_id = Connection.m_nGameID
	local userid = Connection.m_Account:GetID()
	local HP = CStaticAttribMgr_CalcBasicMaxHealthPoint(nClass,150)
	local MP = CStaticAttribMgr_CalcBasicMaxManaPoint(nClass,150)
	local data = {
			["strName"] = strName,
			["strUserName"] = strUserName,
			["nHair"] = g_ParseCustomTblClient[nHair],
			["nHairColor"] = g_ParseCustomTblClient[nHairColor],
			["nFace"] = g_ParseCustomTblClient[nFace],
			["nScale"] = nScale,
			["nSex"] = nSex,
			["nClass"] = nClass,
			["nCamp"] = nCamp,
			["nInherence"] = nInherence,
			["HP"] = HP,
			["MP"] = MP,
			["time"] = os.time(),
			["xPos"] = xPos,
			["yPos"] = yPos,
			["recruitFlag"] = Connection.m_RecruitInfo[nCamp],
			["serverType"] = g_ServerType
		}

	local function CreateRoleLocalSave(role_id,ret_code,userinfo)
		if role_id == 0 then
			if ret_code == 0 then
				ret_code = 10005
			end
			Gas2Gac:ReturnCreateRole(Connection,ret_code)  --MsgBox_Client配置表中id，对应信息：用户名已存在
			return
		end
		userinfo["role_id"] = role_id
		userinfo["game_id"] = game_id
		local function callback(result)
			if not IsCppBound(Connection) then
				return
			end
			local ret, code,tbl = unpack(result)
			local rushRegiAccountFlag  = 0 
			if ret == true then
                rushRegiAccountFlag = tbl["RushRegiAccountFlag"]
			end
			CreateRole(Connection,ret,code, rushRegiAccountFlag)
			if ret then	--创建角色成功
				local char_id = code
				local http = HttpClient:new()
				local host = CISConfig.CISHost
				local port = CISConfig.CISPort
				local url  = CISConfig.CISAddRole_url
				local params = { ['userid'] = userid,
					['rolename'] = strName,
					['roleid'] = code,
					['sex'] = nSex,
					['camp'] = nCamp,
					['class'] = nClass,
					['serv_id'] = tonumber(ERatingConfig["AGIP_Gateway_id"])*100+ g_CurServerId
					}
	
				local function cis_create_role_callback(ret)
				end
				http:SendData(host, port, url, params, cis_create_role_callback)
				if rushRegiAccountFlag ~= 1 then--并且在非抢注角色情况下,才创建好角色后登入游戏
				    Gac2Gas:CharEnterGame( Connection, 0, char_id )
				end
			end
		end

		CallDbTrans("GasCreateRoleDB", "CreateRole", callback, userinfo,userid)
	end

	local callback_func = CreateRoleLocalSave
	local callback_data = data

	if GasConfig.SkipGBSAuth==1 then
		CreateRoleLocalSave("", nil,data)
		return
	end

	local function cis_checkname_callback(resp)
		local ret_code = tonumber(resp)
		if ret_code == 200 then
			g_EProtocol:CreateRole(
				{
					Connection.m_Account:GetID(), strName, nSex, nClass, 
					1, Connection.m_nIP, Connection.m_nPort, nCamp
				},
				{ callback_func, callback_data }
				)
		else
			Gas2Gac:ReturnCreateRole(Connection,40)  --MsgBox_Client配置表中id，对应信息：用户名已存在
		end
	end

	local http = HttpClient:new()
	local host = CISConfig.CISHost
	local port = CISConfig.CISPort
	local url  = CISConfig.CISCheckName_url
	local params = { ['userid'] = Connection.m_Account:GetID(),
					['rolename'] = strName,
					['serv_id'] = tonumber(ERatingConfig["AGIP_Gateway_id"])*100+ g_CurServerId
				}
	
	if not http:SendData(host, port, url, params, cis_checkname_callback) then
		Gas2Gac:ReturnCreateRole(Connection,39)  --MsgBox_Client配置表中id，对应信息：用户名已存在
		return
	end
			
	--]]

end

