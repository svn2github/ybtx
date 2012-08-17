gac_gas_require "framework/message/message"
gac_gas_require "framework/common/CMoney"
cfg_load "message/Message_Common"
cfg_load "sound/MsgMatchSound_Client"

function DisplayMsg(uType, sMessage)
	if(g_GameMain) then
		if(uType == 1) then
			g_GameMain.m_CreateChatWnd.m_ImportMsg:AddMsg(sMessage)
		elseif(uType == 2) then
			g_GameMain.m_CreateChatWnd.m_CChatWnd:SystemInfoSet( 1, sMessage)
		elseif(uType == 3) then
			g_GameMain.m_CreateChatWnd.m_ImportMsg:AddMsg(sMessage)
			g_GameMain.m_CreateChatWnd.m_CChatWnd:SystemInfoSet( 1, sMessage)
		elseif (uType == 4) then
			g_GameMain.m_PlayerFightInfoWnd:AddMsg(sMessage)
		elseif (uType == 6 or uType == 7 or uType == 8 ) then -- 闪烁消息提示，三个级别：6 7 8 -> 低 中 高
			g_GameMain.m_FlashMsgWnd:SetFlashMsgWndText(sMessage,uType)
		elseif uType == 9 then
			g_GameMain.m_FlashSkillMsgWnd:SetFlashSkillMsgWndText(sMessage)
		elseif uType == 10 then
			g_GameMain.m_MessageShapeEffectWnd:SetMsgInfo(sMessage)
		elseif uType == 11 then
			g_GameMain.m_MessageShapeEffectSecondWnd:SetMsgInfo(sMessage)
		elseif uType == 12 then
			g_GameMain.m_MessageMiniTextWnd:SetMsgInfo(sMessage)
		elseif(13 == uType) then
			g_GameMain.m_CenterMsg:OpenPanel(sMessage, uType)
		end
	end
end

function PlayVoiceOfItemWarning(msgId)
	local voiceSourceId = MsgMatchSound_Client(msgId,"VoiceId")
	if voiceSourceId then
		CPlaySoundOfWarning.PlaySoundOfNotice(voiceSourceId)
	end
end

local function GetMoneyDisplay(price)
	return g_MoneyMgr:ChangeMoneyToString(tonumber(price), EGoldType.GoldCoin)
end

local function ReplaceArg(source,TranslateType,arg)
	local TranslateTypeFun = {
		["npc"] = GetNpcDisplayName,
		["obj"] = GetIntObjDispalyName,
		["trap"] = GetTrapDispalyName,
		["scene"] = GetSceneDispalyName,
		["area"] = GetAreaDispalyName,
		["quest"] = g_GetLanQuestName,
		["action"] = g_GetFbActionNameLanStr,
		["yblevel"] = g_GetMercenaryLevelLanName,
		["money"] = GetMoneyDisplay,
	}
	if TranslateType and TranslateType ~= "" then
		TranslateType = loadstring("return {"..TranslateType.."}")()
	end
	
	
	local backup = source
	local n = #arg
	local i = 1
	local beginPos
	local endPos
	local tempStr
	local destStr
	while i<=n do
		tempStr = "#arg".. i .. "%b()"
		beginPos,endPos = string.find(source,tempStr)
		if beginPos == nil then
			break
		end
		destStr = string.sub(source,beginPos,endPos)
		beginPos,endPos = string.find(destStr,"%b()")
		destStr = string.sub(destStr,beginPos+1,-2)
		if arg[i]==nil then
			assert(false,"传入通配参数为nil，字符串为：" .. backup)
		end
		
		---------------------
		local showStr = nil
		if IsString(arg[i]) and TranslateType then
			if TranslateType[i] then
				if TranslateType[i] == "item" then
					local DisplayName = g_ItemInfoMgr:GetItemLanInfoJustByName(arg[i],"DisplayName")
					showStr = DisplayName
				elseif TranslateTypeFun[TranslateType[i]] then
					showStr = TranslateTypeFun[TranslateType[i]](arg[i])
				end
			end
		end
		showStr = showStr or arg[i]
		------------------------
		
		destStr = string.format(destStr,showStr)
		destStr = string.gsub(destStr,"%%","%%%%")
		source = string.gsub(source,tempStr,destStr)
		i = i + 1
	end
	source = string.gsub(source,"%%%%","%%")
	return source
end

local function GacGetStringCommom(MessageCommom, MessageId, TranslateType, arg, MessageCommomName)
	assert(not IsNil(MessageCommom(MessageId)), MessageCommomName .. "中没有" .. MessageId)
	local strRes
	if(next(arg) == nil) then
		strRes = MessageCommom(MessageId,"Content")
	else
		strRes = ReplaceArg(MessageCommom(MessageId,"Content"), TranslateType, arg)
	end
	return strRes
end

function GacAssembleArgs(MessageId, arg)
	local strRes = GacGetStringCommom(Lan_Message_Common, MessageId, Message_Common(MessageId,"TranslateType"), arg, "Lan_Message_Common")
	return strRes, Message_Common(MessageId,"Type")
end

function MsgClient(MessageId,...)
	local arg = {...}
	local str, type = GacAssembleArgs(MessageId, arg)
	DisplayMsg(type, str)
	PlayVoiceOfItemWarning(MessageId)
end

function GetStaticTextClient(uTextId, ...)
	local arg = {...}
	return GacStaticTextAssembleArgs(uTextId, arg)
end

function GetLogClient(uTextId, ...)
	local arg = {...}
	return GacLogAssembleArgs(uTextId, arg)
end

function ToolTipsText(uTextId, ...)
	local arg = {...}
	return GacToolTipsTextAssembleArgs(uTextId, arg)
end

function GetGMMsgClient(uTextId, ...)
	local arg = {...}
	return GacGMMsgClientAssembleArgs(uTextId, arg)
end

--缩短函数名方便网络传输
function _GSTC(uTextId, ...)
	return GetStaticTextClient(uTextId, ...)
end
