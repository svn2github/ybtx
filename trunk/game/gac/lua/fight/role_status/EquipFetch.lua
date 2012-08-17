gac_gas_require "item/Equip/EquipCommonFunc"
cfg_load "equip/intensify/IntensifyStoneMapAddAttr_Common"
cfg_load "equip/advance/JingLingPieceSkill_Common"
lan_load "equip/Lan_PropertyName_Common"

CEquipUpIntensify = class( SQRDialog )
CEquipUpIntensifyPart1 = class( SQRDialog )
CEquipUpIntensifyPart2 = class( SQRDialog )

local AttrBtnNumber = 12

local function SetEnableWnd(Wnd,bool)
	if Wnd ~= nil then
		Wnd.m_EquipUpBtn:EnableWnd( bool )
		Wnd.m_EquipIntensifyBtn:EnableWnd(bool)
		Wnd.m_IntensifyBack:EnableWnd(bool)
	end
end

--创建升级及强化面板
function CEquipUpIntensify:Ctor()
	self:CreateFromRes( "EquipUpIntensifyWnd", g_GameMain )
	self.m_EquipNamePanel 	= self:GetDlgChild("EquipNamePanel")
	self.m_EquipUpBtn 		= self:GetDlgChild("EquipUpBtn")
	self.m_EquipBtnClose 	= self:GetDlgChild("EquipBtnClose")
	self.m_EquipIcon 		= self:GetDlgChild("EquipIcon")
	self.m_TotalSoulNum 	= self:GetDlgChild("SoulNum")
	self.m_AdvanceStone1 	= self:GetDlgChild("AdvanceStone1")
	self.m_LabStatic 		= self:GetDlgChild("LabStatic")
	self.m_Pattern 			= self:GetDlgChild("Pattern")
	self.m_LabStatic1		= self:GetDlgChild("LabStatic1")
	self.m_EquipIcon:InserBefore(self.m_Pattern)
	
	self.m_AttrWndTbl = {}
	for i = 1, AttrBtnNumber do 
		local StaticAttrWnd = self:GetDlgChild("StaticAttrWnd" .. i)
		table.insert(self.m_AttrWndTbl,StaticAttrWnd)
	end
			
	self.m_PhaseCheckBtnTbl = {}
	for i = 1,7 do
		local CheckBtn = self:GetDlgChild("PhaseCheckBtn" .. i)
		table.insert(self.m_PhaseCheckBtnTbl,CheckBtn)
	end
	
	self.m_RichText1 = self:GetDlgChild("RichText1")
	self.m_RichText1:SetWndText(GetStaticTextClient(5012))
	self.m_RichText2 = self:GetDlgChild("RichText2")								
	self.m_AdvancePartWnd = CEquipUpIntensifyPart1:new(self)	
	self.m_IntensifyPartWnd = CEquipUpIntensifyPart2:new(self)
	self.IsShowIntensifyWnd = true
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CEquipUpIntensifyPart1:Ctor(Parent)	
	self:CreateFromRes( "EquipUpIntensifyWndPart1", Parent )	
	Parent.m_JingLingTypeBtn 	= self:GetDlgChild("JingLingTypeBtn")				
	Parent.m_JingLingRebornBtn 	= self:GetDlgChild("JingLingRebornBtn")
	Parent.m_SkillPieceBtn1 	= self:GetDlgChild("SkillPieceBtn1")
	Parent.m_SkillPieceBtn2 	= self:GetDlgChild("SkillPieceBtn2")
	Parent.m_SkillPieceBtn3 	= self:GetDlgChild("SkillPieceBtn3")
	Parent.m_SkillPieceBtn4 	= self:GetDlgChild("SkillPieceBtn4")
	Parent.m_JingLingLabStatic  = self:GetDlgChild("LabStatic")
	
	Parent.m_SkillPieceBtnTbl = {
								Parent.m_SkillPieceBtn1,
								Parent.m_SkillPieceBtn2,
								Parent.m_SkillPieceBtn3,
								Parent.m_SkillPieceBtn4,
							}
end


function CEquipUpIntensifyPart2:Ctor(Parent)
	self:CreateFromRes( "EquipUpIntensifyWndPart2", Parent )
	Parent.m_EquipIntensifyBtn = self:GetDlgChild("EquipIntensifyBtn")	
	Parent.m_EquipIntensifyPro = self:GetDlgChild("EquipIntensifyPro")
	Parent.m_IntensifyBack = self:GetDlgChild("IntensifyBack")
	Parent.m_IntensifyPhasePanel = self:GetDlgChild("IntensifyPhasePanel")
	Parent.m_IntensifyStone1 = self:GetDlgChild("IntensifyStone1")
	Parent.m_StaticWnd5 = self:GetDlgChild("StaticWnd5")
	Parent.m_SuitCheckBtn = self:GetDlgChild("SuitCheckBtn")
	--Parent.m_SuitCheckBtn:ShowWnd(false)
end

--是否处于可强化状态
local function IsInIntensifyState(Wnd)
	if g_MainPlayer:IsInBattleState() and Wnd.BTn.PartType ~= 0 then
		return true,1031
	end
	if g_MainPlayer:IsInForbitUseWeaponState() and Wnd.BTn.PartType ~= 0 then
		return true,1032
	end	
	return false
end

function CEquipUpIntensify:EquipAdvanceGac2GasFun()
	local nBigID,nIndex = self.BTn.m_Info:GetBigID(),self.BTn.m_Info:GetIndex()
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.ItemId)
	if DynInfo.CurAdvancePhase >= DynInfo.MaxAdvancePhase then
		MsgClient(185002)
		return
	end
	local StoneInfoTbl = self.m_AdvanceStoneInfoTbl
	if StoneInfoTbl  then
		Gac2Gas:EquipAdvance(g_Conn,self.ItemId,self.BTn.PartType,StoneInfoTbl[1],StoneInfoTbl[2])
		SetEnableWnd(self,false)
	else
		SetEnableWnd(self,true)
		MessageBox(self,MsgBoxMsg(1014),MB_BtnOK)
	end
end

function CEquipUpIntensify:SendIntensifyRequest(DynInfo)
	if DynInfo.IntensifyPhase >= 3  then
	    if self.m_StoneInfoTbl then 
			Gac2Gas:EquipIntensifyUseStone(g_Conn, self.ItemId, self.BTn.PartType, self.m_StoneInfoTbl[1], self.m_StoneInfoTbl[2])
			SetEnableWnd(self,false)
	    else
	        MessageBox(self, MsgBoxMsg(1010), MB_BtnOK)
	        SetEnableWnd(self,true)
	    end
    else
	    Gac2Gas:IntensifyEquip(g_Conn,self.ItemId,self.BTn.PartType)
	    SetEnableWnd(self,false)
	end
end

--装备强化的回调
local function EquipIntensify(Tick, Wnd)
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Wnd.ItemId)
	if DynInfo.IntensifyQuality <= DynInfo.IntensifyPhase then		--已经强化到最大阶段了
		UnRegisterTick(Wnd.m_EquipIntensifyTick)
		Wnd.m_EquipIntensifyTick = nil
		Wnd.m_MsgBox = MessageBox(Wnd, MsgBoxMsg(1007), MB_BtnOK)
	end
	
	if( Wnd.m_EquipIntensifyBtn:IsHeld() ) then
		if( Wnd.AllFetch > 0 and  Wnd.IntensifyPourSoulNum <= Wnd.IntensifyNeedSoulNum ) then
			local number =	math.ceil(Wnd.IntensifyNeedSoulNum/10)
			if number > Wnd.AllFetch then
				number = Wnd.AllFetch
			end
			local Tempnumber = Wnd.IntensifyPourSoulNum + number
			if Tempnumber > Wnd.IntensifyNeedSoulNum then
				number = Wnd.IntensifyNeedSoulNum - Wnd.IntensifyPourSoulNum
				Wnd.IntensifyPourSoulNum = Wnd.IntensifyNeedSoulNum
			else
				Wnd.IntensifyPourSoulNum = Wnd.IntensifyPourSoulNum + number
			end

			Wnd.AllFetch = Wnd.AllFetch - number
			DynInfo.ItensifySoul = Wnd.IntensifyPourSoulNum
			g_MainPlayer.m_PlayerSoulNum = Wnd.AllFetch
			Wnd.m_EquipIntensifyPro:SetProgressMode(0)
			Wnd.m_EquipIntensifyPro:SetRange( Wnd.IntensifyNeedSoulNum )
			Wnd.m_EquipIntensifyPro:SetPos( Wnd.IntensifyPourSoulNum )
			Wnd.m_EquipIntensifyPro:SetWndText( Wnd.IntensifyPourSoulNum .. "/" .. Wnd.IntensifyNeedSoulNum )			
			if( Wnd.IntensifyPourSoulNum == Wnd.IntensifyNeedSoulNum ) then
				if Wnd.m_EquipIntensifyTick ~= nil then
					UnRegisterTick(Wnd.m_EquipIntensifyTick)
					Wnd.m_EquipIntensifyTick = nil
				end				
				local soul_num = Wnd.IntensifyPourSoulNum - Wnd.TempIntensify
				if soul_num ~= 0 and Wnd.IntensifyPourSoulNum == Wnd.IntensifyNeedSoulNum then
					Gac2Gas:PourIntensifySoul(g_Conn, Wnd.ItemId, soul_num)
					SetEnableWnd(Wnd,false)
				elseif soul_num == 0 and Wnd.IntensifyPourSoulNum == Wnd.IntensifyNeedSoulNum then
					local isInIntensifyState,MsgId = IsInIntensifyState(Wnd)
					if isInIntensifyState then
						MsgClient(MsgId)
						return
					end 
					Wnd:SendIntensifyRequest(DynInfo)
				end
			end		
		end
	else
		local soul_num = Wnd.IntensifyPourSoulNum - Wnd.TempIntensify
		if soul_num > 0 and Wnd.IntensifyPourSoulNum < Wnd.IntensifyNeedSoulNum  then
			Gac2Gas:PourIntensifySoul(g_Conn, Wnd.ItemId, soul_num)
		end
		if Wnd.m_EquipIntensifyTick ~= nil then
			UnRegisterTick(Wnd.m_EquipIntensifyTick)
			Wnd.m_EquipIntensifyTick = nil
		end
		if soul_num > 0 then
		 	SetEnableWnd(Wnd,false)
		end
	end
end

--设置数据显示
function CEquipUpIntensify:InitData(BTn)
	if( not BTn.m_Info ) then return end
	local nBigID,nIndex,nItemID = BTn.m_Info:GetBigID(),BTn.m_Info:GetIndex(),BTn.m_Info:GetItemID()
	local SmallIcon	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"SmallIcon")
	local DynInfo	= g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	
	self.m_AdvanceStoneInfoTbl 	= nil
	self.m_StoneInfoTbl 		= nil
	self.ItemId					= nItemID
	self.BTn					= BTn
	self:UpDateSoulValue(g_MainPlayer.m_PlayerSoulNum)
	self:UpDateInfomation(nBigID,nIndex,nItemID, DynInfo)
	g_LoadIconFromRes(SmallIcon, self.m_EquipIcon, -1,  IP_ENABLE, IP_CLICKDOWN)
	g_DelWndImage(self.m_AdvanceStone1,1,IP_ENABLE, IP_CLICKDOWN)
	g_DelWndImage(self.m_IntensifyStone1,1,IP_ENABLE, IP_CLICKDOWN)
	self:ShowWnd(true)
	self.m_AdvanceStone1:EnableWnd(true)
	self.m_LabStatic:ShowWnd(false)
	self:SetWndFlashInfo()
end

function CEquipUpIntensify:SetWndFlashInfo()
	if self:IsShow() and self.BTn then
		self.m_EquipIcon:DelFlashAttention()
		if g_GameMain then
			g_GameMain.m_WndMainBag.m_UpBtn:DelFlashAttention()
			g_GameMain.m_RoleStatus.m_UpBtn:DelFlashAttention()
		end
	else
		self.m_EquipIcon:AddFlashInfoByName("IntensifyFlash_Self")
		if g_GameMain then 
			if g_GameMain.m_WndMainBag.m_UpBtn then
				g_GameMain.m_WndMainBag.m_UpBtn:AddFlashInfoByName("IntensifyFlash_Self")
			end
			if g_GameMain.m_RoleStatus then
				g_GameMain.m_RoleStatus.m_UpBtn:AddFlashInfoByName("IntensifyFlash_Self")
			end
		end		
	end
end

--打开没有初始化的装备进阶强化面板
function CEquipUpIntensify:OpenNoInitWnd()
	if not self:IsShow() then	
		self.BTn = nil
		self.ItemId = nil		
		for i = 1,#(self.m_AttrWndTbl) do
			self.m_AttrWndTbl[i]:SetWndText("")
		end
		for i = 1,#(self.m_PhaseCheckBtnTbl) do
			self.m_PhaseCheckBtnTbl[i]:ShowWnd(true)
			self.m_PhaseCheckBtnTbl[i]:SetCheck(false)
		end
		self.m_EquipUpBtn:EnableWnd(false)	
		self.m_AdvanceStone1:EnableWnd(false)
		self.m_EquipNamePanel:SetWndText("")
		self.m_EquipIcon:SetMouseOverDescAfter("")
		self.m_RichText2:SetWndText("")
		self.m_AdvancePartWnd:ShowWnd(false)
		self.m_IntensifyPartWnd:ShowWnd(false)			
		self:ShowWnd(true)
		g_DelWndImage(self.m_AdvanceStone1,-1,IP_ENABLE, IP_CLICKDOWN)
		g_DelWndImage(self.m_EquipIcon,-1,IP_ENABLE, IP_CLICKDOWN)
		self:SetWndFlashInfo()
		self.m_LabStatic:ShowWnd(true)
		self.m_LabStatic1:ShowWnd(true)
		self.m_RichText1:ShowWnd(false)
	end
end

function CEquipUpIntensify:UpDate(Item, advanceSucFlag)
	if( not Item ) then return end
	local nBigID,nIndex,nItemID = Item:GetBigID(),Item:GetIndex(),Item:GetItemID()
	if nItemID ~= self.ItemId then
		return
	end
	local DynInfo	= g_DynItemInfoMgr:GetDynItemInfo(nItemID)
	if( not (self:IsShow() and nItemID == self.ItemId) ) then return end
	self:UpDateInfomation(nBigID,nIndex,nItemID, DynInfo, advanceSucFlag)
	if	self.BTn.PartType  == 0 then
		g_SetWndMultiToolTips(self.BTn.iconbtn,nBigID,nIndex,nItemID)
	else
		g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
		g_GameMain.m_RoleStatus:UpdataEquipTips()
	end
end

--更新面板信息
function CEquipUpIntensify:UpDateInfomation(nBigID,nIndex,nItemID, DynInfo, advanceSucFlag)
	local EquipType	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
	local AdvanceNeedSoulNum			= GetIntensifyNeededSoul(DynInfo.BaseLevel,EquipType)
	local IntensifyNeedSoulNum	= GetIntensifyNeededSoul(DynInfo.BaseLevel,EquipType)
	local outputAttr,outputAttrCount,Str,blockRate,blockValue = self:GetSomeInfo(DynInfo, nBigID, nIndex)
	
	self.AllFetch			= g_MainPlayer.m_PlayerSoulNum
	self:UpDateSoulValue(g_MainPlayer.m_PlayerSoulNum)
	self.IntensifyNeedSoulNum		= IntensifyNeedSoulNum
	self.AdvanceNeedSoulNum			= AdvanceNeedSoulNum
	self.IntensifyPourSoulNum		= DynInfo.ItensifySoul
	self.TempIntensify      		= DynInfo.ItensifySoul
	self.AdvancePourSoulNum			= DynInfo.AdvanceSoul
	self.m_IntensifyPhasePanel:SetWndText(DynInfo.IntensifyPhase)
	self.m_IntensifyPhase = DynInfo.IntensifyPhase
	self:AttrWndShow(Str,DynInfo, nBigID, nIndex, outputAttr, outputAttrCount, blockValue)
	g_SetItemRichToolTips(self.m_EquipIcon,nBigID,nIndex,nItemID)
	self:EquipNamePanelShow(DynInfo)
	self:ShowAdvancePorbInfo(DynInfo, nBigID,nIndex)
	self:SetIntensifyText()
	self:SetSkillPieceBtnInfo(DynInfo)
	self:CloseMenuList(advanceSucFlag)
	self:EquipUpShow()
end

--初始化强化信息
function CEquipUpIntensify:EquipUpShow()
	self.m_EquipIntensifyPro:SetProgressMode(0)
	self.m_EquipIntensifyPro:SetRange( self.IntensifyNeedSoulNum )
	self.m_EquipIntensifyPro:SetPos( self.IntensifyPourSoulNum )
	
	if self.AdvanceNeedSoulNum >= self.AdvancePourSoulNum then
		if self.m_AdvanceStoneInfoTbl then
			local fromRoom, srcPos = self.m_AdvanceStoneInfoTbl[1],self.m_AdvanceStoneInfoTbl[2]
	    	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	    	local count = grid:Size()
	    	if count > 0 then
	    		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.ItemId)
				if DynInfo.CurAdvancePhase < DynInfo.MaxAdvancePhase then
					self.m_EquipUpBtn:EnableWnd(true)
				else
					self.m_EquipUpBtn:EnableWnd(false)
					g_DelWndImage(self.m_AdvanceStone1,-1,IP_ENABLE, IP_CLICKDOWN)
					self:SetAdvanceStoneStateInBag(true)
					self.m_AdvanceStoneInfoTbl = nil
				end
			else
				g_DelWndImage(self.m_AdvanceStone1,-1,IP_ENABLE, IP_CLICKDOWN)	
				self.m_EquipUpBtn:EnableWnd(false)
				self:SetAdvanceStoneStateInBag(true)	
				self.m_AdvanceStoneInfoTbl = nil						
			end
		else
			g_DelWndImage(self.m_AdvanceStone1,-1,IP_ENABLE, IP_CLICKDOWN)
			self.m_EquipUpBtn:EnableWnd(false)
			self:SetAdvanceStoneStateInBag(true)
		end			
	else
		self.m_EquipUpBtn:EnableWnd( false )
		self:SetAdvanceStoneStateInBag(true)
	end
	
	if self.IntensifyNeedSoulNum >= self.IntensifyPourSoulNum then
		self.m_EquipIntensifyBtn:EnableWnd( true )
	end
	
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.ItemId)
	if DynInfo.CurAdvancePhase < DynInfo.MaxAdvancePhase then
		self.m_LabStatic1:ShowWnd(true)
	else
		self.m_LabStatic1:ShowWnd(false)
	end
	
	self.m_IntensifyBack:EnableWnd(true)
	self.m_EquipIntensifyPro:SetWndText( self.IntensifyPourSoulNum .. "/" .. self.IntensifyNeedSoulNum )
end

--设置精灵信息
function CEquipUpIntensify:SetSkillPieceBtnInfo(DynInfo)
	local IconTbl = {
						["火"] = g_ImageMgr:GetImagePath(1320),
						["冰"] = g_ImageMgr:GetImagePath(1321),
						["风"] = g_ImageMgr:GetImagePath(1322),
						["水"] = g_ImageMgr:GetImagePath(1323),
						["地"] = g_ImageMgr:GetImagePath(1324),
					}
	local IconStr =  IconTbl[DynInfo.JingLingType]
	
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(self.m_JingLingTypeBtn:GetGraphic(), -1,  IconStr, nil, CFPos:new(), "0xffffffff", 15 ) 	
	self.m_JingLingTypeBtn:SetWndBkImage( Flag, DefaultImage )

	local JingLingPieceSkillIDTbl = {
										DynInfo.AdvanceSkillPiece1,
										DynInfo.AdvanceSkillPiece2,
										DynInfo.AdvanceSkillPiece3,
										DynInfo.AdvanceSkillPiece4
									}
	for i = 1, #(JingLingPieceSkillIDTbl) do
		g_DelWndImage(self.m_SkillPieceBtnTbl[i],-1,IP_ENABLE, IP_CLICKDOWN)
		if JingLingPieceSkillIDTbl[i] ~= nil 
			and JingLingPieceSkillIDTbl[i] > 0 then
			local pieceTitleStr = string.format("%s%d", "SkillPieceName", JingLingPieceSkillIDTbl[i])
			local SkillPieceName = JingLingTypeToPieceSkill_Common(DynInfo.JingLingType, pieceTitleStr)
			local SmallIcon = JingLingPieceSkill_Common(SkillPieceName,"SkillSmallIcon") 
			self.m_SkillPieceBtnTbl[i]:SetMouseOverDescAfter(g_Tooltips:GetJingLingPieceTips(SkillPieceName))
			if DynInfo.ChoosedAdvanceSkillPiece == JingLingPieceSkillIDTbl[i] then
				g_LoadIconFromRes(SmallIcon, self.m_SkillPieceBtnTbl[i], -1,  IP_ENABLE, IP_CLICKDOWN)
				self.m_SkillPieceBtnTbl[i].IsChoosedSkillPiece = true
			else
				g_LoadGrayIconFromRes(SmallIcon, self.m_SkillPieceBtnTbl[i], -1,  IP_ENABLE, IP_CLICKDOWN)
				self.m_SkillPieceBtnTbl[i].IsChoosedSkillPiece = false
			end
			self.m_SkillPieceBtnTbl[i].JingLingPieceID = JingLingPieceSkillIDTbl[i]
			self.m_SkillPieceBtnTbl[i]:EnableWnd(true)
		else
			self.m_SkillPieceBtnTbl[i]:SetMouseOverDescAfter("")
			self.m_SkillPieceBtnTbl[i]:EnableWnd(false)
		end
	end
	
	if DynInfo.ChoosedAdvanceSkillPiece > 0 then
		self.m_JingLingLabStatic:ShowWnd(false)
	else
		self.m_JingLingLabStatic:ShowWnd(true)
	end
	
	for i = 1 ,7 do
		if i > DynInfo.MaxAdvancePhase then
			self.m_PhaseCheckBtnTbl[i]:ShowWnd(false)
		else
			self.m_PhaseCheckBtnTbl[i]:ShowWnd(true)
		end
	end
	
	for i = 1,#(self.m_PhaseCheckBtnTbl) do
		if i <= DynInfo.CurAdvancePhase then
			self.m_PhaseCheckBtnTbl[i]:SetCheck(true)
		else
			self.m_PhaseCheckBtnTbl[i]:SetCheck(false)
		end
	end
	
	if DynInfo.MaxAdvancePhase >= 2 then
		local rt = CFRect:new()
		self:GetLogicRect(rt)
		rt.top = rt.bottom
		rt.bottom = rt.top + self.m_AdvancePartWnd:GetWndOrgHeight()
		self.m_AdvancePartWnd:SetLogicRect(rt)
		self.m_AdvancePartWnd:ShowWnd(true)
		if CheckEquipCanIntensify(DynInfo.MaxAdvancePhase, DynInfo.CurAdvancePhase, DynInfo.IntensifyQuality) then
			rt.top = rt.bottom
			rt.bottom = rt.top + self.m_IntensifyPartWnd:GetWndOrgHeight()
			self.m_IntensifyPartWnd:SetLogicRect(rt)
			self.m_IntensifyPartWnd:ShowWnd(true)
		else
			self.m_IntensifyPartWnd:ShowWnd(false)
		end
	else
		self.m_AdvancePartWnd:ShowWnd(false)
		self.m_IntensifyPartWnd:ShowWnd(false)
	end 
end

--得到装备进阶属性描述
local function GetEquipAdvanceAttributeTbl(DynInfo)
	local StrTbl = {}
	for i= 1, 4 do
		local AttrName = DynInfo["AdvancedAttrName" .. i]
    	local AttrValue = DynInfo["AdvancedAttrValue" .. i]
    	if AttrName and AttrName ~= "" and AttrValue ~= 0 then
    	    local displayAttrName = Lan_PropertyName_Common(MemH64(AttrName),"DisplayName")
    		StrTbl[i] = displayAttrName .. ":" .. AttrValue
    	end    		
	end
	return StrTbl 	
end

function CEquipUpIntensify:AttrWndShow(Str,DynInfo, nBigID, nIndex, outputAttr, outputAttrCount, blockValue)
	self.m_AttrWndTbl[1]:SetWndText(Str) 
	local SetWndNumber = 1
	if DynInfo.DPS ~= nil then
		if g_ItemInfoMgr:IsWeapon(nBigID) then
			self.m_AttrWndTbl[SetWndNumber + 1]:SetWndText(GetStaticTextClient(5002) .. ": " .. DynInfo.AttackSpeed)
			SetWndNumber = SetWndNumber + 1
		end
	elseif g_ItemInfoMgr:IsStaticArmor(nBigID) or g_ItemInfoMgr:IsStaticJewelry(nBigID) then
	    local displayOutputAttr2 = Lan_PropertyName_Common(MemH64(outputAttr[2]),"DisplayName")
	    local displayOutputAttr3 = Lan_PropertyName_Common(MemH64(outputAttr[3]),"DisplayName")
		self.m_AttrWndTbl[SetWndNumber + 1]:SetWndText( displayOutputAttr2 .. ":" .. outputAttrCount[2])
		self.m_AttrWndTbl[SetWndNumber + 2]:SetWndText( displayOutputAttr3 .. ":" .. outputAttrCount[3])
		SetWndNumber = SetWndNumber + 2
		if DynInfo.StaticProValue ~= 0 then
		    local displayOutputAttr4 = Lan_PropertyName_Common(MemH64(outputAttr[4]),"DisplayName")
			self.m_AttrWndTbl[SetWndNumber + 1]:SetWndText(displayOutputAttr4 .. ":" .. outputAttrCount[4])
			SetWndNumber = SetWndNumber + 1
		end
	elseif g_ItemInfoMgr:IsStaticShield(nBigID) then
		if g_ItemInfoMgr:GetItemInfo(nBigID, nIndex,"IsShield") == 1 then 
			self.m_AttrWndTbl[SetWndNumber + 1]:SetWndText(GetStaticTextClient(5003) .. ":" .. blockValue)
			SetWndNumber = SetWndNumber + 1
		end
	end
	
	if DynInfo.CurAdvancePhase > 0 then 
		local StrTbl = GetEquipAdvanceAttributeTbl(DynInfo)
		for i = 1,#(StrTbl) do
			self.m_AttrWndTbl[SetWndNumber + 1]:SetWndText(StrTbl[i])
			SetWndNumber = SetWndNumber + 1 
		end
	end
	
	for i = 1 , 4 do
	    local attrName = DynInfo["AddAttr" .. i]
	    local attrValue = DynInfo["AddAttrValue" .. i]
	    if str ~= "" and attrValue ~= 0 then
    	    local displayAttrName = Lan_PropertyName_Common(MemH64(DynInfo["AddAttr" .. i]),"DisplayName")
    		local str = displayAttrName .. ":" .. DynInfo["AddAttrValue" .. i]
			self.m_AttrWndTbl[SetWndNumber + 1]:SetWndText(str)
			SetWndNumber = SetWndNumber + 1 
		else
			break
		end
	end
	for i = 1 ,#(self.m_AttrWndTbl) do
		if i <  SetWndNumber + 1 then
			self.m_AttrWndTbl[i]:ShowWnd(true)
		else		
			self.m_AttrWndTbl[i]:SetWndText("")
			self.m_AttrWndTbl[i]:ShowWnd(false)
		end
	end
end

function CEquipUpIntensify:GetSomeInfo(DynInfo, nBigID, nIndex)
	local outputAttr,outputAttrCount,Str,blockRate,blockValue = {},{},"",0,0
	if DynInfo.DPS ~= nil then
		if g_ItemInfoMgr:IsWeapon(nBigID) then --是武器
			local MaxAttack, MinAttack = GetWeaponDPSFloat(DynInfo.AttackSpeed, DynInfo.DPS,DynInfo.DPSFloorRate)
			Str = GetStaticTextClient(5008) .. MinAttack .. "-" .. MaxAttack 
		elseif g_ItemInfoMgr:IsRing(nBigID)  then
			local DamageType	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"DamageType")
			if DamageType == 1 then 
				Str = GetStaticTextClient(5006) .. DynInfo.DPS
			elseif DamageType == 2 then--物伤戒指
				Str = GetStaticTextClient(5005) .. DynInfo.DPS
			end
		end
	elseif g_ItemInfoMgr:IsStaticShield(nBigID) then
		local IsShield	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsShield")
		if IsShield == 1 then
			blockRate,blockValue = GetShieldBlock(DynInfo.CurLevel,nBigID, nIndex)
			Str = GetStaticTextClient(5007,blockRate*100) 
		elseif IsShield == 2 then
			local DamageValue = GetShieldMagicDamage(DynInfo.CurLevel,nBigID, nIndex)
			Str = GetStaticTextClient(5006) .. DamageValue			
		end
	else
		local IsStatic	= g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"IsStatic")
		if IsStatic == 0 or IsStatic == "" then
			outputAttr = GetEquipOutputAttrName(nBigID,nIndex)
			
		elseif IsStatic == 1 then
			local NameAndValueTbl = GetStaticPropertyName(nBigID,nIndex)
			for i = 1, #(NameAndValueTbl) do 
				table.insert(outputAttr ,GetAttrName(NameAndValueTbl[i][1]))
			end
		end
		
		outputAttrCount = {DynInfo.OutputAttr1, DynInfo.OutputAttr2, DynInfo.OutputAttr3,DynInfo.StaticProValue}	
		local outputAttr1=  Lan_PropertyName_Common(MemH64(outputAttr[1]),"DisplayName")
		Str = outputAttr1 .. ":" .. outputAttrCount[1]
	end
	return outputAttr, outputAttrCount, Str, blockRate, blockValue
end

--显示装备名称
function CEquipUpIntensify:EquipNamePanelShow(DynInfo)
	local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(DynInfo.m_nBigID,DynInfo.m_nIndex,"DisplayName")
	local PropertyStageName = g_Tooltips:GetEquipShowName(DynInfo,DisplayName, "*")	
	self.m_EquipNamePanel:SetWndText(PropertyStageName)
end

--显示进阶概率
function CEquipUpIntensify:ShowAdvancePorbInfo(DynInfo, nBigID,nIndex)
	local BaseLevel = DynInfo and DynInfo.BaseLevel or g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"BaseLevel")
	if DynInfo.CurAdvancePhase < 7 and DynInfo.CurAdvancePhase <  DynInfo.MaxAdvancePhase then
		if self.m_AdvanceStoneInfoTbl then
			local Probability = GetAdvanceSucPorb(DynInfo.CurAdvancePhase,BaseLevel) 
			if self.m_AdvanceStoneInfoTbl and self.m_AdvanceStoneInfoTbl[4] then
				local StoneProbability = AdvanceStone_Common(self.m_AdvanceStoneInfoTbl[4],"AddSucProbability")
				Probability = Probability + StoneProbability
			end
			if Probability > 1 or DynInfo.AdvancedTimes >= 150  then
				Probability = 1
			end
			
			local euqipType = g_ItemInfoMgr:GetItemInfo(nBigID,nIndex,"EquipType")
			local NeededSoulNumber = GetIntensifyNeededSoul(DynInfo.BaseLevel, euqipType, "AdvanceVanishSoulCount")
			local ProbabilityStr = GetStaticTextClient(5013,Probability*100)
			local NeededStoneNumber = 1
			if GetEquipHandType(euqipType) == "双" then
				NeededStoneNumber = 2
			end
			local NeededSoulStr =  GetStaticTextClient(5014,NeededStoneNumber)
			NeededSoulStr = NeededSoulStr .. "#r" .. GetStaticTextClient(5017,NeededSoulNumber) .. "#r" .. ProbabilityStr 
			self.m_RichText2:SetWndText( NeededSoulStr)
		else
			self.m_RichText2:SetWndText(GetStaticTextClient(5015))
		end
	end
	self.m_RichText1:ShowWnd(false)
end

--升级和强化面板的消息响应
function CEquipUpIntensify:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	local state, context = g_WndMouse:GetCursorState()
	if uMsgID == BUTTON_LCLICK then
		if(Child == self.m_EquipBtnClose) then
			if self.BTn and self.BTn.m_Info then
				local nBigID,nIndex,nItemID = self.BTn.m_Info:GetBigID(),self.BTn.m_Info:GetIndex(),self.BTn.m_Info:GetItemID()
				if	self.BTn.PartType  == 0 then
					g_SetWndMultiToolTips(self.BTn.iconbtn,nBigID,nIndex,nItemID)
				else
					g_SetItemRichToolTips(self.BTn.iconbtn,nBigID,nIndex,nItemID)
					g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips()
				end
				self:CloseSelf(self.BTn.m_Info:GetItemID())
			else
				self:ShowWnd(false)
			end
			self:SetWndFlashInfo()
		elseif (Child == self.m_AdvanceStone1) then 
	        self:SetAdvanceStoneInfo(Child, state,context)
		elseif Child == self.m_EquipUpBtn then		
			self:EquipAdvanceGac2GasFun() 
		elseif Child == self.m_EquipIcon then
            g_GameMain.m_RoleStatus:OnUpEquip(2)
            if self.BTn and self.BTn.m_Info then
                CEquipSuitTipWnd.UpdateEquipSuitTipWnd(self.BTn.m_Info:GetItemID())
            end
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
        if (Child == self.m_AdvanceStone1) then
            self:SetAdvanceStoneStateInBag(true)
            self.m_EquipUpBtn:EnableWnd(false)
        	self.m_AdvanceStoneInfoTbl = nil
        	self.m_AdvanceStone1:SetMouseOverDescAfter("")
    	    g_DelWndImage(Child,-1,IP_ENABLE, IP_CLICKDOWN)
    	    local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.BTn.m_Info:GetItemID())
    	    self:ShowAdvancePorbInfo(DynInfo, self.BTn.m_Info:GetBigID(),self.BTn.m_Info:GetIndex())
    	     g_ItemInfoMgr:PlayItemSound(self.BTn.m_Info:GetBigID(),self.BTn.m_Info:GetIndex())
        end
	end
end

function CEquipUpIntensify:SetIntensifyText()
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.ItemId)
	if CheckEquipCanIntensify(DynInfo.MaxAdvancePhase, DynInfo.CurAdvancePhase, DynInfo.IntensifyQuality) then
		local str = ""
		if DynInfo.IntensifyPhase >= DynInfo.IntensifyQuality then
			str = GetStaticTextClient(5018)
		else
			local euqipType = g_ItemInfoMgr:GetItemInfo(DynInfo.m_nBigID,DynInfo.m_nIndex,"EquipType")
			local NeededSoulNumber = GetIntensifyNeededSoul(DynInfo.BaseLevel, euqipType, "VanishSoulCount")		
			str = GetStaticTextClient(5020) .. "#r" .. GetStaticTextClient(5017,NeededSoulNumber)
			if DynInfo.IntensifyPhase >= 3 then
				local NeededStoneNumber = 1
				if GetEquipHandType(euqipType) == "双" then
					NeededStoneNumber = 2
				end				
				str = str .. "#r" .. GetStaticTextClient(5016,NeededStoneNumber)
			end
		end
		self.m_RichText2:SetWndText(str)
	elseif DynInfo.CurAdvancePhase >= DynInfo.MaxAdvancePhase then
		local str = GetStaticTextClient(5019)
		self.m_RichText2:SetWndText(str)
	end
end

local function IsJingLingReborn(Wnd,uButton)
	if uButton == MB_BtnOK then
		Gac2Gas:AdvancedEquipReborn(g_Conn, Wnd.ItemId, Wnd.BTn.PartType)
	end
	Wnd.m_MsgBox = nil
	return true
end

function CEquipUpIntensifyPart1:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	if uMsgID == BUTTON_LCLICK then
		if Child == Wnd.m_JingLingRebornBtn then
			Wnd.m_MsgBox = MessageBox(Wnd, MsgBoxMsg(1015),BitOr( MB_BtnOK, MB_BtnCancel),IsJingLingReborn,Wnd)
		else
			for i = 1 ,#(Wnd.m_SkillPieceBtnTbl) do
				if Child == Wnd.m_SkillPieceBtnTbl[i] 
					and not Wnd.m_SkillPieceBtnTbl[i].IsChoosedSkillPiece then
					Gac2Gas:ActiveJingLingSkillPiece(g_Conn, Wnd.BTn.m_Info:GetItemID(), Wnd.m_SkillPieceBtnTbl[i].JingLingPieceID,Wnd.BTn.PartType)
				end
			end		
		end
	end
end

function CEquipUpIntensifyPart2:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	local state, context = g_WndMouse:GetCursorState()
	if uMsgID == BUTTON_LCLICK then
		if(Child == Wnd.m_IntensifyBack )then
			local nBigID, nIndex, nItemID = Wnd.BTn.m_Info:GetBigID(),Wnd.BTn.m_Info:GetIndex(),Wnd.BTn.m_Info:GetItemID()
			SetEnableWnd(Wnd,false)
			Gac2Gas:EquipIntensifyBack(g_Conn, nBigID, nIndex, nItemID, Wnd.BTn.PartType)	
		elseif (Child == Wnd.m_IntensifyStone1) then
           Wnd:SetIntensifyStoneInfo(Child, state, context)	
           
        elseif (Child == Wnd.m_SuitCheckBtn) then
            if Wnd.m_EquipSuitTipWnd == nil then
               Wnd.m_EquipSuitTipWnd =  CEquipSuitTipWnd:new()
            end
            local isShow = Wnd.m_EquipSuitTipWnd:IsShow()
            if isShow then
                Wnd.m_EquipSuitTipWnd:ShowWnd(false)
            else
                local equipID = Wnd.BTn.m_Info:GetItemID()
                Wnd.m_EquipSuitTipWnd:ShowWnd(true)
                Wnd.m_EquipSuitTipWnd:SetEquipTipInfo(equipID)
            end
		end
	elseif uMsgID == BUTTON_CLICKDOWN then
		if Child == Wnd.m_EquipIntensifyBtn and Wnd.m_EquipIntensifyTick == nil then
			if Wnd.ItemId ~= nil and Wnd.ItemId ~= 0 then
				local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Wnd.ItemId)
				if not CheckEquipCanIntensify(DynInfo.MaxAdvancePhase, DynInfo.CurAdvancePhase, DynInfo.IntensifyQuality) then
					Child.m_MsgBox = MessageBox(Child, MsgBoxMsg(1000), MB_BtnOK)
					return
				end
			end
			if Wnd.m_IntensifyPhase >= 3 and Wnd.m_StoneInfoTbl == nil then
				Child.m_MsgBox = MessageBox(Child, MsgBoxMsg(1004), MB_BtnOK)
				return
			end
			Wnd.TempIntensify = Wnd.IntensifyPourSoulNum
			Wnd.m_EquipIntensifyTick = RegisterTick("EquipIntensify", EquipIntensify,50, Wnd)	
		end			
	
	end
	
end

function CEquipUpIntensify:SetIntensifyStoneInfo(Child, state,context)
    if state == ECursorState.eCS_PickupItem then --手上有物品
		local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
    	if fromRoom == g_AddRoomIndexClient.PlayerExpandBag then --如果是从背包栏拿出的包裹
    		g_WndMouse:ClearCursorAll()
    		return
    	end
    	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
    	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
    	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
    	local count = grid:Size()
    	local itemType, itemName, bindingType = grid:GetType() 
    	if IntensifyStoneMapAddAttr_Common(itemName, "AttrName") == nil  then
    	   MessageBox(self, MsgBoxMsg(1010), MB_BtnOK)
    	   return  
    	end
    	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"SmallIcon")	
    	g_LoadIconFromRes( SmallIcon, Child, -1, IP_ENABLE, IP_CLICKDOWN)
       	self.m_StoneInfoTbl = {fromRoom, srcPos, itemType, itemName}
    	g_WndMouse:ClearCursorAll()
    	
			g_ItemInfoMgr:PlayItemSound(itemType, itemName)
    else	
    	self.m_StoneInfoTbl = nil
    	g_DelWndImage(Child,1,IP_ENABLE, IP_CLICKDOWN)			
    end
end

--设置进阶石信息
function CEquipUpIntensify:SetAdvanceStoneInfo(Child, state,context)
	if state == ECursorState.eCS_PickupItem or 
	    state == ECursorState.eCS_PickupSplitItem then --手上有物品
		local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
    	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
    	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
    	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
    	local count = grid:Size()
    	local GlobalID = grid:GetItem():GetItemID()
    	local itemType, itemName, bindingType = grid:GetType() 
    	if not g_ItemInfoMgr:IsAdvanceStone(itemType) then
    	   MessageBox(self, MsgBoxMsg(1014), MB_BtnOK)
    	   return  
    	end
	
    	local SmallIcon = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"SmallIcon")
    	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.ItemId)
    	if not CheckAdvancedStoneAdaptedEquip(DynInfo.m_nBigID,DynInfo.m_nIndex, itemName, DynInfo.BaseLevel, DynInfo.CurAdvancePhase) then
    		MessageBox(self, MsgBoxMsg(1016), MB_BtnOK)
    		return
    	end
    	self:SetAdvanceStoneStateInBag(true)
        self.m_AdvanceStoneInfoTbl = nil
        g_DelWndImage(Child,1,IP_ENABLE, IP_CLICKDOWN)	
    	g_LoadIconFromRes( SmallIcon, Child, -1, IP_ENABLE, IP_CLICKDOWN)
       	self.m_AdvanceStoneInfoTbl = {fromRoom, srcPos, itemType, itemName} 
       	g_SetItemRichToolTips(Child,itemType,itemName,GlobalID)
       	self:SetAdvanceStoneStateInBag(false)
 		g_WndMouse:ClearCursorAll()
 		self:ShowAdvancePorbInfo(DynInfo, DynInfo.m_nBigID,DynInfo.m_nIndex)
 		g_ItemInfoMgr:PlayItemSound(itemType,itemName)
 		self.m_EquipUpBtn:EnableWnd(true)
	else
    	self:SetAdvanceStoneList()
	end
end

--重新设置强化面板信息
function CEquipUpIntensify:ChangeEquipInfo(PartInfo)
    local itemID =PartInfo.m_Info:GetItemID()
	g_GameMain.m_WndMainBag.m_ctItemRoom:SetQualityFlashInfo(PartInfo.iconbtn, itemID)
	if self:IsShow() and self.BTn and self.BTn.PartType == PartInfo.PartType then
		local tblInfo = {}
		tblInfo.m_Info        = PartInfo.m_Info
		tblInfo.PartType      = PartInfo.PartType
		tblInfo.iconbtn       = PartInfo.iconbtn
		self:InitData(tblInfo)
		g_GameMain.m_RoleStatus:ReSetUpBoxIcon(PartInfo.m_Info)
	end
end

function CEquipUpIntensify:UpdataAdvanceInfo()
	if self:IsShow() and self.ItemId then
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(self.ItemId)
		self:SetSkillPieceBtnInfo(DynInfo)
	end
end

function CEquipUpIntensify:UpDateSoulValue(total_soul)
	self.AllFetch = total_soul
	self.m_TotalSoulNum:SetWndText(total_soul)
end

function CEquipUpIntensify:CloseSelf(equipID)
	if( self:IsShow() and self.BTn and self.BTn.m_Info:GetItemID() == equipID ) then
		self:ShowWnd(false)
		if self.m_EquipSuitTipWnd ~= nil then
		    self.m_EquipSuitTipWnd:ShowWnd(false)
		end
	end
    if g_GameMain.m_EquipSuperaddWnd ~= nil then
        g_GameMain.m_EquipSuperaddWnd:CloseSelf()
    end
end

--设置进阶石所在包裹中格子的状态
function CEquipUpIntensify:SetAdvanceStoneStateInBag(bFlag, clearAdvanceInfo)
    local wnd = g_GameMain.m_EquipUpIntensifyWnd
    if wnd.m_AdvanceStoneInfoTbl ~= nil then
        local fromRoom, srcPos = wnd.m_AdvanceStoneInfoTbl[1], wnd.m_AdvanceStoneInfoTbl[2]
	    local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
        local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
        local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	    local count = grid:Size()
        if clearAdvanceInfo == nil or (clearAdvanceInfo and count < 1) then
            SrcCt:SetClickedWndState(x, y, fromRoom, bFlag)
            if bFlag then
                wnd.m_AdvanceStone1:SetMouseOverDescAfter("")
            end
        end
    end
end

function CEquipUpIntensify:VirtualExcludeWndClosed()
	g_GameMain.m_RoleStatus:ClearUpIconWnd()
    self:CloseMenuList()
end

local function UpdateEquipBtnQualityInRoleState(equipPart, uItemId)
    local Wnd = g_GameMain.m_RoleStatus
	local Iconbtn
	for i = 1, nAllPartNum do
		if equipPart == Wnd.Part[i].PartType then
			Iconbtn = Wnd.Part[i].iconbtn
			break
		end
	end
  g_GameMain.m_WndMainBag.m_ctItemRoom:SetQualityFlashInfo(Iconbtn, uItemId)
end

--注销Tick
function CEquipUpIntensify:UnRegisterSelfTick()
	if self.m_EquipIntensifyTick ~= nil then
		UnRegisterTick(self.m_EquipIntensifyTick)
		self.m_EquipIntensifyTick = nil
	end

	if self.m_EquipUpgradeTick ~= nil then
		UnRegisterTick(self.m_EquipUpgradeTick)
		self.m_EquipUpgradeTick = nil
	end	
end

--根据包裹中查到的进阶石列表，筛选出符合要进阶的装备的进阶石
function CEquipUpIntensify:GetAdvanceStoneList()
    local stoneType =  g_ItemInfoMgr:GetAdvanceStoneBigID()
    local stoneList = g_GameMain.m_WndMainBag.m_ctItemRoom:GetAdaptedItemList(stoneType)
  
    local equipType, equipName, nItemID = self.BTn.m_Info:GetBigID(),self.BTn.m_Info:GetIndex(),self.BTn.m_Info:GetItemID()    
    local equipDynInfo =  g_DynItemInfoMgr:GetDynItemInfo(nItemID) 

    for i, v in pairs(stoneList) do
        local stoneName = i
        local bFlag = CheckAdvancedStoneAdaptedEquip(equipType, equipName, stoneName, equipDynInfo.BaseLevel, equipDynInfo.CurAdvancePhase)
        local AdaptedEquipLevelScale = g_ItemInfoMgr:GetItemInfo(stoneType, stoneName,"AdaptedEquipLevelScale")
        local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(stoneType, stoneName,"DisplayName")
        if  bFlag ~= true then
            stoneList[stoneName] = nil
        else
            stoneList[stoneName].Level = AdaptedEquipLevelScale
            stoneList[stoneName].DisplayName = DisplayName
        end
    end

    table.sort(stoneList, function(a, b) return a.Level < b.Level end)	
    return stoneList
end

function CEquipUpIntensify:OnClickAdvanceStone()
    local wnd = g_GameMain.m_EquipUpIntensifyWnd
    local showStr = wnd.m_Menu:GetItemName()
    local index = wnd.m_Menu:GetCurSelectedItemIndex()
    local stoneName = wnd.m_StoneListMapMenuItem[index]
    local roomIndex = wnd.m_StoneList[stoneName].RoomIndex
    local pos = wnd.m_StoneList[stoneName].Pos
   	local stoneType = g_ItemInfoMgr:GetAdvanceStoneBigID()
   	local SmallIcon = g_ItemInfoMgr:GetItemInfo(stoneType, stoneName,"SmallIcon")
   	local stoneID = wnd.m_StoneList[stoneName].ID
   	wnd:SetAdvanceStoneStateInBag(true)
    wnd.m_AdvanceStoneInfoTbl = nil
    g_DelWndImage(wnd.m_AdvanceStone1,1,IP_ENABLE, IP_CLICKDOWN)		
	g_LoadIconFromRes(SmallIcon, wnd.m_AdvanceStone1, -1, IP_ENABLE, IP_CLICKDOWN)
   	wnd.m_AdvanceStoneInfoTbl = {roomIndex, pos, stoneType, stoneName}
   	wnd:SetAdvanceStoneStateInBag(false)
   	g_SetItemRichToolTips(wnd.m_AdvanceStone1,stoneType,stoneName,stoneID)
   	if wnd.BTn.m_Info then
   		local equipType, equipName, nItemID = wnd.BTn.m_Info:GetBigID(),wnd.BTn.m_Info:GetIndex(),wnd.BTn.m_Info:GetItemID()    
   		local DynInfo =  g_DynItemInfoMgr:GetDynItemInfo(nItemID)
   		wnd:ShowAdvancePorbInfo(DynInfo, equipType, equipName)	
   		wnd.m_EquipUpBtn:EnableWnd(true)
   	end
end

function CEquipUpIntensify:CloseMenuList(bFlag)
    if self.m_Menu ~= nil then
       self.m_Menu:ShowWnd(false) 
    end
    self:SetAdvanceStoneStateInBag(true, bFlag)
end

function CEquipUpIntensify:SetAdvanceStoneList()
    self.m_Menu = CMenu:new("", self)
    self.m_Menu:SetStyle( 0x40000000 )
    self.m_StoneList = self:GetAdvanceStoneList()
    self.m_StoneListMapMenuItem = {}
    local index = 0
    for i, v in pairs(self.m_StoneList) do
        local stoneName = i
        local stoneCount = v.Count
        local showStr = v.DisplayName .. "  *" .. stoneCount
        self.m_Menu:InsertItem(showStr, self.OnClickAdvanceStone, nil,false,false,nil)     
        index = index + 1
        self.m_StoneListMapMenuItem[index] = stoneName
    end
    if index > 0 then
        local Rt = CFRect:new()
    	self.m_AdvanceStone1:GetLogicRect(Rt)
    	self.m_Menu:SetPos( Rt.left, Rt.bottom )
    else
        self.m_Menu:ShowWnd(false)
    end
end

function CEquipUpIntensify:AddAdaptedAdvanceStone(context)
    local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	if fromRoom == g_AddRoomIndexClient.PlayerExpandBag then --如果是从背包栏拿出的包裹
		g_WndMouse:ClearCursorAll()
		return false
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local count = grid:Size()
    local GlobalID = grid:GetItem():GetItemID()
	local stoneType, stoneName, bindingType = grid:GetType() 
	if g_ItemInfoMgr:IsAdvanceStone(stoneType) ~= true then
	   return false 
	end
	if self.BTn == nil then
		return
	end
    local equipType, equipName, nItemID = self.BTn.m_Info:GetBigID(),self.BTn.m_Info:GetIndex(),self.BTn.m_Info:GetItemID()    
    local equipDynInfo =  g_DynItemInfoMgr:GetDynItemInfo(nItemID)
    local bFlag = CheckAdvancedStoneAdaptedEquip(equipType, equipName, stoneName, equipDynInfo.BaseLevel, equipDynInfo.CurAdvancePhase)
    if  bFlag ~= true then
        MessageBox(self, MsgBoxMsg(1016), MB_BtnOK)
        return false
    else
        self:SetAdvanceStoneStateInBag(true)
        g_DelWndImage(self.m_AdvanceStone1,1,IP_ENABLE, IP_CLICKDOWN)	
        g_SetItemRichToolTips( self.m_AdvanceStone1,stoneType,stoneName,GlobalID)
        local SmallIcon = g_ItemInfoMgr:GetItemInfo(stoneType, stoneName,"SmallIcon")
    	g_LoadIconFromRes(SmallIcon, self.m_AdvanceStone1, -1, IP_ENABLE, IP_CLICKDOWN)
    	self.m_AdvanceStone1:SetFocus()
       	self.m_AdvanceStoneInfoTbl = {fromRoom, srcPos, stoneType, stoneName}
       	self:SetAdvanceStoneStateInBag(false)
       	self.m_EquipUpBtn:EnableWnd(true)
       	self:ShowAdvancePorbInfo(equipDynInfo, equipType,equipName)
    end
  	g_ItemInfoMgr:PlayItemSound(stoneType, stoneName)
end
---------------------------------rpc调用函数----------------------------------------------
--装备精灵重生返回
function CEquipUpIntensify.AdvancedEquipReborn(ItemID)
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemID)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	if Wnd.ItemId == ItemID then
		Wnd:SetSkillPieceBtnInfo(DynInfo)
	end
	g_GameMain.m_RoleStatus:SetPieceSkillNameToNumberTbl()
	if g_GameMain.m_CommonSkillWnd:IsShow() then
		UpdateSkillAllWndRect()
	end
end

--注强化魂失败返回
function CEquipUpIntensify.NoticePourIntensifyError(uItemId, uSoulNum)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(uItemId)
	if DynInfo == nil then
		return
	end
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,uItemId,DynInfo.BindingType)
	g_MainPlayer.m_PlayerSoulNum = g_MainPlayer.m_PlayerSoulNum + uSoulNum
	Wnd.AllFetch = g_MainPlayer.m_PlayerSoulNum
	DynInfo.ItensifySoul = DynInfo.ItensifySoul - uSoulNum
	Wnd:UpDate(Item)	
end

--注强化魂成功返回
function CEquipUpIntensify.NoticePourIntensifySuccess(uItemId)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(uItemId)
	local EquipType	= g_ItemInfoMgr:GetItemInfo(DynInfo.m_nBigID,DynInfo.m_nIndex,"EquipType")
	local AllNumber = GetIntensifyNeededSoul(DynInfo.BaseLevel,EquipType)
	if DynInfo.ItensifySoul == AllNumber and Wnd.ItemId == uItemId then
		Wnd.TempIntensify = 0
		local isInIntensifyState,msgId = IsInIntensifyState(Wnd)
		if isInIntensifyState then
			MsgClient(msgId)
			return
		end
		Wnd:SendIntensifyRequest(DynInfo)
		return
	end
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,uItemId,DynInfo.BindingType)	
	Wnd:UpDate(Item)	
end

--服务器进阶成功返回
function CEquipUpIntensify.NoticeAdvanceSuccess(uItemId)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(uItemId)
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,uItemId,DynInfo.BindingType)
	local clearAdvanceFlag = nil
	if DynInfo.CurAdvancePhase < 7 then
	    clearAdvanceFlag = true
	end
	
	Wnd:UpDate(Item, clearAdvanceFlag)
	 g_GameMain.m_RoleStatus.m_bLoadPiece = nil
	 g_GameMain.m_RoleStatus:DrawRole()
end

--服务器进阶出错返回
function CEquipUpIntensify.NoticeAdvanceError(uItemId)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	if uItemId == 0 then
		return
	end
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(uItemId)
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,uItemId,DynInfo.BindingType)
	Wnd:UpDate(Item)
	g_GameMain.m_RoleStatus.m_bLoadPiece = nil
	g_GameMain.m_RoleStatus:DrawRole()
end

--服务器端强化成功返回
function CEquipUpIntensify.NoticeIntensifySuccess(uItemId, equipPart)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(uItemId)
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,uItemId,DynInfo.BindingType)
	Wnd:UpDate(Item)
	Wnd.m_StoneInfoTbl = nil
	g_DelWndImage(Wnd.m_IntensifyStone1, 1, IP_ENABLE, IP_CLICKDOWN)
    if equipPart == 0 then--包裹中的装备
	    local iconBtn = g_GameMain.m_RoleStatus.iconWnd
	    g_GameMain.m_WndMainBag.m_ctItemRoom:SetQualityFlashInfo(iconBtn, uItemId)
    else
        UpdateEquipBtnQualityInRoleState(equipPart,uItemId)
    end
     g_GameMain.m_RoleStatus.m_bLoadPiece = nil
	 g_GameMain.m_RoleStatus:DrawRole()
	 CEquipSuitTipWnd.UpdateEquipSuitTipWnd(uItemId)	
end

function CEquipUpIntensify.RetEquipIntenBack( equipRoom, equipPos, equipPart, equipID)
	if equipPart == 0 then
		g_GameMain.m_WndMainBag.m_ctItemRoom:SetItemFlashInfo(equipRoom, equipPos)
	else
		UpdateEquipBtnQualityInRoleState(equipPart, equipID)
	end
	g_GameMain.m_RoleStatus:UpdataEquipTips()
	g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(true, equipID, bOriginalPrice)
	CEquipSuitTipWnd.UpdateEquipSuitTipWnd(equipID)	
end

--更新强化面板信息。
function CEquipUpIntensify.UpdateEquipIntensifyInfo(uItemId)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	if uItemId == 0 then
		return
	end
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(uItemId)
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,uItemId,DynInfo.BindingType)
	Wnd:UpDate(Item)	
	
end

--将魂值添加到装备上成功后，server的返回接口
function CEquipUpIntensify.NoticeAbsorbSoul(uNum, nItemId)
	SetEvent(Event.Test.SoulInEquip,true)
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemId)
	local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,nItemId,DynInfo.BindingType)
	Wnd:UpDate(Item)	
	g_GameMain.m_RoleStatus:UpdataEquipTips()
end

function CEquipUpIntensify.ReturnModifyPlayerSoulNum(total_soul)
	if g_MainPlayer then
		g_MainPlayer.m_PlayerSoulNum = total_soul
	end
	if g_GameMain.m_RoleStatus then
		g_GameMain.m_RoleStatus:UpDateSoulValue(total_soul)
	end
	local equipUpIntensifyWnd = g_GameMain.m_EquipUpIntensifyWnd
	if equipUpIntensifyWnd then
		equipUpIntensifyWnd:UpDateSoulValue(total_soul)
	end
	g_GameMain.m_WndMainBag:UpDateSoulValue(total_soul)
end

function CEquipUpIntensify.RetActiveJingLingSkillPiece(ItemID,ActvieIndex,EquipPart)
	local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemID)
	DynInfo.ChoosedAdvanceSkillPiece = ActvieIndex
	g_GameMain.m_RoleStatus:SetPieceSkillNameToNumberTbl()
	if g_GameMain.m_CommonSkillWnd:IsShow() then
		UpdateSkillAllWndRect()
	end
	local Wnd = g_GameMain.m_EquipUpIntensifyWnd
	if Wnd.ItemId == ItemID then
		local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(ItemID)
		local Item = CItem:new(DynInfo.m_nBigID,DynInfo.m_nIndex,ItemID,DynInfo.BindingType)
		Wnd:UpDate(Item)
	end
end