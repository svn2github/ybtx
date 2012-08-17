gas_require "world/player/CreateServerPlayerInc"
--cfg_load "player/MercenaryLevel_Common"
gas_require "log_mgr/CLogMgr"

function  GMCommand.Executor.setpos( Connection, x, y )
	--print("setpos (" .. tostring(x) .. "," .. tostring(y) .. ")")
	--eSPR_Success,eSPR_IsMoving,eSPR_NotAllowed,eSPR_OutOfBound
	local Ret = Connection.m_Player:SetGridPos(x,y)
	if Ret == 0 then
		Ret = "Move to:" .. "x=" .. tostring(x) .. ",y=" .. tostring(y)
	elseif Ret == 1 then
		Ret = "Object is moving, move failed!"
	elseif Ret == 2 then
		Ret = "Move is forbid, move failed!"
	elseif Ret == 3 then
		Ret = "Out of area, move failed!"
	end
	return Ret
end

function  GMCommand.Executor.AddCurHP( Connection, nAdd )
	Connection.m_Player:CppAddCurHP( nAdd )
end

function  GMCommand.Executor.AddMaxHP( Connection, nAdd )
	Connection.m_Player:CppAddAFMaxHP( nAdd )
end

function  GMCommand.Executor.AddCurMP( Connection, nAdd )
	Connection.m_Player:CppAddCurMP( nAdd )
end

function  GMCommand.Executor.AddMaxMP( Connection, nAdd )
	Connection.m_Player:CppAddAFMaxMP( nAdd )
end

function GMCommand.Executor.SetHPMP( Connection, nHP, nMP )
	Connection.m_Player:CppInitHPMP(nHP, nMP)
end

function  GMCommand.Executor.AddMercLev( Connection, nAdd )
	Gas2Gac:ChangeSceneErrMsg(Connection, "This command is not useable now.", "")
	--[[
	--print("GM命令，增加雇佣兵等级！！！！")
	local Player = Connection.m_Player
	local resultMercLev = Player.m_uMercenaryLevel + nAdd
	if resultMercLev <= 0 then
		resultMercLev = 0
	elseif resultMercLev > g_MercenaryLevelTbl.MaxLevel then
		resultMercLev = g_MercenaryLevelTbl.MaxLevel	
	end
	
	local function callback(MLRes)
		if not MLRes then
			Gas2Gac:ChangeSceneErrMsg(Connection, "Haven't registered a mercenary.Can't add mercenary level.", "")
			return
		end
		if Player.m_uMercenaryLevel ~= resultMercLev then
			Player.m_uMercenaryLevel = resultMercLev
			Gas2Gac:RetSetMercenaryLevel(Connection, resultMercLev, true)
			Gas2Gac:RetSetMercenaryIntegral(Connection, Player.m_uMercenaryIntegral )
			--Gas2Gac:ClearMercenaryLevelFinishAwards(Connection)
			--UpdateMercenaryLevelTraceWnd(Connection, MLRes)
			--if Player.m_uMercenaryLevel ~= g_MercenaryLevelTbl.MaxLevel then
				--调用加佣兵称谓接口
			--	local Appellation = g_MercenaryLevelTbl[Player.m_uMercenaryLevel]["特权"]["称号"].Arg
			--	CGasAppellationAndMoral.AddNewAppellation(Player, Appellation)
			--end
		end
	end
	
	local data = {}
	data["CharId"] = Connection.m_Player.m_Properties:GetCharID()
	data["ResultMercLev"] = resultMercLev
	CallAccountAutoTrans(Connection.m_Account, "MercenaryLevelDB", "GMAddMercenaryLevelUp", callback, data)
	--]]
end

function  GMCommand.Executor.AddLevel( Connection, nAdd )
	local resultlevel = Connection.m_Player:CppGetLevel() + nAdd
	local MaxLevel	= g_TestRevisionMaxLevel
	if resultlevel <= 0 then		
		resultlevel = 1
	elseif resultlevel > MaxLevel then		
		resultlevel = MaxLevel	
	end
	local uCharID = Connection.m_Player.m_Properties:GetCharID()
	local function callback(result)
		CLevelUpGas.SetLevel(Connection.m_Player,resultlevel)
		--Connection.m_Player:CppSetLevel( resultlevel )
		CGasTeam.SetTeamMemberLevel(uCharID)
		Connection.m_Player:SetSkillToPlayer()
		Connection.m_Player:CppInitHPMP( Connection.m_Player:CppGetMaxHP(), Connection.m_Player:CppGetMaxMP() )
		if(Connection.m_Player:CppGetLevel()>=60 and Connection.m_Player:CppGetLevel()<120) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
		end
		if(Connection.m_Player:CppGetLevel()>=120) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
			Gas2Gac:NotifyOpenPanel(Connection,3)
		end
		Gas2Gac:SetExp( Connection,0,0,0,Connection.m_Player.m_uLevelExp,LevelExp_Common(resultlevel, "ExpOfCurLevelUp") )
	end
	local data = {}
	data["char_id"] 	= uCharID
	data["char_level"]	= resultlevel
	data["char_exp"]	= Connection.m_Player.m_uLevelExp
	data["nAddExp"] = 0
	data["addExpType"] = event_type_tbl["GM指令加经验"]
	
	OnSavePlayerExpFunc({Connection.m_Player})
	--local CharacterMediatorDB = (g_DBTransDef["CharacterMediatorDB"])
	CallAccountAutoTrans(Connection.m_Account, "CharacterMediatorDB", "AddLevel", callback, data)
end

function  GMCommand.Executor.TargetAddLevel( Connection,sTargetName, nAdd )
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	local resultlevel = Player:CppGetLevel() + nAdd
	local MaxLevel	= g_TestRevisionMaxLevel
	if resultlevel <= 0 then
		resultlevel = 1
	elseif resultlevel > MaxLevel then
		resultlevel = MaxLevel
	end
	local uCharID = Player.m_uID
	local function callback(result)
		local Player = g_GetPlayerInfoByName(sTargetName)
		if Player == nil then
			return
		end
		Player:CppSetLevel( resultlevel )
		Player:SetSkillToPlayer()
		Player:CppInitHPMP( Player:CppGetMaxHP(), Player:CppGetMaxMP() )
		CGasTeam.SetTeamMemberLevel(uCharID)
		if(Player:CppGetLevel()>=60 and Player:CppGetLevel()<120) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
		end
		if(Player:CppGetLevel()>=120) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
			Gas2Gac:NotifyOpenPanel(Connection,3)
		end
		Gas2Gac:SetExp( Player.m_Conn,0,0,0,Player.m_uLevelExp,LevelExp_Common(resultlevel, "ExpOfCurLevelUp") )
	end
	local data = {}
	data["char_id"] 	= uCharID
	data["char_level"]	= resultlevel
	data["char_exp"]	= Player.m_uLevelExp
	data["nAddExp"] = 0
	data["addExpType"] = event_type_tbl["GM指令加经验"]
	--local CharacterMediatorDB = (g_DBTransDef["CharacterMediatorDB"])
	CallAccountAutoTrans(Connection.m_Account, "CharacterMediatorDB", "AddLevel", callback, data)
end

function  GMCommand.Executor.SetTalent( Connection, szName, nPoint )
	local data = { ["name"] = szName, ["level"] = nPoint-1, ["kind"] = FightSkillKind.Talent, ["PlayerId"] = Connection.m_Player.m_uID ,
			["Class"] =  Connection.m_Player:CppGetClass(),["IsGMcmd"] = true, ["layer"] = 0,["PlayerLevel"] = Connection.m_Player:CppGetLevel(),["sceneName"] = Connection.m_Player.m_Scene.m_SceneName}
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then 
			Player:Lua_AddFightSkill(Data)
		else
			MsgToConn(Connection,Data)
			return
		end
	end	
	--local entry = (g_DBTransDef["FightSkillDB"])
	CallAccountAutoTrans(Connection.m_Account, "FightSkillDB", "Lua_AddFightSkill", CallBack, data)	
end
function  GMCommand.Executor.TargetSetTalent( Connection,sTargetName, szName, nPoint )
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	
	local data = { ["name"] = szName, ["level"] = nPoint-1, ["kind"] = FightSkillKind.Talent, ["PlayerId"] = Player.m_uID ,
			["Class"] =  Player:CppGetClass(),["IsGMcmd"] = true, ["layer"] = 0,["PlayerLevel"] = Player:CppGetLevel(),["sceneName"] = Player.m_Scene.m_SceneName}
	local function CallBack(bool,Data)
		local Player = g_GetPlayerInfoByName(sTargetName)
		if Player == nil then
			return
		end
		if bool then 
			Player:Lua_AddFightSkill(Data)
		else
			MsgToConn(Connection,Data)
			return
		end
	end	
	--local entry = (g_DBTransDef["FightSkillDB"])
	CallAccountAutoTrans(Connection.m_Account, "FightSkillDB", "Lua_AddFightSkill", CallBack, data)	
end

function GMCommand.Executor.AddMercExp( Connection, nAdd )
	local Player = Connection.m_Player
	Player.m_uMercenaryIntegral = Player.m_uMercenaryIntegral + nAdd
	if Player.m_uMercenaryIntegral < 0 then
		Player.m_uMercenaryIntegral = 0
	end
	local nCurMercLev = Player.m_uMercenaryLevel
	
	--if nCurMercLev ==#(MercenaryLevel_Common) then
	--	return
	--end
	
--	while Player.m_uMercenaryIntegral >= MercenaryLevel_Common[nCurMercLev].IntegralOfCurLevelUp do
--		--如果不能升级，当前经验为0
--		if MercenaryLevel_Common[nCurMercLev].IntegralOfCurLevelUp == 0 
--			or MercenaryLevel_Common[nCurMercLev].IntegralOfCurLevelUp == "" then
--			Player.m_uMercenaryLevel = 0
--			break
--		end
--		
--		--减去升级经验，并且提升一级
--		Player.m_uMercenaryLevel = Player.m_uMercenaryLevel - MercenaryLevel_Common[nCurMercLev].IntegralOfCurLevelUp
--		nCurMercLev = nCurMercLev + 1
--		if nCurMercLev >= #(MercenaryLevel_Common) then
--			Player.m_uMercenaryIntegral  = 0
--			break
--		end
--	end
	
	local function callback(result)
		Gas2Gac:RetSetMercenaryIntegral( Connection, Player.m_uMercenaryIntegral )
	end
	
	local data = {}
	data["char_id"] = Player.m_Properties:GetCharID()
	data["char_mercLevel"] = nCurMercLev
	data["char_mercIntegral"] = Player.m_uMercenaryIntegral 
	CallAccountAutoTrans(Connection.m_Account, "CharacterMediatorDB", "AddMercenaryLevel", callback, data)
end

function  GMCommand.Executor.AddExp( Connection, nAdd )
	Connection.m_Player.m_uLevelExp = Connection.m_Player.m_uLevelExp + nAdd
	if Connection.m_Player.m_uLevelExp < 0 then
		Connection.m_Player.m_uLevelExp = 0
	end
	local nCurlevel = Connection.m_Player:CppGetLevel()
	if nCurlevel == LevelExp_Common.GetHeight() then
		return
	end
	if nCurlevel == g_TestRevisionMaxLevel then
		return
	end
	local bFlag = false
	while Connection.m_Player.m_uLevelExp >= LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") do
		--如果不能升级，当前经验为0
		if LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") == 0 then
			Connection.m_Player.m_uLevelExp = 0
			break
		end
		--减去升级经验，并且提升一级
		Connection.m_Player.m_uLevelExp = Connection.m_Player.m_uLevelExp - LevelExp_Common(nCurlevel, "ExpOfCurLevelUp")

		nCurlevel=nCurlevel+1
		bFlag = true
		if nCurlevel == LevelExp_Common.GetHeight() then
			Connection.m_Player.m_uLevelExp=0
		end
		if nCurlevel == g_TestRevisionMaxLevel then
			Connection.m_Player.m_uLevelExp=0
		end	
	end
	
	local function callback(result)
		Connection.m_Player:CppSetLevel(nCurlevel)
		Connection.m_Player:SetSkillToPlayer()
		local nCurlevel = Connection.m_Player:CppGetLevel()
		if bFlag then
			CGasTeam.SetTeamMemberLevel(Connection.m_Player.m_uID)
		end
		if(nCurlevel==60) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
		end
		if(nCurlevel==120) then
			Gas2Gac:NotifyOpenPanel(Connection,2)
			Gas2Gac:NotifyOpenPanel(Connection,3)
		end
		Gas2Gac:SetExp( Connection,nAdd,0,0,Connection.m_Player.m_uLevelExp,LevelExp_Common(nCurlevel, "ExpOfCurLevelUp") )
	end
	
	local data = {}
	data["char_id"] 		= Connection.m_Player.m_Properties:GetCharID()
	data["char_level"]	= nCurlevel
	data["char_exp"]	= Connection.m_Player.m_uLevelExp
	data["nAddExp"] 	= nAdd
	data["addExpType"] = event_type_tbl["GM指令加经验"]
	OnSavePlayerExpFunc({Connection.m_Player})
	--local CharacterMediatorDB = (g_DBTransDef["CharacterMediatorDB"])
	CallAccountAutoTrans(Connection.m_Account, "CharacterMediatorDB", "AddLevel", callback, data)
end

function  GMCommand.Executor.AddFx( Connection, nFxID, nPlayTime )
	local Player = Connection.m_Player
	local ObjGlobalId = Player:GetEntityID()
	Gas2Gac:AddFx( Player:GetIS(0),		ObjGlobalId, nFxID, nPlayTime )
	Gas2Gac:AddFx( Player:GetConnection(),  ObjGlobalId, nFxID, nPlayTime )
end

function  GMCommand.Executor.RemoveFx( Connection, nFxID )
	local Player = Connection.m_Player
	local ObjGlobalId = Player:GetEntityID()
	Gas2Gac:RemoveFx( Player:GetIS(0), ObjGlobalId, nFxID )
	Gas2Gac:RemoveFx( Player:GetConnection(), ObjGlobalId, nFxID )
end

function  GMCommand.Executor.SetClass( Connection, nClass )
	Connection.m_Player:CppSetClass(nClass)
	Gas2Gac:SetClass( Connection,	nClass )
end

function  GMCommand.Executor.TargetSetClass( Connection,sTargetName, nClass )
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:CppSetClass(nClass)
	--Gas2Gac:SetClass( Player.m_Conn,	nClass )
end

--设置阵营
function GMCommand.Executor.setcamp(Connection,Camp)
	if not IsNumber(Camp) then
		local str = "The Camp value must be a number!"
		Gas2Gac:ChangeSceneErrMsg(Connection, str, "")
		return
	elseif Camp<0 or Camp>10 then
		local str = "The Camp value is out of range(0-10)!"
		Gas2Gac:ChangeSceneErrMsg(Connection, str, "")
		return
	end
	
	local player = Connection.m_Player
	if Camp>=1 and Camp<=3 then
		local function CallBack(result)
			player:CppSetCamp(Camp)
		end
		--local lGMDBExecutor = (g_DBTransDef["GMDB"])
		local parameter = { ["player_id"] = Connection.m_Player.m_uID, ["camp"] = Camp }
		CallAccountAutoTrans(Connection.m_Account, "GMDB", "SetCamp", CallBack, parameter)
	else
		player:CppSetCamp(Camp)
	end
end

--设置阵营
function GMCommand.Executor.targetsetcamp(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	
	if not IsNumber(Camp) then
		local str = "The Camp value must be a number!"
		Gas2Gac:ChangeSceneErrMsg(Connection, str, "")
		return
	elseif Camp<0 or Camp>10 then
		local str = "The Camp value is out of range(0-10)!"
		Gas2Gac:ChangeSceneErrMsg(Connection, str, "")
		return
	end
	
	if Camp>=1 and Camp<=3 then
		local function CallBack(result)
			local Player = g_GetPlayerInfoByName(sTargetName)
			if Player ~= nil then
				Player:CppSetCamp(Camp)
			end
		end
		local parameter = { ["player_id"] = Player.m_uID, ["camp"] = Camp }
		CallAccountAutoTrans(Connection.m_Account, "GMDB", "SetCamp", CallBack, parameter)
	else
		player:CppSetCamp(Camp)
	end
end

function GMCommand.Executor.setPK(Connection,Value)

--/**旧PK代码**/
--	if Value ~= 2 then
--		Connection.m_Player.m_CriminalTime = 0
--		UnRegisterTick(Connection.m_Player.m_PkCriminalTick)
--		Connection.m_Player.m_PkCriminalTick = nil
--	end
--	Connection.m_Player:SetPKSwitch(Value)
end

function GMCommand.Executor.setPKValue(Connection,Value)
--/**旧PK代码**/
--	if Value > 0 then
--		Connection.m_Player:SetPKSwitch(2)
--	end
--	Connection.m_Player:PlusPkValue(Value)
end

function GMCommand.Executor.addStrikeValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("StrikeValueAdder", Camp)
end

function GMCommand.Executor.addStrikeMultiValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("StrikeMultiValueAdder", Camp)
end

function GMCommand.Executor.addBlockRate(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("BlockRateMultiplier", Camp)
end
function GMCommand.Executor.addBlockDamage(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("BlockDamageAdder", Camp)
end
function GMCommand.Executor.addDefence(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("DefenceAdder", Camp)
end
function GMCommand.Executor.addPhysicalDodgeValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("PhysicalDodgeValueAdder", Camp)
end
function GMCommand.Executor.addAccuratenessValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("AccuratenessValueAdder", Camp)
end
function GMCommand.Executor.addResilienceValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("ResilienceValueAdder", Camp)
end
function GMCommand.Executor.addMagicDodgeValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("MagicDodgeValueAdder", Camp)
end
function GMCommand.Executor.addMagicHitValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("MagicHitValueAdder", Camp)
end
function GMCommand.Executor.addNatureResistanceValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("NatureResistanceValueAdder", Camp)
end
function GMCommand.Executor.addDestructionResistanceValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("DestructionResistanceValueAdder", Camp)
end
function GMCommand.Executor.addEvilResistanceValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("EvilResistanceValueAdder", Camp)
end
function GMCommand.Executor.addPhysicalDPS(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("PhysicalDPSAdder", Camp)
end
function GMCommand.Executor.addMagicDamageValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("MagicDamageValueAdder", Camp)
end
function GMCommand.Executor.addParryValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("ParryValueAdder", Camp)
end
function GMCommand.Executor.addHealthPoint(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("HealthPointAdder", Camp)
end
function GMCommand.Executor.addManaPoint(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("ManaPointAdder", Camp)
end

function GMCommand.Executor.addStrikeResistanceValue(Connection,Camp)
	Connection.m_Player:GM_CppSetPropertyValueByName("StrikeResistanceValueAdder", Camp)
end

function GMCommand.Executor.addMoveSpeed(Connection,Camp)
	Connection.m_Player:ChangeMoveSpeedByGMCommand(Camp)
end

function GMCommand.Executor.addTargetStrikeValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("StrikeValueAdder", Camp)
end

function GMCommand.Executor.addTargetStrikeMultiValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("StrikeMultiValueAdder", Camp)
end

function GMCommand.Executor.addTargetBlockRate(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("BlockRateMultiplier", Camp)
end
function GMCommand.Executor.addTargetBlockDamage(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("BlockDamageAdder", Camp)
end
function GMCommand.Executor.addTargetDefence(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("DefenceAdder", Camp)
end
function GMCommand.Executor.addTargetPhysicalDodgeValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("PhysicalDodgeValueAdder", Camp)
end
function GMCommand.Executor.addTargetAccuratenessValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("AccuratenessValueAdder", Camp)
end
function GMCommand.Executor.addTargetResilienceValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("ResilienceValueAdder", Camp)
end
function GMCommand.Executor.addTargetMagicDodgeValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("MagicDodgeValueAdder", Camp)
end
function GMCommand.Executor.addTargetMagicHitValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("MagicHitValueAdder", Camp)
end
function GMCommand.Executor.addTargetNatureResistanceValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("NatureResistanceValueAdder", Camp)
end
function GMCommand.Executor.addTargetDestructionResistanceValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("DestructionResistanceValueAdder", Camp)
end
function GMCommand.Executor.addTargetEvilResistanceValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("EvilResistanceValueAdder", Camp)
end
function GMCommand.Executor.addTargetPhysicalDPS(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("PhysicalDPSAdder", Camp)
end
function GMCommand.Executor.addTargetMagicDamageValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("MagicDamageValueAdder", Camp)
end
function GMCommand.Executor.addTargetParryValue(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("ParryValueAdder", Camp)
end
function GMCommand.Executor.addTargetHealthPoint(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("HealthPointAdder", Camp)
end
function GMCommand.Executor.addTargetManaPoint(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:GM_CppSetPropertyValueByName("ManaPointAdder", Camp)
end
function GMCommand.Executor.addTargetMoveSpeed(Connection,sTargetName,Camp)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	Player:ChangeMoveSpeedByGMCommand(Camp)
end

function GMCommand.Executor.GotoMercLevel( Connection, nLevel )
	Connection.m_Player.GMGotoMercLevel = nLevel
end

function GMCommand.Executor.OpenAllAreaFb(Connection)
	Gas2Gac:OpenAllAreaFb(Connection)	
end

function  GMCommand.Executor.AddLiveskill( Connection, sSkillName,nExp )
	local function CallBack(result)
		if result and result:GetRowNum() > 0 then
			MsgToConn(Connection,9529)
			CLiveskillBase.RetLiveskillInfo(Connection,result)
		end
	end
	local data = {}
	data.uPlayerID = Connection.m_Player.m_uID
	data.nExp = nExp
	data.sSkillName = sSkillName
	CallAccountAutoTrans(Connection.m_Account,"LiveSkillBaseDB", "AddLiveskillGM", CallBack, data)		
	return ""
end

function  GMCommand.Executor.AddLiveSkillPractice( Connection, sSkillName,sPracticeName,nPractice )
	local function CallBack(bFlag,uMsgID,sMsg)
		if not bFlag then
			if IsNumber(uMsgID) then
				if sMsg then
					MsgToConn(Connection,uMsgID,sMsg)
				else
					MsgToConn(Connection,uMsgID)
				end
			end
			return
		end
		MsgToConn(Connection,9564)
	end
	local data = {}
	data.uPlayerID = Connection.m_Player.m_uID
	data.sPracticeName = sPracticeName
	data.sSkillName = sSkillName
	data.nPractice = nPractice
	CallAccountAutoTrans(Connection.m_Account,"LiveSkillBaseDB", "AddLiveSkillPracticeGM", CallBack, data)		
	return ""
end

function  GMCommand.Executor.ChangeMailTime( Connection,nLeftTime)
	local function CallBack()
	end
	local data = {}
	data.uPlayerID = Connection.m_Player.m_uID
	data.nLeftTime = nLeftTime
	CallAccountAutoTrans(Connection.m_Account,"MailDB", "ChangeMailTimeGM", CallBack, data)		
	return ""
end
function  GMCommand.Executor.AddNonFightSkill( Connection, sSkillName )
	local Player = Connection.m_Player
	if not Player then return end
	local uPlayerID = Player.m_uID
	--角色死亡,不可以学习技能
	if not Player:CppIsLive() then
		return
	end

	local data = { ["SkillName"] = sSkillName, ["PlayerId"] = uPlayerID}
	local function CallBack(bool,Data)
		if bool then 
			if IsCppBound(Player) then
				Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
				if Player:CppIsLive() then
					Player:AddSkill(sSkillName,0)
				else
					Player.AddSkillTbl = {}
					table.insert(Player.AddSkillTbl,{sSkillName,0})
				end
				Gas2Gac:ReturnNonFightSkill(Connection, sSkillName)	
				Gas2Gac:CancelLearnNoneFightSkill(Player.m_Conn, Player:GetEntityID())
			end	
		else
			if IsNumber(Data) then
				MsgToConn(Connection,Data)
				return
			end
		end
	end
	CallAccountAutoTrans(Connection.m_Account,"GMDB","AddNonFightSkill",CallBack,data)		
	return ""
end

function  GMCommand.Executor.SetTimeDistortedRatio( Connection, fTimeDistortedRatio )
	--print("SetTimeDistortedRatio", fTimeDistortedRatio)
	local Player = Connection.m_Player
	local CoreScene = Player.m_Scene.m_CoreScene
	CoreScene:SetTimeDistortedRatio(fTimeDistortedRatio)
end

function  GMCommand.Executor.setRand( Connection, iValue )
	CCfgGlobal_SetRandFixedValue(iValue)
end

function  GMCommand.Executor.setRandf( Connection, dValue )
	CCfgGlobal_SetRandfFixedValue(dValue)
end

function  GMCommand.Executor.clearRand( Connection )
	CCfgGlobal_ClearRandFixedValue()
end

function  GMCommand.Executor.clearRandf( Connection )
	CCfgGlobal_ClearRandfFixedValue()
end

function GMCommand.Executor.FullRage(Connection, bValue)
	if bValue ~= 1 then
		bValue = false
	end
	Connection.m_Player:CppFullRage(bValue)
end
