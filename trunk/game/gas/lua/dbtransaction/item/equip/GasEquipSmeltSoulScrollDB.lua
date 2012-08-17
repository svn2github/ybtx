cfg_load "equip/upgrade/EquipUpgrade_Common"
cfg_load "item/EquipSmeltSoulScroll_Common"
cfg_load "equip/EquipSmeltSoulValue_Common"

local loadstring = loadstring
local g_ItemInfoMgr = g_ItemInfoMgr
local EquipUpgrade_Common = EquipUpgrade_Common
local EquipSmeltSoulScroll_Common = EquipSmeltSoulScroll_Common
local EquipSmeltSoulScrollSql = class()
local event_type_tbl = event_type_tbl
local g_ItemInfoMgr = g_ItemInfoMgr
local EquipSmeltSoulValue_Common = EquipSmeltSoulValue_Common

local GasEquipSmeltSoulScrollDB = CreateDbBox(...)
local FromPlace ={}
FromPlace.Bag =1			--从包裹中取得的
FromPlace.StatusWnd =2		--从主角属性面板取得的
local EquipSmeltSoulScrollTbl = {}
------------------------------------------------------------------------------------------------------	
function GasEquipSmeltSoulScrollDB.InitEquipSmeltSoulScrollTbl()
	local keys = EquipSmeltSoulScroll_Common:GetKeys()
	for i=1,#keys do
		local key = keys[i]
		EquipSmeltSoulScrollTbl[key] = {}
		local EquipSmeltSoulScrollStr = EquipSmeltSoulScroll_Common(key,"EquipPart") --护甲片的部位
		local EquipScrollTbl = loadstring( "return " .. EquipSmeltSoulScrollStr)()
		for j = 1,#EquipScrollTbl do
			EquipSmeltSoulScrollTbl[key][EquipScrollTbl[j]] = true
		end
	end
end
--require文件时，将配置表需要loadstring的内容存储到表中，避免多次loadstring
GasEquipSmeltSoulScrollDB.InitEquipSmeltSoulScrollTbl()
------------------------------------------------------------------------------------------------------
--验证从客户端传过来的装备ID是否在正确
local StmtDef = {
	"_GetEquipPartByEquipId",
	[[ select ce_uEquipPart from tbl_item_equip where is_uId = ? and cs_uId =? ]]
}
DefineSql(StmtDef, EquipSmeltSoulScrollSql)

local StmtDef = {
	"_GetEquipIntensifyPhase",
	[[ select ifnull(iei_uIntensifyPhase,0) from tbl_item_equip_intensify where is_uId = ? ]]
}
DefineSql(StmtDef, EquipSmeltSoulScrollSql)

--@brief 装备炼化魂
--@param 炼化哪儿的装备1:包裹,2主角属性面板
--总魂值 = 按等级和颜色读表的值*(1+已经当前阶段*0.4)+EquipUpgrade_Common(level,"VanishSoulCount")* intensifyPhase *0.5
local function GetSoulPealFromEquip(data)
	local uEquipFromPlace  	= data["fromPlaceType"]
	local uEquipId			= data["EquipID"]
	local Class    			= data["Class"]
	local uCharId			= data["CharID"] 
	local eEquipPart = data["eEquipPart"]
	local sceneName = data["sceneName"]
	local binding = data["binding"]
	assert(IsNumber(uEquipFromPlace))
	assert(IsNumber(uEquipId))
	assert(IsNumber(uCharId))
	
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local itemType, itemName, equipRet = EquipMgrDB.GetEquipBaseInfo( uEquipId )
	if itemType == nil or itemName == nil or equipRet == nil then
		return false, "信息不合法！"
	end
	local intensifyPhaseRet = EquipSmeltSoulScrollSql._GetEquipIntensifyPhase:ExecStat(uEquipId)
	local intensifyPhase = 0
	if intensifyPhaseRet:GetRowNum() > 0 then
		intensifyPhase = intensifyPhaseRet:GetData(0,0)
	end
	local curLevel = equipRet:GetData(0, 2)
	--计算装备可以炼化成魂珠的魂值
	local uQuality = g_ItemInfoMgr:GetItemInfo( itemType, itemName,"Quality" )
	local QualityName = "Quality" .. uQuality

	local uAddCount =	EquipSmeltSoulValue_Common(curLevel,QualityName) 
	if uAddCount == 0 then
		return false,194008
	end
	
	local equipAdvancePhase = EquipMgrDB.GetEquipAdvancePhaseByID(uEquipId)
	local equipAdvanceValue = uAddCount*(1 + equipAdvancePhase * 0.4)
	local soulValues = EquipUpgrade_Common(curLevel,"VanishSoulCount") * intensifyPhase * 0.5
	soulValues = math.floor(soulValues + equipAdvanceValue)
	local soulpearlName = "通用魂珠:"
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	--得到魂珠的type
	local soulpearlType = g_ItemInfoMgr:GetSoulPearlBigID()
	--在静态物品表item_static中插入魂珠物品信息
	local params= {}
	params.m_nType = soulpearlType
	params.m_sName =  soulpearlName .. soulValues
	binding = 0

	params.m_nBindingType = binding
	params.m_nCharID = uCharId
	params.m_sCreateType = event_type_tbl["装备炼化魂珠"]
	local create_soulpearl_id = g_RoomMgr.CreateItem(params)
	if create_soulpearl_id == false then
		return false
	end
	local PearlName = "通用魂珠"
	--创建背包的闭包
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local nRoomIndex,nPos,bool,equipData
	
	local params = {}
	params.m_nCharID = uCharId
	params.m_nItemType = soulpearlType
	params.m_sItemName = PearlName
	params.m_tblItemID = {{create_soulpearl_id}}
	if uEquipFromPlace == FromPlace.Bag then				--从包裹数据库表中删除装备
		local posRet = g_RoomMgr.GetRoomIndexAndPosByItemId(uCharId, uEquipId)
		if posRet == nil then
			return false
		end
		nRoomIndex, nPos= posRet(1,1), posRet(1,2)
		if g_RoomMgr.DelItemByID(uCharId, uEquipId,event_type_tbl["装备炼化"]) == false then  --删除数据库中的盒子物品
			CancelTran()
			return false
		end
	elseif uEquipFromPlace == FromPlace.StatusWnd then	--从装备数据库表中删除装备
		local delEquipNeedData = {["EquipPart"] = eEquipPart, ["PlayerId"] = uCharId,["Class"] = Class ,["sceneName"] = data["sceneName"]}
		local entry = RequireDbBox("GasUseItemDB")
		bool,equipData = entry.DelEquip(delEquipNeedData)
		if not bool then
			CancelTran()
			return false
		end
	end
	--将创建的魂珠放进背包
	local	bFlag, res = item_bag_box.AddCountItem(params)
	if bFlag == false then
		CancelTran()
		return false
	end
	local result = {["CreatePearlID"]=create_soulpearl_id, ["PearlType"]=soulpearlType, ["PearlName"]= PearlName,["PearlInfo"] = {soulValues, RootType,binding},
					["PearlRoomIndex"] =res[1].m_nRoomIndex, ["PearlPos"]= res[1].m_nPos, ["EquipRoomIndex"] =nRoomIndex, ["EquipPos"] = nPos, ["EquipData"] =equipData}
	
	return true, result
end

--@brief 判断该装备是否可以炼化
local function JudgeEquipCanSmeltSoul(bigID,SmeltSoulScrollName,equipPart)
	local b_ifSmeltSoul,errorId
	--判断是不是防具
	if g_ItemInfoMgr:IsArmor(bigID) then
		b_ifSmeltSoul = EquipSmeltSoulScrollTbl[SmeltSoulScrollName]["防具"] or EquipSmeltSoulScrollTbl[SmeltSoulScrollName][equipPart]
	--判断是不是饰品
	elseif g_ItemInfoMgr:IsStaticJewelry(bigID) then
		b_ifSmeltSoul = ( equipPart == "项链" or  equipPart == "佩饰" ) and EquipSmeltSoulScrollTbl[SmeltSoulScrollName]["饰品"] 
	--判断是不是武器
	elseif g_ItemInfoMgr:IsWeapon(bigID) then
		b_ifSmeltSoul = EquipSmeltSoulScrollTbl[SmeltSoulScrollName]["武器"] or EquipSmeltSoulScrollTbl[SmeltSoulScrollName][equipPart]
	--判断是否为盾牌
	elseif g_ItemInfoMgr:IsStaticShield(bigID) then
		b_ifSmeltSoul = EquipSmeltSoulScrollTbl[SmeltSoulScrollName]["副手"]
	--判断是不是戒指
	elseif g_ItemInfoMgr:IsRing(bigID) then
		b_ifSmeltSoul = EquipSmeltSoulScrollTbl[SmeltSoulScrollName]["戒指"]
	end
	if not b_ifSmeltSoul then
		errorId = 194003
	end
	return b_ifSmeltSoul,errorId
end

--@brief 装备炼魂卷轴设定
function GasEquipSmeltSoulScrollDB.EquipSmeltSoul(data)		
	local SmeltSoulScrollRoomIndex = data["SmeltSoulScrollRoomIndex"]
	local SmeltSoulScrollPos = data["SmeltSoulScrollPos"]
	local equipRoomIndex = data["equipRoomIndex"]
	local equipPos = data["equipPos"]
	local equipitemid = data["equipitemid"]
	local uCharId = data["uCharId"]
	local bigID = data["nBigID"]
	local nIndex = data["nIndex"]
	local itemType = data["itemType"]
	local SmeltSoulScrollName = data["SmeltSoulScrollName"]
	local equip_id = {}
	local equipId = 0
	local equip_part = 0
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local smeltSoulScrollId = 0
	local flag
	local equipPart = g_ItemInfoMgr:GetItemInfo(bigID,nIndex,"EquipPart")
	
	local sItemType, sItemName, count = g_RoomMgr.GetTypeCountByPosition(uCharId, SmeltSoulScrollRoomIndex, SmeltSoulScrollPos)
	if sItemType == nil or sItemName == nil or count == 0 or sItemType ~= 36 then
		return false
	end
		 
	local SmeltSoulScrollSet = g_RoomMgr.GetAllItemFromPosition(uCharId, SmeltSoulScrollRoomIndex, SmeltSoulScrollPos)
	if SmeltSoulScrollSet:GetRowNum() > 0 then
		smeltSoulScrollId = SmeltSoulScrollSet(1,1)
	else
		return false
	end
	local sItemType, sItemName, count,binding
	if equipRoomIndex ~= 0 and equipPos ~= 0 then
		sItemType, sItemName, count = g_RoomMgr.GetTypeCountByPosition(uCharId, equipRoomIndex, equipPos)
		binding	= g_RoomMgr.GetBindingTypeByPos(uCharId, equipRoomIndex, equipPos)
		if count == nil or count == 0 or sItemType < 5 or sItemType > 9 then
			return false
		end 
		equip_id = g_RoomMgr.GetAllItemFromPosition(uCharId, equipRoomIndex, equipPos)
		if equip_id:GetRowNum() > 0 then
			equipId = equip_id(1,1)
		end
	else
		if equipitemid ~= 0 then
			local equipSet = EquipSmeltSoulScrollSql._GetEquipPartByEquipId:ExecSql("n",equipitemid,uCharId)
			if nil ~= equipSet and equipSet:GetRowNum() > 0 then
				equipId = equipitemid
				equip_part = equipSet:GetData(0,0)
				flag = true
			else
				return false
			end
		end
	end
	
	local b_ifSmeltSoul,errorId = JudgeEquipCanSmeltSoul(bigID,SmeltSoulScrollName,equipPart)
	if b_ifSmeltSoul then
		local parameter = {
			["fromPlaceType"] = data["fromPlaceType"],
			["EquipID"] = equipId,
			["Class"] = data["Class"],
			["CharID"] = uCharId,
			["eEquipPart"] = data["eEquipPart"],
			["sceneName"] = data["sceneName"],
			["binding"] = binding
		}
		local b_Flag,result = GetSoulPealFromEquip(parameter)
		if b_Flag then
			local DelRet = g_RoomMgr.DelItemByPos(uCharId,SmeltSoulScrollRoomIndex,SmeltSoulScrollPos,1,event_type_tbl["物品使用"])
			if IsNumber(DelRet) then
				CancelTran()
				return false
			end
			local tbl =
				{
					["smeltSoulScrollId"] = smeltSoulScrollId,
					["Flag"] = flag,
					["equipId"] = equipId,
					["equip_part"] = equip_part,
					["b_Flag"] = b_Flag,
					["result"] = result,
				}
			return true,tbl
		else
			return b_Flag,result
		end
	end
	return false,errorId
end


SetDbLocalFuncType(GasEquipSmeltSoulScrollDB.EquipSmeltSoul)
return GasEquipSmeltSoulScrollDB
