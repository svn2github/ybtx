CItemInfoMgr = class()
local CItemTable = CItemTableXml:new()
local ItemNumber = CItemTable:GetItemXmlNumber()
for i=0,ItemNumber-1 do
	cfg_load("item/"..CItemTable:GetItemXmlAppName(i))
	lan_load("item/"..CItemTable:GetItemXmlLanName(i))
end

cfg_load "equip/InitEquip_Common"
cfg_load "item/ItemLife_Common"
cfg_load "item/ItemMaterial_Common"
lan_load "item/Lan_ItemLife_Common"

--[[
	TODO: THIS FUNCTION TO BE REMOVED
--]]

function str_or_num_to_num(str)
	if type(str) == "string" then
		return tonumber(str)
	end
	return str
end

--如果这里的代码修改了，要修改GacBagSpace.lua 相应的代码
--也要修改寄售交易所CSMTreeListMap.lua,那个文件里有关于物品类型id的局部映射表
local BigId2FileTable = {}
local ItemTblNameStr = {}
local BigId2LanFileTable = {}
local tblItemTypeOrder = {}
for i=0,ItemNumber-1 do
	local index = CItemTable:GetItemXmlBigId(i)
	BigId2FileTable[index]=_G[CItemTable:GetItemXmlAppName(i)]
	ItemTblNameStr[index]=CItemTable:GetItemXmlAppName(i)
	BigId2LanFileTable[index]=_G[CItemTable:GetItemXmlLanName(i)]
	table.insert(tblItemTypeOrder,CItemTable:GetItemXmlBigId(i))
end
CItemTable = nil
--下面的策划配置表是非物品使用的，也就是没有大类id
local NotBigIdTable =
{
	[5] = InitEquip_Common,					--5 初始化装备
}
--背包整理时物品大类排序顺序
--【添加新的大类物品的时候记得加这个表】

--下面是"SkillItem_Common"表的SkillCoolDownType列, 策划填表和程序内枚举对应
ESkillCDType = 
{
	DrugItemCD	= "药品CD",
	SpecailItemCD	= "独立CD",
	OtherItemCD	= "其他CD"
}

function CItemInfoMgr:GetItemTypeOrder()
	return tblItemTypeOrder
end

function CItemInfoMgr:Ctor(tblId2,tblNoBigId)
	--不为空表示测试用例自己构造的数据，可以不受正式的数据影响
	if tblId2 ~= nil then
		self.m_tblId2Table = tblId2
	else
		self.m_tblId2Table = BigId2FileTable
		self.m_tblId2LanTable = BigId2LanFileTable
	end

	if tblNoBigId ~= nil then
		self.m_tblNoBigId = tblNoBigId
	else
		self.m_tblNoBigId = NotBigIdTable
	end
end

--是否为静态装备
function CItemInfoMgr:IsStaticEquip(itemType)
	local nBigID = str_or_num_to_num(itemType)
	if nBigID == nil then
		return false
	end
	return nBigID >= 5 and nBigID <= 9
end

--判断是否是魂珠
function CItemInfoMgr:IsSoulPearl(nBigID)
	return 19 == str_or_num_to_num(nBigID)
end


--判断是否是经验瓶/魂瓶
function CItemInfoMgr:IsExpOrSoulBottle(nBigID)
	return 48 == str_or_num_to_num(nBigID)
end
--************************************************************************
function CItemInfoMgr:HaveItem(ItemType,ItemName)
	local ItemType = str_or_num_to_num(ItemType)
	if self:IsSoulPearl(ItemType) then
		ItemName = self:GetSoulpearlInfo(ItemName)
	end
	if not (self.m_tblId2Table[ItemType]) then return end
	return self.m_tblId2Table[ItemType](ItemName) and true or false
end

function CItemInfoMgr:GetItemInfo(ItemType,ItemName,ColumnName)
	local ItemType = str_or_num_to_num(ItemType)
	if self:IsSoulPearl(ItemType) then
		ItemName = self:GetSoulpearlInfo(ItemName)
	end
	if(not self.m_tblId2Table[ItemType]) then
		LogErr("不存在该物品大类", "ItemType:" .. ItemType)
		return
	end
	assert(IsString(ItemName) or IsNumber(ItemName))
	local ItemInfo = self.m_tblId2Table[ItemType](ItemName)
	if(not ItemInfo) then
		LogErr("物品不存在", "Type:" .. ItemType .. "Name:" .. ItemName)
		return
	end
	
	return self.m_tblId2Table[ItemType](ItemName,ColumnName)
end

function CItemInfoMgr:GetItemFunInfo(ItemType,ItemName)
	local ItemType = str_or_num_to_num(ItemType)
	if self:IsSoulPearl(ItemType) then
		ItemName = self:GetSoulpearlInfo(ItemName)
	end
	assert(self.m_tblId2Table[ItemType],"ItemType<" .. ItemType .. ">不存在，请查实" )
	assert(IsString(ItemName) or IsNumber(ItemName))
	return self.m_tblId2Table[ItemType](ItemName)
end

function CItemInfoMgr:GetItemSoundName(ItemType,ItemName)
	local ItemType = str_or_num_to_num(ItemType)
	if self:IsSoulPearl(ItemType) then
		ItemName = self:GetSoulpearlInfo(ItemName)
	end
	local Material = self:GetItemInfo(ItemType,ItemName,"Material")
	if Material and ItemMaterial_Common(Material) then
		return ItemMaterial_Common(Material,"SoundName")
	end
	return ""
end

function CItemInfoMgr:PlayItemSound(ItemType,ItemName)
	local SoundName = self:GetItemSoundName(ItemType,ItemName)
	if SoundName ~= "" then PlayCue(SoundName) end
end

function CItemInfoMgr:GetItemInfoByDisplayName(DisplayName)
	local tblType = self:GetItemTypeOrder()
	local tblItemName = {}
	for i,v in pairs(tblType) do
		local ItemType = v
		local BigTbl = self.m_tblId2Table[ItemType]
		local tblNames = BigTbl:GetKeys()
		for j,k in pairs(tblNames) do
			local ItemName = k
			local disName = self:GetItemLanInfo(ItemType,ItemName,"DisplayName")
			if disName == DisplayName then
				table.insert(tblItemName,{ItemType,ItemName})
			end
		end
	end
	return tblItemName
end

function CItemInfoMgr:GetItemInfoByIndex(ItemName)
	local tblType = self:GetItemTypeOrder()
	for i,v in pairs(tblType) do
		local ItemType = v
		local BigTbl = self.m_tblId2Table[ItemType]
		if BigTbl(ItemName) then
				return {ItemType,ItemName}
		end
	end
end

---------------------------------------------------------------------------------------------------

--模糊匹配
function g_GetItemNameMatch(str_match)   
        local tbl_name_match = {} 
	for i,v in pairs(BigId2FileTable) do
		local keys = v:GetKeys()
		for k,p in pairs(keys) do
			if string.find(p, str_match, 1, true) then
				tbl_name_match[i] = tbl_name_match[i] or {}
				table.insert(tbl_name_match[i],p)
			end
		end
	end
	return tbl_name_match
end

