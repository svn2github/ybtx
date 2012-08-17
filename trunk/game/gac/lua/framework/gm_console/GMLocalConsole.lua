lan_load "gm/Lan_GMCommand_Common"

function Hello()
  return "Hello!"
end

function SetBuffering(number)
	if( type(number) == "number" )then
 		CRenderSystemClient_Inst():SetBackBufferNum(number)
 		if( number > 0 )then
 			return number .. "倍缓冲 开启"
 		else
 			return "缓冲关闭"
 		end 
 	else
 		return "参数为摄象机动画的编号"
 	end
end

function ShowFps(isShow)
	if( isShow ~= nil and type(isShow) == "boolean" )then
		if( isShow ~= CRenderSystemClient_Inst():IsShowFps() ) then
			CRenderSystemClient_Inst():ShowFpsSwitch()
			if(CRenderSystemClient_Inst():IsShowFps())then
  			return "#c00FF00显示FPS" 
 			else
  			return "#cFF0000隐藏FPS"
  		end
		end
	else
		return "参数不合法"
	end
  return 
end

function PlayCameraPath( Index )
 	if( type(Index) == "number" )then
 		CCoreSceneClient_Inst():PlayCameraPath(Index)
 	else
 		return "参数为摄象机动画的编号"
 	end
end

function ShowFog( isShow )
	if( isShow ~= nil and type(isShow) == "boolean" )then
		CRenderSystemClient_Inst():SetShowFog(isShow)
		if(isShow)then
			return "#c00FF00雾效开"
		else
			return "#cFF0000雾效关"
		end
	else
		return "参数不合法"
	end
  return 
end

function SetIsRender( isRender )
	if( isRender ~= nil and type(isRender) == "boolean" )then
		CRenderSystemClient_Inst():SetIsRender(isRender)
	else
		return "参数不合法"
	end
  return 
end

function EnableFreeCamera( isFree )
	if( isFree ~= nil and type(isFree) == "boolean" )then
		CRenderSystemClient_Inst():SetFreeCameraEnabled(isFree)
	else
		return "参数不合法"
	end
  return 
end

function Enable3DCamera( isFree )
		if( isFree ~= nil and type(isFree) == "boolean" )then
		CRenderSystemClient_Inst():SetFree3DCameraEnabled(isFree)
	else
		return "参数不合法"
	end
  return 
end

function ShowExactFightInfo( bFlag )
	if not g_MainPlayer then
		return
	end
	g_GameMain.m_bExactFightInfo = bFlag
end

function SaveFightInfo( bFlag )
	if not g_MainPlayer then
		return
	end
	g_GameMain.m_bSaveFightInfo = bFlag
end

function HideSelf( bHide )
	if not g_MainPlayer or not g_MainPlayer:GetHorseRenderObj() then
		return
	end
	if bHide then
		g_MainPlayer:GetHorseRenderObj():SetAlphaValue(0)
		g_MainPlayer:GetHorseRenderObj():Update()
	else
		g_MainPlayer:GetHorseRenderObj():SetAlphaValue(255)
		g_MainPlayer:GetHorseRenderObj():Update()
	end
	g_MainPlayer:SetCanBeSelected(not bHide)
end


function HideMouseFX ( bHide )
	g_MainPlayer.m_bHideMouseFX = bHide
end

function Help( sCommand )
	if(sCommand) then
		local mh64Command	= MemH64( string.lower( sCommand ) )
		local sDescription	= Lan_GMCommand_Common(tostring(mh64Command), "Description")
		return sCommand .. "->" .. sDescription
	else
		local sResult = ""
		local tbl = Lan_GMCommand_Common:GetKeys()
		for i, v in ipairs(tbl) do
			local command		= Lan_GMCommand_Common(v, "Command")
			local description	= Lan_GMCommand_Common(v, "Description") or ""
			sResult = sResult .. command .. "->" .. description .. "#r"
		end
		return sResult
	end
end

function DoAni(strAniName, bLoop)
	if g_MainPlayer then
		g_MainPlayer:GetRenderObject():DoAni(strAniName, bLoop, -1.0)
	end
end

function GM_LOCAL_Execute( funName )

	--注册到这里的函数才有权限调用
	local FunList =
	{
		["Help"]			=	Help,
		["ShowFps"] 				= ShowFps,
		["PlayCameraPath"]  = PlayCameraPath,
		["SetIsRender"]     = SetIsRender,
		["SetBuffering"]		= SetBuffering,
		["EnableFreeCamera"]= EnableFreeCamera,
		["Enable3DCamera"]	= Enable3DCamera,
		["ShowFog"]					= ShowFog,
		["HideSelf"]				= HideSelf,
		["ShowExactFightInfo"] = ShowExactFightInfo,
		["SaveFightInfo"] 	= SaveFightInfo,
		["HideMouseFX"] = HideMouseFX,
		["DoAni"]		= DoAni,
		["SwitchRecvTrafficStat"] = SwitchRecvTrafficStat,
	}
	
	local TempStr = ""
	local FunStr = ""
	local fun = nil;
	for i = 1, string.len( funName ) do
		local char =  string.sub( funName, i, i)
		if( char  == "(" ) then
			FunStr = TempStr;
			fun = FunList[FunStr];
			if( fun == nil )then
				return "#cFF0000" .. FunStr .. "Can`t find this command#r"
			end
			TempStr = ""
		else
			if( char == ")" and fun ~= nil )then
				if( TempStr == nil )then
					TempStr = loadstring("return " .. FunStr .. "()" )()
					if( TempStr ~= nil )then
						return "#c0000FF" .. TempStr .. "#r"
					else
						return "#c00FF00" .. FunStr .. "Done#r"
					end
				else
					local Str = "return " ..  FunStr .. "(" .. TempStr .. ")"
					TempStr = loadstring(Str)()
					if( TempStr ~= nil )then
						return "#c0000FF" .. TempStr .. "#r"
					else
						return "#c00FF00" .. FunStr .. "Done#r"
					end
				end
			else
				TempStr = TempStr..char
			end
		end
	end
	return "#cFF0000Can not recognise the string#r"
end