CYYCtrl = class(SQRYYCtrl)

local SERVERTYPE_YB = 1		--混合登陆界面
local SERVERTYPE_YY = 2		--YY专区登陆界面

function CYYCtrl.InitYY(Parent)
	local ParamTbl = Parent.m_LoginAccounts.m_Infos[3]
	if ParamTbl and  (tonumber(ParamTbl["yy"]) == SERVERTYPE_YB or tonumber(ParamTbl["yy"]) == SERVERTYPE_YY) then
		Parent.m_InitYY = true
	end
	g_YYLogFlag = nil 
	if Parent.m_InitYY then
		local Rect=CFRect:new()
		Parent.m_LoginAccounts.m_OpenYYBtn:GetLogicRect(Rect)
		CYYManager_CreateYYMgr()
		local param = WndCreateParam:new()
		param.uStyle = 0x40400000
		param.width = 34
		param.height = 34
		param:SetParentWnd(Parent.m_LoginAccounts)
		param.font_size = 14
		param.x = 623
		param.y = 440

		Parent.m_YY = CYYCtrl:new()
		Parent.m_YY:Create(param)
		Parent.m_YY:SetLockFlag(LOCK_TOP)
		Parent.m_YY:SetLocationStyle(2)
		Parent.m_YY:ShowWnd(false)
		Parent.m_LoginAccounts.m_OpenYYBtn:ShowWnd(true)
		Parent.m_LoginAccounts.m_YYicon:ShowWnd(true)
		if ParamTbl and tonumber(ParamTbl["yy"]) == SERVERTYPE_YY then
			Rect.top = Rect.top - 85
			Parent.m_YY:SetLogicRect(Rect)
			Parent.m_LoginAccounts:SetChildWndShow(false)
			Parent.m_YY:ShowWnd(true)
			Parent.m_LoginAccounts.m_BtnYYBack:ShowWnd(true)
		end	
	end
end

local CALL_BACK_MSG_YYQUEST  	= 2 	-- 点击 一键登录 触发
local CALL_BACK_MSG_YYLOGIN  	= 3 	-- yy返回 登陆验证结果
local CALL_BACK_MSG_YYDINGHAO  	= 6 	-- yy顶号不成功
local CALL_BACK_MSG_LOGIN_YYCLIENT = 4  -- 登录YY客户端

function CYYCtrl:CallBackChar(nType,wParam)
	CYYCtrl.ReadYYLog("CallBackChar:" .. "nType:" .. nType .. "wParam" .. wParam)
	if g_Login then 
		if nType == CALL_BACK_MSG_YYLOGIN and g_YYConnFlag then
			CYYCtrl.ReadYYLog("YYKey2发送给GameGas")
			Gac2Gas:YYlogin(g_Conn,wParam)
		end
	end
end

function CYYCtrl:CallBackInt(nType,wParam)
	CYYCtrl.ReadYYLog("CallBackInt:" .. "nType:" .. nType .. "wParam" .. wParam)
	if g_Login then 
		if nType == CALL_BACK_MSG_YYQUEST  and g_YYConnFlag == nil and not g_Login.m_IsConnecting then
			CYYCtrl.ReadYYLog("登陆CALL_BACK_MSG_YYQUEST")
			g_YYConnFlag = true
			g_Login.m_IsConnecting = true
			g_Login.m_LoginAccounts:YYConnect2Server()
			CYYCtrl.ReadYYLog("登陆YY")
		elseif nType == CALL_BACK_MSG_LOGIN_YYCLIENT then

		elseif CALL_BACK_MSG_YYDINGHAO == nType and g_YYConnFlag then
			if IsCppBound(g_Conn) then
				g_Conn:ShutDown()
			end
			g_Login.m_nConnStatus = 0
			g_Login.m_WndCancelMsgbox:ShowWnd(false)
			g_YYConnFlag = nil
			g_Login.m_IsConnecting = false
			CYYCtrl.ReadYYLog("YY超时")
			g_YYLogFlag = true
		end
	end
end

function CYYCtrl.SetYYLoginKey(key)
	CYYCtrl.ReadYYLog("YYKey1请求返回")
	if g_Login.m_InitYY then
		CYYCtrl.ReadYYLog("YYKey1发送给YY")
		g_Login.m_YY:SetLoginKEY(key)
	end
end

function CYYCtrl.YYLoginSucceed()
	if g_Login.m_InitYY and g_YYConnFlag then
		CYYCtrl.ReadYYLog("YYLoginSucceed 登陆成功")
		g_Login.m_YY:LoginSucceed()
		g_YYConnFlag = nil
		g_YYLogFlag = nil
	end
end

function CYYCtrl.YYLoginFailure(MsgStr)
	if g_Login.m_InitYY and g_YYConnFlag then
		CYYCtrl.ReadYYLog("YYLoginFailure 登陆失败")
		g_Login.m_YY:LoginFailure(MsgStr)
		g_YYConnFlag = nil
		g_YYLogFlag = nil
	end
end

function CYYCtrl.ReadYYLog(str)
	local strFileName = g_RootPath .. "etc/gac/yy.txt"
	local f1 = CLuaIO_Open(strFileName, "r")
	if(f1 ~= nil) then
		f1:Close()
		local f = CLuaIO_Open(strFileName, "a")
		if(f ~= nil) then
			local data =  f:WriteString(str .. "\n" )
			f:Close()
		end
	end
end

function CYYCtrl.YYErrLogFun(DescStr)
	if g_Login and g_Login.m_InitYY and g_YYConnFlag then
		local Errstr = ""
		if g_Login.m_LoginAccounts.m_Infos[1] then
			local pos		= string.find(g_Login.m_LoginAccounts.m_Infos[1], ":")
			local strIP		= string.sub(g_Login.m_LoginAccounts.m_Infos[1], 1 ,pos-1)
			local strPort	= string.sub(g_Login.m_LoginAccounts.m_Infos[1], pos+1, -1)
			Errstr = "IP:" .. strIP .. "  Port:" .. strPort 
		end
		Errstr = "DescStr:" .. DescStr .. "," .. Errstr .. "," .. os.date("%Y/%m/%d %H:%M:%S",os.time())
		LogErr("YYLoginErrLog",Errstr)
	end
end