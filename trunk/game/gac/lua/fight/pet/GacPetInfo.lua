gac_require "fight/pet/GacPetInfoInc"
cfg_load "servant/Pet_Common"
cfg_load "servant/Pet_Skill_Common"


--幻兽面板
function  CreatePetInfoWnd(Parent)
	local wnd = CPetInfo:new()
	wnd:Init(Parent)
	wnd:ShowWnd(false)
	return wnd
end

function CPetInfo:Init(Parent)
	self:CreateFromRes("PetWnd",Parent)
	self.m_InpourSoulBtn		= self:GetDlgChild( "InpourSoulBtn" )
	self.m_VimText					= self:GetDlgChild( "VimText" )
	self.m_ActivationBtn		= self:GetDlgChild( "ActivationBtn" )
	self.m_ChangeNameBtn		= self:GetDlgChild( "ChangeNameBtn" )
	self.m_DiscardBtn				= self:GetDlgChild( "DiscardBtn" )
	self.m_PrepareBtn				= self:GetDlgChild( "PrepareBtn" )
	self.m_PetModel					= self:GetDlgChild( "PetModel" )
	self.m_PropertyShowWnd	= self:GetDlgChild( "PropertyShowWnd" )
	self.m_NameAndLevelWnd	= self:GetDlgChild( "NameAndLevelWnd" )
	self.m_CloseBtn					= self:GetDlgChild( "CloseBtn" )
	self.m_PetTypeText			= self:GetDlgChild("PetTypeText")
	self.m_PetPhaseText			= self:GetDlgChild("PetPhaseText")
	self.m_PetAptitudeText	= self:GetDlgChild("PetAptitudeText")
	self.m_HealthPointText	= self:GetDlgChild("HealthPointText")
	self.m_DamageValueText	= self:GetDlgChild("DamageValueText")
	self.m_ExpValueText			= self:GetDlgChild("ExpValueText")
	self.m_NameAndLevelList	= {}
	self.m_PrepareBtn:EnableWnd(false)
	self.m_SkillNameTbl = {}
	self.m_IsFirstOpenPetWnd = false
	self.m_uChoosedIndex = -1
	self.m_SkillBtnTbl = {}
	for i = 1,6 do
		self.m_SkillBtnTbl[i] = self:GetDlgChild( "SkillBtn" .. i)
		self.m_SkillBtnTbl[i].m_bHasIcon = false
	end
end

function CPetInfo:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_InpourSoulBtn) then
			self:PetEggPourSoul()
		elseif (Child == self.m_ChangeNameBtn) then
			self:CreatePetChangeNameWnd(self)
		elseif (Child == self.m_DiscardBtn) then
			self:ThrowAwayPet()
		elseif (Child == self.m_PrepareBtn) then	
			self:PreparePet()
		elseif (Child == self.m_ActivationBtn) then
			
		end
	elseif (uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_NameAndLevelWnd) then
			local chooseItemIndex = self.m_NameAndLevelWnd:GetSelectItem( -1 ) + 1
			self:InitPetInfoWnd(chooseItemIndex)
		end
	end
end

--创建幻兽列表的行窗体
function CPetInfo:CreatePetListItemWnd(Parent)
	local Wnd = CPetListItem:new()
	Wnd:CreateFromRes("PetListItem", Parent)
	Wnd:ShowWnd(true)
	return Wnd
end

local function GetWndWidth(ChildWnd)
	local rt = CFRect:new()
	ChildWnd:GetLogicRect(rt)
	local width = rt.right-rt.left
	return width
end

function CPetInfo:ClearPetWnd()
	self.m_PetTypeText:SetWndText("类型:    " )
	self.m_PetPhaseText:SetWndText("时期:   ")
	self.m_PetAptitudeText:SetWndText("")
	self.m_HealthPointText:SetWndText("")
	self.m_DamageValueText:SetWndText("")
	self.m_ExpValueText:SetWndText("")
	self.m_VimText:SetWndText("")
	self.m_ChangeNameBtn:EnableWnd(false)
	self.m_DiscardBtn:EnableWnd(false)
	self.m_PrepareBtn:EnableWnd(false)
	self.m_ActivationBtn:EnableWnd(false)
	self:InitSkill()
end

--幻兽列表行
function CPetInfo:InitPetWnd(index)
	self.m_NameAndLevelWnd:DeleteAllItem()
	self.tblPetItem = {}
	self.m_NameAndLevelWnd:InsertColumn(0,GetWndWidth(self.m_NameAndLevelWnd))
	local tbl = self.m_NameAndLevelList
	if #tbl == 0 then
		self:ClearPetWnd()
		return
	else
		for i = 1, #tbl do
			self.m_NameAndLevelWnd:InsertItem(i-1, 20)
			local item = self.m_NameAndLevelWnd:GetSubItem(i-1, 0)
			local petItem = self:CreatePetListItemWnd(item)
			if tbl[i][1] == 1 then
				petItem:SetPetItemText(tbl[i][2][2], tbl[i][2][3] .. "级")
			elseif tbl[i][1] == 2 then
				petItem:SetPetItemText(tbl[i][2][1], "0级")
			end
			table.insert(self.tblPetItem, petItem)
			if (index == 0 and self.m_uChoosedIndex ~= -1 and i == self.m_uChoosedIndex + 1) or (i == index + 1) then
				self:InitNameAndLevelList(tbl[i][1],tbl[i][2])
			end
		end
	end
	
	if index == 0 and self.m_uChoosedIndex ~= -1 then
		self.m_NameAndLevelWnd:SelectItem(self.m_uChoosedIndex)
	else
		self.m_NameAndLevelWnd:SelectItem(index)
	end
end


function CPetInfo:InitWnd()
	if self.m_uChoosedIndex ~= -1 then
		local chooseItemIndex = self.m_NameAndLevelWnd:GetSelectItem( -1 )
		if chooseItemIndex ~= self.m_uChoosedIndex then
			self:DrawPetList(self.m_uChoosedIndex)
		end
	end
end

--幻兽列表行
function CPetInfo:DrawPetList(index)
	local tbl = self.m_NameAndLevelList
	if #tbl == 0 then
		self:ClearPetWnd()
		return
	else
		self.m_NameAndLevelWnd:SelectItem(index)
		for i = 1, #tbl do
			if (i == index + 1) then
				self:InitNameAndLevelList(tbl[i][1],tbl[i][2])
			end
		end
	end
end

--幻兽蛋孵化
function CPetInfo:PetEggPourSoul()
	local chooseItemIndex = self.m_NameAndLevelWnd:GetSelectItem( -1 )
	if chooseItemIndex == -1 then
		return
	end
	local chooseItem = self.tblPetItem[chooseItemIndex + 1]
	local petEggName = chooseItem.m_PetName:GetWndText()
	
	local petEggItem = g_ItemInfoMgr:GetItemFunInfo(41, petEggName)
	if not petEggItem then
		return
	end
	
	local petName = petEggItem("PetName")
	local petInfo = Pet_Common(petName)
	if not petInfo then
		return
	end
	
	local vimValue = tonumber( petInfo("VimValue") )
	local consumeParam = tonumber( petInfo("ConsumeParam") )
	local costSoulValue = vimValue * consumeParam
	
	local function CallBack(g_GameMain, index)
		if index == MB_BtnOK then
			Gac2Gas:PetEggPourSoul(g_Conn,petEggName,costSoulValue)
		end
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(14004,costSoulValue),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
end

function CPetInfo:InitSkill()
	for i = 1, 6 do
		if self.m_SkillBtnTbl[i].m_bHasIcon then
			g_DelWndImage(self.m_SkillBtnTbl[i], -1, IP_ENABLE, IP_CLICKDOWN)
		end
	end
end

function CPetInfo:InitPetEggInfo(petType)
	self.m_PetTypeText:SetWndText("类型:    " .. petType)
	self.m_PetPhaseText:SetWndText("时期:    " .. "孵化期")
	self.m_PetAptitudeText:SetWndText("")
	self.m_HealthPointText:SetWndText("")
	self.m_DamageValueText:SetWndText("")
	self.m_ExpValueText:SetWndText("")
	self.m_VimText:SetWndText("")
	self.m_ChangeNameBtn:EnableWnd(false)
	self.m_DiscardBtn:EnableWnd(true)
	self.m_PrepareBtn:EnableWnd(false)
	self.m_ActivationBtn:EnableWnd(false)
	self:InitSkill()
end

function CPetInfo:InitPetInfo(petTbl)
	local uDesMaxHP = CStaticAttribMgr_CalcBasicMaxHealthPoint(5,petTbl[3] )
	local uDesMaxMP = CStaticAttribMgr_CalcBasicMaxManaPoint(5,petTbl[3] )
	self.m_PetTypeText:SetWndText("类型:    " .. petTbl[4])
	self.m_PetPhaseText:SetWndText("时期:    " .. petTbl[5])
	self.m_PetAptitudeText:SetWndText("资质:    " .. petTbl[6])
	self.m_HealthPointText:SetWndText("生命值:   " .. petTbl[7] .. "/" .. uDesMaxHP)
	self.m_DamageValueText:SetWndText("法力值:   " .. petTbl[8] .. "/" .. uDesMaxMP)
	self.m_ExpValueText:SetWndText("经验值:   " .. petTbl[9])
	self.m_VimText:SetWndText("活力值:   " .. petTbl[10])
	self.m_ChangeNameBtn:EnableWnd(true)
	self.m_DiscardBtn:EnableWnd(true)
	self.m_PrepareBtn:EnableWnd(true)
	self.m_ActivationBtn:EnableWnd(true)
	self:InitSkill()
	local skillTbl = self.m_SkillNameTbl[petTbl[1]]
	if skillTbl and #skillTbl > 0 then
		for i = 1, #skillTbl do
			g_LoadIconFromRes( Pet_Skill_Common(skillTbl[i][1], "SmallIcon"), self.m_SkillBtnTbl[skillTbl[i][2]], -2, IP_ENABLE, IP_CLICKDOWN)
			self.m_SkillBtnTbl[skillTbl[i][2]].m_bHasIcon = true
			self.m_SkillBtnTbl[skillTbl[i][2]]:SetMouseOverDescAfter( Pet_Skill_Common(skillTbl[i][1], "ToolTipsDesc") )
		end
	end
end

function CPetInfo:InitNameAndLevelList(uflag,petTbl)
	if uflag == 1 then --给面板上加载的是幻兽
		self:InitPetInfo(petTbl)
	elseif uflag == 2 then --给面板上加载的是幻兽蛋
		self:InitPetEggInfo(petTbl[2])
	end
end

function CPetInfo:InitPetInfoWnd(chooseItemIndex)
	local petTbl = self.m_NameAndLevelList[chooseItemIndex]
	if petTbl[1] == 2 then
		self:InitPetEggInfo(petTbl[2][2])
	else
		self:InitPetInfo(petTbl[2])
	end
end

function CPetListItem:OnChildCreated()
	self.m_PetName = self:GetDlgChild("PetName")
	self.m_PetLevel = self:GetDlgChild("PetLevel")
end

function CPetListItem:SetPetItemText(sPetName, uPetLevel)
	self.m_PetName:SetWndText(sPetName)
	self.m_PetLevel:SetWndText(uPetLevel)
end

--幻兽放生
function CPetInfo:ThrowAwayPet()
	local function CallBack(g_GameMain, index)
		if index == MB_BtnOK then
			local chooseItemIndex = self.m_NameAndLevelWnd:GetSelectItem( -1 ) + 1
			local tbl = self.m_NameAndLevelList[chooseItemIndex]
			if tbl[1] == 2 then
				local petEggName = tbl[2][1]
				Gac2Gas:ThrowAwayPetEgg(g_Conn,petEggName)
			else
				local petId = tbl[2][1]
				Gac2Gas:ThrowAwayPet(g_Conn,petId)
			end
		end
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(14003),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
end

--准备幻兽
function CPetInfo:PreparePet()
	local chooseItemIndex = self.m_NameAndLevelWnd:GetSelectItem( -1 ) + 1
	local tbl = self.m_NameAndLevelList[chooseItemIndex]
	self.m_uChoosedIndex = chooseItemIndex - 1
	if tbl[1] == 1 then
		local petId = tbl[2][1]
		--Gac2Gas:PreparePet(g_Conn,petId)
	end
end

--创建幻兽列表的行窗体
function CPetInfo:CreatePetChangeNameWnd(Parent)
	local Wnd = CPetChangeName:new()
	Wnd:CreateFromRes("PetChangeNameWnd", Parent)
	Wnd.m_OKBtn = Wnd:GetDlgChild("OKBtn")
	Wnd.m_CancelBtn = Wnd:GetDlgChild("CancelBtn")
	Wnd.m_PromptMsg = Wnd:GetDlgChild("PromptMsg")
	Wnd.m_NewPetName = Wnd:GetDlgChild("NewPetName")
	Wnd.m_PromptMsg:SetWndText("请输入幻兽的新名字!")
	Wnd:ShowWnd(true)
	return Wnd
end


--幻兽改名
function CPetChangeName:ChangePetName()
	local  newName = self.m_NewPetName:GetWndText()
	local wnd = g_GameMain.m_PetInfoWnd
	local chooseItemIndex = wnd.m_NameAndLevelWnd:GetSelectItem( -1 ) + 1
	local tbl = wnd.m_NameAndLevelList[chooseItemIndex]
	if tbl[1] == 2 then
		return
	else
		local petId = tbl[2][1]
		Gac2Gas:ChangePetName(g_Conn,petId,newName)
	end
end

function CPetChangeName:OnCtrlmsg(Child,uMsgID,uParam1,uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if (Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_OKBtn) then
			self:ChangePetName()
			self:ShowWnd(false)
		end
	end
end
-------------------------------------------------RPC消息---------------------------------------------------
function Gas2Gac:ReturnIncubatePetEgg(Conn,succ,petEggName,petType)
	local wnd = g_GameMain.m_PetInfoWnd
	if succ then
		wnd:ShowWnd(true)
		table.insert(wnd.m_NameAndLevelList,{2,{petEggName,petType}})
	end
	wnd:InitPetWnd(#wnd.m_NameAndLevelList - 1)
end

function Gas2Gac:ReturnPetEggPourSoul(Conn,petId,petName,petEggName,aptitudeValue,petType,petHealthPoint,petDamageValue,uExpValue,petVimValue,skillName1,skillName2)
	local wnd = g_GameMain.m_PetInfoWnd
	local tbl = wnd.m_NameAndLevelList
	local index
	for i = 1, #tbl  do
		if tbl[i][2][1] == petEggName then
			tbl[i] = {1,{petId,petName,1,petType,"幼年期",aptitudeValue,petHealthPoint,petDamageValue,uExpValue,petVimValue}}
			index = i - 1
			break
		end
	end
	if not wnd.m_SkillNameTbl[petId] then
		wnd.m_SkillNameTbl[petId] = {}
	end
	table.insert(wnd.m_SkillNameTbl[petId],{skillName1,1})
	if skillName2 ~= "" then
		table.insert(wnd.m_SkillNameTbl[petId],{skillName2,2})
	end
	wnd:InitPetWnd(index)
end

function Gas2Gac:RetInitPetWndInfoBegin(Conn)
	local wnd = g_GameMain.m_PetInfoWnd
	wnd.m_IsFirstOpenPetWnd = true
end

function Gas2Gac:ReturnCharPetInfo(Conn,petId,petName,petType,petLevel,aptitudeValue,petHealthPoint,petDamageValue,uExpValue,petVimValue)
	local wnd = g_GameMain.m_PetInfoWnd
	local petPhase
	if petLevel >= 1 and petLevel < 30 then
		petPhase = "幼年期"
	end
	table.insert(wnd.m_NameAndLevelList,{1,{petId,petName,petLevel,petType,petPhase,aptitudeValue,petHealthPoint,petDamageValue,uExpValue,petVimValue}})
end

function Gas2Gac:ReturnCharPetEggInfo(Conn,petEggName,petType)
	local wnd = g_GameMain.m_PetInfoWnd
	table.insert(wnd.m_NameAndLevelList,{2,{petEggName,petType}})
end

function Gas2Gac:ReturnCharPetInfoEnd(Conn)
	local wnd = g_GameMain.m_PetInfoWnd
	wnd:InitPetWnd(0)
end

function Gas2Gac:ReturnThrowAwayPetEgg(Conn,petName)
	local wnd = g_GameMain.m_PetInfoWnd
	local tbl = wnd.m_NameAndLevelList
	for i = 1, #tbl do
		if tbl[i][2][1] == petName then
			table.remove(wnd.m_NameAndLevelList, i)
			break
		end
	end
	wnd:InitPetWnd(0)
end


function Gas2Gac:ReturnThrowAwayPet(Conn,petId,bIsPreparePet)
	local wnd = g_GameMain.m_PetInfoWnd
	local tbl = wnd.m_NameAndLevelList
	for i = 1, #tbl do
		if tbl[i][2][1] == petId then
			table.remove(wnd.m_NameAndLevelList, i)
			break
		end
	end
	if bIsPreparePet then
		wnd.m_uChoosedIndex = -1
	end
	wnd:InitPetWnd(0)
end

function Gas2Gac:ReturnChangePetName(Conn,petId,newName)
	local wnd = g_GameMain.m_PetInfoWnd
	local tbl = wnd.m_NameAndLevelList
	local index
	for i = 1, #tbl do
		if tbl[i][2][1] == petId then
			local item_tbl = tbl[i]
			item_tbl[2][2] = newName
			wnd.m_NameAndLevelList[i] = item_tbl
			index = i - 1
			break
		end
	end
	wnd:InitPetWnd(index)
end

function Gas2Gac:ReturnPreparePet(Conn,petId)
	local wnd = g_GameMain.m_PetInfoWnd
	local tbl = wnd.m_NameAndLevelList
	local index
	for i = 1, #tbl do
		if tbl[i][2][1] == petId then
			index = i - 1
			break
		end
	end
	wnd:DrawPetList(index)
	wnd.m_uChoosedIndex = index
end

function Gas2Gac:ReturnUsePetSkillStone(Conn,petId,skillName,posIndex)
	local wnd = g_GameMain.m_PetInfoWnd
	if not wnd.m_SkillNameTbl[petId] then
		wnd.m_SkillNameTbl[petId] = {}
	end
	table.insert(wnd.m_SkillNameTbl[petId],{skillName,posIndex})
	local tbl = wnd.m_NameAndLevelList
	local index
	for i = 1, #tbl do
		if tbl[i][2][1] == petId then
			index = i - 1
			break
		end
	end
	wnd:DrawPetList(index)
end



