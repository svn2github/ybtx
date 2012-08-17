cfg_load "scene/GamePrompt_Common"
lan_load "scene/Lan_GamePrompt_Common"

CGamePromptWnd = class (SQRDialog)

function CGamePromptWnd:Ctor()

	self:CreateFromRes("GamePromptWnd",	g_GameMain)
	self.m_Text	=	self:GetDlgChild("Text")

	--g_ExcludeWndMgr:InitExcludeWnd(self, "·Ç»¥³â", false)

end


function CGamePromptWnd:ShowPromptwnd()

	if not (IsCppBound(g_Conn) and g_MainPlayer) then
		return
	end

	local	Pos	=	CFPos:new()
	g_MainPlayer:GetPixelPos(Pos)
	local	PosCurX	=	Pos.x	/	EUnits.eGridSpanForObj
	local	PosCurY	=	Pos.y	/	EUnits.eGridSpanForObj

	for	_,PromptID in	pairs(GamePrompt_Common:GetKeys())	do

		local	SceneName	=	GamePrompt_Common(PromptID, "SceneName")
		local	CenterX	=	GamePrompt_Common(PromptID,	"CenterX")
		local	CenterY	=	GamePrompt_Common(PromptID,	"CenterY")
		local	Radii	=	GamePrompt_Common(PromptID,	"Radii")
		local	QuestName	=	GamePrompt_Common(PromptID,	"QuestName")
		local	Describe = Lan_GamePrompt_Common(PromptID, "Describe")

--		print(g_GameMain.m_SceneName)
--		print(PromptID,SceneName,CenterX,CenterY,Radii,Condition,Describe)
--    print(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName])
		if	g_GameMain.m_SceneName	== SceneName then

			local IsArea	=	self:IsTheArea (PosCurX, PosCurY,	CenterX, CenterY,	Radii)
--			print(IsArea,PosCurX,PosCurY)
			local Wnd = g_GameMain.m_GamePromptWnd
			if	IsArea then
--				print(Wnd:IsShow())
				if QuestName ==	""	then
					if	Wnd:IsShow() then
						return
					else
						Wnd.m_Text:SetWndText(Describe)
						Wnd:ShowWnd(true)
--						print(Wnd:IsShow())
					end
				else
					if g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName] then
--						 print(g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName],QuestName)
						if Wnd:IsShow()	then
							return
						else
							Wnd.m_Text:SetWndText(Describe)
							Wnd:ShowWnd(true)
						end
					end
				end
			  return
			else
				if Wnd:IsShow()	then
					Wnd:ShowWnd(false)
				end
			end
		end
	end
end

function CGamePromptWnd:IsTheArea(PosCurX, PosCurY,	CenterX, CenterY,	Radii)

	if PosCurX <= (CenterX + Radii) and PosCurX >= (CenterX	-	Radii) then
		if	PosCurY <= (CenterY	+	Radii) and PosCurY >= (CenterY - Radii) then
			return true
		else return false
		end

	else return	false
	end
end