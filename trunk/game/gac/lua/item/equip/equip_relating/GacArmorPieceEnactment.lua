cfg_load "item/ArmorPieceEnactment_Common"

CArmorPieceEnactment = class(SQRDialog)

function CreateEquipEnactmentAttrWnd(wnd)
	wnd.m_tblInfo = {}
	wnd:CreateFromRes("EquipEnactmentAttr", g_GameMain)
	return wnd
end

function CArmorPieceEnactment:Ctor()
	 CreateEquipEnactmentAttrWnd(self)
end

function CArmorPieceEnactment:GetCheckValue()
	for i = 1, 4 do
		if(self.m_tblCheckBtn[i]:GetCheck()) then
			self.m_tblCheckBtn[i]:SetCheck(false)
			return i
		end
	end
end

function CArmorPieceEnactment:OnChildCreated()
	self.m_OkBtn				= self:GetDlgChild("OkBtn")
	self.m_CloseBtn			= self:GetDlgChild("CancelBtn")
	self.m_MsgWnd				= self:GetDlgChild("MsgWnd")
	self.m_CheckBtnNum = 4
	self.m_tblCheckBtn = {}
	self.m_tblAttrName = {}
	for i = 1, self.m_CheckBtnNum do
		self.m_tblCheckBtn[i] = self:GetDlgChild("Attr" .. i)
		self.m_tblAttrName[i] = self:GetDlgChild("AttrName" .. i)
	end
end

--消息
function CArmorPieceEnactment:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_OkBtn) then
			self:OnArmorPieceEnactmentToGas()
		elseif (Child == self.m_CloseBtn) then 
			self:ShowWnd(false)
			return
		end
	end
end

function CArmorPieceEnactment:OnArmorPieceEnactmentToGas()
	local update_index = self:GetCheckValue()
	self:ArmorPieceEnactmentToGas(update_index)
	self:ShowWnd(false)
end

function CArmorPieceEnactment:SetWndShowOrFalse(index,bFlag)
	self.m_tblCheckBtn[index]:ShowWnd(bFlag)
	self.m_tblAttrName[index]:ShowWnd(bFlag)
end

function CArmorPieceEnactment:BothHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2,AddTimes3,AddTimes4)
	local AddTimes = ArmorPieceEnactment_Common(ArmorPieceName,"AddTimes") 
	if DynInfo.ArmorPieceAttr3 ~= "" and DynInfo.ArmorPieceAttr4 ~= "" then
		if (ArmorPieceName ~= DynInfo.ArmorPieceAttr1 or (ArmorPieceName == DynInfo.ArmorPieceAttr1 and AddTimes == DynInfo.ArmorPiece1AddTimes)) and (ArmorPieceName ~= DynInfo.ArmorPieceAttr2 or (ArmorPieceName == DynInfo.ArmorPieceAttr2 and AddTimes == DynInfo.ArmorPiece2AddTimes))
			and (ArmorPieceName ~= DynInfo.ArmorPieceAttr3 or (ArmorPieceName == DynInfo.ArmorPieceAttr3 and AddTimes == DynInfo.ArmorPiece3AddTimes)) and (ArmorPieceName ~= DynInfo.ArmorPieceAttr4 or (ArmorPieceName == DynInfo.ArmorPieceAttr4 and AddTimes == DynInfo.ArmorPiece4AddTimes)) then
			self:ShowWnd(true)
			if AddTimes1 ~= 1 then
				self.m_tblAttrName[1]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"PropertyValue")*DynInfo.ArmorPiece1AddTimes .. "  (" .. DynInfo.ArmorPiece1AddTimes .. "/" .. AddTimes1 .. ")")
			else
				self.m_tblAttrName[1]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"PropertyValue"))
			end
			if AddTimes2 ~= 1 then
				self.m_tblAttrName[2]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"PropertyValue")*DynInfo.ArmorPiece2AddTimes .. "  (" .. DynInfo.ArmorPiece2AddTimes .. "/" .. AddTimes2 .. ")")
			else
				self.m_tblAttrName[2]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"PropertyValue"))
			end
			
			self:SetWndShowOrFalse(3,true)
			if AddTimes3 ~= 1 then
				self.m_tblAttrName[3]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr3,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr3,"PropertyValue")*DynInfo.ArmorPiece3AddTimes .. "  (" .. DynInfo.ArmorPiece3AddTimes .. "/" .. AddTimes3 .. ")")
			else
				self.m_tblAttrName[3]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr3,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr3,"PropertyValue"))
			end
			self:SetWndShowOrFalse(4,true)
			if AddTimes4 ~= 1 then
				self.m_tblAttrName[4]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr4,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr4,"PropertyValue")*DynInfo.ArmorPiece4AddTimes .. "  (" .. DynInfo.ArmorPiece4AddTimes .. "/" .. AddTimes4 .. ")")
			else
				self.m_tblAttrName[4]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr4,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr4,"PropertyValue"))
			end
		else
			self:ArmorPieceEnactmentToGas(0)
		end
	else
		self:ArmorPieceEnactmentToGas(0)
	end
end


function CArmorPieceEnactment:SingleHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2)
	local AddTimes = ArmorPieceEnactment_Common(ArmorPieceName,"AddTimes") 
	if (ArmorPieceName ~= DynInfo.ArmorPieceAttr1 or (ArmorPieceName == DynInfo.ArmorPieceAttr1 and AddTimes == DynInfo.ArmorPiece1AddTimes)) and (ArmorPieceName ~= DynInfo.ArmorPieceAttr2 or (ArmorPieceName == DynInfo.ArmorPieceAttr2 and AddTimes == DynInfo.ArmorPiece2AddTimes)) then
		self:ShowWnd(true)
		if AddTimes1 ~= 1 then
			self.m_tblAttrName[1]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"PropertyValue")*DynInfo.ArmorPiece1AddTimes .. "  (" .. DynInfo.ArmorPiece1AddTimes .. "/" .. AddTimes1 .. ")")
		else
			self.m_tblAttrName[1]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"EquipType")),"DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"PropertyValue"))
		end
		if AddTimes2~= 1 then
			self.m_tblAttrName[2]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"EquipType")), "DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"PropertyValue")*DynInfo.ArmorPiece2AddTimes .. "  (" .. DynInfo.ArmorPiece2AddTimes .. "/" .. AddTimes2 .. ")")
		else
			self.m_tblAttrName[2]:SetWndText(Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"EquipType")), "DisplayName") .. "  +" .. ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"PropertyValue"))
		end
		self:SetWndShowOrFalse(3,false)
		self:SetWndShowOrFalse(4,false)
	else
		self:ArmorPieceEnactmentToGas(0)
	end
end

local single_equip_cost_tbl = {
								[6] = 5000,
								[7] = 10000,
								[8] = 15000,
								[9] = 20000
}

local both_equip_cost_tbl = {
								[12] = 5000,
								[13] = 5000,
								[14] = 10000,
								[15] = 10000,
								[16] = 15000,
								[17] = 15000,
								[18] = 20000,
								[19] = 20000,
}

--@brief 护甲片设定
function CArmorPieceEnactment:ArmorPieceEnactment()
	local cost = 0
	local messageId = 0
	local state, tbl = g_WndMouse:GetCursorState()
	if(state ~= ECursorState.eCS_ArmorPieceEnactment) then return end
	self.m_tblInfo  = tbl
	local bigID,index,itemid,context,equipRoomIndex,equipPos = unpack(self.m_tblInfo)
	local ArmorPieceName = context[3]
	
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemid)
	if DynInfo == nil then
		return
	end
	local AddTimes = ArmorPieceEnactment_Common(ArmorPieceName,"AddTimes") 
	self.m_MsgWnd:SetWndText(GetStaticTextClient(171001,Lan_PropertyName_Common(MemH64(ArmorPieceEnactment_Common(ArmorPieceName,"EquipType")),"DisplayName"),ArmorPieceEnactment_Common(ArmorPieceName,"PropertyValue")))
	local	equipType = g_ItemInfoMgr:GetItemInfo(bigID,index,"EquipType")
	local AddTimes1 = ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr1,"AddTimes") 
	local AddTimes2 = ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr2,"AddTimes")
	if equipType and equipType ~= "" then
		equipType = string.sub(equipType,1,2)
		if equipType and equipType ~= "" then
			if equipType == "双" or equipType == "长" then
				local AddTimes3 = ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr3,"AddTimes") 
				local AddTimes4 = ArmorPieceEnactment_Common(DynInfo.ArmorPieceAttr4,"AddTimes")
				
				local totalAddTimes = DynInfo.ArmorPiece1AddTimes + DynInfo.ArmorPiece2AddTimes + DynInfo.ArmorPiece3AddTimes + DynInfo.ArmorPiece4AddTimes
				if (ArmorPieceName == DynInfo.ArmorPieceAttr1 and AddTimes >= DynInfo.ArmorPiece1AddTimes) 
					or (ArmorPieceName == DynInfo.ArmorPieceAttr2 and AddTimes >= DynInfo.ArmorPiece2AddTimes)
					or (ArmorPieceName == DynInfo.ArmorPieceAttr3 and AddTimes >= DynInfo.ArmorPiece3AddTimes)
					or (ArmorPieceName == DynInfo.ArmorPieceAttr4 and AddTimes >= DynInfo.ArmorPiece4AddTimes)  then
					cost = both_equip_cost_tbl[totalAddTimes] or 0
				end
				
				local function CallBack(g_GameMain,uButton)
					if uButton == MB_BtnOK then
						if g_MainPlayer.m_nMoney < cost then
							MsgClient(171013)
						else
							self:BothHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2,AddTimes3,AddTimes4)
						end
					end
					g_GameMain.m_ArmorPieceEnactment = nil
					return true
				end

				if cost ~= 0 then
					local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(bigID,index,"DisplayName")
					local equipName = g_Tooltips:GetEquipShowName(DynInfo,DisplayName,"*")
					equipName = string.format("[%s]", equipName)
					g_GameMain.m_ArmorPieceEnactment = MessageBox(g_GameMain, MsgBoxMsg(17011,equipName,totalAddTimes + 1,g_MoneyMgr:ChangeMoneyToString(cost,EGoldType.GoldCoin)),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
				else
					self:BothHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2,AddTimes3,AddTimes4)
				end
			elseif DynInfo.ArmorPieceAttr1 ~= "" and DynInfo.ArmorPieceAttr2 ~= "" then
				local totalAddTimes = DynInfo.ArmorPiece1AddTimes + DynInfo.ArmorPiece2AddTimes
				
				if (ArmorPieceName == DynInfo.ArmorPieceAttr1 and AddTimes > DynInfo.ArmorPiece1AddTimes) or (ArmorPieceName == DynInfo.ArmorPieceAttr2 and AddTimes > DynInfo.ArmorPiece2AddTimes) then
					cost = single_equip_cost_tbl[totalAddTimes] or 0
				end
				
				local function CallBack(g_GameMain,uButton)
					if uButton == MB_BtnOK then
						if g_MainPlayer.m_nMoney < cost then
							MsgClient(171013)
						else
							self:SingleHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2)
						end
					end
					g_GameMain.m_ArmorPieceEnactment = nil
					return true
				end
				if cost ~= 0 then 
					local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(bigID,index,"DisplayName")
					local equipName = g_Tooltips:GetEquipShowName(DynInfo,DisplayName,"*")
					equipName = string.format("[%s]", equipName)
					g_GameMain.m_ArmorPieceEnactment = MessageBox(g_GameMain, MsgBoxMsg(17011,equipName,totalAddTimes + 1,g_MoneyMgr:ChangeMoneyToString(cost,EGoldType.GoldCoin)),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
				else
					self:SingleHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2)
				end
			else
				self:ArmorPieceEnactmentToGas(0)
			end
		end
	elseif DynInfo.ArmorPieceAttr1 ~= "" and DynInfo.ArmorPieceAttr2 ~= "" then
		local totalAddTimes = DynInfo.ArmorPiece1AddTimes + DynInfo.ArmorPiece2AddTimes
		if (ArmorPieceName == DynInfo.ArmorPieceAttr1 and AddTimes > DynInfo.ArmorPiece1AddTimes) or (ArmorPieceName == DynInfo.ArmorPieceAttr2 and AddTimes > DynInfo.ArmorPiece2AddTimes) then
			cost = single_equip_cost_tbl[totalAddTimes] or 0
		end
		
		local function CallBack(g_GameMain,uButton)
			if uButton == MB_BtnOK then
				if g_MainPlayer.m_nMoney < cost then
					MsgClient(171013)
				else
					self:SingleHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2)
				end
			end
			g_GameMain.m_ArmorPieceEnactment = nil
			return true
		end
		if cost ~= 0 then
			local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(bigID,index,"DisplayName")
			local equipName = g_Tooltips:GetEquipShowName(DynInfo,DisplayName,"*")
			equipName = string.format("[%s]", equipName)
			g_GameMain.m_ArmorPieceEnactment = MessageBox(g_GameMain, MsgBoxMsg(17011,equipName,totalAddTimes + 1,g_MoneyMgr:ChangeMoneyToString(cost,EGoldType.GoldCoin)),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
		else
			self:SingleHandsEquipEnactmentCommon(ArmorPieceName,DynInfo,AddTimes1,AddTimes2)
		end
	else
		self:ArmorPieceEnactmentToGas(0)
	end
end


function CArmorPieceEnactment:ArmorPieceEnactmentToGas(update_index) 
	local nBigID,nIndex,itemid,context,equipRoomIndex,equipPos = unpack(self.m_tblInfo)
	local itemType = context[2]
	local ArmorPieceName = context[3]
	local ArmorPieceRoomIndex = context[4]
	local ArmorPiecePos = context[5]

	if update_index == nil then
		return
	end
	--读取护甲片配置表
	local ArmorPiece_EquipLevel = g_ItemInfoMgr:GetItemInfo(itemType,ArmorPieceName,"BaseLevel")
	local ArmorPiece_TopLevel = g_ItemInfoMgr:GetItemInfo(itemType,ArmorPieceName,"TopLevel")
	--通过护甲片的类型和等级读取护甲片的属性值
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(itemid)
	local curLevel = DynInfo.BaseLevel
	if curLevel < ArmorPiece_EquipLevel  then
		MsgClient(171002)
		return
	elseif curLevel > ArmorPiece_TopLevel  then
		MsgClient(171011)
		return
	end
	--护甲片设定
	Gac2Gas:ArmorPieceEnactment(g_Conn,nBigID,nIndex,ArmorPieceRoomIndex,ArmorPiecePos,equipRoomIndex or 0,equipPos or 0,itemid or 0,update_index,itemType,ArmorPieceName)
end


function CArmorPieceEnactment.ReturnArmorPieceEnactment(Conn,result)
	if result then
		MsgClient(171000)
		if g_GameMain.m_RoleStatus and g_GameMain.m_RoleStatus:IsShow() then
			g_GameMain.m_RoleStatus:UpdataEquipTips()
		end
		if g_GameMain.m_WndMainBag.m_ctItemRoom and g_GameMain.m_WndMainBag.m_ctItemRoom:IsShow() then
			g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
		end
	end
end
