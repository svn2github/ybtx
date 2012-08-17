gac_require "test/common/CTstLoginControler"
gac_require "message/message_box/MsgBoxMsg"

function InitGacTestMessage()
	local TestTest=TestCase("TestGacMessage")

	local controler = {}
	
	function TestTest:setup()
	end
	
	function TestTest:TestBegin()
		controler = CTstLoginControler:new()
		controler:OneStepLogin()
	end
	
	function TestTest:TestMsgToConn()
		SetEvent(Event.Test.MsgToClient,false)
		local GMCommandText = [[MsgToConn( Pipe,2,3,"大熊猫",10 )]]
		Gac2Gas:GM_RunScript( g_Conn, GMCommandText );
		local strRet = WaitEvent(true,nil,Event.Test.MsgToClient)
		SetEvent(Event.Test.MsgToClient,false)
		if strRet[1] == EEventState.Success then
			assert_equal("已杀死3只大熊猫，共需杀死10只", strRet[2][1], "MessageToConn Failed!")
		else
			assert_fail("MessageToConn Failed TimeOut")
		end
	end

	function TestTest:TestMsgToScene()
		local GMCommandText =
		[[MsgToScene( Pipe.m_Player.m_Scene,2,3,"大熊猫",10 )]]
		Gac2Gas:GM_RunScript( g_Conn, GMCommandText );
		local strRet = WaitEvent(true,nil,Event.Test.MsgToClient)
		SetEvent(Event.Test.MsgToClient,false)
		if strRet[1] == EEventState.Success then
			assert_equal("已杀死3只大熊猫，共需杀死10只", strRet[2][1], "MessageToScene Failed!")
		else
			assert_fail("MessageToScene Failed TimeOut")
		end
	end
	
	function TestTest:TestMsgToServer()
		local GMCommandText = [[MsgToServer( 2,3,"大熊猫",10 )]]
		Gac2Gas:GM_RunScript( g_Conn, GMCommandText );
		local strRet = WaitEvent(true,nil,Event.Test.MsgToClient)
		SetEvent(Event.Test.MsgToClient,false)
		if strRet[1] == EEventState.Success then
			assert_equal("已杀死3只大熊猫，共需杀死10只", strRet[2][1], "MessageToServer Failed!")
		else
			assert_fail("MessageToServer Failed TimeOut")
		end
	end
	
	--测试MsgBoxMsg
	function TestTest:TestMsgBoxMsg()
		local str
		str = MsgBoxMsg(1,"401")
		assert_equal("与服务器连接错误(GBS: 401)", str, "Test MsgBoxMsg Failed!")
		
		str = MsgBoxMsg(2,"402")
		assert_equal("服务器繁忙，请稍后再试(402)", str, "Test MsgBoxMsg Failed!")
		
		str = MsgBoxMsg(9)
		assert_equal("非法的用户名!", str, "Test MsgBoxMsg Failed!")		
	end
	
	function TestTest:TestEnd()
		controler:LoginOutFromGame()
	end
	
	function TestTest:teardown()
	end
end

