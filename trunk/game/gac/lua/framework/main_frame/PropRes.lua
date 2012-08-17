--加载资源
cfg_load "model_res/ChangeBody_Common"
cfg_load "model_res/NpcRes_Client"
cfg_load "model_res/NpcRes_Common"
cfg_load "model_res/IntObjRes_Client"

local function PrintLogErr(IsError, PrintInfo)
	if not IsError then
		CfgLogErr(PrintInfo)
	end
end

--坐骑
function InitHorseRender(RenderObject, HorseResID, uClass, uSex)
	if HorseResID ~= 0 and HorseResID ~= "" then
		RenderObject:RemoveAllPiece()
		RenderObject:Delframework()
		local HorseRes = HorseRes_Common(g_sParseHorseTblClient[HorseResID])
		if HorseRes ~= nil then
			local Size
			if uClass == EClass.eCL_DwarfPaladin then
				Size = HorseRes("DwarfSize")
			elseif uClass == EClass.eCL_ElfHunter then
				Size = HorseRes("ElfSize")
			elseif uClass == EClass.eCL_OrcWarrior then
				Size = HorseRes("OrcSize")
			else
				if uSex == ECharSex.eCS_Male then
					Size = HorseRes("HumanMaleSize")
				else
					Size = HorseRes("HumanFeMaleSize")
				end
			end
			RenderObject:AddPiece( RES_TYPE.RFT_ARA, HorseRes("AniFile"), "", 0 )
			RenderObject:AddPiece( RES_TYPE.RFT_ARP, HorseRes("ModelFile"), "body", HorseRes("RenderStyle"))
			if Size ~= nil and Size ~= ""  and Size ~= 0 then
				RenderObject:SetSize(Size)
			end
		end
	end
end

--变身
function InitChangeBodyRender(RenderObject, tblResStr)
	assert(IsTable(tblResStr))
	for i = 1, #tblResStr do
		local ResStr = tblResStr[i]
		if ResStr ~= "" and ChangeBody_Common(ResStr)~= nil then
			local ChangeBodyRes = ChangeBody_Common(ResStr)
			RenderObject:AddPiece( RES_TYPE.RFT_ARA, ChangeBodyRes("AniFile"), "", 0 )
			RenderObject:AddPiece( RES_TYPE.RFT_ARP, ChangeBodyRes("ModelFile"), ChangeBodyRes("PieceName"), ChangeBodyRes("RenderStyle") )
		end
	end
end

function InitWeaponRender(RenderObject, WeaponResID, uClass, uSex)
	if WeaponResID == 0 and WeaponsResID == "" then
		return
	end	
	local WeaponRes = WeaponRes_Common(g_sParseWeaponTblClient[WeaponResID])
	if WeaponRes == nil then
		return
	end
	local ModelFileName, AniFileName, EffectFile, EffectFileName, EffectName= "","", "", "", ""
	local PieceName = WeaponRes("PieceName")
	if uClass == EClass.eCL_DwarfPaladin then
		ModelFileName	= WeaponRes("DwarfModelFile")
		EffectFile = WeaponRes("DwarfEffectFile") 
	elseif uClass == EClass.eCL_ElfHunter then
		ModelFileName	= WeaponRes("ElfModelFile")
		EffectFile = WeaponRes("ElfEffectFile") 
	elseif uClass == EClass.eCL_OrcWarrior then
		ModelFileName	= WeaponRes("OrcModelFile")
		EffectFile = WeaponRes("OrcEffectFile") 
	else
		if uSex == ECharSex.eCS_Male then
			ModelFileName	= WeaponRes("HumanMaleModelFile")
			EffectFile = WeaponRes("HumanMaleEffectFile") 
		else
			ModelFileName	= WeaponRes("HumanFeMaleModelFile")
			EffectFile = WeaponRes("HumanFeMaleEffectFile") 
		end
	end
	
	if  "" ~= EffectFile then
		local EffectTable =  split(EffectFile, ",")
		EffectFileName = EffectTable[1]
		EffectName = EffectTable[2]
	end
	
	if RenderObject.weaponEffect ~= nil then
		if RenderObject.weaponRenderObj ~= nil then
			RenderObject.weaponRenderObj:DelEffect(RenderObject.weaponEffect)
		else
			RenderObject:DelEffect(RenderObject.weaponEffect)
		end
		RenderObject.weaponEffect=nil
	end
	if RenderObject.offweaponEffect ~= nil then
		if RenderObject.offweaponRenderObj~=nil then
			RenderObject.offweaponRenderObj:DelEffect(RenderObject.offweaponEffect)
		else
			RenderObject:DelEffect(RenderObject.offweaponEffect)
		end
		RenderObject.offweaponEffect=nil
	end
		
	if WeaponRes("AniFile") ~= "" then		-- Transform 是否变形
		assert(WeaponRes("BindBone"))
		local AniTable = split(WeaponRes("AniFile"), ",")
		AniFileName = AniTable[1]..GetRaceSexTypeString(uClass, uSex)..AniTable[2]
		if PieceName == "weapon" and RenderObject.weaponRenderObj ~= nil then		
			RenderObject.WeaponBindBone = WeaponRes("BindBone")
			RenderObject.weaponRenderObj:AddPiece( RES_TYPE.RFT_ARA, AniFileName, PieceName, 0 )
			RenderObject.weaponRenderObj:AddPiece( RES_TYPE.RFT_ARP, ModelFileName, PieceName, WeaponRes("RenderStyle"))
		elseif PieceName == "offweapon" and RenderObject.offweaponRenderObj ~= nil then
			RenderObject.OffWeaponBindBone = WeaponRes("BindBone")
			RenderObject.offweaponRenderObj:AddPiece( RES_TYPE.RFT_ARA, AniFileName, PieceName, 0 )
			RenderObject.offweaponRenderObj:AddPiece( RES_TYPE.RFT_ARP, ModelFileName, PieceName, WeaponRes("RenderStyle"))
		end
	else
		RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFileName, PieceName, WeaponRes("RenderStyle"))
	end
	
	if EffectFileName~="" then
		if PieceName == "weapon" then
			if  RenderObject.weaponRenderObj ~= nil then
				RenderObject.weaponeffectname = EffectFileName..",".. EffectName
				RenderObject.weaponEffect=RenderObject.weaponRenderObj:AddEffect( EffectFileName, EffectName, -1,  nil )
			else
				RenderObject.weaponEffect=RenderObject:AddEffect( EffectFileName, EffectName, -1, nil )
			end
		elseif PieceName == "offweapon" then
			if RenderObject.offweaponRenderObj ~= nil then
				RenderObject.offweaponeffectname = EffectFileName..",".. EffectName
				RenderObject.offweaponEffect=RenderObject.offweaponRenderObj:AddEffect( EffectFileName, EffectName, -1, nil)
			else
				RenderObject.offweaponEffect=RenderObject:AddEffect( EffectFileName, EffectName, -1, nil )
			end
		end
	end	
end

function InitPlayerCustomRes( RenderObject, tblResID, uClass, uSex)
	for i,p in pairs(tblResID) do
		local nResID = tblResID[i]	
		local PropRes = CustomRes_Common(g_ParseCustomTblClient[nResID])
		local ModelFileName= ""
		if nResID ~= 0 and nResID ~= "" and PropRes ~= nil then
			local PieceName = PropRes("PieceName")
			if uClass == EClass.eCL_DwarfPaladin then
				ModelFileName	= PropRes("DwarfModelFile")
			elseif uClass == EClass.eCL_ElfHunter then
				ModelFileName	= PropRes("ElfModelFile")
			elseif uClass == EClass.eCL_OrcWarrior then
				ModelFileName	= PropRes("OrcModelFile")
			else
				if uSex == ECharSex.eCS_Male then
					ModelFileName	= PropRes("HumanMaleModelFile")
				else
					ModelFileName	= PropRes("HumanFeMaleModelFile")
				end
			end
			RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFileName, PieceName, PropRes("RenderStyle"))
		end
	end
end

function InitPlayerModel( RenderObject, tblResName, uClass, uSex, uCamp)
	local AllHideTbl = {}
	local tblRes={}
	
	if g_GameMain and ((g_GameMain.m_TongBattlePersonalCountWnd and g_GameMain.m_TongBattlePersonalCountWnd:IsShow())
		or g_GameMain.m_SceneName == "古战场") then
		if uCamp == 1 then 
			table.insert(tblRes,"艾米国战装")
		elseif uCamp == 2 then 
			table.insert(tblRes,"神圣国战装")
		elseif uCamp == 3 then
			table.insert(tblRes,"修斯国战装")
		else 
			tblRes= tblResName	
		end
		
	else 
		tblRes= tblResName
	end
	for i,p in pairs(tblRes) do
		local strRes = tblRes[i]	
		if strRes ~= nil then
			local ModelInfo = CModelInfoClient_GetModelInfo(strRes,uClass,uSex)
			if ModelInfo ~= nil then 
				local ModelFileName = ModelInfo:GetModelFileName()
				local EffectFile = ModelInfo:GetFXFileName()
				local PieceName = ModelInfo:GetPieceName()
				local StyleName = ModelInfo:GetStyleName()
				local HidePiece = ModelInfo:GetHidePiece()
				if  ModelFileName ~= "" then
					RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFileName, PieceName, StyleName)
				end
				-- 特效		
				if  EffectFile ~=""  then
					if RenderObject.tblPreEffect == nil then
						RenderObject.tblPreEffect = {}
					end
					RenderObject.tblPreEffect[PieceName] = EffectFile
				end
				
				if  HidePiece ~=""  then
					local HideTbl =  split(HidePiece, ",")
					for _,piecename in pairs(HideTbl) do
						table.insert(AllHideTbl, piecename)
					end
				end
			end
		end
	end
	for _,PieceName in pairs(AllHideTbl) do
		RenderObject:ShowPiece(PieceName, false)
		if RenderObject.tblEffect ~= nil then
			local EffectID = RenderObject.tblEffect[PieceName]
			RenderObject:DelEffect(EffectID)
			RenderObject.tblEffect[PieceName] = nil
		end
		if RenderObject.tblPreEffect ~= nil then
			RenderObject.tblPreEffect[PieceName] = nil
		end
	end
end

function InitPlayerEquipPhaseFX(RenderObject, tblEquipPhase)
	if RenderObject.tblEffect ~= nil then
	for PieceName,EffectIndex in pairs(RenderObject.tblEffect) do
		RenderObject:DelEffect(EffectIndex)
		RenderObject.tblEffect[PieceName] = nil
	end
	end
	local tblPreEffect = RenderObject.tblPreEffect 
	if  tblPreEffect == nil then
		return
	end
	for PieceName,EffectFile in pairs(tblPreEffect) do
		local EffectTable =  split(EffectFile, ",")
		local EffectFileName = EffectTable[1]
		local EffectName;
		local EquipPhase = tblEquipPhase[PieceName]
		if EquipPhase ~= nil and EquipPhase ~= 0 then
			EffectName = PieceName.."/"..EquipPhase
		else
			EffectName = EffectTable[2]
		end
		if EffectName ~= nil then
			local EffectID = RenderObject:AddEffect( EffectFileName, EffectName, -1, nil )
			if RenderObject.tblEffect == nil then
				RenderObject.tblEffect = {}
			end
			RenderObject.tblEffect[PieceName] = EffectID
		end
		RenderObject.tblPreEffect[PieceName] = nil
	end
end

function InitPlayerPiecePhaseFX(RenderObject,PieceName ,EquipPhase)
	if RenderObject.tblEffect ~= nil then
	local EffectID =	 RenderObject.tblEffect[PieceName]
		if EffectID and EffectID~=0 then 
			RenderObject:DelEffect(EffectID)
			RenderObject.tblEffect[PieceName] = nil
		end
	end
	local tblPreEffect = RenderObject.tblPreEffect 
	if  tblPreEffect == nil then
		return
	end
	local EffectFile = tblPreEffect[PieceName]
	if EffectFile then
		local EffectTable =  split(EffectFile, ",")
		local EffectFileName = EffectTable[1]
		local EffectName;
		if EquipPhase ~= nil and EquipPhase ~= 0 then
			EffectName = PieceName.."/"..EquipPhase
		else
			EffectName = EffectTable[2]
		end
		if EffectName ~= nil then
			local EffectID = RenderObject:AddEffect( EffectFileName, EffectName, -1, nil )
			if RenderObject.tblEffect == nil then
				RenderObject.tblEffect = {}
			end
			RenderObject.tblEffect[PieceName] = EffectID
		end
		RenderObject.tblPreEffect[PieceName] = nil
	end
end
-- 一级动作包
function InitPlayerBaseAni(RenderObject, uClass, uSex, HorseType)
	assert(IsNumber(uSex))
	local AniFilePath = "character/ani/zhujue/"
	-- 第一级
	local AniFileName = AniFilePath..GetRaceSexTypeString(uClass, uSex)..".ske"
	RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFileName, "", 0 )
	
	-- 第二级
	InitPlayerCommonAni(RenderObject, uClass, uSex, HorseType)
end

-- 二级动作包
function InitPlayerCommonAni(RenderObject, uClass, uSex, HorseType)
	local AniFilePath = "character/ani/zhujue/"
	local AniFileName = ""
	if HorseType ~= nil and HorseType ~= "" then
		AniFileName = AniFilePath..GetRaceSexTypeString(uClass, uSex).."_"..HorseType..".ske"
	else
		AniFileName = AniFilePath..GetRaceSexTypeString(uClass, uSex).."_"..GetClassTypeSting(uClass)..".ske"
	end
	RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFileName, "", 0 )
end

-- 三级动作包
function InitPlayerFinalAni(RenderObject, uClass, uSex, AniType)
	local AniFilePath = "character/ani/zhujue/"
	local AniFileName = ""
	AniFileName = AniFilePath..MakeAniFileName(uClass, uSex, AniType)..".ske"
	RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFileName, "", 0 )
end

-- 创建角色动作包
function InitCreatePlayerAni(RenderObject, uClass, uSex)
	local AniFilePath = "character/ani/xuanrenjiemian/"
	local AniFileName = ""
	local ClassName = GetClassNameStr(uClass)
	AniFileName = AniFilePath..ClassName.."/"..MakeAniFileName(uClass, uSex, nil)..".ske"
	RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFileName, "", 0 )
end

function InitCustomCharPortrait(RenderObject, tblResID, uClass, uSex)
	if uClass == EClass.eCL_OrcWarrior then
			uClass = uClass
		else
			uClass = EClass.eCL_Warrior 
	end
	InitPlayerCustomRes(RenderObject, tblResID, uClass, uSex)
end

--角色头像
function InitCharPortrait(RenderObject, tblRes, uClass, uSex)
	if uClass == EClass.eCL_OrcWarrior then
			uClass = uClass
		else
			uClass = EClass.eCL_Warrior 
	end
	InitPlayerModel(RenderObject, tblRes, uClass, uSex)
end

local function ObjectAddPiece(RenderObject, PartName, data)
	if PartName == "" or PartName == nil then
		return
	end
	local PartRes = NpcRes_Client(PartName)
	if PartRes == nil then
		return
	end
	local ModelFile = ""
	local Sex, AniFile = unpack(data)
	if Sex == ECharSex.eCS_Male then 
		ModelFile = PartRes("BoyModelFile")
	else
		ModelFile = PartRes("GirlModelFile")
	end
	local LoadAniSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFile, PartRes("PieceName"), 0 )
	local LoadModelSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFile, PartRes("PieceName"), PartRes("RenderStyle") )
	PrintLogErr(LoadAniSucc, PartName.."加载动作文件 "..AniFile.."时发生错误")
	PrintLogErr(LoadModelSucc, PartName.."加载模型文件 "..ModelFile.."时发生错误")
	local HidePiece_Str="{"..PartRes("HidePiece").."}"
	local HideTbl = loadstring("return " .. HidePiece_Str)()
	for i,p in pairs(HideTbl) do
		RenderObject:ShowPiece(p, false)
	end
end
local function ObjectAddPieceWithoutAni(RenderObject, PartName, Sex)
	if PartName == "" or PartName == nil then
		return
	end
	local PartRes = NpcRes_Client(PartName)
	if PartRes == nil then
		return
	end
	local ModelFile = ""
	if Sex == ECharSex.eCS_Male then 
		ModelFile = PartRes("BoyModelFile")
	else
		ModelFile = PartRes("GirlModelFile")
	end
	local LoadModelSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFile, PartRes("PieceName"), PartRes("RenderStyle") )
	PrintLogErr(LoadModelSucc, PartName.."加载模型文件 "..ModelFile.."时发生错误")
	local HidePiece_Str="{"..PartRes("HidePiece").."}"
	local HideTbl = loadstring("return " .. HidePiece_Str)()
	for i,p in pairs(HideTbl) do
		RenderObject:ShowPiece(p, false)
	end
end

function InitRenderNpc(RenderObject, CharacterName, uSex )
	assert(IsNumber(uSex))
	RenderObject:RemoveAllPiece()
	RenderObject:Delframework()
	local CharacterRes = NpcRes_Common(CharacterName)
	PrintLogErr(CharacterRes, "在Npc_ResCommon表中查找不到ObjectName:"..CharacterName)
	local AniFile = "character/ani/npc/" .. CharacterRes("AniFileName") .. ".ske"
	-- 只要加一次动作包
	local LoadAniSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFile, "", 0 )
	PrintLogErr(LoadAniSucc, CharacterName.."加载动作文件 "..AniFile.."时发生错误")
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Face"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Arm"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Shoe"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Weapon"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Caestus"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Shoulder"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Hair"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Back"), uSex)
	--这里hat一定要最后AddPiece 考虑Head会HidePiece的问题 还有以后角色换装问题
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Head"), uSex)
	--这里body一定要最后AddPiece 考虑body会HidePiece的问题 还有以后角色换装问题
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Body"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Armet"), uSex)
	
	if CharacterRes("Scaling") == "" then
		return
	end
	RenderObject:SetScale(CharacterRes("Scaling"))
end

--战斗坐骑
function InitBattleHorsePortrait(RenderObject, CharacterName, uSex)
	assert(IsNumber(uSex))
	RenderObject:RemoveAllPiece()
	RenderObject:Delframework()
	local CharacterRes = NpcRes_Common(CharacterName)
	assert(CharacterRes, "NpcRes_Common:"..CharacterName)
	local AniFileName = CharacterRes("AniFileName") .. ".ske"
	local LoadAniSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFileName, "", 0 )
	PrintLogErr(LoadAniSucc, CharacterName.."加载动作文件 "..AniFileName.."时发生错误")
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Body"), uSex)
	RenderObject:DoAni( "stand01", true, -1.0 )
end

--NPC头像
function InitNpcPortrait(RenderObject, CharacterName, uSex)
	assert(IsNumber(uSex))
	RenderObject:RemoveAllPiece()
	RenderObject:Delframework()
	local CharacterRes = NpcRes_Common(CharacterName)
	PrintLogErr(CharacterRes, "在NpcRes_Common表中查找不到ObjectName:"..CharacterName)
	local AniFile = "character/ani/npc/" .. CharacterRes("AniFileName") .. ".ske"
	--只要加一次动作包
	local LoadAniSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFile, "", 0 )
	PrintLogErr(LoadAniSucc, CharacterName.."加载动作文件 "..AniFile.."时发生错误")
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Face"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Hair"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Head"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Armet"), uSex)
	ObjectAddPieceWithoutAni(RenderObject, CharacterRes("Body"), uSex)
	RenderObject:DoAni( "stand01", true, -1.0 )
--	if CharacterRes("Scaling") == "" then
--		return
--	end
end

function IntObjLoadModel(RenderObject, ModelName)
	local IntObjRes = IntObjRes_Client(ModelName)
	PrintLogErr(IntObjRes, "在IntObj_Client表中查找不到IntObjName:"..ModelName)
	local ModelFile = IntObjRes("ModelFile")
	local AniFile = IntObjRes("AniFile")
	local PieceName = IntObjRes("PieceName")
	local LoadAniSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFile, "", 0 )
	local LoadModelSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFile, "", IntObjRes("RenderStyle") )
	PrintLogErr(LoadAniSucc, ModelName.."加载动作文件 "..AniFile.."时发生错误")
	PrintLogErr(LoadModelSucc, ModelName.."加载模型文件 "..ModelFile.."时发生错误")
	RenderObject:SetScale( IntObjRes("Scaling") )
end


function TrapLoadModel(RenderObject, ModelName)
	if ModelName == " " or ModelName == nil or ModelName == "" then
		return
	end
	
	local TrapRes = IntObjRes_Client(ModelName)
	PrintLogErr(TrapRes, "在scene/Trap_Common表中查找不到TrapName:"..ModelName)
	local ModelFile = TrapRes("ModelFile")
	if ModelFile == nil or ModelFile == "" then
		return
	end
	local AniFile = TrapRes("AniFile")
	local PieceName = TrapRes("PieceName") --"Body"
	local RenderStyle = TrapRes("RenderStyle") --"默认0"
	local LoadAniSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARA, AniFile, "", 0 )
	local LoadModelSucc = RenderObject:AddPiece( RES_TYPE.RFT_ARP, ModelFile, "", RenderStyle)
	PrintLogErr(LoadAniSucc, "陷阱:"..ModelName.."加载动作文件 "..AniFile.."时发生错误")
	PrintLogErr(LoadModelSucc, "陷阱:"..ModelName.."加载模型文件 "..ModelFile.."时发生错误")
	RenderObject:SetScale( TrapRes("Scaling") )
end

function PrepareWeapon(RenderObj,WeaponResID)
	if WeaponResID == 0 and WeaponsResID == "" then
		return
	end	
	local WeaponRes = WeaponRes_Common(g_sParseWeaponTblClient[WeaponResID])
	if WeaponRes == nil then
		return
	end
	
	if WeaponRes("AniFile") ~= "" then		-- Transform 是否变形
		RenderObj.weaponRenderObj = RenderObj:CreateRenderObject(eLinkType.LT_CENTER,"")
	elseif RenderObj.weaponRenderObj ~= nil  then
		RenderObj:DestroyRenderObject(RenderObj.weaponRenderObj)
		RenderObj.weaponRenderObj = nil
	end
end

function PrepareOffWeapon(RenderObj,OffWeaponResID)
	if OffWeaponResID == 0 and OffWeaponResID == "" then
		return
	end	
	local OffWeaponRes = WeaponRes_Common(g_sParseWeaponTblClient[OffWeaponResID])
	if OffWeaponRes == nil then
		return
	end
	
	if OffWeaponRes("AniFile") ~= "" then		-- Transform 是否变形
		RenderObj.offweaponRenderObj = RenderObj:CreateRenderObject(eLinkType.LT_CENTER,"")
	elseif RenderObj.offweaponRenderObj ~= nil  then
		RenderObj:DestroyRenderObject(RenderObj.offweaponRenderObj)
		RenderObj.offweaponRenderObj = nil
	end
end

function SetDirection(RenderObject,uDesDirection,bAtOnce)
	if IsCppBound(RenderObject) ~= true then
		return
	end
	local Dir = CDir:new()
	RenderObject:GetDirection(Dir)
	local uCurDirection = Dir.uDir
	local uHalfDesDirection = math.abs(uDesDirection - uCurDirection)/2
	if uHalfDesDirection > 64 then
		uHalfDesDirection = 128 - uHalfDesDirection
	end
	local uTurnTimes
	if uHalfDesDirection <= 33 then
		uTurnTimes = 6;
	else
		uTurnTimes = 10;
	end

	local uTurnSpeed = math.abs((uDesDirection - uCurDirection )/uTurnTimes)

	local function TurnOnTick(Tick, RenderObject,uDesDirection,uTurnSpeed)
		if IsCppBound(RenderObject) ~= true then
			if RenderObject~=nil and RenderObject.TurnOnTick~=nil then
				UnRegisterTick( RenderObject.TurnOnTick )
			end
			return
		end
		RenderObject:GetDirection(Dir)
		local uCurDirection = Dir.uDir
		if uCurDirection ~= uDesDirection then
			local	nDel = '0'
			nDel =uDesDirection - uCurDirection
			if	math.abs( nDel ) <= uTurnSpeed or 0 == uTurnSpeed then
				uCurDirection = uDesDirection;
			elseif 	nDel > 0 and nDel < 128 then
				uCurDirection = uCurDirection + uTurnSpeed 
			else
				uCurDirection = uCurDirection - uTurnSpeed
			end
			RenderObject:SetDirection( CDir:new( uCurDirection ) )
		else
			if RenderObject.TurnOnTick~=nil then
				UnRegisterTick( RenderObject.TurnOnTick )
			end
		end
	end

	if bAtOnce then
		RenderObject:SetDirection( CDir:new( uDesDirection ))
	else
		RenderObject.TurnOnTick = RegisterTick( "TurnOnTick", TurnOnTick,33,RenderObject,uDesDirection,uTurnSpeed)
	end
end
