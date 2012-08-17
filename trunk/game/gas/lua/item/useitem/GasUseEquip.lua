gac_gas_require "item/Equip/EquipDef"
--[[
		装备真麻烦，又要右键使用，又要能交换，又要卸载，又要能直接删除，再来个左右戒指交换，戒指右键使用，戒指直接对换
	换了装又要改数字，改了数字就要同步，同步了就要换资源，换资源就要注意follower对象是否同步，接下来的问题就是：
	上下线信息是否正确，写log，测试用例，tooltips，人物属性面板 ...... 
	
装备问题概述：
	<1> 使用方式：
	 1：右键点击直接使用装备（仓库不能使用）
	 2：点击物品栏再点击装备栏使用装备（仓库可以使用）
	 3：点击装备栏再点击物品栏进行交换装备，如果物品栏为空就是卸载装备（仓库可以使用）
	 4：点击装备栏再点击空白可以直接删除装备
	<2> 说明：
	 1：3种使用方式都可以转换为右键点击的方式进行调用。
	 2：除了戒指外其他的装备都很容易在client统一接口，戒指比较麻烦，要考虑到左右戒指的问题。
	 直接右键使用的话要智能的选择哪个戒指可以使用，现在判断的条件是如果左戒指是空的，
	 就装备在左戒指上否则就装备到右戒指上。而通过点击物品栏和装备栏的就不需要智能的选择
	 而是直接进行交换就可以了。
	 3：动态装备和静态装备分开设计，直接在代码里区分。最主要的也就是获取魔法属性的方式不一样
	<3> 注意：
	 1：在server端采用了内存缓存已经使用的装备信息，这样在计算人物属性的时候可以带来大大的方便，不过
	 在改逻辑的时候就需要注意这里是需要同步的，包括登录的时候。
--]]

local GasUseItemDB = "GasUseItemDB"

function g_UseEquip(Connection,Player,nRoomIndex,nPos,nBigID,nIndex,nItemID,nEquipPart,tbl, charID)
	---------------------------------高速逻辑-------------------
	local equip_mgr = g_GetEquipMgr()
	local HandType = tbl["HandType"]
	local RetRoomInfo = tbl["RetRoomInfo"]
	local eEquipPart = nEquipPart
	local new_CommonlySkillName = tbl["new_CommonlySkillName"]
	local BindingChange = tbl["BindingChange"]
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local ResID = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"ResID")
	
	--设置身上装备信息
	local EquipInfo = tbl["OldEquipInfo"]
	local RDoubleHandInfo = tbl["RDoubleHandInfo"]
	local OldEquipRet = tbl["OldEquipRet"]
	local NewEquipRet = tbl["NewEquipRet"]
	local ShortCutRet = tbl["ShortCutRet"]
	--套装信息
	local EquipSuitTbl_New = tbl["EquipSuitTbl_New"]
	local EquipSuitTbl_Old = tbl["EquipSuitTbl_Old"]
	local EquipSuitTbl_Associate = tbl["EquipSuitTbl_Associate"]
	local RemoveJingLingSkillIndex = tbl["RemoveJingLingSkillIndex"]
	--设置副手武器的信息
	local AssociateWeaponEquipInfo = tbl["AssociateWeaponInfo"]
	local AssociateWeaponEquipRet = tbl["AssociateWeaponRet"]
    
    local OldEquipDuraState 
    local NewEquipDuraState = NewEquipRet["DuraState"]
    local NewEquipCurDuraValue = NewEquipRet["CurDuraValue"]
    local NewEquipMaxDuraValue = NewEquipRet["MaxDuraValue"]
    local AssociateWeaponDuraState
    
	local old_equip
	if EquipInfo ~= nil then
		old_equip = equip_mgr:GetEquipFromRet(tonumber(EquipInfo[3]), Player.m_uID, tonumber(EquipInfo[1]), EquipInfo[2],OldEquipRet)
	    OldEquipDuraState = OldEquipRet["DuraState"]
	end
	if RDoubleHandInfo ~= nil then
		old_equip = equip_mgr:GetEquipFromRet(tonumber(RDoubleHandInfo[3]), Player.m_uID, tonumber(RDoubleHandInfo[1]), RDoubleHandInfo[2],OldEquipRet)
	end
	--设置要装备的武器信息
	local new_equip = equip_mgr:GetEquipFromRet(nItemID,Player.m_uID,nBigID,nIndex,NewEquipRet)

	local AssociateWeaponEquip
	if AssociateWeaponEquipInfo ~= nil then
		AssociateWeaponEquip = equip_mgr:GetEquipFromRet(tonumber(AssociateWeaponEquipInfo[3]), Player.m_uID, tonumber(AssociateWeaponEquipInfo[1]), AssociateWeaponEquipInfo[2],AssociateWeaponEquipRet)
	    AssociateWeaponDuraState = AssociateWeaponEquipRet["DuraState"]
	end
	
	---需要原来武器的Info信息
	if EquipInfo ~= nil or RDoubleHandInfo ~= nil then
		if old_equip.sSuitName ~= "" and old_equip.sSuitName ~= nil and EquipSuitTbl_Old ~= nil then
			g_EquipMgr:GetSuitAttributeBySuitName(Player,false,EquipSuitTbl_Old[1],EquipSuitTbl_Old[2], EquipSuitTbl_Old[3], old_equip.BaseLevel)
		end
	end
	if new_equip.sSuitName ~= "" and new_equip.sSuitName ~= nil and EquipSuitTbl_New ~= nil then 
		g_EquipMgr:GetSuitAttributeBySuitName(Player,true,EquipSuitTbl_New[1],EquipSuitTbl_New[2],EquipSuitTbl_New[3], new_equip.BaseLevel)
	end
	if HandType == "双" then
		if AssociateWeaponEquipInfo ~= nil then
			if AssociateWeaponEquip.sSuitName ~= "" and AssociateWeaponEquip.sSuitName ~= nil and EquipSuitTbl_Associate ~= nil then
				g_EquipMgr:GetSuitAttributeBySuitName(Player,false,EquipSuitTbl_Associate[1],EquipSuitTbl_Associate[2],EquipSuitTbl_Associate[3], AssociateWeaponEquip.BaseLevel)		
			end
		end
	end 
	
	--取消附加属相	
	if EquipInfo ~= nil then
		old_equip:DisablePropty(Player,eEquipPart, "g_UseEquip1")
		Player.m_ErrorEquipTbl[eEquipPart] = false
		Player:SetPlayerEquipPhase(eEquipPart,0, 0)
	end
	
	if eEquipPart == EEquipPart.eAssociateWeapon and RDoubleHandInfo ~= nil then
        old_equip:DisablePropty(Player,EEquipPart.eWeapon, "g_UseEquip2")
        Player.m_ErrorEquipTbl[EEquipPart.eWeapon] = false
		Player:SetPlayerEquipPhase(EEquipPart.eWeapon,0,0)
		Player:SetEquipIndexID(EEquipPart.eWeapon,0,0)
		Player:ClearEquipInfo(EEquipPart.eWeapon)
	end
	
	--双手武器也取消副手武器的属性
	if HandType == "双" then
		if AssociateWeaponEquipInfo ~= nil then
			AssociateWeaponEquip:DisablePropty(Player,EEquipPart.eAssociateWeapon, "g_UseEquip3")
			Player.m_ErrorEquipTbl[EEquipPart.eAssociateWeapon] = false
			Gas2Gac:RetEquipDuraValue(Connection, 0, EEquipPart.eAssociateWeapon, 2, 0, 0)
			Player:SetPlayerEquipPhase(EEquipPart.eAssociateWeapon,0,0)
			Player:SetEquipIndexID(EEquipPart.eAssociateWeapon,0,0)
			Player:ClearEquipInfo(EEquipPart.eAssociateWeapon)
			Player:CppInitAHNA(nil,"无类型","",0,false)
		end 
	end
	Player:SetEquipIndexID(eEquipPart,nBigID,nIndex)
	
	--替换新的饰品，添加新的属性
	Player:SetEquipInfo(nBigID,nIndex,nItemID,eEquipPart)
	new_equip:EnablePropty(Player,eEquipPart, "g_UseEquip")
	Gas2Gac:RetEquipDuraValue(Connection, nItemID, nEquipPart, NewEquipDuraState, NewEquipCurDuraValue, NewEquipMaxDuraValue)
	Player:SetPlayerEquipPhase(eEquipPart,new_equip.uIntensifyPhase,new_equip.CurAdvancePhase)

	if eEquipPart == EEquipPart.eAssociateWeapon and RDoubleHandInfo ~= nil then		
		Player:CppInitMHNA(new_CommonlySkillName,"无类型","",2.0,false)		
	end
	
	--得到新装备产生的普攻技能
	if eEquipPart == EEquipPart.eWeapon then
	    if NewEquipDuraState ~= 0 then
		    new_equip:SetWeaponSkill(Player,nBigID,nIndex,false)
		else
		    Player:CppInitMHNA(new_CommonlySkillName,"无类型","",2.0,false)
		    Gas2Gac:ReWeaponAttr(Player.m_Conn,false,new_CommonlySkillName)		
		end	
	elseif eEquipPart == EEquipPart.eAssociateWeapon and g_ItemInfoMgr:IsWeapon(nBigID) then
		if NewEquipDuraState ~= 0 then
		    new_equip:SetAssociateSkill(Player,nBigID,nIndex,false)	
		else
		    Player:CppInitAHNA(nil,"无类型","",0,false)
		end
	elseif  eEquipPart == EEquipPart.eAssociateWeapon and g_ItemInfoMgr:IsStaticShield(nBigID) then
		Player:CppInitAHNA(nil,"无类型",EquipType,0,false)
	end
	
	if BindingChange then
		new_equip:SendEquipInfo2Conn(charID)
	end
	Player:SetArmorValue()
	Gas2Gac:RetUseItem(Connection,nRoomIndex,nPos,nItemID,eEquipPart)
	--双手装备时副手装备的信息
	if HandType == "双" then
		if AssociateWeaponEquipInfo ~= nil then
			Gas2Gac:ReAssociateAttr(Connection,false) 
		end
	end
	if ResID ~= nil then 
		Gas2Gac:AoiAddEquip(Player:GetIS(0),Player:GetEntityID(),eEquipPart,nBigID,nIndex)
		Player:TriggerSetupWeapon()
	end
	--替换快捷栏中存储的普攻技能信息
	if eEquipPart == EEquipPart.eWeapon then 
		Gas2Gac:ReCommonlySkill(Connection,new_CommonlySkillName)
		Player:GetShortInfoByRet(ShortCutRet)
	elseif eEquipPart == EEquipPart.eAssociateWeapon then
		if RDoubleHandInfo ~= nil then 
			Gas2Gac:ReCommonlySkill(Connection,new_CommonlySkillName)
			Player:GetShortInfoByRet(ShortCutRet)			
		end 
		Gas2Gac:ReAssociateAttr(Connection,true) 				
	end
	equip_mgr:SetCharJingLingSkill(Player, false, RemoveJingLingSkillIndex, charID)
    --设置三阶段的天赋
    if old_equip ~= nil and old_equip.uIntenTalentIndex and old_equip.uIntenTalentIndex > 0 then
        old_equip:SetIntensifyAddTalent(Player, old_equip.uIntenTalentIndex, old_equip.BaseLevel, false)
	end
	if new_equip.uIntenTalentIndex and new_equip.uIntenTalentIndex > 0 then
	    new_equip:SetIntensifyAddTalent(Player, new_equip.uIntenTalentIndex, new_equip.BaseLevel, true)
	end 
	
	local haveDemagedEquip = tbl["HaveDemagedEquip"]
	if haveDemagedEquip == 0 then
	    Gas2Gac:RetShowEquipDuraWnd(Connection, false) 
	end
	Player:TriggerChangeEquip()
	CGasFightingEvaluation.SaveFightingEvaluationInfo(Player)
end

--戒指的装备麻烦一些，要检查左右的问题.
function g_UseJewelry(Connection,Player,nRoomIndex,nPos,nBigID,nIndex,nItemID,nEquipPart,tbl, charID)
	---------------------------------高速逻辑----------------------------------
	local equip_mgr = g_GetEquipMgr()
	local eEquipPart = nEquipPart

	--设置身上装备信息
	local EquipInfo = tbl["OldEquipInfo"]
	local OldEquipRet = tbl["OldEquipRet"]
    local NewEquipRet = tbl["NewEquipRet"]	
	local BindingChange = tbl["BindingChange"]
	local EquipSuitTbl_New = tbl["EquipSuitTbl_New"]
	local EquipSuitTbl_Old = tbl["EquipSuitTbl_Old"]
	local RemoveJingLingSkillIndex = tbl["RemoveJingLingSkillIndex"]
	local OldEquipDuraState 
	local NewEquipDuraState = NewEquipRet["DuraState"]
	local haveDemagedEquip = tbl["HaveDemagedEquip"]
	local NewEquipCurDuraValue = NewEquipRet["CurDuraValue"]
	local NewEquipMaxDuraValue = NewEquipRet["MaxDuraValue"]
	
	local old_equip
	if EquipInfo ~= nil then
		old_equip = equip_mgr:GetEquipFromRet(tonumber(EquipInfo[3]), Player.m_uID, tonumber(EquipInfo[1]), EquipInfo[2],OldEquipRet)
	    OldEquipDuraState = OldEquipRet["DuraState"]
	end
	
	--设置要装备的武器信息
	
	local new_equip = equip_mgr:GetEquipFromRet(nItemID,Player.m_uID,nBigID,nIndex,NewEquipRet)
	
	if EquipInfo ~= nil then
		if old_equip.sSuitName ~= "" and old_equip.sSuitName ~= nil and EquipSuitTbl_Old ~= nil then
			g_EquipMgr:GetSuitAttributeBySuitName(Player,false,EquipSuitTbl_Old[1],EquipSuitTbl_Old[2], EquipSuitTbl_Old[3], old_equip.BaseLevel)
		end
	end
	if new_equip.sSuitName ~= "" and new_equip.sSuitName ~= nil and EquipSuitTbl_New ~= nil then 
		g_EquipMgr:GetSuitAttributeBySuitName(Player,true,EquipSuitTbl_New[1],EquipSuitTbl_New[2],EquipSuitTbl_New[3], new_equip.BaseLevel)
	end
	
	if EquipInfo ~= nil then
		old_equip:DisablePropty(Player,eEquipPart, "g_UseJewelry")
		Player.m_ErrorEquipTbl[eEquipPart] = false
		Player:SetPlayerEquipPhase(eEquipPart,0,0)
	end
	Player:SetEquipInfo(nBigID,nIndex,nItemID,eEquipPart)
	new_equip:EnablePropty(Player,eEquipPart, "g_UseJewelry")
	Gas2Gac:RetEquipDuraValue(Connection, nItemID, eEquipPart, NewEquipDuraState, NewEquipCurDuraValue, NewEquipMaxDuraValue)
	Player:SetPlayerEquipPhase(eEquipPart,new_equip.uIntensifyPhase,new_equip.CurAdvancePhase)
	if BindingChange then
		new_equip:SendEquipInfo2Conn(charID)
	end
	Player:SetArmorValue()
	Gas2Gac:RetUseItem(Connection,nRoomIndex,nPos,nItemID,eEquipPart)
	equip_mgr:SetCharJingLingSkill(Player, false, RemoveJingLingSkillIndex, charID)
	--设置三阶段的天赋
    if old_equip ~= nil and old_equip.uIntenTalentIndex and old_equip.uIntenTalentIndex > 0 then
        old_equip:SetIntensifyAddTalent(Player, old_equip.uIntenTalentIndex, old_equip.BaseLevel, false)
	end
	if new_equip.uIntenTalentIndex and new_equip.uIntenTalentIndex > 0 then
	    new_equip:SetIntensifyAddTalent(Player, new_equip.uIntenTalentIndex, new_equip.BaseLevel, true)
	end
	if haveDemagedEquip == 0 then
	    Gas2Gac:RetShowEquipDuraWnd(Connection, false) 
	end
	Player:TriggerChangeEquip()
	CGasFightingEvaluation.SaveFightingEvaluationInfo(Player)
end

local function SwitchTwoRing(Connection,Player,data)
	local RingRightInfo = data["RingLeftInfo"]
	local RingLeftInfo = data["RingRightInfo"]
	local RingRightRet = data["RingLeftRet"]
	local RingLeftRet = data["RingRightRet"]
	local equip_mgr = g_GetEquipMgr()
	local RingRight_equip, RingLeft_equip
	local ErrorRingLeft = Player.m_ErrorEquipTbl[EEquipPart.eRingRight]
	local ErrorRingRight = Player.m_ErrorEquipTbl[EEquipPart.eRingLeft]
	
	if RingRightInfo ~= nil then	
		Player:SetEquipInfo(RingRightInfo[1],RingRightInfo[2],RingRightInfo[3],EEquipPart.eRingRight)
		RingRight_equip = equip_mgr:GetEquipFromRet(tonumber(RingRightInfo[3]), Player.m_uID, tonumber(RingRightInfo[1]), RingRightInfo[2],RingRightRet)
		Player:SetPlayerEquipPhase(EEquipPart.eRingRight,RingRight_equip.uIntensifyPhase, RingRight_equip.CurAdvancePhase)
		RingRight_equip:DisablePropty(Player, EEquipPart.eRingLeft , "SwitchTwoRing1")
		Gas2Gac:RetEquipDuraValue(Player.m_Conn, RingRightInfo[3], EEquipPart.eRingLeft, 2, RingRightRet["MaxDuraValue"], RingRightRet["MaxDuraValue"])
	else
		Player:SetPlayerEquipPhase(EEquipPart.eRingRight,0,0)
	end
	
	if RingLeftInfo ~= nil then
		Player:SetEquipInfo(RingLeftInfo[1],RingLeftInfo[2],RingLeftInfo[3],EEquipPart.eRingLeft)
		RingLeft_equip = equip_mgr:GetEquipFromRet(tonumber(RingLeftInfo[3]), Player.m_uID, tonumber(RingLeftInfo[1]), RingLeftInfo[2],RingLeftRet)
		Player:SetPlayerEquipPhase(EEquipPart.eRingLeft,RingLeft_equip.uIntensifyPhase,RingLeft_equip.CurAdvancePhase)
		RingLeft_equip:DisablePropty(Player, EEquipPart.eRingRight, "SwitchTwoRing2")
		Gas2Gac:RetEquipDuraValue(Player.m_Conn, RingLeftInfo[3], EEquipPart.eRingRight, 2, RingLeftRet["MaxDuraValue"], RingLeftRet["MaxDuraValue"])
	else
		Player:SetPlayerEquipPhase(EEquipPart.eRingLeft,0,0)
	end
	
	Player.m_ErrorEquipTbl[EEquipPart.eRingRight] = ErrorRingRight
	if RingRightInfo ~= nil then
	    RingRight_equip:EnablePropty(Player, EEquipPart.eRingRight, "SwitchTwoRing1")
	    Gas2Gac:RetEquipDuraValue(Player.m_Conn, RingRightInfo[3], EEquipPart.eRingRight, RingRightRet["DuraState"], RingRightRet["CurDuraValue"], RingRightRet["MaxDuraValue"])
	end 
	
	Player.m_ErrorEquipTbl[EEquipPart.eRingLeft] = ErrorRingLeft
	if RingLeftInfo ~= nil then
	    RingLeft_equip:EnablePropty(Player, EEquipPart.eRingLeft, "SwitchTwoRing2")
	    Gas2Gac:RetEquipDuraValue(Player.m_Conn, RingLeftInfo[3], EEquipPart.eRingLeft, RingLeftRet["DuraState"], RingLeftRet["CurDuraValue"], RingLeftRet["MaxDuraValue"])
	end
	
	local nRBigID,nRIndex,nLBigID,nLIndex = 0,0,0,0
	if RingRightInfo ~= nil then
		nRBigID = RingRightInfo[1]
		nRIndex = RingRightInfo[2]
	end
	if RingLeftInfo ~= nil then
		nLBigID = RingLeftInfo[1]
		nLIndex = RingLeftInfo[2]
	end
	
	Gas2Gac:RetSwitchTwoRing(Connection,nRBigID,nRIndex,nLBigID,nLIndex)
end

--交换左右戒指,直接在装备栏上进行的
function Gac2Gas:SwitchTwoRing(Connection)
	if Connection.m_Player:IsInBattleState() then
		MsgToConn(Connection,1024)
		return
	end
	
	if Connection.m_Player:IsInForbitUseWeaponState() then
		MsgToConn(Connection,1028)
		return
	end		
	
	local data = {["PlayerId"] = Connection.m_Player.m_uID }
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then 
			SwitchTwoRing(Connection,Player,Data)
		else
			MsgToConn(Connection,Data)
			return
		end 
	end	
	--local entry = (g_DBTransDef["GasUseItemDB"])
	CallAccountManualTrans(Connection.m_Account, GasUseItemDB, "SwitchTwoRing", CallBack, data)
end

local function SwitchWeapon(Connection,Player,data)

	local WeaponInfo = data["AssociateWeaponInfo"]
	local AssociateWeaponInfo =  data["WeaponInfo"]
	local WeaponInfoRet = data["AssociateWeaponInfoTbl"]
	local AssociateWeaponInfoRet = data["WeaponInfoTbl"]
	local ErrorWeapon = Player.m_ErrorEquipTbl[EEquipPart.eAssociateWeapon]
	local ErrorAssociateWeapon = Player.m_ErrorEquipTbl[EEquipPart.eWeapon]	
	local new_CommonlySkillName = data["new_CommonlySkillName"]
	local ShortCutRet = data["ShortCutRet"]
	local equip_mgr = g_GetEquipMgr()
	local nWeaponBigID,nWeaponIndex,nAssociateWeaponBigID,nAssociateWeaponIndex = 0,0,0,0
    local Weapon_equip
	if WeaponInfo then
		Player:SetEquipInfo(WeaponInfo[1],WeaponInfo[2],WeaponInfo[3],EEquipPart.eWeapon)
		Weapon_equip = equip_mgr:GetEquipFromRet(tonumber(WeaponInfo[3]), Player.m_uID, tonumber(WeaponInfo[1]), WeaponInfo[2],WeaponInfoRet)
		Player:SetPlayerEquipPhase(EEquipPart.eWeapon,Weapon_equip.uIntensifyPhase,Weapon_equip.CurAdvancePhase)
		Weapon_equip:DisablePropty(Player, EEquipPart.eAssociateWeapon, "SwitchWeapon1")
		nWeaponBigID = WeaponInfo[1]
		nWeaponIndex = WeaponInfo[2]
		Gas2Gac:RetEquipDuraValue(Player.m_Conn, WeaponInfo[3], EEquipPart.eAssociateWeapon, 2, WeaponInfoRet["MaxDuraValue"], WeaponInfoRet["MaxDuraValue"])
	else
		Player:ClearEquipInfo(EEquipPart.eWeapon)
		Player:SetPlayerEquipPhase(EEquipPart.eWeapon,0,0)
		Player:CppInitMHNA(new_CommonlySkillName,"无类型","",2.0,false) 
	end
	Gas2Gac:ReCommonlySkill(Connection,new_CommonlySkillName)
	
	local AssociateWeapon_equip
	if AssociateWeaponInfo then
		Player:SetEquipInfo(AssociateWeaponInfo[1],AssociateWeaponInfo[2],AssociateWeaponInfo[3],EEquipPart.eAssociateWeapon)
		AssociateWeapon_equip = equip_mgr:GetEquipFromRet(tonumber(AssociateWeaponInfo[3]), Player.m_uID, tonumber(AssociateWeaponInfo[1]), AssociateWeaponInfo[2],AssociateWeaponInfoRet)
		Player:SetPlayerEquipPhase(EEquipPart.eAssociateWeapon,AssociateWeapon_equip.uIntensifyPhase,AssociateWeapon_equip.CurAdvancePhase)
		AssociateWeapon_equip:DisablePropty(Player, EEquipPart.eWeapon, "SwitchWeapon2")
		nAssociateWeaponBigID = AssociateWeaponInfo[1]
		nAssociateWeaponIndex = AssociateWeaponInfo[2]
		Gas2Gac:RetEquipDuraValue(Player.m_Conn, AssociateWeaponInfo[3], EEquipPart.eWeapon, 2, AssociateWeaponInfoRet["MaxDuraValue"], AssociateWeaponInfoRet["MaxDuraValue"])
	else
		Player:ClearEquipInfo(EEquipPart.eAssociateWeapon)
		Player:SetPlayerEquipPhase(EEquipPart.eAssociateWeapon,0,0)
		Player:CppInitAHNA(nil,"无类型","",0,false)
	end
	
	Player.m_ErrorEquipTbl[EEquipPart.eWeapon] = ErrorWeapon
	if not Player.m_ErrorEquipTbl[EEquipPart.eWeapon] then
		if WeaponInfo then
		    Weapon_equip:EnablePropty(Player, EEquipPart.eWeapon, "SwitchWeapon1")
		    Gas2Gac:RetEquipDuraValue(Player.m_Conn, WeaponInfo[3], EEquipPart.eWeapon, WeaponInfoRet["DuraState"], WeaponInfoRet["CurDuraValue"], WeaponInfoRet["MaxDuraValue"])
		    if WeaponInfoRet["CurDuraValue"] > 0 then
		        Weapon_equip:SetWeaponSkill(Player,WeaponInfo[1],WeaponInfo[2],false)
		    else
		    	Player:CppInitMHNA(new_CommonlySkillName,"无类型","",2.0,false)
		    end
		end
	end
	Player.m_ErrorEquipTbl[EEquipPart.eAssociateWeapon] = ErrorAssociateWeapon
	if not Player.m_ErrorEquipTbl[EEquipPart.eAssociateWeapon] then
		if AssociateWeaponInfo then
		    AssociateWeapon_equip:EnablePropty(Player, EEquipPart.eAssociateWeapon, "SwitchWeapon2")
		    Gas2Gac:RetEquipDuraValue(Player.m_Conn, AssociateWeaponInfo[3], EEquipPart.eAssociateWeapon, AssociateWeaponInfoRet["DuraState"], AssociateWeaponInfoRet["CurDuraValue"], AssociateWeaponInfoRet["MaxDuraValue"])
	        if AssociateWeaponInfoRet["CurDuraValue"] > 0 then
		        AssociateWeapon_equip:SetAssociateSkill(Player,AssociateWeaponInfo[1],AssociateWeaponInfo[2],false)
		    else
		    	Player:CppInitAHNA(nil,"无类型","",0,false)
		    end
		end
	end
	
	Player:SetEquipIndexID(EEquipPart.eWeapon,nWeaponBigID,nWeaponIndex)
	Player:SetEquipIndexID(EEquipPart.eAssociateWeapon,nAssociateWeaponBigID,nAssociateWeaponIndex)
	Gas2Gac:RetSwitchWeapon(Connection,nWeaponBigID,nWeaponIndex,nAssociateWeaponBigID,nAssociateWeaponIndex)
	Player:GetShortInfoByRet(ShortCutRet)
	if AssociateWeaponInfo then
		Gas2Gac:ReAssociateAttr(Connection,true) 
	else
		Gas2Gac:ReAssociateAttr(Connection,false)
	end
end



function Gac2Gas:SwitchWeapon(Connection)
	if Connection.m_Player == nil then
		return
	end
		
	if Connection.m_Player:IsInBattleState() then
		MsgToConn(Connection,1024)
		return
	end
	
	if Connection.m_Player:IsInForbitUseWeaponState() then
		MsgToConn(Connection,1028)
		return
	end		
	
	local data = {
				["PlayerId"] = Connection.m_Player.m_uID
				,["Class"] = Connection.m_Player:CppGetClass() }
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then
			SwitchWeapon(Connection,Player,Data)
		else
			if IsNumber(Data) and Data ~= 0 then
				MsgToConn(Connection,Data)
			end
			return
		end
	end
	CallAccountManualTrans(Connection.m_Account, GasUseItemDB,"SwitchWeapon",CallBack,data)
end

local function CommonFetchEquip(Connection,Player,eEquipPart,data, exactFetch, charID)
	local equip_mgr = g_GetEquipMgr()
	local EquipInfo = data["EquipInfo"]
	local OldEquipRet = data["EquipRet_Old"]
	local nRoomIndex = 	data["RoomIndex"]
	local nPos = data["Pos"]
	local new_CommonlySkillName = data["new_CommonlySkillName"]
	local ShortCutRet = data["ShortCutRet"]
	local EquipSuitTbl_Old = data["EquipSuitTbl_Old"]
	local RemoveJingLingSkillIndex = data["RemoveJingLingSkillIndex"]
	Player:SetEquipIndexID(eEquipPart,0,0)
	local equipDuraState = OldEquipRet["DuraState"]
	local MaxDuraValue = OldEquipRet["MaxDuraValue"]
	local CurDuraValue = OldEquipRet["CurDuraValue"]
	local old_equip 
	---------------------------------高速逻辑--------------------------------
	if EquipInfo ~= nil then
		old_equip = equip_mgr:GetEquipFromRet(tonumber(EquipInfo[3]), Player.m_uID, tonumber(EquipInfo[1]), EquipInfo[2],OldEquipRet)
	    
		if old_equip.sSuitName ~= "" and old_equip.sSuitName ~= nil and EquipSuitTbl_Old ~= nil  then
			g_EquipMgr:GetSuitAttributeBySuitName(Player,false,EquipSuitTbl_Old[1],EquipSuitTbl_Old[2], EquipSuitTbl_Old[3], old_equip.BaseLevel)
		end
		--删除相关属性
        old_equip:DisablePropty(Player,eEquipPart, "CommonFetchEquip" .. exactFetch)
        Player.m_ErrorEquipTbl[eEquipPart] = false
		Player:SetPlayerEquipPhase(eEquipPart,0,0)
		Player:ClearEquipInfo(eEquipPart)
	end

	Player:SetArmorValue()
	Gas2Gac:RetEquipDuraValue(Connection, EquipInfo[3], eEquipPart, 2, CurDuraValue, MaxDuraValue)
	Gas2Gac:RetFetchEquipByPart(Connection,eEquipPart,false)
	if nResID ~= 0 then
		Gas2Gac:AoiAddEquip(Player:GetIS(0),Player:GetEntityID(),eEquipPart,EquipInfo[1],EquipInfo[2])
		Player:TriggerSetupWeapon()
	end
	if eEquipPart == EEquipPart.eWeapon  then 
		Player:CppInitMHNA(new_CommonlySkillName,"无类型","",2.0,false)
		Gas2Gac:ReCommonlySkill(Connection,new_CommonlySkillName)
		Player:GetShortInfoByRet(ShortCutRet)
	elseif eEquipPart == EEquipPart.eAssociateWeapon then
		Player:CppInitAHNA(nil,"无类型","",0,false)
		Gas2Gac:ReAssociateAttr(Connection,false)
	end
	equip_mgr:SetCharJingLingSkill(Player, false, RemoveJingLingSkillIndex, charID)
	--设置三阶段的天赋
    if old_equip.uIntenTalentIndex and old_equip.uIntenTalentIndex > 0 then
        old_equip:SetIntensifyAddTalent(Player, old_equip.uIntenTalentIndex, old_equip.BaseLevel, false)
	end
    local haveDemagedEquip = data["HaveDemagedEquip"]
	if haveDemagedEquip == 0 then
        Gas2Gac:RetShowEquipDuraWnd(Connection, false) 
	end
	Player:TriggerChangeEquip()
	CGasFightingEvaluation.SaveFightingEvaluationInfo(Player)
end

local function FetchEquip(Connection,Player,eEquipPart,data)
	CommonFetchEquip(Connection,Player,eEquipPart,data, "Fetch")
end
	
function Gac2Gas:FetchEquipByPart(Connection,eEquipPart,nRoomIndex,nPos)

	if not (Connection.m_Player and IsCppBound( Connection.m_Player)) then
		return
	end
	
	if Connection.m_Player.m_ItemBagLock then
		MsgToConn(Connection,160014)
		return
	end
	
	local data = { ["RoomIndex"] = nRoomIndex
					,["Pos"] = nPos
					,["EquipPart"] = eEquipPart
					,["PlayerId"] = Connection.m_Player.m_uID
					,["Class"] = Connection.m_Player:CppGetClass()
					,["sceneName"] = Connection.m_Player.m_Scene.m_SceneName}
					
	if Connection.m_Player:IsInBattleState() then
		MsgToConn(Connection,1029)
		return
	end				
	
	if Connection.m_Player:IsInForbitUseWeaponState() then
		MsgToConn(Connection,1030)
		return
	end	
	local charID = Connection.m_Player.m_uID
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then
			CommonFetchEquip(Connection,Player,eEquipPart,Data, "Fetch", charID)
		else
			MsgToConn(Connection,Data)
			return
		end
		
	end	
	--local entry = (g_DBTransDef["GasUseItemDB"])
	CallAccountManualTrans(Connection.m_Account, GasUseItemDB, "FetchEquip", CallBack, data)
end


function DelEquip(Connection,Player,eEquipPart,Data, charID)
	local EquipInfo = Data["EquipInfo"]
	local OldEquipRet = Data["EquipRet_Old"]
	local new_CommonlySkillName = Data["new_CommonlySkillName"]
	local ShortCutRet = Data["ShortCutRet"]
	local EquipSuitTbl_Old = Data["EquipSuitTbl_Old"]
	local equip_mgr = g_GetEquipMgr()
	local RemoveJingLingSkillIndex = Data["RemoveJingLingSkillIndex"]
	local equipDuraState = OldEquipRet["DuraState"]
	local equipCurDuraValue = OldEquipRet["CurDuraValue"]
	local equipMaxDuraValue = OldEquipRet["MaxDuraValue"]
	local old_equip 
	Player:SetEquipIndexID(eEquipPart,0,0)
	
	if EquipInfo ~= nil then
	    old_equip = equip_mgr:GetEquipFromRet(tonumber(EquipInfo[3]), Player.m_uID, tonumber(EquipInfo[1]), EquipInfo[2],OldEquipRet)
		
		if old_equip.sSuitName ~= "" and old_equip.sSuitName ~= nil and EquipSuitTbl_Old ~= nil  then
			g_EquipMgr:GetSuitAttributeBySuitName(Player,false,EquipSuitTbl_Old[1],EquipSuitTbl_Old[2],EquipSuitTbl_Old[3], old_equip.BaseLevel)
		end
        
        --删除相关属性
        old_equip:DisablePropty(Player,eEquipPart, "DelEquip")
        Player.m_ErrorEquipTbl[eEquipPart] = false
		Player:SetPlayerEquipPhase(eEquipPart,0,0)
		Player:ClearEquipInfo(eEquipPart)
		local haveDemagedEquip = Data["HaveDemagedEquip"]
		if haveDemagedEquip == 0 then
            Gas2Gac:RetShowEquipDuraWnd(Connection, false) 
		end
		Gas2Gac:RetEquipDuraValue(Connection, EquipInfo[3], eEquipPart, 2, equipCurDuraValue, equipMaxDuraValue)
		CGasFightingEvaluation.SaveFightingEvaluationInfo(Player)
	end	
	
	Player:SetArmorValue()
	Gas2Gac:RetFetchEquipByPart(Connection,eEquipPart,true)
	if eEquipPart == EEquipPart.eWeapon  then 
		Player:CppInitMHNA(new_CommonlySkillName,"无类型","",2.0,false)
		Gas2Gac:ReCommonlySkill(Connection,new_CommonlySkillName)
		Player:GetShortInfoByRet(ShortCutRet)			
	elseif eEquipPart == EEquipPart.eAssociateWeapon then
		Player:CppInitAHNA(nil,"无类型","",0,false)	
	end
	equip_mgr:SetCharJingLingSkill(Player, false, RemoveJingLingSkillIndex, charID)
    if old_equip.uIntenTalentIndex and old_equip.uIntenTalentIndex > 0 then
        old_equip:SetIntensifyAddTalent(Player, old_equip.uIntenTalentIndex, old_equip.BaseLevel, false)
	end
	if nResID ~= 0 then
		Gas2Gac:AoiAddEquip(Player:GetIS(0),Player:GetEntityID(),eEquipPart,EquipInfo[1],EquipInfo[2])
		Player:TriggerSetupWeapon()
	end
	Player:TriggerChangeEquip()
end

--需要记录log，直接删除装备
function Gac2Gas:DelEquip(Connection,eEquipPart)
	if Connection.m_Player.m_ItemBagLock then
		MsgToConn(Connection,160013)
		return
	end
	
	local data = {["EquipPart"] = eEquipPart
					,["PlayerId"] = Connection.m_Player.m_uID
					,["Class"] = Connection.m_Player:CppGetClass(),
					["sceneName"] = Connection.m_Player.m_Scene.m_SceneName}
					
	if Connection.m_Player:IsInBattleState() then
		MsgToConn(Connection,1029)
		return
	end				
	
	if Connection.m_Player:IsInForbitUseWeaponState() then
		MsgToConn(Connection,1030)
		return
	end				
    local charID = Connection.m_Player.m_uID
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then
			DelEquip(Connection,Player, eEquipPart,Data, charID) 
		else
			MsgToConn(Connection,Data)
			return
		end
	end	
	--local entry = (g_DBTransDef["GasUseItemDB"])
	CallAccountManualTrans(Connection.m_Account, GasUseItemDB, "DelEquip", CallBack, data)	
end

function Gac2Gas:RClickFetchEquipByPart(Connection, eEquipPart)
	local Player = Connection.m_Player
	if not IsCppBound(Player) then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Connection,160014)
		return
	end
	local data = { 
					["EquipPart"] = eEquipPart
					,["PlayerId"] = Player.m_uID
					,["Class"] = Player:CppGetClass()
					,["sceneName"] = Player.m_Scene.m_SceneName}
					
	if Player:IsInBattleState() then
		MsgToConn(Connection,1029)
		return
	end				
	
	if Player:IsInForbitUseWeaponState() then
		MsgToConn(Connection,1030)
		return
	end	
    local charID = Player.m_uID
	local function CallBack(bool,Data)
		if bool then
			if IsTable(Data) then 
				CommonFetchEquip(Connection,Player,eEquipPart,Data, "RClick", charID)
			else
				MsgToConn(Connection,Data)
			end
		else
			if IsString(Data) then
				MsgToConn(Connection,1037,Data)
			end
		end
	end	
	
	--local entry = (g_DBTransDef["GasUseItemDB"])
    
	CallAccountManualTrans(Connection.m_Account, GasUseItemDB, "RClickFetchEquip", CallBack, data)
end
