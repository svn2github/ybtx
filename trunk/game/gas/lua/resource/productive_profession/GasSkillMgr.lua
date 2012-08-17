gas_require "resource/productive_profession/GasSkillMgrInc"
cfg_load  "liveskill/SmithingProductRelation_Common"
cfg_load  "liveskill/SmithingProbability_Common"
cfg_load  "liveskill/SmithingExpert_Common"
cfg_load 	"liveskill/LiveSkill_Common"

function CGasSkillMgr:Ctor()
	self.tbl_LiveSkill = LiveSkill_Common
	self.tbl_ProductRelation = SmithingProductRelation_Common	-- 产品关系表
	self.tbl_ProductProb = SmithingProbability_Common			--产品概率表
	self.tbl_Expert = SmithingExpert_Common		--专精表
end

--根据技能等级和名称随机出一个配方
function CGasSkillMgr:RandomDirectBySkill(nSkillName,nSkillLevel,dir_have_learned)
	local tbl = {}
	local ProbSum = 0
	local row = dir_have_learned:GetRowNum()
	local Info = self.tbl_LiveSkill(nSkillName)
	if Info then
		local Directs = Info:GetKeys()
		for m,v in pairs(Directs) do
			local NodesInfo = Info(v)
			if NodesInfo then
				local DirectionNames = NodesInfo:GetKeys()
				for i,p in pairs(DirectionNames) do
					if nSkillLevel == NodesInfo(p,"SkillLevel") and NodesInfo(p,"Probability") < 1  then
						if 0 == row then
							table.insert(tbl,{v,p,NodesInfo(p,"Probability")})
						else
							for j=1,row do
								if p == dir_have_learned(j,1) then
									break
								end
								if j == row then
									table.insert(tbl,{v,p,NodesInfo(p,"Probability")})
								end
							end
						end
					end
				end
			end
		end
	end
	if # tbl == 0 then
		return
	end
	for i =1,# tbl do
		local uRandomNum = math.random(1,1000000)
		local prop = tbl[i][3]*1000000
		if uRandomNum >=0 and uRandomNum <= prop then
			return tbl[i][1], tbl[i][2]
		end
	end
end

function CGasSkillMgr:GetProductByDirection(sSkillName,sDirectionType,DirectionName)
	local Info = self.tbl_LiveSkill(sSkillName,sDirectionType,DirectionName)
	local num = 100000
	local property1,property2 = Info("ProductPro1")*num,Info("ProductPro2")*num
	local uRandomNum = math.random(1,num)
	if uRandomNum <= property1 then
		return Info("ProductType1"),Info("ProductName1")
	else
		return Info("ProductType2"),Info("ProductName2")
	end
end

function CGasSkillMgr:GetDirectionCoolTime(sSkillName,sDirectionType,DirectionName)
	local info = self.tbl_LiveSkill(sSkillName,sDirectionType,DirectionName)
	local CoolTime = tonumber(info("CoolTime")) or 0
	return CoolTime
end

--根据材料名称得可以产出的产品类型(剑刀枪等)
function CGasSkillMgr:GetAllProductTypeByMaterialName(sMaterialName,sQualityItem)
	--主材料对应的产品类型
	if not self.tbl_ProductRelation(sMaterialName) then return {} end
		
	local tbl = self.tbl_ProductRelation:GetKeys(sMaterialName)
	
	--品质材料得概率
	local sQualityType = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetQualityMaterialItemBigID(),sQualityItem,"MaterialType")
	local BasicProb = {}
	for i =1,#tbl do
		local info = self.tbl_ProductProb(sQualityType)
		if info then
			local keys = self.tbl_ProductProb:GetKeys(sQualityType)
			for j,key in pairs(keys) do
				local p = info(key)
				if p and tbl[i] == p("ProductType") then
					local tbl = {tbl[i],p("Probability")}
					table.insert(BasicProb,tbl)
				end
			end
		end
	end
	
	return BasicProb
end

--根据主材料名称和产品类型随机出一个物品
function CGasSkillMgr:RandomOneItem(sMaterialName,sProductType,tblPractices,tblExpert,nSkilLevel)
	local ItemInfo = {}
	local TotalProp = 0
	--先找出所有该材料和产品类型对应的物品信息
	local ExperType,ExperLevel = "",0
	if #tblExpert > 0 then 
		ExperType,ExperLevel = unpack(tblExpert)
	end
	local DefaultKeys = self.tbl_ProductRelation:GetKeys(sMaterialName,sProductType)
	local SubInfo = self.tbl_ProductRelation(sMaterialName,sProductType)
	for i,key in pairs(DefaultKeys) do
			local p = SubInfo(key)
			--根据熟练度、专精、技能等级计算概率
			local nPractic,nExpert = 0,0
			local row = tblPractices:GetRowNum()
			for m=1,row do
				if tblPractices(m,1) == sProductType then
					nPractic = tblPractices(m,2)
					break
				end
			end
			if sProductType == ExperType then
				nExpert = ExperLevel
			end
			local Prop = self:GetPropByString(p("Probability"),nPractic,nExpert,nSkilLevel)
			local tbl = {p("EquipName"),p("EquipType"),sProductType,Prop}
			TotalProp = TotalProp + Prop
			table.insert(ItemInfo,tbl)
	end
	if 0 == TotalProp then
		return {}
	end
	--随机产出一种物品
	local item = {}
	local nRandomNum,nSum = math.random(100),0
	if #ItemInfo > 0 then
		for j =1,#ItemInfo do
			local nProb = ((ItemInfo[j][4])/TotalProp)*100
			if nRandomNum >= nSum and nRandomNum <= (nSum + nProb) and nProb > 0 then
				item = ItemInfo[j]
				break
			end
			if nSum > 100 then
				break
			else
				nSum = nSum + nProb
			end
		end
	end
		 
	return item
end

--根据专精等级获得该等级的专精信息
function CGasSkillMgr:GetExpertInfoByLevel(uExpertLevel)
	return self.tbl_Expert(uExpertLevel)
end

function CGasSkillMgr:GetPropByString(nProp, nPractic,nExpert,nSkilLevel)
	nProp = string.gsub( nProp, "S", nPractic)
	nProp = string.gsub( nProp, "Z", nExpert)
	nProp = string.gsub( nProp, "L", nSkilLevel)
	return loadstring(" return " .. nProp)()
end

--根据主材料和品质材料锻造出一个物品
--参数:主材料、品质材料
function CGasSkillMgr:ProduceItemByMaterial(sEmbryoItem,sQualityItem,tblPractices,tblExpert,nSkilLevel)
	local tblItem = {}
	--根据材料名称获得所有可以产出的产品熟练度类型和概率(剑刀枪)
	local BasicProb = self:GetAllProductTypeByMaterialName(sEmbryoItem,sQualityItem)
	if 0 == #BasicProb then
		return {}
	end
	
	--根据熟练度、专精和技能等级计算概率
	local uSumProb1,sProductType  = 0,""
	local ExperType,ExperLevel = "",0
	if #tblExpert > 0 then 
		ExperType,ExperLevel = unpack(tblExpert)
	end
	for k =1,#BasicProb do
		local nPracticeType,sProp = BasicProb[k][1],BasicProb[k][2]
		local nPractic,nExpert = 0,0
		local row = tblPractices:GetRowNum()
		for m=1,row do
			if tblPractices(m,1) == nPracticeType then
				nPractic = tblPractices(m,2)
				break
			end
		end
		if nPracticeType == ExperType then
			nExpert = ExperLevel
		end
		local Prop = self:GetPropByString(sProp,nPractic,nExpert,nSkilLevel)
		uSumProb1 = uSumProb1+Prop
		BasicProb[k][2] = Prop
	end
	if 0 == uSumProb1 then
		 return {}
	end
	--根据计算出的概率随机出产品类型(剑、枪、刀等)
	local uRandomNum,uSumProb2 = math.random(100),0
	for j = 1,#BasicProb do
		local nPropj = ((BasicProb[j][2])/uSumProb1)*100
		local uProb = uSumProb2 + nPropj
		if uRandomNum >= uSumProb2 and uRandomNum <= uProb and nPropj > 0 then
			sProductType = BasicProb[j][1]
			break
		else
			if uSumProb2 > 100 then
				break
			else
				uSumProb2 = uProb
			end
		end
	end
	
	--从最终产品表随机出一个产品
	if sProductType == "" then
		return {}
	else
		return self:RandomOneItem(sEmbryoItem,sProductType,tblPractices,tblExpert,nSkilLevel)
	end
end
