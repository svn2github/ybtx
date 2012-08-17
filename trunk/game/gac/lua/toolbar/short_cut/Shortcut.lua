gac_require( "toolbar/short_cut/ShortcutInc" )
gac_require( "item/wnd_item_bag/GacUseItem" )
gac_gas_require( "shortcut/ShortcutCommon" )
gac_require "setting/key_map/KPmap"
gac_require "framework/texture_mgr/DynImageMgr"

function GetRenderTime(pCharacter)
	if pCharacter then
		local RenderObj = pCharacter:GetRenderObject()
		if RenderObj then
			return RenderObj:GetRenderTime()
		end
	end
	return nil
end

local PieceNumber = 10
local CDColor = 0xaa0d000e

local fl_tblSkillBarInfo = {{}, {}, {}}
fl_tblSkillBarInfo[1].nGridSize		= 42
fl_tblSkillBarInfo[2].nGridSize		= 40
fl_tblSkillBarInfo[3].nGridSize		= 40
fl_tblSkillBarInfo[1].nPieceNumber	= 10
fl_tblSkillBarInfo[2].nPieceNumber	= 10
fl_tblSkillBarInfo[3].nPieceNumber	= 10
fl_tblSkillBarInfo[1].bVertical		= false
fl_tblSkillBarInfo[2].bVertical		= false
fl_tblSkillBarInfo[3].bVertical		= false
fl_tblSkillBarInfo[1].sBKImagePath	= "guitex/tex/ybtx/gui/4thGUI/Main/jinenglan.tex"
fl_tblSkillBarInfo[2].sBKImagePath	= "guitex/tex/ybtx/gui/4thGUI/Main/jinenglan.tex"
fl_tblSkillBarInfo[3].sBKImagePath	= "guitex/tex/ybtx/gui/4thGUI/Main/jinenglan.tex"
fl_tblSkillBarInfo[1].tblRect		= {0, 42, 79, 121}
fl_tblSkillBarInfo[2].tblRect		= {49, 89, 80, 120}
fl_tblSkillBarInfo[3].tblRect		= {49, 89, 80, 120}


local function GetItemWndObjByType(skillBarIndex)
	local ItemWnd = nil
	if skillBarIndex == 1 then
		ItemWnd = CMainSkillItemWnd:new()
	elseif skillBarIndex == 2 then
		ItemWnd = CMasterSkillItemWnd:new()
	elseif skillBarIndex == 3 then
		ItemWnd = CAddMasterSkillItemWnd:new()
	end
	return ItemWnd
end

function CShortcutPiece:Ctor()
	self.m_State = EShortcutPieceState.eSPS_None
end

function CShortcutPiece:SetItem(parent, skillBarIndex, i)
	self:CreateFromResEx( "SkillBarItem" .. skillBarIndex, parent, true, true)
	self:ShowWnd( true )
	self:SetIsBottom(true)
	self.m_SkillBtn = self:GetDlgChild("SkillBtn")
	self.m_SkillBtn:SetWndText(g_ShortCutKeyNametbl[skillBarIndex][i])
	
	self.m_SkillBtnNum = self:GetDlgChild("SkillBtnNum")
	self.m_SkillCDBtn = self:GetDlgChild("SkillCDBtn")
	self.m_SkillCDBtn:SetBackImageColor(CDColor)
	self.m_SkillStateBtn = self:GetDlgChild("SkillStateBtn")
	self.m_SkillStateClientBtn = self:GetDlgChild("SkillStateClientBtn")
	
	self.m_SkillStateClientBtn:SetBackImageColor(CDColor)
	self.m_SkillStateClientBtn:ShowWnd(false)
	self.m_SkillStateBtn:ShowWnd(false)
	self.m_SkillCDBtn:ShowWnd(false)
	self:SetParent(parent)
	self:SetState(EShortcutPieceState.eSPS_None)
	self:SetMainSkill(skillBarIndex)
	self:SetName("")
	self:SetPos(i)
	self:SetDetectMouseOverWnd(true)
end

function CShortcutPiece:SetParent(parent)
	self.m_Parent = parent
end

function CShortcutPiece:GetParent()
	return self.m_Parent
end

function CShortcutPiece:SetPos(pos)
	self.m_Pos = pos
end

function CShortcutPiece:GetPos()
	return self.m_Pos
end

function CShortcutPiece:SetState(state)
	self.m_State = state
	--self.m_Parent:ShowWnd(state ~= EShortcutPieceState.eSPS_None)
end

function CShortcutPiece:GetState()
	return self.m_State
end

function CShortcutPiece:SetName(Name)
	self.m_Name = Name
end

function CShortcutPiece:GetName()
	return self.m_Name
end

function CShortcutPiece:SetIndex(Index)
	self.m_Index = Index
end

function CShortcutPiece:GetIndex()
	return self.m_Index
end

function CShortcutPiece:SetMainSkill(skillBarIndex)
	self.skillBarIndex = skillBarIndex
end

function CShortcutPiece:GetMainSkill(skillBarIndex)
	return self.skillBarIndex
end

function CShortcutPiece:SetCool(SkillName,Level)
	local skill =  CSkillClient_GetSkillByNameForLua( SkillName)
	self.CoolType = skill:GetCoolDownType()
	self.Level = Level
end

function CShortcutPiece:SetCoolType(CoolType)
	self.CoolType = CoolType
end

function CShortcutPiece:GetCoolType()
	return self.CoolType
end

function CShortcutPiece:GetCoolTime()
	self.CoolTime = g_MainPlayer:GetCoolDownTime( self.m_Name, self.Level ) * 1000
	if g_MainPlayer:IsSwitchEquipSkill(self.m_Name) then
		return 5*60*1000
	end
	return self.CoolTime
end

function CShortcutPiece:SetItemContext(uBigID, uIndex, uGlobalID, num)
	self.m_uBigID = uBigID
	self.m_uIndex = uIndex
	self.m_uGlobalID = uGlobalID
	self.m_Num  = num
	if g_ItemInfoMgr:IsItemBurstSkill(uBigID) then 
		local SkillName = g_ItemInfoMgr:GetItemInfo(uBigID,uIndex,"SkillName")
		if SkillName ~= "" then
			local skill =  CSkillClient_GetSkillByNameForLua(SkillName)
			self:SetCoolType(skill:GetCoolDownType())
			self:SetName(SkillName)
		else
			self:SetCoolType(nil)
			self:SetName(nil)
		end
		self:SetIndex(uIndex)
		self.Level = 1
	else
		self:SetCoolType(uIndex)
		self:SetName(uIndex)
		self:SetIndex(uIndex)		
	end	
end

function CShortcutPiece:GetItemContext()
	return self.m_uBigID, self.m_uIndex, self.m_uGlobalID, self.m_Num
end

function CShortcutPiece:SetItemNum(num)
	self.m_Num = num
end

--设置CD时间
function CShortcutPiece:SetSkillContext(fs)
	self.m_Fs = fs
	--CD时间
	self:SetCool(fs.SkillName,fs.Level)
end

function CShortcutPiece:GetSkillContext()
	return self.m_Fs
end

function CShortcutPiece:SetSkillArrayContext(fs)
	self.m_Fs = fs
end

function CShortcutPiece:GetSkillArrayContext()
	return self.m_Fs
end

function CShortcutPiece:UseItem()
	local uBigID, uIndex = self:GetItemContext()
	g_GacUseItemFromShortcut(g_GameMain,uBigID, uIndex)
end

function CShortcutPiece:UseSkill()
	if g_GameMain.m_PrepareChangeTick then
		return
	end
	local fs = self:GetSkillContext()
	--物品消耗类技能
	if(string.find(fs.SkillName, "普通攻击") ~= nil ) then
		if(g_MainPlayer.m_LockCenter.m_LockingObj == nil) then
			MsgClient(1807) -- 目标不存在
			return
		end
		local TargetID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
		if not g_MainPlayer:IsNormalAttacking() then
			if( g_MainPlayer.m_LockCenter.m_RenderChild and
				g_MainPlayer.m_LockCenter.m_LockingObj ) then
				g_MainPlayer:DestroyLockObjFx(g_MainPlayer.m_LockCenter.m_LockingObj)
			end
			g_MainPlayer:CreateLockObjFx(g_MainPlayer.m_LockCenter.m_LockingObj)
		end
		if not g_MainPlayer.m_PickOre then
			if g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
				g_MainPlayer:DoNormalAttack(TargetID, false)
			else
				g_MainPlayer:DoNormalAttack(TargetID, true)
			end
		end
	else
		local flag = true
		local SkillName = fs.SkillName
		if g_MainPlayer.m_IsTransfer ~= true then
			if g_MainPlayer.m_SkillReplaceTbl[SkillName] then
				SkillName = g_MainPlayer.m_SkillReplaceTbl[SkillName]
			end
			if (SkillName == "气魂击") then
				Gac2Gas:FightSpecialSkill(g_Conn, SkillName)
				if g_GameMain.m_QihunjiZhiyinWnd:IsShow() then
					g_GameMain.m_QihunjiZhiyinWnd:RetShowQihunjiZhiyinWnd(false)
				end
				return
			else
				flag = g_MainPlayer:FightSkill( SkillName, fs.Level )
			end
		end
		--添加快捷键记录功能, 短暂(1s)CD或流程结束后立即释放记录的技能,增加操作性
		--有流程施放过程中FightSkill是true!!		
		if flag == EDoSkillResult.eDSR_InCDTime or g_GameMain.m_skillProgress_Load:IsShow() then
			g_MainPlayer:RecordSkillName(SkillName, fs.Level)
		elseif flag == EDoSkillResult.eDSR_Success then		
			self.m_SkillStateClientBtn:ShowWnd(true)
			g_MainPlayer.szRecordSkillName = nil
			g_MainPlayer.uRecordSkillLevel =nil
		else
			g_MainPlayer.szRecordSkillName = nil
			g_MainPlayer.uRecordSkillLevel =nil
		end
	end
	
end

function CShortcutPiece:UseArray()
	local fs = self:GetSkillContext()
	Gac2Gas:UseBattleArraySkill(g_Conn, fs.SkillName)
end

--使用快捷栏上的东西
function CShortcutPiece:UseShortcut()
	if g_MainPlayer then
		g_MainPlayer:RunToMouse_UnRegisterTick()
	end
	local state = self:GetState()
	if(state == EShortcutPieceState.eSPS_Item) then
		self:UseItem()	--使用物品
	elseif(state == EShortcutPieceState.eSPS_Skill) then
		self:UseSkill()	--使用技能
	elseif(state == EShortcutPieceState.eSPS_Array) then
		self:UseArray()	--使用阵法
	end
end
function CShortcutPiece:OnRButtonClick(nFlags, x, y)
	self:UseShortcut()
end

--function CShortcutPiece:SetAutoSkill()
--	local fs = self:GetSkillContext()
--	
--	--self.m_Fs.AutoSkill = (not g_MainPlayer:IsAutoNormalAttack())
--	if(string.find(fs.SkillName, "普通攻击") ~= nil ) then
----		if(self.m_Fs.AutoSkill) then
----			--g_MainPlayer:SetAutoNormalAttack(true)
----			if(g_MainPlayer.m_LockCenter.m_LockingObj ~= nil) then
----				local TargetID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
----				if g_GameMain.m_SysSetting.m_KPmap:IsMovingByKeyboard() then
----					g_MainPlayer:DoNormalAttack(TargetID, false)
----				else
----					g_MainPlayer:DoNormalAttack(TargetID, true)
----				end
----			end
----		else
----			g_MainPlayer:SetAutoNormalAttack(false)
----		end
--	else
--		g_MainPlayer:FightSkill( self.m_Fs.SkillName, self.m_Fs.Level)
--	end
--
----	if self.m_Fs.AutoSkill then
----		MsgClient(8003,self.m_Fs.SkillName .. "自动释放开启")
----	else
----		MsgClient(8003,self.m_Fs.SkillName .. "自动释放关闭")
----	end
--end

--得到快捷栏的技能图标
function CShortcutPiece:GetSkillPieceIcon()
	local DefaultIcon = 866
	local NonFightSkill = g_Tooltips:CheckNonFightSkill(self.m_Fs.SkillName)
	local SmallIcon
	if self.m_Fs.SkillName == g_GameMain.CommonlySkillName or string.find(self.m_Fs.SkillName,"普通攻击") then
		local Info = g_GameMain.m_RoleStatus.Part[12].m_Info
		if Info == nil  then
			SmallIcon = DefaultIcon
		else
			local DynInfo = g_DynItemInfoMgr:GetDynItemInfo(Info:GetItemID())
			if DynInfo.CurDuraValue > 0 then
				if DynInfo.BaseLevel > g_MainPlayer:CppGetLevel() then
					SmallIcon = DefaultIcon
				else		
					SmallIcon = g_ItemInfoMgr:GetItemInfo(Info:GetBigID(),Info:GetIndex(),"SmallIcon")
				end
			else
				SmallIcon = DefaultIcon
			end
		end
	elseif NonFightSkill ~= nil  then
		SmallIcon = NonFightSkill("SkillSmallIcon")
	else 
		local SkillName = self.m_Fs.SkillName
		if g_MainPlayer.m_SkillReplaceTbl[SkillName] then
			SkillName = g_MainPlayer.m_SkillReplaceTbl[SkillName]
		end
		SmallIcon = SkillPart_Common(SkillName,"SkillSmallIcon")
	end	
	return SmallIcon
end

function CShortcutPiece:RegisterSelfTick(time,runedTime)
	self:UnRegisterSelfTick()
	self:UnRegisterSelfCommTick()
	local SkillCdBtn = self.m_SkillCDBtn
	SkillCdBtn:setTimeEx(time,GetRenderTime(g_MainPlayer))
	SkillCdBtn:setRunedTime(runedTime)
	local number = time - runedTime
	number = math.ceil(number/1000)*1000
	self.nRunTime = number
	local Snumber = math.floor(number/1000)
	local Mnumber = math.floor(Snumber/60)
	local Hnumber = math.floor(Mnumber/60)
	local Text,nTickNumber = "",0
	if Hnumber >= 1 then
		Text = Hnumber .. "h"
	elseif Mnumber >= 1 then
		Text = Mnumber .. "m"
	elseif number > 1 then
		Text = Snumber
	else
		return
	end
	SkillCdBtn:SetWndText(Text)
	self.m_ShortCutTick = RegisterDistortedTickByCoreObj(g_MainPlayer,"ShortCutTick",self.OnTick,1000,self)
end

function CShortcutPiece:UnRegisterSelfTick()
	if self.m_ShortCutTick ~= nil then
		if g_MainPlayer then
			UnRegisterDistortedTickByCoreObj( g_MainPlayer, self.m_ShortCutTick )
		else 
			UnRegisterDistortedTick(self.m_ShortCutTick)
		end
		self.m_ShortCutTick = nil 
		local SkillCdBtn  = self.m_SkillCDBtn
		if SkillCdBtn:IsShow() then
			SkillCdBtn:SetWndText("")
			SkillCdBtn:ShowWnd(false)
		end
	end
end

function CShortcutPiece:UnRegisterSelfCommTick()
	if self.m_ShortCutCommTick ~= nil then
		if g_MainPlayer then
			UnRegisterDistortedTickByCoreObj( g_MainPlayer, self.m_ShortCutCommTick )
		else 
			UnRegisterDistortedTick(self.m_ShortCutCommTick)
		end
		self.m_ShortCutCommTick = nil 
		local SkillCdBtn  = self.m_SkillCDBtn
		if SkillCdBtn:IsShow() then
			SkillCdBtn:SetWndText("")
			SkillCdBtn:ShowWnd(false)
		end		
	end	
end

function CShortcutPiece:OnClearCommonCD()
	self:UnRegisterSelfCommTick()
end

function CShortcutPiece:OnTick()
	local SkillCdBtn  = self.m_SkillCDBtn
	self.nRunTime = self.nRunTime - 1000
	if self.nRunTime <= 0 and self.m_ShortCutTick ~= nil then
		local Name =  self:GetName()
		local Level = self.Level
		if g_MainPlayer:CanUseSkill(Name,Level) then
			self:AddFlashInfoByName("skillcdflash")
		end
		SkillCdBtn:SetWndText("")
		SkillCdBtn:ShowWnd(false)
		if g_MainPlayer then
			UnRegisterDistortedTickByCoreObj( g_MainPlayer, self.m_ShortCutTick )
		else 
			UnRegisterDistortedTick(self.m_ShortCutTick)
		end
		self.m_ShortCutTick = nil
		if g_MainPlayer ~= nil then
			g_MainPlayer:DoRecordSkill()
		end
	elseif self.nRunTime > 0 then
		local num = SkillCdBtn:GetWndText()
		local Snumber = math.floor(self.nRunTime/1000)
		local Mnumber = math.floor(Snumber/60)
		local Hnumber = math.floor(Mnumber/60)
		local Text = "" 
		if Hnumber >= 1 then
			if ( (self.nRunTime + 1000) % (1000 * 60 * 60))  == 0  then
				local leftTime =  math.floor(self.nRunTime / (1000 * 60 * 60))
				if leftTime > 1 then
					Text = leftTime .. "h"
				elseif leftTime == 1 then 
					if self.nRunTime > 1000 * 60 * 60 then
						Text = leftTime .. "h"
					elseif self.nRunTime == 1000 * 60 * 60	then
						Text = "60m"
					end
				end
				SkillCdBtn:SetWndText(Text)
			end
		elseif Mnumber >= 1 then
			if ((self.nRunTime + 1000) % (1000 * 60 ))  == 0  then
				local leftTime =  math.floor(self.nRunTime / (1000 * 60))
				if leftTime > 1 then
					Text = leftTime .. "m"
				elseif leftTime == 1 then
					if self.nRunTime > 1000 * 60 then
						Text = leftTime .. "m"
					elseif self.nRunTime == 1000 * 60 then
						Text = "60"
					end
				end
				SkillCdBtn:SetWndText(Text)
			end
		else
			Text = math.floor(self.nRunTime /1000)
			SkillCdBtn:SetWndText(Text)
		end
	end
end

function CShortcutPiece:OnCommTick()
	local SkillCdBtn  = self.m_SkillCDBtn
	if not SkillCdBtn:IsShow() and self.m_ShortCutCommTick ~= nil then
		local Name =  self:GetName()
		local Level = self.Level
		if g_MainPlayer:CanUseSkill(Name,Level) then
			self:AddFlashInfoByName("skillcdflash")
		end
		self.m_SkillStateBtn:ShowWnd(false)
		SkillCdBtn:SetWndText("")
		if g_MainPlayer then
			UnRegisterDistortedTickByCoreObj( g_MainPlayer, self.m_ShortCutCommTick )
		else 
			UnRegisterDistortedTick(self.m_ShortCutCommTick)
		end
		self.m_ShortCutCommTick = nil
		if g_MainPlayer ~= nil then
			g_MainPlayer:DoRecordSkill()
		end
	end
end
--------------------------------------------------------------------------------------------------------------

--设置冷却时间
function CShortcut:SetPieceCoolTime(Wnd, SkillName)
	local LeftTime = g_MainPlayer:GetSkillLeftCoolDownTime(SkillName)
	if LeftTime == 0 then
		Wnd:UnRegisterSelfTick()
		return
	end
	local CoolTime = Wnd:GetCoolTime()
	local CDType = g_MainPlayer:IsInCommonCD(SkillName)
	if CDType then
		CoolTime = 1000
	end
	Wnd:RegisterSelfTick(CoolTime,CoolTime - LeftTime)
end

function CShortcut:SetQuestItemCoolTime(Wnd,ItemName)
	if not (g_MainPlayer.CoolDownTime and g_MainPlayer.CoolDownTime[ItemName]) then
		return
	end
	local function GetLeftCoolDownTime(CoolDownBeginTime)
		return (os.time() - CoolDownBeginTime) * 1000
	end
		
	local coolDownTime = g_MainPlayer.CoolDownTime[ItemName]
	local coolDownBeginTime = g_MainPlayer.CoolDownBeginTime[ItemName]
	local remCoolDownTime = GetLeftCoolDownTime(coolDownBeginTime)
	if remCoolDownTime == 0 then
		return
	end	
	
	Wnd:RegisterSelfTick(coolDownTime,remCoolDownTime)
end

function CShortcut:HideCoolWnd(Wnd)
	Wnd:UnRegisterSelfTick()
	Wnd:UnRegisterSelfCommTick()	
end

--得到包裹里面某个格子上物品的第一个ItemID
local function GetGridFirstItemID(nBigID,nSmallID)
	local Mgr = g_GameMain.m_BagSpaceMgr
	local nRoomIndex, nPos = Mgr:FindItemBySpace(g_StoreRoomIndex.PlayerBag, nBigID,nSmallID)
	if nRoomIndex then
		local Space = Mgr:GetSpace(nRoomIndex)
		assert(Space~=nil)
		local Grid = Space:GetGrid(nPos)
		local ItemId = Grid:GetItem():GetItemID(1)
		return ItemId
	end
	return 0
end

--在快捷栏窗口显示物品信息
function CShortcut:DrawItem(Wnd)
	--显示物品数量
	local uBigID, uIndex, uGlobalID, num = Wnd:GetItemContext()
	uGlobalID = GetGridFirstItemID(uBigID,uIndex)
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(uBigID,uIndex,"SmallIcon")
	local SkillName = g_ItemInfoMgr:GetItemInfo(uBigID,uIndex,"SkillName")
	local numWnd = Wnd.m_SkillBtnNum
	numWnd:SetWndText(num)
	Wnd.m_SkillStateBtn:ShowWnd(false)
	--显示Tooltips
	if g_ItemInfoMgr:IsItemBurstSkill(uBigID) and SkillName ~= "" then
		self:SetPieceCoolTime(Wnd,SkillName)
	elseif g_ItemInfoMgr:IsQuestItem(uBigID)then
		self:SetQuestItemCoolTime(Wnd,uIndex)
	else
		self:HideCoolWnd(Wnd)
	end
	if num > 0 then
		g_LoadIconFromRes( SmallIcon, Wnd.m_SkillBtn, -2, IP_ENABLE, IP_CLICKDOWN)
	else
		g_LoadGrayIconFromRes(SmallIcon, Wnd.m_SkillBtn, -2, IP_ENABLE, IP_CLICKDOWN)		
	end
	g_SetItemRichToolTips(Wnd.m_SkillBtn,uBigID,uIndex,uGlobalID)
end

--在快捷栏窗口显示技能信息
function CShortcut:DrawSkill(Wnd, fs)
	--显示数量
	local SmallIcon  = Wnd:GetSkillPieceIcon()
	local SkillName  = fs.SkillName
	if g_MainPlayer.m_SkillReplaceTbl[SkillName] then
		SkillName = g_MainPlayer.m_SkillReplaceTbl[SkillName]
	end
	
	local eCoolDownType = g_MainPlayer:GetSkillCoolDownType(SkillName)
	if(eCoolDownType ~= ESkillCoolDownType.eSCDT_FightSkill or
		eCoolDownType ~= ESkillCoolDownType.eSCDT_UnrestrainedFightSkill) then
		Wnd.m_SkillStateBtn:ShowWnd(false)
	end
	--显示Tooltips
	local tips = g_Tooltips:GetWndSkillTips( SkillName,fs.Level)
	self:SetPieceCoolTime(Wnd,SkillName)
	g_LoadIconFromRes( SmallIcon, Wnd.m_SkillBtn, -2, IP_ENABLE, IP_CLICKDOWN)		
	Wnd.m_SkillBtn:SetMouseOverDescAfter(tips)
	Wnd.m_SkillBtnNum:SetWndText("")
end

function CShortcut:DrawArraySkill(Wnd, fs)
	--显示数量
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(26, fs.SkillName,"SmallIcon")
	g_LoadIconFromRes( SmallIcon, Wnd.m_SkillBtn, -2, IP_ENABLE, IP_CLICKDOWN)				
end


--鼠标窗口和快捷栏窗口交换
function CShortcut:ExChangeIconWnd(Wnd)
	--交换图标
	g_WndMouse:ExChangeIconWnd(Wnd.m_SkillBtn)
	--交换数据
	local state, context = g_WndMouse:GetCursorState()
	--快捷栏窗口是物品
	if(Wnd:GetState() == EShortcutPieceState.eSPS_Item) then
		--设置鼠标窗口的数据
		local uBigID, uIndex, uGlobalID, num = Wnd:GetItemContext()
		local PickUpItem = {uBigID, uIndex, uGlobalID, num}
		g_WndMouse:SetCursorState(ECursorState.eCS_PickupItemShortcut, PickUpItem)
		--g_WndMouse:SetPickupItemShortcut(uBigID, uIndex, uGlobalID, num, Wnd.m_SkillBtn)
	--快捷栏窗口是技能
	elseif(Wnd:GetState() == EShortcutPieceState.eSPS_Skill) then
		--设置鼠标窗口的数据
		local fs = Wnd:GetSkillContext()
		local PickUpSkill = {fs}
		g_WndMouse:SetCursorState(ECursorState.eCS_PickupSkill, PickUpSkill)
	end
	
	--设置快捷栏窗口的数据
	if(state == ECursorState.eCS_PickupItemShortcut) then
		Wnd:SetItemContext(context[1],context[2],context[3],context[4])
	elseif(state == ECursorState.eCS_PickupItem) then
		local room, row, col, num = unpack(context)
		local uBigID, uIndex, uGlobalID = g_WndMouse:GetItemBigSmallGlobalID(room, row, col)
		local num = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(room, uBigID, uIndex)
		Wnd:SetItemContext(uBigID, uIndex, uGlobalID, num)
	elseif(state == ECursorState.eCS_PickupSkill) then
		Wnd:SetSkillContext(context[1])
		Wnd:SetName(context[1].SkillName)
		Wnd:SetIndex(nil)
		Wnd:SetItemNum(0)
	elseif(state == ECursorState.eCS_PickupBattleArrayIcon) then
		
	end	
end

--拖动快捷栏上面的图标
function CShortcut:DragShortcut(Wnd)
	assert(Wnd:GetState())
	local state = g_WndMouse:GetCursorState()
	if(Wnd:GetState() == EShortcutPieceState.eSPS_None) then
		return
	elseif(Wnd:GetState() == EShortcutPieceState.eSPS_Item) then
		--从快捷栏上拾取物品
		local uBigID, uIndex, uGlobalID, num = Wnd:GetItemContext()
		g_WndMouse:SetPickupItemShortcut(uBigID, uIndex, uGlobalID, num, Wnd.m_SkillBtn)
	elseif(Wnd:GetState() == EShortcutPieceState.eSPS_Skill) then
		local fs = Wnd:GetSkillContext()
		g_WndMouse:SetPickupSkill(fs, Wnd.m_SkillBtn)
	elseif(Wnd:GetState() == EShortcutPieceState.eSPS_Array) then
		--拾取阵法
		local InfoTbl = Wnd:GetSkillArrayContext()
		g_WndMouse:SetPickupBattleArrayIcon(InfoTbl, Wnd.m_SkillBtn)
	end
	self:DeletePiece(Wnd)
end

--向快捷栏放置图标
function CShortcut:DragDropShortcut(Wnd)
	assert(Wnd:GetState())
	local state , context = g_WndMouse:GetCursorState()
	--鼠标上的物品是从物品栏拾取来的
	if(state == ECursorState.eCS_PickupItem) then
		local room, row, col, num = unpack(context)
		if( room == g_StoreRoomIndex.PlayerDepot ) then return end
		local uBigID, uIndex, uGlobalID = g_WndMouse:GetItemBigSmallGlobalID(room, row, col)
		if( g_ItemInfoMgr:IsStaticEquip(uBigID) ) then return end
		num = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag, uBigID, uIndex)
		if(Wnd:GetState() == EShortcutPieceState.eSPS_None) then
			g_WndMouse:CopyItemIconToWnd(Wnd.m_SkillBtn)
			Wnd:SetItemContext(uBigID, uIndex, uGlobalID, num)
			g_WndMouse:ClearCursorAll()
		else
			self:ExChangeIconWnd(Wnd)
		end
		Wnd:SetState(EShortcutPieceState.eSPS_Item)
	--鼠标上的物品是从快捷栏拾取来的
	elseif(state == ECursorState.eCS_PickupItemShortcut) then
		if(Wnd:GetState() == EShortcutPieceState.eSPS_None) then
			local uBigID, uIndex, uGlobalID, num = g_WndMouse:CopyItemIconShortcutToWnd(Wnd.m_SkillBtn)
			num = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag, uBigID, uIndex)
			Wnd:SetItemContext(uBigID, uIndex, uGlobalID, num)
			g_WndMouse:ClearCursorAll()
		else
			self:ExChangeIconWnd(Wnd)
		end
		Wnd:SetState(EShortcutPieceState.eSPS_Item)
	--鼠标上的物品是从技能学习面板上拾取来的
	elseif(state == ECursorState.eCS_PickupSkill) then
		if(Wnd:GetState() == EShortcutPieceState.eSPS_None) then
			--设置CD时间
			local fs = g_WndMouse:CopySkillIconToWnd(Wnd.m_SkillBtn)
			g_MainPlayer:AddFXCache(fs.SkillName)
			Wnd:SetSkillContext(fs)
			g_WndMouse:ClearCursorAll()
			Wnd:SetName(fs.SkillName)
		else
			self:ExChangeIconWnd(Wnd)
		end
		Wnd:SetState(EShortcutPieceState.eSPS_Skill)
	--鼠标上的是阵法图标
	elseif(state == ECursorState.eCS_PickupBattleArrayIcon) then
		if(Wnd:GetState() == EShortcutPieceState.eSPS_None) then
			local fs = g_WndMouse:CopySkillIconToWnd(Wnd.m_SkillBtn)
			g_WndMouse:ClearCursorAll()
			Wnd:SetSkillArrayContext(fs)
			Wnd:SetState(EShortcutPieceState.eSPS_Array)
			Wnd:SetName(fs)
		else
			self:ExChangeIconWnd(Wnd)
		end
	end
	--保存快捷栏
	SaveShortcut(g_Conn, Wnd)
end

function InitShortCutKeyName()
	g_ShortCutKeyNametbl = {{},{},{}}
	g_TempSkillShortCutKeyNameTbl = {}
	for i = 1, 10 do
		local uCount = KeyMap_Common:GetHeight()
		for j=1, uCount do
			if( KeyMap_Common(j,"Name") == "快捷键"..i )then
				g_ShortCutKeyNametbl[1][i] = GetDisPlayNameByName(Lan_KeyMap_Common(j,"DefaultKey1"))
			elseif (KeyMap_Common(j,"Name")=="扩展技能栏左-技能"..i) then
				g_ShortCutKeyNametbl[2][i] = GetDisPlayNameByName(Lan_KeyMap_Common(j,"DefaultKey1"))
			elseif (KeyMap_Common(j,"Name")=="扩展技能栏右-技能"..i) then
				g_ShortCutKeyNametbl[3][i] = GetDisPlayNameByName(Lan_KeyMap_Common(j,"DefaultKey1"))
			end
		end
	end
	for i = 1,6 do
		local uCount = KeyMap_Common:GetHeight()
		for j = 1,uCount do
			if  KeyMap_Common(j,"Name") == "临时技" .. i then
				g_TempSkillShortCutKeyNameTbl[i] = GetDisPlayNameByName(Lan_KeyMap_Common(j,"DefaultKey1"))
			end
		end
	end
end

function CShortcut:InitData(nType)
	self.m_nGridSize			= fl_tblSkillBarInfo[nType].nGridSize
	self.m_nPieceNumber			= fl_tblSkillBarInfo[nType].nPieceNumber
	self.m_bVertical			= fl_tblSkillBarInfo[nType].bVertical
	self.m_bkImagePath			= fl_tblSkillBarInfo[nType].sBKImagePath
	
	self.m_bkImageRect			= CFRect:new()
	self.m_bkImageRect.left		= fl_tblSkillBarInfo[nType].tblRect[1]
	self.m_bkImageRect.right	= fl_tblSkillBarInfo[nType].tblRect[2]
	self.m_bkImageRect.top		= fl_tblSkillBarInfo[nType].tblRect[3]
	self.m_bkImageRect.bottom	= fl_tblSkillBarInfo[nType].tblRect[4]
end

--初始化快捷栏的格子，参数num表示格子的数目，参数bVertical为true表示竖直排列，为false表示水平排列
--参数bMultiPage为true表示是多页快捷栏，为false表示不是多页快捷栏
--参数bMainSkill为true表示是主技能快捷栏,为false表示是副技能快捷栏
function CShortcut:Init(nSkillbarIndex)
	self:InitData(nSkillbarIndex)
	self.m_tblShortcutPiece = {}
	if(self.m_bVertical) then
		self:InsertColumn(0, self.m_nGridSize)
		for i=1, self.m_nPieceNumber do
			self:InsertItem(i-1, self.m_nGridSize)
			local itemWnd	= GetItemWndObjByType(nSkillbarIndex)
			local parentWnd	= self:GetSubItem(i-1, 0)
			self:SetBackImage(parentWnd)
			itemWnd:SetItem(parentWnd, nSkillbarIndex, i)
			table.insert(self.m_tblShortcutPiece, itemWnd)
		end
	else
		for i = 1, self.m_nPieceNumber do self:InsertColumn(i-1, self.m_nGridSize) end
		self:InsertItem(0, self.m_nGridSize)
		for i = 1, self.m_nPieceNumber do
			local itemWnd	= GetItemWndObjByType(nSkillbarIndex)
			local parentWnd	= self:GetSubItem(0, i-1)
			self:SetBackImage(parentWnd)
			itemWnd:SetItem(parentWnd, nSkillbarIndex, i)
			table.insert(self.m_tblShortcutPiece, itemWnd)
		end
	end
end

function CShortcut:SetBackImage(wnd)
	g_DynImageMgr:AddImageNeedCut(wnd, self.m_bkImagePath, IP_ENABLE, self.m_bkImageRect, CFPos:new())
end

function CShortcut:UpdateItemCount(uBigID, uIndex, num)
	assert(uBigID ,"uIndex:" .. uIndex .. "num:" .. num)
	assert(uIndex ,"uBigID:" .. uBigID .. "num:" .. num)
	assert(num,"uBigID:" .. uBigID .. "uIndex:" .. uIndex)
	for i=1, #(self.m_tblShortcutPiece) do
		local Piece = self.m_tblShortcutPiece[i]
		local bid, sid, gid = Piece:GetItemContext()
		if(bid == uBigID and sid == uIndex) then
			Piece:SetItemContext(uBigID, uIndex, gid, num)
			Piece.m_SkillBtnNum:SetWndText(num)
		end
	end
end

--更新快捷栏的tooltips信息
function CShortcut:UpdateTips()
	for i=1, #(self.m_tblShortcutPiece) do
		local Piece = self.m_tblShortcutPiece[i]
		local DrawPiece = Piece.m_SkillBtn
		local state = Piece:GetState()
		if(state == EShortcutPieceState.eSPS_Item) then
			--该格子上放的是物品
			local uBigID, uIndex, uGlobalID, num = Piece:GetItemContext()
			uGlobalID = GetGridFirstItemID(uBigID,uIndex)
			g_SetItemRichToolTips(DrawPiece,uBigID, uIndex, uGlobalID)
		elseif(state == EShortcutPieceState.eSPS_Skill) then
			--该格子上放的是技能
			local fs = Piece:GetSkillContext()
			local tips = g_Tooltips:GetWndSkillTips( fs.SkillName,fs.Level)
			DrawPiece:SetMouseOverDescAfter(tips)
		end
	end
end

--获得快捷栏格子
function CShortcut:GetTblShortcutPiece()
	return self.m_tblShortcutPiece
end

--删除快捷栏上Wnd上的图标
function CShortcut:DeletePiece(Wnd)
	local pos = Wnd:GetPos()
	local skillBarIndex = Wnd:GetMainSkill()
	Wnd.m_SkillBtn:SetMouseOverDescAfter("")
	Wnd.m_SkillBtnNum:SetWndText("")
	Wnd.m_SkillStateBtn:ShowWnd(false)
	Wnd:SetIndex(nil)
	g_WndMouse:ClearIconWnd( Wnd.m_SkillBtn)
	Wnd:UnRegisterSelfTick()
	Wnd:UnRegisterSelfCommTick()
	
	local type, arg1, arg2, arg3 = GetPieceInfo(Wnd)
	if type == EShortcutPieceState.eSPS_Skill then
		g_MainPlayer:DelLearnSkill(arg1, arg2)
		g_MainPlayer:RemoveFXCache(arg1)
	end
	if g_GameMain.m_MainSkillsToolBar.m_CurrentPage > 1 then
		pos = (g_GameMain.m_MainSkillsToolBar.m_CurrentPage - 1) * 10 + pos	
	end
	g_GameMain.m_MainSkillsToolBar.m_PieceTbl[skillBarIndex][pos] = nil
	Wnd:SetState(EShortcutPieceState.eSPS_None)
	Gac2Gas:SaveShortCut(g_Conn, pos, EShortcutPieceState.eSPS_None, "11", 1, skillBarIndex) --位置、格子的物品属性、技能名称、技能的等级、是否是主技能栏
	Gac2Gas:SaveShortCutEnd(g_Conn)
end

----更新快捷栏上普通攻击的攻击状态
function CShortcut:UpdateCommonlySkill(bool)
	local tblShortcutPiece = self:GetTblShortcutPiece()
	for i = 1,table.maxn(tblShortcutPiece) do
		if tblShortcutPiece[i]:GetState() == EShortcutPieceState.eSPS_Skill then
			local fs = tblShortcutPiece[i]:GetSkillContext()
			if fs.AutoSkill ~= nil then
				fs.AutoSkill = bool
				tblShortcutPiece[i]:SetSkillContext(fs)
			end
		end
	end
end

--注销浮动快捷栏Tick
function CShortcut:UnRegisterToolBar()
	local m_tblShortcutPiece = self:GetTblShortcutPiece()
	for i = 1 ,PieceNumber do 
		local Piece = m_tblShortcutPiece[i]
		Piece:UnRegisterSelfTick()
		Piece:UnRegisterSelfCommTick()
	end
end

--更新格子显示
function CShortcut:LoadShortcutPieces()
	local tblShortcutPiece = self:GetTblShortcutPiece()
	for i = 1, 10 do
		local Piece = tblShortcutPiece[i]
		local DrawPiece = Piece.m_SkillBtn
		local state = Piece:GetState()
		if(state == EShortcutPieceState.eSPS_Item) then
			--该格子上放的是物品
			self:DrawItem(Piece)
		elseif(state == EShortcutPieceState.eSPS_Skill) then
			--该格子上放的是技能
			local fs = Piece:GetSkillContext()
			self:DrawSkill(Piece,fs)
		elseif(state == EShortcutPieceState.eSPS_Array) then
			--该格子上放的是阵法
			local fs = Piece:GetSkillArrayContext()
			self:DrawArraySkill(Piece, fs)
		elseif(state == EShortcutPieceState.eSPS_None) then
			--该格子是空的
			Piece.m_SkillBtn:SetMouseOverDescAfter("")
			g_WndMouse:ClearIconWnd(DrawPiece)
			local numWnd = Piece.m_SkillBtnNum
			numWnd:SetWndText("")
		end
	end
end


function CShortcut:GetItemWndBySkillBtn(Child)
	for i=1, PieceNumber do
		if self.m_tblShortcutPiece[i].m_SkillBtn == Child then
			return self.m_tblShortcutPiece[i]
		end
	end
end

function CShortcut:OnEvent(Child, uMsgID, uParam1, uParam2, parentWnd)
	local Piece =	self:GetItemWndBySkillBtn(Child)
	if Piece == nil then 
		return 
	end
	
	local state = g_WndMouse:GetCursorState()
	if (uMsgID == BUTTON_CLICKUP or uMsgID == BUTTON_LCLICK) and (state ~= ECursorState.eCS_Normal) then
		if state == ECursorState.eCS_PickupItem 
			or state == ECursorState.eCS_PickupItemShortcut 
			or state == ECursorState.eCS_PickupSkill 
			or state == ECursorState.eCS_PickupBattleArrayIcon then 
			--向快捷栏放置图标
			self:DragDropShortcut(Piece)
			self:LoadShortcutPieces()
		end
	elseif (uMsgID == BUTTON_LCLICK or uMsgID == BUTTON_RCLICK) and (state == ECursorState.eCS_Normal) then
		--使用快捷栏上的东西(物品或技能)
		Piece:UseShortcut()
	elseif uMsgID == BUTTON_DRAG then
		if ( g_WndMouse:GetCursorState() == ECursorState.eCS_Normal ) then
			local bIsLock = g_UISettingMgr:GetOnePropState("锁定快捷栏")
			if bIsLock then
				return
			end
			parentWnd.ChoosePiece = Piece
			self:DragShortcut(Piece)
		end
	end
end

--设置快捷栏中的所有技能CD时间为零  o(∩_∩)o...
function CShortcut:setZeroCoolTimer()
	for i = 1,PieceNumber do
		if self.m_tblShortcutPiece[i]:GetState() == EShortcutPieceState.eSPS_Skill then
			if ( self.m_tblShortcutPiece[i].m_SkillCDBtn:IsShow()) then
				local fs = self.m_tblShortcutPiece[i]:GetSkillContext()
				local eCoolDownType = g_MainPlayer:GetSkillCoolDownType(fs.SkillName)
				if eCoolDownType == ESkillCoolDownType.eSCDT_FightSkill 
					or eCoolDownType == ESkillCoolDownType.eSCDT_NoComCDFightSkill
					or eCoolDownType == ESkillCoolDownType.eSCDT_UnrestrainedFightSkill then
					self.m_tblShortcutPiece[i]:UnRegisterSelfTick()
					self.m_tblShortcutPiece[i]:UnRegisterSelfCommTick()
					self.m_tblShortcutPiece[i].m_SkillCDBtn:setTimeEx(0,GetRenderTime(g_MainPlayer))
				end
			end
		end
	end
end
