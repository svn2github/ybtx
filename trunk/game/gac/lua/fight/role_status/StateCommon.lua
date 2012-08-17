
nAllPartNum = 13
------------------------------------------------------------------------------
function CreatePlayerPropertyChildWnd(Parent,ChildMaxNumber)
	Parent.m_IconWndTbl = {}
	Parent.m_RichTextWndTbl = {}
	
	local MaxNumber = ChildMaxNumber
	for i = 1, MaxNumber do 
		local IconWndName = "IconWnd" .. i
		local RichTextWndName = "RichTextWnd" .. i
		Parent.m_IconWndTbl[i]	= Parent:GetDlgChild(IconWndName)
		Parent.m_IconWndTbl[i]:ShowWnd(false)
		Parent.m_RichTextWndTbl[i]	= Parent:GetDlgChild(RichTextWndName)
		Parent.m_RichTextWndTbl[i]:SetWndTextXR(true)
		Parent.m_RichTextWndTbl[i]:SetTipFontSize(14)	
	end
end


function CreateGuiCommWnd(Wnd)
	Wnd.m_XBtn				= Wnd:GetDlgChild("XBtn") 
	Wnd.m_BtnChangeLeft		= Wnd:GetDlgChild("right")
	Wnd.m_BtnChangeRight	= Wnd:GetDlgChild("left")
	Wnd.m_PlayerName		= Wnd:GetDlgChild("PlayerName")
	Wnd.m_WndBody			= Wnd:GetDlgChild("WndBody")
	Wnd.m_WndBodyShowBtn	= Wnd:GetDlgChild("WndBodyShowBtn")
	
	Wnd.m_SoulValueLable	= Wnd:GetDlgChild("SoulValueLable")
	Wnd.m_SoulValue			= Wnd:GetDlgChild("SoulValue")
	
	Wnd.m_PropertyChangeBtn = Wnd:GetDlgChild("PropertyChangeBtn")
	Wnd.m_Exp_pro			= Wnd:GetDlgChild("exp_pro")
	Wnd.m_Exp_pro:SetProgressMode(0)
	Wnd.m_CampStatic		= Wnd:GetDlgChild("campstatic")
	Wnd.m_ClassStatic 		= Wnd:GetDlgChild("classstatic")
	Wnd.m_LevelStatic      	= Wnd:GetDlgChild("levelstatic")
	Wnd.m_TongPosition		= Wnd:GetDlgChild("tongposition")
	Wnd.m_FightingBtn 		= Wnd:GetDlgChild("FightingBtn")
	Wnd.m_FightingRichText 	= Wnd:GetDlgChild("FightingRichText")
	Wnd.m_Exp_pro:ShowWnd(false)
	Wnd.m_PropertyChangeBtn:ShowWnd(false)
	Wnd.m_PlayerName:ShowWnd(false)
	Wnd.m_XBtn:ShowWnd(false)
	Wnd.m_BtnChangeLeft:InserBefore(Wnd.m_WndBody)
	Wnd.m_BtnChangeRight:InserBefore(Wnd.m_WndBody)
	Wnd.m_PropertyChangeBtn:InserBefore(Wnd.m_PlayerPropertyWndTbl[1])
end

CPlayerPropertyWnd = class( SQRDialog )
function CreatePlayerPropertyWnd(Parent)
	Parent.m_DituStatic		= Parent:GetDlgChild("DituStatic")
	local ChildNumberTbl = {5,5,7,4}
	Parent.m_PlayerPropertyWndTbl = {}
	for i = 1, 4 do
		local Wnd = CPlayerPropertyWnd:new()
		local WndName = "PlayerPropertyWnd" .. i
		Wnd:CreateFromRes(WndName, Parent)
		Wnd:ShowWnd( true )
		CreatePlayerPropertyChildWnd(Wnd,ChildNumberTbl[i])
		Parent.m_PlayerPropertyWndTbl[i] = Wnd
	end
	
	Parent.m_PlayerPropertyWndTbl[1].m_IconWndTbl[4]:ShowWnd(false)
	Parent.m_PlayerPropertyWndTbl[1].m_IconWndTbl[5]:ShowWnd(false)
	Parent.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4]:ShowWnd(false)
	Parent.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[5]:ShowWnd(false)	
	
	local rt = CFRect:new()
	Parent.m_DituStatic:GetLogicRect(rt)
	rt.top = rt.top + 5
	rt.bottom = rt.top + Parent.m_PlayerPropertyWndTbl[1]:GetWndOrgHeight()
	rt.left = rt.left + 12
	rt.right  = rt.left + Parent.m_PlayerPropertyWndTbl[1]:GetWndOrgWidth()
	Parent.m_PlayerPropertyWndTbl[1]:SetLogicRect(rt)
	
	for i = 2, 4 do
		Parent.m_PlayerPropertyWndTbl[i - 1]:GetLogicRect(rt)
		rt.top =  rt.bottom + 1
		rt.bottom = rt.top + Parent.m_PlayerPropertyWndTbl[i]:GetWndOrgHeight()
		Parent.m_PlayerPropertyWndTbl[i]:SetLogicRect(rt)
	end
end
--------------------------------------------------------------------------------------
function OnCreateEquipWnd(Wnd, bFlagColor)
	Wnd.Part = {}
	local tblInfo = {	
			{"necklace",	EEquipPart.eNecklace,			ToolTipsText(307)},
			{"baldric",		EEquipPart.eAccouterment,		ToolTipsText(308)},
			{"back",		EEquipPart.eBack,				ToolTipsText(309)},
			{"finger1",		EEquipPart.eRingLeft,			ToolTipsText(310)},
			{"finger2",		EEquipPart.eRingRight,			ToolTipsText(311)},
			{"assistant",	EEquipPart.eAssociateWeapon,	ToolTipsText(312)},
			{"head",		EEquipPart.eHead,				ToolTipsText(313)},
			{"shoulder",	EEquipPart.eShoulder,			ToolTipsText(314)},
			{"clothing",	EEquipPart.eWear,				ToolTipsText(315)},
			{"hand",		EEquipPart.eHand,				ToolTipsText(316)},
			{"feet",		EEquipPart.eShoe,				ToolTipsText(317)},
			{"mainhand",	EEquipPart.eWeapon,				ToolTipsText(318)},
			{"sesh",		EEquipPart.eSash,				ToolTipsText(319)}
	}
	for i = 1, nAllPartNum do
		Wnd.Part[i] = {}
		Wnd.Part[i].iconbtn = Wnd:GetDlgChild(tblInfo[i][1])
		Wnd.Part[i].iconbtn:SetDetectMouseOverWnd(true)
		Wnd.Part[i].PartType = tblInfo[i][2]
		Wnd.Part[i].PartTypeName = tblInfo[i][3]
		if bFlagColor then
    		local str = tblInfo[i][1] .. "Color"
    		Wnd.Part[i].IconColorBtn = Wnd:GetDlgChild(str)
    		Wnd.Part[i].IconColorBtn:ShowWnd(false)
    		Wnd.Part[i].IconColorBtn:SetParent(Wnd.Part[i].iconbtn)
        end
	end
end

function SetShowNeedInfoCommFun(Wnd,Player,Tongpos,tongName)
	local camp = Player:CppGetBirthCamp()
	local campDisplayName = Lan_NewRoleProperty_Common(camp,"DisplayName")
	Wnd.m_CampStatic:SetWndText(campDisplayName)
	local level = Player:CppGetLevel()
	Wnd.m_LevelStatic:SetWndText(GetStaticTextClient(17035) .. level)
	local class = Player:CppGetClass()
	local ClassStr = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(class)
	Wnd.m_ClassStatic:SetWndText(ClassStr)
	if Tongpos and tongName then
		Wnd.m_TongPosition:SetWndText(tongName .. " " .. Tongpos[1] )
	else
		Wnd.m_TongPosition:SetWndText("")
	end
end

function InitPropertyValue(Wnd)
	--普通攻击
	Wnd.NormalAttackDamage = "0-0"
	--暴击率
	Wnd.StrikeRate = 0
	--物理防护率
	Wnd.PhysicDefenceRate = 0
	--物理躲避率 
	Wnd.PhysicDodgeRate = 0
	--招架率
	Wnd.ParryRate = 0
	--法术防御率
	Wnd.MagicResistRate = 0
	--法术完全完全抵抗率
	Wnd.CompleteResistanceRate = 0
	--自然抗性
	Wnd.NatureResistanceRate = 0
	--破坏抗性
	Wnd.DestructionResistanceRate = 0
	--暗黑抗性 
	Wnd.EvilResistanceRate = 0
	--法术躲避率 
	Wnd.MagicDodgeRate = 0
end

function OnCreateRoleModel(Wnd)
	local wndBody = Wnd.m_WndBody
	--创建渲染资源
	wndBody.m_ObjWnd = CRenderSystemClient_Inst():CreateObjectDialog()
	local param   = WndCreateParam:new()
	param.uStyle  = 0x60000000
	param.width  = wndBody:GetWndOrgWidth()
	param.height = wndBody:GetWndOrgHeight()
	param:SetParentWnd(wndBody)
	param.font_size  = 12
	wndBody.m_ObjWnd:Create( param )
	wndBody.m_ObjWnd:SetUseSceneDepth( true )
	
	--建立人物
	wndBody.RenderObj = wndBody.m_ObjWnd:CreateRenderObject()

	--设置RenderWnd的底纹
	local Flag     = IMAGE_PARAM:new()
	Flag.CtrlMask  = SM_BS_BK
	Flag.StateMask = IP_ENABLE
	local Image    = wndBody:GetWndBkImage(Flag)
	wndBody.m_ObjWnd:SetWndBkImage(Flag, Image)
end

function OnDestroy(Wnd)
	Wnd.m_WndBody.m_ObjWnd:DestroyRenderObject( Wnd.m_WndBody.RenderObj )
	CRenderSystemClient_Inst():DestroyObjectDialog( Wnd.m_WndBody.m_ObjWnd )
end

--删除属性上角色模型Piece
function DelWndRolePiece( Wnd,EquipPart )
	if EquipPart == EEquipPart.eWear then
		Wnd.m_WndBody.RenderObj:RemovePiece( "body" )
	elseif EquipPart == EEquipPart.eHead then 
		Wnd.m_WndBody.RenderObj:RemovePiece( "armet" )
	elseif EquipPart == EEquipPart.eShoulder then
		Wnd.m_WndBody.RenderObj:RemovePiece( "shoulder" )
	elseif EquipPart == EEquipPart.eBack then
		Wnd.m_WndBody.RenderObj:RemovePiece( "back" )
	elseif EquipPart == EEquipPart.eHand then
		Wnd.m_WndBody.RenderObj:RemovePiece( "arm" )
	elseif EquipPart == EEquipPart.eShoe then
		Wnd.m_WndBody.RenderObj:RemovePiece( "shoe" )
	elseif EquipPart == EEquipPart.eWeapon then
		Wnd.m_WndBody.RenderObj:RemovePiece( "weapon" )
	elseif EquipPart == EEquipPart.eAssociateWeapon then
		Wnd.m_WndBody.RenderObj:RemovePiece( "offweapon" )
	end
end


local function GetPropertyValueByName(uEntityID,sName)
	local viewinfoMgr = CFighterViewInfoMgr.GetWnd()
	local viewInfo = viewinfoMgr:GetFightViewInfoByID(uEntityID)
	local value    = viewInfo:CppGetPropertyValueByName(GetPropertyName(sName))
	return value
end

local function GetNormalAttackDamageTips(Wnd,PhysicalDPS)
	local WeaponStr = ""
	if Wnd.Part[12].m_Info ~= nil then
		local item = Wnd.Part[12].m_Info
		local nBigID, nIndex ,nItemID = item:GetBigID(), item:GetIndex(),item:GetItemID()
		local AttackSpeed	= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AttackSpeed")
		local DynInfo	= g_DynItemInfoMgr:GetDynItemInfo(nItemID)
		local MaxAttack, MinAttack = GetWeaponDPSFloat(DynInfo.AttackSpeed, DynInfo.DPS,DynInfo.DPSFloorRate)
		MaxAttack = MaxAttack + PhysicalDPS*DynInfo.AttackSpeed
		MinAttack = MinAttack + PhysicalDPS*DynInfo.AttackSpeed
		WeaponStr = GetStaticTextClient(4027,tonumber(AttackSpeed),MinAttack .. "-" .. MaxAttack, PhysicalDPS*DynInfo.AttackSpeed)
	end
	
	local AssociateWeaponStr = ""
	if Wnd.Part[6].m_Info ~= nil then
		local item = Wnd.Part[6].m_Info
		local nBigID, nIndex ,nItemID = item:GetBigID(), item:GetIndex(),item:GetItemID()
		if g_ItemInfoMgr:IsWeapon(nBigID) then 
			local AttackSpeed	= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AttackSpeed")
			local DynInfo	= g_DynItemInfoMgr:GetDynItemInfo(nItemID)
			local MaxAttack, MinAttack = GetWeaponDPSFloat(DynInfo.AttackSpeed, DynInfo.DPS,DynInfo.DPSFloorRate)
			MaxAttack = (MaxAttack + PhysicalDPS*DynInfo.AttackSpeed)/2
			MinAttack = (MinAttack + PhysicalDPS*DynInfo.AttackSpeed)/2			
			AssociateWeaponStr =  GetStaticTextClient(4042,tonumber(AttackSpeed),MinAttack .. "-" .. MaxAttack, PhysicalDPS*DynInfo.AttackSpeed/2)	
		end		
	end
	local Str = ""
	if  WeaponStr ~= "" and AssociateWeaponStr ~= "" then
		Str = WeaponStr	.. "#r" .. AssociateWeaponStr
	elseif WeaponStr ~= "" and AssociateWeaponStr == "" then
		Str = WeaponStr
	elseif WeaponStr == "" and AssociateWeaponStr ~= "" then
		Str = AssociateWeaponStr
	elseif WeaponStr == "" and AssociateWeaponStr == "" then
		Str = GetStaticTextClient(4001,Wnd.NormalAttackDamage)
	end
	return Str
end

local function GetFaZhangAttackDamageTips(Wnd)
	local WeaponStr = ""
	if Wnd.Part[12].m_Info ~= nil then
		local item = Wnd.Part[12].m_Info
		local nBigID, nIndex ,nItemID = item:GetBigID(), item:GetIndex(),item:GetItemID()
		local EquipType	= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"EquipType")
		if EquipType ~= "" and string.find(EquipType,"杖") then 
			local AttackSpeed	= g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"AttackSpeed")
			local DynInfo	= g_DynItemInfoMgr:GetDynItemInfo(nItemID)
			local MaxAttack, MinAttack = GetWeaponDPSFloat(DynInfo.AttackSpeed, DynInfo.DPS,DynInfo.DPSFloorRate)
			WeaponStr = GetStaticTextClient(4043,tonumber(AttackSpeed),MinAttack .. "-" .. MaxAttack )
		end
	end

	return WeaponStr
end

--更新角色的信息 同步角色的属性
function UpdateAllRoleInfo(Wnd)
	if not IsCppBound(Wnd.m_Target) then
		return 
	end
	local Level = Wnd.m_Target:CppGetLevel()
	local ClassID = Wnd.m_Target:CppGetClass()
	local EntityID = Wnd.m_Target:GetEntityID()
	--物理攻击
	local PhysicalDPS       = GetPropertyValueByName(EntityID,"攻击力")
	local AccuratenessValue = GetPropertyValueByName(EntityID,"精准值")
	local StrikeValue       = GetPropertyValueByName(EntityID,"暴击值")
	--self:GetPropertyValueByName("暴击率")
	local CRIMax    = 		GetPropertyValueByName(EntityID,"暴击上限")
	
	--法术攻击(法术攻击普攻伤害跟物理攻击普攻伤害一样)(法术攻击暴击跟物理攻击暴击一样)
	local MagicDamageValue  = GetPropertyValueByName(EntityID,"法伤")
	local NatureDamageValue = GetPropertyValueByName(EntityID,"自然法伤")
	local DestructionDamageValue = GetPropertyValueByName(EntityID,"破坏法伤")
	local EvilDamageValue = GetPropertyValueByName(EntityID,"暗黑法伤")
	
	local MagicHitValue     = GetPropertyValueByName(EntityID,"法术命中")
	
	local MaxWeaponDamage   = GetPropertyValueByName(EntityID,"最大攻击")
	local MinWeaponDamage   = GetPropertyValueByName(EntityID,"最小攻击")
	MaxWeaponDamage = string.format("%.0f",MaxWeaponDamage)
	MinWeaponDamage = string.format("%.0f",MinWeaponDamage)
	local AttackDamage = MinWeaponDamage .. "-" .. MaxWeaponDamage
	
	--物理防御
	local ResilienceValue    = GetPropertyValueByName(EntityID,"韧性值")
	local Defence            = GetPropertyValueByName(EntityID,"护甲值")
	if Defence < 0 then
		Defence = 0
	end
	local PhysicalDodgeValue = GetPropertyValueByName(EntityID,"物理躲避值")
	if PhysicalDodgeValue > 2000000000 then
		PhysicalDodgeValue = 0
	end
	
	local ParryValue         = GetPropertyValueByName(EntityID,"招架值")
	if ParryValue > 2000000000  then
		ParryValue = 0
	end
	
	local StrikeResistanceValue = GetPropertyValueByName(EntityID,"免爆值")
	local BlockRate          = GetPropertyValueByName(EntityID,"格挡率") * 100
	local BlockDamage = GetPropertyValueByName(EntityID,"格挡减免值")
	local MagicResistRate    =  Wnd.MagicResistRate

	--法术防御
	local MagicDodgeValue            = GetPropertyValueByName(EntityID,"法术躲避值")
	if MagicDodgeValue > 2000000000  then
		MagicDodgeValue = 0
	end
	local NatureResistanceValue      = GetPropertyValueByName(EntityID,"自然抗值")
	if NatureResistanceValue > 2000000000  then
		NatureResistanceValue = 0
	end
	local DestructionResistanceValue = GetPropertyValueByName(EntityID,"破坏抗值")
	if DestructionResistanceValue > 2000000000  then
		DestructionResistanceValue = 0
	end
	local EvilResistanceValue        = GetPropertyValueByName(EntityID,"暗黑抗值")
	if EvilResistanceValue > 2000000000  then
		EvilResistanceValue = 0
	end
	--if Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[1].NormalAttackDamage ~= Wnd.NormalAttackDamage then
		local NormalAttack = Wnd.NormalAttackDamage
		local Str = ""
		if ClassID == EClass.eCL_NatureElf 
			or ClassID == EClass.eCL_Priest
			or ClassID == EClass.eCL_EvilElf then
			NormalAttack = 0	
			Str = GetStaticTextClient(4001,NormalAttack)
		else
			Str = GetNormalAttackDamageTips(Wnd,PhysicalDPS)
		end		
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[1]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色1") .. GetStaticTextClient(4001,NormalAttack) )
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[1]:SetMouseOverDescAfter(Str)
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[1].NormalAttackDamage = Wnd.NormalAttackDamage
	--end
	if Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[2].PhysicalDPS ~= PhysicalDPS then
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[2]:SetWndText(g_ColorMgr:GetColor("属性面板文字颜色1") .. GetStaticTextClient(4021, PhysicalDPS) )
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[2]:SetMouseOverDescAfter(GetStaticTextClient(4028, PhysicalDPS) )
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[2].PhysicalDPS = PhysicalDPS
	end
	
	if Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[3].AccuratenessValue ~= AccuratenessValue then
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[3]:SetWndText(g_ColorMgr:GetColor("属性面板文字颜色1") .. GetStaticTextClient(4022, AccuratenessValue ))	
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[3]:SetMouseOverDescAfter(GetStaticTextClient(4029,AccuratenessValue, AccuratenessValue/(AccuratenessValue + Level * 52 + 700 ) * 100 ))	
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[3].AccuratenessValue = AccuratenessValue
	end
	if Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4].StrikeValue ~= StrikeValue
		or Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4].StrikeRate ~= Wnd.StrikeRate then
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色1") .. GetStaticTextClient(4023, StrikeValue) )	
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4]:SetMouseOverDescAfter(GetStaticTextClient(4032, StrikeValue,Wnd.StrikeRate))
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4].StrikeValue = StrikeValue
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[4].StrikeRate = Wnd.StrikeRate
	end
	if Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[5].CRIMax ~= CRIMax then
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[5]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色1") .. GetStaticTextClient(4008, CRIMax) )
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[5]:SetMouseOverDescAfter( GetStaticTextClient(4030, CRIMax,CRIMax/2000*100) )
		Wnd.m_PlayerPropertyWndTbl[1].m_RichTextWndTbl[5].CRIMax = CRIMax
	end
	local Str = GetFaZhangAttackDamageTips(Wnd)
	--if Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[1].AttackDamage ~= AttackDamage then		
		if ClassID == EClass.eCL_Warrior 
			or ClassID == EClass.eCL_MagicWarrior
			or ClassID == EClass.eCL_OrcWarrior or Str == "" then
			AttackDamage = 0
			Str = GetStaticTextClient(4044, AttackDamage)
		end
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[1]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色2") .. GetStaticTextClient(4044, AttackDamage) )
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[1]:SetMouseOverDescAfter(Str)
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[1].AttackDamage = AttackDamage
	--end
	if Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].MagicDamageValue ~= MagicDamageValue
		or Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].NatureDamageValue ~= NatureDamageValue
		or Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].DestructionDamageValue ~= DestructionDamageValue
		or Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].EvilDamageValue ~= EvilDamageValue then
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色2") .. GetStaticTextClient(4018, MagicDamageValue) )
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2]:SetMouseOverDescAfter(GetStaticTextClient(4025, MagicDamageValue,NatureDamageValue,DestructionDamageValue,EvilDamageValue))
		
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].MagicDamageValue = MagicDamageValue
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].NatureDamageValue = NatureDamageValue
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].DestructionDamageValue = DestructionDamageValue
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[2].EvilDamageValue = EvilDamageValue 
	end
	if Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[3].MagicHitValue ~= MagicHitValue then
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[3]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色2") .. GetStaticTextClient(4019, MagicHitValue) )
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[3]:SetMouseOverDescAfter(GetStaticTextClient(4031,MagicHitValue, MagicHitValue/(MagicHitValue + Level*52 + 700)*100))	
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[3].MagicHitValue = MagicHitValue
	end
	if Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[4].StrikeValue ~= StrikeValue 
		or Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[4].StrikeRate ~= Wnd.StrikeRate then
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[4]:SetWndText(g_ColorMgr:GetColor("属性面板文字颜色3") .. GetStaticTextClient(4023, StrikeValue) )	
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[4]:SetMouseOverDescAfter(GetStaticTextClient(4032, StrikeValue,Wnd.StrikeRate))
		
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[4].StrikeValue = StrikeValue 
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[4].StrikeRate = Wnd.StrikeRate
	end
	if Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[5].CRIMax ~= CRIMax then
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[5]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色3") .. GetStaticTextClient(4008, CRIMax ))	
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[5]:SetMouseOverDescAfter( GetStaticTextClient(4030, CRIMax,CRIMax/1000*100 ))	
		Wnd.m_PlayerPropertyWndTbl[2].m_RichTextWndTbl[5].CRIMax = CRIMax
	end
	
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[1].PhysicalDodgeValue ~= PhysicalDodgeValue
		or Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[1].PhysicDodgeRate ~= Wnd.PhysicDodgeRate then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[1]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4015, PhysicalDodgeValue) )
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[1]:SetMouseOverDescAfter( GetStaticTextClient(4033, PhysicalDodgeValue,Wnd.PhysicDodgeRate) )
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[1].PhysicalDodgeValue = PhysicalDodgeValue
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[1].PhysicDodgeRate = Wnd.PhysicDodgeRate
	end
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[2].ParryValue ~= ParryValue
		or Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[2].ParryRate ~= Wnd.ParryRate  then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[2]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4016, ParryValue) )
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[2]:SetMouseOverDescAfter( GetStaticTextClient(4034, ParryValue,Wnd.ParryRate) )
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[2].ParryValue = ParryValue
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[2].ParryRate = Wnd.ParryRate
	end
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[3].BlockRate ~= BlockRate
		or Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[3].BlockDamage ~= BlockDamage then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[3]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4017, BlockRate) )	
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[3]:SetMouseOverDescAfter(GetStaticTextClient(4026, BlockRate,BlockDamage))
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[3].BlockRate = BlockRate
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[3].BlockDamage = BlockDamage
	end
	
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[4].StrikeResistanceValue ~= StrikeResistanceValue then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[4]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4046, StrikeResistanceValue) )	
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[4]:SetMouseOverDescAfter(GetStaticTextClient(4047, StrikeResistanceValue/(StrikeResistanceValue+Level*52+700) * 100))
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[4].StrikeResistanceValue = StrikeResistanceValue
	end	
	
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[5].ResilienceValue ~= ResilienceValue then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[5]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4007, ResilienceValue) )	
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[5]:SetMouseOverDescAfter(GetStaticTextClient(4035, ResilienceValue/2000 * 100))
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[5].ResilienceValue = ResilienceValue
	end
	
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[6].MagicResistRate ~= MagicResistRate
		or Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[6].CompleteResistanceRate ~= Wnd.CompleteResistanceRate then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[6]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4024, MagicResistRate) )	
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[6]:SetMouseOverDescAfter(GetStaticTextClient(4036, MagicResistRate,Wnd.CompleteResistanceRate))
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[6].MagicResistRate = MagicResistRate
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[6].CompleteResistanceRate = Wnd.CompleteResistanceRate
	end
	if Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[7].MagicDodgeValue ~= MagicDodgeValue
		or Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[7].MagicDodgeRate ~= Wnd.MagicDodgeRate then
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[7]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色4") .. GetStaticTextClient(4011, MagicDodgeValue) )		
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[7]:SetMouseOverDescAfter( GetStaticTextClient(4037, MagicDodgeValue,Wnd.MagicDodgeRate) )
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[7].MagicDodgeValue = MagicDodgeValue
		Wnd.m_PlayerPropertyWndTbl[3].m_RichTextWndTbl[7].MagicDodgeRate = Wnd.MagicDodgeRate	
	end
	
	if Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[1].Defence ~= Defence
		or Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[1].PhysicDefenceRate ~= Wnd.PhysicDefenceRate then
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[1]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色5") .. GetStaticTextClient(4003, Defence) )
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[1]:SetMouseOverDescAfter(GetStaticTextClient(4038, Defence,Wnd.PhysicDefenceRate))
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[1].Defence = Defence
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[1].PhysicDefenceRate = Wnd.PhysicDefenceRate
	end
	
	if Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[2].NatureResistanceValue ~= NatureResistanceValue
		or Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[2].NatureResistanceRate ~= Wnd.NatureResistanceRate then
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[2]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色6") .. GetStaticTextClient(4012, NatureResistanceValue) )
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[2]:SetMouseOverDescAfter(GetStaticTextClient(4039, NatureResistanceValue,Wnd.NatureResistanceRate))
		
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[2].NatureResistanceValue = NatureResistanceValue
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[2].NatureResistanceRate = Wnd.NatureResistanceRate	
	end
	if Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[3].DestructionResistanceValue ~= DestructionResistanceValue 
	   or Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[3].DestructionResistanceRate ~=  Wnd.DestructionResistanceRate	then
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[3]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色7") .. GetStaticTextClient(4013, DestructionResistanceValue) )	
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[3]:SetMouseOverDescAfter(GetStaticTextClient(4040, DestructionResistanceValue,Wnd.DestructionResistanceRate))
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[3].DestructionResistanceValue = DestructionResistanceValue 
	   	Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[3].DestructionResistanceRate =  Wnd.DestructionResistanceRate		
	end
	if Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[4].EvilResistanceValue ~= EvilResistanceValue
		or Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[4].EvilResistanceRate ~= Wnd.EvilResistanceRate then
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[4]:SetWndText( g_ColorMgr:GetColor("属性面板文字颜色8") .. GetStaticTextClient(4014, EvilResistanceValue) )	
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[4]:SetMouseOverDescAfter(GetStaticTextClient(4041, EvilResistanceValue,Wnd.EvilResistanceRate))
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[4].EvilResistanceValue = EvilResistanceValue
		Wnd.m_PlayerPropertyWndTbl[4].m_RichTextWndTbl[4].EvilResistanceRate = Wnd.EvilResistanceRate 
	end
end

function GetPlayerEquipAssessValueTbl(Wnd)
	local Tooltips = CTooltips:new()
	local AllEquipAssessTbl = {}
	if Wnd then 
		for i = 1, nAllPartNum do
			if Wnd.Part[i].m_Info then
				local PartType = Wnd.Part[i].PartType
				local Item = Wnd.Part[i].m_Info
				local nBigID, nIndex ,nItemID = Item:GetBigID(), Item:GetIndex(),Item:GetItemID()
				local EquipAssess = Tooltips:GetEquipAssessFun(nBigID,nIndex,nItemID) 
				AllEquipAssessTbl[PartType] = EquipAssess
			end
		end
	end
	return AllEquipAssessTbl
end

function SetAni(renderObj)
	if g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InDrawWeaponMode) then
		renderObj:DoAni( "stand01_y", true, -1.0 )
	else
		renderObj:DoAni( "stand01_w", true, -1.0 )	
	end
end
