cfg_load "scene/SceneMapDifCamp_Common"
gac_gas_require "scene_mgr/SceneCfg"
cfg_load "skill/SkillPart_Common"
cfg_load "skill/NonFightSkill_Common"
gac_gas_require "framework/text_filter_mgr/TextFilterMgr"
gac_gas_require "skill/SkillCommon"
cfg_load "skill/InitSkill_Common"
cfg_load "fb_game/RecruitAward_Common"
cfg_load "server/ServerTypeAward_Common"

local LogErr = LogErr
local GetCharCount= GetCharCount
local GasCreateRole = class()
local textFilter = CTextFilterMgr:new()
local Scene_Common = Scene_Common
local SkillPart_Common = SkillPart_Common
local ECharSex = ECharSex
local EEquipPart = EEquipPart
local SceneMapDifCamp_Common = SceneMapDifCamp_Common
local g_ItemInfoMgr = CItemInfoMgr:new()
local FightSkillKind = FightSkillKind
local EShortcutPieceState = EShortcutPieceState
local MemH64 = MemH64
local NonFightSkill_Common = NonFightSkill_Common
local loadstring = loadstring
local GetSkillLevel = GetSkillLevel
local InitSkill_Common = InitSkill_Common
local event_type_tbl = event_type_tbl
local RecruitAward_Common = RecruitAward_Common
local GetCfgTransformValue = GetCfgTransformValue
local ServerTypeAward_Common = ServerTypeAward_Common

local GasCreateRoleSql = CreateDbBox(...)

-- 阵营与出生地图对应表  yy
local CampToScene = 
{
	[1] = 1,						--轻歌林地
	[2] = 2,						--树屋酒吧
	[3] = 3							--海帕斯高原
}


---------------------------------------------------------------------------------------
--计算删除的表中的角色数量
local  StmtDef=
{
 "CountDname",
 "select count(all cd_sName) from tbl_char_deleted"
}
DefineSql(StmtDef,GasCreateRole)

-- 插入角色人物基本属性
--	nUsID, nHair,nHairColor,nFace, nSex,nClass,m_nInherence
local StmtDef=
{
	"InsertCharStatic",
	[[
	insert ignore into tbl_char_static set
	cs_uId=?,
	us_uId=?,
	cs_sHair=?,
	cs_sHairColor=?,
	cs_sFace=?,
	cs_uScale=?,
	cs_uSex=?,
	cs_uClass=?,
	cs_uCamp=?,
	cs_dtCreateDate	= from_unixtime( ? )
	]]
}
DefineSql(StmtDef,GasCreateRole)


--用于本地验证
local StmtDef=
{
	"InsertCharStaticLocal",
	[[
	insert into tbl_char_static set
	us_uId=?,
	cs_sHair=?,
	cs_sHairColor=?,
	cs_sFace=?,
	cs_uScale=?,
	cs_uSex=?,
	cs_uClass=?,
	cs_uCamp=?,
	cs_dtCreateDate	= from_unixtime( ? )
	]]
}
DefineSql(StmtDef,GasCreateRole)

--查询是否和已删除角色重名的情况
local StmtDef = {
    "SelectDelCharInfo",
    [[
        select count(*) from tbl_char_deleted as del, tbl_char_static as static where del.cd_sName = ? and static.us_uId 
        != ? and del.cs_uId = static.cs_uId ;
    ]]
}
DefineSql(StmtDef, GasCreateRole)

-- 插入角色的名称属性
local StmtDef=
{
	"InsertChar",
	"insert ignore into tbl_char set cs_uId=?,c_sName=?"
}
DefineSql(StmtDef,GasCreateRole)


-- 插入角色等级信息
local StmtDef=
{
	"InsertCharBasic",
	"insert into tbl_char_basic (cs_uId) values(?)"
}
DefineSql(StmtDef,GasCreateRole)

-- 插入角色等级信息
local StmtDef=
{
	"InsertCharMoneyInfo",
	"insert into tbl_char_money(cs_uId) values(?)"
}
DefineSql(StmtDef,GasCreateRole)

--插入角色魂值信息
local StmtDef=
{
	"InsertCharSoul",
	"insert into tbl_char_soul (cs_uId,cs_uSoulNum,cs_uBurstSoulTimes) values(?,?,?)"
}
DefineSql(StmtDef,GasCreateRole)

--插入角色战斗信息
local StmtDef=
{
	"InsertCharFightInfo",
	"insert into tbl_char_fight_info (cs_uId,cfi_uDeadTimes,cfi_uKillNpcCount,cfi_uKillPlayerCount,cfi_uPlayerOnTimeTotal) values(?,?,?,?,?)"
}
DefineSql(StmtDef,GasCreateRole)

-- 插入角色位置信息
local StmtDef=
{
	"InsertCharPosition",
	"insert into tbl_char_position (cs_uId,sc_uId,cp_uPosX,cp_uPosY) values(?,?,?,?)"
}
DefineSql(StmtDef,GasCreateRole)

local StmtDef=
{
	"InsertCharFbPosition",
	"insert into tbl_char_fb_position (cs_uId,sc_uId,cfp_uPosX,cfp_uPosY) values(?,?,?,?)"
}
DefineSql(StmtDef,GasCreateRole)

-- 插入角色经验信息
local StmtDef=
{
	"InsertCharExperience",
	"insert into tbl_char_experience (cs_uId,cs_uLevelExp) values(?,?)"
}
DefineSql(StmtDef,GasCreateRole)

-- 插入角色佣兵积分信息
local StmtDef=
{
	"InsertCharIntegral",
	"insert into tbl_char_integral (cs_uId,cs_uLevelIntegral) values(?,?)"
}
DefineSql(StmtDef,GasCreateRole)

-- 插入角色血蓝信息
local StmtDef=
{
	"InsertCharStatus",
	"insert into tbl_char_status (cs_uId,cs_uCurHP,cs_uCurMP) values(?,?,?)"
}
DefineSql(StmtDef,GasCreateRole)

local StmtDef =
{
	"SelectUid",
	"select us_uId from tbl_user_static where us_sName=? and us_uGameID = ?"
}
DefineSql(StmtDef,GasCreateRole)

local function GasCheckRoleName(strRoleName,userName,game_id)
    local bType,strRet = textFilter:CheckRoleName(strRoleName)
    if bType == false then
        return false, strRet    
    end
    
    local lGMDBExecutor = RequireDbBox("GMDB")
	local nGMLevel = lGMDBExecutor.GetGmLevel({["user_name"] = userName,["game_id"] = game_id})
    if( not nGMLevel ) then
        if textFilter:IsValidName(strRoleName) == false then
            return false, 29    --非法角色名
        end
    end
end

-- 查询角色总数目
local StmtDef= 
{
	"_SelectCharCount",
	[[ 
		select count(*) from tbl_char
	]]
}
DefineSql(StmtDef,GasCreateRole)

local StmtDef= 
{
	"_SelectCharCountByCamp",
	[[ 
		select count(*) from tbl_char c , tbl_char_static s where s.cs_uCamp = ? and c.cs_uId = s.cs_uId
	]]
}
DefineSql(StmtDef,GasCreateRole)

local StmtDef= 
{
	"_SelectCharCountByUserID",
	[[ 
		select count(*) from tbl_char c , tbl_char_static s where s.us_uId = ? and c.cs_uId = s.cs_uId
	]]
}
DefineSql(StmtDef,GasCreateRole)


local StmtDef = 
{
    "_SelectRushRegiAccount",
    [[
        select sc_sVarValue from tbl_conf_server where sc_sVarName = ?;
    
    ]]
}DefineSql(StmtDef, GasCreateRole)

function GasCreateRoleSql.GetServerRushRegiAccountFlag()
	local rushRegiAccountFlagRet = GasCreateRole._SelectRushRegiAccount:ExecStat("RushRegisterAccountFlag")
	local rushRegiAccount = 0
    if rushRegiAccountFlagRet:GetRowNum() > 0 then  
        rushRegiAccount = tonumber(rushRegiAccountFlagRet:GetData(0, 0))
    else
        LogErr("未定义服务器关于是否抢注角色信息")
    end
    return rushRegiAccount
end

local function CreateItemAndPutIntoBag(nCharID, SceneName, itemType, itemName, itemCount, eventType)
	local itemIdTbl = {}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local params= {}
	params.m_nType			= itemType
	params.m_sName			= itemName
	params.m_nBindingType	= g_ItemInfoMgr:GetItemInfo( itemType, itemName,"BindingStyle" ) or 0
	params.m_nCharID		= nCharID
	params.m_sCreateType	= eventType
	for i = 1, itemCount do
		local item_id = g_RoomMgr.CreateItem(params)
		if item_id then
			table.insert(itemIdTbl,{item_id})
		end
	end
	
	local param = {}
	param.m_nCharID		= nCharID
	param.m_nItemType	= itemType
	param.m_sItemName	= itemName
	param.m_tblItemID	= itemIdTbl
	local putitemtobag	= RequireDbBox("CPutItemToBagDB")
	
	return putitemtobag.AddCountItem(param)
end

function GasCreateRoleSql.CanCreateRoleAtRushRegisterActivity(userID, camp)        
    local rushRegiAccountFlag = GasCreateRoleSql.GetServerRushRegiAccountFlag()
    if rushRegiAccountFlag == 0 then
        return true,rushRegiAccountFlag
    end
    
    local MaxRushRoleNumRet = GasCreateRole._SelectRushRegiAccount:ExecStat("MaxRushRoleNumPerServer")
    local MaxRushRoleNum =tonumber(MaxRushRoleNumRet:GetData(0, 0))
    
    local MaxRushRoleNumEveryCampRet = GasCreateRole._SelectRushRegiAccount:ExecStat("MaxRushRoleNumPerCamp")
    local MaxRushRoleNumEveryCamp = tonumber(MaxRushRoleNumEveryCampRet:GetData(0, 0))
    
    local MaxRushRoleNumPerAccountRet = GasCreateRole._SelectRushRegiAccount:ExecStat("MaxRushRoleNumPerAccount")
    local MaxRushRoleNumPerAccount = tonumber(MaxRushRoleNumPerAccountRet:GetData(0, 0))
    
    local roleCountRet = GasCreateRole._SelectCharCount:ExecStat()
    local roleCount = roleCountRet:GetData(0,0) 

    if roleCount >= MaxRushRoleNum then     --检测总角色数目
        return false,  190001 
    end

    local roleCountByCampRet = GasCreateRole._SelectCharCountByCamp:ExecStat(camp) 
    local roleCountByCamp = roleCountByCampRet:GetData(0,0)
    if roleCountByCamp >= MaxRushRoleNumEveryCamp then  --检测每个阵营的角色数目
        return false, 190002 
    end
      
    local roleCountByUserRet = GasCreateRole._SelectCharCountByUserID:ExecStat(userID) 
    local roleCountByUser = roleCountByUserRet:GetData(0,0)
    if roleCountByUser >= MaxRushRoleNumPerAccount then  --检测每个阵营的角色数目
        return false, 190003 
    end  
    
    return true, rushRegiAccountFlag
end

function GasCreateRoleSql.CreateRole(data)
	local role_id = data["role_id"]
	local strName = data["strName"]
	local strUserName = data["strUserName"]
	local game_id = data["game_id"]
	local nHair = data["nHair"]
	local nHairColor = data["nHairColor"]
	local nFace = data["nFace"]
	local nScale = data["nScale"]
	local nSex = data["nSex"]
	local nClass = data["nClass"]
	local HP = data["HP"]
	local MP = data["MP"]
	local time = data["time"]
	local nCamp = data["nCamp"]
	local xPos = data["xPos"]
	local yPos = data["yPos"]
	local recruitFlag = data["recruitFlag"]
	local serverType = tonumber(data["serverType"])
	
	local bType,strRet = GasCheckRoleName(strName,strUserName,game_id)
	if bType == false then
		return {false,strRet}
	end
	
	--骑士职业暂时不允许被创建
	if nClass == 3 then 
	    return {false,10006}
	end
	
	local SceneName = SceneMapDifCamp_Common(nCamp, "SceneMap")
	local g_LogMgr = RequireDbBox("LogMgrDB")
	--根据角色名返回角色id，判断该名字是否已经存在过
	--根据通行证名获得user id
	local res= GasCreateRole.SelectUid:ExecStat(strUserName,game_id)
	local nUserID = res:GetData(0,0)
	
	assert(IsNumber(nUserID))
	
	local rushRegiAccountFlag = 0
    local suc, msgID = GasCreateRoleSql.CanCreateRoleAtRushRegisterActivity(nUserID, nCamp)
    if suc == false then
        return {false, msgID}
    end
    rushRegiAccountFlag = msgID

    --检查是否已经在已删除角色中出现过
    local  res2 = GasCreateRole.SelectDelCharInfo:ExecSql("n", strName, nUserID) 
    local count1= res2:GetData(0,0)
    if count1 > 0 then
        CancelTran()
		return {false, 10002}
    end
    

   	-- 插入角色人物基本属性
	if type(role_id)=="number" and role_id > 0 then
		res = GasCreateRole.InsertCharStatic:ExecSql( "",role_id,nUserID,nHair,nHairColor,nFace,nScale,nSex,nClass,nCamp,time)
	else
		res = GasCreateRole.InsertCharStaticLocal:ExecSql( "",nUserID,nHair,nHairColor,nFace,nScale,nSex,nClass,nCamp,time)
	end
	
	if g_DbChannelMgr:LastAffectedRowNum() ~= 1 then
		CancelTran()
		return {false, 10002}
	end
	-- 获得角色id
	local uCharID = g_DbChannelMgr:LastInsertId()
	
	g_LogMgr.CopyCharInfo(uCharID,nUserID,nHair,nHairColor,nFace,nScale,nSex,nClass,nCamp)
	if g_DbChannelMgr:LastAffectedRowNum() ~= 1 then
		CancelTran()
		return {false, 10005}
	end
	
	-- 插入角色人物名称属性
	GasCreateRole.InsertChar:ExecSql( "", uCharID, strName )
	if g_DbChannelMgr:LastAffectedRowNum() ~= 1 then
		CancelTran()
		return {false, 10002}
	end
	
	--给log备份角色列表
	g_LogMgr.CopyCharList(uCharID, strName )
	if g_DbChannelMgr:LastAffectedRowNum() ~= 1 then
		CancelTran()
		return {false, 10005}
	end
	-- 插入角色装备信息
	
	-- 插入角色等级信息
	
	GasCreateRole.InsertCharBasic:ExecSql("",uCharID)
	--初始化角色金钱信息
	GasCreateRole.InsertCharMoneyInfo:ExecStat(uCharID)
	
	-- 插入角色场景位置信息
--	local SceneID = CampToScene[nCamp]--yy
	local SceneMgrDB = RequireDbBox("SceneMgrDB")	
	local sceneID = SceneMgrDB._GetNormalSceneIdByName(SceneName)
	GasCreateRole.InsertCharPosition:ExecSql("",uCharID, sceneID, xPos, yPos)
	
	-- 插入角色副本位置信息(初始状态无副本信息 所以 sc_uId = 0)
	--GasCreateRole.InsertCharFbPosition:ExecSql("",uCharID,0,0,0)  
	-- 插入角色经验信息
	GasCreateRole.InsertCharExperience:ExecSql("",uCharID,0)
	-- 插入角色佣兵积分信息
	GasCreateRole.InsertCharIntegral:ExecSql("",uCharID,0)
	-- 插入角色血蓝信息
	GasCreateRole.InsertCharStatus:ExecSql("",uCharID,HP,MP)
	--插入玩家完成任务数量
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	RoleQuestDB.InsertCharFinishQuestNum(uCharID,0)
	--大逃杀
	local DaTaoShaDB = RequireDbBox("DaTaoShaFbDB")
	DaTaoShaDB.insertDaTaoShaPoint(uCharID,0,0,0,0)
	--插入积分赛的总分
	local JiFenSaiDB = RequireDbBox("JiFenSaiFbDB")
	JiFenSaiDB.insertJiFenSaiPoint(uCharID,2)--3V3
	--插入佣兵训练活动历史活动次数
	local ActivityCountDB = RequireDbBox("ActivityCountDB")
	ActivityCountDB.InsertActivityHistoryCount(uCharID, "佣兵训练活动", 0, 0)
	--插入区域副本积分信息
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	AreaFbPointDB.InsertPoint(uCharID)
	-- 插入装备
	GasCreateRoleSql.InitEquip(uCharID,nClass,nSex,SceneName)
	local succ,info = GasCreateRoleSql.InitSkill(uCharID,nClass,SceneName)
	--插入非战斗数据初始值
	local CharacterMediatorDB = RequireDbBox("CharacterMediatorDB")
	CharacterMediatorDB.InitNonCombatValue(uCharID, 400, 10000)
	local AreaDB = RequireDbBox("AreaDB")
	AreaDB.NewRoleAddPlayerAreaPlace(uCharID)
	
	--加入默认生活技能
	local LiveSkillDB = RequireDbBox("LiveSkillBaseDB")
	LiveSkillDB.AddDefaultSkills(uCharID)
	
	--插入角色魂值信息 
	GasCreateRole.InsertCharSoul:ExecSql("",uCharID,0,0)
	--插入角色战斗信息
	GasCreateRole.InsertCharFightInfo:ExecSql("",uCharID,0,0,0,0)
	--给角色初始化好友组和黑名单
	local FriendsDB = RequireDbBox("FriendsDB")
	FriendsDB.InitAddFriendGoodClassAndBlackClass(uCharID)

	if nClass == 2 then
		local AureMagicDataTbl = {}
		table.insert(AureMagicDataTbl,{"闪电武器姿态", 1,"闪电武器",5})
		local param = {["char_id"] = uCharID,["AureMagicData"] = AureMagicDataTbl}
		CharacterMediatorDB.SaveAureMagicToDB(param)
	elseif nClass == 9 then
		local AureMagicDataTbl = {}
		table.insert(AureMagicDataTbl,{"黑豹姿态", 1,"黑豹姿态",0})
		local param = {["char_id"] = uCharID,["AureMagicData"] = AureMagicDataTbl}
		CharacterMediatorDB.SaveAureMagicToDB(param)
	end
	local tbl = {succ,info}
	tbl["RushRegiAccountFlag"] = rushRegiAccountFlag
	local GasRecruitSql = RequireDbBox("RecruitPlayerDB")
	local CGasRoomDbBox = RequireDbBox("GasRoomMgrDB")
	
	if recruitFlag ~= 0 then
		local itemInfo = {}
		if nCamp == 1 then
			itemInfo = GetCfgTransformValue(false, "RecruitAward_Common", recruitFlag, "AwardBoxCamp1") 
		elseif nCamp == 2 then
			itemInfo = GetCfgTransformValue(false, "RecruitAward_Common", recruitFlag, "AwardBoxCamp2") 
		else
			itemInfo = GetCfgTransformValue(false, "RecruitAward_Common", recruitFlag, "AwardBoxCamp3") 
		end
		
		local itemType, itemName, itemNum = itemInfo[1], itemInfo[2],itemInfo[3]
		local eventType = event_type_tbl["重金招募奖励"]
		CreateItemAndPutIntoBag(uCharID, nil, itemType, itemName, itemNum, eventType)
		GasRecruitSql.AddRecruitCharInfo(uCharID, recruitFlag)
	end
	
	if serverType ~= 0 then
		local itemInfo = GetCfgTransformValue(false, "ServerTypeAward_Common", serverType,"AwardItem") 
		local itemType, itemName, itemNum = itemInfo[1], itemInfo[2],itemInfo[3]
		local eventType = event_type_tbl["服务器星级奖励"]
		CreateItemAndPutIntoBag(uCharID, nil, itemType, itemName, itemNum, eventType)
	end
	
	return {true, uCharID, tbl}
end
--------------------------------------------------------------------------------------------
--@brief 门派初始装备构造
function GasCreateRoleSql.InitEquip(nCharID,nClass,nSex,SceneName)
	local tblFirstEquip = g_ItemInfoMgr:GetNoBigIdTable(g_ItemInfoMgr:GetInitEquipID(),nClass)
	local nBigID,nIndex,eEquipPart = 0,0,0
	for n=1, 7 do
		if n == 1 then
			nBigID = g_ItemInfoMgr:GetStaticArmorBigID() 
			if nSex == ECharSex.eCS_Male then
				nIndex = tblFirstEquip("Wear") 
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanWear")
			end
			eEquipPart = EEquipPart.eWear
		elseif n == 2 then
			nBigID = g_ItemInfoMgr:GetStaticWeaponBigID() 
			if nSex == ECharSex.eCS_Male then
				nIndex = tblFirstEquip("Weapon")
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanWeapon")
			end
			eEquipPart = EEquipPart.eWeapon
		elseif n == 3 then
			nBigID = g_ItemInfoMgr:GetStaticWeaponBigID()
			if nSex == ECharSex.eCS_Male then 
				nIndex = tblFirstEquip("DWeapon")
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanDweapon")
			end
			eEquipPart = EEquipPart.eAssociateWeapon
		elseif n == 4 then
			nBigID = g_ItemInfoMgr:GetStaticShieldBigID()
			if nSex == ECharSex.eCS_Male then 
				nIndex = tblFirstEquip("Shield")
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanShield")
			end
			eEquipPart = EEquipPart.eAssociateWeapon
		elseif n == 5 then
			nBigID = g_ItemInfoMgr:GetStaticArmorBigID() 
			if nSex == ECharSex.eCS_Male then
				nIndex = tblFirstEquip("Shoe")
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanShoe")
			end
			eEquipPart = EEquipPart.eShoe
		elseif n == 6 then
			nBigID = g_ItemInfoMgr:GetStaticArmorBigID() 
			if nSex == ECharSex.eCS_Male then
				nIndex = tblFirstEquip("Shoulder")
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanShoulder")
			end
			eEquipPart = EEquipPart.eShoulder
		elseif n == 7 then
			nBigID = g_ItemInfoMgr:GetStaticArmorBigID() 
			if nSex == ECharSex.eCS_Male then
				nIndex = tblFirstEquip("Hand")
			elseif nSex == ECharSex.eCS_Female then
				nIndex = tblFirstEquip("WomanHand")
			end
			eEquipPart = EEquipPart.eHand
		end
		
		if g_ItemInfoMgr:CheckType(nBigID,nIndex) then
			local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
			local params= {}
			params.m_nType = nBigID
			params.m_sName =nIndex
			params.m_nBindingType = g_ItemInfoMgr:GetItemInfo( nBigID,nIndex,"BindingStyle" ) or 0
			params.m_nCharID =nCharID
			params.m_sCreateType = event_type_tbl["角色初始化产生装备"]
			local nID = g_RoomMgr.CreateItem(params)
			if not nID then
				LogErr("装备初始化出错!","nCharID:" .. nCharID .. "nID:" .. nID .. "eEquipPart:" .. eEquipPart)
			end
			if g_ItemInfoMgr:IsStaticWeapon(nBigID) and eEquipPart == EEquipPart.eWeapon then
				local CommonlySkill =g_ItemInfoMgr:GetCommonlySkill(nClass,nBigID,nIndex)
				local FightSkillDB = RequireDbBox("FightSkillDB")
				FightSkillDB.Dbs_SaveFightSkill(nCharID, CommonlySkill, FightSkillKind.NormSkill )
				local ShortCutDB = RequireDbBox("ShortcutDB")
				ShortCutDB.Player_SaveShortcut(nCharID, 1, 2, CommonlySkill, 1, 1)
			end
			local EquipMgrDB = RequireDbBox("EquipMgrDB")
			EquipMgrDB.ReplaceEquip(nCharID,nID,eEquipPart)
		end
	end
end




function GasCreateRoleSql.InitSkill(nCharID,nClass,SceneName)
	local tblFirstSkill = InitSkill_Common(tostring(nClass),"1")
	local ShortCutDB = RequireDbBox("ShortcutDB")
	local FightSkillDB = RequireDbBox("FightSkillDB")
	
	local succ, info
	for i = 2, 10 do
		local skillInfo	= tblFirstSkill("SkillName" .. i)
		if( skillInfo and "" ~= skillInfo ) then
			local isItem	= string.find( skillInfo, "," )
			local nMark2	= isItem and string.find( skillInfo, ",", isItem + 1 ) or 0
			local nCount	= isItem and tonumber( string.sub( skillInfo, nMark2 + 1 ) )
			local name		= isItem and string.sub( skillInfo, isItem + 1, nMark2 - 1 ) or skillInfo
			local type		= isItem and EShortcutPieceState.eSPS_Item or EShortcutPieceState.eSPS_Skill
			local arg2		= isItem and tonumber( string.sub( skillInfo, 1, isItem - 1 ) ) or 1
			local arg3		= math.floor( (i - 1)/10 ) + 1
			local pos		= (i - 1)%10 + 1
			
			if( isItem ) then
				local eventType = event_type_tbl["角色初始化携带技能物品"]
				succ, info = CreateItemAndPutIntoBag(nCharID, SceneName, arg2, name, nCount,eventType)
			else
				local isSkill = SkillPart_Common(name)
				if( isSkill ) then
					arg2 = GetSkillLevel( name, 1 )
				end
					
				if( isSkill or NonFightSkill_Common(name) ) then
					local nSkillLevel	= isSkill and GetSkillLevel(name, 1) or 1
					local nSkillKind	= isSkill and FightSkillKind.Skill or FightSkillKind.NonFightSkill
					FightSkillDB.Dbs_SaveFightSkill(nCharID, name, nSkillKind )
					FightSkillDB.Dbs_UpgradeFightSkill(nSkillLevel, nCharID, name, nSkillKind)
				end
			end
			ShortCutDB.Player_SaveShortcut(nCharID, pos, type, name, arg2, arg3)
		end
	end
	
	return succ, info
end

function GasCreateRoleSql.GetTotalChar()
	local roleCountRet = GasCreateRole._SelectCharCount:ExecStat()
  local roleCount = roleCountRet:GetData(0,0) 
  return roleCount
end

function GasCreateRoleSql.GetCharCountByCamp(camp)
	local roleCountByCampRet = GasCreateRole._SelectCharCountByCamp:ExecStat(camp) 
	local roleCountByCamp = roleCountByCampRet:GetData(0,0)
	return roleCountByCamp
end

---------------------------------------------------------------------------------------
return GasCreateRoleSql
