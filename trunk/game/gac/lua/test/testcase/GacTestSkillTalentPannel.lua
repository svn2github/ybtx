gac_require "test/common/CTstLoginControler"

function InitGacTestSkillTalentPannel()
	local TestTest = TestCase("GacTestSkillTalentPannel")
	local controler = CTstLoginControler:new()
	function TestTest:setup()
	end
	
	--进入操作
	function TestTest:TestBegin()
		controler:OneStepLogin()
	end
	
	--天赋面板模块
	function TestTest:test_talentPanne1()
		local Wnd = g_GameMain.m_SkillParent.m_GeniusLearnWnd
		g_MainPlayer:CppSetLevel(100)
		Wnd:ShowWnd(true)
		Wnd:OnCtrlmsg(Wnd.m_Btn1,BUTTON_LCLICK,0,0)
		--第一阶段测试
		local strRet = 0
			for i = 1, 11 do
				for j = 2, 4 do
					Wnd.m_GeniusPlane1:OnCtrlmsg(Wnd.m_GeniusPlane1.m_Btn[j],BUTTON_LCLICK,0,0)
					strRet = WaitEvent(true,nil,Event.Test.AddTalent)
					SetEvent(Event.Test.AddTalent,false)
				end
			end
		---------
		if (strRet[1] == EEventState.Success ) then
			assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[2]:GetWndText()) == 10 )
			assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[3]:GetWndText()) == 10 )
			assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[4]:GetWndText()) == 10 )
			assert(tonumber(Wnd.m_GeniusPlane1.m_PutGeniusNumWnd:GetWndText()) == 30)
			
			assert(tonumber(Wnd.m_GeniusPlane1.m_UseGeniusNumWnd:GetWndText()) == 70)
			assert(Wnd.m_GeniusPlane1.m_Btn[5]:IsEnable() == true)
			assert(Wnd.m_GeniusPlane1.m_Btn[11]:IsEnable() == false)
			assert(Wnd.m_GeniusPlane1.m_Btn[15]:IsEnable() == false)
		end
		
		--第二阶段测试
		if #(Wnd.m_GeniusPlane1.m_Btn) == 16 then
			for i = 1, 11 do
				for j = 7,9 do
					Wnd.m_GeniusPlane1:OnCtrlmsg(Wnd.m_GeniusPlane1.m_Btn[j],BUTTON_LCLICK,0,0)					
					strRet[1] = WaitEvent(true,nil,Event.Test.AddTalent)
					SetEvent(Event.Test.AddTalent,false)
				end
			end
		else
			for i = 1, 11 do
				for j = 6,8 do 
					Wnd.m_GeniusPlane1:OnCtrlmsg(Wnd.m_GeniusPlane1.m_Btn[j],BUTTON_LCLICK,0,0)					
					strRet[1] = WaitEvent(true,nil,Event.Test.AddTalent)
					SetEvent(Event.Test.AddTalent,false)
				end
			end		

		end
		if (strRet[1][1] == EEventState.Success ) then
			if (#(Wnd.m_GeniusPlane1.m_Btn) == 16 ) then
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[7]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[8]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[9]:GetWndText()) == 10 )
				assert(tonumber(Wnd.m_GeniusPlane1.m_PutGeniusNumWnd:GetWndText()) == 60)
				
				assert(tonumber(Wnd.m_GeniusPlane1.m_UseGeniusNumWnd:GetWndText()) == 40)
				assert(Wnd.m_GeniusPlane1.m_Btn[10]:IsEnable() == true)
				assert(Wnd.m_GeniusPlane1.m_Btn[14]:IsEnable() == false)
			else
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[6]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[7]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[8]:GetWndText()) == 10 )
				assert(tonumber(Wnd.m_GeniusPlane1.m_PutGeniusNumWnd:GetWndText()) == 60)
				
				assert(tonumber(Wnd.m_GeniusPlane1.m_UseGeniusNumWnd:GetWndText()) == 40)
				assert(Wnd.m_GeniusPlane1.m_Btn[9]:IsEnable() == true)
				assert(Wnd.m_GeniusPlane1.m_Btn[13]:IsEnable() == false)				
			end
		end	
			
		--第三阶段测试
		if #(Wnd.m_GeniusPlane1.m_Btn) == 16 then
			for i = 1, 11 do
				for j = 10,13 do
					Wnd.m_GeniusPlane1:OnCtrlmsg(Wnd.m_GeniusPlane1.m_Btn[j],BUTTON_LCLICK,0,0)					
					strRet[1] = WaitEvent(true,nil,Event.Test.AddTalent)
					SetEvent(Event.Test.AddTalent,false)
				end
			end
		else
			for i = 1, 11 do
				for j = 10,12 do 
					Wnd.m_GeniusPlane1:OnCtrlmsg(Wnd.m_GeniusPlane1.m_Btn[j],BUTTON_LCLICK,0,0)				
					strRet[1] = WaitEvent(true,nil,Event.Test.AddTalent)
					SetEvent(Event.Test.AddTalent,false)
				end
			end		

		end		
		if (strRet[1][1] == EEventState.Success ) then

			if (#(Wnd.m_GeniusPlane1.m_Btn) == 16 ) then
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[11]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[12]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[13]:GetWndText()) == 10 )
				assert(tonumber(Wnd.m_GeniusPlane1.m_PutGeniusNumWnd:GetWndText()) == 90)
				assert(tonumber(Wnd.m_GeniusPlane1.m_UseGeniusNumWnd:GetWndText()) == 10)
				assert(Wnd.m_GeniusPlane1.m_Btn[14]:IsEnable() == true)
			else
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[10]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[11]:GetWndText()) == 10 )
				assert( tonumber(Wnd.m_GeniusPlane1.m_Btn[12]:GetWndText()) == 10 )
				assert(tonumber(Wnd.m_GeniusPlane1.m_PutGeniusNumWnd:GetWndText()) == 90)
				assert(tonumber(Wnd.m_GeniusPlane1.m_UseGeniusNumWnd:GetWndText()) == 10)
				assert(Wnd.m_GeniusPlane1.m_Btn[13]:IsEnable() == true)				
			end
		end

	end

	--技能面板模块
	function TestTest:test_skillPannel()
		local Wnd = g_GameMain.m_SkillParent.m_SkillLearnWnd
		local strRet = 0
		Wnd:ShowWnd(true)
		Wnd:OnCtrlmsg(Wnd.m_SkillBtn[4],BUTTON_LCLICK,0,0)
		Wnd:OnCtrlmsg(Wnd.m_SkillLearnBtn,BUTTON_LCLICK,0,0)	
		strRet = WaitEvent(true,nil,Event.Test.AddTalent)
		SetEvent(Event.Test.AddTalent,false)
		
		if ( strRet[1] == EEventState.Success )then
			assert(tonumber(Wnd.m_SkillBtn[4]:GetWndText()) == 1)
		end
		Wnd:OnCtrlmsg(Wnd.m_SkillBtn[10],BUTTON_LCLICK,0,0)
		Wnd:OnCtrlmsg(Wnd.m_SkillLearnBtn,BUTTON_LCLICK,0,0)
		strRet = WaitEvent(true,nil,Event.Test.AddTalent)
		SetEvent(Event.Test.AddTalent,false)
		
		if ( strRet[1] == EEventState.Success )then
			assert(tonumber(Wnd.m_SkillBtn[10]:GetWndText()) == 1)
		end
	end
	
	function TestTest:TestEnd()
		controler:LoginOutFromGame()
	end
	function TestTest:teardown()
	end
	
end