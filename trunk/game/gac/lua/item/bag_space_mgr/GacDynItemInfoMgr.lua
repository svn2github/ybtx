CDynItemInfoMgr = class()

function CDynItemInfoMgr:SetBigID(tbl,nBigID,nIndex,nBindingType)
	tbl.m_nBigID = nBigID
	tbl.m_nIndex = nIndex
	tbl.BindingType = nBindingType
end

function CDynItemInfoMgr:CreateItemInfo(nItemID,nBigID,nIndex,nBindingType)
	local tbl = self:GetDynItemInfo(nItemID)
	tbl = tbl or {}
	self:SetBigID(tbl,nBigID,nIndex,nBindingType)
	self.m_tblGlobalItemInfo[nItemID] = tbl
end

function CDynItemInfoMgr:DelItemInfo(nItemID)
	self.m_tblGlobalItemInfo[nItemID] = nil
end

function CDynItemInfoMgr:GetDynItemInfo(nItemID)
	return self.m_tblGlobalItemInfo[nItemID]
end

function CDynItemInfoMgr:SetItemIDToTable(nItemID)
	table.insert(self.m_tblItemIDs,nItemID)
end

function CDynItemInfoMgr:GetTblItemID()
	return self.m_tblItemIDs or {}
end

function CDynItemInfoMgr:InitTblItemID()
	self.m_tblItemIDs = {}
end

function CDynItemInfoMgr:SetItemBindingType(nItemID,nBindingType)
	local tbl = self.m_tblGlobalItemInfo[nItemID]
	tbl = tbl or {}
	tbl.BindingType = nBindingType
	self.m_tblGlobalItemInfo[nItemID] = tbl
end

function CDynItemInfoMgr.Init()
	local DynItemInfoMgr = CDynItemInfoMgr:new()
	DynItemInfoMgr.m_tblGlobalItemInfo = {}
	DynItemInfoMgr.m_tblItemIDs = {}
	DynItemInfoMgr.now_room_index = nil
	return DynItemInfoMgr
end

--物品制作者
function CDynItemInfoMgr:AddItemMakerName(item_id,maker_name)
	local tbl = self.m_tblGlobalItemInfo[item_id]
	tbl = tbl or {}
	tbl.ItemMaker = maker_name
	self.m_tblGlobalItemInfo[item_id] = tbl
end

--分解熟练度
function CDynItemInfoMgr:AddBreakItemExp(nAddExp)
	local nTotalExp = self.m_BreakItemExp or 0
	self.m_BreakItemExp = nTotalExp + nAddExp
end

--分解熟练度
function CDynItemInfoMgr:InitBreakItemExp()
	self.m_BreakItemExp = 0
end

--剩余时间
function CDynItemInfoMgr:AddItemTimeInfo(item_id,nLeftTime)
	local tbl = self.m_tblGlobalItemInfo[item_id]
	tbl = tbl or {}
	tbl.m_nLifeTime = nLeftTime
	self.m_tblGlobalItemInfo[item_id] = tbl
end
---------------装备武器信息-----------------------------

function CDynItemInfoMgr:CreateWeaponInfo(nItemId, ViewName, BindingType, BaseLevel, CurLevel, DPS,
											AttackSpeed,DPSFloorRate, intensifyQuality
											, maxDuraValue, curDuraValue,IntensifyRoot)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	tbl = tbl or {}
	tbl.ViewName = ViewName
	tbl.BindingType = BindingType
	tbl.BaseLevel = BaseLevel
	tbl.CurLevel = CurLevel
	tbl.DPS = string.format("%.1f",DPS)
	tbl.AttackSpeed = string.format("%.1f",AttackSpeed)
	tbl.DPSFloorRate = DPSFloorRate
	tbl.IntensifyQuality = intensifyQuality
	tbl.MaxDuraValue = tonumber(maxDuraValue)
	tbl.CurDuraValue = tonumber(curDuraValue)
	tbl.IntensifyRoot = IntensifyRoot
	if tbl.CurDuraValue > 0 and tbl.CurDuraValue < 1 then
	   tbl.CurDuraValue  = 1 
	end
	self.m_tblGlobalItemInfo[nItemId] = tbl
end

------------装备护甲信息------------------------

function CDynItemInfoMgr:CreateArmorInfo(nItemId, ViewName, BindingType, BaseLevel, CurLevel, OutputAttr1, OutputAttr2, OutputAttr3,StaticProValue,
										 intensifyQuality
										 , maxDuraValue, curDuraValue,IntensifyRoot)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	tbl = tbl or {}
	tbl.ViewName = ViewName
	tbl.BindingType = BindingType
	tbl.BaseLevel = BaseLevel
	tbl.CurLevel = CurLevel
	tbl.OutputAttr1 = OutputAttr1
	tbl.OutputAttr2 = OutputAttr2
	tbl.OutputAttr3 = OutputAttr3
	tbl.StaticProValue = StaticProValue
	tbl.IntensifyQuality = intensifyQuality	
    tbl.MaxDuraValue = tonumber(maxDuraValue)
	tbl.CurDuraValue = tonumber(curDuraValue)
	tbl.IntensifyRoot = IntensifyRoot
    if tbl.CurDuraValue > 0 and tbl.CurDuraValue < 1 then
	   tbl.CurDuraValue  = 1 
	end
	self.m_tblGlobalItemInfo[nItemId] = tbl
end

--------------装备指环信息--------------------------

function CDynItemInfoMgr:CreateRingInfo( nItemId, ViewName, BindingType, BaseLevel, CurLevel, DPS,StaticProValue, 
										 intensifyQuality
										 , maxDuraValue, curDuraValue,IntensifyRoot)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	tbl = tbl or {}
	tbl.ViewName = ViewName
	tbl.BindingType = BindingType
	tbl.BaseLevel = BaseLevel
	tbl.CurLevel = CurLevel
	tbl.DPS = string.format("%.1f",DPS)
	tbl.StaticProValue = string.format("%.1f",StaticProValue)
	tbl.IntensifyQuality = intensifyQuality
	tbl.MaxDuraValue = tonumber(maxDuraValue)
	tbl.CurDuraValue = tonumber(curDuraValue)
	tbl.IntensifyRoot = IntensifyRoot
    if tbl.CurDuraValue > 0 and tbl.CurDuraValue < 1 then
	   tbl.CurDuraValue  = 1 
	end
	self.m_tblGlobalItemInfo[nItemId] = tbl
end

-----------------------盾牌-----------------------------------
function CDynItemInfoMgr:CreateShieldInfo(nItemId, ViewName, BindingType, BaseLevel, CurLevel,
											intensifyQuality,
											uOutputAttrValue1,uOutputAttrValue2,uOutputAttrValue3,uOutputAttrValue4
											, maxDuraValue, curDuraValue,IntensifyRoot)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	tbl = tbl or {}
	tbl.ViewName = ViewName
	tbl.BindingType = BindingType
	tbl.BaseLevel = BaseLevel
	tbl.CurLevel    = CurLevel
	tbl.IntensifyQuality = intensifyQuality
	tbl.OutputAttrValue1 = uOutputAttrValue1
	tbl.OutputAttrValue2 = uOutputAttrValue2
	tbl.OutputAttrValue3 = uOutputAttrValue3
	tbl.OutputAttrValue4 = uOutputAttrValue4
	tbl.MaxDuraValue = tonumber(maxDuraValue)
	tbl.CurDuraValue = tonumber(curDuraValue)
	tbl.IntensifyRoot = IntensifyRoot
    if tbl.CurDuraValue > 0 and tbl.CurDuraValue < 1 then
	   tbl.CurDuraValue  = 1 
	end
	self.m_tblGlobalItemInfo[nItemId] = tbl
end

----------------------装备强化信息-------------------------------
function CDynItemInfoMgr:CreateEquipIntensifyInfo(nItemId, ItensifySoul, AddAttr1, AddAttrValue1, AddAttr2, AddAttrValue2, IntensifyPhase, SuitName, intensifyTimes, intensifyBackTimes, intenAddTalentIndex)
    local tbl = self.m_tblGlobalItemInfo[nItemId]
    if tbl ~= nil then
    	tbl.ItensifySoul = ItensifySoul
    	tbl.AddAttr1 = AddAttr1
    	tbl.AddAttrValue1 = AddAttrValue1
    	tbl.AddAttr2 = AddAttr2
    	tbl.AddAttrValue2 = AddAttrValue2
    	tbl.IntensifyPhase = IntensifyPhase
    	tbl.SuitName = SuitName
    	tbl.IntensifyTimes  =intensifyTimes
    	tbl.IntensifyBackTimes = intensifyBackTimes
        tbl.IntensifyAddTalent = intenAddTalentIndex
    end
end

-----------------------装备强化附加属性----------------------------
function CDynItemInfoMgr:CreateEquipAddAttrInfo( nItemId,sAddAttr4, uAddAttr4Value, sAddAttr5, uAddAttr5Value, sAddAttr6, uAddAttr6Value, sAddAttr7, uAddAttr7Value, sAddAttr8, uAddAttr8Value, sAddAttr9, uAddAttr9Value)
	if self.m_tblGlobalItemInfo[nItemId] ~= nil then
		self.m_tblGlobalItemInfo[nItemId].AddAttr3 = sAddAttr4
		self.m_tblGlobalItemInfo[nItemId].AddAttrValue3 = uAddAttr4Value
		self.m_tblGlobalItemInfo[nItemId].AddAttr4 = sAddAttr5
		self.m_tblGlobalItemInfo[nItemId].AddAttrValue4 = uAddAttr5Value
		self.m_tblGlobalItemInfo[nItemId].AddAttr5 = sAddAttr6
		self.m_tblGlobalItemInfo[nItemId].AddAttrValue5 = uAddAttr6Value
		self.m_tblGlobalItemInfo[nItemId].AddAttr6 = sAddAttr7
		self.m_tblGlobalItemInfo[nItemId].AddAttrValue6 = uAddAttr7Value
		self.m_tblGlobalItemInfo[nItemId].AddAttr7 = sAddAttr8
		self.m_tblGlobalItemInfo[nItemId].AddAttrValue7 = uAddAttr8Value		
		self.m_tblGlobalItemInfo[nItemId].AddAttr8 = sAddAttr9
		self.m_tblGlobalItemInfo[nItemId].AddAttrValue8 = uAddAttr9Value		
	end
end

--装备进阶属性
function CDynItemInfoMgr:CreateEquipAdvanceInfo(nItemId,CurAdvancePhase,MaxAdvancePhase,AdvanceSkillPiece1,AdvanceSkillPiece2,AdvanceSkillPiece3,AdvanceSkillPiece4,ChoosedAdvanceSkillPiece,JingLingType,AdvanceSoul,AdvancedTimes, AdvancedAttrValue1,AdvancedAttrValue2,AdvancedAttrValue3,AdvancedAttrValue4,AdvancedRoot,AdvancedAttrName1,AdvancedAttrName2,AdvancedAttrName3,AdvancedAttrName4)
	if self.m_tblGlobalItemInfo[nItemId] ~= nil then
		self.m_tblGlobalItemInfo[nItemId].CurAdvancePhase = CurAdvancePhase
		self.m_tblGlobalItemInfo[nItemId].MaxAdvancePhase = MaxAdvancePhase
		self.m_tblGlobalItemInfo[nItemId].AdvanceSkillPiece1 = AdvanceSkillPiece1
		self.m_tblGlobalItemInfo[nItemId].AdvanceSkillPiece2 = AdvanceSkillPiece2
		self.m_tblGlobalItemInfo[nItemId].AdvanceSkillPiece3 = AdvanceSkillPiece3
		self.m_tblGlobalItemInfo[nItemId].AdvanceSkillPiece4 = AdvanceSkillPiece4
		self.m_tblGlobalItemInfo[nItemId].ChoosedAdvanceSkillPiece = ChoosedAdvanceSkillPiece
		self.m_tblGlobalItemInfo[nItemId].JingLingType = JingLingType
		self.m_tblGlobalItemInfo[nItemId].AdvanceSoul = AdvanceSoul
		self.m_tblGlobalItemInfo[nItemId].AdvancedTimes = AdvancedTimes
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrValue1 = AdvancedAttrValue1
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrValue2 = AdvancedAttrValue2
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrValue3 = AdvancedAttrValue3
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrValue4 = AdvancedAttrValue4 
		self.m_tblGlobalItemInfo[nItemId].AdvancedRoot = AdvancedRoot
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrName1 = AdvancedAttrName1
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrName2 = AdvancedAttrName2
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrName3 = AdvancedAttrName3
		self.m_tblGlobalItemInfo[nItemId].AdvancedAttrName4 = AdvancedAttrName4
	end
end


function CDynItemInfoMgr:CreateEquipEnactmentInfo(nItemId,ArmorPieceAttr1,ArmorPiece1AddTimes,ArmorPieceAttr2,ArmorPiece2AddTimes,
	ArmorPieceAttr3,ArmorPiece3AddTimes,ArmorPieceAttr4,ArmorPiece4AddTimes)
  local tbl = self.m_tblGlobalItemInfo[nItemId]
  if tbl ~= nil then
  	tbl.ArmorPieceAttr1 = ArmorPieceAttr1
  	tbl.ArmorPiece1AddTimes = ArmorPiece1AddTimes
  	tbl.ArmorPieceAttr2 = ArmorPieceAttr2
  	tbl.ArmorPiece2AddTimes = ArmorPiece2AddTimes
  	tbl.ArmorPieceAttr3 = ArmorPieceAttr3
  	tbl.ArmorPiece3AddTimes = ArmorPiece3AddTimes
  	tbl.ArmorPieceAttr4 = ArmorPieceAttr4
  	tbl.ArmorPiece4AddTimes = ArmorPiece4AddTimes
  end
end
--------------阵法书信息信息--------------------------
function CDynItemInfoMgr:CreateBattleArrayBookInfo(nItemId, nPos1, nPos2, nPos3, nPos4, nPos5)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	if(-5 == nPos1+nPos2+nPos3+nPos4+nPos5) then --返回五个-1
		tbl = {}
	else
		tbl = tbl or {}
		tbl.Pos1 = nPos1
		tbl.Pos2 = nPos2
		tbl.Pos3 = nPos3
		tbl.Pos4 = nPos4
		tbl.Pos5 = nPos5
	end
	self.m_tblGlobalItemInfo[nItemId] = tbl
end

-----------------------邮件文本附件----------------------
function CDynItemInfoMgr:CreateMailTextAttachmentInfo(nItemId,sMailTitle,sSenderName)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	
	local info = tbl or {}
	
	info.sMailTitle = sMailTitle
	info.sSenderName = sSenderName
	
	self.m_tblGlobalItemInfo[nItemId] = info
end

function CDynItemInfoMgr:CreateEquipSuperaddInfo(equipID, curSuperaddPhase, superaddRate)
    local tbl = self.m_tblGlobalItemInfo[equipID]   
    local info = tbl or {}
    info.SuperaddRate = superaddRate/100
    info.CurSuperaddPhase = curSuperaddPhase
    self.m_tblGlobalItemInfo[equipID] = info
end

-------------------魂珠-----------------------------
function CDynItemInfoMgr:CreateSoulPearlItemInfo(nItemId,nSoulNum)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	
	local info = tbl or {}
	
	info.nSoulNum = nSoulNum
	
	self.m_tblGlobalItemInfo[nItemId] = info
end

-------------------矿藏图---------------------------
function CDynItemInfoMgr:CreateOreMapItemInfo(nItemId, sceneName)

	local tbl = self.m_tblGlobalItemInfo[nItemId]
	
	local info = tbl or {}
	
	info.sceneName = sceneName
	
	self.m_tblGlobalItemInfo[nItemId] = info
end
-------------------采矿工具-------------------------
function CDynItemInfoMgr:CreatePickOreItemInfo(nItemId, MaxDura, CurDura)

	local tbl = self.m_tblGlobalItemInfo[nItemId]
	
	local info = tbl or {}
	
	info.MaxDura = MaxDura
	info.CurDura = CurDura
	
	self.m_tblGlobalItemInfo[nItemId] = info
end

function CDynItemInfoMgr:CreateBoxItemInfo(nItemId, openedFlag)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	
	local info = tbl or {}
	
	info.OpenedFlag = openedFlag
	
	self.m_tblGlobalItemInfo[nItemId] = info
end
------------------------------------------------------
function CDynItemInfoMgr:CreateExpOrSoulBottleInfo(nItemId,uState,uStorageNum)
	local tbl = self.m_tblGlobalItemInfo[nItemId]
	local info = tbl or {}
	info.uState = uState
	info.uStorageNum = uStorageNum
	
	self.m_tblGlobalItemInfo[nItemId] = info
end