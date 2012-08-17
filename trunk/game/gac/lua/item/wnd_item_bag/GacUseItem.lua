gac_gas_require "activity/item/ItemUseInfoMgr"
cfg_load "item/CanUseItem_Common"

local BindingStyle1 = 1
--右键点击邮件文本附件，查看邮件内容
--参数：ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item,eEquipPart
local function OpenMailText( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos )
	if g_ItemInfoMgr:IsMailTextAttachment(nBigID) then
		g_WndMouse:ClearCursorAll()
		Gac2Gas:CheckMailTextAttachment(g_Conn,nRoomIndex,nPos)
		return
	end
end

--保存阵法后删除阵法书
local function DelBattleArrayBook( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item )
	local itemId = Item:GetItemID()
	g_GameMain.m_CreateBattleArrayWnd:GetBattleArrayBookInfo(itemId)
	g_GameMain.m_CreateBattleArrayWnd:OpenAndReEdit(nSmallID, 0, itemId) --0从包裹打开
	g_GameMain.m_CreateBattleArrayWnd.m_tblTmpBattleArrayBook = {g_StoreRoomIndex.PlayerBag, nPos	}
end

--使用帮会建筑卷轴创建建筑
local function UseTongBuildingItem( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item )
--	Gac2Gas:CreateBuilding(g_Conn, nSmallID, nRoomIndex, nPos)
	Gac2Gas:ClickItem(g_Conn, nBigID, nSmallID, nRoomIndex, nPos)
end

--答题结果
--local function AnswerResult( QuestionTypeName)
--
--
--	return true
--end
local BoxParameter = nil
local function OpenBoxitem( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos )
		if g_GameMain.m_BoxitemPickupWnd == nil then
			g_GameMain.m_BoxitemPickupWnd = CreateBoxitemPickupWnd(g_GameMain)
		end
		g_GameMain.m_BoxitemPickupWnd.m_CurPageNo = 1
		g_GameMain.m_BoxitemPickupWnd.m_ClickBagGrid = {nRoomIndex, srcPos}		--在物品栏中的位置
		g_WndMouse:ClearCursorAll()
		Gac2Gas:GetBoxitemInfo(g_Conn, nRoomIndex, nPos , nBigID, nSmallID)
end
--答题结束
function Gas2Gac:ShowBoxItemWnd(Conn, Result)

	local ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos, QuestionTypeName
	if BoxParameter == nil then
		return
	end
	ParentWnd = BoxParameter["ParentWnd"]
	nBigID = BoxParameter["nBigID"]
	nSmallID = BoxParameter["nSmallID"]
	nRoomIndex = BoxParameter["nRoomIndex"]
	nPos = BoxParameter["nPos"]
	Item = BoxParameter["Item"]
	eEquipPart = BoxParameter["eEquipPart"]
	srcPos = BoxParameter["srcPos"]
	QuestionTypeName = BoxParameter["QuestionTypeName"]
	BoxParameter = nil

	if Result == true then
		OpenBoxitem( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos )
	else
		--print( "删除" .. Item:GetItemID() .. "盒子")
		--删除将打开的盒子
		Gac2Gas:OnUseDelItemByPos(Conn, nRoomIndex, nPos, 1)
	end
end

function Gas2Gac:InitBoxItemWnd(Conn,Result)
	local ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos, QuestionTypeName
	QuestionTypeName = BoxParameter["QuestionTypeName"]
	if not Result then
		Gas2Gac:RetShowEssayQuestionWnd( g_Conn, QuestionTypeName, "", 1)
		return
	end
	if BoxParameter == nil then
		return
	end
	ParentWnd = BoxParameter["ParentWnd"]
	nBigID = BoxParameter["nBigID"]
	nSmallID = BoxParameter["nSmallID"]
	nRoomIndex = BoxParameter["nRoomIndex"]
	nPos = BoxParameter["nPos"]
	Item = BoxParameter["Item"]
	eEquipPart = BoxParameter["eEquipPart"]
	srcPos = BoxParameter["srcPos"]

	BoxParameter = nil
	OpenBoxitem( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos )
end
--右键点击盒子物品，打开物品拾取面板
local function OpenBoxitemDropWnd( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos )
	if g_ItemInfoMgr:IsBoxitem(nBigID) then
		--加入条件 是否要求答题   是否 答对题目
--		print( "ParentWnd = ", ParentWnd, "nBigID = ", nBigID, "nSmallID = ", nSmallID,"nRoomIndex = ", nRoomIndex, "nPos = ", nPos,"Item = ", Item, "eEquipPart = ", eEquipPart,"srcPos = ", srcPos)
		local QuestionTypeName = BoxItem_Common(nSmallID,"QuestionTypeName")
--		BoxParameter =
--		{
--			["ParentWnd"] 	= ParentWnd,
--			["nBigID"]    	= nBigID,
--			["nSmallID"]  	= nSmallID,
--			["nRoomIndex"]	= nRoomIndex,
--			["nPos"]      	= nPos,
--			["Item"]      	= Item,
--			["eEquipPart"]	= eEquipPart,
--			["srcPos"]    	= srcPos,
--			["QuestionTypeName"] = QuestionTypeName
--		}
		if QuestionTypeName ~= nil and QuestionTypeName ~= "" then
			local param = QuestionControl_Common(QuestionTypeName)
			if tonumber(param.QuestionMode) == 2 then
				BoxParameter =
				{
					["ParentWnd"] 	= ParentWnd,
					["nBigID"]    	= nBigID,
					["nSmallID"]  	= nSmallID,
					["nRoomIndex"]	= nRoomIndex,
					["nPos"]      	= nPos,
					["Item"]      	= Item,
					["eEquipPart"]	= eEquipPart,
					["srcPos"]    	= srcPos,
					["QuestionTypeName"] = QuestionTypeName
				}
				Gac2Gas:SelectBoxState(g_Conn, nRoomIndex, nPos )
			else
				--print("配置表　QuestionControl_Common ".. QuestionTypeName .." 项 的QuestionMode 填写错误 应填写为 2")
			end
		else
			OpenBoxitem( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos )
		end
		return
	end
	if g_ItemInfoMgr:IsTurntableItem(nBigID) then
		OpenBoxitem( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos )
		return
	end
end

--打开文本
local function OpenTextItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos)
	local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(nBigID, nSmallID,"DisplayName")
	local Content	= g_ItemInfoMgr:GetItemLanInfo(nBigID, nSmallID,"Content")
	local wnd = g_GameMain.m_TextItemWnd
	wnd:ShowWnd(true)
	wnd:SetWndText(DisplayName)
	wnd.m_Text:SetWndText(Content)
end


local function UseItem(context, nIndex)
	if nIndex == MB_BtnOK then
		local nRoomIndex = context[1]
		local nPos = context[2]
		local nBigID =  context[3]
		local nIndex = context[4]
		local eEquipPart = context[5]
		if eEquipPart == nil then
			eEquipPart = 0
		end
		Gac2Gas:UseItem(g_Conn,nRoomIndex,nPos,nBigID,nIndex,eEquipPart)
	end
	g_WndMouse:ClearCursorAll()
	return true
end

--使用装备函数
local function UseWeapon(ParentWnd, nBigID, nIndex, nRoomIndex, nPos, Item,eEquipPart)
	local DynInfo
	if g_ItemInfoMgr:IsStaticWeapon(nBigID) or  g_ItemInfoMgr:IsAssociateWeapon(nBigID) then
		DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Item:GetItemID())
	else
		MsgClient(1001)
		return
	end
	if not DynInfo then	
		local ItemId = Item:GetItemID() or 0
		LogErr("物品动态信息错误" .. "sBigID:" .. nBigID .. "sIndex:" .. nIndex  .. "ID:" .. ItemId)
	end
	local WeaponItem = g_GameMain.m_RoleStatus.Part[12].m_Info
	local AssociateWeaponItem = g_GameMain.m_RoleStatus.Part[6].m_Info
	local WeaponInfo,AssociateWeaponInfo
	if WeaponItem ~= nil then
		WeaponInfo = {WeaponItem:GetBigID(),WeaponItem:GetIndex(),WeaponItem:GetItemID()}
	end
	if AssociateWeaponItem ~= nil then
		AssociateWeaponInfo = {AssociateWeaponItem:GetBigID(),AssociateWeaponItem:GetIndex(),AssociateWeaponItem:GetItemID()}
	end
	--装备部位为nil，则把部位ID设置为50，留给服务器端做判断。
	if eEquipPart == nil then
		eEquipPart = 50
	end
	
	local PlayerInfo = {["Id"] = g_MainPlayer.m_uID
						,["Class"] = g_MainPlayer:CppGetClass()
						,["Level"] = g_MainPlayer:CppGetLevel()
						,["Sex"] = g_MainPlayer.m_Properties:GetSex()
						,["Camp"] = g_MainPlayer:CppGetBirthCamp()
						,["IsInBattleState"] = g_MainPlayer:IsInBattleState()
						,["IsInForbitUseWeaponState"] = g_MainPlayer:IsInForbitUseWeaponState()}
	local weaponUseLevel = DynInfo and DynInfo.BaseLevel or g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"BaseLevel")
	local Ruslut , HandType = g_ItemInfoMgr:WeaponCanUse(PlayerInfo,nBigID,nIndex,weaponUseLevel,WeaponInfo,AssociateWeaponInfo,eEquipPart)
	if Ruslut then
		local context = {nRoomIndex, nPos,nBigID, nIndex,eEquipPart}
		if ParentWnd ~= nil and tonumber(DynInfo.BindingType) == BindingStyle1 then
			ParentWnd.m_MsgBox = MessageBox(ParentWnd, MsgBoxMsg(13002), BitOr(MB_BtnOK,MB_BtnCancel),UseItem,context)
		else
			Gac2Gas:UseItem(g_Conn,nRoomIndex,nPos,nBigID, nIndex,eEquipPart)
		end
	else
		MsgClient(HandType)
	end
end

--使用防具函数 防具不需要传递装备部位，
local function UseArmor(ParentWnd, nBigID, nIndex, nRoomIndex, nPos, Item,eEquipPart)
	local DynInfo
	if g_ItemInfoMgr:IsStaticArmor(nBigID) then
		DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Item:GetItemID())
	else
		MsgClient(1001)
		return
	end

	local PlayerInfo = {["Id"] = g_MainPlayer.m_uID
						,["Class"] = g_MainPlayer:CppGetClass()
						,["Level"] = g_MainPlayer:CppGetLevel()
						,["Sex"] = g_MainPlayer.m_Properties:GetSex()
						,["Camp"] = g_MainPlayer:CppGetBirthCamp()
						,["IsInBattleState"] = g_MainPlayer:IsInBattleState()
						,["IsInForbitUseWeaponState"] = g_MainPlayer:IsInForbitUseWeaponState()}
    local armorUseLevel = DynInfo and DynInfo.BaseLevel or g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"BaseLevel")
	local result ,errnum = g_ItemInfoMgr:ArmorCanUse(PlayerInfo,nBigID,nIndex,armorUseLevel)
	if result then
		local context = {nRoomIndex, nPos,nBigID, nIndex,eEquipPart}
		if ParentWnd ~= nil and tonumber(DynInfo.BindingType) == BindingStyle1 then
			ParentWnd.m_MsgBox = MessageBox(ParentWnd, MsgBoxMsg(13002), BitOr(MB_BtnOK,MB_BtnCancel),UseItem,context)
		else
			Gac2Gas:UseItem(g_Conn,nRoomIndex,nPos,nBigID, nIndex,eEquipPart)
		end
	else
		MsgClient(errnum)
	end
end

--使用饰品函数
local function UseJewelry(ParentWnd, nBigID, nIndex, nRoomIndex, nPos ,Item,eEquipPart)
	local DynInfo
	if g_ItemInfoMgr:IsJewelry(nBigID) or g_ItemInfoMgr:IsRing(nBigID) then
		DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Item:GetItemID())
	else
		MsgClient(1001)
		return
	end
	local PlayerInfo = {["Id"] = g_MainPlayer.m_uID
						,["Class"] = g_MainPlayer:CppGetClass()
						,["Level"] = g_MainPlayer:CppGetLevel()
						,["Sex"] = g_MainPlayer.m_Properties:GetSex()
						,["Camp"] = g_MainPlayer:CppGetBirthCamp()
						,["IsInBattleState"] = g_MainPlayer:IsInBattleState()
						,["IsInForbitUseWeaponState"] = g_MainPlayer:IsInForbitUseWeaponState()}

	if g_ItemInfoMgr:IsJewelry(nBigID) or g_ItemInfoMgr:IsRing(nBigID) then
	    local ewelryCanUseLevel = DynInfo and DynInfo.BaseLevel or g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"BaseLevel")
		local result,errnum = g_ItemInfoMgr:JewelryCanUse(PlayerInfo,nBigID,nIndex,ewelryCanUseLevel)
		if result then
			local context = {nRoomIndex, nPos,nBigID, nIndex,eEquipPart}
			if ParentWnd ~= nil and tonumber(DynInfo.BindingType) == BindingStyle1 then
				ParentWnd.m_MsgBox = MessageBox(ParentWnd, MsgBoxMsg(13002), BitOr(MB_BtnOK,MB_BtnCancel),UseItem,context)
			else
				Gac2Gas:UseItem(g_Conn,nRoomIndex,nPos,nBigID, nIndex,eEquipPart)
			end
		else
			MsgClient(errnum)
		end
	end
end

--使用药品
local function UseSkillItems(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart)
	--print(nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart)
	if g_ItemInfoMgr:IsItemBurstSkill(nBigID) then
		local BaseLevel = g_ItemInfoMgr:GetItemInfo(nBigID,nSmallID,"BaseLevel")
		local SkillName = g_ItemInfoMgr:GetItemInfo(nBigID,nSmallID,"SkillName")
		if g_MainPlayer:CppGetLevel() >= BaseLevel then
			if SkillName ==nil or SkillName == "" then
				--3类物品中没这个物品名 不能使用此物品技能
				return
			end
			g_MainPlayer:StopMovingBeforeDoCastingSkill(SkillName)
			Gac2Gas:UseItem(g_Conn,nRoomIndex,nPos,nBigID, nSmallID,eEquipPart)
		else
			-- 提示等级不够
			MsgClient(1003)
		end
	end
end

local function UseQuestSkillItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart)
	local SkillName = g_ItemInfoMgr:GetItemInfo(nBigID,nSmallID,"SkillName")
	if SkillName ~= "" then
		--g_MainPlayer:FightSkill(PropTable.SkillName, 1)
		--Gac2Gas:ClickItem(g_Conn, nSmallID) --已经在g_GacUseItem 使用了
		g_WndMouse:ClearCursorAll()
	end
	local ItemInfo = g_ItemUseInfoMgr:GetItemUseInfo(nSmallID)
	if ItemInfo ~= nil then
		local castType = ItemInfo.CastType[1]
		local effect = ItemInfo.effect
--		if castType ~= "不读条" then
--			g_SetItemGridWndState(nRoomIndex,nPos,false)
--		end
		if effect ~= nil and
			(effect == "对自身释放技能" or effect == "对目标释放技能" or effect == "对地点释放技能") then
			g_MainPlayer:StopMovingBeforeDoCastingSkill(ItemInfo.Arg[1][1])
--			g_MainPlayer:TurnAroundByNonFightSkill(arg[1])
		end
	end
	GroundSelector:CancelSelectGround()
	if g_ItemInfoMgr:IsPickOreItem(nBigID) then
		local item = g_DynItemInfoMgr:GetDynItemInfo(Item:GetItemID())
		if item and IsNumber(item.CurDura) and item.CurDura == 0 then
			MsgClient(9626)
			return
		end
	end
	Gac2Gas:ClickItem(g_Conn, nBigID, nSmallID, nRoomIndex, nPos)
	--OnRClickQuestItem(nBigID, nSmallID)
end

--右键点击装备鉴定卷轴
local function UseEquipIdentifyScroll(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local context = {ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item}
	g_WndMouse:SetPreIdentifyEquip(context)
end

--右键点击护甲片
local function ArmorPieceEnactment(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local context = {ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item}
	g_WndMouse:SetPreArmorPieceEnactment(context)
end

--右键使用装备炼化魂卷轴
local function UseEquipSmeltSoulScroll(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local context = {ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item}
	g_WndMouse:SetPreEquipSmeltSoul(context)
end

--右键点击魂珠将其含有的魂值平均分配到装备上
local function AverageSoulCountToEquip(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local function CallBack(g_GameMain, index)
		if index == MB_BtnOK then
			Gac2Gas:AvargeSoulCountToEquip(g_Conn, nRoomIndex, nPos)
		end
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = nil
		return true
	end
	if g_ItemInfoMgr:IsSoulPearl(nBigID) then
		local SoulType = g_ItemInfoMgr:GetItemInfo(nBigID, nSmallID, "SoulType")
		local sMsg = ""
		local DisplayName = g_ItemInfoMgr:GetItemLanInfo(nBigID, nSmallID, "DisplayName")
		if(1 == SoulType) then --魂珠
			sMsg = MsgBoxMsg(14001)
		elseif(2 == SoulType) then --经验球
			sMsg = MsgBoxMsg(14006,DisplayName)
		elseif(3 == SoulType) then --积分
			local AreaFbPointType = g_ItemInfoMgr:GetItemInfo(nBigID, nSmallID, "AreaFbPointType")
			sMsg = MsgBoxMsg( 14007, DisplayName, g_DisplayCommon.m_tblAreaFbPoinDisplay[AreaFbPointType] )
		end

		g_GameMain.m_MsgBox = MessageBox(g_GameMain,sMsg ,  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
	end
end

--激活经验、魂瓶
local function OpenExpOrSoulBottle(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local base_level = Exp_Soul_Bottle_Common(Item.m_Grid.m_sName,"BaseLevel")
	if g_MainPlayer:CppGetLevel() < base_level then
		MsgClient(801)
		return 
	end
	Gac2Gas:OpenExpOrSoulBottle(g_Conn,nRoomIndex, nPos)
end

--右键孵化幻兽蛋
local function UsePetEgg(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local function CallBack(g_GameMain, index)
		if index == MB_BtnOK then
			Gac2Gas:IncubatePetEgg(g_Conn,nSmallID,nRoomIndex, nPos)
		end
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = nil
		return true
	end
	if g_ItemInfoMgr:IsPetEgg(nBigID) then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(14002),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
	end
end

--右键使用幻兽灵石
local function UsePetSkillStone(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item)
	local function CallBack(g_GameMain, index)
		if index == MB_BtnOK then
			if g_GameMain.m_PetInfoWnd.m_uChoosedIndex == -1 then
				MsgClient(194114)
			else
				Gac2Gas:UsePetSkillStone(g_Conn,nSmallID,nRoomIndex, nPos)
			end
		end
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = nil
		return true
	end
	if g_ItemInfoMgr:IsPetSkillStone(nBigID) then
		if not g_GameMain.m_PetInfoWnd:IsShow() then
			if not g_GameMain.m_PetInfoWnd.m_IsFirstOpenPetWnd then
				Gac2Gas:InitPetWndInfo(g_Conn)
			end
			g_GameMain.m_PetInfoWnd:ShowWnd(true)
		end
		g_GameMain.m_PetInfoWnd:InitWnd()
		g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(14005),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
	end
end

local function UseStone( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos )
	local StoneWnd = CStone.GetWnd()
	local StoneCompoundWnd = CStoneCompound.GetWnd()
	if StoneWnd.StonePartUsing:IsShow() then
		StoneWnd.StonePartUsing:UseStone(nBigID, nSmallID, nRoomIndex, nPos )
	elseif StoneCompoundWnd:IsShow() then
		StoneCompoundWnd:UseMaterial(nRoomIndex, nPos )
	end
end

local function UseHoleMaterial( ParentWnd, nBigID, nSmallID, nRoomIndex, nPos )
	local StoneWnd = CStone.GetWnd()
	local StoneCompoundWnd = CStoneCompound.GetWnd()
	if StoneWnd.StonePartUsing:IsShow() then
		StoneWnd.StonePartUsing:UseHoleMaterial(nBigID, nSmallID, nRoomIndex, nPos )
	elseif StoneCompoundWnd:IsShow() then
		StoneCompoundWnd:UseMaterial(nRoomIndex, nPos )
	end
end

local function UseWhiteStone(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart)
	if g_ItemInfoMgr:IsWhiteStone(nBigID) then
		local Whitestone = CWhiteStone.GetWnd()
		Whitestone:UseWhiteStone(nBigID, nSmallID, nRoomIndex, nPos, eEquipPart )
	end
end

local function UseTongTruckItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos)
	GroundSelector:CancelSelectGround()
	Gac2Gas:ClickItem(g_Conn, nBigID, nSmallID, nRoomIndex, nPos)
end

--local function UseTongItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos)
--	GroundSelector:CancelSelectGround()
--	Gac2Gas:ClickItem(g_Conn, nBigID, nSmallID, nRoomIndex, nPos)
--end

local function UseVIPItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos)
	GroundSelector:CancelSelectGround()
	Gac2Gas:UseVIPItem(g_Conn,nSmallID, nRoomIndex, nPos)
end

local function UseEquipIntensifyBackItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos)
    local useType = g_ItemInfoMgr:GetItemInfo(nBigID, Item.m_Grid.m_sName, "UseType")
    if useType == 2 then
        return 
    end
    local context = {ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item}
    g_WndMouse:SetUseEquipIntenBackItem(context)
end

local GacUseItem =
{
	[1] = UseQuestSkillItem,			 --调用任务物品脚本
	[2] = nil,
	[3] = UseSkillItems,
	[5] = UseWeapon,
	[6] = UseArmor,
	[7] = UseWeapon,
	[8] = UseJewelry,
	[9] = UseJewelry,
	[10] = UseQuestSkillItem,
	--[11] = nil,
	--[12] = nil,
	--[13] = nil,
	[14] = UseStone,
	[15] = UseHoleMaterial,
	[16] = UseQuestSkillItem,			--任务物品
	[17] = OpenMailText,				--右键打开邮件文本附件
	[18] = UseWhiteStone,				--右键使用白宝石
	[19] = AverageSoulCountToEquip,		--右键点击魂珠，将魂珠上的魂值点数灌注到角色身上
	[24] = OpenBoxitemDropWnd,			--右键打开盒子物品掉落物品面板
	[25] = OpenTextItem,				--邮件打开文本物品
	[26] = DelBattleArrayBook,			--用掉阵法书
	[30] = UseQuestSkillItem,			--矿藏图 调用任务物品的特殊物品脚本
	[31] = UseEquipIdentifyScroll,		--使用装备鉴定卷轴
	[32] = UseQuestSkillItem,			--使用种子种花
	[33] = UseTongBuildingItem,			--使用帮会建筑卷轴创建建筑
	[34] = ArmorPieceEnactment,			--右键使用护甲片设定
	[35] = UseTongTruckItem,			--右键使用帮会运输车卷轴
	[36] = UseEquipSmeltSoulScroll,		--右键使用装备炼化魂卷轴
	[37] = UseQuestSkillItem,			--右键使用修行塔卡牌
	[39] = UseQuestSkillItem,			--右键使用创建帮会挑战建筑
	[40] = UseQuestSkillItem,			--右键使用创建NPC
	[41] = UsePetEgg,					--右键孵化幻兽
	[42] = UsePetSkillStone,			--右键使用幻兽灵石
	[43] = UseEquipIntensifyBackItem,	--右键使用大重铸石
	[45] = UseQuestSkillItem,			--右键使用采矿工具
	[46] = UseQuestSkillItem,			--右键使用商城道具
	[47] = OpenBoxitemDropWnd,			--右键打开盒子物品掉落物品面板
	[48] = OpenExpOrSoulBottle,			--右键激活经验瓶或者魂瓶
	[51] = UseVIPItem,					--右键使用vip道具
	--[52] = UseTongItem,					--右键使用小队道具
}


function g_GacUseItem(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos,eEquipPart)
	--角色信息不存在的情况，使用物品直接返回
	if g_MainPlayer == nil then
		return
	end
	--ParentWnd 可以为nil，表示不需要确认直接使用，在交换装备的时候需要
	
	if not g_MainPlayer:CppIsAlive() then
		MsgClient(828)
		return
	end
	if (g_MainPlayer:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill)) then
		local tbl = CanUseItem_Common(nSmallID)
		if not tbl or tbl("BigID") ~= nBigID then
			MsgClient(829)
			return
		end
	elseif (g_MainPlayer:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseMissionItemSkill)) then
		if nBigID == 16 or nBigID == 1 then 
			MsgClient(829)
			return
		end
	end
	
	local func = GacUseItem[nBigID]
	if func ~= nil then
		local srcPos = nPos --在物品栏中的位置，未转换过的（是在附背包还是在主背包中）
		local Mgr = g_GameMain.m_BagSpaceMgr
		local Space = Mgr:GetSpace(nRoomIndex)
		assert(Space~=nil)
		local Grid = Space:GetGrid(nPos)
		
		if( not Grid:GetEnable() ) then --包裹中物品处于锁定状态
			MsgClient(835)
			return
		end
		
		local Item = Grid:GetItem()
		assert(Item ~= nil)
		local ItemInfo = g_DynItemInfoMgr:GetDynItemInfo(Item:GetItemID())
		if ItemInfo then
			if 0 == ItemInfo.m_nLifeTime then
				MsgClient(13)
				return
			end
		end
		if g_GameMain.m_BoxitemPickupWnd ~= nil and
			 g_GameMain.m_BoxitemPickupWnd:IsShow() then
			g_GameMain.m_BoxitemPickupWnd:ShowWnd(false)
			g_GameMain.m_BoxitemPickupWnd:EnableBoxItemRelatedGrid(true)
		end
		if g_MainPlayer.m_ItemBagLock and nBigID ~= 16 then
			MsgClient(160009)
			return
		end
		func(ParentWnd, nBigID, nSmallID, nRoomIndex, nPos, Item, eEquipPart, srcPos)
	else
		g_WndMouse:ClearCursorAll()
	end
	
end

function g_GacUseItemFromShortcut(ParentWnd,nBigID, nIndex)
	local Mgr = g_GameMain.m_BagSpaceMgr
	local nRoomIndex, nPos = Mgr:FindItemBySpace(g_StoreRoomIndex.PlayerBag, nBigID,nIndex)
	if nRoomIndex then
		local Space = Mgr:GetSpace(nRoomIndex)
		assert(Space~=nil)
		local Grid = Space:GetGrid(nPos)
		local Item = Grid:GetItem()
		assert(Item ~= nil)
		if g_GameMain.m_WndMainBag and (not g_GameMain.m_WndMainBag:GetMainItemBagState()) then
			MsgClient(160001)
			return
		end
		g_GacUseItem(ParentWnd, nBigID, nIndex, nRoomIndex, nPos, Item)
	end
end

