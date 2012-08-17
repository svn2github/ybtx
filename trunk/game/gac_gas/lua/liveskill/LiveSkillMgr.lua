gac_gas_require "liveskill/LiveSkillMgrInc"
cfg_load "liveskill/LiveSkill_Common"
cfg_load "liveskill/DirectionType_Common"
cfg_load "liveskill/BasicLiveskill_Common"
cfg_load "liveskill/SmithingSmeltFetchRelation_Common"
cfg_load "liveskill/LiveSkillBigType_Common"
cfg_load "liveskill/Practiced_Common"
cfg_load "liveskill/SmithingExpert_Common"
cfg_load "liveskill/MaterialMatch_Common"


function CLiveSkillMgr:Ctor()
	self.tbl_LiveSkill		= LiveSkill_Common
	self:Init()
	self.m_tblDefaultLearn = {}
	for i, v in ipairs( LiveSkillBigType_Common:GetKeys() ) do
		if( 0 == LiveSkillBigType_Common(v, "SkillBigType") ) then
			table.insert(self.m_tblDefaultLearn, v)
		end
	end
end

function CLiveSkillMgr:Init()--商业生活技能类型对应表
	self.m_ProdLogReturnNum		= 50	--必须是每页显示数目的整数倍
	self.m_RefineProbability	= 20	--淬炼概率(百分比)
	self.m_tblSkillType			= {["锻造"] = 1, ["铸甲"] = 2, ["工艺"] = 3}
	self.m_tblDirectionType		= {}
	self.m_tblPracticed			= {}
	for i, v in ipairs( DirectionType_Common:GetKeys() ) do
		local tblSkillType = loadstring("return {" .. DirectionType_Common(v, "SkillType") .. "}")()
		for j, w in pairs(tblSkillType) do
			if(not self.m_tblDirectionType[w]) then
				self.m_tblDirectionType[w] = {}
			end
			self.m_tblDirectionType[w][DirectionType_Common(v, "Order")] = v
		end
	end
	
	for i, v in ipairs ( Practiced_Common:GetKeys() ) do
		local subInfo = Practiced_Common(v)
		for j, w in ipairs( subInfo:GetKeys() ) do
			if(not self.m_tblPracticed[v]) then
				self.m_tblPracticed[v] = {}
			end
			self.m_tblPracticed[v][subInfo(w, "Order")] = w
		end
	end
	--日志类型
	self.m_tblLogType = {["锻造"] = 1, ["铸甲"] = 2,["淬炼"] = 3,["烹饪"] = 4,["制药"] = 5,["工艺"] = 6}
end

function CLiveSkillMgr:GetOneExpertInfoByColName(sSkillName, sExpertName, sColName)
	return Practiced_Common(sSkillName, sExpertName, sColName)
end

--判断俩材料是否可以配对
function CLiveSkillMgr:MaterialCanMatch(Embryo, QualityMaterial)
	return MaterialMatch_Common(Embryo, QualityMaterial)
end

--判断某熟练度是否存在
function CLiveSkillMgr:ExistPractice(nSkillType,sPracticeType)
	local Practices = self:GetPracticedListByIndex(nSkillType)
	for i =1,#Practices do
		if Practices[i] == sPracticeType then
			return true
		end
	end
	return false
end

--获得日志类型
function CLiveSkillMgr:GetLogTypeByStr(sLogStr)
	return self.m_tblLogType[sLogStr]
end

--根据技能名称和等级得到经验和熟练度提升
function CLiveSkillMgr:GetSpecialAndExpBySkillLevel(skillName, uLevel)
	local uLevel = tostring(uLevel)
	if( BasicLiveskill_Common(skillName) and BasicLiveskill_Common(skillName, uLevel) ) then
		return BasicLiveskill_Common(skillName, uLevel, "Special"), BasicLiveskill_Common(skillName, uLevel, "Experience")
	end
	return 0, 0
end

function CLiveSkillMgr:GetLearnSkillMoney(skillName, uLevel)
	local uLevel = tostring(uLevel)
	if( BasicLiveskill_Common(skillName) and BasicLiveskill_Common(skillName, uLevel) ) then
		return BasicLiveskill_Common(skillName, uLevel, "NeedMoney")
	end
	return 0
end

function CLiveSkillMgr:GetSkillBigType(sSkillName)
	if LiveSkillBigType_Common(sSkillName) then
		return LiveSkillBigType_Common(sSkillName, "SkillBigType")
	end
end

--根据技能名称和等级得到技能信息
function CLiveSkillMgr:HasSkillInfo(skillName, uLevel)
	if BasicLiveskill_Common( skillName) then
		return BasicLiveskill_Common( skillName, tostring(uLevel) )
	end
end

--根据技能名称和等级得到技能信息
function CLiveSkillMgr:GetSkillInfoByName(skillName, uLevel, sColName)
	if BasicLiveskill_Common(skillName) and BasicLiveskill_Common(skillName, tostring(uLevel)) then
		return BasicLiveskill_Common(skillName, tostring(uLevel), sColName)
	end
end

--获得默认学会的生活技能的信息
function CLiveSkillMgr:GetDefaultLearnedSkillInfo()
	return self.m_tblDefaultLearn
end

--根据生活技能名称和产生方向得到所有产品
function CLiveSkillMgr:GetProductsCanMake(skillType, sDirection, nSkillLevel)
	local tbl = {}
	if self.tbl_LiveSkill(skillType) then
		local SubInfo = self.tbl_LiveSkill(skillType, sDirection)
		if SubInfo then
			local EquipNames = SubInfo:GetKeys()
			for i,Name in pairs(EquipNames) do
				local Info = SubInfo(Name)
				if nSkillLevel >= Info("SkillLevel") and Info("Probability") == 1 then
					local tblTemp = {}
					tblTemp.name	= Name
					tblTemp.level	= Info("SkillLevel")
					table.insert(tbl, tblTemp)
				end
			end
		end
	end
	return tbl
end

--根据物品名称和大类获得器魂淬炼表对应的信息
function CLiveSkillMgr:GetEquipSmeltInfo(sItemName, sItemType)
	if SmithingSmeltFetchRelation_Common(sItemName) then
		return SmithingSmeltFetchRelation_Common(sItemName, tostring(sItemType))
	end
end

--根据技能名称得到技能类型
function CLiveSkillMgr:GetSkillTypeByName(sSkillName)
	return self.m_tblSkillType[sSkillName]
end

--根据技能名称得到熟练度名称列表
function CLiveSkillMgr:GetPracticedListByName(sSkillName)
	return self.m_tblPracticed[sSkillName]
end

--根据技能索引得到熟练度名称列表
function CLiveSkillMgr:GetPracticedListByIndex(nSkillIndex)
	for k, v in pairs(self.m_tblSkillType) do
		if(v == nSkillIndex) then
			return self.m_tblPracticed[k]
		end
	end
end

--根据产品名称得到所有生产该产品所需的材料
function CLiveSkillMgr:GetMaterialsByProduct(sSkillName,sDirectType,sProductName)
	local tblMaterials = {}
	local MaterialInfo = self:GetProductInfoByName(sSkillName,sDirectType,sProductName)
	if MaterialInfo then
		for n = 1, 3 do
			local Material = {}
			local MaterialN = "Material" .. n
			local MaterialType = "MaterialType" .. n
			local NumberNeed = "NumberNeed" .. n
			if (MaterialInfo(MaterialN) ~= "" and MaterialInfo(MaterialType) ~= "" and MaterialInfo(NumberNeed) ~= "") then
					if # tblMaterials == 0 then
						table.insert(Material, MaterialInfo(MaterialN))
						table.insert(Material, tonumber(MaterialInfo(MaterialType)))
						table.insert(Material,tonumber(MaterialInfo(NumberNeed)) or 0)
						table.insert(tblMaterials, Material)
					else
						for j = 1,# tblMaterials do
							--材料相同的数量直接相加
							if tblMaterials[j][1] == MaterialInfo(MaterialN) and tblMaterials[j][2] == tonumber(MaterialInfo(MaterialType)) then
								tblMaterials[j][3] = tblMaterials[j][3] + (tonumber(MaterialInfo(NumberNeed)) or 0)
								break
							end
							if j == # tblMaterials then
								table.insert(Material, MaterialInfo(MaterialN))
								table.insert(Material, tonumber(MaterialInfo(MaterialType)))
								table.insert(Material, tonumber(MaterialInfo(NumberNeed)) or 0)
								table.insert(tblMaterials, Material)
							end
						end
					end
			end
		end
	end
	return tblMaterials
end

function CLiveSkillMgr:GetProductInfoByName(sSkillName,sDirectType,sProductName)
	if self.tbl_LiveSkill(sSkillName) and self.tbl_LiveSkill(sSkillName,sDirectType) then
		return self.tbl_LiveSkill(sSkillName,sDirectType,sProductName)
	end
end

function CLiveSkillMgr:GetProductNum(nMaterialNum)
	local nProbability = self.m_RefineProbability * nMaterialNum
	return math.floor(nProbability / 100), nProbability % 100
end

function CLiveSkillMgr:GetProductTotalNum(nMaterialNum)
	local nNum,nProb = self:GetProductNum(nMaterialNum)
	if nProb > 0 then
		local ranNum = math.random(100)
		if ranNum >= 0 and ranNum <= nProb then
			nNum = nNum + 1
		end
	end
	return nNum
end

function CLiveSkillMgr:GetMateialInfoByProductInfo(sSkillName,sDirectType,prescripName)
    local productInfo = self:GetProductInfoByName(sSkillName,sDirectType,prescripName)
    local materialType1 = tonumber(productInfo("MaterialType1"))
    local materialName1 = productInfo("Material1")
    local materialNum1 = tonumber(productInfo("NumberNeed1"))
    local materialType2 =  tonumber(productInfo("MaterialType2"))
    local materialName2 = productInfo("Material2")
    local materialNum2 = tonumber(productInfo("NumberNeed2"))
    local materialType3 =  tonumber(productInfo("MaterialType3"))
    local materialName3 = productInfo("Material3")
    local materialNum3 =tonumber( productInfo("NumberNeed3"))
    
    local materialTbl = {{materialType1, materialName1, materialNum1},
    {materialType2, materialName2, materialNum2},
    {materialType3, materialName3, materialNum3}}
    return materialTbl
end

function CLiveSkillMgr:GetExpertInfoByLevelEx(nExpertLevel, sColName)
	return SmithingExpert_Common(nExpertLevel, sColName)
end

g_LiveSkillMgr = g_LiveSkillMgr or CLiveSkillMgr:new()
