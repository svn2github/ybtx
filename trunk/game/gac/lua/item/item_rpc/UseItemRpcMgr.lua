gac_require "framework/main_frame/PropRes"
gac_gas_require "item/Equip/EquipDef"

CUseItemRpcMgr = class()

local nAllPartNum = 13
local function UpdateBaseSysWndTooltip()
    if g_GameMain.m_CSMSellOrderWnd:IsShow() then
       g_GameMain.m_CSMSellOrderWnd:UpdateCSMSellListTooltip()
    end
    if g_GameMain.m_CSMSellCharOrderWnd ~= nil and
        g_GameMain.m_CSMSellCharOrderWnd:IsShow() then
        g_GameMain.m_CSMSellCharOrderWnd:UpdateCSMSellListTooltip()
    end
    if g_GameMain.m_PlayersTradeWnd:IsShow() then
        g_GameMain.m_PlayersTradeWnd:UpdatePlayersTradeToo1tip()
    end
    if g_GameMain.m_NPCShopSell:IsShow() then
        g_GameMain.m_NPCShopSell:NpcSellUpdateMultiTooltips()
    end
    if g_GameMain.m_NPCShopPlayerSold:IsShow() then
        g_GameMain.m_NPCShopPlayerSold:UpdateNpcSoldItemTooltip()
    end
end

local function UseSkillItem(nRoomIndex,nPos,nBigID,nIndex,nItemID)
	g_GameMain.m_BagSpaceMgr:DelItemByRoomPos(nRoomIndex,nPos,nItemID)
	local type = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"CoolType")
	local time = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"CoolDown")*1000
	g_GameMain.m_BagSpaceMgr:UpdateRoomPos(nRoomIndex,nPos, type, time)
	g_DynItemInfoMgr:DelItemInfo(nItemID)
end

--使用装备 即向装备面板添加装备
local function UseEquip(nRoomIndex,nPos,nBigID,nIndex,nItemID,eEquipPart)
	g_ItemInfoMgr:PlayItemSound(nBigID,nIndex)
		
	local EquipPart,Item,PartInfo = 0,0,0
	if g_ItemInfoMgr:IsWeapon(nBigID) then
		EquipPart = eEquipPart
	elseif g_ItemInfoMgr:IsArmor(nBigID) then
		EquipPart = EquipPartNameToNum[g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")]
	elseif g_ItemInfoMgr:IsAssociateWeapon(nBigID) then
		EquipPart = EEquipPart.eAssociateWeapon
	elseif g_ItemInfoMgr:IsJewelry(nBigID) then
		EquipPart = EquipPartNameToNum[g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipPart")]
	elseif g_ItemInfoMgr:IsRing(nBigID) then
		EquipPart = eEquipPart
	end
	local Wnd = g_GameMain.m_RoleStatus
	local itemDynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	local equip_type, equip_name, nBindingType = itemDynInfo.m_nBigID, itemDynInfo.m_nIndex, itemDynInfo.BindingType
	
	for i = 1, nAllPartNum do
		if EquipPart == Wnd.Part[i].PartType then
			Item = Wnd.Part[i].m_Info or 0
			Wnd.Part[i].m_Info = CItem:new(equip_type, equip_name, nItemID, nBindingType, nil)
			PartInfo = Wnd.Part[i]
			local RenderObj = g_MainPlayer:GetRenderObject()
			g_DelPieceByPart(EquipPart,RenderObj)
			local PartDynInfo = g_DynItemInfoMgr:GetDynItemInfo(PartInfo.m_Info:GetItemID())
			if EquipPart == EEquipPart.eWeapon then
				local EquipTypei = g_ItemInfoMgr:GetItemInfo(equip_type,equip_name,"EquipType")
				CUseItemRpcMgr.SetAttackEffect(equip_type,equip_name,EEquipPart.eWeapon,g_MainPlayer,g_MainPlayer.m_Properties:GetHorseType(),
				g_MainPlayer.m_Properties:GetBehaviorType(),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
				g_MainPlayer.m_uWeaponType = EquipTypei
				local AssociateWeapon = Wnd.Part[6]
				local ruslut , HandType = g_ItemInfoMgr:EquipTypeCheck(EquipTypei,g_MainPlayer:CppGetClass())
				if HandType == "双" and AssociateWeapon.m_Info ~= nil then
					if Item == 0 then
						Item = AssociateWeapon.m_Info
					end
					g_MainPlayer:GetRenderObject():RemovePiece( "offweapon" )
					Wnd:DelWndRolePiece(EEquipPart.eAssociateWeapon)
					CUseItemRpcMgr.CancelAttackEffect(EEquipPart.eAssociateWeapon)
					
					AssociateWeapon.iconbtn:SetMouseOverDescAfter("")
					AssociateWeapon.iconbtn:DelFlashAttention()
					g_DelWndImage(AssociateWeapon.iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)
					g_GameMain.m_EquipUpIntensifyWnd:CloseSelf(Item:GetItemID())
					local AssociateDynInfo = g_DynItemInfoMgr:GetDynItemInfo(AssociateWeapon.m_Info:GetItemID())
					AssociateWeapon.m_Info = nil
				end
			elseif EquipPart == EEquipPart.eAssociateWeapon then
				CUseItemRpcMgr.SetAttackEffect(equip_type,equip_name,EEquipPart.eAssociateWeapon,g_MainPlayer,g_MainPlayer.m_Properties:GetHorseType(),
				g_MainPlayer.m_Properties:GetBehaviorType(),g_MainPlayer:CppGetClass(),g_MainPlayer.m_Properties:GetSex())
				local Weapon = Wnd.Part[12]
				if Weapon.m_Info ~= nil then
					local WeaponType = g_ItemInfoMgr:GetItemInfo(Weapon.m_Info:GetBigID(),Weapon.m_Info:GetIndex(),"EquipType")
					local _, HandType = g_ItemInfoMgr:EquipTypeCheck(WeaponType,g_MainPlayer:CppGetClass())
					if HandType == "双" then
						Item = Wnd.Part[12].m_Info 
						g_MainPlayer:GetRenderObject():RemovePiece( "weapon" )
						Wnd:DelWndRolePiece(EEquipPart.eWeapon)
						CUseItemRpcMgr.CancelAttackEffect(EEquipPart.eWeapon)
						Weapon.iconbtn:SetMouseOverDescAfter("")
						Weapon.iconbtn:DelFlashAttention()
						g_DelWndImage(Weapon.iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)						
						g_GameMain.m_EquipUpIntensifyWnd:CloseSelf(Item:GetItemID())
						g_MainPlayer.m_uWeaponType = ""
						Weapon.m_Info = nil
					end		
				end			
			end
			break
		end
	end
	g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"SmallIcon"), PartInfo.iconbtn, -2, IP_ENABLE, IP_CLICKDOWN)
	--设置ToolTips
	g_SetItemRichToolTips(PartInfo.iconbtn,nBigID,nIndex,nItemID)
	Wnd:UpdateAllRoleInfo()
	Wnd:DelWndRolePiece(EquipPart)
	local bool = g_IsUpdateModel(eEquipPart)
	if bool then
		UpdateModel(g_MainPlayer)
		if EEquipPart.eWear == eEquipPart or EEquipPart.eHead == eEquipPart then
			UpdateFollowerCoreInfo(g_MainPlayer)
		end
		Wnd.m_bLoadPiece = nil
		Wnd:DrawRole()
	end
	g_GameMain.m_RoleStatus:UpdataEquipTips()
	g_GameMain.m_RoleStatus:ShowErrorEquipInfo()
	g_GameMain.m_RoleStatus:SetPieceSkillNameToNumberTbl()
	g_GameMain.m_EquipUpIntensifyWnd:ChangeEquipInfo(PartInfo)
	if g_GameMain.m_CommonSkillWnd:IsShow() then
		UpdateSkillAllWndRect()
	end
	g_GameMain.m_EquipUpIntensifyWnd:UpdataAdvanceInfo()
	UpdateBaseSysWndTooltip()

end

--nRoomIndex,nPos都为0，表示直接删除就不需要在物品栏再创建了
--删除装备时回调的函数
function CUseItemRpcMgr.RetFetchEquipByPart(eEquipPart, bDel)
	local Item,PartInfo = 0,0
	local Wnd = g_GameMain.m_RoleStatus
	for i = 1, nAllPartNum do
		if eEquipPart == Wnd.Part[i].PartType then
			Item = Wnd.Part[i].m_Info or 0
			PartInfo = Wnd.Part[i]
			local RenderObj = g_MainPlayer:GetRenderObject()
			g_DelPieceByPart(eEquipPart,RenderObj)
			if eEquipPart == EEquipPart.eWeapon then
				CUseItemRpcMgr.CancelAttackEffect(EEquipPart.eWeapon)
				g_MainPlayer.m_uWeaponType=""
			elseif eEquipPart == EEquipPart.eAssociateWeapon then
				CUseItemRpcMgr.CancelAttackEffect(EEquipPart.eAssociateWeapon)	
			end
			Wnd.Part[i].m_Info = nil
			g_GameMain.m_EquipUpIntensifyWnd:CloseSelf(Item:GetItemID())
			break
		end
	end
	assert(Item ~= 0)
	
	--设置窗体为enable
	Wnd:UpdateAllRoleInfo()
	Wnd:DelWndRolePiece(eEquipPart)
	PartInfo.iconbtn:SetMouseOverDescAfter("")
	PartInfo.iconbtn:DelFlashAttention()
	g_DelWndImage(PartInfo.iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)
	if(bDel) then --直接丢弃
		g_DynItemInfoMgr:DelItemInfo(Item:GetItemID())
	end
	
	--nResID==0表示不需要换装备,把衣服脱了就设置默认的装备
	local bool = g_IsUpdateModel(eEquipPart)
	if bool then
		UpdateModel(g_MainPlayer)
		if EEquipPart.eWear == eEquipPart or EEquipPart.eHead == eEquipPart then
			UpdateFollowerCoreInfo(g_MainPlayer)
		end
		Wnd.m_bLoadPiece = nil
		Wnd:DrawRole()
	end
	
	g_GameMain.m_RoleStatus:UpdataEquipTips()
	SetEvent( Event.Test.FetchEquip,true, "" )
	UpdateBaseSysWndTooltip()
	g_GameMain.m_RoleStatus:SetPieceSkillNameToNumberTbl()
	g_GameMain.m_RoleStatus:ShowErrorEquipInfo()
	if g_GameMain.m_CommonSkillWnd:IsShow() then
		UpdateSkillAllWndRect()
	end
	g_GameMain.m_EquipUpIntensifyWnd:UpdataAdvanceInfo()
	g_ItemInfoMgr:PlayItemSound(Item:GetBigID(),Item:GetIndex())
	
	--同步刷新修理装备面板的数据
	if g_GameMain.m_NewFixEquipWnd:IsShow() then
		g_GameMain.m_CEquipInRoleWnd:SetData()
		g_GameMain.m_CEquipInBagWnd:SetData()
	end
end

--戒指交换函数
function CUseItemRpcMgr.RetSwitchTwoRing(Connection,nRBigID,nRIndex,nLBigID,nLIndex)
	local Wnd = g_GameMain.m_RoleStatus
	Wnd.Part[4].m_Info,Wnd.Part[5].m_Info = Wnd.Part[5].m_Info,Wnd.Part[4].m_Info
	if nRBigID ~= 0 then
		g_DelWndImage(Wnd.Part[4].iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)
		Wnd.Part[4].iconbtn:DelFlashAttention()
	end
	if nLBigID ~= 0 then
		g_DelWndImage(Wnd.Part[5].iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)
		Wnd.Part[5].iconbtn:DelFlashAttention()
	end
	
	if nRBigID ~= 0 then
		--右戒指的信息
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(nRBigID,nRIndex,"SmallIcon")
		g_LoadIconFromRes(SmallIcon, Wnd.Part[5].iconbtn, -2, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_EquipUpIntensifyWnd:ChangeEquipInfo(Wnd.Part[5])
	end
	if nLBigID ~= 0 then
		--左戒指的信息
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(nLBigID,nLIndex,"SmallIcon")
		g_LoadIconFromRes(SmallIcon, Wnd.Part[4].iconbtn, -2, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_EquipUpIntensifyWnd:ChangeEquipInfo(Wnd.Part[4],nLBigID,nLIndex,Wnd.Part[4].m_Info:GetItemID())
	end
	g_GameMain.m_RoleStatus:UpdataEquipTips()
	g_GameMain.m_RoleStatus:ShowErrorEquipInfo()
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()	
	SetEvent(Event.Test.Switch2Ring,true)
	UpdateBaseSysWndTooltip()
	
end

--武器交换函数
function CUseItemRpcMgr.RetSwitchWeapon(Connection,nWeaponBigID,nWeaponIndex,nAssociateWeaponBigID,nAssociateWeaponIndex)
	local Wnd = g_GameMain.m_RoleStatus
	Wnd.Part[12].m_Info,Wnd.Part[6].m_Info = Wnd.Part[6].m_Info,Wnd.Part[12].m_Info
	if nWeaponBigID ~= 0 then
		g_DelWndImage(Wnd.Part[6].iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)
		Wnd.Part[6].iconbtn:DelFlashAttention()
	end
	if nAssociateWeaponBigID ~= 0 then
		g_DelWndImage(Wnd.Part[12].iconbtn, 0, IP_ENABLE, IP_CLICKDOWN)
		Wnd.Part[12].iconbtn:DelFlashAttention()
	end
		
	if nWeaponBigID ~= 0 then
		--主手武器的信息
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(nWeaponBigID,nWeaponIndex,"SmallIcon")
		g_LoadIconFromRes(SmallIcon, Wnd.Part[12].iconbtn, -2, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_EquipUpIntensifyWnd:ChangeEquipInfo(Wnd.Part[12])
	end
	if nAssociateWeaponBigID ~= 0 then
		--副手武器的信息
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(nAssociateWeaponBigID,nAssociateWeaponIndex,"SmallIcon")
		g_LoadIconFromRes(SmallIcon, Wnd.Part[6].iconbtn, -2, IP_ENABLE, IP_CLICKDOWN)
		g_GameMain.m_EquipUpIntensifyWnd:ChangeEquipInfo(Wnd.Part[6])
	end
		
	g_GameMain.m_RoleStatus:UpdataEquipTips()
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
	g_GameMain.m_RoleStatus:UpdateAllRoleInfo()
	Wnd:DelWndRolePiece(EEquipPart.eWeapon)
	Wnd:DelWndRolePiece(EEquipPart.eAssociateWeapon)
	g_GameMain.m_RoleStatus:ShowErrorEquipInfo()
	UpdateModel(g_MainPlayer)
	Wnd.m_bLoadPiece = nil
	Wnd:DrawRole()
    UpdateBaseSysWndTooltip()
    
    --同步刷新修理装备面板的数据
    if g_GameMain.m_NewFixEquipWnd:IsShow() then
    	g_GameMain.m_CEquipInRoleWnd:SetData()
    	g_GameMain.m_CEquipInBagWnd:SetData()
    end
end

local UseItemExecuteFunc =
{
	[1] = nil,
	[2] = nil,
	[3] = UseSkillItem,					-- 使用药品
	[5] = UseEquip,						-- 装备武器
	[6] = UseEquip,  					-- 防具
	[7] = UseEquip,  					-- 暗器
	[8] = UseEquip,						-- 饰品
	[9] = UseEquip,
	[10] = UseEquip,
	[11] = UseEquip,
	[12] = UseEquip,					-- 动态饰品
}

--使用物品
function CUseItemRpcMgr.RetUseItem( Connection,nRoomIndex,nPos,nItemID,eEquipPart)
	local itemDynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	local nItemType, sItemName = itemDynInfo.m_nBigID, itemDynInfo.m_nIndex
	if UseItemExecuteFunc[nItemType]~= nil then
		UseItemExecuteFunc[nItemType](nRoomIndex, nPos, nItemType, sItemName, nItemID, eEquipPart)
	end
	--同步更新快捷栏的相应物品的数量
	local num = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag, nItemType, sItemName)
	g_GameMain.m_MasterSkillArea.m_WndSkill:UpdateItemCount(nItemType, sItemName, num)
	g_GameMain.m_MainSkillsToolBar.m_WndSkill:UpdateItemCount(nItemType, sItemName, num)
	
	--同步刷新修理装备面板的数据
	if g_GameMain.m_NewFixEquipWnd:IsShow() then
		g_GameMain.m_CEquipInRoleWnd:SetData()
		g_GameMain.m_CEquipInBagWnd:SetData()
	end
end
															
function CUseItemRpcMgr.AoiAddEquip(Connection,uEntityID,EquipPart,nBigID,nIndex)
	local Character = CCharacterFollower_GetCharacterByID(uEntityID)
	if Character == nil or Character.m_Properties.m_StaticSyncRootVariant == nil then -- 判断消息发送过来，对象已经不存在了。
		return
	end
	local ResID = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"ResID")
	if EquipPart == EEquipPart.eWeapon then
		CUseItemRpcMgr.SetAttackEffect(nBigID,nIndex , EEquipPart.eWeapon,Character,Character.m_Properties:GetHorseType(),
				Character.m_Properties:GetBehaviorType(),Character:CppGetClass(),Character.m_Properties:GetSex())
	elseif EquipPart == EEquipPart.eAssociateWeapon then
		CUseItemRpcMgr.SetAttackEffect(nBigID,nIndex,EEquipPart.eAssociateWeapon,Character,Character.m_Properties:GetHorseType(),
				Character.m_Properties:GetBehaviorType(),Character:CppGetClass(),Character.m_Properties:GetSex())
	end
	
	if ResID ~= 0 and ResID ~= "" then
		UpdateModel(Character)	
		if EEquipPart.eWear == EquipPart or EEquipPart.eHead == EquipPart then
			UpdateFollowerCoreInfo(g_MainPlayer)
		end
	end
end

---设置动作特效
function CUseItemRpcMgr.SetAttackEffect (nBigID,nIndex , EquipPart,Character, HorseType, BehaviorType, uClass, uSex)
	if Character.m_Properties.m_StaticSyncRootVariant == nil then
		return
	end
	--local HorseType = Character.m_Properties:GetHorseType()
	--local BehaviorType = Character.m_Properties:GetBehaviorType()
	local AniType=""
	local EquipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local AttackEffect = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"AttackEffect")
	if BehaviorType ~= "" and BehaviorType ~= nil then 
		AniType = "wf"..","..BehaviorType
--	elseif HorseType ~= "" and HorseType ~= nil then
--		if Character:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) then
--			AniType = "jt"..","..HorseType
--		elseif Character:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse) and EquipType ~= nil then		
--			AniType = EquipType..","..HorseType
--		elseif EquipType ~= nil then
--			AniType = EquipType
--		end
	else
		AniType = EquipType
	end
	local ArpFileName =  MakeAniFileName(uClass,uSex,AniType)
	Character:SetSpliceName(ArpFileName.."_skill/")
	Character:SetSkillFxFile("fx/setting/skill/"..ArpFileName.."_skill/")
	if EquipPart == EEquipPart.eWeapon and AttackEffect~= "" then 
		local FxTable = split(AttackEffect, ",")
		Character:SetAttackFxFile(FxTable[1]..ArpFileName.."_".. FxTable[2])
	end
end

--取消动作特效
function CUseItemRpcMgr.CancelAttackEffect(EquipPart)
	if EquipPart == EEquipPart.eWeapon then 
		g_MainPlayer:SetAttackFxFile("")
		g_MainPlayer:SetSkillFxFile("")
	elseif EquipPart == EEquipPart.eAssociateWeapon then
		g_MainPlayer:SetAttackFxFile("")
		g_MainPlayer:SetSkillFxFile("")
	end
end

function CUseItemRpcMgr.SpecailItemCoolDown(Conn,ItemName,time,coolDownType,nRoomIndex,nPos)
	g_GameMain.m_WndMainBag.m_ctItemRoom:BeginCoolDown(ItemName, time, coolDownType,nRoomIndex,nPos) 
end

function CUseItemRpcMgr.SetItemCoolDownTime(Conn, itemName, CoolDownTime, processTime)
	if g_MainPlayer.CoolDownTime == nil then g_MainPlayer.CoolDownTime = {} end
	if g_MainPlayer.CoolDownBeginTime == nil then g_MainPlayer.CoolDownBeginTime = {} end
	g_MainPlayer.CoolDownTime[itemName] = CoolDownTime
	g_MainPlayer.CoolDownBeginTime[itemName] = os.time() - processTime
end
