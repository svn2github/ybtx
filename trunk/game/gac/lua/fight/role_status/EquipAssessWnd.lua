cfg_load "equip/EquipAssess_Common"
cfg_load "fb_game/AreaFb_Common"
lan_load "equip/Lan_PropertyName_Common"

CEquipAssessWnd  = class( SQRDialog )
local ColorTbl = {}
ColorTbl["装备评定系统给定"] = g_ColorMgr:GetColor("装备评定系统给定")
ColorTbl["装备评定满足颜色"] = g_ColorMgr:GetColor("装备评定满足颜色")
ColorTbl["装备评定不足颜色"] = g_ColorMgr:GetColor("装备评定不足颜色")
local FightingEvaluationTbl = nil
local PlayerInfo = nil
local ResistanceValue = {}
function CEquipAssessWnd:Ctor(Parent,DifficultyLevel,NpcLevel,AreaFbName)
	self:CreateFromRes("EquipAssessWnd", Parent)
	self.m_TitleRichText = self:GetDlgChild("TitleRichText")
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_CloseBtn:ShowWnd(false)
	self.m_IconStaticTbl = {}
	self.m_AssessRichTextTbl = {}
	self.m_NameStaticTbl = {}
	self.m_LevelStaticTbl = {}
	for i = 1, 5 do 
		local IconStatic = self:GetDlgChild("IconStatic" .. i)
		table.insert(self.m_IconStaticTbl,IconStatic)
		local AssessRichText = self:GetDlgChild("AssessRichText" .. i)
		table.insert(self.m_AssessRichTextTbl,AssessRichText)
		local NameStatic = self:GetDlgChild("NameStatic" .. i )
		table.insert(self.m_NameStaticTbl,NameStatic)
		local LevelStatic = self:GetDlgChild("LevelStatic" .. i)
		table.insert(self.m_LevelStaticTbl,LevelStatic)
	end
	
	if AreaFbName then
		self:SetWndResistanceInfo(AreaFbName)
		if ResistanceValue then
			for j = 1,#(ResistanceValue) do 
				self:SetWndResistanceItemInfo(unpack(ResistanceValue[j]))
			end
		end
		ResistanceValue = {}
	elseif DifficultyLevel and NpcLevel then
		if FightingEvaluationTbl == nil then
			return
		end
		self:SetWndEquipAssessInfo(DifficultyLevel,NpcLevel)	
	end
	FightingEvaluationTbl = nil
	PlayerInfo = nil
	
	local Rt = CFRect:new()	
	Parent:GetLogicRect(Rt)
	Rt.bottom = Rt.top - 2
	Rt.top =  Rt.bottom - self:GetWndOrgHeight()
	Rt.left = (Rt.right + Rt.left)/2 - self:GetWndOrgWidth()/2
	Rt.right = Rt.left + self:GetWndOrgWidth()
	self:SetLogicRect(Rt)
end


function CEquipAssessWnd:SetEquipAssessInfo(index)
	local IconStatic = self.m_IconStaticTbl[index]
	local PlayerID = IconStatic.m_PlayerID
	if PlayerID  then
		local AssessRichText = self.m_AssessRichTextTbl[index]
		local EquipAssess = FightingEvaluationTbl[PlayerID]
		if EquipAssess >= self.m_EquipToTalPoints then
			EquipAssess = ColorTbl["装备评定满足颜色"] .. EquipAssess
		else
			EquipAssess = ColorTbl["装备评定不足颜色"] .. EquipAssess
		end
		AssessRichText:SetWndText(GetStaticTextClient(27002,EquipAssess))	
	end
end

function CEquipAssessWnd:SetBtnInfo(index)
	local IconStatic = self.m_IconStaticTbl[index]
	local AssessRichText = self.m_AssessRichTextTbl[index]
	local NameStatic = self.m_NameStaticTbl[index]
	local LevelStatic = self.m_LevelStaticTbl[index]
	if PlayerInfo and PlayerInfo[index] then
		IconStatic:ShowWnd(true)
		AssessRichText:ShowWnd(true)
		NameStatic:ShowWnd(true)
		LevelStatic:ShowWnd(true)
		local PlayerID,PlayerName,PlayerClass,PlayerLevel,PlayerCamp = unpack(PlayerInfo[index])
		local sEnableImg, sDisableImg, sCamp, sClass = g_DynImageMgr:GetClassPic(PlayerCamp, PlayerClass)
		g_DynImageMgr:AddImageByIP(IconStatic, sEnableImg, IP_ENABLE)
		NameStatic:SetWndText(PlayerName) 
		LevelStatic:SetWndText( GetStaticTextClient(8322,PlayerLevel))
		IconStatic.m_PlayerID = PlayerID
	else
		IconStatic:ShowWnd(false)
		AssessRichText:ShowWnd(false)
		NameStatic:ShowWnd(false)
		LevelStatic:ShowWnd(false)
	end
end

local PartTbl = {	
					[EEquipPart.eNecklace] 			= ToolTipsText(307),
					[EEquipPart.eAccouterment]		= ToolTipsText(308),
					[EEquipPart.eBack]				= ToolTipsText(309),
					[EEquipPart.eRingLeft]			= ToolTipsText(310),
					[EEquipPart.eRingRight] 		= ToolTipsText(311),
					[EEquipPart.eAssociateWeapon]   = ToolTipsText(312),
					[EEquipPart.eHead]				= ToolTipsText(313),
					[EEquipPart.eShoulder]			= ToolTipsText(314),
					[EEquipPart.eWear]				= ToolTipsText(315),
					[EEquipPart.eHand]				= ToolTipsText(316),
					[EEquipPart.eShoe]				= ToolTipsText(317),
					[EEquipPart.eWeapon]			= ToolTipsText(318),
					[EEquipPart.eSash]				= ToolTipsText(319)
				}

function CEquipAssessWnd.GetEquipAssessTipsStr(DifficultyLevel,NpcLevel,DataWnd)
	local EquipAssessValueTbl =  GetPlayerEquipAssessValueTbl(DataWnd)
	local RowInfo = EquipAssess_Common(tostring(DifficultyLevel),tostring(NpcLevel))
	local EquipPointsTbl = {
								[EEquipPart.eWeapon] 			= RowInfo("Weapon"),
								[EEquipPart.eAssociateWeapon] 	= RowInfo("AssociateWeapon"),
								[EEquipPart.eHead]				= RowInfo("Head"),
								[EEquipPart.eWear] 				= RowInfo("Wear"),
								[EEquipPart.eSash]				= RowInfo("Sash"),
								[EEquipPart.eShoe]				= RowInfo("Shoe"),
								[EEquipPart.eRingLeft] 			= RowInfo("RingLeft"),
								[EEquipPart.eRingRight]         = RowInfo("RingRight"),
								[EEquipPart.eNecklace] 			= RowInfo("Necklace"),
								[EEquipPart.eAccouterment] 		= RowInfo("Accouterment"),
								[EEquipPart.eShoulder] 			= RowInfo("Shoulder"),
								[EEquipPart.eHand] 				= RowInfo("Hand"),
								[EEquipPart.eBack]				= RowInfo("Back"),
							}
	
	local TipStr = ""			
	for Part,Value in pairs(EquipPointsTbl)	do
		if EquipAssessValueTbl[Part] then 
			if EquipAssessValueTbl[Part] < tonumber(Value) then
				if TipStr == "" then
					TipStr = ToolTipsText(331) .. "#r" ..  ToolTipsText(330) .. "#r"
				else
					TipStr = TipStr .. "#r"
				end
				TipStr = TipStr .. PartTbl[Part] .. ColorTbl["装备评定不足颜色"] .. "(" .. EquipAssessValueTbl[Part] .. ")#R" .. ColorTbl["装备评定系统给定"] .. Value .."#W"
			end
		end
	end
	return TipStr
end

function CEquipAssessWnd:SetBtnTips(DifficultyLevel,NpcLevel)
	local MainPlayerID = g_GameMain.m_PlayerInfoTbl.m_PlayerCharID
	for i = 1,5 do
		if self.m_IconStaticTbl[i].m_PlayerID == MainPlayerID then
			local TipStr = CEquipAssessWnd.GetEquipAssessTipsStr(DifficultyLevel,NpcLevel,g_GameMain.m_RoleStatus)
			self.m_AssessRichTextTbl[i]:SetMouseOverDescAfter(TipStr)
			break
		end
	end
end

--显示装备评分面板
function CEquipAssessWnd:SetWndEquipAssessInfo(DifficultyLevel,NpcLevel)
	local RowInfo = EquipAssess_Common(tostring(DifficultyLevel),tostring(NpcLevel))
	self.m_EquipToTalPoints = tonumber(RowInfo("EquipToTalPoints"))
	local Str = "" 
	if tonumber(self.m_EquipToTalPoints) > 0 then
		Str = GetStaticTextClient(27000, NpcLevel,self.m_EquipToTalPoints)
	else
		Str = GetStaticTextClient(27001, NpcLevel)
	end
	self.m_TitleRichText:SetWndText(Str)
	for i = 1, 5 do 
		self:SetBtnInfo(i)
		self:SetEquipAssessInfo(i)
	end
	self:SetBtnTips(DifficultyLevel,NpcLevel)
	self:ShowWnd(true)
end

function CEquipAssessWnd:SetWndResistanceInfo(AreaFbName)
	self.m_AreaFbName = AreaFbName
	self.m_PropertyValue = tonumber(AreaFb_Common(self.m_AreaFbName,"PropertyValue"))
	self.m_Type = AreaFb_Common(self.m_AreaFbName,"Type")
	local NpcLevel = tonumber(AreaFb_Common(self.m_AreaFbName,"NpcLevel"))
	
	local Str = GetStaticTextClient(1150, GetSceneDispalyName(AreaFbName)) .. NpcLevel
	local PropertyName = ""
	if self.m_Type == "护甲" then
		PropertyName = Lan_PropertyName_Common(MemH64("护甲值"),"DisplayName")
	elseif self.m_Type == "暗黑" or self.m_Type == "破坏" or self.m_Type == "自然" then
		PropertyName = Lan_PropertyName_Common(MemH64(self.m_Type .. "抗值"),"DisplayName")
	end
	Str = Str .. "#r" .. GetStaticTextClient(1151, PropertyName) .. self.m_PropertyValue
	self.m_TitleRichText:SetWndText(Str)
	
	for i = 1,5 do
		self:SetBtnInfo(i)
	end
	self:ShowWnd(true)
end

function CEquipAssessWnd:SetWndResistanceItemInfo(PlayerID,NatureResistanceValue,EvilResistanceValue,DestructionResistanceValue,Defence)
	if not self.m_AreaFbName then
		return	
	end
	for i = 1, 5 do 
		local IconStatic = self.m_IconStaticTbl[i]
		if IconStatic.m_PlayerID == PlayerID then --1:自然.2:暗黑.3:破坏.4:护甲
			local Value = 0
			if self.m_Type == "自然" then
				Value = NatureResistanceValue
			elseif self.m_Type == "暗黑" then
				Value = EvilResistanceValue
			elseif self.m_Type == "破坏" then
				Value = DestructionResistanceValue
			elseif self.m_Type == "护甲" then	
				Value = Defence
			end
			local AssessRichText = self.m_AssessRichTextTbl[i]
			
			if Value >= self.m_PropertyValue then
				Value = ColorTbl["装备评定满足颜色"] .. Value
			else
				Value = ColorTbl["装备评定不足颜色"] .. Value
			end	
			AssessRichText:SetWndText(Value)
			break
		end
	end
end

function CEquipAssessWnd.SaveResistanceValue(tbl)
	if not ResistanceValue then
		ResistanceValue = {}
	end
	table.insert(ResistanceValue,tbl)
end

function CEquipAssessWnd.SaveFightingEvaluationFun(PlayerID,FightingEvaluation)
	if FightingEvaluationTbl == nil then
		FightingEvaluationTbl = {}
	end
	FightingEvaluationTbl[PlayerID]= FightingEvaluation
end


function CEquipAssessWnd.SavePlayerInfoFun(PlayerID,PlayerName,PlayerClass,PlayerLevel,PlayerCamp)
	if PlayerInfo == nil then
		PlayerInfo = {}
	end
	table.insert(PlayerInfo,{PlayerID,PlayerName,PlayerClass,PlayerLevel,PlayerCamp})
end