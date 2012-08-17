gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "skill/SkillCommon"
cfg_load "skill/SkillPart_Common"
cfg_load "skill/NonFightSkill_Common"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"

local FightSkillDB = "FightSkillDB"
local tblSkillLearnLevel = 
			{
				[1] = 10,		--学习每层对应需要的玩家等级
				[2] = 20,
				[3] = 30,
				[4] = 45,
				[5] = 30,		
				[6] = 45,
				[7] = 30,
				[8] = 45,				
			}
			
local SkillNode = 
 	 { 
 	 		[1] = 1,  --索引代表结点，值对应层，共7个结点，4层
 	 		[2] = 2,
 	 		[3] = 3,
 	 		[4] = 3,
 	 		[5] = 4,
 	 		[6] = 4,
 	 		[7] = 4
 	 }
--------------		skill 	------------------------
function CCharacterMediator:SetSkillToPlayer()
	if self.m_FightSkillTbl then
		for SkillName , SkillLevel in pairs(self.m_FightSkillTbl) do
			self:AddSkill(SkillName , SkillLevel)
		end
	end
	self:AddNonFightSkill()
end

CCharacterMediator.OldAddSkill = CCharacterMediator.AddSkill
function CCharacterMediator:NewAddSkill(SkillName,SkillLevel)
	if SkillPart_Common(SkillName) then
		local LevelTbl  = g_PlayerLevelToSkillLevelTbl[SkillName]
		if (#(LevelTbl) == 1 and tostring(LevelTbl[1][3]) == "l") then
			SkillLevel = self:CppGetLevel()
		end
		if self.m_FightSkillTbl then
			self.m_FightSkillTbl[SkillName]  = SkillLevel
		elseif self.m_FightSkillTbl == nil then
			self.m_FightSkillTbl = {} 
			self.m_FightSkillTbl[SkillName]  = SkillLevel
		end
	end
	self:OldAddSkill(SkillName,SkillLevel)
end
CCharacterMediator.AddSkill = CCharacterMediator.NewAddSkill

function CCharacterMediator:Lua_AddNonFightSkill( SkillName,QuestInfo)
	--角色死亡,不可以学习技能
	if not self:CppIsLive() then
		return
	end
	local data = { 
					["SkillName"] = SkillName, 
					["PlayerId"] = self.m_uID ,
					["QuestInfo"] = QuestInfo
				}
	local function CallBack(bool,Data)
		if bool then 
			if self:CppIsLive() then
				self:AddSkill(Data.SkillName,0)
			else
				self.AddSkillTbl = {}
				table.insert(self.AddSkillTbl,{Data.SkillName,0})
			end
			Gas2Gac:ReturnNonFightSkill( self.m_Conn, Data.SkillName)		
			Gas2Gac:ReturnLearnSkill(self.m_Conn, Data.SkillName,0)
			if Data["QuestInfo"] then
				Gas2Gac:RetAddQuestVar(self.m_Conn, Data["QuestInfo"]["sQuestName"], Data["QuestInfo"]["sVarName"],Data["QuestInfo"]["iNum"])
			end
			--更新佣兵等级特权和追踪窗口
			--UpdateMercenaryLevelTraceWnd(self.m_Conn, Data["MLRes"])
			--if Data["MLPower"] then
			--	Gas2Gac:RetUpdateMercenaryLevelAwardItem(self.m_Conn, Data["MLPower"])
			--end
			if Data["IsSaveShortCut"] then
				Gas2Gac:ReturnShortcut(self.m_Conn, 10, EShortcutPieceState.eSPS_Skill, "气魂击", 1, 1)
			end
		else
			if Data ~= 0 then
				MsgToConn(self.m_Conn,Data)
			end
			return
		end
	end
	CallAccountManualTrans(self.m_Conn.m_Account, FightSkillDB, "LuaDB_AddNonFightSkill", CallBack, data)	
end

function CCharacterMediator:Lua_AddFightSkill( data ) --该函数需要优化
	local fs_kind = data["kind"]
	local current_level = data["current_level"]
	local IsGMcmd = data["IsGMcmd"]
	local name = data["name"]
	
	if not self.m_Conn.m_Player:CppIsLive() then
		self.AddTalentTbl = {}
		self.AddSkillTbl = {}
		local ClassID = self:CppGetClass()
		local TalentTbl =  loadGeniusfile(ClassID)		
		if IsGMcmd and fs_kind == FightSkillKind.Talent then
			if tonumber(TalentTbl(name,"IsSkill")) == 1 then
				table.insert(self.AddSkillTbl,{name,current_level})
				Gas2Gac:ReturnLearnSkill(self.m_Conn, name,current_level)
			end
			table.insert(self.AddTalentTbl,{name,10})
		elseif fs_kind == FightSkillKind.Talent then
			if tonumber(TalentTbl(name,"IsSkill")) == 1 then
				table.insert(self.AddSkillTbl,{name,current_level})
				Gas2Gac:ReturnLearnSkill(self.m_Conn, name,current_level)
			end
			table.insert(self.AddTalentTbl,{name,10})
		elseif (fs_kind == FightSkillKind.Skill) then
			table.insert(self.AddSkillTbl,{name,current_level})	
			Gas2Gac:ReturnLearnSkill(self.m_Conn, name,current_level)	
		end
	else
		if IsGMcmd and fs_kind == FightSkillKind.Talent then
			local ClassID = self:CppGetClass()
			local TalentTbl =  loadGeniusfile(ClassID)
			if tonumber(TalentTbl(name,"IsSkill")) == 1 then
				self:AddSkill(name,current_level)
				Gas2Gac:ReturnLearnSkill(self.m_Conn, name,current_level)
			end
			self:InsertTalent( name, 10 )
		elseif fs_kind == FightSkillKind.Talent then
			local ClassID = self:CppGetClass()
			local TalentTbl =  loadGeniusfile(ClassID)	
			if tonumber(TalentTbl(name,"IsSkill")) == 1 then	
				self:AddSkill(name,current_level)
				Gas2Gac:ReturnLearnSkill(self.m_Conn, name,current_level)
			end
			self:InsertTalent( name, 10 )
		elseif (fs_kind == FightSkillKind.Skill) then	
			self:AddSkill(name,current_level)
			Gas2Gac:ReturnLearnSkill(self.m_Conn, name,current_level)
		end		
	end
	Gas2Gac:AddTalentSkill( self.m_Conn, name, current_level  )	
end

--学习天赋
function Gac2Gas:AddTalentSkill( Connection, TalentName,TreeNode)
	if not Connection.m_Player then
		return
	end
	
	--角色死亡,不可以学习技能
	if not Connection.m_Player:CppIsLive() then
		return
	end
	--4阶段,0说明是共用技能
	if not (TreeNode > 0 and TreeNode <= 8) then
		return
	end
	if TreeNode > 0 then
		--等级不够不能学习
		if Connection.m_Player:CppGetLevel() < tblSkillLearnLevel[TreeNode] then
			MsgToConn(Connection,1500)
			return
		end
	end
	
	local data = { 	
					["TalentName"] = TalentName, 
					["PlayerId"] = Connection.m_Player.m_uID,
					["Class"] =  Connection.m_Player:CppGetClass(),
					["TreeNode"] = TreeNode,
					["PlayerLevel"] = Connection.m_Player:CppGetLevel()
				}
	local function CallBack(bool,Data)	
		local Player = Connection.m_Player
		if bool then
			Player:Lua_AddFightSkill(Data)
			--天赋学习刷新战斗力
			CGasFightingEvaluation.UpdateFightingEvaluationInfo(Player)
		else
			if Data ~= 0 then
				MsgToConn(Connection,Data)
			end
		end
	end
	CallAccountManualTrans(Connection.m_Account, FightSkillDB, "LuaDB_AddTalentSkill", CallBack, data)
end

--升级天赋技能和公共技能、学习公共技能
function Gac2Gas:UpdateFightSkill( Connection, name, level,ActionType)
	local Player = Connection.m_Player
	if not Player then
		return
	end
	--角色死亡,不可以学习技能
	if not Player:CppIsLive() then
		return
	end
	Player:OnSavePlayerSoulTickFunc(nil,Player)
	local data = { ["name"] = name, ["ActionType"] = ActionType, ["PlayerId"] = Player.m_uID ,["SkillLevel"] = level,
			["Class"] =  Player:CppGetClass(),["PlayerLevel"] = Player:CppGetLevel()}
	local function CallBack(bool,Data,questTbl)
		if bool then
			if Player:CppIsLive() then
				Player:AddSkill(Data.name,Data.current_level)
			else
				Player.AddSkillTbl = {}
				table.insert(Player.AddSkillTbl,{Data.name,Data.current_level})
			end
			Player.m_uSoulValue = Data.SoulRet + Player.m_uKillDropSoulCount
			Gas2Gac:ReturnModifyPlayerSoulNum(Connection,Player.m_uSoulValue)			
			Gas2Gac:ReturnLearnSkill(Connection,Data.name,Data.current_level)
			Gas2Gac:UpdateFightSkill(Connection,Data.name,Data.current_level,ActionType)
			--Renton,加完技能后的回调部分
			if questTbl ~= nil then
				for i = 1,#questTbl do
					local quest_playId = questTbl[i]["char_id"]
					local quest_questname = questTbl[i]["sQuestName"]
					local quest_questvarname = questTbl[i]["sVarName"]
					local quest_inum = questTbl[i]["iNum"]
					--db()
					local player = g_GetPlayerInfo(quest_playId)
					if player then
						Gas2Gac:RetAddQuestVar(player.m_Conn, quest_questname, quest_questvarname,quest_inum)
					end
				end
			end
			if ActionType == 0 then
				if Data["ShortcutInfo"] then
					local res = Data["ShortcutInfo"]
					local row = res:GetRowNum()
					if row > 0 then
						for n = 1, row do
							local Pos, Type, Arg1, Arg2, Arg3 = res(n,1),res(n,2),res(n,3),res(n,4),res(n,5)
							Gas2Gac:ReturnShortcut(Connection, Pos, Type, Arg1, Arg2, Arg3)
						end
					end
					Player:InitShortcutEnd(Player)
				end
			end
			---------------------------------
		else
			if Data ~= 0 then
				MsgToConn(Connection,Data)
				Gas2Gac:UpdateFightSkillError(Connection,name)
			end
		end
	end

	CallAccountManualTrans(Connection.m_Account, FightSkillDB, "Lua_UpdateFightSkill", CallBack, data)		
end

--【选择主修系】
function Gac2Gas:SetSeries( Connection,nSys)
	--判断是否在对应的npc旁边
--	if not DistanceJudgeBetweenNpcAndPlayer(Connection,"技能Npc")	then
--		return
--	end
	
	if not (nSys >= 1 and nSys <= 3) then
		return
	end
	
	--等级不够不能学习
	if Connection.m_Player:CppGetLevel() < tblSkillLearnLevel[1] then
		MsgToConn(Connection,1500)
		return
	end

	local data = { ["nSys"] = nSys,["nCharID"] = Connection.m_Player.m_uID,
					 ["ClassID"] = Connection.m_Player:CppGetClass(),
					 ["CampID"] = Connection.m_Player:CppGetBirthCamp()}
	
	local function CallBack(bFlag,Data)
		if not bFlag then
			if IsNumber(Data) then
				MsgToConn(Connection,Data)
			end
		else
			Gas2Gac:ReturnSeries(Connection,nSys)
			Gas2Gac:ReturnSkillNode(Connection,1)
			if Data.TalentSkillTbl["name"] then
				Connection.m_Player:Lua_AddFightSkill(Data["TalentSkillTbl"])
			end
			
			--若其他任务有完成转职的事件需求，加佣兵任务计数
			local QuestVarTbl = Data.QuestVarTbl
			if IsCppBound(Connection) and IsCppBound(Connection.m_Player) and QuestVarTbl and QuestVarTbl[Connection.m_Player.m_uID] then
				local res = QuestVarTbl[Connection.m_Player.m_uID][1]
				Gas2Gac:RetAddQuestVar(Connection, res[1],res[2],res[3])
			end
			
			--更新佣兵等级追踪窗口
			--local MLRes = Data.MLRes
			--UpdateMercenaryLevelTraceWnd(Connection, MLRes)
		end
	end
	CallAccountManualTrans(Connection.m_Account, FightSkillDB, "SetSeriesDB", CallBack, data)
end

--【加载所有技能相关信息】
function Gac2Gas:RequestSkillSeries(Connection)
	if Connection.m_Player == nil then
		return
	end
	local data = {["nCharID"] = Connection.m_Player.m_uID}
	local function CallBack(series,SkillRetTbl,tblNodes)
		Gas2Gac:ReturnSeries(Connection,series)
		for i =1,#tblNodes do
			Gas2Gac:ReturnSkillNode(Connection,tblNodes[i])
		end
		Gas2Gac:RetAddFightSkillStart(Connection)
		for i=1,#(SkillRetTbl) do
			local name = SkillRetTbl[i][1]
			local level = SkillRetTbl[i][2]
			local kind 	= SkillRetTbl[i][3]
			Gas2Gac:RetAllFightSkill( Connection, name, level, kind )			--Gac
		end
		Gas2Gac:RetAddFightSkillEnd(Connection)
	end
	CallAccountManualTrans(Connection.m_Account, FightSkillDB, "GetSeriesDB", CallBack, data)
end

--洗天赋
function CCharacterMediator:Lua_ClearAllGenius(skills)
	--角色死亡,不可以洗天赋
	if self.m_Conn.m_Player:CppIsLive() then
		self:ClearAllTalent()
		for i=1,#(skills) do
			local name = skills[i][1]
			local kind 	= skills[i][3]
			if kind == FightSkillKind.TalentSkill then --天赋技能要从角色身上清除
				self:RemoveSkill( name )
				self.m_FightSkillTbl[name] = nil
			end
		end
	else
		self.ClearAllTalentBool = true
	end

	Gas2Gac:ClearAllGenius(self.m_Conn)
end

function Gac2Gas:ClearAllGenius(Connection)
	--判断是否在对应的npc旁边
--	if not DistanceJudgeBetweenNpcAndPlayer(Connection,"技能Npc")	then
--		return
--	end
	if not Connection.m_Player then
		return
	end
	
	if not Connection.m_Player:CppIsLive() then
		MsgToConn(Connection,1604)
		return
	end
	if Connection.m_Player:IsInBattleState() then
		MsgToConn(Connection,1607)
		return
	end
	Connection.m_Player:OnSavePlayerSoulTickFunc(nil,Connection.m_Player)
	local data = {["PlayerId"] = Connection.m_Player.m_uID,
					["PlayerLevel"] =  Connection.m_Player:CppGetLevel()
				}
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then 
			Player:Lua_ClearAllGenius(Data["Skills"])
			--同步魂值
			Player.m_uSoulValue = Data["SoulRet"] + Player.m_uKillDropSoulCount
			Gas2Gac:ReturnModifyPlayerSoulNum(Connection,Player.m_uSoulValue)
			--洗天赋刷新战斗力
			CGasFightingEvaluation.UpdateFightingEvaluationInfo(Player)
		else
			MsgToConn(Connection,Data)
			return
		end
	end	
	
	CallAccountManualTrans(Connection.m_Account, FightSkillDB, "Lua_ClearAllGenius", CallBack, data)	
end

--得到普攻技能
function CCharacterMediator:Lua_LoadCommonlyFightSkill(skills)
	local function LoadCommonlyFightSkill()
		if skills == 0 then
			return
		end

		local name = skills[1]
		local level = skills[2]
		local kind 	= skills[3]
		self:AddSkill( name, level )
		Gas2Gac:ReCommonlySkill(self.m_Conn,name)
	end
	apcall(LoadCommonlyFightSkill)
end

--设置技能到角色身上
function CCharacterMediator:LoadAllFightSkill(skills)
	for i=1,#(skills) do
		local name = skills[i][1]
		local level = skills[i][2]
		local kind 	= skills[i][3]
		if kind == FightSkillKind.Skill 
			or kind == FightSkillKind.TalentSkill then
			self:AddSkill( name, level )
			Gas2Gac:ReturnLearnSkill(self.m_Conn, name,level)
		elseif kind == FightSkillKind.Talent then
			self:InsertTalent(name, 10)
		elseif kind == FightSkillKind.NonFightSkill then
			self:AddSkill( name, 0 )
			Gas2Gac:ReturnLearnSkill(self.m_Conn, name,level)
		end
	end
	local tblNames = SkillItem_Common:GetKeys()
	for i=1,#tblNames do
		local SkillName = tblNames[i]
		local skillItem = SkillItem_Common(SkillName)
		if SkillName ~= "" and (skillItem("SkillCoolDownType") == ESkillCDType.DrugItemCD
			 or skillItem("SkillCoolDownType") == ESkillCDType.SpecailItemCD) then
			self:AddSkill(SkillName, skillItem("SkillLevel") )
		end
	end
	self:AddNonFightSkill()
end

--发送技能信息到客户端
function CCharacterMediator:LoadAllFightSkillToGac(skills)
	for i=1,#(skills) do
		local name = skills[i][1]
		local level = skills[i][2]
		local kind 	= skills[i][3]
		if kind == FightSkillKind.Skill 
			or kind == FightSkillKind.TalentSkill
			or kind == FightSkillKind.NonFightSkill then
				Gas2Gac:ReturnLearnSkill(self.m_Conn, name,level)
		end
	end
end

function CCharacterMediator:AddNonFightSkill() 
--	for i,v in pairs(NonFightSkill_Common) do
--		if v.IsNoLearnSkill == 1 then
--			self:AddSkill(i,0)
--		end
--	end	
end

function CCharacterMediator:ReSendAllFightSkill(skills)
	for i=1,#(skills) do
		local name = skills[i][1]
		local level = skills[i][2]
		if kind == FightSkillKind.Skill then
			Gas2Gac:ReturnLearnSkill(self.m_Conn, name,level)
		end
	end
end


local function CheckAllCharAreBlankChar(content)
	local contentNotAllBlankChar = false
	local contentLen = string.len(content)
	for i=1, contentLen do
		local char = string.sub(content,i, i)
		if char ~= ' ' and char~= '\n' then
			contentNotAllBlankChar = true 
			break
		end
	end
	return contentNotAllBlankChar
end

--召唤兽名字设置
function Gac2Gas:RequestSetServantName(Connection, uServantID, sServantResName, sServantRealName)
	--sServantResName  召唤兽的资源名 ,应该要做配置表检测,看这个召唤兽是否是存在的!  记得补上  Haik
	if (sServantRealName == nil or sServantRealName == "" or not CheckAllCharAreBlankChar(sServantRealName) )then
		MsgToConn(Connection, 197001)
		return
	end
	local textFltMgr	= CTextFilterMgr:new()
	if( not textFltMgr:IsValidName(sServantRealName) ) then
		MsgToConn(Connection, 197005)
		return
	end
	if(string.len(sServantRealName) > 32 )then
		MsgToConn(Connection, 197002)
		return
	end
	local Player = Connection.m_Player
	Player:SetServantShowNameFromClient(Connection, uServantID, sServantResName, sServantRealName)
end

-----------------------------------角色组---------------------------
--测试子弹魔法
function Gac2Gas:StartBulletMagicTest(Conn)
	CTestBulletMagic_StartTest(Conn.m_Player:GetEntityID())
	--CTestShockWaveMagic_StartTest()
end

function Gac2Gas:ShutDownBulletMagicTest(Conn)
	CTestBulletMagic_ShutDownTest()
	--CTestShockWaveMagic_ShutDownTest()
end
