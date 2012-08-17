-- 注册使用物品的处理函数
gas_require "item/useitem/GasUseSkillItem"
gas_require "item/useitem/GasUseEquip"

--白宝石鉴定
function g_UseWhiteStone(Connection, Player, nRoomIndex, nPos, nBigID, nIndex, nItemID,nEquipPart,Data)
	CStoneMgr.GetStoneMgr():UseWhiteStone(Connection, Player, nRoomIndex, nPos, nBigID, nIndex, nItemID,nEquipPart,Data)
end

local UseItemExecuteFunc =
{
	[1] = nil,
	[2] = nil,
	[3] = g_UseSkillItem,				-- 使用技能物品
	[5] = g_UseEquip,					-- 装备，静态
	[6] = g_UseEquip,
	[7] = g_UseEquip,
	[8] = g_UseJewelry,					-- 戒指
	[9] = g_UseJewelry,					-- 饰品
	[10] = g_UseEquip,
	[11] = g_UseEquip,
	[12]= g_UseJewelry,         
	[18] = g_UseWhiteStone,			--白宝石鉴定
}


function Gac2Gas:UseItem(Connection,nRoomIndex,nPos,nBigID,Index,eEquipPart)
	if Connection.m_Player == nil then
		return
	end
	if not Connection.m_Player:CppIsLive() then
		return
	end	
	if Connection.m_Player.m_ItemBagLock then
		MsgToConn(Connection,160009)
		return
	end
	if eEquipPart == 50  then --装备的部位为50的部位，需要替换成0。
		eEquipPart = 0
	end
	
	local PlayerInfo = {["Id"] = Connection.m_Player.m_uID
						,["Class"] = Connection.m_Player:CppGetClass()
						,["Level"] = Connection.m_Player:CppGetLevel() 
						,["Sex"] = Connection.m_Player.m_Properties:GetSex()
						,["Camp"] = Connection.m_Player:CppGetBirthCamp()
						,["IsInBattleState"] = Connection.m_Player:IsInBattleState()
						,["IsInForbitUseWeaponState"] = Connection.m_Player:IsInForbitUseWeaponState()
						}
	local data = { ["RoomIndex"] = nRoomIndex, ["Pos"] = nPos, ["nBigID"] = nBigID, ["Index"] = Index ,["EquipPart"] = eEquipPart, ["PlayerInfo"] = PlayerInfo
					,["sceneName"] = Connection.m_Player.m_Scene.m_SceneName}
	local charID = Connection.m_Player.m_uID
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if not IsCppBound(Player) then
			return
		end
		if bool then 
			local EquipInfo = Data["NewEquipInfo"]
			local nItemBigID,nItemIndex,nItemID = unpack(EquipInfo)
			local nEquipPart = Data["EquipPart"]
			if nItemBigID~= -1 and nItemBigID ~= 0 then
				if UseItemExecuteFunc[nItemBigID] ~= nil then
					UseItemExecuteFunc[nItemBigID](Connection,Player,nRoomIndex,nPos,nItemBigID,nItemIndex,nItemID,nEquipPart,Data, charID)
				end
			end 
		else
			if IsNumber(Data) then
				MsgToConn(Connection,Data)
			end
			return
		end
		--高速加佣兵等级指引计数，更新追踪窗口
		--AddMercenaryLevelCount(Connection, "护甲值", Player:GetDefence())
		--UpdateMercenaryLevelTraceWnd(Connection, Data["MlRes"])
	end	
	--local entry = (g_DBTransDef["GasUseItemDB"])
	local result = CallAccountManualTrans(Connection.m_Account, "GasUseItemDB", "EntryFunc", CallBack, data)
end