-- GMConsole
gac_require "framework/gm_console/GMConsoleInc"
gac_require "framework/gm_console/GMLocalConsole"
gac_require "framework/gm_command/GMLanCommand"

function CreateGMConsole( Parent )
	local Wnd = GMConsole:new()
	Wnd:CreateFromRes( "GMConsole", Parent )
	
	Wnd.BtnClose	= Wnd:GetDlgChild( "BtnClose" )
	Wnd.BtnOK		= Wnd:GetDlgChild( "BtnOK" )
	Wnd.InputBox	= Wnd:GetDlgChild( "InputBox" )
	Wnd.OutputBox	= Wnd:GetDlgChild( "OutputBox" )
	
	Wnd.History = CList:new()
	Wnd.History.CurrentIter = Wnd.History:head()
	--子窗口消息
	return Wnd
end

function GMConsole:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if( Child == self.BtnClose ) then
			self:ShowWnd( false )
		end
	end	
	
	if( uMsgID == EDIT_CTRLRETURN ) then
		local GMCommandText = self.InputBox:GetWndText();
		if( string.len( GMCommandText ) > 0 ) then
			if( string.sub( GMCommandText, 1, 1) == '$' ) then
				
				-----------------------------------------------------
				local tblCommand = {}
				
				local function Replace(text)
					if( text ~= "" ) then
						bFlag, text = GMLanCommand:_Available(text)
						if(not bFlag) then
							self:SetResult(text)
							return false
						end
						table.insert(tblCommand, text)
						return true
					else
						return true
					end
				end
				
				local GMText = ""
				for i = 1, string.len( GMCommandText ) do
					local char =  string.sub( GMCommandText, i, i)
					if( string.byte( char ) == 10 ) then
						if( not Replace(GMText) ) then return end
						GMText = ""
					else
						GMText = GMText..char
					end
				end
				if( not Replace(GMText) ) then return end
				
				-----------------------------------------------------
				
				Gac2Gas:GM_Execute(g_Conn, "$StartGmCmd()")
				for k, v in ipairs(tblCommand) do
					Gac2Gas:GM_Execute( g_Conn, v )
				end
				Gac2Gas:GM_Execute(g_Conn, "$EndGmCmd()")
			else 
				if( string.sub( GMCommandText, 1, 1) == '/' ) then
					self.OutputBox:AddWndText(GM_LOCAL_Execute(string.sub( GMCommandText, 2, string.len( GMCommandText ))))
				else
					Gac2Gas:GM_RunScript( g_Conn, GMCommandText );
				end
			end
			local n = self.History:size()
			if ( n > 300 ) then
				for i = 1, 100 do
					self.History:pop_front()
				end
			end
			
			if( GMCommandText ~= self.History.CurrentIter:getvalue() ) then
				self.History:push_back( GMCommandText )
				self.History.CurrentIter = self.History:tail()
				self.History.CurrentIter:dec()
			end
		end
	elseif( uMsgID == VK_ESCAPE )then
		self:ShowWnd(false)
	end
	
	if( uMsgID == EDIT_CTRLUP ) then
		self.History.CurrentIter:dec()
		if( self.History.CurrentIter ~= self.History:tail() ) then			
			self.InputBox:SetWndText( self.History.CurrentIter:getvalue() )
		end			
	end
	
	if( uMsgID == EDIT_CTRLDOWN ) then
		self.History.CurrentIter:inc()
		if( self.History.CurrentIter ~= self.History:tail() ) then			
			self.InputBox:SetWndText( self.History.CurrentIter:getvalue() )
		else
			self.History.CurrentIter:dec()
		end
	end
end

function GMConsole:OnReceiveResult( strResult )
	local i, j = string.find( strResult, "%^%^", 1)

	local l = string.sub( strResult, 1, i-1 )
	local r = string.sub( strResult, j + 1 )

	if ( "T" == r ) then
		strResult = l
	else
		strResult = r
	end
		
	local GMStr = strResult;
	if( string.len( GMStr ) > 0 and string.sub( GMStr, -1 ) ~= "\n" ) then GMStr = GMStr.."\n" end
	SetEvent( Event.Test.GMRunScriptDone, true, GMStr )
	GMStr = string.gsub( GMStr, "\n", "#r" )
	self.OutputBox:AddWndText( GMStr )	
end
	
	
function GMConsole:SetInputActive()	
	self.InputBox:SetFocus()
end
	
function GMConsole:SetResult(szStr)
	if( string.len( szStr ) > 0 and string.sub( szStr, -1 ) ~= "\n" ) then szStr = szStr.."\n" end
	szStr = string.gsub( szStr, "\n", "#r" )
	self.OutputBox:AddWndText( szStr )
end
	
--按键映射功能
function OpenGMConsole()
	Gac2Gas:GM_OpenConsole(g_Conn)
end

--返回是否可以打开GM面板
function Gas2Gac:RetOpenGMConsole(Connection, bFlag)
	if not bFlag then
		return
	else
		g_GameMain.m_GMConsole:ShowWnd( not g_GameMain.m_GMConsole:IsShow() )
		if(g_GameMain.m_GMConsole:IsShow())then
			g_GameMain.m_GMConsole:SetInputActive()
		end
	end
end

function Gas2Gac:ChangeSceneErrMsg(Connection, str, sceneName)
	local strFormat = ("" == sceneName) and str or string.format( str, GetSceneDispalyName(sceneName) )
	g_GameMain.m_GMConsole:SetResult(strFormat)
end

function Gas2Gac:RetGMGetSceneGridPos(Connection, str, sceneName)
	local strFormat = string.format( str, GetSceneDispalyName(sceneName) )
	g_GameMain.m_GMConsole:SetResult(strFormat)
end

function Gas2Gac:OpenGacProfiler(Conn, bOpen)
	if bOpen then
		profiler.Start()
	else
		profiler.Stop()
	end
end
