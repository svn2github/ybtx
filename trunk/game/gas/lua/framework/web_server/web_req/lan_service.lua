local function Lan_Service_Load_Lan(Lang)
	--物品名称
	local tbl = {}
	local CItemTable = CItemTableXml:new()
	local ItemNumber = CItemTable:GetItemXmlNumber()
	for i=0,ItemNumber-1 do
		local index = CItemTable:GetItemXmlBigId(i)
		local str = "item/"..CItemTable:GetItemXmlLanName(i)
		tbl[index] = mult_lan_load(Lang, str)
	end
	if _G["Lan_"..Lang] == nil then
		_G["Lan_"..Lang] = {}
	end
	_G["Lan_"..Lang]["Lan_ItemType_Common"] = tbl

	--任务名称
	local tbl = {}
	for i=1, 3 do
		table.insert(tbl, "quest/Lan_QuestAll_CommonA"..i)
		table.insert(tbl, "quest/Lan_QuestAll_CommonB"..i)
		table.insert(tbl, "quest/Lan_QuestAll_CommonC"..i)
	end
	
	table.insert(tbl,   "quest/Lan_CallBack_Quest_Common")
	table.insert(tbl,   "quest/Lan_ZhiYin_Quest_Common")
	
	table.insert(tbl,   "quest/Lan_Hide_Quest_Common")
	table.insert(tbl,   "quest/Lan_TreeBar_Quest_Common")
	table.insert(tbl,   "quest/Lan_HeYangDaCaoYuan_Quest_Common")
	table.insert(tbl,   "quest/Lan_ShengXueShan_Quest_Common")
	table.insert(tbl,   "quest/Lan_ShiZiHeGu_Quest_Common")
	table.insert(tbl,   "quest/Lan_XiLinDao_Quest_Common")
	table.insert(tbl,   "quest/Lan_MercenaryQuest_Direct_Common")
	table.insert(tbl,   "quest/Lan_MercenaryQuest_XiLinDao_CommonA")
	table.insert(tbl,   "quest/Lan_MercenaryQuest_XiLinDao_CommonB")
	table.insert(tbl,   "quest/Lan_MercenaryQuest_XiLinDao_CommonC")
	
	for i=1, 5 do
		table.insert(tbl, "quest/Lan_MercenaryQuest_CommonA"..i)
		table.insert(tbl, "quest/Lan_MercenaryQuest_CommonB"..i)
		table.insert(tbl, "quest/Lan_MercenaryQuest_CommonC"..i)
	end
	
	for i=1, 3 do
		table.insert(tbl, "quest/Lan_DareQuest_Common"..i)
		table.insert(tbl, "quest/Lan_ActionQuest_Common"..i)
		table.insert(tbl, "quest/Lan_LoopQuest_Common"..i)
	end
	mult_lan_load_list(Lang,"Lan_QuestCommon", unpack(tbl))
	
	--装备套装信息
	mult_lan_load(Lang, "equip/suit/Lan_EquipSuit_Common")
	mult_lan_load(Lang, "equip/suit/Lan_SuitPart_Client")
	mult_lan_load(Lang, "equip/intensify/Lan_SuitPartMapTalentOrSkill_Common")
	
	--装备属性信息
	mult_lan_load(Lang, "equip/Lan_PropertyName_Common")
	
	--装备精灵技能
	mult_lan_load(Lang, "equip/advance/Lan_AttributeOfJingLingSkill_Common")
	
	--场景信息
	mult_lan_load(Lang, "scene/Lan_Scene_Basic_Common")
	
	--技能信息
	mult_lan_load(Lang, "skill/Lan_SkillPart_Common")
	mult_lan_load(Lang, "skill/Lan_NonFightSkill_Common")
	mult_lan_load(Lang, "skill/Lan_SkillName_Client")
	local tbl = {
		[1]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common1"),
		[2]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common2"),
		[3]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common3"),
		[4]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common4"),
		[5]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common5"),
		[6]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common6"),
		[7]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common7"),
		[8]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common8"),
		[9]  = mult_lan_load(Lang, "skill/Lan_GiftSkill_Common9"),
	}
	_G["Lan_"..Lang]["Lan_GiftSkill_Common"] = tbl
	
	--称谓
	mult_lan_load(Lang, "appellation/Lan_AppellationText_Common")
	
	--NPC
	mult_lan_load(Lang, "npc/Lan_Npc_Common")
	
	--佣兵小队科技
	mult_lan_load(Lang, "tong/Lan_TongFightTech_Common")
	
	--职业
	mult_lan_load(Lang, "player/Lan_Class_Common")
end

local function Lan_service_Load_Cfg()
	--装备套装信息
	cfg_load "equip/suit/EquipSuit_Common"
	cfg_load "equip/suit/GodWarSuitNeedEquipPart_Client"
	cfg_load "equip/suit/MagicActSuitNeedEquipPart_Client"
	cfg_load "equip/intensify/SuitPartMapTalentOrSkill_Common"
	
	--装备护甲片信息
	cfg_load "item/ArmorPieceEnactment_Common"
	
	--装备精灵技能
	cfg_load "equip/advance/JingLingTypeToPieceSkill_Common"
	cfg_load "equip/advance/AttributeOfJingLingSkill_Common"
	
	--称谓
	cfg_load "appellation/AppellationIdAndText_Common"
	
	--场景信息
	gac_gas_require "areamgr/AreaMgr"
end

local function CallBack(rs)
	for _, v in pairs(rs) do
		Lan_Service_Load_Lan(v)
	end
end
CallDbTrans("GMToolsDbActionDB", "GetGMSLangs", CallBack, {})
Lan_service_Load_Cfg()

function Item_GetDisplayName(_req)
	local Lang = _req["lang"]
	local Type = tonumber(_req["type"])
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	if Type == nil then
		for i, v in pairs(Lan.Lan_ItemType_Common) do
			local DisplayName = v(MemH64(Name),"DisplayName")
			if DisplayName ~= nil then
				Ret = Ret .. "<Item>" .. DisplayName .. "</Item>\n"
				break
			end
		end
	else
		if Lan.Lan_ItemType_Common[Type] then
			local DisplayName = Lan.Lan_ItemType_Common[Type](MemH64(Name),"DisplayName")
			if DisplayName ~= nil then
				Ret = Ret .. "<Item>" .. DisplayName .. "</Item>\n"
			end
		end
	end

	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Item_GetRealName(_req)
	local Lang = _req["lang"]
	local Type = tonumber(_req["type"])
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	if Type == nil then
		for i, v in pairs(Lan.Lan_ItemType_Common) do
			local keys = v:GetKeys()
			for _, n in pairs(keys) do
				if Name == v(n,"DisplayName") then
					Ret = Ret .. "<Item>" .. MemH64ToMbcs(n) .. "</Item>\n"
				end
			end
		end
	else
		local v = Lan.Lan_ItemType_Common[Type]
		if v then
			local keys = v:GetKeys()
			for _, n in pairs(keys) do
				if Name == v(n,"DisplayName") then
					Ret = Ret .. "<Item>" .. MemH64ToMbcs(n) .. "</Item>\n"
				end
			end
		end
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Item_SearchByRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	for Type, Common in pairs(Lan.Lan_ItemType_Common) do
		local Str = "";
		local Keys = Common:GetKeys()
		for _, Base in pairs(Keys) do
			local RealName = MemH64ToMbcs(Base);
			if string.find(RealName, Name, 1, true) then
				Str = Str .. "<Item>\n<RealName>" .. Base .. "</RealName>\n<DisplayName>" .. Common(Base,"DisplayName") .. "</DisplayName>\n</Item>\n"
			end
		end
		if Str ~= "" then
			Ret = Ret .. "<Type id='" .. Type .. "'>\n" .. Str .. "</Type>\n"
		end
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Item_SearchByDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]

	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	for Type, Common in pairs(Lan.Lan_ItemType_Common) do
		local Str = "";
		local Keys = Common:GetKeys()
		for _, Base in pairs(Keys) do
			local DisplayName = Common(Base,"DisplayName");
			if string.find(DisplayName, Name, 1, true) then
				Str = Str .. "<Item>\n<RealName>" .. Base .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n</Item>\n"
			end
		end
		if Str ~= "" then
			Ret = Ret .. "<Type id='" .. Type .. "'>\n" .. Str .. "</Type>\n"
		end
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Quest_GetDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	local DisplayName = Lan.Lan_QuestCommon(MemH64(Name .. "任务名"),"NewContent")
	
	if DisplayName ~= nil then
		Ret = "<Quest>" .. DisplayName .. "</Quest>\n"
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Quest_GetRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	local v = Lan.Lan_QuestCommon
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		local longName = MemH64ToMbcs(n)
		if "任务名" == string.sub(longName,-6) then
			if Name == v(n,"NewContent") then
				Ret = Ret .. "<Quest>" .. string.sub(longName,1,-7) .. "</Quest>\n"
			end
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Quest_SearchByDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	local v = Lan.Lan_QuestCommon
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		local longName = MemH64ToMbcs(n)
		if "任务名" == string.sub(longName,-6) then
			local DisplayName = v(n,"NewContent")
			if string.find(DisplayName, Name, 1, true) then
				Ret = Ret .. "<Quest>\n<RealName>" .. MemH64(string.sub(longName,1,-7)) .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n</Quest>\n"
			end
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Quest_SearchByRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	local v = Lan.Lan_QuestCommon
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		local longName = MemH64ToMbcs(n)
		if "任务名" == string.sub(longName,-6) then
			local RealName = string.sub(longName,1,-7)
			if string.find(RealName, Name, 1, true) then
				local DisplayName = v(n,"NewContent")
				Ret = Ret .. "<Quest>\n<RealName>" .. MemH64(RealName) .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n</Quest>\n"
			end
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Equip_GetSkillName(_req)
	local Lang = _req["lang"]
	local Type = _req["type"]
	local Index = _req["index"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	local PieceIndex
	local pieceTitleStr = string.format("%s%d", "SkillPieceName", Index)
  local SkillPieceName = JingLingTypeToPieceSkill_Common(Type, pieceTitleStr)
	local Keys = AttributeOfJingLingSkill_Common:GetKeys()
	for _,v in pairs (Keys) do
		if SkillPieceName == AttributeOfJingLingSkill_Common(v,"SkillPieceName") then
			PieceIndex = v
			break
		end
	end
	if PieceIndex ~= nil then
		Ret = "<Skill>" .. Lan.Lan_AttributeOfJingLingSkill_Common(PieceIndex,"DisplaySkillPieceName") .. "</Skill>\n"
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Equip_GetPropertyName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local DisplayName = Lan.Lan_PropertyName_Common(MemH64(Name),"DisplayName")
	if DisplayName ~= nil then
		Ret = "<Property>" .. DisplayName .. "</Property>\n"
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Equip_GetSuitInfo(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	local Talent = _req["talent"] or ""
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	local DisplayName = ""
	
	equipPartInfo = GodWarSuitNeedEquipPart_Client:GetKeys(Name)
	SuitTbl = GodWarSuitNeedEquipPart_Client
	if equipPartInfo and table.maxn(equipPartInfo) == 0 then
		equipPartInfo = MagicActSuitNeedEquipPart_Client:GetKeys(Name)
		SuitTbl = MagicActSuitNeedEquipPart_Client
	end
	
	for i, v in pairs(equipPartInfo) do
		local Part = MemH64(SuitTbl(Name, v, "EquipPart"))
		if Part ~= nil then
			local Part_display = Lan.Lan_SuitPart_Client(Part,"DisplaySuitPartName")
			if Part_display ~= nil then
				Ret = Ret .. "<Part>" .. Part_display .. "</Part>\n"
			end
		end
	end
	
	local Keys = EquipSuit_Common:GetKeys(Name)
	for _, v in pairs(Keys) do
		local PartCount = tonumber(v)
		if PartCount == nil then
			PartCount = Lan.Lan_Class_Common(MemH64(v),"DisplayName")
		end
		local SuitName = MemH64(string.format("%s%s", Name, v))
		local Display = Lan.Lan_EquipSuit_Common(SuitName,"DisplayName")
		if Display ~= nil then
			DisplayName = "<Name>" .. Display .. "</Name>\n"
		end
		local DescInfo = loadstring("return {" .. EquipSuit_Common(Name, v, "SuitAttributeParameter") .. "}")()
		local Desc = GetStrAccordWildcard(Lan.Lan_EquipSuit_Common(SuitName,"DisplaySuitAttributeDescription"), DescInfo[1][2])
		if Desc ~= nil then
			Ret = Ret .. "<Desc number=\"" .. PartCount .. "\">" .. Desc .. "</Desc>\n"
		end
	end
	if DisplayName ~= "" then
		Ret = DisplayName .. Ret
	end
	if Talent ~= "" then
		local DescStr = Lan.Lan_SuitPartMapTalentOrSkill_Common(tonumber(Talent), "DisplaySuitPartAttributeDescription")
		local Value = SuitPartMapTalentOrSkill_Common(tonumber(Talent), "SuitPartAttributeParameter")
		if DescStr ~= nil and Value ~= nil then
			local Values = loadstring("return {" .. Value .. "}")()
			local Desc = GetStrAccordWildcard(DescStr, Values[1][2])
			if Desc ~= nil then
				Ret = Ret .. "<Talent>" .. Desc .. "</Talent>\n"
			end
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Equip_GetArmorPiece(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local AddTimes = ArmorPieceEnactment_Common(Name,"AddTimes")
	local EquipType = ArmorPieceEnactment_Common(Name,"EquipType")
	local PropertyValue = ArmorPieceEnactment_Common(Name,"PropertyValue")
	local DisplayName = Lan.Lan_PropertyName_Common(MemH64(EquipType),"DisplayName")
	if DisplayName ~= nil then
		Ret = "<Property>" .. DisplayName .. "</Property>\n"
	end
	if AddTimes ~= nil then
		Ret = Ret .. "<AddTimes>" .. AddTimes .. "</AddTimes>\n"
	end
	if PropertyValue ~= nil then
		Ret = Ret .. "<Value>" .. PropertyValue .. "</Value>\n"
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Scene_GetDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local DisplayName = Lan.Lan_Scene_Basic_Common(MemH64(Name),"DisplayName")
	if DisplayName ~= nil then
		Ret = "<Scene>" .. DisplayName .. "</Scene>\n"
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Scene_GetRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local v = Lan.Lan_Scene_Basic_Common
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		if Name == v(n,"DisplayName") then
			Ret = Ret .. "<Scene>" .. MemH64ToMbcs(n) .. "</Scene>\n"
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Scene_SearchByDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local v = Lan.Lan_Scene_Basic_Common
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		local DisplayName = v(n,"DisplayName")
		if string.find(DisplayName, Name, 1, true) then
			local SceneType = Scene_Basic_Common_Shell(MemH64ToMbcs(n),"SceneType")
			local InitPosX  = Scene_Basic_Common_Shell(MemH64ToMbcs(n),"InitPosX")
			local InitPosY  = Scene_Basic_Common_Shell(MemH64ToMbcs(n),"InitPosY")
			Ret = Ret .. "<Scene>\n<RealName>" .. n .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n<SceneType>" .. SceneType .. "</SceneType>\n<InitPosX>" .. InitPosX .. "</InitPosX>\n<InitPosY>" .. InitPosY .. "</InitPosY>\n</Scene>\n"
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Scene_SearchByRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local v = Lan.Lan_Scene_Basic_Common
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		local RealName = MemH64ToMbcs(n)
		if string.find(RealName, Name, 1, true) then
			local DisplayName = v(n,"DisplayName")
			local SceneType = Scene_Basic_Common_Shell(RealName,"SceneType")
			local InitPosX  = Scene_Basic_Common_Shell(RealName,"InitPosX")
			local InitPosY  = Scene_Basic_Common_Shell(RealName,"InitPosY")
			Ret = Ret .. "<Scene>\n<RealName>" .. n .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n<SceneType>" .. SceneType .. "</SceneType>\n<InitPosX>" .. InitPosX .. "</InitPosX>\n<InitPosY>" .. InitPosY .. "</InitPosY>\n</Scene>\n"
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Skill_GetDisplayName(_req)
	local Lang = _req["lang"]
	local skillname = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";

	local Lan_skill = Lan.Lan_SkillPart_Common(MemH64(skillname))
	local DisplayName = Lan_skill and Lan_skill("DisplayName")
	if DisplayName == nil then
		Lan_skill = Lan.Lan_NonFightSkill_Common(MemH64(skillname))
		DisplayName = Lan_skill and Lan_skill("DisplayName")
	end
	if DisplayName == nil then
		Lan_skill = Lan.Lan_SkillName_Client(MemH64(skillname))
		DisplayName = Lan_skill and Lan_skill("DisplayName")
	end
	if DisplayName == nil then
		for _, n in pairs(Lan.Lan_GiftSkill_Common) do
			Lan_skill = n(MemH64(skillname))
			DisplayName = Lan_skill and Lan_skill("DisplayName")
			if DisplayName ~= nil then
				break
			end
		end
	end
	if DisplayName == nil then
		Ret = ""
	else
		Ret = "<Skill>" .. DisplayName .. "</Skill>\n"
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Appellation(_req)
	local Lang = _req["lang"]
	local Id = tonumber(_req["id"])
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	local RealName = AppellationIdAndText_Common(Id, "Content")
	if RealName ~= nil then
		local DisplayName = Lan.Lan_AppellationText_Common(MemH64(RealName),"Content")
		if DisplayName ~= nil then
			Ret = "<Appellation>" .. DisplayName .. "</Appellation>\n"
		end
	end
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end  

function NPC_GetDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local DisplayName = Lan.Lan_Npc_Common(MemH64(Name),"Title")
	local Honour = Lan.Lan_Npc_Common(MemH64(Name),"NpcHonour")
	if DisplayName ~= nil then
		if Honour ~= nil and Honour ~= "" then
			DisplayName = DisplayName .. "(" .. Honour .. ")"
		end
		Ret = "<NPC>" .. DisplayName .. "</NPC>\n"
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function NPC_GetRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local v = Lan.Lan_Npc_Common
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		if Name == Lan.Lan_Npc_Common(n,"Title") then
			Ret = Ret .. "<NPC>" .. MemH64ToMbcs(n) .. "</NPC>\n"
		end
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function NPC_SearchByRealName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	
	local v = Lan.Lan_Npc_Common
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		if string.find(MemH64ToMbcs(n), Name, 1, true) then
			local DisplayName = Lan.Lan_Npc_Common(n,"Title")
			local Honour = Lan.Lan_Npc_Common(n,"NpcHonour")
			if Honour ~= nil and Honour ~= "" then
				DisplayName = DisplayName .. "(" .. Honour .. ")"
			end
			Ret = Ret .. "<NPC>\n<RealName>" .. n .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n</NPC>\n"
		end
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function NPC_SearchByDisplayName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = "";
	
	local v = Lan.Lan_Npc_Common
	local keys = v:GetKeys()
	for _, n in pairs(keys) do
		local DisplayName = Lan.Lan_Npc_Common(n,"Title")
		if string.find(DisplayName, Name, 1, true) then
			local Honour = Lan.Lan_Npc_Common(n,"NpcHonour")
			if Honour ~= nil and Honour ~= "" then
				DisplayName = DisplayName .. "(" .. Honour .. ")"
			end
			Ret = Ret .. "<NPC>\n<RealName>" .. n .. "</RealName>\n<DisplayName>" .. DisplayName .. "</DisplayName>\n</NPC>\n"
		end
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

function Tong_GetTechName(_req)
	local Lang = _req["lang"]
	local Name = _req["name"]
	
	local Lan = _G["Lan_"..Lang]
	
	local Ret = ""
	
	local DisplayName = Lan.Lan_TongFightTech_Common(MemH64(Name),"DisplayName")
	if DisplayName ~= nil then
		Ret = "<Tech>" .. DisplayName .. "</Tech>\n"
	end
	
	Ret = "<root>\n" .. Ret .. "</root>\n"
	return true, Ret
end

g_WebFuncDef["Lan_Item_GetDisplayName"] 			= Item_GetDisplayName
g_WebFuncDef["Lan_Item_GetRealName"] 					= Item_GetRealName
g_WebFuncDef["Lan_Item_GetByDisplayName"]			= Item_SearchByDisplayName
g_WebFuncDef["Lan_Item_GetByRealName"]				= Item_SearchByRealName
g_WebFuncDef["Lan_Quest_GetDisplayName"] 			= Quest_GetDisplayName
g_WebFuncDef["Lan_Quest_GetRealName"] 				= Quest_GetRealName
g_WebFuncDef["Lan_Quest_GetByDisplayName"]		= Quest_SearchByDisplayName
g_WebFuncDef["Lan_Quest_GetByRealName"]				= Quest_SearchByRealName
g_WebFuncDef["Lan_Equip_GetSkillName"] 				= Equip_GetSkillName
g_WebFuncDef["Lan_Equip_GetPropertyName"]			= Equip_GetPropertyName
g_WebFuncDef["Lan_Equip_GetSuitInfo"]					= Equip_GetSuitInfo
g_WebFuncDef["Lan_Equip_GetArmorPiece"]				= Equip_GetArmorPiece
g_WebFuncDef["Lan_Scene_GetDisplayName"]			= Scene_GetDisplayName
g_WebFuncDef["Lan_Scene_GetRealName"]					= Scene_GetRealName
g_WebFuncDef["Lan_Scene_GetByDisplayName"]		= Scene_SearchByDisplayName
g_WebFuncDef["Lan_Scene_GetByRealName"]				= Scene_SearchByRealName
g_WebFuncDef["Lan_Skill_GetDisplayName"]			= Skill_GetDisplayName
g_WebFuncDef["Lan_NPC_GetDisplayName"]				= NPC_GetDisplayName
g_WebFuncDef["Lan_NPC_GetRealName"]						= NPC_GetRealName
g_WebFuncDef["Lan_NPC_GetByDisplayName"]			= NPC_SearchByDisplayName
g_WebFuncDef["Lan_NPC_GetByRealName"]					= NPC_SearchByRealName
g_WebFuncDef["Lan_Appellation"]								= Appellation
g_WebFuncDef["Lan_Tong_GetTechName"]					= Tong_GetTechName
