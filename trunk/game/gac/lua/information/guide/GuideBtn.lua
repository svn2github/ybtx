cfg_load "information/guide/GuideAlert_Common"

CGuideBtn = class(SQRDialog)

function CGuideBtn:Ctor(parent)
	self:CreateFromRes("GuideBtn", parent)
	self:SetIsBottom(true)
end

function CGuideBtn:OnChildCreated()
	self.m_Btn = self:GetDlgChild("Btn")
end

function CGuideBtn:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_Btn) then
			self:Animation(false)
		end
	end
end

function CGuideBtn:Clear()
	g_GameMain.m_CenterMsg:ShowWnd(false)
	self.m_Btn:DelFlashAttention()
end

function CGuideBtn:BeAlert(subNode)
	if(subNode) then
		local nMsgIndex	= subNode("AlertIndex")
		local nType		= subNode("Type")
		if("" ~= nType) then
			g_GameMain.m_GuideWnd.m_ForceAlertType = nType
		end
		if( g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn or (1 ~= g_GameMain.m_SceneType and 5 ~= g_GameMain.m_SceneType) ) then
			if( 0 ~= nMsgIndex ) then
				self.m_nTempMsgID = nMsgIndex
			end
		else
			self.m_Btn:AddFlashInfoByName("Guidebuttonflash")
			if( 0 ~= nMsgIndex ) then
				MsgClient(nMsgIndex)
			end
		end
	end
end

function CGuideBtn:BeAlertLevelUp()
	local nCurLevel	= g_GameMain.m_PlayerInfoTbl.m_PlayerLevel
	local subNode	= GuideAlert_Common( tostring(nCurLevel) )
	self:BeAlert(subNode)
end

function CGuideBtn:BeAlertQuestFinish(sQuestName)
	local subNode = GuideAlert_Common(sQuestName)
	self:BeAlert(subNode)
end

function CGuideBtn:BeAlertQuestEmptyAlert()
	local IsShow = g_GameMain.m_GuideData:BeQuestEmpty()
	if(IsShow) then
		self:BtnFlash()
	else
		self:Clear()
	end
end

function CGuideBtn:BtnFlash()
	if( g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn
		or (1 ~= g_GameMain.m_SceneType and 25 ~= g_GameMain.m_SceneType) ) then
		return
	end
	self.m_Btn:AddFlashInfoByName("Guidebuttonflash")
	if( g_GameMain.m_PlayerInfoTbl.m_PlayerLevel < 20 and not g_GameMain.m_CenterMsg:IsShow() ) then
		MsgClient(410006)
	end
end

function CGuideBtn:Animation(bFlag)
	local arg = g_ParseCommandline()[3] --ÏÊÑªÊÔÁ¶·þ
	if arg and arg.gastype == 1 then
		return
	end
	
	local zoomSize	= self:GetRootZoomSize()
	local rect1		= CFRect:new()
	local rect2		= CFRect:new()
	self:GetWndRect(rect1)
	rect2.left		= rect1.left - 50*zoomSize
	rect2.right		= rect1.right - 50*zoomSize
	rect2.top		= rect1.top
	rect2.bottom	= rect1.bottom
	if(bFlag) then
		self:ShowWnd(true)
		self:SetTransparent(0.1)
		self:SetTransparentObj(350, false)
		self:SetShapeEffect(250, rect2, rect1)
		self.m_bHiding = false
	else
		self:SetTransparentObj(350, true)
		self:SetShapeEffect(250, rect1, rect2)
		self.m_bHiding = true
	end
end

function CGuideBtn:OnShapeEffectEnded()
	if(self.m_bHiding) then
		self:ShowWnd(false)
		g_GameMain.m_GuideWnd:OpenPanel(true)
		self.m_bHiding = false
	else
		if(not g_GameMain.m_GuideWnd.m_GuideOnTrackTick) then
			self:BeAlertQuestEmptyAlert()
		end
	end
	if(self.m_bSpecialAnimation) then
		self.m_bSpecialAnimation = nil
		self.m_Btn:AddFlashInfoByName("Guidebuttonflash2")
	end
end

function CGuideBtn:InitShow()
	local arg = g_ParseCommandline()[3] --ÏÊÑªÊÔÁ¶·þ
	if arg and arg.gastype == 1 then
		return
	end
	
	self:ShowWnd(not g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn)
end

function CGuideBtn:SpecialAnimation()
	self.m_bSpecialAnimation = true
	self:Animation(true)
end

function CGuideBtn:OnTransfered(nFromSceneType, nToSceneType)
	if( (1 ~= nFromSceneType and 25 ~= nFromSceneType) and (1 == nToSceneType or 25 == nToSceneType) ) then
		self:BeAlertQuestEmptyAlert()
		if(self.m_nTempMsgID) then
			MsgClient(self.m_nTempMsgID)
			self.m_nTempMsgID = nil
		end
	elseif((1 == nFromSceneType or 25 == nFromSceneType) and (1 ~= nToSceneType and 25 ~= nToSceneType)) then
		g_GameMain.m_CenterMsg:ShowWnd(false)
	end
end