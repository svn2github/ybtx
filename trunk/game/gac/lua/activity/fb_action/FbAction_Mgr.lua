lan_load "fb_game/Lan_FbActionDirect_Common"
lan_load "fb_game/Lan_MatchGame_Common"
lan_load "message/Lan_Description_Client"
lan_load "fb_game/Lan_JuQingTransmit_Common"

gac_require "activity/fb_action/join_fb_wnd/DrinkWnd"
gac_require "activity/fb_action/into_fb_wnd/FbActionMgrWnd"


function g_GetFbActionNameLanStr(str)
	local showStr = nil
	if str == "抢夺资源点" then
		showStr = GetStaticTextClient(9440)
	end
	if not showStr and Lan_FbActionDirect_Common(MemH64(str)) then
		showStr = Lan_FbActionDirect_Common(MemH64(str), "DisplayName")
	end
	if not showStr and Lan_MatchGame_Common(MemH64(str)) then
		showStr = Lan_MatchGame_Common(MemH64(str), "DisplayName")
	end
	if not showStr and Lan_JuQingTransmit_Common(MemH64(str))then
		showStr = Lan_JuQingTransmit_Common(MemH64(str), "DisplayName")
	end
	
	if not showStr then
		assert(false, str .. "副本,没有显示名")
	end
	return showStr
end

function GetDescriptionDisplayName(name)
	if Lan_Description_Client(MemH64(name)) == nil then
		return ""
	else
		return Lan_Description_Client(MemH64(name),"Content")
	end
end
