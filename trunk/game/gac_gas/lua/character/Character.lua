-- 不是player对象也需要这个资源，比如在角色选择界面和创建角色的时候。也需要知道缺省的装备资源id
-- 这样在删除完装备后，还能显示正常的模型
cfg_load "model_res/HorseRes_Common"
cfg_load "model_res/WeaponRes_Common"
cfg_load "model_res/CustomRes_Common"
cfg_load "item/Equip_Weapon_Common"
cfg_load "item/Equip_Armor_Common"
cfg_load "item/Equip_Jewelry_Common"
cfg_load "item/Equip_Ring_Common" 
cfg_load "item/Equip_Shield_Common"

-- 坐骑资源
g_sParseHorseTblServer = {}
g_sParseHorseTblClient = {}
AddCheckLeakFilterObj(g_sParseHorseTblServer)
AddCheckLeakFilterObj(g_sParseHorseTblClient)

function ParseHorseResServer()
	local count = 1
	for i,p in ipairs(HorseRes_Common:GetKeys()) do
		g_sParseHorseTblServer[p] = count
		count = count + 1
	end
end	

function ParseHorseResClient()
	local count = 1
		for i,p in ipairs(HorseRes_Common:GetKeys()) do
		g_sParseHorseTblClient[count] = p
		count = count + 1
	end
end

ParseHorseResServer()
ParseHorseResClient()
AddCheckLeakFilterObj(g_sParseHorseTblServer)
AddCheckLeakFilterObj(g_sParseHorseTblClient)

--武器资源
g_sParseWeaponTblServer = {}
g_sParseWeaponTblClient = {}
AddCheckLeakFilterObj(g_sParseWeaponTblServer)
AddCheckLeakFilterObj(g_sParseWeaponTblClient)

function ParseWeaponResServer()
	local count = 1
	for i,p in ipairs(WeaponRes_Common:GetKeys()) do
		g_sParseWeaponTblServer[p] = count
		count = count + 1
	end
end	

function ParseWeaponResClient()
	local count = 1
	for i,p in ipairs(WeaponRes_Common:GetKeys()) do
		g_sParseWeaponTblClient[count] = p
		count = count + 1
	end
end

ParseWeaponResServer()
ParseWeaponResClient()
AddCheckLeakFilterObj(g_sParseWeaponTblServer)
AddCheckLeakFilterObj(g_sParseWeaponTblClient)

g_ParseCustomTblServer = {}
g_ParseCustomTblClient = {}

function ParseCustomResServer()
	local count = 1
	for i,p in ipairs(CustomRes_Common:GetKeys()) do
		g_ParseCustomTblServer[p] = count
		count = count + 1
	end
end	

function ParseCustomResClient()
	local count = 1
	for i,p in ipairs(CustomRes_Common:GetKeys()) do
		g_ParseCustomTblClient[count] = p
		count = count + 1
	end
end

ParseCustomResServer()
ParseCustomResClient()
AddCheckLeakFilterObj(g_ParseCustomTblServer)
AddCheckLeakFilterObj(g_ParseCustomTblClient)

--武器解析表
g_SParseWeaponTbl = {}
g_CParseWeaponTbl = {}
AddCheckLeakFilterObj(g_SParseWeaponTbl)
AddCheckLeakFilterObj(g_CParseWeaponTbl)

function ParseWeaponTbl()
	local count = 1
	for i,p in ipairs(Equip_Weapon_Common:GetKeys()) do
		g_SParseWeaponTbl[p] = count
		g_CParseWeaponTbl[count] = p
		count = count + 1
	end
end

ParseWeaponTbl()

-- 盾牌解析表
g_SParseShieldTbl = {}
g_CParseShieldTbl = {}
AddCheckLeakFilterObj(g_SParseShieldTbl)
AddCheckLeakFilterObj(g_CParseShieldTbl)

function ParseShieldTbl()
	local count = 1
	for i,p in ipairs(Equip_Shield_Common:GetKeys()) do
		g_SParseShieldTbl[p] = count
		g_CParseShieldTbl[count] =  p
		count = count + 1
	end
end

ParseShieldTbl()

-- 戒指解析表
g_SParseRingTbl = {}
g_CParseRingTbl = {}
AddCheckLeakFilterObj(g_SParseRingTbl)
AddCheckLeakFilterObj(g_CParseRingTbl)

function ParseRingTbl()
	local count = 1
	for i,p in ipairs(Equip_Ring_Common:GetKeys()) do
		g_SParseRingTbl[p] = count
		g_CParseRingTbl[count] =  p
		count = count + 1
	end		
end

ParseRingTbl()

--饰品解析表 
g_SParseJewelryTbl = {}
g_CParseJewelryTbl = {}
AddCheckLeakFilterObj(g_SParseJewelryTbl)
AddCheckLeakFilterObj(g_CParseJewelryTbl)

function ParseJewelryTbl()
	local count = 1
	for i,p in ipairs(Equip_Jewelry_Common:GetKeys()) do
		g_SParseJewelryTbl[p] = count
		g_CParseJewelryTbl[count] =  p
		count = count + 1
	end		
end

ParseJewelryTbl()

--防具解析表
g_SParseArmorTbl = {}
g_CParseArmorTbl = {}
AddCheckLeakFilterObj(g_SParseArmorTbl)
AddCheckLeakFilterObj(g_CParseArmorTbl)

function ParseArmorTbl()
	local count = 1
	for i,p in ipairs(Equip_Armor_Common:GetKeys()) do
		g_SParseArmorTbl[p] = count
		g_CParseArmorTbl[count] =  p
		count = count + 1
	end		
end

ParseArmorTbl()

function g_GetNewRoleRes(nClass)
	--    模型10个部件
	--    头盔         衣服        背部(暂无)  肩部            手部       脚部        主手       副手(某些职业)
	local uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID =
		"","","","","","",0,0
		
	if nClass == EClass.eCL_Warrior then			--大剑士
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"大剑士头盔","大剑士身体","大剑士肩甲","大剑士护腕","大剑士鞋子",g_sParseWeaponTblServer["大剑士武器"]
	
	elseif nClass == EClass.eCL_MagicWarrior then	--魔剑士
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"魔剑士头盔","魔剑士身体","魔剑士肩甲","魔剑士护腕","魔剑士鞋子",g_sParseWeaponTblServer["魔剑士武器"]
	
	elseif nClass == EClass.eCL_Paladin then		--人类骑士
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"人类骑士头盔","人类骑士身体","人类骑士肩甲","人类骑士护腕","人类骑士鞋子",g_sParseWeaponTblServer["人类骑士武器"]
	
	elseif nClass == EClass.eCL_NatureElf then		--法师
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"法师头盔","法师身体","法师肩甲","法师护腕","法师鞋子",g_sParseWeaponTblServer["法师武器"]
	
	elseif nClass == EClass.eCL_EvilElf then		--邪魔
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"邪魔头盔","邪魔身体","邪魔肩甲","邪魔护腕","邪魔鞋子",g_sParseWeaponTblServer["邪魔武器"]
	
	elseif nClass == EClass.eCL_Priest then			--牧师
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"牧师头盔","牧师身体","牧师肩甲","牧师护腕","牧师鞋子",g_sParseWeaponTblServer["牧师武器"]
	
	elseif nClass == EClass.eCL_ElfHunter then		--精灵弓箭手
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"精灵弓箭手头盔","精灵弓箭手身体","精灵弓箭手肩甲","精灵弓箭手护腕","精灵弓箭手鞋子",g_sParseWeaponTblServer["精灵弓箭手武器"]
		
	elseif nClass == EClass.eCL_DwarfPaladin then	--矮人骑士
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID =
		"矮人骑士头盔","矮人骑士身体","矮人骑士肩甲","矮人骑士护腕","矮人骑士鞋子",g_sParseWeaponTblServer["矮人骑士武器"]
	
	elseif nClass == EClass.eCL_OrcWarrior then		--兽人战士
		uArmetResID, uBodyResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID =
		"兽人战士头盔","兽人战士身体","兽人战士肩甲","兽人战士护腕","兽人战士鞋子",g_sParseWeaponTblServer["兽人战士武器"],g_sParseWeaponTblServer["兽人战士副手武器"]
	end
	
	--return uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID
	local ResTbl = {}
	ResTbl["ArmetResID"] 		= uArmetResID
	ResTbl["BodyResID"] 		= uBodyResID
	ResTbl["ArmResID"] 			= uArmResID
	ResTbl["ShoeResID"] 		= uShoeResID
	ResTbl["ShoulderResID"] 	= uShoulderResID
	ResTbl["WeaponResID"] 		= uWeaponID
	ResTbl["OffWeaponResID"] 	= uOffWeaponID
	return ResTbl
end

function g_DelPieceByPart(EquipPart,RenderObj)
	if EquipPart == EEquipPart.eWear then --衣服
		RenderObj:RemovePiece( "body" )
	elseif EquipPart == EEquipPart.eHead then  --头
		RenderObj:RemovePiece( "armet" )
	elseif EquipPart == EEquipPart.eBack then --背
		RenderObj:RemovePiece( "back" )
	elseif EquipPart == EEquipPart.eShoe then --鞋
		RenderObj:RemovePiece( "shoe" )
	elseif EquipPart == EEquipPart.eWeapon then --主手
		RenderObj:RemovePiece( "weapon" )
	elseif EquipPart == EEquipPart.eAssociateWeapon then --副手
		RenderObj:RemovePiece( "offweapon" )
	elseif EquipPart == EEquipPart.eShoulder then --肩
		RenderObj:RemovePiece( "shoulder" )
	elseif EquipPart == EEquipPart.eHand then --手
		RenderObj:RemovePiece( "arm" )	
	end
end

function g_GetDefaultResID(nClass)
	return  "裸体身体","裸体护腕","裸体鞋子"
end

function g_IsUpdateModel(EquipPart)
	if EquipPart == EEquipPart.eWear --衣服
		or EquipPart == EEquipPart.eHead --头部
		or EquipPart == EEquipPart.eShoulder--肩部
		or EquipPart == EEquipPart.eShoe --脚部
		or EquipPart == EEquipPart.eWeapon --主手武器
		or EquipPart == EEquipPart.eAssociateWeapon --副手武器	
		or EquipPart == EEquipPart.eHand --手部
		or EquipPart == EEquipPart.eBack then --背部 								
		return true
	end
	return false
end

function CheckWeaponEquipType(EquipType)
	local hand = string.sub(EquipType,1,2)
	if hand == "单" then
		return true
	end
	return false
end

function g_GetResIDByEquipPart(EquipPart,Player)
	local uBodyResID, uArmResID, uShoeResID = g_GetDefaultResID(Player:CppGetClass())
	local ResID = 0
	if EquipPart == EEquipPart.eWear then --衣服
		local BodyResID = nil 
		local IndexID = Player.m_Properties:GetBodyIndexID()
		if IndexID ~= 0 then
			BodyResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
		end
		return (BodyResID and BodyResID or uBodyResID)
	elseif EquipPart == EEquipPart.eHead then  --头部
		local HeadResID = nil
		local IndexID =  Player.m_Properties:GetHeadIndexID()
		local bShowArmet = Player.m_Properties:GetShowArmet()
		if IndexID ~= 0 and bShowArmet then
			HeadResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
		end
		return (HeadResID and HeadResID or ResID )
	elseif EquipPart == EEquipPart.eShoulder then --肩部
		local ShoulderResID	= nil
		local IndexID =  Player.m_Properties:GetShoulderIndexID()
		if IndexID ~= 0 then
			ShoulderResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
		end
		return (ShoulderResID and ShoulderResID or ResID)
	elseif EquipPart == EEquipPart.eShoe then   --脚部
		local ShoeResID = nil  
		local IndexID =  Player.m_Properties:GetShoeIndexID()
		if IndexID ~= 0 then
			ShoeResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
		end
		return ShoeResID and ShoeResID or uShoeResID
	elseif EquipPart == EEquipPart.eWeapon then --主手武器
		local WeaponResID = nil
		local IndexID =  Player.m_Properties:GetWeaponIndexID()
		if IndexID ~= 0 then
			WeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticWeaponBigID(),g_CParseWeaponTbl[IndexID],"ResID")]
		end
		return (WeaponResID and WeaponResID or ResID)
	elseif EquipPart == EEquipPart.eAssociateWeapon then --副手武器	
		local OffWeaponResID  = nil
		local nBigID =  Player.m_Properties:GetOffWeaponBigID()
		if nBigID ~= 0 then
			local IndexID =  Player.m_Properties:GetOffWeaponIndexID()
			if g_ItemInfoMgr:IsStaticWeapon(nBigID) then
				if CheckWeaponEquipType(g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseWeaponTbl[IndexID],"EquipType")) then 
					OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseWeaponTbl[IndexID],"AssociateResID")]
				else
					OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseWeaponTbl[IndexID],"ResID")]
				end
			elseif g_ItemInfoMgr:IsStaticShield(nBigID) then
				OffWeaponResID = g_sParseWeaponTblServer[g_ItemInfoMgr:GetItemInfo(nBigID,g_CParseShieldTbl[IndexID],"ResID")]
			end
			
		end
		return (OffWeaponResID and OffWeaponResID or ResID)		
	elseif EquipPart == EEquipPart.eHand then  --手部
		local HandResID  = nil
		local IndexID =  Player.m_Properties:GetArmIndexID()
		if IndexID ~= 0 then 
			HandResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
		end 
		return (HandResID and HandResID or uArmResID)			
	elseif EquipPart == EEquipPart.eBack then  --背部
		local BackResID  = nil
		local IndexID =  Player.m_Properties:GetBackIndexID()
		if IndexID ~= 0 then 
			BackResID = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetStaticArmorBigID(),g_CParseArmorTbl[IndexID],"ResID")
		end
		return (BackResID and BackResID or ResID)			
	end
end

function g_GetEquipPhaseByEquipPart(eEquipPart,Player)
	if eEquipPart == EEquipPart.eWeapon then
		return Player.m_Properties:GetWeaponPhase()
	elseif eEquipPart == EEquipPart.eAssociateWeapon then
		return Player.m_Properties:GetOffWeaponPhase()
	elseif eEquipPart == EEquipPart.eWear then
		return Player.m_Properties:GetBodyPhase()
	elseif eEquipPart == EEquipPart.eHead then
		local bShowArmet = Player.m_Properties:GetShowArmet()
		if bShowArmet then
			return Player.m_Properties:GetHeadPhase()
		else
			return 0
		end
	elseif eEquipPart == EEquipPart.eShoulder then
		return Player.m_Properties:GetShoulderPhase()
	elseif eEquipPart == EEquipPart.eHand then
		return Player.m_Properties:GetArmPhase()
	elseif eEquipPart == EEquipPart.eShoe then
		return Player.m_Properties:GetShoePhase()
	end	
end

ERace = {}
ERace.Human = "人类" 
ERace.Dwarf = "矮人"
ERace.Elf   = "精灵" 	
ERace.Orc   = "兽人" 

function ClassToRace(Class)
	if Class == EClass.eCL_Warrior or  Class == EClass.eCL_MagicWarrior or Class == EClass.eCL_Paladin or 
		 Class == EClass.eCL_NatureElf or Class == EClass.eCL_EvilElf or Class == EClass.eCL_Priest then
		return ERace.Human
	elseif Class == EClass.eCL_ElfHunter then
		return ERace.Elf
	elseif Class == EClass.eCL_DwarfPaladin then
		return ERace.Dwarf
	elseif Class == EClass.eCL_OrcWarrior then
		return ERace.Orc
	end
end

function GetClassNameStr(uClass)
	local ClassName
	if uClass == EClass.eCL_Warrior  then
		ClassName = "djs"
	elseif uClass == EClass.eCL_MagicWarrior then
		ClassName = "mjs"
	elseif	 uClass == EClass.eCL_Paladin then
		ClassName = "qs"
	elseif	 uClass == EClass.eCL_NatureElf then
		ClassName = "fs"
	elseif	 uClass == EClass.eCL_EvilElf then
		ClassName = "xm"
	elseif	 uClass == EClass.eCL_Priest then
		ClassName = "ms"
	elseif uClass == EClass.eCL_ElfHunter then
		ClassName = "djs"
	elseif uClass == EClass.eCL_DwarfPaladin then
		ClassName = "djs"
	elseif uClass == EClass.eCL_OrcWarrior then
		ClassName = "sr"
   	end
   	return ClassName
end

function GetNewRoleFxName(uClass, uSex)
	local FxFilePath = "fx/setting/xuanrenjiemian/"
	local ClassName = GetClassNameStr(uClass)
   	return 	FxFilePath..ClassName.."/"..MakeAniFileName(uClass, uSex, nil)..".efx"
end

function split (s, delim)
	assert (type (delim) == "string" and string.len (delim) > 0, "bad delimiter")
	local start = 1
	local t = {}  -- results table
	
	if string.sub(s,1,1) == "\"" and string.sub(s,-1) == "\"" then
		s = string.sub(s,2,string.len(s)-1)
	end
	
	while true do
		local pos = string.find (s, delim, start, true) -- plain find
		if not pos then
			break
		end
		table.insert (t, string.sub (s, start, pos - 1))
		start = pos + string.len (delim)
	end -- while
	
	-- insert final one (after last delimiter)
	table.insert (t, string.sub (s, start))
	return t
end

function GetModelName(ModelFileName)
	if ModelFileName == "" or ModelFileName ==nil then
		return ""
	end 
	local temp1,temp2
	temp1 = string.gsub(ModelFileName,"/",",")
	temp2 = string.gsub(temp1,".mod","")
	local Table = split(temp2, ",")
	return Table[table.getn(Table)]
end

-- 动作包名拼接
function MakeAniFileName( uClass, uSex, AniType )
	local strRaceSexType = GetRaceSexTypeString( uClass, uSex )
	local strAniType = ""
	if AniType ~= nil then
		local AniTypeTable = split(AniType, ",")
		strAniType = AniTypeTable[1]
		if table.getn(AniTypeTable) > 1 then
			if strAniType=="wf" then
				local strPlayType = AniTypeTable[2]
				if strPlayType == "" or strPlayType == nil then
					print("AniType Error")
				else 
					return strAniType.."/"..strRaceSexType .."_"..strAniType.."_"..strPlayType
				end
			elseif strAniType == "jt" then
				local strHorseType = AniTypeTable[2]
				if strHorseType == "" or strHorseType == nil then
					print("AniType Error")
				else 
					return strAniType.."/"..strRaceSexType.."_"..strHorseType
				end
			elseif strAniType == "zq" then
				local strHandType = GetHandTypeString(AniTypeTable[2], uClass)
				local strHorseType = AniTypeTable[3]
				return strRaceSexType.."_"..strHorseType.."_"..strHandType
			end
		end
	end 
	
	local strClassType = GetClassTypeSting(uClass)	
	local strHandType = GetHandTypeString(strAniType, uClass)
	strAniFileName = strRaceSexType .."_"..strClassType.."_"..strHandType
	return strAniFileName
end

-- 得到"种族性别"（动作包名第一级字段）
function GetRaceSexTypeString(uClass, uSex)
    local strRace, strSex
	if EClass.eCL_Warrior == uClass or EClass.eCL_NatureElf == uClass or EClass.eCL_MagicWarrior == uClass or
		EClass.eCL_Priest == uClass or EClass.eCL_EvilElf == uClass or EClass.eCL_Paladin == uClass then
		strRace = "rl"		-- 人类
	elseif EClass.eCL_OrcWarrior == uClass then 
		strRace = "sr"		-- 兽人
--	elseif EClass.eCL_DwarfPaladin == uClass then
--		strRace = "ar"
--	elseif EClass.eCL_ElfHunter == uClass then
--		strRace = "jl"		
	else
		strRace = "rl"
		--LogErr("帮助信息，传递给动作拼接函数的[职业]参数有误，怀疑是varient出错！" .. uClass )	
	end
	
	if ECharSex.eCS_Male == uSex then
		strSex="m"			-- 男
	elseif ECharSex.eCS_Female == uSex then
		strSex="w"			-- 女
	else
		strSex="m"
		--LogErr("帮助信息，传递给动作拼接函数的[性别]参数有误，怀疑是varient出错！" .. uSex )
	end
	
	return strRace..strSex
end

-- 得到"职业类型"（动作包名第二级字段）
function GetClassTypeSting( uClass )
	local strClassType = ""
	if EClass.eCL_Warrior == uClass or EClass.eCL_MagicWarrior == uClass or EClass.eCL_OrcWarrior == uClass then
		strClassType = "js"		-- 大剑士、魔剑士、兽人战士
	elseif EClass.eCL_NatureElf == uClass or EClass.eCL_EvilElf == uClass or EClass.eCL_Priest == uClass then
		strClassType = "fs"		-- 法师、牧师、邪魔
	elseif EClass.eCL_Paladin == uClass then
		strClassType = "qs"		-- 人骑、
--	elseif EClass.eCL_DwarfPaladin == uClass then
--		strClassType = "qs"		-- 矮骑
--	elseif EClass.eCL_ElfHunter == uClass then
--		strClassType = "gs"		-- 精灵弓箭手
	else
		strClassType = "js"
		--LogErr("帮助信息，传递给动作拼接函数的[职业]参数有误，怀疑是varient出错！" .. uClass )
	end
	
	return strClassType
end

-- 得到"单双持类型"（动作包名第二级字段）
function GetHandTypeString(strAniType, uClass)
	local result, HandType = g_ItemInfoMgr:EquipTypeCheck(strAniType, uClass)
	if HandType ~= nil then
		if HandType == "双" then
			strHandType="ss"
		else
			strHandType="ds"
		end	
	else
		strHandType="ds"		-- 空手动作放在单手动作包中
	end
	return strHandType
end
