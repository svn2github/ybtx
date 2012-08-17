engine_require "common/Math/MathHelper"

function MessageBox( Father, strText, nStyle, CallBackFun, Context, imageStr, autoresize )
	local CMessageHandler = class(SQRMsgBoxHandler)
	local Handler = CMessageHandler:new() 
	Handler.CallBack = function( self, nIndex, Param ) 
		return not CallBackFun or CallBackFun( Context, nIndex )		
	end	
	nStyle = nStyle or BitOr( MB_BtnOK, MB_BtnCancel )
	if imageStr == nil then
	   imageStr = "" 
	end
	if autoresize ==nil then
		 autoresize = false
	end
	
	return SMsgBox( 0, Handler, Father, strText, nStyle, 0, imageStr, autoresize )	
end

function IMAGE_PARAM:Ctor( CtrlMask, StateMask )
	self.CtrlMask = CtrlMask
	self.StateMask = StateMask
end

local Cpp_GetDlgChild = SQRWnd.GetDlgChild
function SQRWnd:GetDlgChild( szChildName )
	local dlgchildWnd = Cpp_GetDlgChild( self, szChildName )
	return dlgchildWnd
end

local Cpp_ShowWnd = SQRWnd.ShowWnd
function SQRWnd:ShowWnd(bShow, bFlag)
	if(self.TYPE) then
		g_ExcludeWndMgr:ShowWnd(Cpp_ShowWnd, self, bShow, bFlag)
	else
		Cpp_ShowWnd(self, bShow)
	end
end