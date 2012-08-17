gac_gas_require "liveskill/LiveSkillMgr"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gas_require "resource/productive_profession/GasSkillMgr"

local CLiveSkillMgr = CLiveSkillMgr
local GasSkillMgr = CGasSkillMgr:new()
local event_type_tbl = event_type_tbl
-----------------------------------

local StmtOperater = {}	
local GasLiveSkillBox = CreateDbBox(...)

local StmtDef = {
	"_AddExperience",
	[[
		update tbl_commerce_skill set cs_uExperience = cs_uExperience + ?  
		where cs_uId = ? and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_AddSpecialize",
	[[
		replace into tbl_specialize_smithing(cs_uId,ss_uType,ss_uSpecialize,ss_uSkillType) values(?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_UpdateSpecialize",
	[[
		update tbl_specialize_smithing set ss_uSpecialize = ss_uSpecialize + ?  
		where 	cs_uId = ? 
		and 		ss_uType = ? 
		and 		ss_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

--【添加配方】
local StmtDef = {
	"_AddSkillDirection",
	[[
		replace into tbl_skill_directions(cs_uId,sd_sName,sd_sSkillName,sd_sDirectionType) 
		values(?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_AddDirectionCoolTime",
	[[
		replace into tbl_directions_cool_time(cs_uId,sd_sName,sd_sSkillName,sd_sDirectionType,sd_dtStartTime) 
		values(?,?,?,?,now())
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_GetDirectionCoolTime",
	[[
		select unix_timestamp(now()) - unix_timestamp(sd_dtStartTime) from tbl_directions_cool_time
		where cs_uId = ? 
		and sd_sSkillName = ? 
		and sd_sDirectionType = ?
		and sd_sName = ? 
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_CountSkillByType",
	[[
		select count(*) from tbl_commerce_skill  
		where cs_uId  = ?	and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function GasLiveSkillBox.IsStudySkill(uPlayerID,sSkillName)
	local tblCount = StmtOperater._CountSkillByType:ExecStat(uPlayerID,sSkillName)
	return tblCount:GetNumber(0,0) > 0
end

function GasLiveSkillBox.GetCoolDownTime(data)
	local tblTime = StmtOperater._GetDirectionCoolTime:ExecStat(data.uPlayerID,data.sSkillName,data.DirectType,data.DirectionName)
	if tblTime:GetRowNum() == 0 then return -1 end
	return tblTime(1,1)
end

function GasLiveSkillBox.IsCoolDownDirection(uPlayerID,sSkillName,DirectType,DirectionName)
	local data = {}
	data.uPlayerID,data.sSkillName = uPlayerID,sSkillName
	data.DirectType,data.DirectionName = DirectType,DirectionName
	local PassTime = GasLiveSkillBox.GetCoolDownTime(data)
	local CoolTime = GasSkillMgr:GetDirectionCoolTime(sSkillName,DirectType,DirectionName)
	if -1 == PassTime then return false end
	return PassTime < CoolTime 
end

--【获得学会的配方】
local StmtDef = {
	"_GetSkillDirections",
	[[
		select sd_sName from tbl_skill_directions where cs_uId = ? and sd_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
local StmtDef = {
	"_GetExpertInfo",
	[[
		select es_uType,es_uLevel from tbl_expert_smithing 
		where cs_uId = ? and es_uSkillType = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function GasLiveSkillBox.GetProductsCanMake(parameters)
	local uPlayerID = parameters.uPlayerID
	local sSkillName = parameters.sSkillName
	local DirectType = parameters.DirectType
	local query_result = StmtOperater._GetSkillDirections:ExecStat(uPlayerID,sSkillName)
	local liveSkill = CLiveSkillMgr:new()
	local nSkillIndex = liveSkill:GetSkillTypeByName(sSkillName)
	local tbl_expert = StmtOperater._GetExpertInfo:ExecStat(uPlayerID,nSkillIndex)
	return query_result,tbl_expert
end

-------------------------------------------------------
function GasLiveSkillBox.MaterialIsEnough(uPlayerID,tblMaterials, nNum)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	for i=1,#tblMaterials do
		local MaterialName, MaterialType,NumberNeed = tblMaterials[i][1],tblMaterials[i][2],tblMaterials[i][3]
		local uMaterialCount = g_RoomMgr.GetItemCount(uPlayerID, MaterialType, MaterialName)
		if  nNum*NumberNeed > uMaterialCount then
			--"材料不够"
			return false,9501
		end
	end
	return true
end

function GasLiveSkillBox.DelMaterials(uPlayerID,sSkillName,DirectType,DirectionName, nNum)
	local res = {}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local tbl = {}
	local liveSkill = CLiveSkillMgr:new()
	local tblMaterials = liveSkill:GetMaterialsByProduct(sSkillName,DirectType,DirectionName)	-- 得到生产该产品所需的所有材料
	for i=1,#tblMaterials do
		local MaterialName, MaterialType,NumberNeed = tblMaterials[i][1],tblMaterials[i][2],tblMaterials[i][3]
		local resi = g_RoomMgr.DelItem(uPlayerID, MaterialType, MaterialName, nNum*NumberNeed,nil,event_type_tbl["烹饪制药"])
		if IsNumber(resi) then
			CancelTran()
			return
		end
		table.insert(res,resi)
	end
	return true,res
end

function GasLiveSkillBox.AddProducts(data)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local tbl_id = {}	
	local ItemType = data.ProdType
	local uPlayerID = data.uPlayerID
	local params= {}
	params.m_nType = ItemType
	params.m_sName = data.sProdName
	params.m_nCharID = data.uPlayerID
	params.m_sCreateType = event_type_tbl["烹饪制药"]
	params.m_nEventID = data.nEventID
	for i=1,data.nProdNum do
		local item_id = g_RoomMgr.CreateItem(params)
		if IsNumber(item_id) then
			table.insert(tbl_id, {item_id})
		else
			CancelTran()
			return
		end
	end
	if ItemType >= 5 and ItemType <= 9 then
		local ItemMakerMgrDB = RequireDbBox("ItemMakerMgrDB")
		for j =1,#tbl_id do
			if not ItemMakerMgrDB.AddItemMaker(uPlayerID,tbl_id[j][1]) then
				CancelTran()
				return
			end
		end
	end
	local ret_info = {}
	if g_RoomMgr.HaveEnoughRoomPos(uPlayerID,ItemType,data.sProdName,#tbl_id) then
		local param = {}
		param.m_nCharID = uPlayerID
		param.m_nItemType = ItemType
		param.m_sItemName = data.sProdName
		param.m_tblItemID = tbl_id
		param.m_nBindType = data.BindType
		local item_bag_box = RequireDbBox("CPutItemToBagDB")
		local succ, info = item_bag_box.AddCountItem(param)
		if(not succ) then
			CancelTran()
			return false,info
		end
		ret_info = info
	else
			local SysMailDB = RequireDbBox("SysMailDB")
			local SenderName = 1014
			local MailTitle = 1014
			local MailContent = ""
			local nSendMoney = 0
			local succ = SysMailDB.SendSysMail(SenderName,uPlayerID,MailTitle,MailContent,tbl_id,nSendMoney,8,5001)
			if not succ  then
				CancelTran()
				return
			end
			Db2CallBack("LiveSkillSendMail",uPlayerID)
	end
	return true,info,tbl_id
end

--判断是否可以制造
function GasLiveSkillBox.CanMakeDirection(uPlayerID,sSkillName,DirectType,DirectionName)
	local liveSkill = CLiveSkillMgr:new()
	local productInfo =  liveSkill:GetProductInfoByName(sSkillName,DirectType,DirectionName)		--得到该产品对应配置表信息
	if not productInfo then 
		return 9568
	end
	
	if not GasLiveSkillBox.IsStudySkill(uPlayerID,sSkillName) then
		--“你还没有学习gai技能！”
		return 9530
	end
	if GasLiveSkillBox.IsCoolDownDirection(uPlayerID,sSkillName,DirectType,DirectionName) then
		return 9567
	end
	if (sSkillName == "锻造" or sSkillName == "铸甲" or sSkillName == "工艺") and (DirectType ~= "战神" and DirectType ~= "法魔") then
		--专精等级判断
		local ExpertAndPracticeDB = RequireDbBox("ExpertAndPracticeDB")
		if not ExpertAndPracticeDB.IsStudyExpert(uPlayerID,DirectType) then
			return 9565
		end
		local nSkillType = liveSkill:GetSkillTypeByName(sSkillName)
		local expert_info = ExpertAndPracticeDB.GetExpert(uPlayerID,nSkillType)
		if #expert_info > 0 and expert_info[2] < productInfo("SkillLevel") then
			return 9558
		end
	else
		local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
		local DBSkillInfo = LiveSkillBaseDB.GetSkillInfo(uPlayerID,sSkillName)
		if #DBSkillInfo == 0 then return end
		local now_skill_level = DBSkillInfo[2]
		if now_skill_level < productInfo("SkillLevel") then
			--技能等级不够
			return 9516
		end
	end
	return true
end

function GasLiveSkillBox.CanProduceByProductName(params)
	local uPlayerID, DirectionName, nNum = params.uPlayerID,params.DirectionName,params.nNum
	local sSkillName,DirectType = params.sSkillName,params.sDirectType
	local res = GasLiveSkillBox.CanMakeDirection(uPlayerID,sSkillName,DirectType,DirectionName)
	if not res or IsNumber(res) then
		return res
	end
	local liveSkill = CLiveSkillMgr:new()
	local tblMaterials = liveSkill:GetMaterialsByProduct(sSkillName,DirectType,DirectionName)	-- 得到生产该产品所需的所有材料
	if not GasLiveSkillBox.MaterialIsEnough(uPlayerID,tblMaterials, nNum) then
		return 9501
	end
	return true
end

--【添加经验】
function GasLiveSkillBox.AddLiveSkillExp(uPlayerID,SkillName,nExp)
	StmtOperater._AddExperience:ExecStat(nExp, uPlayerID, SkillName)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

local StmtDef = {
	"_GetExperience",
	[[
		select cs_uExperience from  tbl_commerce_skill
		where cs_uId = ? and cs_sSkillName = ?
	]]
}
DefineSql( StmtDef, StmtOperater )
function GasLiveSkillBox.GetExperience(uPlayerID,sSkillName)
	local query_result = StmtOperater._GetExperience:ExecStat(uPlayerID,sSkillName)
  if query_result:GetRowNum() == 0 then
  	return 0
  else
  	return query_result:GetNumber(0,0)
  end
end

--【添加熟练度和经验】
function GasLiveSkillBox.AddSpeciAndExp(uPlayerID,sSkillName,DirectType,DirectionName,event_type)
	
	local ExpertAndPracticeDB = RequireDbBox("ExpertAndPracticeDB")
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local DBSkillInfo = LiveSkillBaseDB.GetSkillInfo(uPlayerID,sSkillName)
	if #DBSkillInfo == 0 then return end
	local now_skill_level,nNowExp = DBSkillInfo[2],DBSkillInfo[3]
	
	local CommonSkillMgr = CLiveSkillMgr:new()
	local DirectionInfo =  CommonSkillMgr:GetProductInfoByName(sSkillName,DirectType,DirectionName)		--得到该产品对应配置表信息
	local SpecializeType = DirectionInfo("SpecializeType")
	--获得熟练度和经验上限
	local nMaxSpecial		= CommonSkillMgr:GetSkillInfoByName(sSkillName,now_skill_level, "UpperSpecialize") or 0
	local nMaxExp	= CommonSkillMgr:GetSkillInfoByName(sSkillName,now_skill_level+1, "ExperienceNeed") or CommonSkillMgr:GetSkillInfoByName(sSkillName,now_skill_level, "ExperienceNeed") or 0
	
	--获得提升的经验和熟练度
	local uSpecialAddNum,ExpAddNum = DirectionInfo("AddSpecialize"),DirectionInfo("AddExperience")
	
	local specialize = ExpertAndPracticeDB.GetSpecializeByType(uPlayerID,SpecializeType)
	local nSkillType = CommonSkillMgr:GetSkillTypeByName(sSkillName)
	if specialize >= 0 then
		--判断是否达到熟练度上限
		local nNowSpec = specialize
		if nNowSpec < nMaxSpecial  then
		  if (nNowSpec + uSpecialAddNum) > nMaxSpecial then
					uSpecialAddNum = nMaxSpecial - nNowSpec
			end
			StmtOperater._UpdateSpecialize:ExecStat(uSpecialAddNum,uPlayerID,SpecializeType,nSkillType)
		else
			uSpecialAddNum = 0
		end
	else
		if nSkillType then
			if uSpecialAddNum > nMaxSpecial then
				uSpecialAddNum = nMaxSpecial
			end
			StmtOperater._AddSpecialize:ExecStat(uPlayerID,SpecializeType,uSpecialAddNum,nSkillType)
		end
	end
	--增加X点锻造经验
	--经验达到可升级经验就不再长，升级后才可继续长经验
	local event_id
	local nTotalExp = 0
	if nNowExp < nMaxExp then
		if (nNowExp + ExpAddNum) > nMaxExp then
			ExpAddNum = nMaxExp - nNowExp
		end
		GasLiveSkillBox.AddLiveSkillExp(uPlayerID,sSkillName,ExpAddNum)
		--记录log
		nTotalExp =  GasLiveSkillBox.GetExperience(uPlayerID,sSkillName)
		local g_LogMgr = RequireDbBox("LogMgrDB")
		event_id = g_LogMgr.SaveLiveskillInfo(uPlayerID,sSkillName,now_skill_level,nTotalExp,event_type)
	else
		ExpAddNum = 0
	end
	return uSpecialAddNum,ExpAddNum,nTotalExp,event_id
end

--【制造】
function GasLiveSkillBox.ProduceByProductName(params)
	local uMsgID = GasLiveSkillBox.CanProduceByProductName(params)
	if not uMsgID or IsNumber(uMsgID) then
		return false,uMsgID
	end
	
	local uPlayerID = params.uPlayerID
	local DirectionName = params.DirectionName
	local nNum = params.nNum
	local sceneName = params.sceneName
	local sSkillName = params.sSkillName
	local DirectType = params.sDirectType
	
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local liveSkill = CLiveSkillMgr:new()
	local productInfo =  liveSkill:GetProductInfoByName(sSkillName,DirectType,DirectionName)		--得到该产品对应配置表信息
	if not productInfo then return end
		
	local DBSkillInfo = LiveSkillBaseDB.GetSkillInfo(uPlayerID,sSkillName)
	if #DBSkillInfo == 0 then return end
		
	local now_skill_level,nNowExp = DBSkillInfo[2],DBSkillInfo[3]
	
	--为了使加经验和添加物品的eventid一致，所以把加经验放到了加物品前面
	local AddSpecial,AddExp,nTotalExp,event_id = GasLiveSkillBox.AddSpeciAndExp(uPlayerID,sSkillName,DirectType,DirectionName,8)
	--删除材料
	local bFlag,res = GasLiveSkillBox.DelMaterials(uPlayerID,sSkillName,DirectType,DirectionName, nNum)
	if not bFlag then return end
	
	--添加产出物品
	local param = {}
	param.uPlayerID = uPlayerID
	param.ProdType,param.sProdName = GasSkillMgr:GetProductByDirection(sSkillName,DirectType,DirectionName)
	param.nProdNum = nNum
	param.sceneName = sceneName
	param.nEventID = event_id
	local bFlag2,info,tbl_ids = GasLiveSkillBox.AddProducts(param)
	if not bFlag2 then
		if info then
			return false,info
		else return end
	end
	
	--随机学会一种配方
	local data = {}
	data.uPlayerID = uPlayerID
	data.sSkillName = sSkillName
	data.DirectType = DirectType
	local dir_have_learned = GasLiveSkillBox.GetProductsCanMake(data)
	local sDirection, PropDirection = GasSkillMgr:RandomDirectBySkill(sSkillName,productInfo("SkillLevel"),dir_have_learned)
	if PropDirection then
		StmtOperater._AddSkillDirection:ExecStat(uPlayerID,PropDirection,sSkillName,DirectType)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CancelTran()
			return
		end
	end
	local CoolTime = GasSkillMgr:GetDirectionCoolTime(sSkillName,DirectType,DirectionName)
	if CoolTime and CoolTime > 0 then
		StmtOperater._AddDirectionCoolTime:ExecStat(uPlayerID,DirectionName,sSkillName,DirectType)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CancelTran()
			return
		end
	end
	local tblRetRes = {}
	tblRetRes.m_tblDelRes = res
	tblRetRes.m_tblAddRes = info
	tblRetRes.m_AddExp = AddExp
	tblRetRes.m_AddSpecial= AddSpecial
	tblRetRes.new_direction = sDirection and {sDirection, PropDirection} or nil
	tblRetRes.m_nTotalExp = nTotalExp
	tblRetRes.CoolTime = CoolTime
	return true,tblRetRes
end

--【代工厂制造】
function GasLiveSkillBox.MakeDirectionForContractManufacture(uPlayerID,target_id,sSkillName,DirectType,DirectionName,SceneName)
	local res = GasLiveSkillBox.CanMakeDirection(uPlayerID,sSkillName,DirectType,DirectionName)
	if not res or IsNumber(res) then
		return false,res
	end
	
	local LiveSkillBaseDB = RequireDbBox("LiveSkillBaseDB")
	local liveSkill = CLiveSkillMgr:new()
	local productInfo =  liveSkill:GetProductInfoByName(sSkillName,DirectType,DirectionName)		--得到该产品对应配置表信息
		
	local DBSkillInfo = LiveSkillBaseDB.GetSkillInfo(uPlayerID,sSkillName)
	if #DBSkillInfo == 0 then return end
		
	local now_skill_level,nNowExp = DBSkillInfo[2],DBSkillInfo[3]
	
	--为了使加经验和添加物品的eventid一致，所以把加经验放到了加物品前面
	local event_type = 164
	local AddSpecial,AddExp,nTotalExp,event_id = GasLiveSkillBox.AddSpeciAndExp(uPlayerID,sSkillName,DirectType,DirectionName,event_type)

	--添加产出物品
	local ProdType,sProdName = GasSkillMgr:GetProductByDirection(sSkillName,DirectType,DirectionName)
	local params= {}
	params.m_nType = ProdType
	params.m_sName = sProdName
	params.m_nCharID = uPlayerID
	params.m_sCreateType = event_type
	params.m_nEventID =event_id
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local item_id = g_RoomMgr.CreateItem(params)
	
	local SysMailDB = RequireDbBox("SysMailDB")
	local SenderName = 1000
	local MailTitle = 999
	local MailContent = "998_item:" .. ProdType .. "|" .. sProdName
	local nSendMoney = 0
	local succ = SysMailDB.SendSysMail(SenderName,target_id,MailTitle,MailContent,{{item_id}},nSendMoney,event_type,5008)
	if not succ  then
		CancelTran()
		return
	end
	
	--随机学会一种配方
	local data = {}
	data.uPlayerID = uPlayerID
	data.sSkillName = sSkillName
	data.DirectType = DirectType
	local dir_have_learned = GasLiveSkillBox.GetProductsCanMake(data)
	local sDirection, PropDirection = GasSkillMgr:RandomDirectBySkill(sSkillName,productInfo("SkillLevel"),dir_have_learned)
	if PropDirection then
		StmtOperater._AddSkillDirection:ExecStat(uPlayerID,PropDirection,sSkillName,DirectType)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CancelTran()
			return
		end
	end
	local CoolTime = GasSkillMgr:GetDirectionCoolTime(sSkillName,DirectType,DirectionName)
	if CoolTime and CoolTime > 0 then
		StmtOperater._AddDirectionCoolTime:ExecStat(uPlayerID,DirectionName,sSkillName,DirectType)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CancelTran()
			return
		end
	end
	
	local itemIDTbl  = {{item_id}}
    if ProdType >= 5 and ProdType <= 9 then
		local ItemMakerMgrDB = RequireDbBox("ItemMakerMgrDB")
		for j =1,# itemIDTbl do
			if not ItemMakerMgrDB.AddItemMaker(uPlayerID,itemIDTbl[j][1]) then
				CancelTran()
				return
			end
		end
	end
	
	local tblRetRes = {}
	tblRetRes.m_AddExp = AddExp
	tblRetRes.m_AddSpecial= AddSpecial
	tblRetRes.m_nTotalExp = nTotalExp
	tblRetRes.CoolTime = CoolTime
	tblRetRes.sSkillName = sSkillName
	tblRetRes.target_id = target_id
	tblRetRes.new_direction = sDirection and {sDirection, PropDirection} or nil
	tblRetRes.ItemIDTbl = {item_id}
	Db2CallBack("ReturnMakeDirectionForCM",uPlayerID,tblRetRes)
	return true,tblRetRes
end

----------------------------
SetDbLocalFuncType(GasLiveSkillBox.ProduceByProductName)
SetDbLocalFuncType(GasLiveSkillBox.CanProduceByProductName)
SetDbLocalFuncType(GasLiveSkillBox.GetProductsCanMake)
return GasLiveSkillBox
